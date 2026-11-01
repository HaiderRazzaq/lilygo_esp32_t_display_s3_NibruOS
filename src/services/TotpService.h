#pragma once
#include <stdint.h>
#include <stddef.h>

constexpr uint8_t kMaxTotpAccounts = 8;

struct TotpAccount {
  char name[24]{};
  char user[24]{};
  uint8_t secret[32]{};
  uint8_t secretLen = 0;
  uint8_t digits = 6;
  uint8_t period = 30;
};

/**
 * @brief RFC 6238 Time-Based One-Time Password (TOTP) generator and encrypted account manager.
 */
class TotpService {
public:
  void begin();
  void lock();
  bool isLoaded() const { return loaded_; }
  bool hasStorageError() const { return storageError_; }
  // Fail closed if storage is unavailable; changing PIN would change the vault key.
  static bool canChangeCredentials();

  // Decodes RFC 4648 Base32 string into binary buffer
  static int decodeBase32(const char* base32Str, uint8_t* outBuf, size_t maxOutLen);

  // Computes 6-digit TOTP code for a secret and Unix epoch timestamp
  static bool computeTotp(const uint8_t* secret, size_t secretLen,
                          uint32_t epoch, uint8_t periodSec, uint8_t digits,
                          char* outCode, size_t maxCodeLen, uint8_t& outRemainingSec);

  // Account management
  uint8_t accountCount() const { return accountCount_; }
  const TotpAccount* getAccount(uint8_t index) const;

  bool addAccount(const uint8_t* vaultKey, const char* name, const char* user, const char* base32Secret,
                  uint8_t digits = 6, uint8_t period = 30);
  bool removeAccount(const uint8_t* vaultKey, uint8_t index);

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

