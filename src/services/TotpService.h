#pragma once
#include <stdint.h>
#include <stddef.h>

constexpr uint8_t kMaxTotpAccounts = 12;

enum class OtpType : uint8_t {
  Totp = 0,
  Hotp = 1
};

enum class OtpAlgo : uint8_t {
  Sha1 = 0,
  Sha256 = 1,
  Sha512 = 2
};

struct TotpAccount {
  char name[24]{};
  char user[24]{};
  uint8_t secret[64]{};
  uint8_t secretLen = 0;
  uint8_t digits = 6;
  uint8_t period = 30;
  uint8_t type = 0;     // 0 = OtpType::Totp, 1 = OtpType::Hotp
  uint8_t algo = 0;     // 0 = OtpAlgo::Sha1, 1 = OtpAlgo::Sha256, 2 = OtpAlgo::Sha512
  uint8_t reserved[3]{};
  uint32_t counter = 0; // for HOTP
};

static_assert(sizeof(TotpAccount) == 124, "TotpAccount v2 must be 124 bytes");

/**
 * @brief RFC 6238 TOTP and RFC 4226 HOTP generator supporting SHA-1, SHA-256, SHA-512
 * and encrypted account vault storage.
 */
class TotpService {
public:
  void begin();
  void lock();
  bool isLoaded() const { return loaded_; }
  bool hasStorageError() const { return storageError_; }
  // Fail closed if storage is unavailable; changing PIN would change the vault key.
  static bool canChangeCredentials();

  // Decodes RFC 4648 Base32 string into binary buffer (supports up to 64 bytes for SHA-512)
  static int decodeBase32(const char* base32Str, uint8_t* outBuf, size_t maxOutLen);

  // Computes raw OTP code for a given step counter
  static bool computeOtp(const uint8_t* secret, size_t secretLen,
                         uint64_t step, OtpAlgo algo, uint8_t digits,
                         char* outCode, size_t maxCodeLen);

  // Computes TOTP code for a secret and Unix epoch timestamp
  static bool computeTotp(const uint8_t* secret, size_t secretLen,
                          uint32_t epoch, uint8_t periodSec, uint8_t digits,
                          OtpAlgo algo, char* outCode, size_t maxCodeLen,
                          uint8_t& outRemainingSec);

  // Computes HOTP code for a secret and counter
  static bool computeHotp(const uint8_t* secret, size_t secretLen,
                          uint32_t counter, uint8_t digits, OtpAlgo algo,
                          char* outCode, size_t maxCodeLen);

  // Account management
  uint8_t accountCount() const { return accountCount_; }
  const TotpAccount* getAccount(uint8_t index) const;

  bool addAccount(const uint8_t* vaultKey, const char* name, const char* user, const char* base32Secret,
                  uint8_t digits = 6, uint8_t period = 30,
                  OtpType type = OtpType::Totp, OtpAlgo algo = OtpAlgo::Sha1,
                  uint32_t initialCounter = 0);
  bool removeAccount(const uint8_t* vaultKey, uint8_t index);

  // Increments HOTP counter and updates encrypted vault
  bool incrementHotp(const uint8_t* vaultKey, uint8_t index);

  // Computes code for registered account
  bool getAccountCode(uint8_t index, uint32_t epoch, char* outCode, size_t maxCodeLen,
                      uint8_t& outRemainingSec) const;

  // Vault persistence (encrypted using AES-256-GCM with active session key)
  bool loadVault(const uint8_t* vaultKey);
  bool saveVault(const uint8_t* vaultKey);

private:
  TotpAccount accounts_[kMaxTotpAccounts]{};
  uint8_t accountCount_ = 0;

  bool loaded_ = false;
  bool storageError_ = false;
};

