#include "services/SecurityService.h"
#include "services/CryptoService.h"
#include "core/AppContext.h"
#include "hardware/DisplayManager.h"
#include "hardware/TouchManager.h"
#include <Arduino.h>
#include <Preferences.h>
#include <string.h>

namespace {
constexpr uint32_t kPbkdf2Iterations = 2048;
constexpr const char* kPrefNamespace = "nibru_sec";
}

void SecurityService::begin() {
  load();
  wipeSessionKey();

  if (pinEnabled_) {
    locked_ = true;
  } else {
    locked_ = false;
  }
  dirty_ = true;
}

void SecurityService::wipeSessionKey() {
  CryptoService::secureZero(activeVaultKey_, sizeof(activeVaultKey_));
  activeKeyValid_ = false;
}

void SecurityService::load() {
  pinEnabled_ = false; hashInitialized_ = false; failedAttempts_ = 0; lockoutUntilMs_ = 0;
  Preferences prefs;
  if (!prefs.begin(kPrefNamespace, false)) { writable_ = false; pinEnabled_ = true; return; }
  writable_ = true;

  // Versioned credential blob: atomic PIN/salt/hash update, no mixed credentials.
  if (prefs.isKey("credentials")) {
    uint8_t record[51]{};
    bool valid = prefs.getBytesLength("credentials") == sizeof(record) &&
        prefs.getBytes("credentials", record, sizeof(record)) == sizeof(record) &&
        record[0] == 1 && record[1] <= 1 && record[2] >= 4 && record[2] <= 8;
    failedAttempts_ = prefs.getUInt("fail_cnt", 0);
    prefs.end();
    if (!valid) { writable_ = false; pinEnabled_ = true; hashInitialized_ = false; return; }
    pinEnabled_ = record[1]; pinLength_ = record[2];
    memcpy(salt_, record + 3, 16); memcpy(pinHash_, record + 19, 32);
    hashInitialized_ = true;
    CryptoService::secureZero(record, sizeof(record));
    if (failedAttempts_ >= 3) lockoutUntilMs_ = millis() +
        (failedAttempts_ >= 10 ? 300000 : failedAttempts_ >= 5 ? 30000 : 5000);
    return;
  }

  pinEnabled_ = prefs.getBool("enabled", false);
  failedAttempts_ = prefs.getUInt("fail_cnt", 0);
  pinLength_ = prefs.getUChar("pin_len", 4);

  // Check for legacy plaintext migration
  if (prefs.isKey("pin")) {
    String legacy = prefs.getString("pin", "");
    prefs.end();
    if (setPin(legacy.c_str())) {
      if (prefs.begin(kPrefNamespace, false)) { prefs.remove("pin"); prefs.end(); }
      Serial.println("[Security] Migrated legacy PIN metadata.");
    } else { pinEnabled_ = true; writable_ = false; }
    if (legacy.length()) CryptoService::secureZero(&legacy[0], legacy.length());
    return;
  }

  // Load salt and hash
  if (prefs.isKey("salt") && prefs.isKey("hash")) {
    size_t sLen = prefs.getBytes("salt", salt_, sizeof(salt_));
    size_t hLen = prefs.getBytes("hash", pinHash_, sizeof(pinHash_));
    if (sLen == sizeof(salt_) && hLen == sizeof(pinHash_)) {
      hashInitialized_ = true;
    }
  }

  prefs.end();

  // Fresh devices require explicit PIN provisioning. Existing vaults without
  // valid credentials must fail closed; never invent a default over user data.
  if (!hashInitialized_) {
    if (credentialChangeGuard_ && !credentialChangeGuard_()) {
      pinEnabled_ = true; writable_ = false;
    } else pinEnabled_ = false;
  }

  // If rebooted with active failed attempts penalty, enforce initial lockout
  if (failedAttempts_ >= 3) {
    uint32_t penaltyMs = (failedAttempts_ >= 10) ? 300000 : (failedAttempts_ >= 5 ? 30000 : 5000);
    lockoutUntilMs_ = millis() + penaltyMs;
    Serial.printf("[Security] Enforcing persistent lockout: %u ms\n", penaltyMs);
  }
}

