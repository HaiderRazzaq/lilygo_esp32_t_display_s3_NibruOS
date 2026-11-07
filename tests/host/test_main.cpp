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
#include "services/BleHidService.h"
#include "services/BleAdvertisingService.h"
#include "services/BleAdvertisingPayload.h"
#include "apps/BleLabApp.h"
#include "services/WifiDefenseService.h"
#include "apps/WifiDefenseApp.h"
#include "apps/WiFiChannelsApp.h"
#include "apps/BeaconLabApp.h"
#include "apps/SecurityLabApp.h"
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

class TestBleAdvertiser final : public BleAdvertisingTransport {
public:
  bool beginOk = true, requestOk = true, endOk = true, lastConnectable = false;
  unsigned begins = 0, ends = 0;
  uint32_t seedValue = 0x12345678;
  bool varySeed = false;
  BleAdvertisingOperation requested = BleAdvertisingOperation::None;
  BleAdvertisingEvent event{};
  uint8_t address[6]{}, payload[31]{};
  size_t payloadLength = 0;
  bool begin() override { ++begins; return beginOk; }
  bool setAddress(const uint8_t* bytes) override {
    memcpy(address, bytes, sizeof(address));
    requested = BleAdvertisingOperation::Address;
    return requestOk;
  }
  bool setData(const uint8_t* bytes, size_t length) override {
    payloadLength = length;
    if (length <= sizeof(payload)) memcpy(payload, bytes, length);
    requested = BleAdvertisingOperation::Data;
    return requestOk;
  }
  bool start(bool connectable) override {
    lastConnectable = connectable;
    requested = BleAdvertisingOperation::Start;
    return requestOk;
  }
  bool stop() override { requested = BleAdvertisingOperation::Stop; return requestOk; }
  bool end() override { ++ends; return endOk; }
  uint32_t randomSeed() override { return varySeed ? seedValue++ : seedValue; }
  BleAdvertisingEvent poll() override {
    const auto result = event;
    event = {};
    return result;
  }
  void complete(bool success = true) { event = {requested, success}; }
};

