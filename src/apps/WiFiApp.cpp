#include "apps/WiFiApp.h"
#include "core/AppContext.h"
#include "hardware/DisplayManager.h"
#include <Arduino.h>
#include <string.h>

void WiFiApp::onEnter(AppContext& ctx) {
  (void)ctx;
  subView_ = WiFiSubView::Menu;
  lastConnState_ = ctx.wifi.connectState();
  menuFullRedraw_ = true;
  invalidate();
}

void WiFiApp::onExit(AppContext& ctx) {
  if (subView_ == WiFiSubView::Scanner) {
    scannerApp_.onExit(ctx);
  }
  subView_ = WiFiSubView::Menu;
  menuFullRedraw_ = true;
}

void WiFiApp::onTick(AppContext& ctx, uint32_t nowMs) {
  if (subView_ == WiFiSubView::Scanner) {
    scannerApp_.onTick(ctx, nowMs);
    if (scannerApp_.consumeDirty()) scannerApp_.render(ctx);
  } else {
    if (ctx.wifi.connectState() != lastConnState_) {
      lastConnState_ = ctx.wifi.connectState();
      invalidate();
    }
  }
}

bool WiFiApp::onBack(AppContext& ctx) {
  if (subView_ == WiFiSubView::Scanner) {
    if (scannerApp_.onBack(ctx)) {
      return true;
    }
    scannerApp_.onExit(ctx);
    subView_ = WiFiSubView::Menu;
    menuFullRedraw_ = true;
    invalidate();
    return true;
  }
  if (subView_ == WiFiSubView::Saved) {
    subView_ = WiFiSubView::Menu;
    menuFullRedraw_ = true;
    invalidate();
    return true;
  }
  return false;
}

void WiFiApp::onPrimaryAction(AppContext& ctx) {
  if (subView_ == WiFiSubView::Scanner) {
    scannerApp_.onPrimaryAction(ctx);
  }
}

void WiFiApp::onTouch(AppContext& ctx, const TouchEvent& ev) {
  if (subView_ == WiFiSubView::Scanner) {
    scannerApp_.onTouch(ctx, ev);
  } else if (subView_ == WiFiSubView::Menu) {
    handleMenuTouch(ctx, ev);
  } else if (subView_ == WiFiSubView::Saved) {
    handleSavedTouch(ctx, ev);
  }
}

void WiFiApp::handleMenuTouch(AppContext& ctx, const TouchEvent& ev) {
  if (ev.gesture != TouchGesture::Tap && ev.gesture != TouchGesture::DoubleTap) return;

  if (ev.x < 45 && ev.y < 35) {
    onBack(ctx);
    return;
  }

  uint8_t target = 255;
  if (ev.y >= 52 && ev.y <= 96 && ev.x >= 10 && ev.x <= 310) {
    target = 0;
  } else if (ev.y >= 98 && ev.y <= 144 && ev.x >= 10 && ev.x <= 310) {
    target = 1;
  }

  if (target != 255) {
    if (armedCard_ == target && ev.contactId != armedCardContactId_ && ev.contactId != 0) {
      armedCard_ = 255;
      armedCardContactId_ = 0;
      if (target == 0) {
        subView_ = WiFiSubView::Scanner;
        scannerApp_.onEnter(ctx);
      } else {
        subView_ = WiFiSubView::Saved;
        savedOffset_ = 0;
        invalidate();
      }
    } else {
      armedCard_ = target;
      armedCardContactId_ = ev.contactId;
      menuFullRedraw_ = true;
      invalidate();
    }
  }
}

void WiFiApp::handleSavedTouch(AppContext& ctx, const TouchEvent& ev) {
  if (ev.gesture != TouchGesture::Tap && ev.gesture != TouchGesture::DoubleTap) return;

  if (ev.x < 45 && ev.y < 35) {
    onBack(ctx);
    return;
  }

  const uint8_t count = ctx.wifi.savedCount();
  for (uint8_t i = 0; i < 3; ++i) {
    const uint8_t idx = savedOffset_ + i;
    if (idx >= count) break;
    const SavedNetwork* net = ctx.wifi.savedNetwork(idx);
    if (!net) break;

    const int y = 40 + i * 36;
    if (ev.y >= y && ev.y <= y + 32) {
      const bool isCurrent = (ctx.wifi.connectState() == ConnectState::Connected &&
                              strcmp(ctx.wifi.connectedSSID(), net->ssid) == 0);
      if (isCurrent) {
        if (ev.x >= 200 && ev.x <= 308) {
          ctx.wifi.disconnect();
          invalidate();
          return;
        }
      } else {
        if (ev.x >= 136 && ev.x <= 220) {
          ctx.wifi.connectSaved(idx, millis());
          subView_ = WiFiSubView::Scanner;
          scannerApp_.onEnter(ctx);
          return;
        }
        if (ev.x >= 222 && ev.x <= 308) {
          ctx.wifi.forgetNetwork(idx);
          invalidate();
          return;
        }
      }
    }
  }
}

void WiFiApp::render(AppContext& ctx) {
  if (subView_ == WiFiSubView::Scanner) {
    scannerApp_.render(ctx);
  } else if (subView_ == WiFiSubView::Saved) {
    renderSaved(ctx);
  } else {
    renderMenu(ctx);
  }
}

