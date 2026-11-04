#include "apps/GeneratorApp.h"
#include "core/AppContext.h"
#include "hardware/DisplayManager.h"
#include "services/CryptoService.h"
#include "services/UsbHidService.h"
#include "services/BleHidService.h"
#include "services/VaultService.h"
#include "services/SecurityService.h"
#include "ui/Theme.h"
#include <Arduino.h>
#include <cstdio>
#include <cstring>

void GeneratorApp::generatePassword(AppContext& ctx) {
  (void)ctx;
  static const char kLetters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  static const char kDigits[] = "0123456789";
  static const char kSymbols[] = "!@#$%^&*()-_=+";

  char charset[80]{};
  size_t charsetLen = 0;
  memcpy(charset, kLetters, sizeof(kLetters) - 1);
  charsetLen += sizeof(kLetters) - 1;

  if (includeDigits_) {
    memcpy(charset + charsetLen, kDigits, sizeof(kDigits) - 1);
    charsetLen += sizeof(kDigits) - 1;
  }
  if (includeSymbols_) {
    memcpy(charset + charsetLen, kSymbols, sizeof(kSymbols) - 1);
    charsetLen += sizeof(kSymbols) - 1;
  }

  uint8_t randomBytes[32]{};
  CryptoService::fillRandom(randomBytes, length_);

  for (uint8_t i = 0; i < length_; ++i) {
    currentPass_[i] = charset[randomBytes[i] % charsetLen];
  }
  currentPass_[length_] = '\0';
  CryptoService::secureZero(randomBytes, sizeof(randomBytes));
}

void GeneratorApp::onEnter(AppContext& ctx) {
  status_ = "";
  feedbackUntilMs_ = 0;
  generatePassword(ctx);
  fullRedraw_ = true;
  invalidate();
}

void GeneratorApp::onExit(AppContext& ctx) {
  (void)ctx;
  CryptoService::secureZero(currentPass_, sizeof(currentPass_));
  status_ = "";
  feedbackUntilMs_ = 0;
  pendingSave_ = false;
}

void GeneratorApp::onDisplayRestored(AppContext& ctx) {
  fullRedraw_ = true;
  if (pendingSave_) {
    pendingSave_ = false;
    if (ctx.security.hasActiveKey()) {
      if (!ctx.vault.isLoaded()) {
        ctx.vault.loadVault(ctx.security.getActiveVaultKey());
      }
      if (ctx.vault.isLoaded()) {
        char title[24]{};
        snprintf(title, sizeof(title), "Gen-%03u", static_cast<unsigned>(millis() % 1000));
        if (ctx.vault.addEntry(ctx.security.getActiveVaultKey(), title, "user", currentPass_, "GENERATED")) {
          status_ = "Saved to Vault!";
        } else {
          status_ = "Vault save failed";
        }
      } else {
        status_ = "Vault load failed";
      }
      feedbackUntilMs_ = millis() + 2500;
    }
  }
  invalidate();
}

void GeneratorApp::executeSave(AppContext& ctx) {
  if (!ctx.security.isPinEnabled()) {
    status_ = "Enable PIN in Settings";
    feedbackUntilMs_ = millis() + 2500;
    invalidate();
    return;
  }

  if (!ctx.security.hasActiveKey()) {
    pendingSave_ = true;
    ctx.security.lock();
    ctx.security.invalidateFull();
    return;
  }

  if (!ctx.vault.isLoaded()) {
    ctx.vault.loadVault(ctx.security.getActiveVaultKey());
  }

  if (!ctx.vault.isLoaded()) {
    status_ = "Vault load failed";
  } else {
    char title[24]{};
    snprintf(title, sizeof(title), "Gen-%03u", static_cast<unsigned>(millis() % 1000));
    if (ctx.vault.addEntry(ctx.security.getActiveVaultKey(), title, "user", currentPass_, "GENERATED")) {
      status_ = "Saved to Vault!";
    } else {
      status_ = "Vault save failed";
    }
  }
  feedbackUntilMs_ = millis() + 2500;
  invalidate();
}

void GeneratorApp::onTick(AppContext& ctx, uint32_t nowMs) {
  (void)ctx;
  if (feedbackUntilMs_ && static_cast<int32_t>(nowMs - feedbackUntilMs_) >= 0) {
    feedbackUntilMs_ = 0;
    status_ = "";
    invalidate();
  }
}

bool GeneratorApp::onBack(AppContext& ctx) {
  (void)ctx;
  return false; // return to Home launcher
}