void testBlePairingPayloads() {
  uint8_t payload[31]{};
  assert(buildBleAdvertisingPayload(BleAdvertisingProfile::Android, 0, 123, payload) == 17);
  assert(payload[5] == 0x2c && payload[6] == 0xfe);
  assert(payload[9] == 0x2c && payload[10] == 0xfe);
  assert(payload[11] == 0x00 && payload[12] == 0x01 && payload[13] == 0xf0);
  const auto windowsLength = buildBleAdvertisingPayload(BleAdvertisingProfile::Windows, 0, 123, payload);
  assert(windowsLength <= 31 && payload[4] == 0xff);
  assert(payload[5] == 0x06 && payload[6] == 0x00 && payload[7] == 0x03);
  assert(buildBleAdvertisingPayload(BleAdvertisingProfile::Apple, 0, 123, payload) == 31);
  assert(payload[0] == 30 && payload[2] == 0x4c && payload[4] == 7 && payload[5] == 25);
  assert(payload[6] == 7 && payload[7] == 0x0e && payload[8] == 0x20);
  assert(buildBleAdvertisingPayload(BleAdvertisingProfile::AppleNotYourDevice, 0, 123, payload) == 31);
  assert(payload[6] == 1 && payload[7] == 0x0e && payload[8] == 0x20);
  const auto actionLength =
      buildBleAdvertisingPayload(BleAdvertisingProfile::AppleAction, 0, 123, payload);
  assert(actionLength == 14);
  assert(payload[0] == 2 && payload[1] == 1 && payload[2] == 6);
  assert(payload[3] == 10 && payload[4] == 0xff && payload[5] == 0x4c);
  assert(payload[6] == 0x00 && payload[7] == 0x0f && payload[8] == 5);
  assert(buildBleAdvertisingPayload(BleAdvertisingProfile::Beacon, 0, 123, payload) == 31);
  assert(payload[3] == 3 && payload[4] == 3 && payload[5] == 0x12 && payload[6] == 0x18);
  assert(payload[11] == 19 && payload[12] == 9);
  assert(memcmp(payload + 13, "NeverGonnaGiveYoUp", 18) == 0);
  assert(buildBleAdvertisingPayload(BleAdvertisingProfile::Beacon, 4, 123, payload) > 0);
  assert(memcmp(payload + 13, "FBISurveillanceVan", 18) == 0);
  uint8_t first[31];
  memcpy(first, payload, sizeof(first));
  buildBleAdvertisingPayload(BleAdvertisingProfile::Apple, 0, 124, payload);
  assert(memcmp(first + 15, payload + 15, 16) != 0);
  assert(buildBleAdvertisingPayload(BleAdvertisingProfile::SamsungBuds, 0, 123, payload) == 31);
  assert(payload[0] == 27 && payload[2] == 0x75 && payload[4] == 0x42);
  assert(payload[28] == 0x10 && payload[29] == 0xff && payload[30] == 0x75);
  assert(buildBleAdvertisingPayload(BleAdvertisingProfile::SamsungWatch, 0, 123, payload) == 18);
  assert(payload[5] == 0x75 && payload[17] == 0x1a);
  assert(buildBleAdvertisingPayload(static_cast<BleAdvertisingProfile>(99), 0, 123, payload) == 0);
  for (uint8_t profile = 0; profile < 6; ++profile) {
    for (uint8_t identity = 0; identity < 10; ++identity) {
      const auto length = buildBleAdvertisingPayload(static_cast<BleAdvertisingProfile>(profile),
                                                      identity, 123, payload);
      assert(length > 0 && length <= sizeof(payload));
    }
    assert(buildBleAdvertisingPayload(static_cast<BleAdvertisingProfile>(profile), 10, 123, payload) == 0);
  }
  for (uint8_t identity = 0; identity < 50; ++identity) {
    assert(buildBleAdvertisingPayload(BleAdvertisingProfile::All, identity, 123, payload) > 0);
  }
  assert(buildBleAdvertisingPayload(BleAdvertisingProfile::All, 50, 123, payload) == 0);
  for (uint32_t seed = 0; seed < 50; ++seed) {
    const auto length = buildBleAdvertisingPayload(BleAdvertisingProfile::Random, 0, seed, payload);
    assert(length > 0 && length <= 31);
  }
  assert(buildBleAdvertisingPayload(BleAdvertisingProfile::Random, 10, 123, payload) == 0);
  assert(buildBleAdvertisingPayload(BleAdvertisingProfile::AppleAction, 10, 123, payload) == 0);
  assert(buildBleAdvertisingPayload(BleAdvertisingProfile::Beacon, 10, 123, payload) == 0);
  assert(buildBleAdvertisingPayload(static_cast<BleAdvertisingProfile>(99), 0, 123, payload) == 0);
  puts("PASS: Bruce pairing, Apple action, Not Your Device and beacon payload formats");
}

