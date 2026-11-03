#include "services/AccountImportService.h"
#include "services/CryptoService.h"
#include "services/TotpService.h"
#include <cstring>

#if defined(ARDUINO_ARCH_ESP32)
#include <Arduino.h>
#include <WiFiUdp.h>
namespace {
WiFiUDP importSocket;
bool nativeOpen(ImportMode mode) { return mode == ImportMode::Usb || importSocket.begin(AccountImportService::Port) == 1; }
void nativeClose() { importSocket.stop(); }
int nativeRead(ImportMode mode, uint8_t* out, size_t capacity) {
  if (mode == ImportMode::Wifi) {
    if (!importSocket.available()) importSocket.parsePacket();
    return importSocket.available() ? importSocket.read(out, capacity) : 0;
  }
  size_t n = 0;
  while (n < capacity && Serial.available()) { const int c = Serial.read(); if (c < 0) break; out[n++] = c; }
  return static_cast<int>(n);
}
const AccountImportTransport nativeTransport(nativeOpen, nativeClose, nativeRead);
}
#endif

namespace {
constexpr const char* kAad = "NibruOS/account-import/v1";
int nibble(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  return -1;
}
bool unhex(const char* text, size_t count, uint8_t* out) {
  for (size_t i = 0; i < count; ++i) {
    const int high = nibble(text[i * 2]), low = nibble(text[i * 2 + 1]);
    if (high < 0 || low < 0) return false;
    out[i] = static_cast<uint8_t>((high << 4) | low);
  }
  return true;
}
}

void AccountImportService::stop() {
  if (transport_ && state_ == ImportState::Waiting) transport_->close();
  CryptoService::secureZero(pairing_, sizeof(pairing_));
  CryptoService::secureZero(key_, sizeof(key_));
  CryptoService::secureZero(frame_, sizeof(frame_));
  CryptoService::secureZero(&candidate_, sizeof(candidate_));
  used_ = rejected_ = 0; overflow_ = false;
  state_ = ImportState::Idle;
}
bool AccountImportService::start(ImportMode mode, uint32_t now) {
  stop();
#if defined(ARDUINO_ARCH_ESP32)
  if (!transport_) transport_ = &nativeTransport;
#endif
  if (!transport_ || !transport_->open || !transport_->close || !transport_->read) return false;
  if (!transport_->open(mode)) { transport_->close(); return false; }
  mode_ = mode; state_ = ImportState::Waiting; started_ = now;
  uint8_t entropy[16]{};
  CryptoService::fillRandom(entropy, sizeof(entropy));
  for (size_t i = 0; i < 16; ++i) pairing_[i] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567"[entropy[i] & 31];
  CryptoService::secureZero(entropy, sizeof(entropy));
  if (!CryptoService::sha256(reinterpret_cast<const uint8_t*>(pairing_), 16, key_)) { stop(); return false; }
  if (mode_ == ImportMode::Usb) broadcastPairingCode();
  return true;
}
void AccountImportService::broadcastPairingCode() {
  if (state_ != ImportState::Waiting || !pairing_[0]) return;
#if defined(ARDUINO_ARCH_ESP32)
  if (mode_ == ImportMode::Usb) {
    Serial.print("PAIRING:");
    Serial.println(pairing_);
  }
#endif
}
uint32_t AccountImportService::remainingSeconds(uint32_t now) const {
  if (state_ != ImportState::Waiting && state_ != ImportState::Review) return 0;
  const uint32_t elapsed = now - started_;
  return elapsed >= TimeoutMs ? 0 : (TimeoutMs - elapsed + 999) / 1000;
}
void AccountImportService::tick(uint32_t now) {
  if (state_ != ImportState::Waiting && state_ != ImportState::Review) return;
  if (now - started_ >= TimeoutMs) { stop(); state_ = ImportState::Expired; return; }
  if (state_ == ImportState::Review) return;
  uint8_t bytes[64]{};
  const int n = transport_->read(mode_, bytes, sizeof(bytes));
  if (n < 0 || n > static_cast<int>(sizeof(bytes))) { stop(); state_ = ImportState::Expired; return; }
  for (int i = 0; i < n; ++i) {
    const char c = static_cast<char>(bytes[i]);
    if (c == '\n') {
      if (used_ && !overflow_ && decodeFrame()) {
        transport_->close();
        state_ = ImportState::Review;
        CryptoService::secureZero(pairing_, sizeof(pairing_));
        CryptoService::secureZero(key_, sizeof(key_));
        CryptoService::secureZero(frame_, sizeof(frame_));
        used_ = 0;
        break;
      }
      if (used_ || overflow_) ++rejected_;
      used_ = 0; overflow_ = false;
      CryptoService::secureZero(frame_, sizeof(frame_));
      if (rejected_ >= 8) { stop(); state_ = ImportState::Expired; break; }
    } else if (c != '\r') {
      if (!c || used_ + 1 >= sizeof(frame_)) overflow_ = true;
      else if (!overflow_) { frame_[used_++] = c; frame_[used_] = 0; }
    }
  }
  CryptoService::secureZero(bytes, sizeof(bytes));
}
bool AccountImportService::decodeFrame() {
  if (std::strncmp(frame_, "NI1:", 4)) return false;
  char* first = std::strchr(frame_ + 4, ':');
  char* second = first ? std::strchr(first + 1, ':') : nullptr;
  if (!first || !second || first - (frame_ + 4) != 24 || std::strlen(second + 1) != 32) return false;
  const size_t hexLength = second - first - 1;
  if (!hexLength || hexLength % 2 || hexLength > 192) return false;
  const size_t size = hexLength / 2;
  uint8_t iv[12]{}, cipher[96]{}, tag[16]{}, decoded[32]{};
  char plain[97]{};
  bool ok = unhex(frame_ + 4, 12, iv) && unhex(first + 1, size, cipher) && unhex(second + 1, 16, tag) &&
      CryptoService::aesGcmDecrypt(key_, 256, iv, 12, reinterpret_cast<const uint8_t*>(kAad), std::strlen(kAad),
          cipher, size, tag, 16, reinterpret_cast<uint8_t*>(plain));
  if (ok && std::memchr(plain, 0, size)) ok = false;
  char* secret = ok ? std::strchr(plain, '\t') : nullptr;
  char* digits = secret ? std::strchr(secret + 1, '\t') : nullptr;
  char* period = digits ? std::strchr(digits + 1, '\t') : nullptr;
  if (!secret || !digits || !period) ok = false;
  if (ok) {
    *secret++ = *digits++ = *period++ = 0;
    bool nonSpace = false;
    for (const char* p = plain; *p; ++p) { if (*p < 32 || *p > 126) ok = false; if (*p != ' ') nonSpace = true; }
    ok = ok && nonSpace && std::strlen(plain) < sizeof(candidate_.name) &&
        std::strlen(secret) < sizeof(candidate_.secret) &&
        (!std::strcmp(digits, "6") || !std::strcmp(digits, "8")) &&
        (!std::strcmp(period, "30") || !std::strcmp(period, "60")) &&
        TotpService::decodeBase32(secret, decoded, sizeof(decoded)) > 0;
    if (ok) {
      std::strcpy(candidate_.name, plain); std::strcpy(candidate_.secret, secret);
      candidate_.digits = digits[0] - '0'; candidate_.period = period[0] == '3' ? 30 : 60;
    }
  }
  CryptoService::secureZero(plain, sizeof(plain));
  CryptoService::secureZero(decoded, sizeof(decoded));
  return ok;
}
