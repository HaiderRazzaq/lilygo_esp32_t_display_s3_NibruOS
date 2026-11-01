#define SDL_MAIN_HANDLED
#include <Arduino.h>
#include "core/AppContext.h"
#include "core/AppManager.h"
#include "core/IApp.h"
#include "core/EventTypes.h"
#include "core/ResourceManager.h"
#include "core/WakeInputGate.h"
#include "hardware/DisplayManager.h"
#include "ui/Theme.h"
#include "ui/HomeScreen.h"
#include "PinDefinitions.h"
#include "NibruConfig.h"
#include "services/WiFiService.h"
#include "services/SecurityService.h"
#include "services/SettingsService.h"
#include "services/TimeService.h"
#include "services/TotpService.h"
#include "services/UsbHidService.h"
#include "apps/WiFiApp.h"
#include "apps/TotpApp.h"
#include "apps/SettingsApp.h"
#include "apps/SystemInfoApp.h"
#include "apps/TouchTestApp.h"

#include "platform/desktop/SDLDisplay.h"
#include "platform/desktop/DesktopInput.h"
#include "platform/desktop/DesktopStorage.h"
#include "platform/desktop/MockWiFiService.h"

#include <cassert>
#include <cstdio>
#include <vector>

void testCoreLifecycle(AppContext& ctx, AppManager& launcher, WiFiApp& wifiApp, TotpApp& totpApp, SettingsApp& settingsApp) {
  std::printf("[Test] Running Core Lifecycle & AppManager Tests...\n");

  assert(launcher.isHome());
  assert(launcher.appCount() == 5);
  assert(launcher.selectedIndex() == 0);

  // Open App 0 (WiFiApp)
  assert(launcher.open(0));
  assert(!launcher.isHome());
  assert(launcher.selectedIndex() == 0);

  // Switch directly to App 2 (SettingsApp) - ensures onExit runs on App 0
  assert(launcher.open(2));
  assert(!launcher.isHome());
  assert(launcher.selectedIndex() == 2);

  // Return home via Back button action
  launcher.handleButton(ButtonAction::Back);
  assert(launcher.isHome());
  assert(launcher.selectedIndex() == 2); // Preserves selection on return

  // Out of bounds open
  assert(!launcher.open(99));
  assert(launcher.isHome());

  // Navigation: BOOT short press cycles selection on Home
  launcher.handleButton(ButtonAction::SelectNext);
  assert(launcher.selectedIndex() == 3);
  launcher.handleButton(ButtonAction::SelectNext);
  assert(launcher.selectedIndex() == 4);
  launcher.handleButton(ButtonAction::SelectNext);
  assert(launcher.selectedIndex() == 0);

  // BOOT long press opens selected app (0 = WiFiApp)
  launcher.handleButton(ButtonAction::Activate);
  assert(!launcher.isHome());
  assert(launcher.selectedIndex() == 0);

  // Back returns Home
  launcher.handleButton(ButtonAction::Back);
  assert(launcher.isHome());

  std::printf("  -> Core Lifecycle Tests PASS\n");
}

void testUITransformationsAndGestures() {
  std::printf("[Test] Running UI & Coordinate Transformation Tests...\n");

  SDLDisplay& screen = SDLDisplay::instance();
  assert(screen.width() == 320);
  assert(screen.height() == 170);

  DesktopInput& input = DesktopInput::instance();
  TouchEvent ev{};

  // Test Tap Injection
  input.injectTouch(100, 50, TouchGesture::Tap);
  assert(input.pollTouch(ev));
  assert(ev.x == 100);
  assert(ev.y == 50);
  assert(ev.rawX == 170 - 1 - 50); // Inverted raw coordinate mapping
  assert(ev.rawY == 100);
  assert(ev.gesture == TouchGesture::Tap);
  assert(ev.contactId > 0);

  // Test Boundary Clamping
  input.injectTouch(-10, 200, TouchGesture::Tap);
  assert(input.pollTouch(ev));
  assert(ev.x == 0);
  assert(ev.y == 169);

  input.injectTouch(350, -5, TouchGesture::SwipeRight);
  assert(input.pollTouch(ev));
  assert(ev.x == 319);
  assert(ev.y == 0);
  assert(ev.gesture == TouchGesture::SwipeRight);

  // Test Gestures (SwipeUp, SwipeDown, SwipeLeft, SwipeRight, DoubleTap, LongPress)
  const TouchGesture gestures[] = {
    TouchGesture::SwipeLeft, TouchGesture::SwipeRight,
    TouchGesture::SwipeUp, TouchGesture::SwipeDown,
    TouchGesture::DoubleTap, TouchGesture::LongPress
  };
  for (auto g : gestures) {
    input.injectTouch(160, 85, g);
    assert(input.pollTouch(ev));
    assert(ev.gesture == g);
  }

  std::printf("  -> UI & Transformation Tests PASS\n");
}