bool SecurityService::save() {
  if (!writable_) return false;
  Preferences prefs;
  if (!prefs.begin(kPrefNamespace, false)) return false;
  uint8_t record[51] = {1, static_cast<uint8_t>(pinEnabled_), pinLength_};
  memcpy(record + 3, salt_, 16); memcpy(record + 19, pinHash_, 32);
  const bool ok = prefs.putBytes("credentials", record, sizeof(record)) == sizeof(record);
  prefs.end();
  CryptoService::secureZero(record, sizeof(record));
  return ok;
}

void SecurityService::recordFailedAttempt() {
  failedAttempts_++;
  Preferences prefs;
  if (prefs.begin(kPrefNamespace, false)) {
    prefs.putUInt("fail_cnt", failedAttempts_);
    prefs.end();
  }

  uint32_t penaltyMs = 1500;
  if (failedAttempts_ >= 10) {
    penaltyMs = 300000; // 5 minutes
  } else if (failedAttempts_ >= 5) {
    penaltyMs = 30000;  // 30 seconds
  } else if (failedAttempts_ >= 3) {
    penaltyMs = 5000;   // 5 seconds
  }
  lockoutUntilMs_ = millis() + penaltyMs;
  errorUntilMs_ = millis() + 1500;
  errorState_ = true;
}

void SecurityService::resetFailedAttempts() {
  if (failedAttempts_ > 0) {
    failedAttempts_ = 0;
    Preferences prefs;
    if (prefs.begin(kPrefNamespace, false)) {
      prefs.putUInt("fail_cnt", 0);
      prefs.end();
    }
  }
  lockoutUntilMs_ = 0;
  errorState_ = false;
}

bool SecurityService::isLockedOut(uint32_t nowMs) const {
  return (failedAttempts_ >= 3 && nowMs < lockoutUntilMs_);
}

uint32_t SecurityService::getLockoutRemainingSec(uint32_t nowMs) const {
  if (nowMs >= lockoutUntilMs_) return 0;
  return (lockoutUntilMs_ - nowMs + 999) / 1000;
}

bool SecurityService::setPinEnabled(bool en) {
  if (en && !hashInitialized_) return false;
  if (!en && credentialChangeGuard_ && !credentialChangeGuard_()) return false;
  const bool old = pinEnabled_;
  pinEnabled_ = en;
  if (!save()) { pinEnabled_ = old; return false; }
  if (!pinEnabled_) {
    wipeSessionKey();
    unlock();
  }
  return true;
}

bool SecurityService::setPin(const char* newPin) {
  if (!newPin || !writable_ || (credentialChangeGuard_ && !credentialChangeGuard_())) return false;
  const size_t len = strlen(newPin);
  if (len < 4 || len > 8) return false;

  for (size_t i = 0; i < len; ++i) {
    if (newPin[i] < '0' || newPin[i] > '9') return false;
  }

  uint8_t oldSalt[16], oldHash[32];
  memcpy(oldSalt, salt_, sizeof(salt_)); memcpy(oldHash, pinHash_, sizeof(pinHash_));
  const uint8_t oldLength = pinLength_;
  const bool oldInitialized = hashInitialized_;
  // Entropy prerequisites are supplied by the board CryptoService.
  CryptoService::fillRandom(salt_, sizeof(salt_));

  // Derive PBKDF2 verification hash
  if (!CryptoService::deriveKey(newPin, len, salt_, sizeof(salt_),
                                kPbkdf2Iterations, pinHash_, sizeof(pinHash_))) {
    memcpy(salt_, oldSalt, sizeof(salt_));
    memcpy(pinHash_, oldHash, sizeof(pinHash_));
    CryptoService::secureZero(oldSalt, sizeof(oldSalt));
    CryptoService::secureZero(oldHash, sizeof(oldHash));
    return false;
  }

  pinLength_ = static_cast<uint8_t>(len);
  hashInitialized_ = true;
  const bool saved = save();
  if (!saved) {
    memcpy(salt_, oldSalt, sizeof(salt_)); memcpy(pinHash_, oldHash, sizeof(pinHash_));
    pinLength_ = oldLength; hashInitialized_ = oldInitialized;
  } else { wipeSessionKey(); }
  CryptoService::secureZero(oldSalt, sizeof(oldSalt));
  CryptoService::secureZero(oldHash, sizeof(oldHash));
  if (!saved) return false;
  Serial.println("[Security] New PIN hash and salt stored securely.");
  return true;
}

