#include <Arduino.h>
#include "core/AppContext.h"
#include "core/AppManager.h"
#include "core/ResourceManager.h"
#include "hardware/DisplayManager.h"
#include "hardware/TouchManager.h"
#include "hardware/ButtonManager.h"
#include "services/WiFiService.h"
#include "apps/SystemInfoApp.h"
#include "apps/TouchTestApp.h"
#include "apps/WiFiScannerApp.h"

namespace {
DisplayManager display;
TouchManager touch;
ButtonManager buttons;
ResourceManager resources;
WiFiService wifi;
AppContext context{display, resources, wifi};
AppManager launcher(context);
SystemInfoApp systemInfo;
TouchTestApp touchTest;
WiFiScannerApp wifiScanner;
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n[NibruOS] boot v0.1");
  display.begin();
  touch.begin();
  buttons.begin();
  launcher.registerApp(systemInfo);
  launcher.registerApp(touchTest);
  launcher.registerApp(wifiScanner);
  launcher.begin();
}
void loop() {
  const uint32_t now = millis();
  TouchEvent ev{};
  if (touch.poll(ev)) launcher.handleTouch(ev);
  const ButtonAction action = buttons.poll(now);
  if (action != ButtonAction::None) launcher.handleButton(action);
  launcher.tick(now);
  delay(12);
}
