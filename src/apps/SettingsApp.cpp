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

namespace {
// Fixed-width opaque glyphs replace old text without blanking a whole panel.
void metricLabel(DisplayManager& screen, int x, int y, const char* text,
                 uint16_t color = Colors::White, uint8_t size = 1,
                 uint16_t bg = Colors::Bg) {
  char padded[49];
  snprintf(padded, sizeof(padded), "%-48.48s", text);
  screen.label(x, y, padded, color, size, bg);
}
}

void SettingsApp::onEnter(AppContext&) {
  subView_ = SettingsSubView::Menu;
  focusedIndex_ = menuIndex_ = scrollOffset_ = metricPage_ = 0;
  confirmAction_ = ConfirmAction::None;
  fullRedraw_ = true;
  hasRendered_ = false;
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

void SettingsApp::onDisplayRestored(AppContext& ctx) {
  fullRedraw_ = true;
  hasRendered_ = false;
  if (subView_ == SettingsSubView::TouchTest) touchTest_.onDisplayRestored(ctx);
  invalidate();
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
    focusedIndex_ = deviceIndex_;
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
    menuIndex_ = static_cast<uint8_t>((menuIndex_ + 1) % 7);
    if (menuIndex_ < scrollOffset_ || menuIndex_ >= scrollOffset_ + 3)
      scrollOffset_ = menuIndex_ > 2 ? menuIndex_ - 2 : 0;
  } else if (subView_ == SettingsSubView::Display) {
    focusedIndex_ = (focusedIndex_ + 1) % 10;
  } else if (subView_ == SettingsSubView::Theme) {
    focusedIndex_ = (focusedIndex_ + 1) % 4;
  } else if (subView_ == SettingsSubView::Device) {
    focusedIndex_ = (focusedIndex_ + 1) % 2;
  } else if (subView_ == SettingsSubView::Memory) {
    metricPage_ ^= 1; fullRedraw_ = true;
  } else if (subView_ == SettingsSubView::TouchTest) {
    touchTest_.onPrimaryAction(ctx);
  }
  invalidate();
}

void SettingsApp::onActivate(AppContext& ctx) {
  if (subView_ == SettingsSubView::Menu) {
    static const SettingsSubView views[] = {SettingsSubView::Display, SettingsSubView::Theme,
      SettingsSubView::Memory, SettingsSubView::WiFiDiag, SettingsSubView::Device,
      SettingsSubView::Security, SettingsSubView::TouchTest};
    subView_ = views[menuIndex_]; focusedIndex_ = metricPage_ = 0; fullRedraw_ = true;
    if (subView_ == SettingsSubView::TouchTest) touchTest_.onEnter(ctx);
  } else if (subView_ == SettingsSubView::Display) {
    const uint8_t bright[] = {25, 50, 75, 100};
    const uint16_t timeout[] = {30, 60, 180, 300, 0};
    if (focusedIndex_ < 4) ctx.settings.setBrightness(bright[focusedIndex_]);
    else if (focusedIndex_ < 9) ctx.settings.setScreenTimeoutSec(timeout[focusedIndex_ - 4]);
    else ctx.display.sleep();
  } else if (subView_ == SettingsSubView::Theme) ctx.settings.setTheme(focusedIndex_);
  else if (subView_ == SettingsSubView::Device) {
    if (confirmAction_ == ConfirmAction::None) {
      deviceIndex_ = focusedIndex_;
      confirmAction_ = focusedIndex_ == 0 ? ConfirmAction::Reboot : ConfirmAction::ResetSettings;
      focusedIndex_ = 0; // Cancel is always the initial selection.
    } else if (focusedIndex_ == 0) {
      confirmAction_ = ConfirmAction::None;
      focusedIndex_ = deviceIndex_;
    }
    else {
      if (confirmAction_ == ConfirmAction::Reboot) {
        ctx.wifi.shutdown();
        ctx.resources.releaseWiFi(ResourceOwner::WiFiScanner);
        ctx.security.lock();
        Serial.println("[Settings] Confirmed restart; radio stopped");
        Serial.flush();
        ESP.restart();
      } else ctx.settings.resetToDefaults();
      confirmAction_ = ConfirmAction::None;
      focusedIndex_ = deviceIndex_;
    }
  } else if (subView_ == SettingsSubView::Memory) onPrimaryAction(ctx);
  invalidate();
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
  if (ev.gesture == TouchGesture::SwipeUp || ev.gesture == TouchGesture::SwipeDown) {
    if (ev.gesture == TouchGesture::SwipeUp && scrollOffset_ < 4) ++scrollOffset_;
    if (ev.gesture == TouchGesture::SwipeDown && scrollOffset_ > 0) --scrollOffset_;
    menuIndex_ = scrollOffset_; invalidate(); return;
  }
  if (ev.gesture != TouchGesture::Tap && ev.gesture != TouchGesture::DoubleTap) return;
  if (ev.x < 10 || ev.x >= 310 || ev.y < 36 || ev.y >= 144) return;
  const uint8_t row = (ev.y - 36) / 36;
  if ((ev.y - 36) % 36 >= 32) return;
  menuIndex_ = scrollOffset_ + row;
  onActivate(ctx);
}