bool SecurityService::verifyPin(const char* pin) {
  if (!pin || !hashInitialized_ || isLockedOut(millis())) return false;
  const size_t len = strlen(pin);

  uint8_t testHash[32]{};
  if (!CryptoService::deriveKey(pin, len, salt_, sizeof(salt_),
                                kPbkdf2Iterations, testHash, sizeof(testHash))) {
    return false;
  }

  // Constant-time comparison prevents timing analysis
  const bool match = CryptoService::constantTimeEquals(pinHash_, testHash, sizeof(pinHash_));
  CryptoService::secureZero(testHash, sizeof(testHash));

  if (match) {
    // Derive active session vault key using domain-separated salt
    uint8_t vaultSalt[16];
    memcpy(vaultSalt, salt_, sizeof(vaultSalt));
    vaultSalt[0] ^= 0xA5; // Domain separation

    if (CryptoService::deriveKey(pin, len, vaultSalt, sizeof(vaultSalt),
                                 kPbkdf2Iterations, activeVaultKey_, sizeof(activeVaultKey_))) {
      activeKeyValid_ = true;
    }
    CryptoService::secureZero(vaultSalt, sizeof(vaultSalt));
    if (!activeKeyValid_) return false;
    resetFailedAttempts();
    return true;
  } else {
    wipeSessionKey();
    recordFailedAttempt();
    return false;
  }
}

void SecurityService::lock() {
  if (!pinEnabled_) return;
  locked_ = true;
  wipeSessionKey();
  inputLen_ = 0;
  inputPin_[0] = '\0';
  errorState_ = false;
  dirty_ = true;
}

void SecurityService::unlock() {
  locked_ = false;
  inputLen_ = 0;
  inputPin_[0] = '\0';
  errorState_ = false;
  dirty_ = true;
}

void SecurityService::tick(uint32_t nowMs) {
  if (errorState_ && nowMs >= errorUntilMs_) {
    errorState_ = false;
    dirty_ = true;
  }
}

void SecurityService::handleTouch(AppContext&, const TouchEvent& ev) {
  if (ev.gesture != TouchGesture::Tap && ev.gesture != TouchGesture::DoubleTap) return;

  const uint32_t now = millis();
  if (isLockedOut(now)) {
    // Ignore input during brute-force lockout
    return;
  }

  const char keys[3][4] = {
    {'1', '2', '3', '4'},
    {'5', '6', '7', '8'},
    {'9', '0', 'D', 'E'}
  };

  for (int row = 0; row < 3; ++row) {
    const int ry = 70 + row * 26;
    if (ev.y >= ry && ev.y <= ry + 22) {
      for (int col = 0; col < 4; ++col) {
        const int rx = 16 + col * 74;
        if (ev.x >= rx && ev.x <= rx + 68) {
          const char k = keys[row][col];
          if (k >= '0' && k <= '9') {
            if (inputLen_ < 8) {
              inputPin_[inputLen_++] = k;
              inputPin_[inputLen_] = '\0';
              dirty_ = true;
              if (inputLen_ == pinLength_) {
                if (verifyPin(inputPin_)) {
                  unlock();
                  Serial.println("[Security] PIN verified. Device unlocked.");
                  return;
                } else {
                  inputLen_ = 0;
                  inputPin_[0] = '\0';
                  dirty_ = true;
                  Serial.printf("[Security] Invalid PIN! Total failures: %u\n", failedAttempts_);
                  return;
                }
              }
            }
          } else if (k == 'D') {
            if (inputLen_ > 0) {
              inputPin_[--inputLen_] = '\0';
              errorState_ = false;
              dirty_ = true;
            }
          } else if (k == 'E') {
            if (inputLen_ >= 4) {
              if (verifyPin(inputPin_)) {
                unlock();
                Serial.println("[Security] PIN verified via ENT. Device unlocked.");
                return;
              } else {
                inputLen_ = 0;
                inputPin_[0] = '\0';
                dirty_ = true;
                Serial.printf("[Security] Invalid PIN via ENT! Failures: %u\n", failedAttempts_);
                return;
              }
            } else {
              errorState_ = true;
              errorUntilMs_ = millis() + 1500;
              dirty_ = true;
            }
          }
          return;
        }
      }
    }
  }
}

