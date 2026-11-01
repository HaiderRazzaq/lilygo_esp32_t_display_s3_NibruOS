#include "apps/SettingsApp.h"
#include "core/AppContext.h"
#include "core/ResourceManager.h"
#include "core/MemoryProfiler.h"
#include "hardware/DisplayManager.h"
#include "hardware/TouchManager.h"
#include "services/SettingsService.h"
#include "services/WiFiService.h"
#include "services/SecurityService.h"
#include "services/CryptoService.h"
#include "BoardConfig.h"
#include <Arduino.h>
#include <cstdio>
#include <cstring>

void SettingsApp::onEnter(AppContext& ctx) {
  subView_ = SettingsSubView::Menu;
  focusedIndex_ = 0;
  confirmAction_ = ConfirmAction::None;
  fullRedraw_ = true;
  lastMetricTickMs_ = millis();
  pinInputLen_ = 0;
  pinInputBuffer_[0] = '\0';
  pinError_ = false;
  invalidate();
}

void SettingsApp::onExit(AppContext& ctx) {
  if (subView_ == SettingsSubView::TouchTest) {
    touchTest_.onExit(ctx);
  }
  CryptoService::secureZero(pinInputBuffer_, sizeof(pinInputBuffer_));
  pinInputLen_ = 0;
  subView_ = SettingsSubView::Menu;
}

void SettingsApp::onTick(AppContext& ctx, uint32_t nowMs) {
  if (subView_ == SettingsSubView::TouchTest) {
    touchTest_.onTick(ctx, nowMs);
    if (touchTest_.consumeDirty()) touchTest_.render(ctx);
  } else if (subView_ == SettingsSubView::Memory || subView_ == SettingsSubView::WiFiDiag) {
    if (nowMs - lastMetricTickMs_ >= 1000) {
      lastMetricTickMs_ = nowMs;
      invalidate();
    }
  }
}

bool SettingsApp::onBack(AppContext& ctx) {
  if (confirmAction_ != ConfirmAction::None) {
    confirmAction_ = ConfirmAction::None;
    invalidate();
    return true;
  }
  if (subView_ == SettingsSubView::SetPin) {
    CryptoService::secureZero(pinInputBuffer_, sizeof(pinInputBuffer_));
    pinInputLen_ = 0;
    subView_ = SettingsSubView::Security;
    fullRedraw_ = true;
    invalidate();
    return true;
  }
  if (subView_ != SettingsSubView::Menu) {
    if (subView_ == SettingsSubView::TouchTest) {
      touchTest_.onExit(ctx);
    }
    subView_ = SettingsSubView::Menu;
    fullRedraw_ = true;
    invalidate();
    return true;
  }
  return false; // Shell goes Home
}

void SettingsApp::onPrimaryAction(AppContext& ctx) {
  if (subView_ == SettingsSubView::Menu) {
    // BOOT button cycles focus
    focusedIndex_ = static_cast<uint8_t>((focusedIndex_ + 1) % 6);
    invalidate();
  } else if (subView_ == SettingsSubView::TouchTest) {
    touchTest_.onPrimaryAction(ctx);
  }
}

void SettingsApp::onTouch(AppContext& ctx, const TouchEvent& ev) {
  // Global header back zone
  if ((ev.gesture == TouchGesture::Tap || ev.gesture == TouchGesture::DoubleTap) &&
      ev.x < 45 && ev.y < 35) {
    onBack(ctx);
    return;
  }

  switch (subView_) {
    case SettingsSubView::Menu:
      handleMenuTouch(ctx, ev);
      break;
    case SettingsSubView::Display:
      handleDisplayTouch(ctx, ev);
      break;
    case SettingsSubView::Theme:
      handleThemeTouch(ctx, ev);
      break;
    case SettingsSubView::Memory:
      handleMemoryTouch(ctx, ev);
      break;
    case SettingsSubView::Security:
      handleSecurityTouch(ctx, ev);
      break;
    case SettingsSubView::WiFiDiag:
      handleWiFiDiagTouch(ctx, ev);
      break;
    case SettingsSubView::Device:
      handleDeviceTouch(ctx, ev);
      break;
    case SettingsSubView::TouchTest:
      touchTest_.onTouch(ctx, ev);
      break;
    case SettingsSubView::SetPin:
      handleSetPinTouch(ctx, ev);
      break;
  }
}

