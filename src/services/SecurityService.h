#pragma once
#include <stdint.h>
#include <stddef.h>

struct AppContext;
struct TouchEvent;

/**
 * @brief Hardened device lock and vault security manager.
 * Uses PBKDF2-HMAC-SHA256 for PIN verification, maintains an in-memory session key
 * that is wiped on lock/sleep, and enforces reboot-persistent brute-force lockouts.
 */
class SecurityService {
public:
  void begin();

  bool isPinEnabled() const { return pinEnabled_; }
  bool setPinEnabled(bool en);
  void setCredentialChangeGuard(bool (*guard)()) { credentialChangeGuard_ = guard; }

  bool isLocked() const { return locked_; }
  void lock();
  void unlock();

  // Verifies entered PIN against PBKDF2 hash using constant-time comparison
  bool verifyPin(const char* pin);

  // Updates PIN: generates a new 16-byte salt and derives a 32-byte PBKDF2 hash (4-8 digits)
  bool setPin(const char* newPin);
  uint8_t getPinLength() const { return pinLength_; }

  // Active vault key (32 bytes) available in memory ONLY when device is unlocked
  bool hasActiveKey() const { return (!locked_ && activeKeyValid_); }
  const uint8_t* getActiveVaultKey() const { return (hasActiveKey() ? activeVaultKey_ : nullptr); }

  // Brute-force lockout state
  bool isLockedOut(uint32_t nowMs) const;
  uint32_t getLockoutRemainingSec(uint32_t nowMs) const;

  void tick(uint32_t nowMs);
  void handleTouch(AppContext& ctx, const TouchEvent& ev);
  void render(AppContext& ctx);
  bool consumeDirty() { bool d = dirty_; dirty_ = false; return d; }
  void invalidate() { dirty_ = true; }

private:
  bool pinEnabled_ = false;
  bool locked_ = false;
  bool dirty_ = true;
  uint8_t pinLength_ = 4;

  // Salt & PBKDF2 hash (32 bytes derived from PIN + salt)
  uint8_t salt_[16]{};
  uint8_t pinHash_[32]{};
  bool hashInitialized_ = false;

  // Session Vault Key kept in SRAM only while unlocked; wiped on lock
  uint8_t activeVaultKey_[32]{};
  bool activeKeyValid_ = false;

  // Lock screen input buffer
  char inputPin_[10]{};
  uint8_t inputLen_ = 0;
  bool errorState_ = false;
  uint32_t errorUntilMs_ = 0;

  // Anti-brute-force rate limiting
  uint32_t failedAttempts_ = 0;
  uint32_t lockoutUntilMs_ = 0;

  void load();
  bool save();
  bool (*credentialChangeGuard_)() = nullptr;
  bool writable_ = true;
  void recordFailedAttempt();
  void resetFailedAttempts();
  void wipeSessionKey();
};
