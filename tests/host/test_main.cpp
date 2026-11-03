#include "services/AccountImportService.h"
#include "core/AppManager.h"
#include "core/IApp.h"
#include "core/EventTypes.h"
#include "ui/Theme.h"
#include "PinDefinitions.h"
#include "NibruConfig.h"
#include "BoardConfig.h"
#include "core/AppContext.h"
#include "core/ResourceManager.h"
#include "hardware/DisplayManager.h"
#include "hardware/TouchManager.h"
#include "hardware/ButtonManager.h"
#include "services/WiFiService.h"
#include "apps/SystemInfoApp.h"
#include "apps/TouchTestApp.h"
#include "apps/WiFiScannerApp.h"
#include "apps/WiFiApp.h"
#include "apps/SettingsApp.h"
#include "services/CryptoService.h"
#include "services/SecurityService.h"
#include "services/SettingsService.h"
#include "services/VaultService.h"
#include "services/TimeService.h"
#include "services/TotpService.h"
#include "services/UsbHidService.h"
#include "services/WebPortalService.h"
#include <Arduino.h>
#include <WiFi.h>
#include <cassert>
#include <cstdio>
uint32_t fakeMs=100;
int fakePins[64]{};
MockSerial Serial;
MockESP ESP;
MockWiFi WiFi;
void testSettings(AppContext& ctx);
bool fakeTouchPending=false;
TouchData fakeTouchData{};

void testTouchPressCycles() {
  TouchManager driver;
  driver.begin();
  TouchEvent event{};
  auto frame = [&](byte gesture, byte points, byte phase) {
    fakeMs += 200;
    fakeTouchData = TouchData{gesture, points, phase, 42, 75};
    fakeTouchPending = true;
    return driver.poll(event);
  };
  // Down is not a completed tap: it may still become a swipe.
  assert(!frame(NONE, 1, 0));
  assert(driver.isPressed());
  assert(frame(SINGLE_CLICK, 1, 2));
  assert(event.gesture == TouchGesture::Tap);
  const uint32_t id = event.contactId;
  for (int i = 0; i < 8; ++i) assert(!frame(SINGLE_CLICK, 1, 2));
  assert(!frame(LONG_PRESS, 1, 2));
  // The library has one IRQ flag: a new Down must recover from a missed Up.
  assert(!frame(NONE, 1, 0));
  assert(frame(SINGLE_CLICK, 1, 2));
  assert(event.contactId != id);
  assert(!frame(NONE, 0, 1));
  assert(!driver.isPressed());
  assert(!frame(NONE, 1, 0));
  assert(frame(SWIPE_UP, 0, 1)); // recognized gesture may arrive on release
  assert(event.gesture == TouchGesture::SwipeLeft && event.contactId != id);
  assert(!frame(SWIPE_UP, 0, 1)); // duplicate release cannot create another gesture
  assert(!frame(NONE, 1, 0));
  assert(frame(SINGLE_CLICK, 0, 1));
  assert(event.gesture == TouchGesture::Tap);
  assert(!frame(SINGLE_CLICK, 0, 1));
  // A contact with no decoded gesture still produces one tap when released.
  assert(!frame(NONE, 1, 0));
  assert(frame(NONE, 0, 1));
  assert(event.gesture == TouchGesture::Tap);
  assert(!frame(NONE, 0, 1));
  fakeTouchData = TouchData{};
}