void SettingsApp::handleMenuTouch(AppContext& ctx, const TouchEvent& ev) {
  if (ev.gesture != TouchGesture::Tap && ev.gesture != TouchGesture::DoubleTap) return;

  const int col = (ev.x >= 164) ? 1 : 0;
  if (ev.x < 10 || ev.x > 310) return;

  int row = -1;
  if (ev.y >= 36 && ev.y <= 68) row = 0;
  else if (ev.y >= 74 && ev.y <= 106) row = 1;
  else if (ev.y >= 112 && ev.y <= 144) row = 2;
  if (row < 0) return;

  const uint8_t idx = static_cast<uint8_t>(col * 3 + row);
  focusedIndex_ = idx;

  switch (idx) {
    case 0: subView_ = SettingsSubView::Display; break;
    case 1: subView_ = SettingsSubView::Theme; break;
    case 2: subView_ = SettingsSubView::Memory; fullRedraw_ = true; break;
    case 3: subView_ = SettingsSubView::Security; fullRedraw_ = true; break;
    case 4: subView_ = SettingsSubView::WiFiDiag; fullRedraw_ = true; break;
    case 5: subView_ = SettingsSubView::Device; confirmAction_ = ConfirmAction::None; break;
  }
  invalidate();
}

void SettingsApp::handleDisplayTouch(AppContext& ctx, const TouchEvent& ev) {
  if (ev.gesture != TouchGesture::Tap && ev.gesture != TouchGesture::DoubleTap) return;

  // Brightness row (Y: 48..70)
  if (ev.y >= 48 && ev.y <= 70) {
    if (ev.x >= 14 && ev.x <= 80) {
      ctx.settings.setBrightness(25);
      invalidate();
      return;
    }
    if (ev.x >= 88 && ev.x <= 154) {
      ctx.settings.setBrightness(50);
      invalidate();
      return;
    }
    if (ev.x >= 162 && ev.x <= 228) {
      ctx.settings.setBrightness(75);
      invalidate();
      return;
    }
    if (ev.x >= 236 && ev.x <= 302) {
      ctx.settings.setBrightness(100);
      invalidate();
      return;
    }
  }

  // Timeout row (Y: 88..110): 30s, 1m, 3m, 5m, Never
  if (ev.y >= 88 && ev.y <= 110) {
    if (ev.x >= 10 && ev.x <= 64) {
      ctx.settings.setScreenTimeoutSec(30);
      invalidate();
      return;
    }
    if (ev.x >= 70 && ev.x <= 124) {
      ctx.settings.setScreenTimeoutSec(60);
      invalidate();
      return;
    }
    if (ev.x >= 130 && ev.x <= 184) {
      ctx.settings.setScreenTimeoutSec(180);
      invalidate();
      return;
    }
    if (ev.x >= 190 && ev.x <= 244) {
      ctx.settings.setScreenTimeoutSec(300);
      invalidate();
      return;
    }
    if (ev.x >= 250 && ev.x <= 308) {
      ctx.settings.setScreenTimeoutSec(0); // Never
      invalidate();
      return;
    }
  }

  // Sleep now button (Y: 118..144, X: 60..260)
  if (ev.y >= 118 && ev.y <= 144 && ev.x >= 60 && ev.x <= 260) {
    ctx.display.sleep();
    return;
  }
}

void SettingsApp::handleThemeTouch(AppContext& ctx, const TouchEvent& ev) {
  if (ev.gesture != TouchGesture::Tap && ev.gesture != TouchGesture::DoubleTap) return;

  // 4 Theme cards: Row 0 (Y: 48..82), Row 1 (Y: 92..126)
  if (ev.y >= 48 && ev.y <= 82) {
    if (ev.x >= 14 && ev.x <= 154) {
      ctx.settings.setTheme(0); // Cyber Cyan
      invalidate();
      return;
    }
    if (ev.x >= 164 && ev.x <= 304) {
      ctx.settings.setTheme(1); // Emerald Green
      invalidate();
      return;
    }
  } else if (ev.y >= 92 && ev.y <= 126) {
    if (ev.x >= 14 && ev.x <= 154) {
      ctx.settings.setTheme(2); // Amber Gold
      invalidate();
      return;
    }
    if (ev.x >= 164 && ev.x <= 304) {
      ctx.settings.setTheme(3); // Neon Magenta
      invalidate();
      return;
    }
  }
}

void SettingsApp::handleMemoryTouch(AppContext&, const TouchEvent&) {}

