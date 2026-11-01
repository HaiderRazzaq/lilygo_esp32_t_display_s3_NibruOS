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
  Preferences prefs;
  if (!prefs.begin(kPrefNamespace, false)) return;

  pinEnabled_ = prefs.getBool("enabled", false);
  failedAttempts_ = prefs.getUInt("fail_cnt", 0);
  pinLength_ = prefs.getUChar("pin_len", 4);

  // Check for legacy plaintext migration
  if (prefs.isKey("pin")) {
    String legacy = prefs.getString("pin", "1234");
    prefs.remove("pin");
    prefs.end();
    setPin(legacy.c_str());
    Serial.println("[Security] Migrated legacy plaintext PIN to PBKDF2-HMAC-SHA256.");
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

  // If no hash exists yet, initialize with default "1234"
  if (!hashInitialized_) {
    setPin("1234");
  }

  // If rebooted with active failed attempts penalty, enforce initial lockout
  if (failedAttempts_ >= 3) {
    uint32_t penaltyMs = (failedAttempts_ >= 10) ? 300000 : (failedAttempts_ >= 5 ? 30000 : 5000);
    lockoutUntilMs_ = millis() + penaltyMs;
    Serial.printf("[Security] Enforcing persistent lockout: %u ms\n", penaltyMs);
  }
}

void SecurityService::save() {
  Preferences prefs;
  if (!prefs.begin(kPrefNamespace, false)) return;

  prefs.putBool("enabled", pinEnabled_);
  prefs.putUChar("pin_len", pinLength_);
  prefs.putBytes("salt", salt_, sizeof(salt_));
  prefs.putBytes("hash", pinHash_, sizeof(pinHash_));
  prefs.putUInt("fail_cnt", failedAttempts_);
  prefs.end();
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

void SecurityService::setPinEnabled(bool en) {
  pinEnabled_ = en;
  save();
  if (!pinEnabled_) {
    unlock();
  }
}

bool SecurityService::setPin(const char* newPin) {
  if (!newPin) return false;
  const size_t len = strlen(newPin);
  if (len < 4 || len > 8) return false;

  for (size_t i = 0; i < len; ++i) {
    if (newPin[i] < '0' || newPin[i] > '9') return false;
  }

  // Generate 16 bytes of true hardware random salt
  CryptoService::fillRandom(salt_, sizeof(salt_));

  // Derive PBKDF2 verification hash
  if (!CryptoService::deriveKey(newPin, len, salt_, sizeof(salt_),
                                kPbkdf2Iterations, pinHash_, sizeof(pinHash_))) {
    return false;
  }

  pinLength_ = static_cast<uint8_t>(len);
  hashInitialized_ = true;
  save();
  Serial.println("[Security] New PIN hash and salt stored securely.");
  return true;
}

bool SecurityService::verifyPin(const char* pin) {
  if (!pin || !hashInitialized_) return false;
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
