#include <Arduino.h>
#include "core/AppContext.h"
#include "core/AppManager.h"
#include "core/ResourceManager.h"
#include "hardware/DisplayManager.h"
#include "hardware/TouchManager.h"
#include "hardware/ButtonManager.h"
#include "services/WiFiService.h"
#include "services/SecurityService.h"
#include "services/SettingsService.h"
#include "apps/WiFiApp.h"
#include "apps/SettingsApp.h"

namespace {
DisplayManager display;
TouchManager touch;
ButtonManager buttons;
ResourceManager resources;
WiFiService wifi;
SecurityService security;
SettingsService settings;
AppContext context{display, resources, wifi, security, settings};
AppManager launcher(context);
WiFiApp wifiApp;
SettingsApp settingsApp;
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n[NibruOS] boot v0.3");
  display.begin();
  settings.begin(&display);
  touch.begin();
  buttons.begin();
  wifi.begin(millis());
  security.begin();
  launcher.registerApp(wifiApp);
  launcher.registerApp(settingsApp);
  launcher.begin();

  if (security.isPinEnabled()) {
    security.lock();
  }
}

void loop() {
  uint32_t now = millis();
  TouchEvent ev{};
  const bool hasTouch = touch.poll(ev);
  const ButtonAction action = buttons.poll(now);

  if (display.isSleeping()) {
    if (hasTouch || action != ButtonAction::None) {
      display.wake();
      if (security.isPinEnabled()) {
        security.lock();
      }
    }
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
  wifi.tick(now);
  if (security.isLocked()) {
    security.tick(now);
    if (security.consumeDirty()) {
      security.render(context);
    }
  } else {
    launcher.tick(now);
  }
  delay(12);
}