void SettingsApp::handleWiFiDiagTouch(AppContext&, const TouchEvent&) {}

void SettingsApp::handleDeviceTouch(AppContext& ctx, const TouchEvent& ev) {
  if (ev.gesture != TouchGesture::Tap && ev.gesture != TouchGesture::DoubleTap) return;

  if (confirmAction_ == ConfirmAction::None) {
    // Card 1: Reboot (Y: 46..84, X: 16..304)
    if (ev.y >= 46 && ev.y <= 84 && ev.x >= 16 && ev.x <= 304) {
      confirmAction_ = ConfirmAction::Reboot;
      invalidate();
      return;
    }
    // Card 2: Reset Settings (Y: 96..134, X: 16..304)
    if (ev.y >= 96 && ev.y <= 134 && ev.x >= 16 && ev.x <= 304) {
      confirmAction_ = ConfirmAction::ResetSettings;
      invalidate();
      return;
    }
  } else {
    // Modal buttons: Cancel (X: 35..135, Y: 100..128), Confirm (X: 185..285, Y: 100..128)
    if (ev.y >= 100 && ev.y <= 128) {
      if (ev.x >= 35 && ev.x <= 135) {
        confirmAction_ = ConfirmAction::None;
        invalidate();
        return;
      }
      if (ev.x >= 185 && ev.x <= 285) {
        if (confirmAction_ == ConfirmAction::Reboot) {
          ctx.display.clear(Colors::Bg);
          ctx.display.label(100, 75, "Restarting NibruOS...", Colors::Cyan, 1, Colors::Bg);
          Serial.println("[System] Reboot confirmed by user. Restarting...");
          delay(200);
          ESP.restart();
        } else if (confirmAction_ == ConfirmAction::ResetSettings) {
          ctx.settings.resetToDefaults();
          confirmAction_ = ConfirmAction::None;
          invalidate();
          return;
        }
      }
    }
  }
}

void SettingsApp::render(AppContext& ctx) {
  switch (subView_) {
    case SettingsSubView::Menu:      renderMenu(ctx); break;
    case SettingsSubView::Display:   renderDisplay(ctx); break;
    case SettingsSubView::Theme:     renderTheme(ctx); break;
    case SettingsSubView::Memory:    renderMemory(ctx); break;
    case SettingsSubView::Security:  renderSecurity(ctx); break;
    case SettingsSubView::WiFiDiag:  renderWiFiDiag(ctx); break;
    case SettingsSubView::Device:    renderDevice(ctx); break;
    case SettingsSubView::TouchTest: touchTest_.render(ctx); break;
    case SettingsSubView::SetPin:    renderSetPin(ctx); break;
  }
}

void SettingsApp::renderMenu(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();
  screen.clear();
  screen.header("Settings", true);

  struct MenuItem {
    const char* tag;
    const char* title;
    const char* sub;
    uint16_t color;
    int col;
    int row;
  };

  const MenuItem items[6] = {
    {"DISP", "Display", "Backlight & Sleep", Colors::Cyan, 0, 0},
    {"THM",  "Theme",   ctx.settings.themeName(), Colors::Green, 0, 1},
    {"MEM",  "Memory",  "ESP32-S3 & Heap", Colors::Amber, 0, 2},
    {"SEC",  "Security", ctx.security.isPinEnabled() ? "PIN Protected" : "PIN Disabled",
                         ctx.security.isPinEnabled() ? Colors::Green : Colors::Amber, 1, 0},
    {"WIFI", "Wi-Fi Diag", "Radio & Scanner", Colors::Cyan, 1, 1},
    {"DEV",  "Device",  "Reboot & Reset", Colors::Red, 1, 2}
  };

  for (uint8_t i = 0; i < 6; ++i) {
    const int x = (items[i].col == 0) ? 10 : 164;
    const int y = 36 + items[i].row * 38;
    const bool isFocused = (focusedIndex_ == i);

    t.fillRoundRect(x, y, 146, 33, 5, Colors::Panel);
    t.drawRoundRect(x, y, 146, 33, 5, isFocused ? ctx.settings.themeColor() : Colors::Border);

    // Tag badge
    t.fillRoundRect(x + 4, y + 5, 34, 23, 3, items[i].color);
    screen.label(x + 7, y + 12, items[i].tag, Colors::Bg, 1, items[i].color);

    // Title and sub
    screen.label(x + 42, y + 6, items[i].title, Colors::White, 1, Colors::Panel);
    screen.label(x + 42, y + 18, items[i].sub, Colors::Muted, 1, Colors::Panel);
  }

  screen.footer("Tap card | BOOT = cycle focus");
}

