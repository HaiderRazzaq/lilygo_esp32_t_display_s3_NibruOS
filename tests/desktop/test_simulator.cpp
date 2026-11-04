#include "services/AccountImportService.h"
#define SDL_MAIN_HANDLED
#include <Arduino.h>
#include <Preferences.h>
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
#include "services/VaultService.h"
#include "services/TotpService.h"
#include "services/UsbHidService.h"
#include "services/WebPortalService.h"
#include "services/BleHidService.h"
#include "apps/WiFiApp.h"
#include "apps/TotpApp.h"
#include "apps/VaultApp.h"
#include "apps/SettingsApp.h"
#include "apps/SystemInfoApp.h"
#include "apps/TouchTestApp.h"
#include "apps/GeneratorApp.h"
#include "apps/ClockApp.h"

#include "platform/desktop/SDLDisplay.h"
#include "platform/desktop/DesktopInput.h"
#include "platform/desktop/DesktopStorage.h"
#include "platform/desktop/MockWiFiService.h"
#include "tests/support/AuthenticatorFlow.h"
#include "tests/support/AccountImportFixture.h"

#include <cassert>
#include <cstdio>
#include <vector>
#include <filesystem>
#include <fstream>

void testCoreLifecycle(AppContext& ctx, AppManager& launcher, WiFiApp& wifiApp, TotpApp& totpApp, SettingsApp& settingsApp) {
  (void)ctx; (void)wifiApp; (void)totpApp; (void)settingsApp;
  std::printf("[Test] Running Core Lifecycle & AppManager Tests...\n");

  assert(launcher.isHome());
  assert(launcher.appCount() == 8);
  assert(launcher.selectedIndex() == 0);

  // Open App 0 (WiFiApp)
  assert(launcher.open(0));
  assert(!launcher.isHome());
  assert(launcher.selectedIndex() == 0);

  // Switch directly to App 3 (SettingsApp) - ensures onExit runs on App 0
  assert(launcher.open(3));
  assert(!launcher.isHome());
  assert(launcher.selectedIndex() == 3);

  // Return home via Back button action
  launcher.handleButton(ButtonAction::Back);
  assert(launcher.isHome());
  assert(launcher.selectedIndex() == 3); // Preserves selection on return

  // Out of bounds open
  assert(!launcher.open(99));
  assert(launcher.isHome());

  // Navigation: BOOT short press cycles selection on Home
  launcher.handleButton(ButtonAction::SelectNext);
  assert(launcher.selectedIndex() == 4);
  launcher.handleButton(ButtonAction::SelectNext);
  assert(launcher.selectedIndex() == 5);
  launcher.handleButton(ButtonAction::SelectNext);
  assert(launcher.selectedIndex() == 6);
  launcher.handleButton(ButtonAction::SelectNext);
  assert(launcher.selectedIndex() == 7);
  launcher.handleButton(ButtonAction::SelectNext);
  assert(launcher.selectedIndex() == 0);

  // BOOT long press opens selected app (0 = WiFiApp)
  launcher.handleButton(ButtonAction::Activate);
  assert(!launcher.isHome());
  assert(launcher.selectedIndex() == 0);

  // Back returns Home
  launcher.handleButton(ButtonAction::Back);
  assert(launcher.isHome());

  // Test Select-then-Confirm touch behavior on Home:
  // Tap 1 on card 1 (x=160, y=85, contactId=1) selects card 1 without opening
  TouchEvent evSelect{160, 85, 84, 160, TouchGesture::Tap, 1};
  launcher.handleTouch(evSelect);
  assert(launcher.isHome());
  assert(launcher.selectedIndex() == 1);
  assert(launcher.isArmed(1));

  // Repeated event with same contactId does NOT open
  launcher.handleTouch(evSelect);
  assert(launcher.isHome());

  // Tap on card 2 shifts selection without opening
  TouchEvent evShift{260, 85, 84, 260, TouchGesture::Tap, 2};
  launcher.handleTouch(evShift);
  assert(launcher.isHome());
  assert(launcher.selectedIndex() == 2);
  assert(launcher.isArmed(2));

  // Second separate tap on card 2 (contactId=3) confirms and opens!
  TouchEvent evConfirm{260, 85, 84, 260, TouchGesture::Tap, 3};
  launcher.handleTouch(evConfirm);
  assert(!launcher.isHome());
  assert(launcher.selectedIndex() == 2);

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
  assert(s.rotation() == 1);

  // Valid changes
  assert(s.setBrightness(50));
  assert(s.setScreenTimeoutSec(60));
  assert(s.setTheme(2)); // Amber
  assert(s.setRotation(3)); // Inverted (180)
  assert(s.brightness() == 50);
  assert(s.screenTimeoutSec() == 60);
  assert(s.theme() == 2);
  assert(s.rotation() == 3);

  // Reload new instance from persistent storage file
  SettingsService reloaded;
  reloaded.begin(&ctx.display);
  assert(reloaded.brightness() == 50);
  assert(reloaded.screenTimeoutSec() == 60);
  assert(reloaded.theme() == 2);
  assert(reloaded.rotation() == 3);

  // Invalid value rejections
  assert(!reloaded.setBrightness(15));
  assert(!reloaded.setBrightness(105));
  assert(!reloaded.setScreenTimeoutSec(45));
  assert(!reloaded.setTheme(5));
  assert(!reloaded.setRotation(0));
  assert(!reloaded.setRotation(2));
  // Values remain unchanged
  assert(reloaded.brightness() == 50);
  assert(reloaded.rotation() == 3);

  // Reset to defaults
  assert(reloaded.resetToDefaults());
  assert(reloaded.brightness() == 100);
  assert(reloaded.screenTimeoutSec() == 30);
  assert(reloaded.theme() == 0);
  assert(reloaded.rotation() == 1);

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

  std::printf("  -> 25 Lifecycle Cycles PASS (host control flow; no physical leak measurement)\n");
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

void testSleepLockWakeFlow(AppContext& ctx, WiFiApp& wifiApp, AppManager& launcher) {
  std::printf("[TEST] Sleep / Lock / Wake and Display Restoration Flow...\n");

  // 1. Enter WiFiApp
  wifiApp.onEnter(ctx);
  assert(wifiApp.consumeDirty());
  wifiApp.render(ctx);

  // 2. Put display into sleep mode
  ctx.display.sleep();
  assert(ctx.display.isSleeping());

  // 3. When display is sleeping, lock must engage if PIN enabled
  assert(ctx.security.setPin("1234"));
  assert(ctx.security.setPinEnabled(true));
  ctx.security.lock();
  assert(ctx.security.isLocked());

  // While sleeping, display is inactive
  assert(ctx.display.isSleeping());

  // 4. Wake display up
  ctx.display.wake();
  assert(!ctx.display.isSleeping());

  // On wake, invalidateFull is called to guarantee clean frame redraw
  ctx.security.invalidateFull();
  assert(ctx.security.consumeDirty());
  ctx.security.render(ctx);

  // 5. Unlock and test display restoration for WiFiApp
  ctx.security.unlock();
  assert(!ctx.security.isLocked());

  // AppManager / wasLocked notifies active app that display was restored
  wifiApp.onDisplayRestored(ctx);
  assert(wifiApp.consumeDirty());
  wifiApp.render(ctx);

  ctx.security.setPinEnabled(false);
  wifiApp.onExit(ctx);
  launcher.goHome();
  std::printf("  -> PASS: Sleep/Lock/Wake and Display Restoration verified.\n");
}


void testEncryptedVaultStorage(AppContext& ctx) {
  std::printf("[TEST] Running Encrypted Password Vault Desktop Simulation Tests...\n");
  ctx.vault.begin();
  assert(!ctx.vault.isLoaded());

  uint8_t testKey[32]{};
  for (int i = 0; i < 32; ++i) testKey[i] = static_cast<uint8_t>(i + 0x20);

  assert(ctx.vault.loadVault(testKey));
  assert(ctx.vault.isLoaded() && ctx.vault.count() == 0);

  assert(ctx.vault.addEntry(testKey, "Email", "user@proton.me", "V3ry$ecureP@ss", "Main email"));
  assert(ctx.vault.addEntry(testKey, "AWS", "dev-admin", "AKIAIOSFODNN7EXAMPLE", "Cloud"));
  assert(ctx.vault.count() == 2);

  const VaultEntry* e = ctx.vault.getEntry(0);
  assert(e && std::strcmp(e->title, "Email") == 0);
  assert(std::strcmp(e->username, "user@proton.me") == 0);
  assert(std::strcmp(e->password, "V3ry$ecureP@ss") == 0);

  // Backup and restore simulation
  uint8_t backup[1024]{};
  size_t backupLen = 0;
  assert(ctx.vault.exportBackup("BackupSecret123", backup, sizeof(backup), backupLen));
  assert(backupLen > 48);

  ctx.vault.lock();
  assert(!ctx.vault.isLoaded() && ctx.vault.count() == 0);

  // Restore with wrong password must fail
  assert(!ctx.vault.importBackup("BadPass", backup, backupLen, testKey));

  // Reload valid vault from disk
  assert(ctx.vault.loadVault(testKey));
  assert(ctx.vault.count() == 2);

  // Clean up
  assert(ctx.vault.removeEntry(testKey, 0));
  assert(ctx.vault.removeEntry(testKey, 0));
  assert(ctx.vault.count() == 0);
  ctx.vault.lock();
  CryptoService::secureZero(testKey, sizeof(testKey));
  std::printf("  -> PASS: Encrypted Password Vault Desktop Storage & Backup verified.\n");
}

void testVaultAppUI(AppContext& ctx, VaultApp& vaultApp, AppManager& launcher) {
  std::printf("[TEST] Running Password Vault Application UI & Confirmed USB HID Tests...\n");
  (void)launcher;
  ctx.security.begin();
  assert(ctx.security.setPin("739126"));
  assert(ctx.security.setPinEnabled(true));
  assert(ctx.security.verifyPin("739126"));
  ctx.security.unlock();
  assert(ctx.security.hasActiveKey());

  ctx.vault.loadVault(ctx.security.getActiveVaultKey());
  ctx.vault.addEntry(ctx.security.getActiveVaultKey(), "GitHub", "octocat", "SecretPassword456", "Dev account");
  assert(ctx.vault.count() == 1);

  vaultApp.onEnter(ctx);
  vaultApp.render(ctx);

  // 1. First tap on 'Type Password' arms the button (does not type yet)
  TouchEvent tap1{200, 100, 200, 100, TouchGesture::Tap, 101, 1000};
  vaultApp.onTouch(ctx, tap1);
  assert(ctx.usbHid.transferState() == UsbTransferState::Idle);

  // 2. Second tap with same finger without lifting (same contactId) does NOT confirm
  TouchEvent tap1Repeat{200, 100, 200, 100, TouchGesture::Tap, 101, 1050};
  vaultApp.onTouch(ctx, tap1Repeat);
  assert(ctx.usbHid.transferState() == UsbTransferState::Idle);

  // 3. Second separate tap with lifted finger (different contactId) confirms and transmits
  TouchEvent tap2{200, 100, 200, 100, TouchGesture::Tap, 102, 1200};
  vaultApp.onTouch(ctx, tap2);
  assert(ctx.usbHid.transferState() == UsbTransferState::Sending || ctx.usbHid.transferState() == UsbTransferState::Sent);

  // 4. Test Show/Hide password toggle
  TouchEvent showTap{250, 75, 250, 75, TouchGesture::Tap, 103, 1300};
  vaultApp.onTouch(ctx, showTap);
  vaultApp.render(ctx);
  vaultApp.onTouch(ctx, showTap);
  vaultApp.render(ctx);

  // 5. Test Sleep/Lock Session Clearing & Automatic Wake-up Reload
  vaultApp.clearSession(ctx);
  assert(!ctx.vault.isLoaded()); // Decrypted vault memory zeroized on lock

  // Re-unlock device with PIN
  assert(ctx.security.verifyPin("739126"));
  ctx.security.unlock();
  assert(ctx.security.hasActiveKey());

  // Waking up / onTick should reload vault into memory cleanly
  vaultApp.onDisplayRestored(ctx);
  assert(ctx.vault.isLoaded());
  assert(ctx.vault.count() == 1);

  // 6. Clean up entry
  ctx.vault.removeEntry(ctx.security.getActiveVaultKey(), 0);
  assert(ctx.vault.count() == 0);

  vaultApp.onExit(ctx);
  std::printf("  -> PASS: Password Vault Application UI & Confirmed USB HID verified.\n");
}

void testWebPortalSimulation(AppContext& ctx) {
  std::printf("[Test] Running Web Portal Service & Simulation Tests...\n");
  assert(!ctx.webPortal.isActive());
  assert(ctx.webPortal.start(1000, ctx));
  assert(ctx.webPortal.isActive());
  assert(strncmp(ctx.webPortal.ssid(), "NibruOS-", 8) == 0);
  assert(strlen(ctx.webPortal.password()) == 8);
  assert(strlen(ctx.webPortal.webPin()) == 4);

  // Status check
  char resp[1024];
  int code = ctx.webPortal.simulateRequest(ctx, "GET", "/api/status", "", resp, sizeof(resp));
  assert(code == 200);
  assert(strstr(resp, "\"ok\":true") != nullptr);

  // Invalid PIN check
  code = ctx.webPortal.simulateRequest(ctx, "POST", "/api/vault/add", "{\"pin\":\"0000\",\"title\":\"Test\"}", resp, sizeof(resp));
  assert(code == 401);

  // Locked check: with valid PIN, but device locked -> 403
  ctx.security.lock();
  assert(!ctx.security.hasActiveKey());
  char validWrongLock[128];
  snprintf(validWrongLock, sizeof(validWrongLock), "{\"pin\":\"%s\",\"title\":\"Test\"}", ctx.webPortal.webPin());
  code = ctx.webPortal.simulateRequest(ctx, "POST", "/api/vault/add", validWrongLock, resp, sizeof(resp));
  assert(code == 403);

  // Now unlock device security
  assert(ctx.security.verifyPin("739126"));
  ctx.security.unlock();
  assert(ctx.security.hasActiveKey());

  // Valid Vault addition
  char validVault[256];
  snprintf(validVault, sizeof(validVault),
           "{\"pin\":\"%s\",\"title\":\"WebMail\",\"username\":\"haider\",\"password\":\"Pass123\",\"notes\":\"Work\"}",
           ctx.webPortal.webPin());
  code = ctx.webPortal.simulateRequest(ctx, "POST", "/api/vault/add", validVault, resp, sizeof(resp));
  assert(code == 200);
  assert(strstr(resp, "\"success\":true") != nullptr);
  assert(ctx.vault.count() == 1);

  // Valid TOTP addition
  char validTotp[256];
  snprintf(validTotp, sizeof(validTotp),
           "{\"pin\":\"%s\",\"name\":\"Cloud\",\"secret\":\"JBSWY3DPEHPK3PXP\"}",
           ctx.webPortal.webPin());
  code = ctx.webPortal.simulateRequest(ctx, "POST", "/api/totp/add", validTotp, resp, sizeof(resp));
  assert(code == 200);
  assert(strstr(resp, "\"success\":true") != nullptr);

  // Test Web Portal Backup & Restore endpoints
  char backupReq[128];
  snprintf(backupReq, sizeof(backupReq), "{\"pin\":\"%s\",\"passphrase\":\"BackupPass1\"}", ctx.webPortal.webPin());
  code = ctx.webPortal.simulateRequest(ctx, "POST", "/api/vault/backup", backupReq, resp, sizeof(resp));
  assert(code == 200);
  assert(strstr(resp, "\"payload\":") != nullptr);

  const char* pKey = "\"payload\":\"";
  const char* pStart = strstr(resp, pKey);
  assert(pStart != nullptr);
  pStart += strlen(pKey);
  const char* pEnd = strchr(pStart, '"');
  assert(pEnd != nullptr);
  char hexPayload[1024]{};
  size_t pLen = static_cast<size_t>(pEnd - pStart);
  assert(pLen < sizeof(hexPayload));
  memcpy(hexPayload, pStart, pLen);
  hexPayload[pLen] = '\0';

  char restoreReq[1200];
  snprintf(restoreReq, sizeof(restoreReq), "{\"pin\":\"%s\",\"passphrase\":\"BackupPass1\",\"payload\":\"%s\"}",
           ctx.webPortal.webPin(), hexPayload);
  code = ctx.webPortal.simulateRequest(ctx, "POST", "/api/vault/restore", restoreReq, resp, sizeof(resp));
  std::printf("RESTORE RESULT: code=%d resp=%s\n", code, resp);
  assert(code == 200);
  assert(strstr(resp, "\"success\":true") != nullptr);

  // Cleanup vault entry
  ctx.vault.removeEntry(ctx.security.getActiveVaultKey(), 0);
  assert(ctx.vault.count() == 0);

  // Timeout test
  ctx.webPortal.tick(1000 + 181 * 1000, ctx);
  assert(!ctx.webPortal.isActive());
  assert(ctx.resources.wifiOwner() == ResourceOwner::None);

  std::printf("  -> PASS: Web Portal Service & REST Simulation verified.\n");
}

void testNewMvpFeatures(AppContext& ctx, GeneratorApp& genApp, ClockApp& clockApp, AppManager& launcher) {
  (void)launcher;
  std::printf("[Test] Running New MVP Features (Generator, Clock, BLE) Tests...\n");

  // 1. ClockApp & Screensaver tests
  clockApp.onEnter(ctx);
  clockApp.render(ctx);
  TouchEvent tapEv{};
  tapEv.gesture = TouchGesture::Tap;
  tapEv.x = 100; tapEv.y = 100;
  clockApp.onTouch(ctx, tapEv); // cycle theme
  // Tap Screensaver toggle pill
  assert(!ctx.settings.clockScreensaver());
  tapEv.x = 200; tapEv.y = 125;
  clockApp.onTouch(ctx, tapEv);
  assert(ctx.settings.clockScreensaver());
  clockApp.onTouch(ctx, tapEv);
  assert(!ctx.settings.clockScreensaver());
  clockApp.onTouch(ctx, tapEv);
  assert(ctx.settings.clockScreensaver());

  clockApp.onTick(ctx, 1000);
  clockApp.render(ctx);
  clockApp.renderScreensaver(ctx, 1000);
  clockApp.onExit(ctx);

  // DisplayManager screensaver inactivity test
  assert(ctx.display.screensaverEnabled());
  ctx.display.setSleepTimeoutSeconds(2);
  ctx.display.notifyActivity(1000);
  ctx.display.tick(3001);
  assert(ctx.display.isScreensaverActive());
  clockApp.renderScreensaver(ctx, 3001);
  ctx.display.stopScreensaver();
  assert(!ctx.display.isScreensaverActive());

  // 2. GeneratorApp tests
  genApp.onEnter(ctx);
  genApp.render(ctx);
  // Tap Length button
  tapEv.x = 50; tapEv.y = 85;
  genApp.onTouch(ctx, tapEv);
  // Tap Regen button
  tapEv.x = 250; tapEv.y = 85;
  genApp.onTouch(ctx, tapEv);
  // Tap Type USB
  tapEv.x = 50; tapEv.y = 125;
  genApp.onTouch(ctx, tapEv);
  // Save to Vault test
  if (!ctx.security.hasActiveKey()) {
    ctx.security.verifyPin("739126");
    ctx.security.unlock();
  }
  tapEv.x = 200; tapEv.y = 125;
  genApp.onTouch(ctx, tapEv); // Tap Save to Vault
  assert(ctx.vault.count() >= 1);
  ctx.vault.removeEntry(ctx.security.getActiveVaultKey(), 0);
  genApp.render(ctx);
  genApp.onExit(ctx);

  // 3. BleHidService tests
  assert(!ctx.bleHid.isEnabled());
  assert(!ctx.bleHid.isConnected());
  assert(ctx.bleHid.setEnabled(true, ctx));
  assert(ctx.bleHid.isEnabled());
  assert(ctx.resources.wifiOwner() == ResourceOwner::BleKeyboard);
  ctx.bleHid.simulateConnected(true);
  assert(ctx.bleHid.isConnected());
  assert(ctx.bleHid.typeText("test"));
  assert(ctx.bleHid.setEnabled(false, ctx));
  assert(!ctx.bleHid.isEnabled());
  assert(ctx.resources.wifiOwner() == ResourceOwner::None);

  // 4. GeneratorApp & BLE typing integration test
  genApp.onEnter(ctx);
  assert(ctx.bleHid.setEnabled(true, ctx));
  ctx.bleHid.simulateConnected(true);
  const uint32_t countBefore = ctx.bleHid.typedCount();
  tapEv.gesture = TouchGesture::Tap;
  tapEv.x = 150; tapEv.y = 125; // Tap Type BLE
  genApp.onTouch(ctx, tapEv);
  assert(ctx.bleHid.typedCount() > countBefore);
  genApp.onExit(ctx);
  assert(ctx.bleHid.setEnabled(false, ctx));

  // 5. SettingsApp Bluetooth Subview Touch & PrimaryAction test
  SettingsApp settingsApp;
  settingsApp.onEnter(ctx);
  // Navigate to Bluetooth subview (index 9 in menu)
  for (int i = 0; i < 9; ++i) settingsApp.onPrimaryAction(ctx);
  settingsApp.onActivate(ctx);
  settingsApp.render(ctx);

  // Touch ENABLE BLE KEYBOARD button (x: 160, y: 120)
  assert(!ctx.bleHid.isEnabled());
  tapEv.gesture = TouchGesture::Tap;
  tapEv.x = 160; tapEv.y = 120;
  settingsApp.onTouch(ctx, tapEv);
  assert(ctx.bleHid.isEnabled()); // Verifies SettingsApp::onTouch routes to handleBluetoothTouch

  // Touch DISABLE button (x: 160, y: 120)
  settingsApp.onTouch(ctx, tapEv);
  assert(!ctx.bleHid.isEnabled());

  // Test BOOT button (onPrimaryAction) toggle
  settingsApp.onPrimaryAction(ctx);
  assert(ctx.bleHid.isEnabled());
  settingsApp.onPrimaryAction(ctx);
  assert(!ctx.bleHid.isEnabled());

  settingsApp.onExit(ctx);

  std::printf("  -> PASS: New MVP Features verified successfully.\n");
}
namespace {
bool simUsbReady() { return true; }
bool simUsbSend(uint8_t, uint8_t) { return true; }
bool simUsbConnected() { return true; }
UsbHidTransport simUsbTransport{simUsbReady, simUsbSend, simUsbConnected};
}

int main() {
  std::printf("\n==================================================\n");
  std::printf("  NibruOS Automated Desktop & Shared Logic Tests\n");
  std::printf("==================================================\n\n");

  // Initialize headless simulator environment
  DesktopStorage::instance().init(".testbuild/auth_runtime_test");
  SDLDisplay::instance().init(1, true); // headless = true

  DisplayManager display;
  ResourceManager resources;
  WiFiService wifi;
  SecurityService security;
  SettingsService settings;
  TimeService timeService;
  TotpService totp;
  UsbHidService usbHid{&simUsbTransport};
  usbHid.begin();

  AccountImportService accountImport;
  VaultService vault;
  WebPortalService webPortal;
  BleHidService bleHid;

  AppContext context{display, resources, wifi, security, settings, timeService, totp, usbHid, accountImport, vault, webPortal, bleHid};
  AppManager launcher(context);

  WiFiApp wifiApp;
  TotpApp totpApp;
  VaultApp vaultApp;
  SettingsApp settingsApp;
  GeneratorApp generatorApp;
  ClockApp clockApp;
  SystemInfoApp sysInfoApp;
  TouchTestApp touchTestApp;

  launcher.registerApp(wifiApp);
  launcher.registerApp(totpApp);
  launcher.registerApp(vaultApp);
  launcher.registerApp(settingsApp);
  launcher.registerApp(generatorApp);
  launcher.registerApp(clockApp);
  launcher.registerApp(sysInfoApp);
  launcher.registerApp(touchTestApp);
  launcher.begin();

  testCoreLifecycle(context, launcher, wifiApp, totpApp, settingsApp);
  testUITransformationsAndGestures();
  testWiFiSimulation(context);
  testSettingsPersistence(context);
  testModularArchitecture(context);
  testSleepLockWakeFlow(context, wifiApp, launcher);
  testEncryptedVaultStorage(context);
  testVaultAppUI(context, vaultApp, launcher);
  testWebPortalSimulation(context);
  testNewMvpFeatures(context, generatorApp, clockApp, launcher);
  testRepeatedLifecycleCycles(launcher);
  // Shared application UI, real Windows crypto, disk-backed test-only NVS.
  Preferences::resetMock();
  security.setCredentialChangeGuard(TotpService::canChangeCredentials);
  security.begin();
  assert(security.setPin("739126"));
  assert(security.setPinEnabled(true));
  assert(security.verifyPin("739126")); security.unlock();
  totp.begin();
  testAuthenticatorEntryFlow(context, totpApp);
  assert(DesktopStorage::instance().reloadNvs()); // Reopen from actual disk, not the in-memory map.
  security.begin();
  assert(security.verifyPin("739126")); security.unlock();
  totpApp.onEnter(context);
  totpApp.render(context);
  // Export a fixture-only framebuffer for visual QA, outside source control.
  if (FILE* image = std::fopen(".testbuild/authenticator-card.ppm", "wb")) {
    std::fprintf(image, "P6\n320 170\n255\n");
    const uint16_t* pixels = SDLDisplay::instance().framebuffer();
    for (int i = 0; i < 320 * 170; ++i) {
      const uint16_t p = pixels[i];
      const unsigned char rgb[] = {static_cast<unsigned char>((p >> 11) * 255 / 31),
          static_cast<unsigned char>(((p >> 5) & 63) * 255 / 63), static_cast<unsigned char>((p & 31) * 255 / 31)};
      std::fwrite(rgb, 1, 3, image);
    }
    std::fclose(image);
  }
  authAdd(totpApp, context);
  totpApp.render(context);
  if (FILE* image = std::fopen(".testbuild/entry-keyboard.ppm", "wb")) {
    std::fprintf(image, "P6\n320 170\n255\n");
    for (int i = 0; i < 320 * 170; ++i) {
      const uint16_t p = SDLDisplay::instance().framebuffer()[i];
      const unsigned char rgb[] = {static_cast<unsigned char>((p >> 11) * 255 / 31),
          static_cast<unsigned char>(((p >> 5) & 63) * 255 / 63), static_cast<unsigned char>((p & 31) * 255 / 31)};
      std::fwrite(rgb,1,3,image);
    }
    std::fclose(image);
  }
  assert(totpApp.onBack(context));
  totpApp.onExit(context);
  AccountImportTransport io(ImportFixture::openImport, ImportFixture::closeImport, ImportFixture::readImport);
  AccountImportService paired(&io);
  AppContext pairedContext{display, resources, wifi, security, settings, timeService, totp, usbHid, paired, vault, webPortal, bleHid};
  totpApp.onEnter(pairedContext);
  authTap(totpApp, pairedContext, 110, 158);
  authTap(totpApp, pairedContext, 160, 88);
  assert(paired.state() == ImportState::Waiting);
  ImportFixture::input = ImportFixture::frame(paired, "Imported\tGEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQ\t6\t30");
  for (unsigned i = 0; i < 8; ++i) totpApp.onTick(pairedContext, millis() + i);
  assert(paired.state() == ImportState::Review && totp.accountCount() == 1);
  const uint32_t confirmNow = millis() + 400;
  setCustomMillis(confirmNow);
  authTap(totpApp, pairedContext, 240, 125);
  assert(totp.accountCount() == 2 && paired.state() == ImportState::Idle);
  totpApp.onExit(pairedContext);
  totpApp.onExit(context);
  Preferences::resetMock();
  {
    auto& storage = DesktopStorage::instance();
    const std::filesystem::path file = ".testbuild/auth_runtime_test/nvs_store.dat";
    const std::filesystem::path backup = ".testbuild/auth_runtime_test/write-failure-backup.dat";
    assert(storage.nvsOpen("fault_fixture", false));
    const uint8_t before = 17, after = 29;
    assert(storage.nvsPutBytes("fault_fixture", "value", &before, 1) == 1);
    std::filesystem::rename(file, backup);
    std::filesystem::create_directory(file); // Real OS replacement/write failure.
    const size_t written = storage.nvsPutBytes("fault_fixture", "value", &after, 1);
    std::filesystem::remove(file);
    std::filesystem::rename(backup, file); // Restore fixture before asserting.
    assert(written == 0);
    uint8_t recovered = 0;
    assert(storage.nvsGetBytes("fault_fixture", "value", &recovered, 1) == 1 && recovered == 17);
    storage.nvsClose("fault_fixture");
    {
      std::ofstream corrupt(file, std::ios::binary | std::ios::trunc);
      corrupt << "BAD";
    }
    assert(!storage.reloadNvs());
    assert(!storage.nvsOpen("must_not_overwrite", false));
    storage.flush();
    std::ifstream preserved(file, std::ios::binary);
    const std::string contents((std::istreambuf_iterator<char>(preserved)), {});
    assert(contents == "BAD");
    preserved.close();
    storage.resetNvs(); // Explicit recovery of this test-only fixture.
  }

  std::printf("\n==================================================\n");
  std::printf("  ALL DESKTOP SIMULATOR TESTS PASSED SUCCESSFULLY!\n");
  std::printf("==================================================\n\n");
  return 0;
}