void SettingsApp::handleDisplayTouch(AppContext& ctx, const TouchEvent& ev) {
  if (ev.gesture != TouchGesture::Tap && ev.gesture != TouchGesture::DoubleTap) return;
  const int bx[] = {14, 88, 162, 236};
  const int sx[] = {10, 70, 130, 190, 250};
  if (ev.y >= 48 && ev.y < 70) {
    for (uint8_t i = 0; i < 4; ++i) if (ev.x >= bx[i] && ev.x < bx[i] + 66) {
      focusedIndex_ = i; onActivate(ctx); return;
    }
  } else if (ev.y >= 88 && ev.y < 110) {
    for (uint8_t i = 0; i < 5; ++i) if (ev.x >= sx[i] && ev.x < sx[i] + 54) {
      focusedIndex_ = i + 4; onActivate(ctx); return;
    }
  } else if (ev.y >= 118 && ev.y < 144 && ev.x >= 60 && ev.x < 260) {
    focusedIndex_ = 9; onActivate(ctx);
  }
}

void SettingsApp::handleThemeTouch(AppContext& ctx, const TouchEvent& ev) {
  if (ev.gesture != TouchGesture::Tap && ev.gesture != TouchGesture::DoubleTap) return;
  for (uint8_t i = 0; i < 4; ++i) {
    const int x = i % 2 ? 164 : 14, y = i < 2 ? 50 : 92;
    if (ev.x >= x && ev.x < x + 142 && ev.y >= y && ev.y < y + 34) {
      focusedIndex_ = i; onActivate(ctx); return;
    }
  }
}

void SettingsApp::handleMemoryTouch(AppContext& ctx, const TouchEvent& ev) {
  if (ev.gesture == TouchGesture::SwipeLeft || ev.gesture == TouchGesture::SwipeRight ||
      ev.gesture == TouchGesture::SwipeUp || ev.gesture == TouchGesture::SwipeDown) onPrimaryAction(ctx);
}

void SettingsApp::handleWiFiDiagTouch(AppContext&, const TouchEvent&) {}

void SettingsApp::handleDeviceTouch(AppContext& ctx, const TouchEvent& ev) {
  // Double taps do not confirm destructive actions.
  if (ev.gesture != TouchGesture::Tap) return;
  if (confirmAction_ == ConfirmAction::None) {
    if (ev.x < 16 || ev.x >= 304) return;
    if (ev.y >= 44 && ev.y < 84) focusedIndex_ = 0;
    else if (ev.y >= 94 && ev.y < 134) focusedIndex_ = 1;
    else return;
    confirmContactId_ = ev.contactId;
    confirmOpenedMs_ = millis();
  } else {
    if (ev.y < 96 || ev.y >= 124) return;
    if (ev.x >= 35 && ev.x < 145) focusedIndex_ = 0;
    else if (ev.x >= 175 && ev.x < 285) {
      // NONE/down and delayed SINGLE_CLICK may both be Tap for one contact.
      // Require a fresh contact beyond the double-tap window, without blocking.
      const int32_t contactDelta = static_cast<int32_t>(ev.contactId - confirmContactId_);
      if (static_cast<int32_t>(ev.contactStartedMs - confirmOpenedMs_) < 500) {
        // A contact begun inside the rejection window stays rejected even if
        // its delayed SINGLE_CLICK arrives after the window has elapsed.
        if (contactDelta > 0) confirmContactId_ = ev.contactId;
        return;
      }
      if (contactDelta <= 0) return;
      focusedIndex_ = 1;
    }
    else return;
  }
  onActivate(ctx);
}

