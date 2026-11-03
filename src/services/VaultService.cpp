#include "services/VaultService.h"
#include "services/CryptoService.h"
#include <Arduino.h>
#include <Preferences.h>
#include <cstring>
#include <cstdio>

namespace {
constexpr uint32_t kBackupPbkdf2Iterations = 2048;

bool isValidEntry(const VaultEntry& e) {
  // Title must not be empty and must be printable ASCII
  if (e.title[0] == '\0') return false;
  for (size_t i = 0; i < sizeof(e.title) && e.title[i] != '\0'; ++i) {
    if (e.title[i] < 32 || e.title[i] > 126) return false;
  }
  return true;
}
}

void VaultService::begin() {
  lock();
}

void VaultService::lock() {
  CryptoService::secureZero(entries_, sizeof(entries_));
  count_ = 0;
  loaded_ = false;
  storageError_ = false;
}

const VaultEntry* VaultService::getEntry(uint8_t index) const {
  if (!loaded_ || index >= count_) return nullptr;
  return &entries_[index];
}

bool VaultService::addEntry(const uint8_t* activeVaultKey, const char* title, const char* username,
                            const char* password, const char* notes) {
  if (!activeVaultKey || !loaded_ || count_ >= kMaxVaultEntries || !title || title[0] == '\0') {
    return false;
  }
  VaultEntry candidate{};
  strncpy(candidate.title, title, sizeof(candidate.title) - 1);
  if (username) strncpy(candidate.username, username, sizeof(candidate.username) - 1);
  if (password) strncpy(candidate.password, password, sizeof(candidate.password) - 1);
  if (notes) strncpy(candidate.notes, notes, sizeof(candidate.notes) - 1);

  entries_[count_++] = candidate;
  CryptoService::secureZero(&candidate, sizeof(candidate));

  if (saveVault(activeVaultKey)) {
    Serial.printf("[VaultService] Added entry '%s', total: %u\n", entries_[count_ - 1].title, count_);
    return true;
  }

  // Rollback on persistence failure
  CryptoService::secureZero(&entries_[--count_], sizeof(VaultEntry));
  return false;
}

bool VaultService::updateEntry(const uint8_t* activeVaultKey, uint8_t index, const char* title,
                               const char* username, const char* password, const char* notes) {
  if (!activeVaultKey || !loaded_ || index >= count_ || !title || title[0] == '\0') {
    return false;
  }
  VaultEntry previous = entries_[index];
  VaultEntry candidate{};
  strncpy(candidate.title, title, sizeof(candidate.title) - 1);
  if (username) strncpy(candidate.username, username, sizeof(candidate.username) - 1);
  if (password) strncpy(candidate.password, password, sizeof(candidate.password) - 1);
  if (notes) strncpy(candidate.notes, notes, sizeof(candidate.notes) - 1);

  entries_[index] = candidate;
  CryptoService::secureZero(&candidate, sizeof(candidate));

  if (saveVault(activeVaultKey)) {
    CryptoService::secureZero(&previous, sizeof(previous));
    return true;
  }

  // Rollback
  entries_[index] = previous;
  CryptoService::secureZero(&previous, sizeof(previous));
  return false;
}

bool VaultService::removeEntry(const uint8_t* activeVaultKey, uint8_t index) {
  if (!activeVaultKey || !loaded_ || index >= count_) return false;

  VaultEntry previous[kMaxVaultEntries]{};
  memcpy(previous, entries_, sizeof(previous));

  for (uint8_t i = index; i + 1 < count_; ++i) {
    entries_[i] = entries_[i + 1];
  }
  CryptoService::secureZero(&entries_[--count_], sizeof(VaultEntry));

  if (saveVault(activeVaultKey)) {
    CryptoService::secureZero(previous, sizeof(previous));
    Serial.printf("[VaultService] Removed entry at %u, remaining: %u\n", index, count_);
    return true;
  }

  // Rollback
  memcpy(entries_, previous, sizeof(entries_));
  ++count_;
  CryptoService::secureZero(previous, sizeof(previous));
  return false;
}