void testBleLab(AppContext& ctx) {
  TestBleAdvertiser transport;
  BleAdvertisingService service(&transport);
  ctx.bleAdvertising = &service;
  ctx.wifi.shutdown();
  assert(!service.start(0, 30, fakeMs, ctx));
  assert(!service.start(11, 30, fakeMs, ctx));
  assert(!service.start(5, 61, fakeMs, ctx));
  assert(ctx.resources.acquireWiFi(ResourceOwner::BleKeyboard));
  assert(!service.start(5, 30, fakeMs, ctx));
  ctx.resources.releaseWiFi(ResourceOwner::BleKeyboard);

  assert(service.start(2, 5, fakeMs, ctx));
  assert(ctx.resources.ownsWiFi(ResourceOwner::BleAdvertising));
  const auto firstAddress = transport.address[5];
  for (int i = 0; i < 3; ++i) {
    transport.complete();
    service.tick(++fakeMs, ctx);
  }
  assert(service.advertisedCount() == 1);
  assert(!transport.lastConnectable);
  assert(transport.payloadLength == 31 && transport.payload[0] == 2);
  assert(transport.payload[21] == 9 && transport.payload[22] == 9);
  assert(memcmp(transport.payload + 23, "LabBLE01", 8) == 0);
  assert((transport.address[0] & 0xc0) == 0xc0);
  fakeMs += 400;
  service.tick(fakeMs, ctx);
  assert(transport.requested == BleAdvertisingOperation::Stop);
  transport.complete();
  service.tick(++fakeMs, ctx);
  assert(transport.address[5] != firstAddress);
  for (int i = 0; i < 3; ++i) {
    transport.complete();
    service.tick(++fakeMs, ctx);
  }
  assert(service.advertisedCount() == 2);
  assert(memcmp(transport.payload + 23, "LabBLE02", 8) == 0);
  fakeMs += 400;
  service.tick(fakeMs, ctx);
  transport.complete();
  service.tick(++fakeMs, ctx);
  assert(transport.address[5] == firstAddress && transport.begins == 1);
  for (int i = 0; i < 3; ++i) {
    transport.complete();
    service.tick(++fakeMs, ctx);
  }
  assert(service.advertisedCount() == 2);
  fakeMs += 5000;
  service.tick(fakeMs, ctx);
  assert(!service.isActive() && ctx.resources.wifiOwner() == ResourceOwner::None);

  assert(service.start(1, 5, fakeMs, ctx));
  transport.complete(false);
  service.tick(++fakeMs, ctx);
  assert(!service.isActive() && service.lastError()[0]);
  assert(ctx.resources.wifiOwner() == ResourceOwner::None);

  assert(service.start(1, 5, fakeMs, ctx));
  fakeMs += 2001;
  service.tick(fakeMs, ctx);
  assert(!service.isActive() && service.lastError()[0]);

  transport.beginOk = false;
  assert(!service.start(1, 5, fakeMs, ctx));
  assert(ctx.resources.wifiOwner() == ResourceOwner::None);
  transport.beginOk = true;

  transport.requestOk = false;
  assert(!service.start(1, 5, fakeMs, ctx));
  assert(!service.isActive() && ctx.resources.wifiOwner() == ResourceOwner::None);
  transport.requestOk = true;

  assert(service.start(10, 60, fakeMs, ctx));
  for (unsigned device = 0; device < 10; ++device) {
    for (unsigned operation = 0; operation < 3; ++operation) {
      transport.complete();
      service.tick(++fakeMs, ctx);
    }
    if (device < 9) {
      fakeMs += 400;
      service.tick(fakeMs, ctx);
      transport.complete();
      service.tick(++fakeMs, ctx);
    }
  }
  assert(service.advertisedCount() == 10);
  assert(memcmp(transport.payload + 23, "LabBLE10", 8) == 0);
  service.stop(fakeMs, ctx);

  assert(service.start(1, 30, fakeMs, ctx, BleAdvertisingProfile::All));
  assert(service.identityCount() == 5);
  const unsigned expectedLengths[] = {17, 26, 31, 31, 18};
  for (unsigned profile = 0; profile < 5; ++profile) {
    for (unsigned operation = 0; operation < 3; ++operation) {
      transport.complete();
      service.tick(++fakeMs, ctx);
    }
    assert(transport.lastConnectable);
    assert(transport.payloadLength == expectedLengths[profile]);
    if (profile < 4) {
      fakeMs += 400;
      service.tick(fakeMs, ctx);
      transport.complete();
      service.tick(++fakeMs, ctx);
    }
  }
  assert(service.advertisedCount() == 5);
  service.stop(fakeMs, ctx);

  assert(service.start(1, 5, fakeMs, ctx));
  transport.endOk = false;
  service.stop(fakeMs, ctx);
  assert(service.isActive() && ctx.resources.ownsWiFi(ResourceOwner::BleAdvertising));
  transport.endOk = true;
  fakeMs += 1000;
  service.tick(fakeMs, ctx);
  assert(!service.isActive() && ctx.resources.wifiOwner() == ResourceOwner::None);

  transport.varySeed = true;
  assert(service.start(1, 30, fakeMs, ctx, BleAdvertisingProfile::Random));
  uint8_t randomAddress[6];
  memcpy(randomAddress, transport.address, sizeof(randomAddress));
  for (unsigned operation = 0; operation < 3; ++operation) {
    transport.complete();
    service.tick(++fakeMs, ctx);
  }
  fakeMs += 400;
  service.tick(fakeMs, ctx);
  transport.complete();
  service.tick(++fakeMs, ctx);
  assert(memcmp(randomAddress, transport.address, sizeof(randomAddress)) != 0);
  assert((transport.address[0] & 0xc0) == 0xc0);
  service.stop(fakeMs, ctx);

  assert(service.start(1, 5, fakeMs, ctx, BleAdvertisingProfile::AppleAction));
  for (unsigned operation = 0; operation < 3; ++operation) {
    transport.complete();
    service.tick(++fakeMs, ctx);
  }
  assert(transport.lastConnectable);
  assert(transport.payloadLength == 14 && transport.payload[5] == 0x4c);
  service.stop(fakeMs, ctx);
  assert(!service.isActive() && ctx.resources.wifiOwner() == ResourceOwner::None);

  assert(service.start(1, 5, fakeMs, ctx, BleAdvertisingProfile::Beacon));
  for (unsigned operation = 0; operation < 3; ++operation) {
    transport.complete();
    service.tick(++fakeMs, ctx);
  }
  assert(!transport.lastConnectable);
  assert(transport.payloadLength == 31 && transport.payload[5] == 0x12);
  service.stop(fakeMs, ctx);
  assert(!service.isActive() && ctx.resources.wifiOwner() == ResourceOwner::None);
  transport.varySeed = false;

  BleLabApp app;
  app.onEnter(ctx);
  app.onTouch(ctx, {140, 48, 0, 0, TouchGesture::Tap, 101});
  assert(app.isSelectingTarget());
  app.onTouch(ctx, {235, 50, 0, 0, TouchGesture::Tap, 101});
  assert(app.isSelectingTarget()); // Ignore the contact which opened the picker.
  app.onTouch(ctx, {235, 50, 0, 0, TouchGesture::Tap, 102});
  assert(!app.isSelectingTarget() && app.selectedProfile() == BleAdvertisingProfile::Windows);
  app.onPrimaryAction(ctx);
  for (unsigned operation = 0; operation < 3; ++operation) {
    transport.complete();
    service.tick(++fakeMs, ctx);
  }
  assert(transport.payloadLength == 26 && transport.payload[5] == 0x06);
  app.onExit(ctx);
  app.onTouch(ctx, {140, 48, 0, 0, TouchGesture::Tap});
  app.onTouch(ctx, {235, 105, 0, 0, TouchGesture::Tap});
  assert(app.selectedProfile() == BleAdvertisingProfile::All);
  app.onPrimaryAction(ctx); // All pairing types, five identities per type.
  assert(service.identityCount() == 25);
  app.onExit(ctx);
  for (unsigned i = 0; i < 20; ++i) {
    app.onEnter(ctx);
    app.onPrimaryAction(ctx);
    assert(service.isActive());
    app.onTick(ctx, fakeMs);
    app.render(ctx);
    app.onExit(ctx);
    assert(!service.isActive() && ctx.resources.wifiOwner() == ResourceOwner::None);
  }
  app.onEnter(ctx);
  app.onTouch(ctx, {140, 48, 0, 0, TouchGesture::Tap});
  app.onTouch(ctx, {235, 133, 0, 0, TouchGesture::Tap});
  assert(app.selectedProfile() == BleAdvertisingProfile::Random);
  app.onPrimaryAction(ctx);
  assert(service.isActive() && service.identityCount() == 1);
  app.onExit(ctx);

  app.onEnter(ctx);
  app.onTouch(ctx, {140, 48, 0, 0, TouchGesture::Tap, 201});
  assert(app.isSelectingTarget());
  app.onTouch(ctx, {295, 15, 0, 0, TouchGesture::Tap, 202});
  app.onTouch(ctx, {235, 50, 0, 0, TouchGesture::Tap, 203});
  assert(app.selectedProfile() == BleAdvertisingProfile::AppleNotYourDevice);
  app.onExit(ctx);

  app.onEnter(ctx);
  app.onTouch(ctx, {140, 48, 0, 0, TouchGesture::Tap, 204});
  app.onTouch(ctx, {295, 15, 0, 0, TouchGesture::Tap, 205});
  app.onTouch(ctx, {20, 45, 0, 0, TouchGesture::Tap, 206});
  assert(app.selectedProfile() == BleAdvertisingProfile::AppleAction);
  app.onExit(ctx);

  app.onEnter(ctx);
  app.onTouch(ctx, {140, 48, 0, 0, TouchGesture::Tap, 207});
  app.onTouch(ctx, {295, 15, 0, 0, TouchGesture::Tap, 208});
  app.onTouch(ctx, {20, 73, 0, 0, TouchGesture::Tap, 209});
  assert(app.selectedProfile() == BleAdvertisingProfile::Beacon);
  app.onExit(ctx);

  app.onTouch(ctx, {140, 48, 0, 0, TouchGesture::Tap});
  assert(app.onBack(ctx) && !app.isSelectingTarget());
  app.onTouch(ctx, {140, 48, 0, 0, TouchGesture::Tap});
  app.onPrimaryAction(ctx);
  app.onActivate(ctx);
  assert(!app.isSelectingTarget());
  app.onExit(ctx);
  ctx.bleAdvertising = nullptr;
  puts("PASS: BLE Lab GAP sequence, payload, timeouts, cleanup and 20 host cycles");
}

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
  BleHidService bleHid;
  WifiDefenseService wifiDefense;
  AppContext context{display, resources, wifi, security, settings, timeService, totpService, usbHid, accountImport, vault, webPortal, bleHid, wifiDefense};
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
  // Summaries include scan records beyond the twelve-entry connection list.
  assert(wifi.startScan(fakeMs));
  wifi.tick(fakeMs + Board::kWiFiWarmupMs);
  WiFi.scans = 20;
  WiFi.complete = true;
  wifi.tick(fakeMs + Board::kWiFiWarmupMs + 1);
  assert(wifi.count() == 12 && wifi.scanResultCount() == 20);
  assert(wifi.channelSummary(6)->accessPoints == 20);
  assert(wifi.channelSummary(6)->strongestRssi == -62);
  assert(wifi.channelSummary(1)->accessPoints == 0);
  assert(wifi.channelSummary(0) == nullptr && wifi.channelSummary(15) == nullptr);
  assert(wifi.startScan(fakeMs));
  assert(wifi.scanResultCount() == 0 && wifi.channelSummary(6)->accessPoints == 0);
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

  // Wi-Fi starts first with modem sleep disabled. BLE needs it throughout its lease.
  assert(!WiFi.sleepEnabled);
  WiFi.canSetSleep = false;
  assert(!bleHid.setEnabled(true, context));
  assert(!bleHid.isEnabled());
  assert(resources.wifiOwner() == ResourceOwner::None);
  WiFi.canSetSleep = true;
  assert(bleHid.setEnabled(true, context));
  assert(WiFi.sleepEnabled);
  bleHid.onClientConnected();
  assert(!bleHid.isConnected());
  assert(!bleHid.typeText("test"));
  bleHid.onPairingCode(12345);
  assert(bleHid.pairingCode() == 12345);
  bleHid.onInputSubscription(true);
  assert(!bleHid.isConnected());
  assert(!bleHid.typeText("test"));
  bleHid.onLinkAuthenticated(true);
  assert(bleHid.pairingCode() == BleHidService::kNoPairingCode);
  assert(bleHid.typeText("test"));
  bleHid.onLinkAuthenticated(false);
  assert(!bleHid.typeText("test"));
  bleHid.onLinkAuthenticated(true);
  assert(bleHid.typeText("test"));
  bleHid.onInputSubscription(false);
  assert(!bleHid.typeText("test"));
  bleHid.onPairingCode(654321);
  bleHid.onClientDisconnected();
  assert(bleHid.pairingCode() == BleHidService::kNoPairingCode);
  bleHid.onInputSubscription(true);
  assert(!bleHid.typeText("test"));
  bleHid.onClientConnected();
  bleHid.onPairingCode(1000000);
  assert(bleHid.pairingCode() == BleHidService::kNoPairingCode);
  bleHid.onLinkAuthenticated(true);
  assert(!bleHid.typeText("test"));
  bleHid.onInputSubscription(true);
  assert(bleHid.typeText("test"));
  assert(wifi.connect("TestNet", "MyPass1234", fakeMs));
  assert(WiFi.sleepEnabled);
  assert(bleHid.setEnabled(false, context));
  assert(!WiFi.sleepEnabled);
  bleHid.onClientDisconnected();
  assert(!bleHid.isEnabled());

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

  // Wi-Fi Defense IDS Host Tests
  wifiDefense.begin();
  assert(!wifiDefense.isRunning());
  assert(wifiDefense.totalPackets() == 0);
  assert(wifiDefense.deauthCount() == 0);

  assert(wifiDefense.start(6, true, fakeMs));
  assert(wifiDefense.isRunning());
  assert(wifiDefense.channel() == 6);
  assert(wifiDefense.autoHop());

  // 1. Inject synthetic 802.11 Deauth packet (0xC0)
  const uint8_t deauthPacket[26] = {
    0xC0, 0x00,                         // Frame Control (Deauth)
    0x00, 0x00,                         // Duration
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Destination (Broadcast)
    0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, // Source / Attacker MAC
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, // BSSID
    0x10, 0x00,                         // Sequence Control
    0x07, 0x00                          // Reason Code 7 (Class 3)
  };
  wifiDefense.processFrame(deauthPacket, sizeof(deauthPacket), -48, 6, fakeMs);
  assert(wifiDefense.totalPackets() == 1);
  assert(wifiDefense.deauthCount() == 1);
  assert(wifiDefense.hasIncident());
  assert(wifiDefense.lastIncident().isDeauth == true);
  assert(wifiDefense.lastIncident().reasonCode == 7);
  assert(wifiDefense.lastIncident().rssi == -48);
  assert(wifiDefense.lastIncident().sourceMac[0] == 0x12 && wifiDefense.lastIncident().sourceMac[5] == 0xBC);

  // 2. Inject synthetic EAPOL handshake packet (Data frame with 0x88 0x8E)
  uint8_t eapolPacket[36] = {
    0x08, 0x00, 0x00, 0x00,
    0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
    0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
    0x20, 0x00
  };
  eapolPacket[24] = 0xAA; eapolPacket[25] = 0xAA; eapolPacket[26] = 0x03;
  eapolPacket[27] = 0x00; eapolPacket[28] = 0x00; eapolPacket[29] = 0x00;
  eapolPacket[30] = 0x88; eapolPacket[31] = 0x8E;
  wifiDefense.processFrame(eapolPacket, sizeof(eapolPacket), -55, 6, fakeMs);
  assert(wifiDefense.totalPackets() == 2);
  assert(wifiDefense.eapolCount() == 1);

  // 3. Channel Mode Cycling
  wifiDefense.cycleChannelMode();
  assert(!wifiDefense.autoHop() && wifiDefense.channel() == 1);
  wifiDefense.cycleChannelMode();
  assert(!wifiDefense.autoHop() && wifiDefense.channel() == 6);
  wifiDefense.cycleChannelMode();
  assert(!wifiDefense.autoHop() && wifiDefense.channel() == 11);
  wifiDefense.cycleChannelMode();
  assert(wifiDefense.autoHop());

  // 4. Attack Rate Triggering
  for (int i = 0; i < 5; ++i) {
    wifiDefense.processFrame(deauthPacket, sizeof(deauthPacket), -50, 6, fakeMs);
  }
  wifiDefense.tick(fakeMs + 1050);
  assert(wifiDefense.deauthRate() >= 5);
  assert(wifiDefense.isUnderAttack());

  // 5. WifiDefenseApp Lifecycle & Clean Exit
  WifiDefenseApp defenseApp;
  defenseApp.onEnter(context);
  defenseApp.onTick(context, fakeMs + 1200);
  defenseApp.onExit(context);

  wifiDefense.resetStats();
  assert(wifiDefense.totalPackets() == 0);
  assert(wifiDefense.deauthCount() == 0);
  assert(!wifiDefense.hasIncident());
  wifiDefense.stop();
  assert(!wifiDefense.isRunning());

  testBlePairingPayloads();
  testBleLab(context);

  {
    BleLabApp ble;
    BeaconLabApp bcn;
    WifiDefenseApp def;
    WiFiChannelsApp ch;
    SecurityLabApp hub(ble, bcn, def, ch);

    hub.onEnter(context);
    assert(hub.activeTool() == nullptr);
    assert(hub.selectedIndex() == 0);

    hub.onPrimaryAction(context);
    assert(hub.selectedIndex() == 1);
    hub.onPrimaryAction(context);
    assert(hub.selectedIndex() == 2);
    hub.onPrimaryAction(context);
    assert(hub.selectedIndex() == 3);
    hub.onPrimaryAction(context);
    assert(hub.selectedIndex() == 0);

    hub.onActivate(context);
    assert(hub.activeTool() == &ble);
    assert(hub.onBack(context) == true);
    assert(hub.activeTool() == nullptr);
    assert(hub.onBack(context) == false);

    hub.onTouch(context, {50, 120, 0, 0, TouchGesture::Tap});
    assert(hub.activeTool() == &def);
    hub.onExit(context);
    assert(hub.activeTool() == nullptr);

    bcn.onEnter(context);
    bcn.forceStart(context);
    bcn.onExit(context);
  }

  puts("PASS: host navigation/resource/async scan/timeout/connect/crypto/settings/defense smoke tests");
}
