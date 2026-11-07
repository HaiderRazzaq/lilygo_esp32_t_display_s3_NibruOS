#include "apps/WiFiScannerApp.h"
#include "core/AppContext.h"
#include "BoardConfig.h"
#include "core/ResourceManager.h"
#include "hardware/DisplayManager.h"
#include "ui/TextKeyboard.h"
#include <Arduino.h>
#include <cstdio>
#include <cstring>

void WiFiScannerApp::setView(WiFiView newView) {
  if (view_ != newView) {
    view_ = newView;
    viewChangeMs_ = millis();
    fullRedraw_ = true;
    listHeaderDrawn_ = false;
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
  listHeaderDrawn_ = false;
  autoScanMode_ = false;
  animPhase_ = 0;
  viewChangeMs_ = millis();
  lastKeyMs_ = 0;
  lastAnimMs_ = millis();
  lastAutoScanStartMs_ = 0;
  lastRenderedCount_ = 255;
  acquired_ = ctx.resources.acquireWiFi(ResourceOwner::WiFiScanner);
  if (acquired_) {
    ctx.wifi.startScan(millis(), false); // Standard reliable scan on enter!
  }
  lastScanState_ = ctx.wifi.state();
  lastConnState_ = ctx.wifi.connectState();
  invalidate();
}

void WiFiScannerApp::onExit(AppContext& ctx) {
  autoScanMode_ = false;
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
  listHeaderDrawn_ = false;
}

bool WiFiScannerApp::onBack(AppContext&) {
  if (view_ != WiFiView::List) {
    setView(WiFiView::List);
    return true;
  }
  autoScanMode_ = false;
  return false;
}

void WiFiScannerApp::refresh(AppContext& ctx) {
  if (!acquired_) {
    acquired_ = ctx.resources.acquireWiFi(ResourceOwner::WiFiScanner);
  }
  if (!acquired_) return;
  if (ctx.wifi.state() == ScanState::Scanning || ctx.wifi.state() == ScanState::Preparing) return;
  ctx.wifi.startScan(millis(), autoScanMode_, passiveScanMode_);
  invalidate();
}

void WiFiScannerApp::onPrimaryAction(AppContext& ctx) {
  if (view_ == WiFiView::List) {
    // Hardware BOOT button triggers instant manual fast scan
    refresh(ctx);
  } else if (view_ == WiFiView::Keyboard) {
    startConnection(ctx);
  }
}

void WiFiScannerApp::onTick(AppContext& ctx, uint32_t now) {
  if (!acquired_) return;
  ctx.wifi.tick(now);

  const ScanState state = ctx.wifi.state();
  if (state != lastScanState_) {
    lastScanState_ = state;
    invalidate();
    if (state == ScanState::Complete) {
      lastAutoScanStartMs_ = now;
    }
  }

  if (ctx.wifi.connectState() != lastConnState_) {
    lastConnState_ = ctx.wifi.connectState();
    if (lastConnState_ == ConnectState::Connected) {
      setView(WiFiView::Connected);
      return;
    } else {
      invalidate();
    }
  }

  // Continuous Live Auto-Scan trigger: starts next fast scan after 500ms pause
  if (view_ == WiFiView::List && autoScanMode_ && state == ScanState::Complete) {
    if (now - lastAutoScanStartMs_ >= 500) {
      refresh(ctx);
    }
  }

  if (view_ == WiFiView::Connecting) {
    if (now - lastAnimMs_ > 500) {
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
  if (millis() - viewChangeMs_ < 300) return;

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
    if (offset_ + 3 < ctx.wifi.count()) {
      ++offset_;
      invalidate(); // differential in-place update!
    }
    return;
  }
  if (ev.gesture == TouchGesture::SwipeDown) {
    if (offset_ > 0) {
      --offset_;
      invalidate(); // differential in-place update!
    }
    return;
  }
  if (ev.gesture == TouchGesture::Tap || ev.gesture == TouchGesture::DoubleTap) {
    // Header back zone tap (<)
    if (ev.x < 45 && ev.y < 35) {
      onBack(ctx);
      return;
    }
    // Top bar: Connected status pill
    if (ctx.wifi.connectState() == ConnectState::Connected &&
        ev.y >= 35 && ev.y <= 65 && ev.x >= 5 && ev.x <= 98) {
      setView(WiFiView::Connected);
      return;
    }
    // Mode toggle button tap [ACTIVE / PASSIVE] (x: 102..198, y: 35..65)
    if (ev.y >= 35 && ev.y <= 65 && ev.x >= 102 && ev.x <= 198) {
      passiveScanMode_ = !passiveScanMode_;
      if (ctx.wifi.state() != ScanState::Preparing && ctx.wifi.state() != ScanState::Scanning) {
        refresh(ctx);
      }
      invalidate();
      return;
    }
    // Fast Scan / Manual Refresh button tap (x: 200..315, y: 35..65)
    if (ev.y >= 35 && ev.y <= 65 && ev.x >= 200 && ev.x <= 315) {
      refresh(ctx);
      return;
    }
    // Network row taps: 3 visible rows
    for (uint8_t row = 0; row < 3; ++row) {
      const int rowY = 67 + row * 26;
      if (ev.y >= rowY && ev.y < rowY + 25 && ev.x >= 6 && ev.x <= 314) {
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
  if (now - lastKeyMs_ < 170) return;
  lastKeyMs_ = now;
  errorMsg_[0] = '\0';

  if (ev.y >= 2 && ev.y <= 24) {
    if (ev.x >= 42 && ev.x <= 90) {
      setView(WiFiView::List);
      return;
    }
    if (ev.x >= 265 && ev.x <= 318) {
      passwordLen_ = 0;
      password_[0] = '\0';
      renderPasswordBox(ctx);
      return;
    }
  }

  const auto key = TextKeyboard::hit(ev.x, ev.y, kbMode_ == KeyboardMode::Uppercase, kbMode_ == KeyboardMode::Symbols);
  if (key.action == TextKeyboard::Action::Shift) {
    kbMode_ = kbMode_ == KeyboardMode::Uppercase ? KeyboardMode::Lowercase : KeyboardMode::Uppercase;
    fullRedraw_ = true; invalidate();
  } else if (key.action == TextKeyboard::Action::Symbols) {
    kbMode_ = kbMode_ == KeyboardMode::Symbols ? KeyboardMode::Lowercase : KeyboardMode::Symbols;
    fullRedraw_ = true; invalidate();
  } else if (key.action == TextKeyboard::Action::Delete) {
    if (passwordLen_) password_[--passwordLen_] = 0;
    renderPasswordBox(ctx);
  } else if (key.action == TextKeyboard::Action::Cancel) setView(WiFiView::List);
  else if (key.action == TextKeyboard::Action::Accept) startConnection(ctx);
  else if ((key.action == TextKeyboard::Action::Character || key.action == TextKeyboard::Action::Space) &&
           passwordLen_ + 1 < sizeof(password_)) {
    password_[passwordLen_++] = key.character; password_[passwordLen_] = 0;
    renderPasswordBox(ctx);
  }
}

void WiFiScannerApp::handleConnectingTouch(AppContext& ctx, const TouchEvent& ev) {
  if (ev.gesture != TouchGesture::Tap && ev.gesture != TouchGesture::DoubleTap) return;

  const ConnectState st = ctx.wifi.connectState();
  if (st == ConnectState::Connecting) {
    if (ev.x >= 105 && ev.x <= 215 && ev.y >= 130 && ev.y <= 165) {
      ctx.wifi.disconnect();
      setView(WiFiView::List);
    }
  } else if (st == ConnectState::ConnectFailed) {
    if (ev.x >= 10 && ev.x <= 100 && ev.y >= 130 && ev.y <= 165) {
      startConnection(ctx);
      return;
    }
    if (ev.x >= 105 && ev.x <= 210 && ev.y >= 130 && ev.y <= 165) {
      setView(WiFiView::Keyboard);
      return;
    }
    if (ev.x >= 215 && ev.x <= 310 && ev.y >= 130 && ev.y <= 165) {
      setView(WiFiView::List);
      return;
    }
  }
}

void WiFiScannerApp::handleConnectedTouch(AppContext& ctx, const TouchEvent& ev) {
  if (ev.gesture != TouchGesture::Tap && ev.gesture != TouchGesture::DoubleTap) return;

  if (ev.x >= 20 && ev.x <= 150 && ev.y >= 130 && ev.y <= 165) {
    ctx.wifi.disconnect();
    setView(WiFiView::List);
    return;
  }
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

void WiFiScannerApp::renderSignalBars(AppContext& ctx, int x, int y, int16_t rssi, uint16_t sigColor) {
  TFT_eSPI& t = ctx.display.gfx();
  int level = 1;
  if (rssi >= -55) level = 4;
  else if (rssi >= -68) level = 3;
  else if (rssi >= -80) level = 2;

  for (int b = 0; b < 4; ++b) {
    const int bx = x + b * 5;
    const int bh = 4 + b * 3;
    const int by = (y + 13) - bh;
    const uint16_t col = (b < level) ? sigColor : Colors::Border;
    t.fillRect(bx, by, 3, bh, col);
  }
}

void WiFiScannerApp::renderList(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();

  // 1. Draw structural frame (Header & Footer) ONLY ONCE on entry
  if (fullRedraw_ || !listHeaderDrawn_) {
    screen.header(title(), true);
    screen.footer("Tap to connect | Swipe scroll | BOOT: Scan");
    t.fillRect(0, 34, 320, 117, Colors::Bg);
    listHeaderDrawn_ = true;
    fullRedraw_ = false;
  }

  const ScanState state = ctx.wifi.state();
  const bool isScanning = (state == ScanState::Preparing || state == ScanState::Scanning);

  // 2. Control bar (Y: 37..62) - update individual button boxes without blanking the whole bar!
  // A. Left side status pill / network count (X: 6..100)
  t.fillRect(6, 37, 94, 26, Colors::Bg);
  if (ctx.wifi.connectState() == ConnectState::Connected) {
    t.fillRoundRect(6, 37, 92, 25, 4, Colors::Green);
    screen.label(12, 45, "CONNECTED >", Colors::White, 1, Colors::Green);
  } else {
    char countBuf[20];
    snprintf(countBuf, sizeof(countBuf), "Found: %-3u", ctx.wifi.count());
    screen.label(8, 45, countBuf, Colors::White, 1, Colors::Bg);
  }

  // B. Scan Mode toggle button [ACTIVE / PASSIVE] (X: 104..196, Y: 37..62)
  if (passiveScanMode_) {
    t.fillRoundRect(104, 37, 92, 25, 4, Colors::Amber);
    screen.label(110, 45, "[PASSIVE]", Colors::Bg, 1, Colors::Amber);
  } else {
    t.fillRoundRect(104, 37, 92, 25, 4, Colors::Panel);
    t.drawRoundRect(104, 37, 92, 25, 4, Colors::Border);
    screen.label(112, 45, "[ACTIVE]", Colors::Cyan, 1, Colors::Panel);
  }

  // C. Scan / Refresh button (X: 202..314, Y: 37..62)
  if (isScanning) {
    t.fillRoundRect(202, 37, 112, 25, 5, Colors::Panel);
    t.drawRoundRect(202, 37, 112, 25, 5, Colors::Cyan);
    screen.label(216, 45, ctx.wifi.isPassiveScan() ? "BEACONS..." : "PROBING...", Colors::Cyan, 1, Colors::Panel);
  } else {
    t.fillRoundRect(202, 37, 112, 25, 5, Colors::Cyan);
    screen.label(226, 45, "RESCAN", Colors::Bg, 1, Colors::Cyan);
  }

  if (!acquired_) {
    t.fillRect(0, 66, 320, 85, Colors::Bg);
    screen.label(20, 88, "Wi-Fi Resource Busy", Colors::Red, 2, Colors::Bg);
    screen.label(20, 114, "Radio is owned by another task", Colors::Muted, 1, Colors::Bg);
    lastRenderedCount_ = 0;
    return;
  }

  // 3. Card list area (Y: 66..150)
  const uint8_t count = ctx.wifi.count();
  if (count == 0) {
    if (state != lastRenderedScanState_ || lastRenderedCount_ != 0) {
      t.fillRect(0, 66, 320, 85, Colors::Bg);
      lastRenderedScanState_ = state;
    }

    if (isScanning) {
      screen.label(25, 85, ctx.wifi.isPassiveScan() ? "Listening for Beacons..." : "Scanning Wi-Fi...", Colors::Cyan, 2, Colors::Bg);
      screen.label(25, 112, ctx.wifi.isPassiveScan() ? "Capturing raw AP beacons (1-13)" : "Listening on 2.4GHz channels (1-13)", Colors::Muted, 1, Colors::Bg);
    } else if (state == ScanState::Failed) {
      screen.label(25, 85, "Wi-Fi Scan Failed", Colors::Red, 2, Colors::Bg);
      screen.label(25, 112, ctx.wifi.lastError(), Colors::Muted, 1, Colors::Bg);
    } else {
      screen.label(25, 85, "No Networks Found", Colors::Amber, 2, Colors::Bg);
      char durBuf[56];
      snprintf(durBuf, sizeof(durBuf), "Swept ch 1-13 (%s) in %lums",
               ctx.wifi.isPassiveScan() ? "Passive" : "Active",
               static_cast<unsigned long>(ctx.wifi.lastScanDurationMs()));
      screen.label(18, 112, durBuf, Colors::Muted, 1, Colors::Bg);
    }
    lastRenderedCount_ = 0;
    return;
  }

  // If transitioning from 0 to networks, clear the status message area once!
  if (lastRenderedCount_ == 0) {
    t.fillRect(0, 66, 320, 85, Colors::Bg);
  }
  lastRenderedCount_ = count;

  // Draw exactly 3 rows in-place
  for (uint8_t row = 0; row < 3; ++row) {
    const int rowY = 67 + row * 27;
    const uint8_t idx = offset_ + row;

    if (idx < count) {
      const NetworkRecord* net = ctx.wifi.network(idx);
      if (!net) {
        t.fillRect(0, rowY - 1, 320, 27, Colors::Bg);
        continue;
      }

      // Card box
      t.fillRoundRect(4, rowY, 312, 25, 4, Colors::Panel);
      t.drawRoundRect(4, rowY, 312, 25, 4, Colors::Border);

      // SSID (max 13 chars)
      char shortSsid[16];
      snprintf(shortSsid, sizeof(shortSsid), "%.13s", net->ssid[0] ? net->ssid : "(hidden)");
      screen.label(12, rowY + 6, shortSsid, Colors::White, 1, Colors::Panel);

      // Signal color
      uint16_t sigColor = Colors::Red;
      if (net->rssi >= -55) sigColor = Colors::Green;
      else if (net->rssi >= -68) sigColor = Colors::Cyan;
      else if (net->rssi >= -80) sigColor = Colors::Amber;

      // 4-Bar Signal Meter
      renderSignalBars(ctx, 134, rowY + 5, net->rssi, sigColor);

      // Numeric RSSI (dBm)
      char rssiStr[12];
      snprintf(rssiStr, sizeof(rssiStr), "%4ddBm", net->rssi);
      screen.label(158, rowY + 6, rssiStr, sigColor, 1, Colors::Panel);

      // Channel
      char chStr[8];
      snprintf(chStr, sizeof(chStr), "c%02d", net->channel);
      screen.label(226, rowY + 6, chStr, Colors::Muted, 1, Colors::Panel);

      // Security Badge
      screen.label(262, rowY + 6, net->secured ? "[LOCK]" : "[OPEN]",
                   net->secured ? Colors::Amber : Colors::Green, 1, Colors::Panel);
    } else {
      // Overdraw unused row slot cleanly with background color
      t.fillRect(0, rowY - 1, 320, 27, Colors::Bg);
    }
  }
}

void WiFiScannerApp::renderPasswordBox(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();

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
    t.fillRect(0, 0, 320, 25, Colors::Bg);
    t.fillRect(0, 48, 320, 122, Colors::Bg);

    t.fillRoundRect(42, 2, 46, 20, 3, Colors::Panel);
    t.drawRoundRect(42, 2, 46, 20, 3, Colors::Cyan);
    screen.label(47, 8, "< LIST", Colors::Cyan, 1, Colors::Panel);

    char titleBuf[28];
    snprintf(titleBuf, sizeof(titleBuf), "AP: %.15s", selectedNet_.ssid[0] ? selectedNet_.ssid : "(open)");
    screen.label(96, 8, titleBuf, Colors::White, 1, Colors::Bg);

    t.fillRoundRect(266, 2, 50, 20, 3, Colors::Panel);
    t.drawRoundRect(266, 2, 50, 20, 3, Colors::Red);
    screen.label(274, 8, "CLEAR", Colors::Red, 1, Colors::Panel);

    TextKeyboard::draw(screen, kbMode_ == KeyboardMode::Uppercase, kbMode_ == KeyboardMode::Symbols, "JOIN");
    fullRedraw_ = false;
  }

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

  t.fillRoundRect(25, 134, 125, 26, 4, Colors::Panel);
  t.drawRoundRect(25, 134, 125, 26, 4, Colors::Red);
  screen.label(45, 142, "DISCONNECT", Colors::Red, 1, Colors::Panel);

  t.fillRoundRect(165, 134, 130, 26, 4, Colors::Panel);
  t.drawRoundRect(165, 134, 130, 26, 4, Colors::Cyan);
  screen.label(180, 142, "SCANNER LIST", Colors::Cyan, 1, Colors::Panel);
}