bool VaultService::loadVault(const uint8_t* activeVaultKey) {
  lock();
  storageError_ = true;
  if (!activeVaultKey) return false;

  Preferences prefs;
  if (!prefs.begin(kNamespace, false)) return false;

  if (!prefs.isKey("vault")) {
    prefs.end();
    loaded_ = true;
    storageError_ = false;
    return true; // Fresh, empty vault
  }

  const size_t length = prefs.getBytesLength("vault");
  if (length < kPrefixSize || length > kMaxBlobSize) {
    prefs.end();
    return false;
  }

  uint8_t blob[kMaxBlobSize]{};
  if (prefs.getBytes("vault", blob, length) != length) {
    prefs.end();
    CryptoService::secureZero(blob, sizeof(blob));
    return false;
  }
  prefs.end();

  // Validate Header: 'N', 'V', version 1
  if (blob[0] != 'N' || blob[1] != 'V' || blob[2] != 1) {
    CryptoService::secureZero(blob, sizeof(blob));
    return false;
  }

  const uint8_t count = blob[3];
  if (count > kMaxVaultEntries) {
    CryptoService::secureZero(blob, sizeof(blob));
    return false;
  }

  const size_t expectedDataBytes = sizeof(VaultEntry) * count;
  if (length != kPrefixSize + expectedDataBytes) {
    CryptoService::secureZero(blob, sizeof(blob));
    return false;
  }

  VaultEntry plain[kMaxVaultEntries]{};
  const uint8_t* iv = blob + kHeaderSize;
  const uint8_t* tag = blob + kHeaderSize + kIvSize;
  const uint8_t* ciphertext = blob + kPrefixSize;

  bool ok = CryptoService::aesGcmDecrypt(activeVaultKey, 256, iv, kIvSize,
                                         blob, kHeaderSize,
                                         ciphertext, expectedDataBytes,
                                         tag, kTagSize,
                                         reinterpret_cast<uint8_t*>(plain));

  if (ok) {
    for (uint8_t i = 0; i < count; ++i) {
      if (!isValidEntry(plain[i])) {
        ok = false;
        break;
      }
    }
  }

  if (ok) {
    memcpy(entries_, plain, expectedDataBytes);
    count_ = count;
    loaded_ = true;
    storageError_ = false;
    Serial.printf("[VaultService] Loaded %u encrypted entries successfully.\n", count_);
  }

  CryptoService::secureZero(plain, sizeof(plain));
  CryptoService::secureZero(blob, sizeof(blob));
  return ok;
}

bool VaultService::saveVault(const uint8_t* activeVaultKey) {
  if (!activeVaultKey || !loaded_) return false;

  Preferences prefs;
  if (!prefs.begin(kNamespace, false)) return false;

  if (count_ == 0) {
    const bool ok = prefs.clear();
    prefs.end();
    return ok;
  }

  uint8_t blob[kMaxBlobSize]{};
  blob[0] = 'N';
  blob[1] = 'V';
  blob[2] = 1;
  blob[3] = count_;

  uint8_t* iv = blob + kHeaderSize;
  uint8_t* tag = blob + kHeaderSize + kIvSize;
  uint8_t* ciphertext = blob + kPrefixSize;

  CryptoService::fillRandom(iv, kIvSize);
  const size_t dataBytes = sizeof(VaultEntry) * count_;

  bool ok = CryptoService::aesGcmEncrypt(activeVaultKey, 256, iv, kIvSize,
                                         blob, kHeaderSize,
                                         reinterpret_cast<const uint8_t*>(entries_), dataBytes,
                                         ciphertext, tag, kTagSize);

  if (ok) {
    const size_t totalBytes = kPrefixSize + dataBytes;
    ok = (prefs.putBytes("vault", blob, totalBytes) == totalBytes);
  }

  prefs.end();
  CryptoService::secureZero(blob, sizeof(blob));
  return ok;
}