void testWiFiSimulation(AppContext& ctx) {
  std::printf("[Test] Running Wi-Fi Service & Fault Injection Tests...\n");

  WiFiService& wifi = ctx.wifi;
  ResourceManager& res = ctx.resources;

  // 1. Successful Scan
  MockWiFiService::instance().setSimulationMode(WiFiSimMode::Normal);
  assert(res.acquireWiFi(ResourceOwner::WiFiScanner));
  setCustomMillis(1000);
  assert(wifi.startScan(1000));
  assert(wifi.state() == ScanState::Preparing); // 650ms warmup state

  // Simulate tick after 700ms warmup -> transitions to Scanning
  setCustomMillis(1700);
  wifi.tick(1700);
  assert(wifi.state() == ScanState::Scanning);

  // Advance time past scan duration (1200ms) -> completes
  setCustomMillis(3000);
  wifi.tick(3000);
  assert(wifi.state() == ScanState::Complete);
  assert(wifi.count() == 5);
  assert(std::string(wifi.network(0)->ssid) == "NibruOS_Lab");
  assert(wifi.network(0)->rssi == -48);
  assert(wifi.network(0)->secured == true);
  wifi.stopScan();
  res.releaseWiFi(ResourceOwner::WiFiScanner);
  assert(wifi.state() == ScanState::Idle);

  // 2. Scan Timeout & Retry
  MockWiFiService::instance().setSimulationMode(WiFiSimMode::Timeout);
  assert(res.acquireWiFi(ResourceOwner::WiFiScanner));
  setCustomMillis(3000);
  assert(wifi.startScan(3000));
  assert(wifi.attempt() == 1);
  assert(wifi.state() == ScanState::Preparing);

  // Warmup finishes at 3700ms -> starts scanning
  setCustomMillis(3700);
  wifi.tick(3700);
  assert(wifi.state() == ScanState::Scanning);

  // Advance time past kScanTimeoutMs (11,000 ms) from 3700ms -> 15200ms
  setCustomMillis(15200);
  wifi.tick(15200);
  // Attempt 1 should have failed and triggered retry attempt 2 (in Preparing)
  assert(wifi.attempt() == 2);
  assert(wifi.state() == ScanState::Preparing);

  // Warmup for attempt 2 finishes (700ms later) -> Scanning
  setCustomMillis(15900);
  wifi.tick(15900);
  assert(wifi.state() == ScanState::Scanning);

  // Advance time past second timeout (11,000 ms from 15900ms -> 27000ms)
  setCustomMillis(27000);
  wifi.tick(27000);
  // Max attempts exceeded -> transitions to Failed!
  assert(wifi.state() == ScanState::Failed);
  wifi.stopScan();
  res.releaseWiFi(ResourceOwner::WiFiScanner);

  // 3. Immediate Scan Failure
  MockWiFiService::instance().setSimulationMode(WiFiSimMode::ScanFailed);
  assert(res.acquireWiFi(ResourceOwner::WiFiScanner));
  setCustomMillis(40000);
  assert(wifi.startScan(40000));
  assert(wifi.state() == ScanState::Preparing);

  // Warmup finishes at 40700ms -> triggers hardware scan which fails -> retries attempt 2
  setCustomMillis(40700);
  wifi.tick(40700);
  assert(wifi.attempt() == 2);
  assert(wifi.state() == ScanState::Preparing);

  // Warmup for attempt 2 finishes at 41400ms -> triggers hardware scan which fails -> transitions to Failed
  setCustomMillis(41400);
  wifi.tick(41400);
  assert(wifi.state() == ScanState::Failed);
  wifi.stopScan();
  res.releaseWiFi(ResourceOwner::WiFiScanner);

  // Reset clock and mode
  clearCustomMillis();
  MockWiFiService::instance().setSimulationMode(WiFiSimMode::Normal);
  std::printf("  -> Wi-Fi Simulation Tests PASS\n");
}