void SettingsApp::render(AppContext& ctx) {
  if (!hasRendered_ || drawnView_ != subView_ || drawnConfirmation_ != confirmAction_ ||
      drawnSaveOk_ != ctx.settings.lastSaveOk())
    fullRedraw_ = true;
  const bool controls = subView_ == SettingsSubView::Menu || subView_ == SettingsSubView::Display ||
                        subView_ == SettingsSubView::Theme || subView_ == SettingsSubView::Device;
  if (controls && !fullRedraw_ && drawnFocus_ == focusedIndex_ && drawnMenu_ == menuIndex_ &&
      drawnScroll_ == scrollOffset_ && drawnBrightness_ == ctx.settings.brightness() &&
      drawnTheme_ == ctx.settings.theme() && drawnTimeout_ == ctx.settings.screenTimeoutSec() &&
      drawnSaveOk_ == ctx.settings.lastSaveOk()) return;
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
  if (!ctx.settings.lastSaveOk()) ctx.display.footer("NVS error: preferences were not saved");
  hasRendered_ = true; drawnView_ = subView_; drawnConfirmation_ = confirmAction_;
  drawnFocus_ = focusedIndex_; drawnMenu_ = menuIndex_; drawnScroll_ = scrollOffset_;
  drawnBrightness_ = ctx.settings.brightness(); drawnTheme_ = ctx.settings.theme();
  drawnTimeout_ = ctx.settings.screenTimeoutSec(); drawnSaveOk_ = ctx.settings.lastSaveOk();
  fullRedraw_ = false;
}

void SettingsApp::renderMenu(AppContext& ctx) {
  auto& screen = ctx.display;
  auto& t = screen.gfx();
  if (fullRedraw_) {
    screen.clear(); screen.header("Settings", true);
    screen.footer("Swipe | BOOT next/hold open | Back");
  }
  const bool rowsChanged = fullRedraw_ || drawnScroll_ != scrollOffset_;
  const char* titles[] = {"Display & timeout", "Accent color", "System & memory",
    "Wi-Fi diagnostics", "Reboot / reset settings", "Security (existing)", "Touch test"};
  for (uint8_t row = 0; row < 3; ++row) {
    const uint8_t i = scrollOffset_ + row;
    const int y = 36 + row * 36;
    if (rowsChanged) {
      t.fillRoundRect(10, y, 294, 32, 5, Colors::Panel);
      screen.label(20, y + 10, titles[i], Colors::White, 1, Colors::Panel);
    }
    if (rowsChanged || i == menuIndex_ || i == drawnMenu_)
      t.drawRoundRect(10, y, 294, 32, 5, i == menuIndex_ ? screen.themeColor() : Colors::Border);
  }
  if (rowsChanged) {
    t.fillRect(312, 36, 3, 108, Colors::Bg);
    t.fillRect(312, 36 + scrollOffset_ * 12, 3, 40, screen.themeColor());
  }
}

