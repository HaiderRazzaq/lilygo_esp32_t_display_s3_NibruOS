#include "services/TotpService.h"
#include "services/CryptoService.h"
#include <Arduino.h>
#include <Preferences.h>
#include <string.h>

namespace {
constexpr const char* kTotpNamespace = "nibru_totp";
}

void TotpService::begin() {
  // If no accounts loaded, provide demo accounts for immediate testing
  if (accountCount_ == 0) {
    seedDefaultDemoAccounts();
  }
}

int TotpService::decodeBase32(const char* str, uint8_t* out, size_t maxOut) {
  if (!str || !out || maxOut == 0) return -1;

  int buffer = 0;
  int bitsLeft = 0;
  size_t count = 0;

  for (size_t i = 0; str[i] != '\0'; ++i) {
    char ch = str[i];
    if (ch == ' ' || ch == '-' || ch == '=') continue; // Skip formatting

    int val = -1;
    if (ch >= 'A' && ch <= 'Z') val = ch - 'A';
    else if (ch >= 'a' && ch <= 'z') val = ch - 'a';
    else if (ch >= '2' && ch <= '7') val = ch - '2' + 26;
    else return -1; // Invalid character

    buffer = (buffer << 5) | val;
    bitsLeft += 5;

    if (bitsLeft >= 8) {
      if (count >= maxOut) return -1; // Overflow
      out[count++] = (buffer >> (bitsLeft - 8)) & 0xFF;
      bitsLeft -= 8;
    }
  }

  return static_cast<int>(count);
}

bool TotpService::computeTotp(const uint8_t* secret, size_t secretLen,
                              uint32_t epoch, uint8_t periodSec, uint8_t digits,
                              char* outCode, size_t maxCodeLen, uint8_t& outRemainingSec) {
  if (!secret || secretLen == 0 || periodSec == 0 || digits < 6 || digits > 8 || !outCode || maxCodeLen <= digits) {
    return false;
  }

  outRemainingSec = periodSec - (epoch % periodSec);

  // Time step counter in big-endian 64-bit integer
  uint64_t step = epoch / periodSec;
  uint8_t msg[8];
  for (int i = 7; i >= 0; --i) {
    msg[i] = static_cast<uint8_t>(step & 0xFF);
    step >>= 8;
  }

  // HMAC-SHA1 produces a 20-byte MAC
  uint8_t mac[20]{};
  if (!CryptoService::hmacSha1(secret, secretLen, msg, sizeof(msg), mac)) {
    return false;
  }

  // Dynamic truncation (RFC 4226 / RFC 6238)
  const int offset = mac[19] & 0x0F;
  const uint32_t binary = ((mac[offset] & 0x7F) << 24) |
                          ((mac[offset + 1] & 0xFF) << 16) |
                          ((mac[offset + 2] & 0xFF) << 8) |
                          (mac[offset + 3] & 0xFF);

  CryptoService::secureZero(mac, sizeof(mac));

  // Compute modulo for requested number of digits
  uint32_t mod = 1000000;
  if (digits == 7) mod = 10000000;
  else if (digits == 8) mod = 100000000;

  const uint32_t codeNum = binary % mod;

  if (digits == 6) {
    snprintf(outCode, maxCodeLen, "%06u", codeNum);
  } else if (digits == 7) {
    snprintf(outCode, maxCodeLen, "%07u", codeNum);
  } else {
    snprintf(outCode, maxCodeLen, "%08u", codeNum);
  }

  return true;
}

const TotpAccount* TotpService::getAccount(uint8_t index) const {
  if (index >= accountCount_) return nullptr;
  return &accounts_[index];
}

bool TotpService::addAccount(const char* name, const char* user, const char* base32Secret,
                             uint8_t digits, uint8_t period) {
  if (accountCount_ >= kMaxTotpAccounts || !name || !base32Secret) return false;

  uint8_t decoded[32]{};
  int dLen = decodeBase32(base32Secret, decoded, sizeof(decoded));
  if (dLen <= 0) return false;

  TotpAccount& acc = accounts_[accountCount_];
  strncpy(acc.name, name, sizeof(acc.name) - 1);
  acc.name[sizeof(acc.name) - 1] = '\0';

  if (user) {
    strncpy(acc.user, user, sizeof(acc.user) - 1);
    acc.user[sizeof(acc.user) - 1] = '\0';
  } else {
    acc.user[0] = '\0';
  }

  memcpy(acc.secret, decoded, dLen);
  acc.secretLen = static_cast<uint8_t>(dLen);
  acc.digits = (digits >= 6 && digits <= 8) ? digits : 6;
  acc.period = (period > 0) ? period : 30;

  CryptoService::secureZero(decoded, sizeof(decoded));
  accountCount_++;
  return true;
}