void SettingsApp::renderDisplay(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();
  screen.clear();
  screen.header("Display & Screen", true);

  // Section 1: Brightness
  char bStr[32];
  snprintf(bStr, sizeof(bStr), "BRIGHTNESS: %u%%", ctx.settings.brightness());
  screen.label(14, 36, bStr, Colors::Cyan, 1, Colors::Bg);

  const uint8_t brightVals[4] = {25, 50, 75, 100};
  const char* brightLabels[4] = {"25%", "50%", "75%", "100%"};
  const int bX[4] = {14, 88, 162, 236};
  const uint8_t currentB = ctx.settings.brightness();

  for (int i = 0; i < 4; ++i) {
    const bool active = (currentB == brightVals[i]);
    const uint16_t bg = active ? Colors::Cyan : Colors::Panel;
    const uint16_t tc = active ? Colors::Bg : Colors::White;
    t.fillRoundRect(bX[i], 48, 66, 22, 4, bg);
    t.drawRoundRect(bX[i], 48, 66, 22, 4, active ? Colors::White : Colors::Border);
    const int tx = bX[i] + (66 - static_cast<int>(strlen(brightLabels[i])) * 6) / 2;
    screen.label(tx, 55, brightLabels[i], tc, 1, bg);
  }

  // Section 2: Screen Timeout (A4: 30s, 1m, 3m, 5m, Never)
  char sStr[48];
  const uint16_t sec = ctx.settings.screenTimeoutSec();
  if (sec == 0) {
    snprintf(sStr, sizeof(sStr), "SCREEN TIMEOUT: Never (Always On)");
  } else if (sec >= 60) {
    snprintf(sStr, sizeof(sStr), "SCREEN TIMEOUT: %u min", sec / 60);
  } else {
    snprintf(sStr, sizeof(sStr), "SCREEN TIMEOUT: %u sec", sec);
  }
  screen.label(14, 75, sStr, Colors::Green, 1, Colors::Bg);

  const uint16_t sleepVals[5] = {30, 60, 180, 300, 0};
  const char* sleepLabels[5] = {"30s", "1m", "3m", "5m", "Off"};
  const int sX[5] = {10, 70, 130, 190, 250};

  for (int i = 0; i < 5; ++i) {
    const bool active = (sec == sleepVals[i]);
    const uint16_t bg = active ? Colors::Green : Colors::Panel;
    const uint16_t tc = active ? Colors::Bg : Colors::White;
    t.fillRoundRect(sX[i], 88, 54, 22, 4, bg);
    t.drawRoundRect(sX[i], 88, 54, 22, 4, active ? Colors::White : Colors::Border);
    const int tx = sX[i] + (54 - static_cast<int>(strlen(sleepLabels[i])) * 6) / 2;
    screen.label(tx, 95, sleepLabels[i], tc, 1, bg);
  }

  // Sleep now test button
  t.fillRoundRect(60, 118, 200, 26, 6, Colors::Panel);
  t.drawRoundRect(60, 118, 200, 26, 6, Colors::Amber);
  screen.label(78, 126, "[ TEST SLEEP MODE NOW ]", Colors::Amber, 1, Colors::Panel);

  screen.footer("Tap option to apply | Touch to wake");
}

void SettingsApp::renderTheme(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();
  screen.clear();
  screen.header("Theme Accent", true);

  screen.label(14, 36, "SELECT SYSTEM ACCENT COLOR:", Colors::White, 1, Colors::Bg);

  const char* names[4] = {"Cyber Cyan", "Emerald Green", "Amber Gold", "Neon Magenta"};
  const uint16_t colors[4] = {Colors::Cyan, Colors::Green, Colors::Amber, Colors::Magenta};
  const uint8_t curTheme = ctx.settings.theme();

  for (uint8_t i = 0; i < 4; ++i) {
    const int x = (i % 2 == 0) ? 14 : 164;
    const int y = (i < 2) ? 50 : 92;
    const bool active = (curTheme == i);

    t.fillRoundRect(x, y, 142, 34, 5, Colors::Panel);
    t.drawRoundRect(x, y, 142, 34, 5, active ? colors[i] : Colors::Border);

    // Color swatch circle
    t.fillCircle(x + 18, y + 17, 8, colors[i]);
    if (active) {
      t.drawCircle(x + 18, y + 17, 10, Colors::White);
    }

    screen.label(x + 36, y + 13, names[i], active ? colors[i] : Colors::White, 1, Colors::Panel);
  }

  screen.footer("Theme accent applies across NibruOS");
}

