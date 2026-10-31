#include "apps/WiFiScannerApp.h"
#include "core/AppContext.h"
#include "BoardConfig.h"
#include "core/ResourceManager.h"
#include "hardware/DisplayManager.h"
#include <Arduino.h>
#include <cstdio>
#include <cstring>

namespace {
char getRow0Char(int i, KeyboardMode mode) {
  static const char lower[] = "qwertyuiop";
  static const char upper[] = "QWERTYUIOP";
  static const char syms[]  = "1234567890";
  if (i < 0 || i >= 10) return '\0';
  if (mode == KeyboardMode::Uppercase) return upper[i];
  if (mode == KeyboardMode::Symbols) return syms[i];
  return lower[i];
}

char getRow1Char(int i, KeyboardMode mode) {
  static const char lower[] = "asdfghjkl";
  static const char upper[] = "ASDFGHJKL";
  static const char syms[]  = "@#$%&*-+_!";
  if (mode == KeyboardMode::Symbols) {
    if (i < 0 || i >= 10) return '\0';
    return syms[i];
  }
  if (i < 0 || i >= 9) return '\0';
  if (mode == KeyboardMode::Uppercase) return upper[i];
  return lower[i];
}

char getRow2Char(int i, KeyboardMode mode) {
  static const char lower[] = "zxcvbnm";
  static const char upper[] = "ZXCVBNM";
  static const char syms[]  = "?.,/\\:;";
  if (i < 0 || i >= 7) return '\0';
  if (mode == KeyboardMode::Uppercase) return upper[i];
  if (mode == KeyboardMode::Symbols) return syms[i];
  return lower[i];
}
}

void WiFiScannerApp::setView(WiFiView newView) {
  if (view_ != newView) {
    view_ = newView;
    viewChangeMs_ = millis();
    fullRedraw_ = true;
    invalidate();
  }
}

void WiFiScannerApp::onEnter(AppContext& ctx) {
  offset_ = 0;
  view_ = WiFiView::List;
  password_[0] = '\0';
  passwordLen_ = 0;
  errorMsg_[0] = '\0';
  fullRedraw_ = true;
  viewChangeMs_ = millis();
  lastKeyMs_ = 0;
  acquired_ = ctx.resources.acquireWiFi(ResourceOwner::WiFiScanner);
  if (acquired_) ctx.wifi.startScan(millis());
  lastScanState_ = ctx.wifi.state();
  lastConnState_ = ctx.wifi.connectState();
  lastAnimMs_ = millis();
  invalidate();
}

void WiFiScannerApp::onExit(AppContext& ctx) {
  if (acquired_) {
    if (ctx.wifi.state() == ScanState::Scanning || ctx.wifi.state() == ScanState::Preparing) {
      ctx.wifi.stopScan();
    }
    if (ctx.wifi.connectState() == ConnectState::Disconnected) {
      ctx.wifi.shutdown();
    }
    ctx.resources.releaseWiFi(ResourceOwner::WiFiScanner);
  }
  acquired_ = false;
  view_ = WiFiView::List;
  fullRedraw_ = true;
}

bool WiFiScannerApp::onBack(AppContext&) {
  if (view_ != WiFiView::List) {
    setView(WiFiView::List);
    return true;
  }
  return false;
}

void WiFiScannerApp::refresh(AppContext& ctx) {
  if (!acquired_ || ctx.wifi.state() == ScanState::Scanning ||
      ctx.wifi.state() == ScanState::Preparing) return;
  offset_ = 0;
  ctx.wifi.startScan(millis());
  invalidate();
}

void WiFiScannerApp::onPrimaryAction(AppContext& ctx) {
  if (view_ == WiFiView::List) refresh(ctx);
  else if (view_ == WiFiView::Keyboard) startConnection(ctx);
}

