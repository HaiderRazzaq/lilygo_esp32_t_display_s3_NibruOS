#include "services/TotpService.h"
#include "services/CryptoService.h"
#include <Arduino.h>
#include <Preferences.h>
#include <string.h>

namespace {
constexpr const char* kTotpNamespace = "nibru_totp";
}

void TotpService::begin() {
  lock();
}

int TotpService::decodeBase32(const char* str, uint8_t* out, size_t maxOut) {
  if (!str || !out || maxOut == 0) return -1;

  uint32_t buffer = 0;
  int bitsLeft = 0;
  size_t count = 0;
  bool padding = false;
  size_t symbols = 0;

  for (size_t i = 0; str[i] != '\0'; ++i) {
    char ch = str[i];
    if (ch == ' ' || ch == '-') continue;
    if (ch == '=') { padding = true; continue; }
    if (padding) return -1;

    int val = -1;
    if (ch >= 'A' && ch <= 'Z') val = ch - 'A';
    else if (ch >= 'a' && ch <= 'z') val = ch - 'a';
    else if (ch >= '2' && ch <= '7') val = ch - '2' + 26;
    else return -1; // Invalid character
    ++symbols;

    buffer = (buffer << 5) | val;
    bitsLeft += 5;

    if (bitsLeft >= 8) {
      if (count >= maxOut) return -1; // Overflow
      out[count++] = (buffer >> (bitsLeft - 8)) & 0xFF;
      bitsLeft -= 8;
    }
  }

  // Only complete Base32 encodings with zero trailing bits are accepted.
  const size_t rem = symbols % 8;
  if (count == 0 || rem == 1 || rem == 3 || rem == 6 ||
      (bitsLeft && (buffer & ((1U << bitsLeft) - 1U)))) return -1;
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
  return loaded_ && index < accountCount_ ? &accounts_[index] : nullptr;
}

void TotpService::lock() {
  CryptoService::secureZero(accounts_, sizeof(accounts_));
  accountCount_ = 0;
  loaded_ = false;
  storageError_ = false;
}

namespace {
constexpr size_t kHeaderSize = 4;
constexpr size_t kPrefixSize = kHeaderSize + 12 + 16;
constexpr size_t kMaxBlobSize = kPrefixSize + sizeof(TotpAccount) * kMaxTotpAccounts;
static_assert(sizeof(TotpAccount) == 83, "Vault v1 uses fixed 83-byte records");

bool validAccount(const TotpAccount& a) {
  return a.name[0] && memchr(a.name, 0, sizeof(a.name)) &&
         memchr(a.user, 0, sizeof(a.user)) && a.secretLen > 0 &&
         a.secretLen <= sizeof(a.secret) && a.digits >= 6 && a.digits <= 8 && a.period > 0;
}
bool hasRecords(Preferences& prefs) {
  return prefs.isKey("vault") || prefs.isKey("cnt") || prefs.isKey("data") ||
         prefs.isKey("iv") || prefs.isKey("tag");
}
}

bool TotpService::canChangeCredentials() {
  Preferences prefs;
  if (!prefs.begin(kTotpNamespace, false)) return false;
  const bool allowed = !hasRecords(prefs);
  prefs.end();
  return allowed;
}

bool TotpService::addAccount(const uint8_t* key, const char* name, const char* user,
                             const char* base32, uint8_t digits, uint8_t period) {
  if (!key || !loaded_ || accountCount_ >= kMaxTotpAccounts || !name || !name[0] ||
      strlen(name) >= sizeof(TotpAccount::name) || (user && strlen(user) >= sizeof(TotpAccount::user)) ||
      !base32 || digits < 6 || digits > 8 || !period) return false;
  TotpAccount candidate{};
  const int length = decodeBase32(base32, candidate.secret, sizeof(candidate.secret));
  if (length <= 0) { CryptoService::secureZero(&candidate, sizeof(candidate)); return false; }
  strcpy(candidate.name, name);
  if (user) strcpy(candidate.user, user);
  candidate.secretLen = static_cast<uint8_t>(length);
  candidate.digits = digits;
  candidate.period = period;
  accounts_[accountCount_++] = candidate;
  CryptoService::secureZero(&candidate, sizeof(candidate));
  if (saveVault(key)) return true;
  CryptoService::secureZero(&accounts_[--accountCount_], sizeof(TotpAccount));
  return false;
}