void SettingsApp::renderMemory(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();
  char line[64];

  MemorySnapshot mem = MemoryProfiler::capture();

  if (fullRedraw_) {
    screen.clear();
    screen.header("Memory Monitor", true);

    // Left info card
    t.fillRoundRect(10, 36, 146, 108, 6, Colors::Panel);
    t.drawRoundRect(10, 36, 146, 108, 6, Colors::Border);
    screen.label(16, 42, "DEVICE & SYSTEM", Colors::Cyan, 1, Colors::Panel);

    snprintf(line, sizeof(line), "Ver:   %s", Board::kVersion);
    screen.label(16, 56, line, Colors::White, 1, Colors::Panel);

    snprintf(line, sizeof(line), "Chip:  ESP32-S3");
    screen.label(16, 68, line, Colors::White, 1, Colors::Panel);

    snprintf(line, sizeof(line), "CPU:   %d MHz", ESP.getCpuFreqMHz());
    screen.label(16, 80, line, Colors::White, 1, Colors::Panel);

    snprintf(line, sizeof(line), "Flash: %lu MB", static_cast<unsigned long>(mem.flashSize / 1048576UL));
    screen.label(16, 92, line, Colors::White, 1, Colors::Panel);

    snprintf(line, sizeof(line), "Image: %lu KB", static_cast<unsigned long>(mem.sketchSize / 1024UL));
    screen.label(16, 104, line, Colors::Muted, 1, Colors::Panel);

    snprintf(line, sizeof(line), "Rst:   %s", mem.resetReason);
    screen.label(16, 116, line, Colors::Amber, 1, Colors::Panel);

    // Right card 1: Internal SRAM
    t.fillRoundRect(164, 36, 146, 56, 5, Colors::Panel);
    t.drawRoundRect(164, 36, 146, 56, 5, Colors::Border);
    screen.label(170, 41, "INTERNAL SRAM", Colors::Green, 1, Colors::Panel);

    // Right card 2: External PSRAM
    t.fillRoundRect(164, 96, 146, 48, 5, Colors::Panel);
    t.drawRoundRect(164, 96, 146, 48, 5, Colors::Border);
    screen.label(170, 101, "EXTERNAL PSRAM", Colors::Cyan, 1, Colors::Panel);

    screen.footer("Real ESP-IDF runtime metrics");
    fullRedraw_ = false;
  }

  // Dynamic values inside SRAM card
  t.fillRect(170, 54, 134, 32, Colors::Panel);
  snprintf(line, sizeof(line), "Free:  %lu KB", static_cast<unsigned long>(mem.freeInternalHeap / 1024UL));
  screen.label(170, 54, line, Colors::White, 1, Colors::Panel);

  snprintf(line, sizeof(line), "Min:   %lu KB", static_cast<unsigned long>(mem.minFreeInternalHeap / 1024UL));
  screen.label(170, 65, line, Colors::Muted, 1, Colors::Panel);

  snprintf(line, sizeof(line), "Block: %lu KB", static_cast<unsigned long>(mem.largestFreeBlock / 1024UL));
  screen.label(170, 76, line, Colors::Cyan, 1, Colors::Panel);

  // Dynamic values inside PSRAM card
  t.fillRect(170, 114, 134, 24, Colors::Panel);
  snprintf(line, sizeof(line), "Total: %lu MB", static_cast<unsigned long>(mem.totalPsram / 1048576UL));
  screen.label(170, 114, line, Colors::White, 1, Colors::Panel);

  snprintf(line, sizeof(line), "Free:  %lu KB", static_cast<unsigned long>(mem.freePsram / 1024UL));
  screen.label(170, 126, line, Colors::Green, 1, Colors::Panel);
}

