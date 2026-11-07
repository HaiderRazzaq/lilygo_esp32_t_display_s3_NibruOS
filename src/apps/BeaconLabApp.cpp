#include "apps/BeaconLabApp.h"
#include "core/AppContext.h"
#include "core/ResourceManager.h"
#include "hardware/DisplayManager.h"
#include "services/WiFiService.h"
#include <Arduino.h>
#include <cstdio>

void BeaconLabApp::onEnter(AppContext&) {
  status_ = "";
  invalidate();
}

void BeaconLabApp::onExit(AppContext& ctx) {
  if (!ctx.resources.ownsWiFi(ResourceOwner::BeaconLab)) return;
  ctx.wifi.stopBeaconLab();
  ctx.resources.releaseWiFi(ResourceOwner::BeaconLab);
  status_ = "Stopped";
  invalidate();
}

void BeaconLabApp::onTick(AppContext& ctx, uint32_t nowMs) {
  if (!ctx.resources.ownsWiFi(ResourceOwner::BeaconLab)) return;
  const auto accepted = ctx.wifi.beaconAcceptedNames();
  ctx.wifi.tickBeaconLab(nowMs);
  if (accepted != ctx.wifi.beaconAcceptedNames() &&
      ctx.wifi.beaconAcceptedNames() == openCount_ + securedCount_) invalidate();
  if (!ctx.wifi.beaconActive()) {
    ctx.resources.releaseWiFi(ResourceOwner::BeaconLab);
    status_ = "Session ended";
    invalidate();
  }
}

void BeaconLabApp::forceStart(AppContext& ctx) {
  if (ctx.resources.ownsWiFi(ResourceOwner::BeaconLab)) {
    onExit(ctx);
    return;
  }
  if (ctx.wifi.isConnected() || ctx.wifi.connectState() == ConnectState::Connecting) {
    ctx.wifi.disconnect();
  }
  ctx.wifi.stopScan();

  if (ctx.resources.wifiOwner() != ResourceOwner::None &&
      !ctx.resources.ownsWiFi(ResourceOwner::BeaconLab)) {
    ctx.resources.releaseWiFi(ctx.resources.wifiOwner());
  }

  if (!ctx.resources.acquireWiFi(ResourceOwner::BeaconLab)) {
    status_ = "Radio busy";
  } else if (!ctx.wifi.startBeaconLab(openCount_, securedCount_, durationSeconds_, millis())) {
    ctx.resources.releaseWiFi(ResourceOwner::BeaconLab);
    status_ = "Wi-Fi busy or unavailable";
  } else {
    status_ = "Broadcasting (Wi-Fi disconnected)";
  }
  invalidate();
}

void BeaconLabApp::onActivate(AppContext& ctx) {
  if (ctx.wifi.isConnected()) {
    forceStart(ctx);
  } else {
    onPrimaryAction(ctx);
  }
}

void BeaconLabApp::onPrimaryAction(AppContext& ctx) {
  if (ctx.resources.ownsWiFi(ResourceOwner::BeaconLab)) {
    onExit(ctx);
    return;
  }
  if (ctx.wifi.isConnected()) {
    status_ = "Wi-Fi busy (Hold START to force)";
    invalidate();
    return;
  }
  if (!ctx.resources.acquireWiFi(ResourceOwner::BeaconLab)) {
    status_ = "Radio busy";
  } else if (!ctx.wifi.startBeaconLab(openCount_, securedCount_, durationSeconds_, millis())) {
    ctx.resources.releaseWiFi(ResourceOwner::BeaconLab);
    status_ = "Wi-Fi busy or unavailable";
  } else {
    status_ = "Broadcasting";
  }
  invalidate();
}