void WiFiScannerApp::onTick(AppContext& ctx, uint32_t now) {
  if (!acquired_) return;
  ctx.wifi.tick(now);

  if (ctx.wifi.state() != lastScanState_) {
    lastScanState_ = ctx.wifi.state();
    invalidate();
  }

  if (ctx.wifi.connectState() != lastConnState_) {
    lastConnState_ = ctx.wifi.connectState();
    if (lastConnState_ == ConnectState::Connected) {
      setView(WiFiView::Connected);
    } else {
      invalidate();
    }
  }

  // Animation ticks for scanning and connecting states
  if (view_ == WiFiView::Connecting ||
      ((view_ == WiFiView::List) &&
       (ctx.wifi.state() == ScanState::Scanning || ctx.wifi.state() == ScanState::Preparing))) {
    if (now - lastAnimMs_ > 600) {
      lastAnimMs_ = now;
      invalidate();
    }
  }
}

void WiFiScannerApp::startConnection(AppContext& ctx) {
  if (selectedNet_.secured && passwordLen_ < 8) {
    snprintf(errorMsg_, sizeof(errorMsg_), "Min 8 characters required");
    renderPasswordBox(ctx);
    return;
  }
  errorMsg_[0] = '\0';
  Serial.printf("[WiFiApp] Initiating connection to '%s', pass_len=%u\n",
                selectedNet_.ssid, static_cast<unsigned>(passwordLen_));
  setView(WiFiView::Connecting);
  ctx.wifi.connect(selectedNet_.ssid, password_, millis());
}

void WiFiScannerApp::onTouch(AppContext& ctx, const TouchEvent& ev) {
  // Prevent touch bleed-through immediately following a view switch
  if (millis() - viewChangeMs_ < 350) return;

  switch (view_) {
    case WiFiView::List:
      handleListTouch(ctx, ev);
      break;
    case WiFiView::Keyboard:
      handleKeyboardTouch(ctx, ev);
      break;
    case WiFiView::Connecting:
      handleConnectingTouch(ctx, ev);
      break;
    case WiFiView::Connected:
      handleConnectedTouch(ctx, ev);
      break;
  }
}

void WiFiScannerApp::handleListTouch(AppContext& ctx, const TouchEvent& ev) {
  if (ev.gesture == TouchGesture::SwipeUp) {
    if (offset_ + 3 < ctx.wifi.count()) { ++offset_; invalidate(); }
  } else if (ev.gesture == TouchGesture::SwipeDown) {
    if (offset_ > 0) { --offset_; invalidate(); }
  } else if (ev.gesture == TouchGesture::Tap || ev.gesture == TouchGesture::DoubleTap) {
    // Header back zone tap
    if (ev.x < 45 && ev.y < 35) {
      onBack(ctx);
      return;
    }
    // Top bar: Rescan button
    if (ev.y >= 36 && ev.y <= 64 && ev.x >= 220) {
      refresh(ctx);
      return;
    }
    // Top bar: Connected status pill
    if (ctx.wifi.connectState() == ConnectState::Connected &&
        ev.y >= 36 && ev.y <= 64 && ev.x >= 95 && ev.x < 215) {
      setView(WiFiView::Connected);
      return;
    }
    // Network row taps: 3 visible rows
    for (uint8_t row = 0; row < 3; ++row) {
      const int rowY = 67 + row * 25;
      if (ev.y >= rowY && ev.y < rowY + 24 && ev.x >= 6 && ev.x <= 314) {
        const uint8_t idx = offset_ + row;
        if (idx < ctx.wifi.count()) {
          const NetworkRecord* net = ctx.wifi.network(idx);
          if (net) {
            selectedNet_ = *net;
            if (!selectedNet_.secured) {
              password_[0] = '\0';
              passwordLen_ = 0;
              startConnection(ctx);
            } else {
              const char* savedPass = ctx.wifi.getSavedPassword(selectedNet_.ssid);
              if (savedPass && savedPass[0] != '\0') {
                strncpy(password_, savedPass, sizeof(password_) - 1);
                password_[sizeof(password_) - 1] = '\0';
                passwordLen_ = static_cast<uint8_t>(strlen(password_));
              } else {
                password_[0] = '\0';
                passwordLen_ = 0;
              }
              errorMsg_[0] = '\0';
              kbMode_ = KeyboardMode::Lowercase;
              setView(WiFiView::Keyboard);
            }
            return;
          }
        }
      }
    }
  }
}