void SettingsApp::renderWiFiDiag(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();
  char line[64];

  if (fullRedraw_) {
    screen.clear();
    screen.header("Wi-Fi Diagnostics", true);

    t.fillRoundRect(10, 36, 300, 108, 6, Colors::Panel);
    t.drawRoundRect(10, 36, 300, 108, 6, Colors::Border);
    screen.footer("WiFiService real-time telemetry");
    fullRedraw_ = false;
  }

  t.fillRect(16, 40, 288, 98, Colors::Panel);

  // Status & Link
  const char* status = ctx.wifi.statusName();
  const bool isConn = (ctx.wifi.connectState() == ConnectState::Connected);
  snprintf(line, sizeof(line), "Link:   %s", isConn ? ctx.wifi.connectedSSID() : "(Disconnected)");
  screen.label(16, 42, line, isConn ? Colors::Green : Colors::White, 1, Colors::Panel);

  snprintf(line, sizeof(line), "Status: %s (RSSI: %d dBm)", status, ctx.wifi.currentRSSI());
  screen.label(16, 54, line, isConn ? Colors::Cyan : Colors::Muted, 1, Colors::Panel);

  // IP Address
  char ipBuf[24]{};
  ctx.wifi.getLocalIP(ipBuf, sizeof(ipBuf));
  snprintf(line, sizeof(line), "IP:     %s", ipBuf[0] ? ipBuf : "0.0.0.0");
  screen.label(16, 66, line, Colors::White, 1, Colors::Panel);

  // Scan state & duration
  const char* scanStateStr = "Idle";
  switch (ctx.wifi.state()) {
    case ScanState::Preparing: scanStateStr = "Preparing"; break;
    case ScanState::Scanning:  scanStateStr = "Scanning..."; break;
    case ScanState::Complete:  scanStateStr = "Complete"; break;
    case ScanState::Failed:    scanStateStr = "Failed"; break;
    default: break;
  }
  snprintf(line, sizeof(line), "Scan:   %s (%lu ms)", scanStateStr,
           static_cast<unsigned long>(ctx.wifi.lastScanDurationMs()));
  screen.label(16, 78, line, Colors::Amber, 1, Colors::Panel);

  // Discovered APs & Retry
  snprintf(line, sizeof(line), "APs:    %u found | Attempt: %u/2",
           ctx.wifi.count(), ctx.wifi.attempt());
  screen.label(16, 90, line, Colors::White, 1, Colors::Panel);

  // Resource Owner
  const bool hasOwner = (ctx.resources.wifiOwner() != ResourceOwner::None);
  snprintf(line, sizeof(line), "Owner:  %s", hasOwner ? "WiFiScanner (Active)" : "Available");
  screen.label(16, 102, line, hasOwner ? Colors::Amber : Colors::Green, 1, Colors::Panel);

  // Last error
  const char* err = ctx.wifi.lastError();
  if (!err || !err[0]) err = "None";
  snprintf(line, sizeof(line), "Error:  %s", err);
  screen.label(16, 114, line, (err && strcmp(err, "None") != 0) ? Colors::Red : Colors::Muted, 1, Colors::Panel);
}

void SettingsApp::renderDevice(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();
  screen.clear();
  screen.header("Device Controls", true);

  if (confirmAction_ == ConfirmAction::None) {
    // Card 1: Reboot
    t.fillRoundRect(16, 44, 288, 40, 6, Colors::Panel);
    t.drawRoundRect(16, 44, 288, 40, 6, Colors::Amber);
    screen.label(28, 52, "REBOOT DEVICE", Colors::Amber, 1, Colors::Panel);
    screen.label(28, 66, "Flush resources and perform software restart", Colors::Muted, 1, Colors::Panel);

    // Card 2: Reset Settings
    t.fillRoundRect(16, 94, 288, 40, 6, Colors::Panel);
    t.drawRoundRect(16, 94, 288, 40, 6, Colors::Red);
    screen.label(28, 102, "RESET SETTINGS TO DEFAULTS", Colors::Red, 1, Colors::Panel);
    screen.label(28, 116, "Restore default brightness, timeout & theme", Colors::Muted, 1, Colors::Panel);

    screen.footer("Tap option | Confirmation required");
  } else {
    // Modal confirmation box
    t.fillRoundRect(24, 40, 272, 94, 8, Colors::Panel);
    t.drawRoundRect(24, 40, 272, 94, 8, (confirmAction_ == ConfirmAction::Reboot) ? Colors::Amber : Colors::Red);

    if (confirmAction_ == ConfirmAction::Reboot) {
      screen.label(80, 50, "Restart Device?", Colors::White, 2, Colors::Panel);
      screen.label(48, 74, "Firmware will reboot immediately.", Colors::Muted, 1, Colors::Panel);
    } else {
      screen.label(64, 50, "Reset Settings?", Colors::White, 2, Colors::Panel);
      screen.label(42, 74, "Reset preferences? (Wi-Fi preserved)", Colors::Muted, 1, Colors::Panel);
    }

    // Cancel button
    t.fillRoundRect(35, 96, 110, 28, 5, Colors::Border);
    screen.label(65, 104, "CANCEL", Colors::White, 1, Colors::Border);

    // Confirm button
    const uint16_t cBg = (confirmAction_ == ConfirmAction::Reboot) ? Colors::Amber : Colors::Red;
    t.fillRoundRect(175, 96, 110, 28, 5, cBg);
    screen.label(confirmAction_ == ConfirmAction::Reboot ? 195 : 190, 104,
                 confirmAction_ == ConfirmAction::Reboot ? "REBOOT" : "RESET",
                 Colors::Bg, 1, cBg);

    screen.footer("Select CANCEL or CONFIRM");
  }
}

