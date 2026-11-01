#include "services/SecurityService.h"
#include "core/AppContext.h"
#include "hardware/DisplayManager.h"
#include "hardware/TouchManager.h"
#include <Arduino.h>
#include <Preferences.h>
#include <string.h>

void SecurityService::begin() {
  load();
  if (pinEnabled_) {
    locked_ = true;
  } else {
    locked_ = false;
  }
  dirty_ = true;
}

void SecurityService::load() {
  Preferences prefs;
  if (prefs.begin("nibru_sec", true)) {
    pinEnabled_ = prefs.getBool("enabled", false);
    String p = prefs.getString("pin", "1234");
    strncpy(pinCode_, p.c_str(), sizeof(pinCode_) - 1);
    pinCode_[sizeof(pinCode_) - 1] = '\0';
    prefs.end();
  }
}

void SecurityService::save() {
  Preferences prefs;
  if (prefs.begin("nibru_sec", false)) {
    prefs.putBool("enabled", pinEnabled_);
    prefs.putString("pin", pinCode_);
    prefs.end();
  }
}

void SecurityService::setPinEnabled(bool en) {
  pinEnabled_ = en;
  save();
  if (!pinEnabled_) {
    locked_ = false;
  }
}

bool SecurityService::verifyPin(const char* pin) const {
  if (!pin) return false;
  return (strcmp(pinCode_, pin) == 0);
}

bool SecurityService::setPin(const char* newPin) {
  if (!newPin || strlen(newPin) != 4) return false;
  for (int i = 0; i < 4; ++i) {
    if (newPin[i] < '0' || newPin[i] > '9') return false;
  }
  strncpy(pinCode_, newPin, sizeof(pinCode_) - 1);
  pinCode_[sizeof(pinCode_) - 1] = '\0';
  save();
  return true;
}

void SecurityService::lock() {
  if (!pinEnabled_) return;
  locked_ = true;
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

  const char keys[3][4] = {
    {'1', '2', '3', '4'},
    {'5', '6', '7', '8'},
    {'9', '0', 'C', 'D'}
  };

  for (int row = 0; row < 3; ++row) {
    const int ry = 70 + row * 26;
    if (ev.y >= ry && ev.y <= ry + 22) {
      for (int col = 0; col < 4; ++col) {
        const int rx = 16 + col * 74;
        if (ev.x >= rx && ev.x <= rx + 68) {
          const char k = keys[row][col];
          if (k >= '0' && k <= '9') {
            if (inputLen_ < 4) {
              inputPin_[inputLen_++] = k;
              inputPin_[inputLen_] = '\0';
              dirty_ = true;
              if (inputLen_ == 4) {
                if (verifyPin(inputPin_)) {
                  unlock();
                  Serial.println("[Security] PIN verified. Device unlocked.");
                  return;
                } else {
                  errorState_ = true;
                  errorUntilMs_ = millis() + 1500;
                  inputLen_ = 0;
                  inputPin_[0] = '\0';
                  dirty_ = true;
                  Serial.println("[Security] Invalid PIN attempted!");
                  return;
                }
              }
            }
          } else if (k == 'C') {
            inputLen_ = 0;
            inputPin_[0] = '\0';
            errorState_ = false;
            dirty_ = true;
          } else if (k == 'D') {
            if (inputLen_ > 0) {
              inputPin_[--inputLen_] = '\0';
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

  screen.clear(Colors::Bg);

  // Top header bar
  t.fillRect(0, 0, 320, 28, Colors::Panel);
  t.drawFastHLine(0, 28, 320, Colors::Border);
  screen.label(86, 6, "NibruOS Locked", Colors::Cyan, 2, Colors::Panel);

  // Subtitle
  if (errorState_) {
    screen.label(64, 34, "Incorrect PIN! Try again.", Colors::Red, 1, Colors::Bg);
  } else {
    screen.label(80, 34, "Enter 4-Digit Security PIN", Colors::White, 1, Colors::Bg);
  }

  // 4 PIN circles centered at Y = 50
  for (int i = 0; i < 4; ++i) {
    const int cx = 115 + i * 30;
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
    {"9", "0", "CLR", "DEL"}
  };

  for (int row = 0; row < 3; ++row) {
    const int ry = 70 + row * 26;
    for (int col = 0; col < 4; ++col) {
      const int rx = 16 + col * 74;
      const bool isSpecial = (row == 2 && (col == 2 || col == 3));
      t.fillRoundRect(rx, ry, 68, 22, 4, isSpecial ? (col == 2 ? Colors::Amber : Colors::Red) : Colors::Panel);
      t.drawRoundRect(rx, ry, 68, 22, 4, Colors::Border);

      const char* lbl = labels[row][col];
      const int textW = static_cast<int>(strlen(lbl)) * (isSpecial ? 6 : 12);
      const int tx = rx + (68 - textW) / 2;
      const int ty = ry + (isSpecial ? 7 : 4);
      screen.label(tx, ty, lbl, isSpecial ? (col == 2 ? Colors::Bg : Colors::White) : Colors::White,
                   isSpecial ? 1 : 2, isSpecial ? (col == 2 ? Colors::Amber : Colors::Red) : Colors::Panel);
    }
  }

  t.drawFastHLine(0, 151, 320, Colors::Border);
  t.fillRect(0, 152, 320, 18, Colors::Bg);
  screen.label(48, 156, "Protected Device  |  Type PIN to unlock", Colors::Muted, 1);
}