void WiFiScannerApp::handleKeyboardTouch(AppContext& ctx, const TouchEvent& ev) {
  if (ev.gesture != TouchGesture::Tap && ev.gesture != TouchGesture::DoubleTap) return;

  const uint32_t now = millis();
  if (now - lastKeyMs_ < 170) return; // Enforce minimum spacing between keystrokes
  lastKeyMs_ = now;
  errorMsg_[0] = '\0';

  // Header row (Y: 2..24)
  if (ev.y >= 2 && ev.y <= 24) {
    if (ev.x >= 42 && ev.x <= 90) {
      // < LIST (back to list)
      setView(WiFiView::List);
      return;
    }
    if (ev.x >= 265 && ev.x <= 318) {
      // CLEAR password
      passwordLen_ = 0;
      password_[0] = '\0';
      renderPasswordBox(ctx); // Differential update
      return;
    }
  }

  // Row 0 (Y: 49..75) - 10 keys
  if (ev.y >= 49 && ev.y <= 75) {
    const int i = (ev.x - 2) / 32;
    if (i >= 0 && i < 10) {
      const char c = getRow0Char(i, kbMode_);
      if (c && passwordLen_ < sizeof(password_) - 1) {
        password_[passwordLen_++] = c;
        password_[passwordLen_] = '\0';
        renderPasswordBox(ctx); // Differential update: only redraws text box!
      }
      return;
    }
  }

  // Row 1 (Y: 78..104)
  if (ev.y >= 78 && ev.y <= 104) {
    if (kbMode_ == KeyboardMode::Symbols) {
      const int i = (ev.x - 2) / 32;
      if (i >= 0 && i < 10) {
        const char c = getRow1Char(i, kbMode_);
        if (c && passwordLen_ < sizeof(password_) - 1) {
          password_[passwordLen_++] = c;
          password_[passwordLen_] = '\0';
          renderPasswordBox(ctx);
        }
        return;
      }
    } else {
      const int i = (ev.x - 8) / 34;
      if (i >= 0 && i < 9) {
        const char c = getRow1Char(i, kbMode_);
        if (c && passwordLen_ < sizeof(password_) - 1) {
          password_[passwordLen_++] = c;
          password_[passwordLen_] = '\0';
          renderPasswordBox(ctx);
        }
        return;
      }
    }
  }

  // Row 2 (Y: 107..133)
  if (ev.y >= 107 && ev.y <= 133) {
    if (ev.x >= 2 && ev.x <= 42) {
      // Left function key: CAPS / ABC
      if (kbMode_ == KeyboardMode::Symbols) {
        kbMode_ = KeyboardMode::Lowercase;
      } else {
        kbMode_ = (kbMode_ == KeyboardMode::Lowercase) ? KeyboardMode::Uppercase : KeyboardMode::Lowercase;
      }
      fullRedraw_ = true; // Redraw keys for new case
      invalidate();
      return;
    }
    if (ev.x >= 45 && ev.x <= 262) {
      const int i = (ev.x - 45) / 31;
      if (i >= 0 && i < 7) {
        const char c = getRow2Char(i, kbMode_);
        if (c && passwordLen_ < sizeof(password_) - 1) {
          password_[passwordLen_++] = c;
          password_[passwordLen_] = '\0';
          renderPasswordBox(ctx);
        }
        return;
      }
    }
    if (ev.x >= 265 && ev.x <= 318) {
      // DEL / Backspace
      if (passwordLen_ > 0) {
        password_[--passwordLen_] = '\0';
        renderPasswordBox(ctx); // Differential update!
      }
      return;
    }
  }

  // Row 3 (Y: 136..166) - Action row
  if (ev.y >= 136 && ev.y <= 166) {
    if (ev.x >= 2 && ev.x <= 54) {
      // Mode toggle (?123 / ABC)
      kbMode_ = (kbMode_ == KeyboardMode::Symbols) ? KeyboardMode::Lowercase : KeyboardMode::Symbols;
      fullRedraw_ = true;
      invalidate();
      return;
    }
    if (ev.x >= 57 && ev.x <= 183) {
      // SPACE
      if (passwordLen_ < sizeof(password_) - 1) {
        password_[passwordLen_++] = ' ';
        password_[passwordLen_] = '\0';
        renderPasswordBox(ctx);
      }
      return;
    }
    if (ev.x >= 186 && ev.x <= 240) {
      // CANCEL
      setView(WiFiView::List);
      return;
    }
    if (ev.x >= 243 && ev.x <= 318) {
      // JOIN / Connect
      startConnection(ctx);
      return;
    }
  }
}