int main() {
  for(auto& p:fakePins)p=HIGH;
  testTouchPressCycles();
  // Verify tested Touch Glass landscape conversion and gesture mapping.
  TouchManager touchDriver;
  touchDriver.begin();
  fakeTouchData.x=42;
  fakeTouchData.y=75;
  fakeTouchData.gestureID=SWIPE_DOWN;
  fakeTouchPending=true;
  fakeMs+=200;
  TouchEvent touchEvent{};
  assert(touchDriver.poll(touchEvent));
  assert(touchEvent.x==75 && touchEvent.y==127);
  assert(touchEvent.gesture==TouchGesture::SwipeRight); // native DOWN -> visible RIGHT
  assert(touchDriver.isPressed());
  const uint32_t firstContact = touchEvent.contactId;
  assert(firstContact != 0);
  fakeTouchData.gestureID = NONE;
  fakeTouchPending = true;
  fakeMs += 700;
  assert(!touchDriver.poll(touchEvent));
  assert(touchDriver.isPressed()); // contact exists without a dispatchable event
  fakeTouchData.points = 0;
  fakeTouchPending = true;
  assert(!touchDriver.poll(touchEvent));
  assert(!touchDriver.isPressed());
  fakeTouchData.points = 1;
  assert(!touchDriver.poll(touchEvent));
  // Other directions are likewise rotated for landscape navigation.
  struct GestureCase { byte native; TouchGesture landscape; };
  const GestureCase cases[] = {
    {SWIPE_UP, TouchGesture::SwipeLeft},
    {SWIPE_LEFT, TouchGesture::SwipeDown},
    {SWIPE_RIGHT, TouchGesture::SwipeUp},
    {SINGLE_CLICK, TouchGesture::Tap},
  };
  for (const auto& c : cases) {
    fakeMs += 200;
    fakeTouchData.gestureID = c.native;
    fakeTouchPending = true;
    assert(touchDriver.poll(touchEvent));
    assert(touchEvent.gesture == c.landscape);
    assert(touchEvent.contactId != firstContact);
    fakeTouchData.points = 0;
    fakeTouchData.event = 1;
    fakeTouchData.gestureID = NONE;
    fakeTouchPending = true;
    assert(!touchDriver.poll(touchEvent));
    fakeTouchData.points = 1;
    fakeTouchData.event = 0;
  }
  // Physical buttons: BOOT short changes selection, BOOT long activates;
  // GPIO14 short must provide escape.
  ButtonManager keyDriver;
  keyDriver.begin();
  fakeMs+=100;
  fakePins[0]=LOW;
  assert(keyDriver.poll(fakeMs)==ButtonAction::None);
  fakeMs+=90;
  fakePins[0]=HIGH;
  assert(keyDriver.poll(fakeMs)==ButtonAction::SelectNext);
  fakeMs+=90;
  fakePins[0]=LOW;
  keyDriver.poll(fakeMs);
  fakeMs+=700;
  assert(keyDriver.poll(fakeMs)==ButtonAction::Activate);
  fakeMs+=90;
  fakePins[0]=HIGH;
  assert(keyDriver.poll(fakeMs)==ButtonAction::None);
  fakeMs+=90;
  fakePins[14]=LOW;
  keyDriver.poll(fakeMs);
  fakeMs+=90;
  fakePins[14]=HIGH;
  assert(keyDriver.poll(fakeMs)==ButtonAction::Back);

  DisplayManager display;
  ResourceManager resources;
  WiFiService wifi;
  SecurityService security;
  SettingsService settings;
  TimeService timeService;
  TotpService totpService;
  UsbHidService usbHid;
  AccountImportService accountImport;
  VaultService vault;
  WebPortalService webPortal;
  AppContext context{display, resources, wifi, security, settings, timeService, totpService, usbHid, accountImport, vault, webPortal};
  AppManager mgr(context);
  SystemInfoApp info;
  TouchTestApp touch;
  WiFiScannerApp scanner;
  assert(mgr.registerApp(info));
  assert(mgr.registerApp(touch));
  assert(mgr.registerApp(scanner));
  mgr.begin();
  mgr.tick(fakeMs);
  assert(mgr.isHome() && mgr.appCount()==3);
  assert(resources.wifiOwner()==ResourceOwner::None);
  // Home selection and open.
  mgr.handleButton(ButtonAction::SelectNext);
  assert(mgr.selectedIndex()==1);
  mgr.handleButton(ButtonAction::Activate);
  assert(!mgr.isHome());
  mgr.handleTouch(TouchEvent{123,100,70,123,TouchGesture::SwipeLeft});
  mgr.tick(fakeMs);
  mgr.handleButton(ButtonAction::Back);
  assert(mgr.isHome());
  // Select-then-Confirm: tap 1 selects and arms tile 2 (Wi-Fi).
  mgr.handleTouch(TouchEvent{255, 88, 81, 255, TouchGesture::Tap, 10});
  assert(mgr.isHome());
  assert(mgr.selectedIndex() == 2);
  assert(mgr.isArmed(2));

  // Same contact does NOT confirm (user did not lift finger).
  mgr.handleTouch(TouchEvent{255, 88, 81, 255, TouchGesture::Tap, 10});
  assert(mgr.isHome());

  // Touching tile 0 shifts selection without opening.
  mgr.handleTouch(TouchEvent{50, 88, 81, 50, TouchGesture::Tap, 11});
  assert(mgr.isHome());
  assert(mgr.selectedIndex() == 0);
  assert(mgr.isArmed(0));

  // Tap 1 on tile 2 arms tile 2 again.
  mgr.handleTouch(TouchEvent{255, 88, 81, 255, TouchGesture::Tap, 12});
  assert(mgr.isHome());
  assert(mgr.selectedIndex() == 2);
  assert(mgr.isArmed(2));

  // Tap 2 on tile 2 with separate contact confirms and opens!
  mgr.handleTouch(TouchEvent{255, 88, 81, 255, TouchGesture::Tap, 13});
  assert(!mgr.isHome());
  assert(resources.ownsWiFi(ResourceOwner::WiFiScanner));
  assert(wifi.state()==ScanState::Preparing);
  assert(WiFi.modeValue==WIFI_STA);
  assert(WiFi.started==0);  // must not scan during STA startup
  mgr.tick(fakeMs+Board::kWiFiWarmupMs-1);
  assert(WiFi.started==0);
  mgr.tick(fakeMs+Board::kWiFiWarmupMs);
  assert(wifi.state()==ScanState::Scanning && WiFi.started==1);
  WiFi.scans=2;
  WiFi.complete=true;
  mgr.tick(fakeMs+Board::kWiFiWarmupMs+20);
  assert(wifi.state()==ScanState::Complete);
  assert(wifi.count()==2);
  const NetworkRecord* record=wifi.network(0);
  assert(record && record->rssi==-62 && record->channel==6 && record->secured);
  // Rescan through app onPrimaryAction; app exit stops an in-flight HW scan.
  mgr.handleButton(ButtonAction::SelectNext);
  assert(wifi.state()==ScanState::Preparing);
  mgr.tick(fakeMs+Board::kWiFiWarmupMs);
  assert(wifi.state()==ScanState::Scanning);
  const int stoppedBeforeExit=WiFi.stopped;
  mgr.handleButton(ButtonAction::Back);
  assert(mgr.isHome() && WiFi.modeValue==WIFI_OFF);
  assert(WiFi.stopped==stoppedBeforeExit+1);
  assert(resources.wifiOwner()==ResourceOwner::None);
  // Timeout of first attempt leads to automatic reset and second attempt.
  assert(wifi.startScan(fakeMs));
  wifi.tick(fakeMs+Board::kWiFiWarmupMs);
  assert(wifi.state()==ScanState::Scanning && wifi.attempt()==1);
  wifi.tick(fakeMs+Board::kWiFiWarmupMs+Board::kScanTimeoutMs+1);
  assert(wifi.state()==ScanState::Preparing && wifi.attempt()==2);
  const uint32_t retryAt=fakeMs+Board::kWiFiWarmupMs+Board::kScanTimeoutMs+1;
  wifi.tick(retryAt+Board::kWiFiWarmupMs);
  assert(wifi.state()==ScanState::Scanning && wifi.attempt()==2);
  wifi.tick(retryAt+Board::kWiFiWarmupMs+Board::kScanTimeoutMs+1);
  assert(wifi.state()==ScanState::Failed && WiFi.modeValue==WIFI_OFF);
  assert(std::strcmp(wifi.lastError(), "Scan timed out") == 0);
  wifi.shutdown();
  // The retry must also be able to recover, not just to fail twice.
  assert(wifi.startScan(fakeMs));
  WiFi.complete=false;
  wifi.tick(fakeMs+Board::kWiFiWarmupMs);
  wifi.tick(fakeMs+Board::kWiFiWarmupMs+Board::kScanTimeoutMs+1);
  assert(wifi.state()==ScanState::Preparing && wifi.attempt()==2);
  WiFi.complete=false;
  wifi.tick(fakeMs+Board::kScanTimeoutMs+Board::kWiFiWarmupMs*2+1);
  assert(wifi.state()==ScanState::Scanning);
  WiFi.complete=true;
  WiFi.scans=1;
  wifi.tick(fakeMs+Board::kScanTimeoutMs+Board::kWiFiWarmupMs*2+2);
  assert(wifi.state()==ScanState::Complete && wifi.count()==1);
  wifi.shutdown();
  // Completion should win even if poll was delayed past the local deadline.
  assert(wifi.startScan(fakeMs));
  WiFi.complete=false;
  wifi.tick(fakeMs+Board::kWiFiWarmupMs);
  WiFi.complete=true;
  wifi.tick(fakeMs+Board::kWiFiWarmupMs+Board::kScanTimeoutMs+10);
  assert(wifi.state()==ScanState::Complete);
  wifi.shutdown();
  // Failure paths: radio initialization, scan initiation and completion.
  WiFi.canMode=false;
  assert(!wifi.startScan(fakeMs));
  assert(wifi.state()==ScanState::Failed);
  WiFi.canMode=true;
  WiFi.failStart=true;
  assert(wifi.startScan(fakeMs));
  wifi.tick(fakeMs+Board::kWiFiWarmupMs);
  assert(wifi.state()==ScanState::Preparing && wifi.attempt()==2);
  wifi.tick(fakeMs+Board::kWiFiWarmupMs*2);
  assert(wifi.state()==ScanState::Failed && WiFi.modeValue==WIFI_OFF);
  WiFi.failStart=false;
  assert(wifi.startScan(fakeMs));
  WiFi.failComplete=true;
  wifi.tick(fakeMs+Board::kWiFiWarmupMs);
  wifi.tick(fakeMs+Board::kWiFiWarmupMs+1);
  assert(wifi.state()==ScanState::Preparing && wifi.attempt()==2);
  wifi.tick(fakeMs+Board::kWiFiWarmupMs*2+1);
  wifi.tick(fakeMs+Board::kWiFiWarmupMs*2+2);
  assert(wifi.state()==ScanState::Failed && WiFi.modeValue==WIFI_OFF);
  WiFi.failComplete=false;
  wifi.shutdown();
  // Protect from unauthorized resource release.
  assert(resources.acquireWiFi(ResourceOwner::WiFiScanner));
  resources.releaseWiFi(ResourceOwner::None);
  assert(resources.ownsWiFi(ResourceOwner::WiFiScanner));
  resources.releaseWiFi(ResourceOwner::WiFiScanner);
  // Upper-left header tap returns Home from any app.
  assert(mgr.open(0));
  mgr.handleTouch(TouchEvent{12,12,0,0,TouchGesture::Tap});
  assert(mgr.isHome());
  // Register a fourth app and verify pagination in the Home shell.
  struct DummyApp : App {
    const char* title() const override {return "Dummy";}
    void onEnter(AppContext&) override {invalidate();}
    void onExit(AppContext&) override {}
    void onTick(AppContext&, uint32_t) override {}
    void onTouch(AppContext&, const TouchEvent&) override {}
    void render(AppContext&) override {}
  } dummy;
  assert(mgr.registerApp(dummy));
  mgr.handleTouch(TouchEvent{200, 90, 0, 0, TouchGesture::SwipeLeft});
  assert(mgr.selectedIndex()==3);
  // Select-then-Confirm: first tap arms, second tap confirms open
  mgr.handleTouch(TouchEvent{20, 90, 0, 0, TouchGesture::Tap, 20});
  assert(mgr.isHome() && mgr.selectedIndex()==3 && mgr.isArmed(3));
  mgr.handleTouch(TouchEvent{20, 90, 0, 0, TouchGesture::Tap, 21});
  assert(!mgr.isHome() && mgr.selectedIndex()==3);
  mgr.goHome();
  assert(mgr.isHome());

  // Wi-Fi Connection state machine tests
  assert(wifi.connect("TestNet", "MyPass1234", fakeMs));
  assert(wifi.connectState() == ConnectState::Connecting);
  assert(std::strcmp(wifi.targetSSID(), "TestNet") == 0);
  WiFi.statusValue = WL_CONNECTED;
  wifi.tick(fakeMs + 50);
  assert(wifi.connectState() == ConnectState::Connected);
  assert(std::strcmp(wifi.connectedSSID(), "TestNet") == 0);
  char ipBuf[24]{};
  wifi.getLocalIP(ipBuf, sizeof(ipBuf));
  assert(std::strcmp(ipBuf, "192.168.1.100") == 0);
  wifi.disconnect();
  assert(wifi.connectState() == ConnectState::Disconnected);

  // Connection timeout test with underflow guard
  assert(wifi.connect("TimeoutNet", "SecretPass", fakeMs));
  WiFi.statusValue = WL_IDLE_STATUS;
  // Verify stale tick timestamp does not trigger underflow timeout
  wifi.tick(fakeMs - 5);
  assert(wifi.connectState() == ConnectState::Connecting);
  wifi.tick(fakeMs + Board::kConnectTimeoutMs + 10);
  assert(wifi.connectState() == ConnectState::ConnectFailed);
  assert(std::strncmp(wifi.connectError(), "Timed out", 9) == 0);
  wifi.shutdown();

  // SettingsApp sub-navigation test with Select-then-Confirm
  SettingsApp settingsApp;
  settingsApp.onEnter(context);
  // Tap 1 selects and arms row
  settingsApp.onTouch(context, TouchEvent{100, 60, 0, 0, TouchGesture::Tap, 30});
  // Not entered yet: onBack at menu returns false
  assert(settingsApp.onBack(context) == false);
  // Tap 1 arms again
  settingsApp.onTouch(context, TouchEvent{100, 60, 0, 0, TouchGesture::Tap, 31});
  // Tap 2 confirms and enters subview
  settingsApp.onTouch(context, TouchEvent{100, 60, 0, 0, TouchGesture::Tap, 32});
  // Back returns to Settings Menu
  assert(settingsApp.onBack(context) == true);
  // At menu, Back returns false so AppManager shell can return Home
  assert(settingsApp.onBack(context) == false);
  settingsApp.onExit(context);

  // WiFiApp sub-navigation test with Select-then-Confirm
  WiFiApp wifiApp;
  wifiApp.onEnter(context);
  // Tap 1 arms Scan Networks card
  wifiApp.onTouch(context, TouchEvent{100, 70, 0, 0, TouchGesture::Tap, 40});
  assert(wifiApp.onBack(context) == false); // still at menu
  // Tap 1 arms again
  wifiApp.onTouch(context, TouchEvent{100, 70, 0, 0, TouchGesture::Tap, 41});
  // Tap 2 confirms Scan Networks
  wifiApp.onTouch(context, TouchEvent{100, 70, 0, 0, TouchGesture::Tap, 42});
  // Back returns to WiFi Menu
  assert(wifiApp.onBack(context) == true);

  // Tap 1 arms Saved Networks card
  wifiApp.onTouch(context, TouchEvent{100, 120, 0, 0, TouchGesture::Tap, 43});
  assert(wifiApp.onBack(context) == false); // still at menu
  // Tap 1 arms again
  wifiApp.onTouch(context, TouchEvent{100, 120, 0, 0, TouchGesture::Tap, 44});
  // Tap 2 confirms Saved Networks
  wifiApp.onTouch(context, TouchEvent{100, 120, 0, 0, TouchGesture::Tap, 45});
  // Back returns to WiFi Menu
  assert(wifiApp.onBack(context) == true);
  // At menu, Back returns false so shell returns Home
  assert(wifiApp.onBack(context) == false);
  wifiApp.onExit(context);

  // Saved networks persistence and management tests
  while (wifi.savedCount()) wifi.forgetNetwork(static_cast<uint8_t>(0));
  assert(wifi.savedCount() == 0);
  assert(wifi.saveNetwork("HomeNet", "Password123"));
  assert(wifi.savedCount() == 1);
  assert(std::strcmp(wifi.savedNetwork(0)->ssid, "HomeNet") == 0);
  assert(std::strcmp(wifi.getSavedPassword("HomeNet"), "Password123") == 0);
  assert(wifi.saveNetwork("OfficeNet", "CorpSecret"));
  assert(wifi.savedCount() == 2);
  // MRU: OfficeNet is at index 0, HomeNet is at index 1
  assert(std::strcmp(wifi.savedNetwork(0)->ssid, "OfficeNet") == 0);
  assert(std::strcmp(wifi.savedNetwork(1)->ssid, "HomeNet") == 0);
  assert(wifi.forgetNetwork(static_cast<uint8_t>(0)));
  assert(wifi.savedCount() == 1);
  assert(std::strcmp(wifi.savedNetwork(0)->ssid, "HomeNet") == 0);
  assert(wifi.forgetNetwork("HomeNet"));
  assert(wifi.savedCount() == 0);

  // CryptoService hardware/mock tests
  uint8_t randBuf[16]{};
  CryptoService::fillRandom(randBuf, sizeof(randBuf));
  // Zeroize test
  CryptoService::secureZero(randBuf, sizeof(randBuf));
  for (size_t i = 0; i < sizeof(randBuf); ++i) {
    assert(randBuf[i] == 0);
  }
  // Constant-time equality test
  uint8_t aBuf[4] = {1, 2, 3, 4};
  uint8_t bBuf[4] = {1, 2, 3, 4};
  uint8_t cBuf[4] = {1, 2, 3, 5};
  assert(CryptoService::constantTimeEquals(aBuf, bBuf, 4));
  assert(!CryptoService::constantTimeEquals(aBuf, cBuf, 4));

  // PBKDF2 key derivation test
  uint8_t salt[16] = {0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80,
                      0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0, 0x01};
  uint8_t derivedKey[32]{};
  assert(CryptoService::deriveKey("1234", 4, salt, sizeof(salt), 100, derivedKey, sizeof(derivedKey)));

  // AES-GCM mock encrypt/decrypt
  uint8_t iv[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
  uint8_t plain[16] = {'H', 'e', 'l', 'l', 'o', ' ', 'S', 'e', 'c', 'u', 'r', 'e', 'G', 'e', 'n', '!'};
  uint8_t cipher[16]{};
  uint8_t tag[16]{};
  uint8_t decrypted[16]{};
  assert(CryptoService::aesGcmEncrypt(derivedKey, 256, iv, sizeof(iv), nullptr, 0, plain, sizeof(plain), cipher, tag, sizeof(tag)));
  assert(CryptoService::aesGcmDecrypt(derivedKey, 256, iv, sizeof(iv), nullptr, 0, cipher, sizeof(cipher), tag, sizeof(tag), decrypted));
  assert(std::memcmp(plain, decrypted, sizeof(plain)) == 0);

  // SecurityService PIN & session vault key lifecycle tests
  SecurityService secService;
  secService.begin();
  assert(secService.setPin("4321"));
  assert(!secService.verifyPin("0000")); // Wrong PIN
  assert(!secService.hasActiveKey());
  assert(secService.verifyPin("4321")); // Correct PIN
  assert(secService.hasActiveKey());
  assert(secService.getActiveVaultKey() != nullptr);
  secService.setPinEnabled(true);
  secService.lock();
  assert(!secService.hasActiveKey());
  assert(secService.getActiveVaultKey() == nullptr);

  // Modular architecture and HAL tests
  IApp* appTestPtr = &info;
  assert(std::strcmp(appTestPtr->title(), "System Info") == 0);
  assert(Theme::getAccentColor(0) == Colors::Cyan);
  assert(Pins::TouchSda == 18 && Pins::TouchScl == 17);
  char testMac[24]{};
  wifi.getMacAddress(testMac, sizeof(testMac));
  assert(std::strlen(testMac) > 0);

  testSettings(context);
  extern void testAuthenticator();
  testAuthenticator();
  extern void testAccountImport();
  testAccountImport();
  extern void runVaultTests();
  runVaultTests();
  extern void runWebPortalHostTests();
  runWebPortalHostTests();
  puts("PASS: host navigation/resource/async scan/timeout/connect/crypto/settings smoke tests");
}