bool VaultService::exportBackup(const char* passphrase, uint8_t* outPayload, size_t maxLen, size_t& outLen) const {
  outLen = 0;
  if (!passphrase || passphrase[0] == '\0' || !loaded_ || !outPayload) return false;

  const size_t dataBytes = sizeof(VaultEntry) * count_;
  const size_t totalBytes = kBackupPrefixSize + dataBytes;
  if (maxLen < totalBytes) return false;

  // 1. Header: 'N', 'B', 'K', 1
  outPayload[0] = 'N';
  outPayload[1] = 'B';
  outPayload[2] = 'K';
  outPayload[3] = 1;

  uint8_t* salt = outPayload + kBackupHeaderSize;
  uint8_t* iv = salt + kBackupSaltSize;
  uint8_t* tag = iv + kIvSize;
  uint8_t* ciphertext = tag + kTagSize;

  // 2. Generate cryptographically secure Salt and IV
  CryptoService::fillRandom(salt, kBackupSaltSize);
  CryptoService::fillRandom(iv, kIvSize);

  // 3. Derive 256-bit backup key from passphrase + salt
  uint8_t backupKey[32]{};
  if (!CryptoService::deriveKey(passphrase, strlen(passphrase), salt, kBackupSaltSize,
                                kBackupPbkdf2Iterations, backupKey, sizeof(backupKey))) {
    CryptoService::secureZero(backupKey, sizeof(backupKey));
    return false;
  }

  // 4. Authenticated AES-256-GCM encryption with Associated Authenticated Data (Header + Salt)
  const size_t aadLen = kBackupHeaderSize + kBackupSaltSize;
  bool ok = CryptoService::aesGcmEncrypt(backupKey, 256, iv, kIvSize,
                                         outPayload, aadLen,
                                         reinterpret_cast<const uint8_t*>(entries_), dataBytes,
                                         ciphertext, tag, kTagSize);

  CryptoService::secureZero(backupKey, sizeof(backupKey));

  if (ok) {
    outLen = totalBytes;
    Serial.printf("[VaultService] Exported authenticated backup (%u entries, %u bytes)\n", count_, (unsigned)totalBytes);
  }
  return ok;
}

bool VaultService::importBackup(const char* passphrase, const uint8_t* payload, size_t len,
                                const uint8_t* activeVaultKey) {
  if (!passphrase || !payload || !activeVaultKey || !loaded_) return false;
  if (len < kBackupPrefixSize) return false;

  // Validate Header: 'N', 'B', 'K', 1
  if (payload[0] != 'N' || payload[1] != 'B' || payload[2] != 'K' || payload[3] != 1) {
    return false;
  }

  const size_t dataBytes = len - kBackupPrefixSize;
  if (dataBytes % sizeof(VaultEntry) != 0) return false;

  const uint8_t newCount = static_cast<uint8_t>(dataBytes / sizeof(VaultEntry));
  if (newCount > kMaxVaultEntries) return false;

  const uint8_t* salt = payload + kBackupHeaderSize;
  const uint8_t* iv = salt + kBackupSaltSize;
  const uint8_t* tag = iv + kIvSize;
  const uint8_t* ciphertext = tag + kTagSize;

  // Derive 256-bit backup key
  uint8_t backupKey[32]{};
  if (!CryptoService::deriveKey(passphrase, strlen(passphrase), salt, kBackupSaltSize,
                                kBackupPbkdf2Iterations, backupKey, sizeof(backupKey))) {
    CryptoService::secureZero(backupKey, sizeof(backupKey));
    return false;
  }

  VaultEntry plain[kMaxVaultEntries]{};
  const size_t aadLen = kBackupHeaderSize + kBackupSaltSize;
  bool ok = CryptoService::aesGcmDecrypt(backupKey, 256, iv, kIvSize,
                                         payload, aadLen,
                                         ciphertext, dataBytes,
                                         tag, kTagSize,
                                         reinterpret_cast<uint8_t*>(plain));

  CryptoService::secureZero(backupKey, sizeof(backupKey));

  if (!ok) {
    Serial.println("[VaultService] Backup decryption failed! Tag mismatch or invalid passphrase.");
    CryptoService::secureZero(plain, sizeof(plain));
    return false; // Fail closed
  }

  // Validate all entries
  for (uint8_t i = 0; i < newCount; ++i) {
    if (!isValidEntry(plain[i])) {
      CryptoService::secureZero(plain, sizeof(plain));
      return false;
    }
  }

  // Atomically update in-memory entries and persist to NVS
  VaultEntry previous[kMaxVaultEntries]{};
  memcpy(previous, entries_, sizeof(previous));
  const uint8_t prevCount = count_;

  memcpy(entries_, plain, sizeof(entries_));
  count_ = newCount;
  CryptoService::secureZero(plain, sizeof(plain));

  if (saveVault(activeVaultKey)) {
    CryptoService::secureZero(previous, sizeof(previous));
    Serial.printf("[VaultService] Successfully imported and persisted %u entries from backup.\n", count_);
    return true;
  }

  // Rollback on NVS failure
  memcpy(entries_, previous, sizeof(entries_));
  count_ = prevCount;
  CryptoService::secureZero(previous, sizeof(previous));
  return false;
}
