#include "services/AccountImportService.h"
#include <Arduino.h>
#include "core/AppContext.h"
#include "core/AppManager.h"
#include "core/ResourceManager.h"
#include "core/MemoryProfiler.h"
#include "core/WakeInputGate.h"
#include "hardware/DisplayManager.h"
#include "hardware/TouchManager.h"
#include "hardware/ButtonManager.h"
#include "services/WiFiService.h"
#include "services/SecurityService.h"
#include "services/SettingsService.h"
#include "services/TimeService.h"
#include "services/VaultService.h"
#include "services/TotpService.h"
#include "services/UsbHidService.h"
#include "services/WebPortalService.h"
#include "services/BleHidService.h"
#include "services/BleAdvertisingService.h"
#include "apps/BleLabApp.h"
#include "apps/WiFiApp.h"
#include "apps/WiFiChannelsApp.h"
#include "apps/BluetoothApp.h"
#include "apps/BeaconLabApp.h"
#include "apps/TotpApp.h"
#include "apps/VaultApp.h"
#include "apps/SettingsApp.h"
#include "apps/GeneratorApp.h"
#include "apps/ClockApp.h"
#include "services/WifiDefenseService.h"
#include "apps/WifiDefenseApp.h"
#include "apps/SecurityLabApp.h"

namespace {
DisplayManager display;
TouchManager touch;
ButtonManager buttons;
ResourceManager resources;
WiFiService wifi;
SecurityService security;
SettingsService settings;
TimeService timeService;
TotpService totp;
UsbHidService usbHid;
AccountImportService accountImport;
VaultService vault;
WebPortalService webPortal;
BleHidService bleHid;
WifiDefenseService wifiDefense;
BleAdvertisingService bleAdvertising;
AppContext context{display, resources, wifi, security, settings, timeService, totp, usbHid, accountImport, vault, webPortal, bleHid, wifiDefense, &bleAdvertising};
AppManager launcher(context);
WiFiApp wifiApp;
WiFiChannelsApp wifiChannelsApp;
BleLabApp bleLabApp;
BluetoothApp bluetoothApp;
WifiDefenseApp wifiDefenseApp;
BeaconLabApp beaconLabApp;
SecurityLabApp securityLabApp(bleLabApp, beaconLabApp, wifiDefenseApp, wifiChannelsApp);
TotpApp totpApp;
VaultApp vaultApp;
GeneratorApp generatorApp;
ClockApp clockApp;
SettingsApp settingsApp;
WakeInputGate wakeInput;
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n[NibruOS] boot v0.23.6");
  MemoryProfiler::checkpoint("startup-basic");
  display.begin();
  touch.begin();
  settings.begin(&display, &touch);
  MemoryProfiler::checkpoint("display-touch-ready");
  buttons.begin();
  wifi.begin(millis());
  security.setCredentialChangeGuard(TotpService::canChangeCredentials);
  security.begin();
  timeService.begin();
  timeService.setTimezoneOffset(settings.timezoneOffset());
  totp.begin();
  vault.begin();
  usbHid.begin();
  bleHid.begin();
  wifiDefense.begin();
  launcher.registerApp(wifiApp);
  launcher.registerApp(bluetoothApp);
  launcher.registerApp(securityLabApp);
  launcher.registerApp(totpApp);
  launcher.registerApp(vaultApp);
  launcher.registerApp(generatorApp);
  launcher.registerApp(clockApp);
  launcher.registerApp(settingsApp);
  launcher.begin();
  launcher.tick(millis());
  MemoryProfiler::checkpoint("home-services-ready");

  if (security.isPinEnabled()) {
    security.lock();
  }
}

void loop() {
#if NIBRU_PROFILE
  const uint32_t loopStart = micros();
#endif
  uint32_t now = millis();
  TouchEvent ev{};
  const bool hasTouch = touch.poll(ev);
  const ButtonAction action = buttons.poll(now);

  static bool wasScreensaver = false;
  if (!wasScreensaver && display.isScreensaverActive()) {
    clockApp.resetScreensaver();
  }
  wasScreensaver = display.isScreensaverActive();

  if (display.isSleeping()) {
    if (hasTouch || touch.isPressed() || action != ButtonAction::None) {
      display.wake();
      wakeInput.arm(now);
      if (security.isPinEnabled()) {
        security.lock();
        security.invalidateFull();
      } else {
        launcher.invalidate();
      }
    }
  } else if (display.isScreensaverActive()) {
    if (hasTouch || touch.isPressed() || action != ButtonAction::None) {
      display.stopScreensaver();
      wakeInput.arm(now);
      if (security.isPinEnabled() && !webPortal.isActive()) {
        security.lock();
        security.invalidateFull();
      } else {
        launcher.invalidate();
      }
    } else {
      clockApp.renderScreensaver(context, now);
    }
  } else if (wakeInput.consume(hasTouch, action != ButtonAction::None,
                               buttons.anyPressed() || touch.isPressed(), now)) {
    display.notifyActivity(now);
  } else if (security.isLocked()) {
    if (hasTouch) {
      display.notifyActivity(now);
      security.handleTouch(context, ev);
    }
    if (action != ButtonAction::None) {
      display.notifyActivity(now);
    }
  } else {
    if (hasTouch) {
      display.notifyActivity(now);
      launcher.handleTouch(ev);
    }
    if (action != ButtonAction::None) {
      display.notifyActivity(now);
      launcher.handleButton(action);
    }
  }

  static bool wasLocked = false;
  if (wasLocked && !security.isLocked()) {
    launcher.invalidate();
  }
  wasLocked = security.isLocked();

  now = millis();
  display.tick(now);
  if (display.isSleeping() && security.isPinEnabled() && !security.isLocked() && !webPortal.isActive()) {
    security.lock();
  }
  if (display.isSleeping() || security.isLocked()) {
    if (totp.isLoaded()) totpApp.clearSession(context);
    if (vault.isLoaded()) vaultApp.clearSession(context);
    usbHid.cancel();
  }
  if (display.isSleeping() || display.isScreensaverActive() || security.isLocked()) {
    securityLabApp.onExit(context);
    bleAdvertising.stop(now, context);
  }
  bleAdvertising.tick(now, context);
  wifi.tick(now);
  while (Serial.available() > 0) {
    char c = static_cast<char>(Serial.read());
    if (timeService.processSerialInput(c)) {
      launcher.invalidate();
    }
  }
  timeService.tick(now, wifi.isConnected());
  if (!display.isSleeping() && !display.isScreensaverActive()) {
    if (security.isLocked()) {
      security.tick(now);
      if (security.consumeDirty()) {
        security.render(context);
      }
    } else {
      launcher.tick(now);
    }
  }
  usbHid.tick(now);
  webPortal.tick(now, context);

#if NIBRU_PROFILE
  MemoryProfiler::observeLoop(micros() - loopStart);
#endif
  delay(12);
}
