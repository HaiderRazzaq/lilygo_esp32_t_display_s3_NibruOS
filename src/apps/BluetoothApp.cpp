#include "apps/BluetoothApp.h"
#include "core/AppContext.h"
#include "core/ResourceManager.h"
#include "hardware/DisplayManager.h"
#include "services/LocalizationService.h"
#include <Arduino.h>
#include <cstdio>

const char* BluetoothApp::title() const {
  return LocalizationService::instance().isArabic() ? "بلوتوث" : "Bluetooth";
}

const char* BluetoothApp::subtitle() const {
  return LocalizationService::instance().isArabic() ? "لوحة مفاتيح" : "Keyboard";
}

void BluetoothApp::onEnter(AppContext&) {
  feedback_ = nullptr;
  invalidate();
}

void BluetoothApp::onExit(AppContext&) {
  // The shared keyboard stays enabled so other apps can send HID reports.
  feedback_ = nullptr;
}

void BluetoothApp::onTick(AppContext& ctx, uint32_t nowMs) {
  if (drawnState_ != ctx.bleHid.state() ||
      drawnPairingCode_ != ctx.bleHid.pairingCode()) invalidate();
  if (feedback_ && nowMs - feedbackSinceMs_ >= 2500) {
    feedback_ = nullptr;
    invalidate();
  }
}

void BluetoothApp::toggle(AppContext& ctx) {
  const bool enable = !ctx.bleHid.isEnabled();
  if (ctx.bleHid.setEnabled(enable, ctx)) {
    feedback_ = enable ? "BLE Keyboard Advertising..." : "BLE Keyboard Disabled";
  } else {
    feedback_ = ctx.resources.wifiOwner() == ResourceOwner::None
        ? "BLE start failed (see Serial)" : "Radio busy";
  }
  feedbackSinceMs_ = millis();
  invalidate();
}

void BluetoothApp::testKeyboard(AppContext& ctx) {
  feedback_ = ctx.bleHid.typeText("NibruOS-123456\n")
      ? "Test keys sent" : "BLE keyboard not ready";
  feedbackSinceMs_ = millis();
  invalidate();
}

void BluetoothApp::onPrimaryAction(AppContext& ctx) {
  if (ctx.bleHid.isConnected()) testKeyboard(ctx);
  else toggle(ctx);
}

void BluetoothApp::onTouch(AppContext& ctx, const TouchEvent& ev) {
  if ((ev.gesture != TouchGesture::Tap && ev.gesture != TouchGesture::DoubleTap) ||
      ev.x < 10 || ev.x > 310 || ev.y < 98 || ev.y > 150) return;
  if (drawnState_ == BleConnectionState::Connected) {
    if (ev.x <= 154) testKeyboard(ctx);
    else if (ev.x >= 162 && ctx.bleHid.isEnabled()) toggle(ctx);
  } else if (ctx.bleHid.isEnabled() == (drawnState_ != BleConnectionState::Disabled)) {
    toggle(ctx);
  }
}

void BluetoothApp::render(AppContext& ctx) {
  auto& screen = ctx.display;
  auto& t = screen.gfx();
  const auto& loc = LocalizationService::instance();
  const bool ar = loc.isArabic();
  drawnState_ = ctx.bleHid.state();
  drawnPairingCode_ = ctx.bleHid.pairingCode();
  const bool enabled = drawnState_ != BleConnectionState::Disabled;
  const bool ready = drawnState_ == BleConnectionState::Connected;

  screen.clear();
  screen.header(loc.get(TextId::MenuBluetooth), true);
  t.fillRoundRect(10, 36, 300, 62, 6, Colors::Panel);
  t.drawRoundRect(10, 36, 300, 62, 6, ready ? Colors::Green : Colors::Border);
  screen.label(20, 46, ctx.bleHid.deviceName(), Colors::White, 1, Colors::Panel);

  const char* status = ar ? "البلوتوث معطل" : "BLUETOOTH OFF";
  if (drawnState_ == BleConnectionState::Advertising)
    status = ar ? "في انتظار الاقتران" : "WAITING FOR PAIRING";
  else if (drawnState_ == BleConnectionState::Linking)
    status = ar ? "جار إعداد لوحة المفاتيح" : "SETTING UP KEYBOARD";
  else if (ready)
    status = ar ? "لوحة المفاتيح جاهزة" : "KEYBOARD READY";
  if (drawnState_ == BleConnectionState::Linking &&
      drawnPairingCode_ != BleHidService::kNoPairingCode) {
    char code[8];
    snprintf(code, sizeof(code), "%06lu", static_cast<unsigned long>(drawnPairingCode_));
    screen.label(20, 64, ar ? "أدخل الرمز على الجهاز الآخر" : "Enter this code on your host",
                 Colors::White, 1, Colors::Panel);
    screen.label(20, 77, code, Colors::Cyan, 2, Colors::Panel);
  } else {
    screen.label(20, 69, status, ready ? Colors::Green : Colors::Muted, 1, Colors::Panel);
  }

  if (ready) {
    t.fillRoundRect(10, 110, 144, 32, 6, Colors::Green);
    screen.label(26, 120, "TEST KEYBOARD", Colors::Bg, 1, Colors::Green);
  }
  const int x = ready ? 162 : 10;
  const uint16_t color = enabled ? Colors::Red : Colors::Cyan;
  t.fillRoundRect(x, 110, 310 - x, 32, 6, color);
  screen.label(ready ? 194 : 75, 120, enabled
      ? (ar ? "إيقاف البلوتوث" : "DISABLE BLE")
      : (ar ? "تفعيل لوحة المفاتيح" : "ENABLE BLE KEYBOARD"),
      enabled ? Colors::White : Colors::Bg, 1, color);
  screen.footer(feedback_ ? feedback_ : "Back | Tap controls");
}