void BeaconLabApp::onTouch(AppContext& ctx, const TouchEvent& touch) {
  // 1. Long Press on START button: auto-disconnect Wi-Fi and force start
  if (touch.gesture == TouchGesture::LongPress &&
      touch.x >= 13 && touch.x < 307 && touch.y >= 129 && touch.y < 159) {
    forceStart(ctx);
    return;
  }

  // 2. Click on [DISCONNECT WI-FI] badge on status line
  if (touch.gesture == TouchGesture::Tap && ctx.wifi.isConnected() &&
      !ctx.wifi.beaconActive() && touch.y >= 156 && touch.y <= 170 &&
      touch.x >= 13 && touch.x <= 130) {
    ctx.wifi.disconnect();
    status_ = "Wi-Fi disconnected";
    invalidate();
    return;
  }

  // 3. Regular Tap handling
  if (touch.gesture != TouchGesture::Tap) return;
  if (touch.x < 13 || touch.x >= 307) return;
  if (touch.y >= 129 && touch.y < 159) {
    onPrimaryAction(ctx);
    return;
  }
  if (ctx.wifi.beaconActive() || touch.y < 38 || touch.y >= 122) return;
  const bool increase = touch.x >= 264;
  if (!increase && touch.x >= 57) return;
  const uint8_t total = openCount_ + securedCount_;
  if (touch.y >= 94) {
    const int value = durationSeconds_ + (increase ? 5 : -5);
    if (value >= WiFiService::kBeaconMinSeconds && value <= WiFiService::kBeaconMaxSeconds)
      durationSeconds_ = static_cast<uint8_t>(value);
  } else {
    auto& count = touch.y < 66 ? openCount_ : securedCount_;
    if (increase && total < WiFiService::kBeaconMaxCount) ++count;
    if (!increase && count > 0 && total > 1) --count;
  }
  invalidate();
}

void BeaconLabApp::render(AppContext& ctx) {
  auto& screen = ctx.display;
  auto& gfx = screen.gfx();
  const bool running = ctx.wifi.beaconActive();
  screen.clear();
  screen.header("Beacon Lab", true);
  const auto row = [&](int y, const char* name, uint8_t value) {
    char label[24];
    snprintf(label, sizeof(label), "%s: %u", name, static_cast<unsigned>(value));
    screen.label(88, y + 8, label, Colors::White, 1, Colors::Bg);
    if (running) return;
    gfx.fillRoundRect(13, y, 44, 24, 4, Colors::Panel);
    gfx.fillRoundRect(264, y, 43, 24, 4, Colors::Panel);
    screen.label(32, y + 8, "-", Colors::White, 1, Colors::Panel);
    screen.label(283, y + 8, "+", Colors::White, 1, Colors::Panel);
  };
  row(38, "Open", openCount_);
  row(66, "Secured", securedCount_);
  row(94, "Seconds", durationSeconds_);
  const auto color = running ? Colors::Red : Colors::Cyan;
  gfx.fillRoundRect(13, 129, 294, 27, 6, color);
  if (running) {
    screen.label(136, 138, "STOP", Colors::Bg, 1, color);
  } else if (ctx.wifi.isConnected()) {
    screen.label(70, 138, "START (HOLD: AUTO-DISC)", Colors::Bg, 1, color);
  } else {
    screen.label(136, 138, "START", Colors::Bg, 1, color);
  }

  if (ctx.wifi.isConnected() && !running) {
    gfx.fillRoundRect(13, 157, 108, 12, 2, Colors::Panel);
    gfx.drawRoundRect(13, 157, 108, 12, 2, Colors::Amber);
    screen.label(16, 159, "DISCONNECT WI-FI", Colors::Amber, 1, Colors::Panel);
  } else {
    screen.label(13, 160, status_, Colors::Muted, 1, Colors::Bg);
  }
  char accepted[24];
  snprintf(accepted, sizeof(accepted), "TX accepted: %u",
           static_cast<unsigned>(ctx.wifi.beaconAcceptedNames()));
  screen.label(197, 160, accepted, Colors::Muted, 1, Colors::Bg);
}
