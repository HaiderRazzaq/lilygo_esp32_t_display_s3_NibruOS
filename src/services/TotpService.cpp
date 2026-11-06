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

bool TotpService::computeOtp(const uint8_t* secret, size_t secretLen,
                             uint64_t step, OtpAlgo algo, uint8_t digits,
                             char* outCode, size_t maxCodeLen) {
  if (!secret || secretLen == 0 || digits < 6 || digits > 8 || !outCode || maxCodeLen <= digits) {
    return false;
  }

  // Time / counter step counter in big-endian 64-bit integer
  uint8_t msg[8];
  for (int i = 7; i >= 0; --i) {
    msg[i] = static_cast<uint8_t>(step & 0xFF);
    step >>= 8;
  }

  uint8_t mac[64]{};
  size_t hashLen = 20;
  bool ok = false;

  switch (algo) {
    case OtpAlgo::Sha256:
      hashLen = 32;
      ok = CryptoService::hmacSha256(secret, secretLen, msg, sizeof(msg), mac);
      break;
    case OtpAlgo::Sha512:
      hashLen = 64;
      ok = CryptoService::hmacSha512(secret, secretLen, msg, sizeof(msg), mac);
      break;
    case OtpAlgo::Sha1:
    default:
      hashLen = 20;
      ok = CryptoService::hmacSha1(secret, secretLen, msg, sizeof(msg), mac);
      break;
  }

  if (!ok) {
    CryptoService::secureZero(mac, sizeof(mac));
    return false;
  }

  // Dynamic truncation (RFC 4226 / RFC 6238)
  const int offset = mac[hashLen - 1] & 0x0F;
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

bool TotpService::computeTotp(const uint8_t* secret, size_t secretLen,
                              uint32_t epoch, uint8_t periodSec, uint8_t digits,
                              OtpAlgo algo, char* outCode, size_t maxCodeLen,
                              uint8_t& outRemainingSec) {
  if (periodSec == 0) return false;
  outRemainingSec = periodSec - (epoch % periodSec);
  const uint64_t step = epoch / periodSec;
  return computeOtp(secret, secretLen, step, algo, digits, outCode, maxCodeLen);
}

bool TotpService::computeHotp(const uint8_t* secret, size_t secretLen,
                              uint32_t counter, uint8_t digits, OtpAlgo algo,
                              char* outCode, size_t maxCodeLen) {
  return computeOtp(secret, secretLen, static_cast<uint64_t>(counter), algo, digits, outCode, maxCodeLen);
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
struct TotpAccountV1 {
  char name[24]{};
  char user[24]{};
  uint8_t secret[32]{};
  uint8_t secretLen = 0;
  uint8_t digits = 6;
  uint8_t period = 30;
};
static_assert(sizeof(TotpAccountV1) == 83, "Vault v1 uses fixed 83-byte records");

constexpr size_t kHeaderSize = 4;
constexpr size_t kPrefixSize = kHeaderSize + 12 + 16;
constexpr size_t kMaxBlobSize = kPrefixSize + sizeof(TotpAccount) * kMaxTotpAccounts;

bool validAccount(const TotpAccount& a) {
  return a.name[0] && memchr(a.name, 0, sizeof(a.name)) &&
         memchr(a.user, 0, sizeof(a.user)) && a.secretLen > 0 &&
         a.secretLen <= sizeof(a.secret) && a.digits >= 6 && a.digits <= 8 &&
         (a.type == 1 || a.period > 0) && a.algo <= 2;
}

bool validAccountV1(const TotpAccountV1& a) {
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
                             const char* base32, uint8_t digits, uint8_t period,
                             OtpType type, OtpAlgo algo, uint32_t initialCounter) {
  if (!key || !loaded_ || accountCount_ >= kMaxTotpAccounts || !name || !name[0] ||
      strlen(name) >= sizeof(TotpAccount::name) || (user && strlen(user) >= sizeof(TotpAccount::user)) ||
      !base32 || digits < 6 || digits > 8) return false;
  if (type == OtpType::Totp && !period) return false;

  TotpAccount candidate{};
  const int length = decodeBase32(base32, candidate.secret, sizeof(candidate.secret));
  if (length <= 0) { CryptoService::secureZero(&candidate, sizeof(candidate)); return false; }
  strncpy(candidate.name, name, sizeof(candidate.name) - 1);
  if (user) strncpy(candidate.user, user, sizeof(candidate.user) - 1);
  candidate.secretLen = static_cast<uint8_t>(length);
  candidate.digits = digits;
  candidate.period = period ? period : 30;
  candidate.type = static_cast<uint8_t>(type);
  candidate.algo = static_cast<uint8_t>(algo);
  candidate.counter = initialCounter;

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

bool TotpService::incrementHotp(const uint8_t* vaultKey, uint8_t index) {
  if (!vaultKey || !loaded_ || index >= accountCount_) return false;
  if (accounts_[index].type != static_cast<uint8_t>(OtpType::Hotp)) return false;
  accounts_[index].counter++;
  return saveVault(vaultKey);
}

bool TotpService::getAccountCode(uint8_t index, uint32_t epoch, char* code,
                                 size_t capacity, uint8_t& remaining) const {
  const TotpAccount* account = getAccount(index);
  if (!account) return false;
  if (account->type == static_cast<uint8_t>(OtpType::Hotp)) {
    remaining = 0;
    return computeHotp(account->secret, account->secretLen, account->counter,
                       account->digits, static_cast<OtpAlgo>(account->algo), code, capacity);
  }
  return computeTotp(account->secret, account->secretLen, epoch,
                     account->period, account->digits, static_cast<OtpAlgo>(account->algo),
                     code, capacity, remaining);
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
  uint8_t count = 0;
  bool ok = false;
  if (prefs.isKey("vault")) {
    const size_t length = prefs.getBytesLength("vault");
    if (length >= kPrefixSize && length <= sizeof(blob) &&
        prefs.getBytes("vault", blob, length) == length && blob[0] == 'N' && blob[1] == 'T') {
      count = blob[3];
      if (blob[2] == 2 && count <= kMaxTotpAccounts &&
          length == kPrefixSize + sizeof(TotpAccount) * count) {
        TotpAccount plain[kMaxTotpAccounts]{};
        ok = CryptoService::aesGcmDecrypt(key, 256, blob + kHeaderSize, 12,
            blob, kHeaderSize, blob + kPrefixSize, sizeof(TotpAccount) * count,
            blob + kHeaderSize + 12, 16, reinterpret_cast<uint8_t*>(plain));
        if (ok) {
          for (uint8_t i = 0; i < count; ++i) if (!validAccount(plain[i])) ok = false;
        }
        if (ok) {
          memcpy(accounts_, plain, sizeof(accounts_));
          accountCount_ = count;
        }
        CryptoService::secureZero(plain, sizeof(plain));
      } else if (blob[2] == 1 && count <= kMaxTotpAccounts &&
                 length == kPrefixSize + sizeof(TotpAccountV1) * count) {
        TotpAccountV1 plainV1[kMaxTotpAccounts]{};
        ok = CryptoService::aesGcmDecrypt(key, 256, blob + kHeaderSize, 12,
            blob, kHeaderSize, blob + kPrefixSize, sizeof(TotpAccountV1) * count,
            blob + kHeaderSize + 12, 16, reinterpret_cast<uint8_t*>(plainV1));
        if (ok) {
          for (uint8_t i = 0; i < count; ++i) if (!validAccountV1(plainV1[i])) ok = false;
        }
        if (ok) {
          accountCount_ = count;
          for (uint8_t i = 0; i < count; ++i) {
            strncpy(accounts_[i].name, plainV1[i].name, sizeof(accounts_[i].name) - 1);
            strncpy(accounts_[i].user, plainV1[i].user, sizeof(accounts_[i].user) - 1);
            memcpy(accounts_[i].secret, plainV1[i].secret, plainV1[i].secretLen);
            accounts_[i].secretLen = plainV1[i].secretLen;
            accounts_[i].digits = plainV1[i].digits;
            accounts_[i].period = plainV1[i].period;
            accounts_[i].type = static_cast<uint8_t>(OtpType::Totp);
            accounts_[i].algo = static_cast<uint8_t>(OtpAlgo::Sha1);
            accounts_[i].counter = 0;
          }
        }
        CryptoService::secureZero(plainV1, sizeof(plainV1));
      }
    }
  } else if (prefs.isKey("cnt") && prefs.getBytesLength("iv") == 12 &&
             prefs.getBytesLength("tag") == 16) {
    // Read-only compatibility with previous multi-key encrypted format (v1).
    count = prefs.getUChar("cnt", 255);
    const size_t bytes = sizeof(TotpAccountV1) * count;
    if (count <= kMaxTotpAccounts && prefs.getBytesLength("data") == bytes &&
        prefs.getBytes("iv", blob + kHeaderSize, 12) == 12 &&
        prefs.getBytes("tag", blob + kHeaderSize + 12, 16) == 16 &&
        prefs.getBytes("data", blob + kPrefixSize, bytes) == bytes) {
      TotpAccountV1 plainV1[kMaxTotpAccounts]{};
      ok = CryptoService::aesGcmDecrypt(key, 256, blob + kHeaderSize, 12, nullptr, 0,
          blob + kPrefixSize, bytes, blob + kHeaderSize + 12, 16,
          reinterpret_cast<uint8_t*>(plainV1));
      if (ok) {
        for (uint8_t i = 0; i < count; ++i) if (!validAccountV1(plainV1[i])) ok = false;
      }
      if (ok) {
        accountCount_ = count;
        for (uint8_t i = 0; i < count; ++i) {
          strncpy(accounts_[i].name, plainV1[i].name, sizeof(accounts_[i].name) - 1);
          strncpy(accounts_[i].user, plainV1[i].user, sizeof(accounts_[i].user) - 1);
          memcpy(accounts_[i].secret, plainV1[i].secret, plainV1[i].secretLen);
          accounts_[i].secretLen = plainV1[i].secretLen;
          accounts_[i].digits = plainV1[i].digits;
          accounts_[i].period = plainV1[i].period;
          accounts_[i].type = static_cast<uint8_t>(OtpType::Totp);
          accounts_[i].algo = static_cast<uint8_t>(OtpAlgo::Sha1);
          accounts_[i].counter = 0;
        }
      }
      CryptoService::secureZero(plainV1, sizeof(plainV1));
    }
  }
  prefs.end();
  if (ok) {
    loaded_ = true;
    storageError_ = false;
  }
  CryptoService::secureZero(blob, sizeof(blob));
  return ok;
}

bool TotpService::saveVault(const uint8_t* key) {
  if (!key || !loaded_) return false;
  Preferences prefs;
  if (!prefs.begin(kTotpNamespace, false)) return false;
  if (!accountCount_) { const bool ok = prefs.clear(); prefs.end(); return ok; }
  uint8_t blob[kMaxBlobSize]{};
  blob[0] = 'N'; blob[1] = 'T'; blob[2] = 2; blob[3] = accountCount_;
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