void WiFiScannerApp::handleConnectingTouch(AppContext& ctx, const TouchEvent& ev) {
  if (ev.gesture != TouchGesture::Tap && ev.gesture != TouchGesture::DoubleTap) return;

  const ConnectState st = ctx.wifi.connectState();
  if (st == ConnectState::Connecting) {
    // CANCEL button at (110, 134, 100, 26)
    if (ev.x >= 105 && ev.x <= 215 && ev.y >= 130 && ev.y <= 165) {
      ctx.wifi.disconnect();
      setView(WiFiView::List);
    }
  } else if (st == ConnectState::ConnectFailed) {
    // RETRY button at (15, 134, 85, 26)
    if (ev.x >= 10 && ev.x <= 100 && ev.y >= 130 && ev.y <= 165) {
      startConnection(ctx);
      return;
    }
    // EDIT PASS button at (110, 134, 100, 26)
    if (ev.x >= 105 && ev.x <= 210 && ev.y >= 130 && ev.y <= 165) {
      setView(WiFiView::Keyboard);
      return;
    }
    // BACK button at (220, 134, 85, 26)
    if (ev.x >= 215 && ev.x <= 310 && ev.y >= 130 && ev.y <= 165) {
      setView(WiFiView::List);
      return;
    }
  }
}

void WiFiScannerApp::handleConnectedTouch(AppContext& ctx, const TouchEvent& ev) {
  if (ev.gesture != TouchGesture::Tap && ev.gesture != TouchGesture::DoubleTap) return;

  // DISCONNECT button at (25, 134, 125, 26)
  if (ev.x >= 20 && ev.x <= 150 && ev.y >= 130 && ev.y <= 165) {
    ctx.wifi.disconnect();
    setView(WiFiView::List);
    return;
  }
  // SCANNER LIST button at (165, 134, 130, 26)
  if (ev.x >= 160 && ev.x <= 300 && ev.y >= 130 && ev.y <= 165) {
    setView(WiFiView::List);
    return;
  }
}

void WiFiScannerApp::render(AppContext& ctx) {
  switch (view_) {
    case WiFiView::List:
      renderList(ctx);
      break;
    case WiFiView::Keyboard:
      renderKeyboard(ctx);
      break;
    case WiFiView::Connecting:
      renderConnecting(ctx);
      break;
    case WiFiView::Connected:
      renderConnected(ctx);
      break;
  }
}

