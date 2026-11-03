#pragma once
#include <stdint.h>
#include <stddef.h>

constexpr uint8_t kMaxVaultEntries = 16;
constexpr size_t kVaultTitleMax = 24;
constexpr size_t kVaultUserMax = 32;
constexpr size_t kVaultPasswordMax = 48;
constexpr size_t kVaultNotesMax = 24;

/**
 * @brief Discrete entry within the hardened password vault.
 */
struct VaultEntry {
  char title[kVaultTitleMax]{};
  char username[kVaultUserMax]{};
  char password[kVaultPasswordMax]{};
  char notes[kVaultNotesMax]{};
};

/**
 * @brief Hardened encrypted password vault storage engine.
 * Encrypts credentials with AES-256-GCM using in-memory session keys derived via PBKDF2,
 * enforces constant-time checks, zeros buffers on lock, and provides authenticated backups.
 */
class VaultService {
public:
  void begin();
  void lock(); // Securely wipes plaintext buffers from SRAM

  bool isLoaded() const { return loaded_; }
  bool hasStorageError() const { return storageError_; }

  // Record management
  uint8_t count() const { return count_; }
  const VaultEntry* getEntry(uint8_t index) const;

  // Transactional operations: encrypts and commits using active session vault key
  bool addEntry(const uint8_t* activeVaultKey, const char* title, const char* username,
                const char* password, const char* notes = "");
  bool updateEntry(const uint8_t* activeVaultKey, uint8_t index, const char* title,
                   const char* username, const char* password, const char* notes = "");
  bool removeEntry(const uint8_t* activeVaultKey, uint8_t index);

  // Vault persistence (AES-256-GCM)
  bool loadVault(const uint8_t* activeVaultKey);
  bool saveVault(const uint8_t* activeVaultKey);

  // Encrypted Backup & Restore:
  // Derives an independent backup key via PBKDF2 from a passphrase,
  // encrypts with AES-256-GCM, and produces an authenticated standalone payload.
  bool exportBackup(const char* passphrase, uint8_t* outPayload, size_t maxLen, size_t& outLen) const;

  // Verifies and decrypts backup payload; if authentic, saves with active session vault key.
  bool importBackup(const char* passphrase, const uint8_t* payload, size_t len,
                    const uint8_t* activeVaultKey);

private:
  VaultEntry entries_[kMaxVaultEntries]{};
  uint8_t count_ = 0;
  bool loaded_ = false;
  bool storageError_ = false;

  static constexpr const char* kNamespace = "nibru_vlt";
  static constexpr size_t kHeaderSize = 4; // 'N', 'V', 1, count
  static constexpr size_t kIvSize = 12;
  static constexpr size_t kTagSize = 16;
  static constexpr size_t kPrefixSize = kHeaderSize + kIvSize + kTagSize; // 32 bytes
  static constexpr size_t kMaxBlobSize = kPrefixSize + sizeof(VaultEntry) * kMaxVaultEntries;

  static constexpr size_t kBackupHeaderSize = 4; // 'N', 'B', 'K', 1
  static constexpr size_t kBackupSaltSize = 16;
  static constexpr size_t kBackupPrefixSize = kBackupHeaderSize + kBackupSaltSize + kIvSize + kTagSize; // 48 bytes
};
