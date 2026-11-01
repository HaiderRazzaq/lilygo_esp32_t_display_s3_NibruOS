#include "apps/TotpApp.h"
#include "core/AppContext.h"
#include "hardware/DisplayManager.h"
#include "hardware/TouchManager.h"
#include "services/TimeService.h"
#include "services/TotpService.h"
#include "services/UsbHidService.h"
#include "services/SecurityService.h"
#include <Arduino.h>

void TotpApp::onEnter(AppContext&) {
  accountIndex_ = 0;
  injectionFeedback_ = false;
  lastSecond_ = 0;
  fullRedraw_ = true;
  invalidate();
  Serial.println("[TotpApp] Entered single-card 2FA Authenticator.");
}

void TotpApp::onExit(AppContext&) {
  injectionFeedback_ = false;
  Serial.println("[TotpApp] Exited 2FA Authenticator.");
}

void TotpApp::onTick(AppContext& ctx, uint32_t nowMs) {
  if (injectionFeedback_ && nowMs >= feedbackUntilMs_) {
    injectionFeedback_ = false;
    invalidate();
  }

  uint32_t currentSec = ctx.time.getEpoch();
  if (currentSec == 0) {
    currentSec = nowMs / 1000;
  }

  if (currentSec != lastSecond_) {
    lastSecond_ = currentSec;
    invalidate();
  }
}

void TotpApp::onTouch(AppContext& ctx, const TouchEvent& ev) {
  if (ev.gesture != TouchGesture::Tap &&
      ev.gesture != TouchGesture::SwipeLeft &&
      ev.gesture != TouchGesture::SwipeRight) {
    return;
  }

  const uint8_t totalAccounts = ctx.totp.accountCount();
  if (totalAccounts == 0) return;

  // Swipe navigation between single accounts
  if (ev.gesture == TouchGesture::SwipeLeft) {
    if (accountIndex_ + 1 < totalAccounts) {
      accountIndex_++;
      fullRedraw_ = true;
      invalidate();
    }
    return;
  }
  if (ev.gesture == TouchGesture::SwipeRight) {
    if (accountIndex_ > 0) {
      accountIndex_--;
      fullRedraw_ = true;
      invalidate();
    }
    return;
  }

  if (ev.gesture == TouchGesture::Tap) {
    // 1. Prominent Large Left Navigation Button Hitbox (X <= 54, Y: 26..148)
    if (ev.x <= 54 && ev.y >= 26 && ev.y <= 148) {
      if (accountIndex_ > 0) {
        accountIndex_--;
        fullRedraw_ = true;
        invalidate();
      }
      return;
    }

    // 2. Prominent Large Right Navigation Button Hitbox (X >= 266, Y: 26..148)
    if (ev.x >= 266 && ev.y >= 26 && ev.y <= 148) {
      if (accountIndex_ + 1 < totalAccounts) {
        accountIndex_++;
        fullRedraw_ = true;
        invalidate();
      }
      return;
    }

    // 3. Bottom Footer Navigation Hitboxes (X < 80 or X > 240, Y >= 144)
    if (ev.y >= 144) {
      if (ev.x < 80 && accountIndex_ > 0) {
        accountIndex_--;
        fullRedraw_ = true;
        invalidate();
        return;
      }
      if (ev.x > 240 && accountIndex_ + 1 < totalAccounts) {
        accountIndex_++;
        fullRedraw_ = true;
        invalidate();
        return;
      }
    }

    // 4. Large Tactile [ TYPE CODE (USB) ] Button: X: 58..262, Y: 98..138
    if (ev.y >= 98 && ev.y <= 138 && ev.x >= 58 && ev.x <= 262) {
      char code[10]{};
      uint8_t rem = 0;
      uint32_t epoch = ctx.time.getEpoch();
      if (epoch == 0) epoch = millis() / 1000;
      if (ctx.totp.getAccountCode(accountIndex_, epoch, code, sizeof(code), rem)) {
        ctx.usbHid.typeCode(code, false);
        injectionFeedback_ = true;
        feedbackUntilMs_ = millis() + 1500;
        invalidate();
        Serial.printf("[TotpApp] Injected TOTP code for #%u: %s\n", accountIndex_, code);
      }
      return;
    }
  }
}