bool TotpService::removeAccount(uint8_t index) {
  if (index >= accountCount_) return false;

  CryptoService::secureZero(&accounts_[index], sizeof(TotpAccount));
  for (uint8_t i = index; i + 1 < accountCount_; ++i) {
    accounts_[i] = accounts_[i + 1];
  }
  accountCount_--;
  CryptoService::secureZero(&accounts_[accountCount_], sizeof(TotpAccount));
  return true;
}

bool TotpService::getAccountCode(uint8_t index, uint32_t epoch, char* outCode, size_t maxCodeLen,
                                uint8_t& outRemainingSec) const {
  const TotpAccount* acc = getAccount(index);
  if (!acc) return false;

  return computeTotp(acc->secret, acc->secretLen, epoch, acc->period, acc->digits,
                     outCode, maxCodeLen, outRemainingSec);
}

void TotpService::seedDefaultDemoAccounts() {
  // Standard RFC 6238 test seeds (Base32: "JBSWY3DPEHPK3PXP" corresponds to secret "1234567890")
  addAccount("GitHub", "dev-sec", "JBSWY3DPEHPK3PXP", 6, 30);
  addAccount("Google", "primary", "JBSWY3DPEHPK3PXQ", 6, 30);
  addAccount("Proton", "vault", "JBSWY3DPEHPK3PXR", 6, 30);
}

bool TotpService::loadVault(const uint8_t* vaultKey) {
  if (!vaultKey) return false;
  Preferences prefs;
  if (!prefs.begin(kTotpNamespace, true)) return false;

  if (!prefs.isKey("cnt") || !prefs.isKey("iv") || !prefs.isKey("tag") || !prefs.isKey("data")) {
    prefs.end();
    return false;
  }

  uint8_t storedCount = prefs.getUChar("cnt", 0);
  if (storedCount > kMaxTotpAccounts) {
    prefs.end();
    return false;
  }

  uint8_t iv[12]{};
  uint8_t tag[16]{};
  prefs.getBytes("iv", iv, sizeof(iv));
  prefs.getBytes("tag", tag, sizeof(tag));

  const size_t dataLen = sizeof(TotpAccount) * storedCount;
  uint8_t* cipher = new uint8_t[dataLen];
  prefs.getBytes("data", cipher, dataLen);
  prefs.end();

  uint8_t* plain = new uint8_t[dataLen];
  bool ok = CryptoService::aesGcmDecrypt(vaultKey, 256, iv, sizeof(iv), nullptr, 0,
                                        cipher, dataLen, tag, sizeof(tag), plain);
  if (ok) {
    accountCount_ = storedCount;
    memcpy(accounts_, plain, dataLen);
  }

  CryptoService::secureZero(plain, dataLen);
  delete[] plain;
  delete[] cipher;
  return ok;
}

bool TotpService::saveVault(const uint8_t* vaultKey) {
  if (!vaultKey) return false;

  Preferences prefs;
  if (!prefs.begin(kTotpNamespace, false)) return false;

  uint8_t iv[12]{};
  CryptoService::fillRandom(iv, sizeof(iv));

  const size_t dataLen = sizeof(TotpAccount) * accountCount_;
  uint8_t* cipher = new uint8_t[dataLen];
  uint8_t tag[16]{};

  bool ok = CryptoService::aesGcmEncrypt(vaultKey, 256, iv, sizeof(iv), nullptr, 0,
                                        reinterpret_cast<const uint8_t*>(accounts_), dataLen,
                                        cipher, tag, sizeof(tag));
  if (ok) {
    prefs.putUChar("cnt", accountCount_);
    prefs.putBytes("iv", iv, sizeof(iv));
    prefs.putBytes("tag", tag, sizeof(tag));
    prefs.putBytes("data", cipher, dataLen);
  }

  delete[] cipher;
  prefs.end();
  return ok;
}