void SettingsApp::renderDisplay(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();
  if (fullRedraw_) {
    screen.clear(); screen.header("Display & Screen", true);
    screen.footer("BOOT next/hold set | Touch to wake");
  }

  // Section 1: Brightness
  char bStr[32];
  snprintf(bStr, sizeof(bStr), "BRIGHTNESS: %u%%", ctx.settings.brightness());
  if (fullRedraw_ || drawnBrightness_ != ctx.settings.brightness()) {
    t.fillRect(14, 36, 292, 10, Colors::Bg);
    screen.label(14, 36, bStr, Colors::Cyan, 1, Colors::Bg);
  }

  const uint8_t brightVals[4] = {25, 50, 75, 100};
  const char* brightLabels[4] = {"25%", "50%", "75%", "100%"};
  const int bX[4] = {14, 88, 162, 236};
  const uint8_t currentB = ctx.settings.brightness();

  for (int i = 0; i < 4; ++i) {
    const bool active = (currentB == brightVals[i]);
    const uint16_t bg = active ? Colors::Cyan : Colors::Panel;
    const uint16_t tc = active ? Colors::Bg : Colors::White;
    const bool valueChanged = fullRedraw_ || drawnBrightness_ != currentB;
    if (valueChanged) {
      t.fillRoundRect(bX[i], 48, 66, 22, 4, bg);
      const int tx = bX[i] + (66 - static_cast<int>(strlen(brightLabels[i])) * 6) / 2;
      screen.label(tx, 55, brightLabels[i], tc, 1, bg);
    }
    if (valueChanged || focusedIndex_ == i || drawnFocus_ == i)
      t.drawRoundRect(bX[i], 48, 66, 22, 4, focusedIndex_ == i ? Colors::White : Colors::Border);
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
  if (fullRedraw_ || drawnTimeout_ != sec) {
    t.fillRect(14, 75, 292, 10, Colors::Bg);
    screen.label(14, 75, sStr, Colors::Green, 1, Colors::Bg);
  }

  const uint16_t sleepVals[5] = {30, 60, 180, 300, 0};
  const char* sleepLabels[5] = {"30s", "1m", "3m", "5m", "Off"};
  const int sX[5] = {10, 70, 130, 190, 250};

  for (int i = 0; i < 5; ++i) {
    const bool active = (sec == sleepVals[i]);
    const uint16_t bg = active ? Colors::Green : Colors::Panel;
    const uint16_t tc = active ? Colors::Bg : Colors::White;
    const bool valueChanged = fullRedraw_ || drawnTimeout_ != sec;
    if (valueChanged) {
      t.fillRoundRect(sX[i], 88, 54, 22, 4, bg);
      const int tx = sX[i] + (54 - static_cast<int>(strlen(sleepLabels[i])) * 6) / 2;
      screen.label(tx, 95, sleepLabels[i], tc, 1, bg);
    }
    if (valueChanged || focusedIndex_ == i + 4 || drawnFocus_ == i + 4)
      t.drawRoundRect(sX[i], 88, 54, 22, 4, focusedIndex_ == i + 4 ? Colors::White : Colors::Border);
  }

  // Sleep now test button
  if (fullRedraw_) {
    t.fillRoundRect(60, 118, 200, 26, 6, Colors::Panel);
    screen.label(78, 126, "[ TEST SLEEP MODE NOW ]", Colors::Amber, 1, Colors::Panel);
  }
  if (fullRedraw_ || focusedIndex_ == 9 || drawnFocus_ == 9)
    t.drawRoundRect(60, 118, 200, 26, 6, focusedIndex_ == 9 ? Colors::White : Colors::Amber);
}

void SettingsApp::renderTheme(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();
  if (fullRedraw_) {
    screen.clear();
    screen.label(14, 36, "SELECT SYSTEM ACCENT COLOR:", Colors::White, 1, Colors::Bg);
    screen.footer("BOOT next/hold set | Back: menu");
  }
  if (fullRedraw_ || drawnTheme_ != ctx.settings.theme()) screen.header("Theme Accent", true);

  const char* names[4] = {"Cyber Cyan", "Emerald Green", "Amber Gold", "Neon Magenta"};
  const uint16_t colors[4] = {Colors::Cyan, Colors::Green, Colors::Amber, Colors::Magenta};
  const uint8_t curTheme = ctx.settings.theme();

  for (uint8_t i = 0; i < 4; ++i) {
    const int x = (i % 2 == 0) ? 14 : 164;
    const int y = (i < 2) ? 50 : 92;
    const bool active = (curTheme == i);
    if (!fullRedraw_ && drawnTheme_ == curTheme) {
      if (i == focusedIndex_ || i == drawnFocus_)
        t.drawRoundRect(x, y, 142, 34, 5, focusedIndex_ == i ? Colors::White : Colors::Border);
      continue;
    }
    if (!fullRedraw_ && i != curTheme && i != drawnTheme_ && i != focusedIndex_ && i != drawnFocus_) continue;
    t.fillRoundRect(x, y, 142, 34, 5, Colors::Panel);
    t.drawRoundRect(x, y, 142, 34, 5, focusedIndex_ == i ? Colors::White : Colors::Border);

    // Color swatch circle
    t.fillCircle(x + 18, y + 17, 8, colors[i]);
    if (active) {
      t.drawCircle(x + 18, y + 17, 10, Colors::White);
    }

    screen.label(x + 36, y + 13, names[i], active ? colors[i] : Colors::White, 1, Colors::Panel);
  }

}

void SettingsApp::renderMemory(AppContext& ctx) {
  auto& screen = ctx.display;
  char line[48];
  const auto m = MemoryProfiler::capture();
  if (fullRedraw_) {
    screen.clear(); screen.header(metricPage_ ? "Memory monitor" : "System info", true);
    screen.footer("Swipe / BOOT: page | Back: menu"); fullRedraw_ = false;
  }
  if (!metricPage_) {
    snprintf(line, sizeof(line), "Firmware: %s | Chip: %.16s", Board::kVersion, ESP.getChipModel());
    metricLabel(screen, 10, 38, line);
    snprintf(line, sizeof(line), "CPU: %d MHz | Flash: %lu KiB", ESP.getCpuFreqMHz(), (unsigned long)(m.flashSize/1024));
    metricLabel(screen, 10, 54, line);
    snprintf(line, sizeof(line), "Firmware image: %lu bytes", (unsigned long)m.sketchSize);
    metricLabel(screen, 10, 70, line);
    snprintf(line, sizeof(line), "Uptime: %lu s", (unsigned long)m.uptimeSec);
    metricLabel(screen, 10, 86, line);
    snprintf(line, sizeof(line), "Reset: %.36s", m.resetReason);
    metricLabel(screen, 10, 102, line);
  } else {
    snprintf(line, sizeof(line), "Internal free: %lu B", (unsigned long)m.freeInternalHeap); metricLabel(screen, 10,38,line);
    snprintf(line, sizeof(line), "Internal min: %lu B", (unsigned long)m.minFreeInternalHeap); metricLabel(screen, 10,54,line);
    snprintf(line, sizeof(line), "Largest internal: %lu B", (unsigned long)m.largestFreeBlock); metricLabel(screen, 10,70,line);
    snprintf(line, sizeof(line), "PSRAM total/free: %lu / %lu B", (unsigned long)m.totalPsram, (unsigned long)m.freePsram); metricLabel(screen, 10,86,line);
    snprintf(line, sizeof(line), "PSRAM min: %lu B", (unsigned long)m.minFreePsram); metricLabel(screen, 10,102,line);
    snprintf(line, sizeof(line), "Loop stack HWM: %lu bytes", (unsigned long)m.stackHighWaterBytes); metricLabel(screen, 10,118,line);
  }
  if (!m.available) metricLabel(screen, 10,134,"HOST MOCK - no device measurements",Colors::Amber);
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


  // Status & Link
  const char* status = ctx.wifi.statusName();
  const bool isConn = (ctx.wifi.connectState() == ConnectState::Connected);
  snprintf(line, sizeof(line), "Link:   %s", isConn ? ctx.wifi.connectedSSID() : "(Disconnected)");
  metricLabel(screen, 16, 42, line, isConn ? Colors::Green : Colors::White, 1, Colors::Panel);

  snprintf(line, sizeof(line), "Status: %s (RSSI: %d dBm)", status, ctx.wifi.currentRSSI());
  metricLabel(screen, 16, 54, line, isConn ? Colors::Cyan : Colors::Muted, 1, Colors::Panel);

  // IP Address
  char ipBuf[24]{};
  ctx.wifi.getLocalIP(ipBuf, sizeof(ipBuf));
  snprintf(line, sizeof(line), "IP:     %s", ipBuf[0] ? ipBuf : "0.0.0.0");
  metricLabel(screen, 16, 66, line, Colors::White, 1, Colors::Panel);

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
  metricLabel(screen, 16, 78, line, Colors::Amber, 1, Colors::Panel);

  // Discovered APs & Retry
  snprintf(line, sizeof(line), "APs:    %u found | Attempt: %u/2",
           ctx.wifi.count(), ctx.wifi.attempt());
  metricLabel(screen, 16, 90, line, Colors::White, 1, Colors::Panel);

  // Resource Owner
  const bool hasOwner = (ctx.resources.wifiOwner() != ResourceOwner::None);
  snprintf(line, sizeof(line), "Owner:  %s", hasOwner ? "WiFiScanner (Active)" : "Available");
  metricLabel(screen, 16, 102, line, hasOwner ? Colors::Amber : Colors::Green, 1, Colors::Panel);

  // Last error
  const char* err = ctx.wifi.lastError();
  if (!err || !err[0]) err = "None";
  snprintf(line, sizeof(line), "Error:  %s", err);
  metricLabel(screen, 16, 114, line, (err && strcmp(err, "None") != 0) ? Colors::Red : Colors::Muted, 1, Colors::Panel);
}

void SettingsApp::renderDevice(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();
  if (!fullRedraw_) {
    if (confirmAction_ == ConfirmAction::None) {
      t.drawRoundRect(16, 44, 288, 40, 6, focusedIndex_ == 0 ? Colors::White : Colors::Amber);
      t.drawRoundRect(16, 94, 288, 40, 6, focusedIndex_ == 1 ? Colors::White : Colors::Red);
    } else {
      t.drawRoundRect(35, 96, 110, 28, 5, focusedIndex_ == 0 ? Colors::White : Colors::Border);
      t.drawRoundRect(175, 96, 110, 28, 5, focusedIndex_ == 1 ? Colors::White : Colors::Border);
    }
    return;
  }
  if (!hasRendered_ || drawnView_ != subView_) {
    screen.clear(); screen.header("Device Controls", true);
  } else t.fillRect(0, 33, 320, 118, Colors::Bg);

  if (confirmAction_ == ConfirmAction::None) {
    // Card 1: Reboot
    t.fillRoundRect(16, 44, 288, 40, 6, Colors::Panel);
    t.drawRoundRect(16, 44, 288, 40, 6, focusedIndex_ == 0 ? Colors::White : Colors::Amber);
    screen.label(28, 52, "REBOOT DEVICE", Colors::Amber, 1, Colors::Panel);
    screen.label(28, 66, "Flush resources and perform software restart", Colors::Muted, 1, Colors::Panel);

    // Card 2: Reset Settings
    t.fillRoundRect(16, 94, 288, 40, 6, Colors::Panel);
    t.drawRoundRect(16, 94, 288, 40, 6, focusedIndex_ == 1 ? Colors::White : Colors::Red);
    screen.label(28, 102, "RESET SETTINGS TO DEFAULTS", Colors::Red, 1, Colors::Panel);
    screen.label(28, 116, "Restore default brightness, timeout & theme", Colors::Muted, 1, Colors::Panel);

    screen.footer("BOOT next/hold open | Back: menu");
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

    t.drawRoundRect(35, 96, 110, 28, 5, focusedIndex_ == 0 ? Colors::White : Colors::Border);

    // Confirm button
    const uint16_t cBg = (confirmAction_ == ConfirmAction::Reboot) ? Colors::Amber : Colors::Red;
    t.fillRoundRect(175, 96, 110, 28, 5, cBg);
    screen.label(confirmAction_ == ConfirmAction::Reboot ? 195 : 190, 104,
                 confirmAction_ == ConfirmAction::Reboot ? "REBOOT" : "RESET",
                 Colors::Bg, 1, cBg);

    t.drawRoundRect(175, 96, 110, 28, 5, focusedIndex_ == 1 ? Colors::White : Colors::Border);
    screen.footer("BOOT next/hold confirm | Back: cancel");
  }
}

void SettingsApp::renderSecurity(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();
  if (fullRedraw_) {
    screen.clear(); screen.header("Security & PIN", true);
  }

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
  if (fullRedraw_) {
    screen.clear(); screen.header("Set Security PIN", true);
  }
  t.fillRect(10, 34, 300, 12, Colors::Bg);

  // Status & Guidance (Y: 30..44)
  if (pinError_) {
    screen.label(64, 36, "Error: PIN must be 4 to 8 digits!", Colors::Red, 1, Colors::Bg);
  } else {
    screen.label(60, 36, "Enter 4 to 8 digits, then tap SAVE", Colors::White, 1, Colors::Bg);
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

  if (fullRedraw_) for (int row = 0; row < 3; ++row) {
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

  if (fullRedraw_) screen.footer("Tap Back to discard | PIN hidden");
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