void TotpApp::renderCard(AppContext& ctx, uint8_t, const char* name,
                         const char* user, const char* code, uint8_t remainingSec) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();

  const int rx = 54;
  const int ry = 28;
  const int rw = 212;
  const int rh = 116;

  // Solid Center Card Background
  t.fillRoundRect(rx, ry, rw, rh, 6, Colors::Panel);
  t.drawRoundRect(rx, ry, rw, rh, 6, Colors::Border);

  // Row 1: Service Name & User Tag
  screen.label(rx + 10, ry + 8, name, Colors::Cyan, 2, Colors::Panel);
  screen.label(rx + 10, ry + 28, user, Colors::Muted, 1, Colors::Panel);

  // Row 1 Right: Countdown Timer Badge
  uint16_t timerColor = Colors::Green;
  if (remainingSec <= 5) {
    timerColor = Colors::Red;
  } else if (remainingSec <= 10) {
    timerColor = Colors::Amber;
  }

  char secStr[10];
  snprintf(secStr, sizeof(secStr), "%2u sec", remainingSec);
  t.fillRoundRect(rx + 140, ry + 8, 62, 20, 3, Colors::Bg);
  t.drawRoundRect(rx + 140, ry + 8, 62, 20, 3, timerColor);
  screen.label(rx + 146, ry + 14, secStr, timerColor, 1, Colors::Bg);

  // Row 2: Code display box (Compact, clean font size 2, centered inside)
  t.fillRoundRect(58, 66, 204, 30, 4, Colors::Bg);
  t.drawRoundRect(58, 66, 204, 30, 4, Colors::Border);

  char formattedCode[12]{};
  if (strlen(code) == 6) {
    snprintf(formattedCode, sizeof(formattedCode), "%.3s  %.3s", code, code + 3);
  } else {
    strncpy(formattedCode, code, sizeof(formattedCode) - 1);
  }
  // Centered at 58 + (204 - 96)/2 = 112
  screen.label(112, 73, formattedCode, Colors::White, 2, Colors::Bg);

  // Row 3: Large tactile [ TYPE CODE (USB) ] button
  const bool isTyped = injectionFeedback_;
  const uint16_t btnColor = isTyped ? Colors::Green : Colors::Cyan;
  t.fillRoundRect(58, 102, 204, 32, 5, btnColor);

  const char* btnLabel = isTyped ? "SENT TO USB!" : "TYPE CODE (USB)";
  const int lblW = static_cast<int>(strlen(btnLabel)) * 6;
  const int lblX = 58 + (204 - lblW) / 2;
  screen.label(lblX, 113, btnLabel, Colors::Bg, 1, btnColor);
}