void WiFiScannerApp::renderList(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();

  if (fullRedraw_) {
    screen.clear();
    screen.header(title(), true);
    fullRedraw_ = false;
  }

  if (ctx.wifi.connectState() == ConnectState::Connected) {
    t.fillRoundRect(92, 37, 122, 25, 4, Colors::Green);
    screen.label(98, 45, "CONNECTED >", Colors::White, 1, Colors::Green);
  }

  t.fillRoundRect(223, 37, 90, 25, 5, Colors::Cyan);
  screen.label(242, 45, "RESCAN", Colors::Bg, 1, Colors::Cyan);

  if (!acquired_) {
    screen.label(10, 76, "Wi-Fi resource busy", Colors::Red, 2);
    screen.footer("GPIO14 = Home");
    return;
  }

  const ScanState state = ctx.wifi.state();
  if (state == ScanState::Preparing) {
    t.fillRect(10, 75, 300, 50, Colors::Bg);
    char line[42];
    snprintf(line, sizeof(line), "Starting radio... %u/%u",
             ctx.wifi.attempt(), Board::kScanMaxAttempts);
    screen.label(12, 78, line, Colors::Cyan, 2, Colors::Bg);
    screen.label(12, 110, "Preparing Wi-Fi station", Colors::Muted, 1, Colors::Bg);
  } else if (state == ScanState::Scanning) {
    t.fillRect(10, 75, 300, 50, Colors::Bg);
    char line[42];
    snprintf(line, sizeof(line), "Scanning %u/%u... %lu s",
             ctx.wifi.attempt(), Board::kScanMaxAttempts,
             static_cast<unsigned long>((millis() - ctx.wifi.scanStartedAt()) / 1000UL));
    screen.label(12, 78, line, Colors::Cyan, 2, Colors::Bg);
    screen.label(12, 112, "UI remains responsive", Colors::Muted, 1, Colors::Bg);
  } else if (state == ScanState::Failed) {
    t.fillRect(10, 75, 300, 50, Colors::Bg);
    screen.label(12, 77, "Wi-Fi scan failed", Colors::Red, 2, Colors::Bg);
    screen.label(12, 105, ctx.wifi.lastError(), Colors::Muted, 1, Colors::Bg);
    screen.label(12, 124, "RESCAN | See Serial 115200", Colors::Muted, 1, Colors::Bg);
  } else if (state == ScanState::Complete) {
    char countText[32];
    snprintf(countText, sizeof(countText), "Found: %u", ctx.wifi.count());
    screen.label(9, 45, countText, Colors::Muted, 1, Colors::Bg);
    if (ctx.wifi.count() == 0) {
      t.fillRect(10, 75, 300, 50, Colors::Bg);
      screen.label(12, 89, "No networks found", Colors::Amber, 2, Colors::Bg);
    } else {
      for (uint8_t row = 0; row < 3; ++row) {
        const NetworkRecord* net = ctx.wifi.network(offset_ + row);
        const int y = 69 + row * 25;
        if (!net) {
          t.fillRect(6, y - 2, 308, 23, Colors::Bg);
          continue;
        }
        t.fillRoundRect(6, y - 2, 308, 23, 3, Colors::Panel);
        t.drawRoundRect(6, y - 2, 308, 23, 3, Colors::Border);

        char shortSsid[20];
        snprintf(shortSsid, sizeof(shortSsid), "%.16s", net->ssid[0] ? net->ssid : "(hidden)");
        screen.label(12, y + 2, shortSsid, Colors::White, 1, Colors::Panel);

        char rssiStr[16];
        snprintf(rssiStr, sizeof(rssiStr), "%4d dBm", net->rssi);
        screen.label(152, y + 2, rssiStr, Colors::Cyan, 1, Colors::Panel);

        screen.label(236, y + 2, net->secured ? "[Sec] TAP" : "[Opn] TAP",
                     net->secured ? Colors::Amber : Colors::Green, 1, Colors::Panel);
      }
    }
  } else {
    screen.label(10, 80, "Tap RESCAN to start", Colors::Muted, 1, Colors::Bg);
  }
  screen.footer("Tap net to connect | GPIO14 Back");
}

