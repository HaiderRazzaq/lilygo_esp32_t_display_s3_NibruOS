#include "services/VaultService.h"
#include "services/CryptoService.h"
#include "services/SecurityService.h"
#include <cassert>
#include <cstdio>
#include <cstring>

void runVaultTests() {
  std::printf("[TEST] Running Encrypted Password Vault Storage & Backup Tests...\n");

  // Mock active vault session key (32 bytes derived from PIN PBKDF2)
  uint8_t testVaultKey[32]{};
  for (int i = 0; i < 32; ++i) testVaultKey[i] = static_cast<uint8_t>(i + 1);

  VaultService vault;
  vault.begin();
  assert(!vault.isLoaded());
  assert(vault.count() == 0);

  // 1. Loading fresh vault initializes empty container
  assert(vault.loadVault(testVaultKey));
  assert(vault.isLoaded());
  assert(vault.count() == 0);

  // 2. Add credentials
  assert(vault.addEntry(testVaultKey, "GitHub", "octocat@github.com", "ghp_Secr3tToken12345!", "2FA Enabled"));
  assert(vault.addEntry(testVaultKey, "Server-SSH", "root", "p@ssw0rd!Super#Key", "192.168.1.100"));
  assert(vault.addEntry(testVaultKey, "Google", "user@gmail.com", "g00gl3-App-P@ss", "Personal"));
  assert(vault.count() == 3);

  const VaultEntry* e0 = vault.getEntry(0);
  assert(e0 != nullptr);
  assert(std::strcmp(e0->title, "GitHub") == 0);
  assert(std::strcmp(e0->username, "octocat@github.com") == 0);
  assert(std::strcmp(e0->password, "ghp_Secr3tToken12345!") == 0);
  assert(std::strcmp(e0->notes, "2FA Enabled") == 0);

  const VaultEntry* e1 = vault.getEntry(1);
  assert(e1 != nullptr);
  assert(std::strcmp(e1->title, "Server-SSH") == 0);
  assert(std::strcmp(e1->username, "root") == 0);

  // 3. Update entry
  assert(vault.updateEntry(testVaultKey, 0, "GitHub-Work", "work@company.com", "new_token_456", "Work SSO"));
  const VaultEntry* e0_updated = vault.getEntry(0);
  assert(std::strcmp(e0_updated->title, "GitHub-Work") == 0);
  assert(std::strcmp(e0_updated->username, "work@company.com") == 0);

  // 4. Lock wipes SRAM completely
  vault.lock();
  assert(!vault.isLoaded());
  assert(vault.count() == 0);
  assert(vault.getEntry(0) == nullptr);

  // 5. Load with wrong key must fail authentication (AES-GCM tag mismatch)
  uint8_t wrongKey[32]{};
  wrongKey[0] = 0xFF;
  assert(!vault.loadVault(wrongKey));
  assert(!vault.isLoaded());
  assert(vault.count() == 0);

  // 6. Load with correct key restores credentials accurately
  assert(vault.loadVault(testVaultKey));
  assert(vault.isLoaded());
  assert(vault.count() == 3);
  assert(std::strcmp(vault.getEntry(0)->title, "GitHub-Work") == 0);
  assert(std::strcmp(vault.getEntry(1)->title, "Server-SSH") == 0);
  assert(std::strcmp(vault.getEntry(2)->title, "Google") == 0);

  // 7. Remove an entry
  assert(vault.removeEntry(testVaultKey, 1)); // Remove Server-SSH
  assert(vault.count() == 2);
  assert(std::strcmp(vault.getEntry(0)->title, "GitHub-Work") == 0);
  assert(std::strcmp(vault.getEntry(1)->title, "Google") == 0);

  // 8. Encrypted Backup Export
  uint8_t backupPayload[2048]{};
  size_t backupLen = 0;
  const char* backupPass = "MyStrongBackupPassphrase2026!";
  assert(vault.exportBackup(backupPass, backupPayload, sizeof(backupPayload), backupLen));
  assert(backupLen > 48);
  assert(backupPayload[0] == 'N' && backupPayload[1] == 'B' && backupPayload[2] == 'K' && backupPayload[3] == 1);

  // 9. Tampering detection: Corrupting 1 byte in ciphertext must fail restoration
  uint8_t tamperedPayload[2048]{};
  std::memcpy(tamperedPayload, backupPayload, backupLen);
  tamperedPayload[backupLen - 1] ^= 0x01; // flip 1 bit
  assert(!vault.importBackup(backupPass, tamperedPayload, backupLen, testVaultKey));

  // 10. Wrong passphrase must fail restoration
  assert(!vault.importBackup("WrongPassphrase!", backupPayload, backupLen, testVaultKey));

  // 11. Remove all entries from active vault
  assert(vault.removeEntry(testVaultKey, 0));
  assert(vault.removeEntry(testVaultKey, 0));
  assert(vault.count() == 0);

  // 12. Valid import restores all backed-up entries cleanly
  assert(vault.importBackup(backupPass, backupPayload, backupLen, testVaultKey));
  assert(vault.count() == 2);
  assert(std::strcmp(vault.getEntry(0)->title, "GitHub-Work") == 0);
  assert(std::strcmp(vault.getEntry(1)->title, "Google") == 0);

  vault.lock();
  CryptoService::secureZero(testVaultKey, sizeof(testVaultKey));
  std::printf("  -> PASS: Encrypted Password Vault Storage & Backup verified.\n");
}
