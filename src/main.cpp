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
#include "apps/WiFiApp.h"
#include "apps/TotpApp.h"
#include "apps/VaultApp.h"
#include "apps/SettingsApp.h"

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
AppContext context{display, resources, wifi, security, settings, timeService, totp, usbHid, accountImport, vault, webPortal};
AppManager launcher(context);
WiFiApp wifiApp;
TotpApp totpApp;
VaultApp vaultApp;
SettingsApp settingsApp;
WakeInputGate wakeInput;
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n[NibruOS] boot v0.9.1");
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
  launcher.registerApp(wifiApp);
  launcher.registerApp(totpApp);
  launcher.registerApp(vaultApp);
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
  if (display.isSleeping() && security.isPinEnabled() && !security.isLocked()) security.lock();
  if (display.isSleeping() || security.isLocked()) {
    if (totp.isLoaded()) totpApp.clearSession(context);
    if (vault.isLoaded()) vaultApp.clearSession(context);
    webPortal.stop(context);
    usbHid.cancel();
  }
  wifi.tick(now);
  while (Serial.available() > 0) {
    char c = static_cast<char>(Serial.read());
    if (timeService.processSerialInput(c)) {
      launcher.invalidate();
    }
  }
  timeService.tick(now, wifi.isConnected());
  if (!display.isSleeping()) {
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