void WiFiScannerApp::renderPasswordBox(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();

  // Redraw ONLY the password panel without touching keys or clearing the screen
  t.fillRoundRect(4, 25, 312, 21, 3, Colors::Panel);
  t.drawRoundRect(4, 25, 312, 21, 3, errorMsg_[0] ? Colors::Red : Colors::Border);

  if (errorMsg_[0] != '\0') {
    screen.label(10, 31, errorMsg_, Colors::Red, 1, Colors::Panel);
  } else if (passwordLen_ == 0) {
    screen.label(10, 31, "Password (visible):", Colors::Muted, 1, Colors::Panel);
    screen.label(135, 31, "_", Colors::Cyan, 1, Colors::Panel);
  } else {
    char disp[40];
    if (passwordLen_ <= 28) {
      snprintf(disp, sizeof(disp), "%s_", password_);
    } else {
      snprintf(disp, sizeof(disp), "...%s_", password_ + (passwordLen_ - 25));
    }
    screen.label(10, 31, disp, Colors::White, 1, Colors::Panel);
  }
}

void WiFiScannerApp::renderKeyboard(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();

  if (fullRedraw_) {
    // Clear keyboard area without a jarring full-screen blanking
    t.fillRect(0, 0, 320, 25, Colors::Bg);
    t.fillRect(0, 48, 320, 122, Colors::Bg);

    // 1. Header bar
    t.fillRoundRect(42, 2, 46, 20, 3, Colors::Panel);
    t.drawRoundRect(42, 2, 46, 20, 3, Colors::Cyan);
    screen.label(47, 8, "< LIST", Colors::Cyan, 1, Colors::Panel);

    char titleBuf[28];
    snprintf(titleBuf, sizeof(titleBuf), "AP: %.15s", selectedNet_.ssid[0] ? selectedNet_.ssid : "(open)");
    screen.label(96, 8, titleBuf, Colors::White, 1, Colors::Bg);

    t.fillRoundRect(266, 2, 50, 20, 3, Colors::Panel);
    t.drawRoundRect(266, 2, 50, 20, 3, Colors::Red);
    screen.label(274, 8, "CLEAR", Colors::Red, 1, Colors::Panel);

    // 2. Virtual Keyboard keys
    // Row 0 (Y: 49..75)
    for (int i = 0; i < 10; ++i) {
      const int kx = 2 + i * 32;
      t.fillRoundRect(kx, 49, 30, 26, 3, Colors::Panel);
      t.drawRoundRect(kx, 49, 30, 26, 3, Colors::Border);
      const char c = getRow0Char(i, kbMode_);
      char chStr[2] = { c, '\0' };
      screen.label(kx + 12, 58, chStr, Colors::White, 1, Colors::Panel);
    }

    // Row 1 (Y: 78..104)
    if (kbMode_ == KeyboardMode::Symbols) {
      for (int i = 0; i < 10; ++i) {
        const int kx = 2 + i * 32;
        t.fillRoundRect(kx, 78, 30, 26, 3, Colors::Panel);
        t.drawRoundRect(kx, 78, 30, 26, 3, Colors::Border);
        const char c = getRow1Char(i, kbMode_);
        char chStr[2] = { c, '\0' };
        screen.label(kx + 12, 87, chStr, Colors::White, 1, Colors::Panel);
      }
    } else {
      for (int i = 0; i < 9; ++i) {
        const int kx = 8 + i * 34;
        t.fillRoundRect(kx, 78, 32, 26, 3, Colors::Panel);
        t.drawRoundRect(kx, 78, 32, 26, 3, Colors::Border);
        const char c = getRow1Char(i, kbMode_);
        char chStr[2] = { c, '\0' };
        screen.label(kx + 13, 87, chStr, Colors::White, 1, Colors::Panel);
      }
    }

    // Row 2 (Y: 107..133)
    const uint16_t capsBg = (kbMode_ == KeyboardMode::Uppercase) ? Colors::Cyan : Colors::Panel;
    const uint16_t capsFg = (kbMode_ == KeyboardMode::Uppercase) ? Colors::Bg : Colors::Cyan;
    t.fillRoundRect(2, 107, 40, 26, 3, capsBg);
    t.drawRoundRect(2, 107, 40, 26, 3, Colors::Border);
    screen.label(6, 116, (kbMode_ == KeyboardMode::Symbols) ? "ABC" : "CAPS", capsFg, 1, capsBg);

    for (int i = 0; i < 7; ++i) {
      const int kx = 45 + i * 31;
      t.fillRoundRect(kx, 107, 29, 26, 3, Colors::Panel);
      t.drawRoundRect(kx, 107, 29, 26, 3, Colors::Border);
      const char c = getRow2Char(i, kbMode_);
      char chStr[2] = { c, '\0' };
      screen.label(kx + 11, 116, chStr, Colors::White, 1, Colors::Panel);
    }

    t.fillRoundRect(265, 107, 53, 26, 3, Colors::Panel);
    t.drawRoundRect(265, 107, 53, 26, 3, Colors::Red);
    screen.label(277, 116, "DEL", Colors::Red, 1, Colors::Panel);

    // Row 3 (Y: 136..166) - Action row
    t.fillRoundRect(2, 136, 52, 30, 3, Colors::Panel);
    t.drawRoundRect(2, 136, 52, 30, 3, Colors::Cyan);
    screen.label(10, 147, (kbMode_ == KeyboardMode::Symbols) ? "ABC" : "?123", Colors::Cyan, 1, Colors::Panel);

    t.fillRoundRect(57, 136, 126, 30, 3, Colors::Panel);
    t.drawRoundRect(57, 136, 126, 30, 3, Colors::Border);
    screen.label(98, 147, "SPACE", Colors::Muted, 1, Colors::Panel);

    t.fillRoundRect(186, 136, 54, 30, 3, Colors::Panel);
    t.drawRoundRect(186, 136, 54, 30, 3, Colors::Border);
    screen.label(192, 147, "CANCEL", Colors::Muted, 1, Colors::Panel);

    t.fillRoundRect(243, 136, 75, 30, 3, Colors::Green);
    screen.label(265, 147, "JOIN", Colors::White, 1, Colors::Green);

    fullRedraw_ = false;
  }

  // Always render the visible password box differentials cleanly
  renderPasswordBox(ctx);
}