void SettingsApp::renderSecurity(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();
  screen.clear();
  screen.header("Security & PIN", true);

  const bool pinEn = ctx.security.isPinEnabled();

  // Top Card: Status & Specs (Y: 34..72)
  t.fillRoundRect(10, 34, 300, 38, 5, Colors::Panel);
  t.drawRoundRect(10, 34, 300, 38, 5, pinEn ? Colors::Green : Colors::Border);

  char statStr[48];
  snprintf(statStr, sizeof(statStr), "PIN LOCK: %s (LEN: %u)",
           pinEn ? "ENABLED" : "DISABLED",
           ctx.security.getPinLength());
  screen.label(18, 40, statStr, pinEn ? Colors::Green : Colors::Amber, 1, Colors::Panel);
  screen.label(18, 54, "Crypto: PBKDF2-HMAC-SHA256 (2048 iter) | TRNG Salt", Colors::Muted, 1, Colors::Panel);

  // Button 1: Toggle PIN Lock (Y: 76..98, H: 22)
  t.fillRoundRect(10, 76, 300, 22, 4, Colors::Panel);
  t.drawRoundRect(10, 76, 300, 22, 4, pinEn ? Colors::Amber : Colors::Green);
  const char* togStr = pinEn ? "LOCK: [ ENABLED ] (Tap to Disable)" : "LOCK: [ DISABLED ] (Tap to Enable)";
  screen.label(30, 82, togStr, pinEn ? Colors::Green : Colors::Amber, 1, Colors::Panel);

  // Button 2: Change PIN (Y: 102..124, H: 22)
  t.fillRoundRect(10, 102, 300, 22, 4, Colors::Panel);
  t.drawRoundRect(10, 102, 300, 22, 4, Colors::Cyan);
  screen.label(30, 108, "[ CONFIGURE / CHANGE PIN (4-8 DIGITS) ]", Colors::Cyan, 1, Colors::Panel);

  // Button 3: Lock Now (Y: 128..150, H: 22)
  t.fillRoundRect(10, 128, 300, 22, 4, Colors::Panel);
  t.drawRoundRect(10, 128, 300, 22, 4, Colors::Red);
  screen.label(86, 134, "[ LOCK DEVICE NOW ]", Colors::Red, 1, Colors::Panel);

  screen.footer("Tap Back to exit | Zero plaintext secrets");
}

void SettingsApp::handleSecurityTouch(AppContext& ctx, const TouchEvent& ev) {
  if (ev.gesture != TouchGesture::Tap && ev.gesture != TouchGesture::DoubleTap) return;

  // Button 1: Toggle PIN Lock (Y: 76..98)
  if (ev.y >= 76 && ev.y <= 98 && ev.x >= 10 && ev.x <= 310) {
    ctx.security.setPinEnabled(!ctx.security.isPinEnabled());
    invalidate();
    return;
  }

  // Button 2: Change PIN (Y: 102..124)
  if (ev.y >= 102 && ev.y <= 124 && ev.x >= 10 && ev.x <= 310) {
    subView_ = SettingsSubView::SetPin;
    pinInputLen_ = 0;
    pinInputBuffer_[0] = '\0';
    pinError_ = false;
    fullRedraw_ = true;
    invalidate();
    return;
  }

  // Button 3: Lock Now (Y: 128..150)
  if (ev.y >= 128 && ev.y <= 150 && ev.x >= 10 && ev.x <= 310) {
    if (!ctx.security.isPinEnabled()) {
      ctx.security.setPinEnabled(true);
    }
    ctx.security.lock();
    return;
  }
}

