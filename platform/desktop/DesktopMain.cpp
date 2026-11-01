#include "services/AccountImportService.h"
#define SDL_MAIN_HANDLED
#include <Arduino.h>
#include "core/AppContext.h"
#include "core/AppManager.h"
#include "core/ResourceManager.h"
#include "core/MemoryProfiler.h"
#include "core/WakeInputGate.h"
#include "hardware/DisplayManager.h"
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
#include "platform/desktop/DesktopDiagnostics.h"
#include "platform/desktop/MockWiFiService.h"

#include <SDL.h>
#include <cstdio>

int main(int argc, char* argv[]) {
  uint8_t scale = 3;
  bool headless = false;
  int maxFrames = -1;
  const char* runtimeDir = "simulator/runtime";

  for (int i = 1; i < argc; ++i) {
    if (std::strcmp(argv[i], "--headless") == 0) {
      headless = true;
    } else if (std::strncmp(argv[i], "--scale=", 8) == 0) {
      scale = static_cast<uint8_t>(std::atoi(argv[i] + 8));
    } else if (std::strncmp(argv[i], "--test-frames=", 14) == 0) {
      maxFrames = std::atoi(argv[i] + 14);
    } else if (std::strncmp(argv[i], "--runtime-dir=", 14) == 0) {
      runtimeDir = argv[i] + 14;
    }
  }

  std::printf("==================================================\n");
  std::printf("  NibruOS Desktop Simulator (Windows Target)\n");
  std::printf("  Resolution: 320x170 (ST7789 landscape simulation)\n");
  std::printf("  Default Scale: %ux | Headless: %s\n", scale, headless ? "YES" : "NO");
  std::printf("==================================================\n");
  std::printf("Controls:\n");
  std::printf("  Mouse: Click/Drag = Touchscreen Tap & Swipes\n");
  std::printf("  Keyboard:\n");
  std::printf("    Escape/Backspace/U = Back (GPIO14 User Button)\n");
  std::printf("    Space/Tab/B/Right  = Select Next (BOOT short press)\n");
  std::printf("    Enter/O            = Activate / Open (BOOT long press)\n");
  std::printf("    Arrow Up/Down/Left = Touch gestures\n");
  std::printf("  Simulator Function Keys:\n");
  std::printf("    F1 = Cycle Window Scale (1x -> 2x -> 3x -> 4x)\n");
  std::printf("    F2 = Wi-Fi Scan Mode: Normal\n");
  std::printf("    F3 = Wi-Fi Scan Mode: Timeout (simulates timeout bug & retry)\n");
  std::printf("    F4 = Wi-Fi Scan Mode: Scan Error\n");
  std::printf("    F5 = Device Reboot / Restart\n");
  std::printf("    F6 = Toggle Diagnostic HUD\n");
  std::printf("    F9 = Wi-Fi Mode: STA Error\n");
  std::printf("==================================================\n\n");

  DesktopStorage::instance().init(runtimeDir);

  if (!SDLDisplay::instance().init(scale, headless)) {
    std::printf("[Error] Failed to initialize SDLDisplay backend.\n");
    return 1;
  }

  DisplayManager display;
  ResourceManager resources;
  WiFiService wifi;
  SecurityService security;
  SettingsService settings;
  TimeService timeService;
  TotpService totp;
  UsbHidService usbHid;

  AccountImportService accountImport;

  AppContext context{display, resources, wifi, security, settings, timeService, totp, usbHid, accountImport};
  AppManager launcher(context);

  WiFiApp wifiApp;
  TotpApp totpApp;
  SettingsApp settingsApp;

  WakeInputGate wakeInput;

  auto bootSystem = [&]() {
    std::printf("\n[NibruOS] Booting system services...\n");
    display.begin();
    settings.begin(&display);
    wifi.begin(millis());
    security.setCredentialChangeGuard(TotpService::canChangeCredentials);
    security.begin();
    timeService.begin();
    totp.begin();
    usbHid.begin();

    // Register applications exactly matching physical device baseline (src/main.cpp)
    launcher.registerApp(wifiApp);
    launcher.registerApp(totpApp);
    launcher.registerApp(settingsApp);

    launcher.begin();
    launcher.tick(millis());

    if (security.isPinEnabled()) {
      security.lock();
    }
  };

  bootSystem();
  DesktopDiagnostics::instance().init();

  bool wasLocked = false;
  uint32_t lastFrameMs = millis();

  while (!DesktopInput::instance().shouldQuit()) {
    const uint32_t now = millis();

    // Process SDL window, mouse, and keyboard events
    DesktopInput::instance().processEvents(now, SDLDisplay::instance().scale());

    // Check for simulated reboot
    if (DesktopInput::instance().consumeRebootRequest()) {
      wifi.shutdown();
      launcher.goHome();
      DesktopStorage::instance().reloadNvs();
      bootSystem();
      continue;
    }

    TouchEvent ev{};
    const bool hasTouch = DesktopInput::instance().pollTouch(ev);
    const ButtonAction action = DesktopInput::instance().pollButtons(now);

    if (display.isSleeping()) {
      if (hasTouch || action != ButtonAction::None) {
        display.wake();
        wakeInput.arm(now);
        if (security.isPinEnabled()) {
          security.lock();
        }
      }
    } else if (wakeInput.consume(hasTouch, action != ButtonAction::None,
                                 DesktopInput::instance().anyButtonPressed() ||
                                 DesktopInput::instance().isTouchPressed(), now)) {
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

    if (wasLocked && !security.isLocked()) {
      launcher.invalidate();
    }
    wasLocked = security.isLocked();

    const uint32_t tickNow = millis();
    display.tick(tickNow);
    if (display.isSleeping() && security.isPinEnabled() && !security.isLocked()) security.lock();
    if (display.isSleeping() || security.isLocked()) {
      if (totp.isLoaded()) totpApp.clearSession(context);
      usbHid.cancel();
    }
    wifi.tick(tickNow);
    timeService.tick(tickNow, wifi.isConnected());

    if (security.isLocked()) {
      security.tick(tickNow);
      if (security.consumeDirty()) {
        security.render(context);
      }
    } else {
      launcher.tick(tickNow);
    }

    usbHid.tick(tickNow);

    DesktopDiagnostics::instance().tick(tickNow, launcher, resources, wifi);
    DesktopDiagnostics::instance().renderOverlay();
    SDLDisplay::instance().present();

    if (maxFrames > 0 && --maxFrames == 0) {
      DesktopInput::instance().requestQuit();
    }

    // Frame pacing: maintain ~60 FPS
    const uint32_t frameDuration = millis() - lastFrameMs;
    if (frameDuration < 16) {
      SDL_Delay(16 - frameDuration);
    }
    lastFrameMs = millis();
  }

  std::printf("\n[NibruOS] Simulator shutting down cleanly...\n");
  launcher.goHome();
  wifi.shutdown();
  DesktopStorage::instance().flush();
  SDLDisplay::instance().destroy();
  std::printf("[NibruOS] Goodbye.\n");
  return 0;
}