void WiFiScannerApp::renderConnecting(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();

  if (fullRedraw_) {
    screen.clear();
    screen.header("Wi-Fi Connection", false);

    char buf[64];
    snprintf(buf, sizeof(buf), "Target: %s", selectedNet_.ssid);
    screen.label(12, 36, buf, Colors::White, 1, Colors::Bg);

    snprintf(buf, sizeof(buf), "Pass:   %s", password_[0] ? password_ : "(none)");
    screen.label(12, 54, buf, Colors::Muted, 1, Colors::Bg);

    fullRedraw_ = false;
  }

  const ConnectState st = ctx.wifi.connectState();
  char buf[64];

  if (st == ConnectState::Connecting) {
    t.fillRect(10, 72, 300, 58, Colors::Bg);
    screen.label(12, 74, "Connecting to AP...", Colors::Cyan, 2, Colors::Bg);
    const uint32_t curMs = millis();
    const unsigned long elapsed = (curMs >= ctx.wifi.connectStartedAt()) ?
        ((curMs - ctx.wifi.connectStartedAt()) / 1000UL) : 0;
    snprintf(buf, sizeof(buf), "Elapsed: %lu s / %u s", elapsed,
             static_cast<unsigned>(Board::kConnectTimeoutMs / 1000U));
    screen.label(12, 98, buf, Colors::White, 1, Colors::Bg);

    snprintf(buf, sizeof(buf), "Status:  %s", ctx.wifi.statusName());
    screen.label(12, 114, buf, Colors::Muted, 1, Colors::Bg);

    t.fillRoundRect(110, 134, 100, 26, 4, Colors::Panel);
    t.drawRoundRect(110, 134, 100, 26, 4, Colors::Red);
    screen.label(135, 142, "CANCEL", Colors::Red, 1, Colors::Panel);
  } else if (st == ConnectState::ConnectFailed) {
    t.fillRect(10, 72, 300, 58, Colors::Bg);
    screen.label(12, 74, "Connection Failed", Colors::Red, 2, Colors::Bg);

    snprintf(buf, sizeof(buf), "Reason:  %s", ctx.wifi.connectError());
    screen.label(12, 98, buf, Colors::Amber, 1, Colors::Bg);

    if (strstr(ctx.wifi.connectError(), "Wrong") || strstr(ctx.wifi.connectError(), "14") ||
        strstr(ctx.wifi.connectError(), "202")) {
      screen.label(12, 114, "Check password / WPA key", Colors::White, 1, Colors::Bg);
    } else if (strstr(ctx.wifi.connectError(), "201")) {
      screen.label(12, 114, "AP missing; check 2.4GHz", Colors::White, 1, Colors::Bg);
    } else {
      screen.label(12, 114, "Check router auth / signal", Colors::White, 1, Colors::Bg);
    }

    t.fillRoundRect(15, 134, 85, 26, 4, Colors::Panel);
    t.drawRoundRect(15, 134, 85, 26, 4, Colors::Cyan);
    screen.label(35, 142, "RETRY", Colors::Cyan, 1, Colors::Panel);

    t.fillRoundRect(110, 134, 100, 26, 4, Colors::Panel);
    t.drawRoundRect(110, 134, 100, 26, 4, Colors::Amber);
    screen.label(125, 142, "EDIT PASS", Colors::Amber, 1, Colors::Panel);

    t.fillRoundRect(220, 134, 85, 26, 4, Colors::Panel);
    t.drawRoundRect(220, 134, 85, 26, 4, Colors::Border);
    screen.label(245, 142, "BACK", Colors::White, 1, Colors::Panel);
  }
}