void SettingsApp::renderSetPin(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();
  screen.clear();
  screen.header("Set Security PIN", true);

  // Status & Guidance (Y: 30..44)
  if (pinError_) {
    screen.label(64, 32, "Error: PIN must be 4 to 8 digits!", Colors::Red, 1, Colors::Bg);
  } else {
    screen.label(60, 32, "Enter 4 to 8 digits, then tap SAVE", Colors::White, 1, Colors::Bg);
  }

  // PIN Display Box (Y: 46..64)
  t.fillRoundRect(70, 46, 180, 20, 3, Colors::Panel);
  t.drawRoundRect(70, 46, 180, 20, 3, Colors::Border);

  if (pinInputLen_ == 0) {
    screen.label(135, 52, "(empty)", Colors::Muted, 1, Colors::Panel);
  } else {
    const int startX = 160 - (pinInputLen_ * 16) / 2;
    for (uint8_t i = 0; i < pinInputLen_; ++i) {
      t.fillCircle(startX + i * 16 + 8, 56, 4, Colors::Green);
    }
  }

  // Keypad: 3 rows x 4 cols (Y: 68..144)
  const char* labels[3][4] = {
    {"1", "2", "3", "4"},
    {"5", "6", "7", "8"},
    {"9", "0", "DEL", "SAVE"}
  };

  for (int row = 0; row < 3; ++row) {
    const int ry = 68 + row * 26;
    for (int col = 0; col < 4; ++col) {
      const int rx = 16 + col * 74;
      const bool isSpecial = (row == 2 && (col == 2 || col == 3));

      uint16_t btnColor = Colors::Panel;
      if (isSpecial) {
        btnColor = (col == 2) ? Colors::Red : Colors::Green;
      }

      t.fillRoundRect(rx, ry, 68, 22, 4, btnColor);
      t.drawRoundRect(rx, ry, 68, 22, 4, Colors::Border);

      const char* lbl = labels[row][col];
      const int textW = static_cast<int>(strlen(lbl)) * (isSpecial ? 6 : 12);
      const int tx = rx + (68 - textW) / 2;
      const int ty = ry + (isSpecial ? 7 : 4);
      screen.label(tx, ty, lbl, isSpecial ? (col == 2 ? Colors::White : Colors::Bg) : Colors::White,
                   isSpecial ? 1 : 2, btnColor);
    }
  }

  screen.footer("Tap Cancel to discard | PBKDF2 Encrypted");
}

void SettingsApp::handleSetPinTouch(AppContext& ctx, const TouchEvent& ev) {
  if (ev.gesture != TouchGesture::Tap && ev.gesture != TouchGesture::DoubleTap) return;

  const char keys[3][4] = {
    {'1', '2', '3', '4'},
    {'5', '6', '7', '8'},
    {'9', '0', 'D', 'S'}
  };

  for (int row = 0; row < 3; ++row) {
    const int ry = 68 + row * 26;
    if (ev.y >= ry && ev.y <= ry + 22) {
      for (int col = 0; col < 4; ++col) {
        const int rx = 16 + col * 74;
        if (ev.x >= rx && ev.x <= rx + 68) {
          const char k = keys[row][col];
          if (k >= '0' && k <= '9') {
            if (pinInputLen_ < 8) {
              pinInputBuffer_[pinInputLen_++] = k;
              pinInputBuffer_[pinInputLen_] = '\0';
              pinError_ = false;
              invalidate();
            }
          } else if (k == 'D') {
            if (pinInputLen_ > 0) {
              pinInputBuffer_[--pinInputLen_] = '\0';
              pinError_ = false;
              invalidate();
            }
          } else if (k == 'S') {
            if (pinInputLen_ < 4) {
              pinError_ = true;
              invalidate();
            } else {
              ctx.security.setPin(pinInputBuffer_);
              ctx.security.setPinEnabled(true);
              CryptoService::secureZero(pinInputBuffer_, sizeof(pinInputBuffer_));
              pinInputLen_ = 0;
              subView_ = SettingsSubView::Security;
              fullRedraw_ = true;
              invalidate();
              Serial.println("[Settings] New security PIN set and enabled.");
            }
          }
          return;
        }
      }
    }
  }
}