void GeneratorApp::onTouch(AppContext& ctx, const TouchEvent& ev) {
  if (ev.gesture != TouchGesture::Tap) return;

  // Header Back button
  if (ev.x < 45 && ev.y < 35) {
    onBack(ctx);
    return;
  }

  // Length button: x: 12..98, y: 74..102
  if (ev.x >= 12 && ev.x <= 98 && ev.y >= 74 && ev.y <= 102) {
    if (length_ == 8) length_ = 12;
    else if (length_ == 12) length_ = 16;
    else if (length_ == 16) length_ = 20;
    else if (length_ == 20) length_ = 24;
    else length_ = 8;
    generatePassword(ctx);
    invalidate();
    return;
  }

  // Symbols toggle: x: 104..198, y: 74..102
  if (ev.x >= 104 && ev.x <= 198 && ev.y >= 74 && ev.y <= 102) {
    includeSymbols_ = !includeSymbols_;
    generatePassword(ctx);
    invalidate();
    return;
  }

  // Regen button: x: 204..308, y: 74..102
  if (ev.x >= 204 && ev.x <= 308 && ev.y >= 74 && ev.y <= 102) {
    generatePassword(ctx);
    status_ = "New pass generated";
    feedbackUntilMs_ = millis() + 1500;
    invalidate();
    return;
  }

  // Type USB button: x: 12..100, y: 110..146
  if (ev.x >= 12 && ev.x <= 100 && ev.y >= 110 && ev.y <= 146) {
    ctx.usbHid.typeText(currentPass_);
    status_ = "Typing to PC via USB...";
    feedbackUntilMs_ = millis() + 2000;
    invalidate();
    return;
  }

  // Type BLE button: x: 104..194, y: 110..146
  if (ev.x >= 104 && ev.x <= 194 && ev.y >= 110 && ev.y <= 146) {
    if (!ctx.bleHid.isEnabled()) {
      status_ = "Enable BLE in Settings";
    } else if (!ctx.bleHid.isConnected()) {
      status_ = "BLE not connected";
    } else {
      ctx.bleHid.typeText(currentPass_);
      status_ = "Typed via BLE!";
    }
    feedbackUntilMs_ = millis() + 2000;
    invalidate();
    return;
  }

  // Save to Vault button: x: 198..308, y: 110..146
  if (ev.x >= 198 && ev.x <= 308 && ev.y >= 110 && ev.y <= 146) {
    executeSave(ctx);
    return;
  }
}

void GeneratorApp::render(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();

  if (fullRedraw_) {
    screen.clear();
    screen.header(title(), true);
    fullRedraw_ = false;
  }

  // 1. Password Display Panel
  t.fillRoundRect(12, 36, 296, 32, 6, Colors::Panel);
  t.drawRoundRect(12, 36, 296, 32, 6, Colors::Cyan);
  const int passW = static_cast<int>(strlen(currentPass_)) * 12; // Size 2
  const int passX = (passW <= 280) ? (12 + (296 - passW) / 2) : 18;
  screen.label(passX, 44, currentPass_, Colors::Cyan, (passW <= 280 ? 2 : 1), Colors::Panel);

  // 2. Control Row (Length, Symbols, Regen)
  // Length Pill
  t.fillRoundRect(12, 74, 86, 28, 4, Colors::Bg);
  t.drawRoundRect(12, 74, 86, 28, 4, Colors::Border);
  char lenStr[16];
  snprintf(lenStr, sizeof(lenStr), "LEN: %u", length_);
  screen.label(24, 82, lenStr, Colors::White, 1, Colors::Bg);

  // Symbols Toggle Pill
  t.fillRoundRect(104, 74, 94, 28, 4, Colors::Bg);
  t.drawRoundRect(104, 74, 94, 28, 4, includeSymbols_ ? Colors::Green : Colors::Border);
  screen.label(114, 82, includeSymbols_ ? "SYM: ON" : "SYM: OFF", includeSymbols_ ? Colors::Green : Colors::Muted, 1, Colors::Bg);

  // Regen Pill
  t.fillRoundRect(204, 74, 104, 28, 4, Colors::Amber);
  screen.label(232, 82, "REGEN", Colors::Bg, 1, Colors::Amber);

  // 3. Action Buttons Row (USB, BLE, and Vault)
  // Type USB Button
  t.fillRoundRect(12, 110, 88, 34, 5, Colors::Cyan);
  screen.label(18, 122, "TYPE USB", Colors::Bg, 1, Colors::Cyan);

  // Type BLE Button
  const bool bleOk = ctx.bleHid.isEnabled() && ctx.bleHid.isConnected();
  t.fillRoundRect(104, 110, 90, 34, 5, bleOk ? Colors::Green : Colors::Panel);
  t.drawRoundRect(104, 110, 90, 34, 5, bleOk ? Colors::Green : Colors::Border);
  screen.label(114, 122, "TYPE BLE", bleOk ? Colors::Bg : Colors::Muted, 1, bleOk ? Colors::Green : Colors::Panel);

  // Save to Vault Button
  t.fillRoundRect(198, 110, 110, 34, 5, Colors::Amber);
  screen.label(216, 122, "+ VAULT", Colors::Bg, 1, Colors::Amber);

  // 4. Status / Feedback Banner
  if (status_ && status_[0] != '\0') {
    screen.footer(status_);
  } else {
    screen.footer(LocalizationService::instance().isArabic() ? "اضغط على الأزرار لتخصيص الرمز وحفظه" : "Tap buttons to customize & send");
  }
}