void TotpApp::render(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();

  const uint8_t totalAccounts = ctx.totp.accountCount();

  if (fullRedraw_) {
    screen.clear(Colors::Bg);

    // Top Header Bar
    t.fillRect(0, 0, 320, 26, Colors::Panel);
    t.drawFastHLine(0, 26, 320, Colors::Border);

    // Back button indicator
    screen.label(6, 6, "< Back", Colors::Cyan, 1, Colors::Panel);

    // Centered Title
    screen.label(86, 6, "2FA Authenticator", Colors::White, 2, Colors::Panel);

    // Prominent Large Left Navigation Button (X=4, Y=28, W=46, H=116)
    const bool canPrev = (accountIndex_ > 0);
    t.fillRoundRect(4, 28, 46, 116, 6, canPrev ? Colors::Panel : Colors::Bg);
    t.drawRoundRect(4, 28, 46, 116, 6, canPrev ? Colors::Cyan : Colors::Border);
    if (canPrev) {
      t.drawRoundRect(5, 29, 44, 114, 5, Colors::Cyan); // Bold double border
      t.fillTriangle(14, 86, 36, 68, 36, 104, Colors::Cyan); // Large solid chevron
      screen.label(12, 114, "PREV", Colors::Cyan, 1, Colors::Panel);
    } else {
      t.drawTriangle(14, 86, 36, 68, 36, 104, Colors::Border);
    }

    // Prominent Large Right Navigation Button (X=270, Y=28, W=46, H=116)
    const bool canNext = (accountIndex_ + 1 < totalAccounts);
    t.fillRoundRect(270, 28, 46, 116, 6, canNext ? Colors::Panel : Colors::Bg);
    t.drawRoundRect(270, 28, 46, 116, 6, canNext ? Colors::Cyan : Colors::Border);
    if (canNext) {
      t.drawRoundRect(271, 29, 44, 114, 5, Colors::Cyan); // Bold double border
      t.fillTriangle(306, 86, 284, 68, 284, 104, Colors::Cyan); // Large solid chevron
      screen.label(278, 114, "NEXT", Colors::Cyan, 1, Colors::Panel);
    } else {
      t.drawTriangle(306, 86, 284, 70, 284, 104, Colors::Border);
    }

    // Bottom Navigation Bar
    t.drawFastHLine(0, 146, 320, Colors::Border);
    t.fillRect(0, 147, 320, 23, Colors::Bg);

    fullRedraw_ = false;
  }

  // Header Time / NTP Status Badge
  char timeBadge[16];
  if (ctx.time.isSynced()) {
    ctx.time.formatTime(timeBadge, sizeof(timeBadge));
    t.fillRect(255, 0, 65, 25, Colors::Panel);
    screen.label(260, 8, timeBadge, Colors::Green, 1, Colors::Panel);
  } else {
    t.fillRect(255, 0, 65, 25, Colors::Panel);
    screen.label(260, 8, "NTP Wait", Colors::Amber, 1, Colors::Panel);
  }

  if (totalAccounts == 0) {
    t.fillRect(42, 32, 236, 110, Colors::Bg);
    screen.label(70, 75, "No Accounts in Vault", Colors::Muted, 2, Colors::Bg);
    screen.label(50, 105, "Add accounts via Web Cabinet", Colors::White, 1, Colors::Bg);
    return;
  }

  if (accountIndex_ >= totalAccounts) {
    accountIndex_ = totalAccounts - 1;
  }

  uint32_t epoch = ctx.time.getEpoch();
  if (epoch == 0) {
    epoch = millis() / 1000;
  }

  // Render single active account
  const TotpAccount* acc = ctx.totp.getAccount(accountIndex_);
  char code[10] = "------";
  uint8_t remainingSec = 30;
  ctx.totp.getAccountCode(accountIndex_, epoch, code, sizeof(code), remainingSec);

  renderCard(ctx, accountIndex_, acc->name, acc->user, code, remainingSec);

  // Footer navigation counter
  t.fillRect(0, 147, 320, 23, Colors::Bg);

  if (accountIndex_ > 0) {
    screen.label(10, 153, "< Prev", Colors::Cyan, 1, Colors::Bg);
  }

  char navStr[36];
  snprintf(navStr, sizeof(navStr), "%u of %u  (Tap < > or Swipe)", accountIndex_ + 1, totalAccounts);
  const int navW = static_cast<int>(strlen(navStr)) * 6;
  const int navX = (320 - navW) / 2;
  screen.label(navX, 153, navStr, Colors::Muted, 1, Colors::Bg);

  if (accountIndex_ + 1 < totalAccounts) {
    screen.label(268, 153, "Next >", Colors::Cyan, 1, Colors::Bg);
  }
}