void WiFiScannerApp::renderConnected(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();

  if (fullRedraw_) {
    screen.clear();
    screen.header("Wi-Fi Connected", false);
    fullRedraw_ = false;
  }

  char buf[48];
  snprintf(buf, sizeof(buf), "SSID:    %s", ctx.wifi.connectedSSID());
  screen.label(12, 40, buf, Colors::Green, 2, Colors::Bg);

  char ipBuf[24];
  ctx.wifi.getLocalIP(ipBuf, sizeof(ipBuf));
  snprintf(buf, sizeof(buf), "IP:      %s", ipBuf[0] ? ipBuf : "N/A");
  screen.label(12, 68, buf, Colors::White, 1, Colors::Bg);

  char gwBuf[24];
  ctx.wifi.getGatewayIP(gwBuf, sizeof(gwBuf));
  snprintf(buf, sizeof(buf), "Gateway: %s", gwBuf[0] ? gwBuf : "N/A");
  screen.label(12, 86, buf, Colors::White, 1, Colors::Bg);

  snprintf(buf, sizeof(buf), "Signal:  %d dBm  |  Ch: %d",
           ctx.wifi.currentRSSI(), ctx.wifi.currentChannel());
  screen.label(12, 104, buf, Colors::Cyan, 1, Colors::Bg);

  // Action buttons
  t.fillRoundRect(25, 134, 125, 26, 4, Colors::Panel);
  t.drawRoundRect(25, 134, 125, 26, 4, Colors::Red);
  screen.label(45, 142, "DISCONNECT", Colors::Red, 1, Colors::Panel);

  t.fillRoundRect(165, 134, 130, 26, 4, Colors::Panel);
  t.drawRoundRect(165, 134, 130, 26, 4, Colors::Cyan);
  screen.label(180, 142, "SCANNER LIST", Colors::Cyan, 1, Colors::Panel);
}