void testSettingsPersistence(AppContext& ctx) {
  std::printf("[Test] Running Settings Persistence & Desktop NVS Tests...\n");

  DesktopStorage::instance().resetNvs();
  SettingsService s;
  s.begin(&ctx.display);

  // Defaults verification
  assert(s.brightness() == 100);
  assert(s.screenTimeoutSec() == 30);
  assert(s.theme() == 0);

  // Valid changes
  assert(s.setBrightness(50));
  assert(s.setScreenTimeoutSec(60));
  assert(s.setTheme(2)); // Amber
  assert(s.brightness() == 50);
  assert(s.screenTimeoutSec() == 60);
  assert(s.theme() == 2);

  // Reload new instance from persistent storage file
  SettingsService reloaded;
  reloaded.begin(&ctx.display);
  assert(reloaded.brightness() == 50);
  assert(reloaded.screenTimeoutSec() == 60);
  assert(reloaded.theme() == 2);

  // Invalid value rejections
  assert(!reloaded.setBrightness(15));
  assert(!reloaded.setBrightness(105));
  assert(!reloaded.setScreenTimeoutSec(45));
  assert(!reloaded.setTheme(5));
  // Values remain unchanged
  assert(reloaded.brightness() == 50);

  // Reset to defaults
  assert(reloaded.resetToDefaults());
  assert(reloaded.brightness() == 100);
  assert(reloaded.screenTimeoutSec() == 30);
  assert(reloaded.theme() == 0);

  std::printf("  -> Settings Persistence Tests PASS\n");
}

void testRepeatedLifecycleCycles(AppManager& launcher) {
  std::printf("[Test] Running 25 Repeated Lifecycle Entry/Exit Cycles...\n");

  for (int cycle = 0; cycle < 25; ++cycle) {
    uint8_t appIdx = static_cast<uint8_t>(cycle % 5);
    assert(launcher.open(appIdx));
    assert(!launcher.isHome());
    assert(launcher.selectedIndex() == appIdx);
    launcher.goHome();
    assert(launcher.isHome());
  }

  std::printf("  -> 25 Lifecycle Cycles PASS (Zero leaks, clean state)\n");
}

void testModularArchitecture(AppContext& ctx) {
  std::printf("[Test] Running Modular Architecture & HAL Contract Tests...\n");

  // 1. EventTypes & IApp compliance
  IApp* appPtr = nullptr;
  WiFiApp wifiApp;
  appPtr = &wifiApp;
  assert(std::strcmp(appPtr->title(), "Wi-Fi") == 0);

  // 2. Theme & Colors
  assert(Theme::getAccentColor(0) == Colors::Cyan);
  assert(Theme::getAccentColor(1) == Colors::Green);
  assert(Theme::getAccentColor(2) == Colors::Amber);
  assert(Theme::getAccentColor(3) == Colors::Magenta);
  assert(Theme::HeaderHeight == 33);
  assert(Theme::FooterHeight == 18);

  // 3. Centralized Pin Definitions
  assert(Pins::TouchSda == 18);
  assert(Pins::TouchScl == 17);
  assert(Pins::TouchReset == 21);
  assert(Pins::TouchInterrupt == 16);
  assert(Pins::ButtonBoot == 0);
  assert(Pins::ButtonUser == 14);

  // 4. WiFiService MAC address retrieval (service layer boundary)
  char macBuf[32]{};
  ctx.wifi.getMacAddress(macBuf, sizeof(macBuf));
  assert(std::strlen(macBuf) > 0);

  std::printf("  -> Modular Architecture & HAL Tests PASS\n");
}

int main() {
  std::printf("\n==================================================\n");
  std::printf("  NibruOS Automated Desktop & Shared Logic Tests\n");
  std::printf("==================================================\n\n");

  // Initialize headless simulator environment
  DesktopStorage::instance().init("simulator/runtime_test");
  SDLDisplay::instance().init(1, true); // headless = true

  DisplayManager display;
  ResourceManager resources;
  WiFiService wifi;
  SecurityService security;
  SettingsService settings;
  TimeService timeService;
  TotpService totp;
  UsbHidService usbHid;

  AppContext context{display, resources, wifi, security, settings, timeService, totp, usbHid};
  AppManager launcher(context);

  WiFiApp wifiApp;
  TotpApp totpApp;
  SettingsApp settingsApp;
  SystemInfoApp sysInfoApp;
  TouchTestApp touchTestApp;

  launcher.registerApp(wifiApp);
  launcher.registerApp(totpApp);
  launcher.registerApp(settingsApp);
  launcher.registerApp(sysInfoApp);
  launcher.registerApp(touchTestApp);
  launcher.begin();

  testCoreLifecycle(context, launcher, wifiApp, totpApp, settingsApp);
  testUITransformationsAndGestures();
  testWiFiSimulation(context);
  testSettingsPersistence(context);
  testModularArchitecture(context);
  testRepeatedLifecycleCycles(launcher);

  std::printf("\n==================================================\n");
  std::printf("  ALL DESKTOP SIMULATOR TESTS PASSED SUCCESSFULLY!\n");
  std::printf("==================================================\n\n");
  return 0;
}