void SecurityService::render(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();
  const uint32_t now = millis();

  screen.clear(Colors::Bg);

  // Top header bar
  t.fillRect(0, 0, 320, 28, Colors::Panel);
  t.drawFastHLine(0, 28, 320, Colors::Border);
  screen.label(86, 6, "NibruOS Locked", Colors::Cyan, 2, Colors::Panel);

  // Status and subtitle
  if (isLockedOut(now)) {
    char lockMsg[40];
    snprintf(lockMsg, sizeof(lockMsg), "Locked out! Wait %u s", getLockoutRemainingSec(now));
    screen.label(70, 34, lockMsg, Colors::Red, 1, Colors::Bg);
  } else if (errorState_) {
    screen.label(64, 34, "Incorrect PIN! Try again.", Colors::Red, 1, Colors::Bg);
  } else {
    char subMsg[36];
    snprintf(subMsg, sizeof(subMsg), "Enter %u-Digit Security PIN", pinLength_);
    const int subW = static_cast<int>(strlen(subMsg)) * 6;
    screen.label((320 - subW) / 2, 34, subMsg, Colors::White, 1, Colors::Bg);
  }

  // Centered PIN circles for dynamic pinLength_
  const int startX = 160 - (pinLength_ * 26) / 2;
  for (int i = 0; i < pinLength_; ++i) {
    const int cx = startX + i * 26 + 13;
    if (i < inputLen_) {
      t.fillCircle(cx, 50, 6, errorState_ ? Colors::Red : Colors::Green);
    } else {
      t.drawCircle(cx, 50, 6, Colors::Border);
    }
  }

  // Keypad: 3 rows x 4 cols
  const char* labels[3][4] = {
    {"1", "2", "3", "4"},
    {"5", "6", "7", "8"},
    {"9", "0", "DEL", "ENT"}
  };

  const bool lockedOut = isLockedOut(now);

  for (int row = 0; row < 3; ++row) {
    const int ry = 70 + row * 26;
    for (int col = 0; col < 4; ++col) {
      const int rx = 16 + col * 74;
      const bool isSpecial = (row == 2 && (col == 2 || col == 3));

      uint16_t btnColor = Colors::Panel;
      if (lockedOut) {
        btnColor = Colors::Muted;
      } else if (isSpecial) {
        btnColor = (col == 2) ? Colors::Red : Colors::Green;
      }

      t.fillRoundRect(rx, ry, 68, 22, 4, btnColor);
      t.drawRoundRect(rx, ry, 68, 22, 4, Colors::Border);

      const char* lbl = labels[row][col];
      const int textW = static_cast<int>(strlen(lbl)) * (isSpecial ? 6 : 12);
      const int tx = rx + (68 - textW) / 2;
      const int ty = ry + (isSpecial ? 7 : 4);
      screen.label(tx, ty, lbl, isSpecial ? (col == 2 ? Colors::White : Colors::Bg) : (lockedOut ? Colors::Bg : Colors::White),
                   isSpecial ? 1 : 2, btnColor);
    }
  }

  t.drawFastHLine(0, 151, 320, Colors::Border);
  t.fillRect(0, 152, 320, 18, Colors::Bg);
  if (lockedOut) {
    screen.label(40, 156, "Anti-Brute-Force Lockout Active", Colors::Amber, 1);
  } else {
    screen.label(48, 156, "Protected Device  |  Type PIN to unlock", Colors::Muted, 1);
  }
}