void WiFiApp::renderMenu(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();

  if (menuFullRedraw_) {
    screen.clear();
    screen.header("Wi-Fi", true);

    // Card 0: Scan Networks (Y: 54, H: 42)
    t.fillRoundRect(10, 54, 300, 42, 6, Colors::Panel);
    t.drawRoundRect(10, 54, 300, 42, 6, Colors::Border);
    t.fillRoundRect(18, 60, 42, 30, 4, Colors::Cyan);
    screen.label(22, 70, "SCAN", Colors::Bg, 1, Colors::Cyan);
    screen.label(68, 60, "Scan Networks", Colors::White, 2, Colors::Panel);
    screen.label(69, 79, "Search & connect 2.4GHz", Colors::Muted, 1, Colors::Panel);
    screen.label(292, 67, ">", Colors::Cyan, 2, Colors::Panel);

    // Card 1: Saved Networks (Y: 100, H: 42)
    t.fillRoundRect(10, 100, 300, 42, 6, Colors::Panel);
    t.drawRoundRect(10, 100, 300, 42, 6, Colors::Border);
    t.fillRoundRect(18, 106, 42, 30, 4, Colors::Green);
    screen.label(20, 116, "SAVED", Colors::Bg, 1, Colors::Green);
    screen.label(68, 106, "Saved Networks", Colors::White, 2, Colors::Panel);
    char sub[32];
    snprintf(sub, sizeof(sub), "%u network(s) remembered", ctx.wifi.savedCount());
    screen.label(69, 125, sub, Colors::Muted, 1, Colors::Panel);
    screen.label(292, 113, ">", Colors::Green, 2, Colors::Panel);

    screen.footer("GPIO14 = Home  |  Tap card to open");
    menuFullRedraw_ = false;
  }

  // Differential live status bar update at Y: 36..50

  const bool isConn = (ctx.wifi.connectState() == ConnectState::Connected);
  const bool isConnecting = (ctx.wifi.connectState() == ConnectState::Connecting);
  t.fillCircle(16, 43, 4, isConn ? Colors::Green : (isConnecting ? Colors::Amber : Colors::Muted));
  if (isConn) {
    char statusBuf[48];
    snprintf(statusBuf, sizeof(statusBuf), "Connected: %s (%d dBm)",
             ctx.wifi.connectedSSID(), ctx.wifi.currentRSSI());
    screen.labelPadded(26, 39, statusBuf, 36, Colors::Green, 1, Colors::Bg);
  } else if (isConnecting) {
    char statusBuf[48];
    snprintf(statusBuf, sizeof(statusBuf), "Connecting to %s...", ctx.wifi.targetSSID());
    screen.labelPadded(26, 39, statusBuf, 36, Colors::Amber, 1, Colors::Bg);
  } else {
    screen.labelPadded(26, 39, "Disconnected", 36, Colors::Muted, 1, Colors::Bg);
  }
}

void WiFiApp::renderSaved(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();
  screen.clear();
  screen.header("Saved Networks", true);

  const uint8_t count = ctx.wifi.savedCount();
  if (count == 0) {
    t.fillRoundRect(10, 46, 300, 94, 6, Colors::Panel);
    t.drawRoundRect(10, 46, 300, 94, 6, Colors::Border);
    screen.label(28, 62, "No saved networks yet", Colors::White, 2, Colors::Panel);
    screen.label(28, 92, "Connect to any network in Scan Networks", Colors::Muted, 1, Colors::Panel);
    screen.label(28, 110, "and it will be saved here automatically.", Colors::Muted, 1, Colors::Panel);
    screen.footer("GPIO14 = Back  |  < Back to return");
    return;
  }

  for (uint8_t i = 0; i < 3; ++i) {
    const uint8_t idx = savedOffset_ + i;
    if (idx >= count) break;
    const SavedNetwork* net = ctx.wifi.savedNetwork(idx);
    if (!net) break;

    const int y = 40 + i * 36;
    const bool isCurrent = (ctx.wifi.connectState() == ConnectState::Connected &&
                            strcmp(ctx.wifi.connectedSSID(), net->ssid) == 0);

    t.fillRoundRect(8, y, 304, 32, 5, Colors::Panel);
    t.drawRoundRect(8, y, 304, 32, 5, isCurrent ? Colors::Green : Colors::Border);

    char displaySsid[18];
    strncpy(displaySsid, net->ssid, sizeof(displaySsid) - 1);
    displaySsid[sizeof(displaySsid) - 1] = '\0';
    screen.label(16, y + 10, displaySsid, Colors::White, 1, Colors::Panel);

    if (isCurrent) {
      t.fillRoundRect(140, y + 5, 58, 22, 3, Colors::Green);
      screen.label(147, y + 11, "ACTIVE", Colors::Bg, 1, Colors::Green);

      t.fillRoundRect(204, y + 5, 100, 22, 3, Colors::Amber);
      screen.label(216, y + 11, "DISCONNECT", Colors::Bg, 1, Colors::Amber);
    } else {
      t.fillRoundRect(140, y + 5, 78, 22, 3, Colors::Cyan);
      screen.label(154, y + 11, "CONNECT", Colors::Bg, 1, Colors::Cyan);

      t.fillRoundRect(224, y + 5, 80, 22, 3, Colors::Red);
      screen.label(240, y + 11, "FORGET", Colors::White, 1, Colors::Red);
    }
  }

  screen.footer("Tap Connect / Forget  |  GPIO14 = Back");
}