bool TotpService::removeAccount(const uint8_t* key, uint8_t index) {
  if (!key || !loaded_ || index >= accountCount_) return false;
  TotpAccount previous[kMaxTotpAccounts]{};
  memcpy(previous, accounts_, sizeof(previous));
  for (uint8_t i = index; i + 1 < accountCount_; ++i) accounts_[i] = accounts_[i + 1];
  CryptoService::secureZero(&accounts_[--accountCount_], sizeof(TotpAccount));
  const bool saved = saveVault(key);
  if (!saved) { ++accountCount_; memcpy(accounts_, previous, sizeof(previous)); }
  CryptoService::secureZero(previous, sizeof(previous));
  return saved;
}

bool TotpService::getAccountCode(uint8_t index, uint32_t epoch, char* code,
                                size_t capacity, uint8_t& remaining) const {
  const TotpAccount* account = getAccount(index);
  return account && computeTotp(account->secret, account->secretLen, epoch,
      account->period, account->digits, code, capacity, remaining);
}

bool TotpService::loadVault(const uint8_t* key) {
  lock();
  storageError_ = true;
  if (!key) return false;
  Preferences prefs;
  if (!prefs.begin(kTotpNamespace, false)) return false;
  if (!hasRecords(prefs)) {
    prefs.end(); loaded_ = true; storageError_ = false; return true;
  }
  uint8_t blob[kMaxBlobSize]{};
  TotpAccount plain[kMaxTotpAccounts]{};
  uint8_t count = 0;
  bool ok = false;
  if (prefs.isKey("vault")) {
    const size_t length = prefs.getBytesLength("vault");
    if (length >= kPrefixSize && length <= sizeof(blob) &&
        prefs.getBytes("vault", blob, length) == length && blob[0] == 'N' &&
        blob[1] == 'T' && blob[2] == 1 && blob[3] <= kMaxTotpAccounts &&
        length == kPrefixSize + sizeof(TotpAccount) * blob[3]) {
      count = blob[3];
      ok = CryptoService::aesGcmDecrypt(key, 256, blob + kHeaderSize, 12,
          blob, kHeaderSize, blob + kPrefixSize, sizeof(TotpAccount) * count,
          blob + kHeaderSize + 12, 16, reinterpret_cast<uint8_t*>(plain));
    }
  } else if (prefs.isKey("cnt") && prefs.getBytesLength("iv") == 12 &&
             prefs.getBytesLength("tag") == 16) {
    // Read-only compatibility with the previous multi-key encrypted format.
    // The next successful edit writes v1; failures never overwrite old records.
    count = prefs.getUChar("cnt", 255);
    const size_t bytes = sizeof(TotpAccount) * count;
    if (count <= kMaxTotpAccounts && prefs.getBytesLength("data") == bytes &&
        prefs.getBytes("iv", blob + kHeaderSize, 12) == 12 &&
        prefs.getBytes("tag", blob + kHeaderSize + 12, 16) == 16 &&
        prefs.getBytes("data", blob + kPrefixSize, bytes) == bytes) {
      ok = CryptoService::aesGcmDecrypt(key, 256, blob + kHeaderSize, 12, nullptr, 0,
          blob + kPrefixSize, bytes, blob + kHeaderSize + 12, 16,
          reinterpret_cast<uint8_t*>(plain));
    }
  }
  prefs.end();
  if (ok) {
    for (uint8_t i = 0; i < count; ++i) if (!validAccount(plain[i])) ok = false;
  }
  if (ok) {
    memcpy(accounts_, plain, sizeof(accounts_));
    accountCount_ = count;
    loaded_ = true;
    storageError_ = false;
  }
  CryptoService::secureZero(plain, sizeof(plain));
  CryptoService::secureZero(blob, sizeof(blob));
  return ok;
}

bool TotpService::saveVault(const uint8_t* key) {
  if (!key || !loaded_) return false;
  Preferences prefs;
  if (!prefs.begin(kTotpNamespace, false)) return false;
  // Explicit deletion of the last account removes only the TOTP namespace.
  if (!accountCount_) { const bool ok = prefs.clear(); prefs.end(); return ok; }
  uint8_t blob[kMaxBlobSize]{};
  blob[0] = 'N'; blob[1] = 'T'; blob[2] = 1; blob[3] = accountCount_;
  CryptoService::fillRandom(blob + kHeaderSize, 12);
  const size_t bytes = sizeof(TotpAccount) * accountCount_;
  bool ok = CryptoService::aesGcmEncrypt(key, 256, blob + kHeaderSize, 12,
      blob, kHeaderSize, reinterpret_cast<const uint8_t*>(accounts_), bytes,
      blob + kPrefixSize, blob + kHeaderSize + 12, 16);
  if (ok) ok = prefs.putBytes("vault", blob, kPrefixSize + bytes) == kPrefixSize + bytes;
  prefs.end();
  CryptoService::secureZero(blob, sizeof(blob));
  return ok;
}
