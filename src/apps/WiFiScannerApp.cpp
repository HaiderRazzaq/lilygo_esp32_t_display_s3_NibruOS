#include "apps/WiFiScannerApp.h"
#include "core/AppContext.h"
#include "BoardConfig.h"
#include "core/ResourceManager.h"
#include "hardware/DisplayManager.h"
#include <Arduino.h>
#include <cstdio>
#include <cstring>

void WiFiScannerApp::onEnter(AppContext& ctx) {
  offset_ = 0;
  acquired_ = ctx.resources.acquireWiFi(ResourceOwner::WiFiScanner);
  if (acquired_) ctx.wifi.startScan(millis());
  lastState_ = ctx.wifi.state();
  lastAnimMs_ = millis();
  invalidate();
}
void WiFiScannerApp::onExit(AppContext& ctx) {
  if (acquired_) {
    ctx.wifi.shutdown();
    ctx.resources.releaseWiFi(ResourceOwner::WiFiScanner);
  }
  acquired_ = false;
}
void WiFiScannerApp::refresh(AppContext& ctx) {
  if (!acquired_ || ctx.wifi.state() == ScanState::Scanning ||
      ctx.wifi.state() == ScanState::Preparing) return;
  offset_ = 0;
  ctx.wifi.startScan(millis());
  invalidate();
}
void WiFiScannerApp::onPrimaryAction(AppContext& ctx) { refresh(ctx); }
void WiFiScannerApp::onTick(AppContext& ctx, uint32_t now) {
  if (!acquired_) return;
  ctx.wifi.tick(now);
  if (ctx.wifi.state() != lastState_) {
    lastState_ = ctx.wifi.state();
    invalidate();
  }
  if ((ctx.wifi.state() == ScanState::Scanning ||
       ctx.wifi.state() == ScanState::Preparing) && now - lastAnimMs_ > 700) {
    lastAnimMs_ = now;
    invalidate();
  }
}
void WiFiScannerApp::onTouch(AppContext& ctx, const TouchEvent& ev) {
  if (ev.gesture == TouchGesture::SwipeUp) {
    if (offset_ + 3 < ctx.wifi.count()) { ++offset_; invalidate(); }
  } else if (ev.gesture == TouchGesture::SwipeDown) {
    if (offset_ > 0) { --offset_; invalidate(); }
  } else if ((ev.gesture == TouchGesture::Tap || ev.gesture == TouchGesture::DoubleTap)
             && ev.y >= 36 && ev.y <= 64 && ev.x >= 215) {
    refresh(ctx);
  } else if (ev.gesture == TouchGesture::Tap && ev.y > 120 && ev.x > 225) {
    if (offset_ + 3 < ctx.wifi.count()) { ++offset_; invalidate(); }
  }
}
void WiFiScannerApp::render(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();
  screen.clear();
  screen.header(title(), true);
  t.fillRoundRect(223, 37, 90, 25, 5, Colors::Cyan);
  screen.label(242, 45, "RESCAN", Colors::Bg, 1, Colors::Cyan);
  if (!acquired_) {
    screen.label(10, 76, "Wi-Fi resource busy", Colors::Red, 2);
    screen.footer("GPIO14 = Home");
    return;
  }
  const ScanState state = ctx.wifi.state();
  if (state == ScanState::Preparing) {
    char line[42];
    snprintf(line, sizeof(line), "Starting radio... %u/%u",
             ctx.wifi.attempt(), Board::kScanMaxAttempts);
    screen.label(12, 78, line, Colors::Cyan, 2);
    screen.label(12, 110, "Preparing Wi-Fi station", Colors::Muted);
  } else if (state == ScanState::Scanning) {
    char line[42];
    snprintf(line, sizeof(line), "Scanning %u/%u... %lu s",
             ctx.wifi.attempt(), Board::kScanMaxAttempts,
             static_cast<unsigned long>((millis() - ctx.wifi.scanStartedAt()) / 1000UL));
    screen.label(12, 78, line, Colors::Cyan, 2);
    screen.label(12, 112, "UI remains responsive", Colors::Muted);
  } else if (state == ScanState::Failed) {
    screen.label(12, 77, "Wi-Fi scan failed", Colors::Red, 2);
    screen.label(12, 105, ctx.wifi.lastError(), Colors::Muted);
    screen.label(12, 124, "RESCAN | See Serial 115200", Colors::Muted);
  } else if (state == ScanState::Complete) {
    char countText[32];
    snprintf(countText, sizeof(countText), "Found: %u (max 12)", ctx.wifi.count());
    screen.label(9, 45, countText, Colors::Muted);
    if (ctx.wifi.count() == 0) {
      screen.label(12, 89, "No networks found", Colors::Amber, 2);
    } else {
      for (uint8_t row = 0; row < 3; ++row) {
        const NetworkRecord* net = ctx.wifi.network(offset_ + row);
        if (!net) break;
        const int y = 69 + row * 25;
        char line[55];
        char shortSsid[20];
        snprintf(shortSsid, sizeof(shortSsid), "%.18s", net->ssid[0] ? net->ssid : "(hidden)");
        snprintf(line, sizeof(line), "%-19s %4d dBm", shortSsid, net->rssi);
        screen.label(10, y, line, Colors::White);
        snprintf(line, sizeof(line), "Ch:%d %s", net->channel, net->secured ? "Secure" : "Open");
        screen.label(24, y + 11, line, Colors::Muted);
      }
    }
  } else {
    screen.label(10, 80, "Tap RESCAN to start", Colors::Muted);
  }
  screen.footer("Swipe up/down to scroll | GPIO14 Back");
}
