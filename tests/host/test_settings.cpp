#include "services/SettingsService.h"
#include "hardware/DisplayManager.h"
#include "core/AppManager.h"
#include "core/ResourceManager.h"
#include "services/WiFiService.h"
#include "apps/SettingsApp.h"
#include "core/WakeInputGate.h"
#include "hardware/TouchManager.h"
#include "services/SecurityService.h"
#include "services/LocalizationService.h"
#include <Preferences.h>
#include <cassert>

extern uint32_t fakeMs;
extern bool fakeTouchPending;
extern TouchData fakeTouchData;

void testSettings(AppContext& ctx) {
  WakeInputGate gate;
  gate.arm(0xfffffff0U);
  assert(gate.consume(true, false, false, 20));
  assert(gate.consume(false, true, true, 800)); // long press after wake
  assert(gate.consume(false, true, false, 900)); // release is consumed
  assert(gate.consume(false, false, false, 1300));
  assert(!gate.consume(true, false, false, 1301));
  Preferences::resetMock();
  SettingsService s;
  s.begin(&ctx.display);
  assert(s.brightness() == 100 && s.screenTimeoutSec() == 30 && s.theme() == 0);
  const unsigned initialWrites = Preferences::writes;
  assert(s.setBrightness(100));
  assert(Preferences::writes == initialWrites);
  assert(!s.setBrightness(0) && !s.setBrightness(99));
  assert(!s.setScreenTimeoutSec(31) && !s.setTheme(4) && !s.setRotation(0) && !s.setRotation(2) && !s.setLanguage(2));
  assert(s.setBrightness(25) && s.setScreenTimeoutSec(0) && s.setTheme(3) && s.setRotation(3) && s.setLanguage(1));
  assert(LocalizationService::instance().isArabic());
  assert(ctx.display.rotation() == 3);
  SettingsService restored;
  restored.begin();
  assert(restored.brightness() == 25 && restored.screenTimeoutSec() == 0 && restored.theme() == 3 && restored.rotation() == 3 && restored.language() == 1);
  // Failure must be observable, with no runtime configuration change.
  Preferences::failWrite = true;
  assert(!s.setBrightness(50));
  assert(s.brightness() == 25 && ctx.display.brightness() == 25);
  Preferences::failWrite = false;
  Preferences::failOpen = true;
  assert(!s.setTheme(1));
  assert(s.theme() == 3);
  Preferences::failOpen = false;
  Preferences::failOpen = true;
  restored.load();
  assert(!restored.lastSaveOk());
  Preferences::failOpen = false;
  assert(!restored.setTheme(1)); // failed load must not overwrite an unread record

  Preferences p;
  p.begin("nibru_sec"); p.putString("test", "sentinel"); p.end();
  p.begin("nibru_wifi"); p.putString("test", "sentinel"); p.end();
  p.begin("nibru_totp"); p.putString("test", "sentinel"); p.end();
  assert(s.resetToDefaults());
  for (const char* ns : {"nibru_sec", "nibru_wifi", "nibru_totp"}) {
    p.begin(ns, true); assert(p.getString("test").length() == 8); p.end();
  }
  restored.load();
  assert(restored.brightness() == 100 && restored.screenTimeoutSec() == 30 && restored.theme() == 0 && restored.rotation() == 1 && restored.language() == 0);

  // Legacy values are validated, imported once, and never removed.
  Preferences::resetMock();
  p.begin("nibru_cfg"); p.putUChar("bright", 75); p.putUShort("sleep_sec", 300); p.end();
  p.begin("nibru_settings"); p.end(); // empty destination must not shadow legacy
  s.load(); assert(s.brightness() == 75 && s.screenTimeoutSec() == 300);
  const unsigned migratedWrites = Preferences::writes;
  p.begin("nibru_cfg"); p.putUChar("bright", 25); p.end();
  s.load(); assert(s.brightness() == 75);
  assert(Preferences::writes == migratedWrites + 1);
  // An older nibru_settings key layout also migrates, despite namespace presence.
  Preferences::resetMock();
  p.begin("nibru_settings"); p.putUChar("bright", 50); p.putUShort("timeout", 180); p.putUChar("theme", 2); p.end();
  s.load(); assert(s.brightness() == 50 && s.screenTimeoutSec() == 180 && s.theme() == 2);
  // Invalid/corrupt current blob falls back visibly; no unsupported schema overwrite.
  const uint8_t future[] = {2, 25, 0, 0, 1};
  p.begin("nibru_settings"); p.putBytes("config", future, sizeof(future)); p.end();
  s.load(); assert(s.brightness() == 100 && s.theme() == 0);
  assert(!s.setTheme(1));
  assert(s.resetToDefaults()); // explicit confirmation may replace unsupported preferences
  Preferences::resetMock();
  ctx.settings.begin(&ctx.display);

  SettingsApp app;
  AppManager manager(ctx);
  manager.registerApp(app);
  manager.begin();
  manager.open(0);
  manager.handleButton(ButtonAction::Activate); // Display
  manager.tick(fakeMs);
  const unsigned displayClears = ctx.display.gfx().fullClears;
  manager.handleButton(ButtonAction::Activate); // 25%
  manager.tick(fakeMs);
  assert(ctx.display.gfx().fullClears == displayClears); // no full-screen wipe for value change
  assert(ctx.settings.brightness() == 25);
  manager.handleButton(ButtonAction::SelectNext);
  manager.handleButton(ButtonAction::Activate);
  assert(ctx.settings.brightness() == 50);
  manager.tick(fakeMs);
  const unsigned unchangedDraws = ctx.display.gfx().drawCalls;
  manager.handleTouch({110, 58, 0, 0, TouchGesture::Tap, 700}); // already 50%
  manager.tick(fakeMs);
  assert(ctx.display.gfx().drawCalls == unchangedDraws); // selecting same value is a no-op
  manager.handleTouch({180, 120, 0, 0, TouchGesture::Tap, 750}); // Invert (180)
  assert(ctx.settings.rotation() == 3);
  manager.handleTouch({50, 120, 0, 0, TouchGesture::Tap, 760}); // Normal (0)
  assert(ctx.settings.rotation() == 1);
  ctx.security.setPin("4875");
  ctx.security.setPinEnabled(true);
  ctx.security.lock();
  ctx.security.render(ctx); // another renderer replaced the physical display
  const unsigned lockClears = ctx.display.gfx().fullClears;
  ctx.security.unlock();
  manager.invalidate(); // exactly the shell's unlock restoration path
  manager.tick(fakeMs);
  assert(ctx.display.gfx().fullClears == lockClears + 1);
  manager.handleButton(ButtonAction::Back);
  manager.handleButton(ButtonAction::SelectNext); // Theme
  manager.handleButton(ButtonAction::Activate);
  manager.handleButton(ButtonAction::SelectNext);
  manager.handleButton(ButtonAction::Activate);
  assert(ctx.settings.theme() == 1);
  manager.tick(fakeMs);
  const unsigned themeClears = ctx.display.gfx().fullClears;
  manager.handleButton(ButtonAction::SelectNext);
  manager.handleButton(ButtonAction::Activate);
  manager.tick(fakeMs);
  assert(ctx.display.gfx().fullClears == themeClears);
  manager.handleButton(ButtonAction::Back);
  // Scroll through menu to Device, enter confirmation. Cancel is focused first.
  for (int i = 0; i < 3; ++i) manager.handleButton(ButtonAction::SelectNext);
  manager.handleButton(ButtonAction::Activate);
  manager.handleButton(ButtonAction::SelectNext); // Reset
  manager.handleButton(ButtonAction::Activate);
  manager.handleButton(ButtonAction::Activate); // Cancel
  assert(ctx.settings.brightness() == 50);
  manager.handleButton(ButtonAction::Activate); // Reset modal again
  manager.handleButton(ButtonAction::SelectNext);
  manager.handleButton(ButtonAction::Activate); // Confirm
  assert(ctx.settings.brightness() == 100 && ctx.settings.theme() == 0);
  manager.handleButton(ButtonAction::Back);
  manager.handleButton(ButtonAction::Back);
  assert(manager.isHome());
  for (int i = 0; i < 20; ++i) { manager.open(0); manager.tick(fakeMs); manager.goHome(); }
  assert(ctx.resources.wifiOwner() == ResourceOwner::None);

  // Real touch path may emit initial NONE as Tap, before DoubleTap classification.
  manager.open(0);
  for (int i = 0; i < 4; ++i) manager.handleTouch({200, 80, 0, 0, TouchGesture::SwipeUp});
  manager.handleTouch({100, 45, 0, 0, TouchGesture::Tap}); // Device
  ctx.settings.setBrightness(25);
  fakeMs = 10000;
  manager.handleTouch({200, 110, 0, 0, TouchGesture::Tap, 55, 10000}); // opens reset
  fakeMs += 180;
  manager.handleTouch({200, 110, 0, 0, TouchGesture::Tap, 56, 10180}); // raw double tap
  assert(ctx.settings.brightness() == 25);
  fakeMs += 700;
  manager.handleTouch({200, 110, 0, 0, TouchGesture::Tap, 56, 10180}); // held second contact, begun too early
  assert(ctx.settings.brightness() == 25);
  manager.handleTouch({200, 110, 0, 0, TouchGesture::Tap, 55, 10000}); // same held contact
  assert(ctx.settings.brightness() == 25);
  manager.handleTouch({200, 110, 0, 0, TouchGesture::Tap, 57, 10880}); // deliberate new contact
  assert(ctx.settings.brightness() == 100);
  // Raw Down/NONE is withheld: confirmation timing must use contact start,
  // not the later classified/release report.
  TouchManager rawTouch;
  rawTouch.begin();
  auto rawFrame = [&](uint32_t at, byte gesture, byte points, byte phase) {
    fakeMs = at;
    fakeTouchData = TouchData{gesture, points, phase, 59, 200}; // screen(200,110)
    fakeTouchPending = true;
    TouchEvent event{};
    if (rawTouch.poll(event)) manager.handleTouch(event);
  };
  ctx.settings.setBrightness(25);
  rawFrame(20000, NONE, 1, 0);
  rawFrame(20100, SINGLE_CLICK, 0, 1); // opens reset
  rawFrame(20300, NONE, 1, 0); // second contact begins inside rejection window
  rawFrame(20700, SINGLE_CLICK, 0, 1); // delivered late, must still be rejected
  assert(ctx.settings.brightness() == 25);
  rawFrame(20800, NONE, 1, 0);
  rawFrame(20900, SINGLE_CLICK, 0, 1);
  assert(ctx.settings.brightness() == 100);

  TouchManager rotTouch;
  rotTouch.begin();
  assert(rotTouch.rotation() == 1);
  rotTouch.setRotation(3);
  assert(rotTouch.rotation() == 3);
  fakeMs = 30000;
  fakeTouchData = TouchData{NONE, 1, 0, 59, 200};
  fakeTouchPending = true;
  TouchEvent rotEv{};
  assert(!rotTouch.poll(rotEv)); // Down frame stores contact, withholds semantic event
  fakeTouchData = TouchData{SINGLE_CLICK, 0, 1, 59, 200};
  fakeTouchPending = true;
  assert(rotTouch.poll(rotEv));
  assert(rotEv.x == 119 && rotEv.y == 59 && rotEv.gesture == TouchGesture::Tap);
  fakeTouchData = TouchData{NONE, 1, 0, 59, 200};
  fakeTouchPending = true;
  assert(!rotTouch.poll(rotEv));
  fakeTouchData = TouchData{SWIPE_UP, 0, 1, 59, 200};
  fakeTouchPending = true;
  assert(rotTouch.poll(rotEv));
  assert(rotEv.gesture == TouchGesture::SwipeRight);
  manager.goHome(); manager.open(0);
  for (int i = 0; i < 4; ++i) manager.handleButton(ButtonAction::SelectNext);
  manager.handleButton(ButtonAction::Activate); // Device
  manager.handleButton(ButtonAction::Activate); // Reboot confirmation
  const unsigned restarts = ESP.restarts;
  manager.handleButton(ButtonAction::Activate); // Cancel
  assert(ESP.restarts == restarts);
  assert(ctx.resources.acquireWiFi(ResourceOwner::WiFiScanner));
  ctx.wifi.startScan(fakeMs);
  manager.handleButton(ButtonAction::Activate);
  manager.handleButton(ButtonAction::SelectNext);
  manager.handleButton(ButtonAction::Activate); // Confirm reboot
  assert(ESP.restarts == restarts + 1);
  assert(ctx.resources.wifiOwner() == ResourceOwner::None);
  assert(ctx.wifi.state() == ScanState::Idle);
  manager.goHome();

  ctx.display.setSleepTimeoutSeconds(30);
  fakeMs = 0xfffffff0U;
  ctx.display.notifyActivity(fakeMs);
  ctx.display.tick(fakeMs + 29999U); assert(!ctx.display.isSleeping());
  ctx.display.tick(fakeMs + 30000U); assert(ctx.display.isSleeping());
  ctx.display.wake(); assert(!ctx.display.isSleeping());
  ctx.display.setSleepTimeoutSeconds(0);
  ctx.display.tick(fakeMs + 900000U); assert(!ctx.display.isSleeping());
#if NIBRU_BACKLIGHT_LEDC
  fakePwmOk = true;
  DisplayManager pwm;
  pwm.begin();
  pwm.setBrightness(25); assert(fakeDuty == 64);
  pwm.setBrightness(50); assert(fakeDuty == 128);
  pwm.setBrightness(75); assert(fakeDuty == 191);
  pwm.setBrightness(100); assert(fakeDuty == 255);
  pwm.sleep(); assert(fakeDuty == 0);
  pwm.wake(); assert(fakeDuty == 255);
  fakePwmOk = false;
  DisplayManager fallback;
  fallback.begin(); fallback.setBrightness(25);
  assert(fakePins[38] == HIGH && fakePins[15] == HIGH);
  fallback.sleep(); assert(fakePins[38] == LOW);
  fallback.wake(); assert(fakePins[38] == HIGH);
  fakePwmOk = true;
#endif
  Preferences::resetMock();
}
