#include "services/BleHidService.h"
#include "core/AppContext.h"
#include "core/ResourceManager.h"
#include <Arduino.h>
#include <string.h>

#if defined(ARDUINO_ARCH_ESP32) && !defined(NIBRU_DESKTOP_SIMULATOR) && !defined(UNIT_TEST)
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <BLEHIDDevice.h>

namespace {
// Standard 8-byte HID keyboard report map
const uint8_t hidReportMap[] = {
  0x05, 0x01,  // USAGE_PAGE (Generic Desktop)
  0x09, 0x06,  // USAGE (Keyboard)
  0xA1, 0x01,  // COLLECTION (Application)
  0x85, 0x01,  //   REPORT_ID (1)
  0x05, 0x07,  //   USAGE_PAGE (Keyboard/Keypad)
  0x19, 0xE0,  //   USAGE_MINIMUM (Keyboard LeftControl)
  0x29, 0xE7,  //   USAGE_MAXIMUM (Keyboard Right GUI)
  0x15, 0x00,  //   LOGICAL_MINIMUM (0)
  0x25, 0x01,  //   LOGICAL_MAXIMUM (1)
  0x75, 0x01,  //   REPORT_SIZE (1)
  0x95, 0x08,  //   REPORT_COUNT (8)
  0x81, 0x02,  //   INPUT (Data,Var,Abs) -- Modifier byte
  0x95, 0x01,  //   REPORT_COUNT (1)
  0x75, 0x08,  //   REPORT_SIZE (8)
  0x81, 0x01,  //   INPUT (Cnst,Ary,Abs) -- Reserved byte
  0x95, 0x05,  //   REPORT_COUNT (5)
  0x75, 0x01,  //   REPORT_SIZE (1)
  0x05, 0x08,  //   USAGE_PAGE (LEDs)
  0x19, 0x01,  //   USAGE_MINIMUM (Num Lock)
  0x29, 0x05,  //   USAGE_MAXIMUM (Kana)
  0x91, 0x02,  //   OUTPUT (Data,Var,Abs) -- LED report
  0x95, 0x01,  //   REPORT_COUNT (1)
  0x75, 0x03,  //   REPORT_SIZE (3)
  0x91, 0x01,  //   OUTPUT (Cnst,Ary,Abs) -- LED padding
  0x95, 0x06,  //   REPORT_COUNT (6)
  0x75, 0x08,  //   REPORT_SIZE (8)
  0x15, 0x00,  //   LOGICAL_MINIMUM (0)
  0x25, 0x65,  //   LOGICAL_MAXIMUM (101)
  0x05, 0x07,  //   USAGE_PAGE (Keyboard/Keypad)
  0x19, 0x00,  //   USAGE_MINIMUM (Reserved)
  0x29, 0x65,  //   USAGE_MAXIMUM (Keyboard Application)
  0x81, 0x00,  //   INPUT (Data,Ary,Abs) -- Key arrays
  0xC0         // END_COLLECTION
};

struct KeyReport8 {
  uint8_t modifiers;
  uint8_t reserved;
  uint8_t keys[6];
};

bool encodeChar(char c, uint8_t& usage, uint8_t& modifiers) {
  modifiers = 0;
  if (c >= 'a' && c <= 'z') { usage = c - 'a' + 4; return true; }
  if (c >= 'A' && c <= 'Z') { usage = c - 'A' + 4; modifiers = 2; return true; }
  if (c >= '1' && c <= '9') { usage = c - '1' + 0x1e; return true; }
  if (c == '0') { usage = 0x27; return true; }
  if (c == '\n' || c == '\r') { usage = 0x28; return true; }
  if (c == '\t') { usage = 0x2b; return true; }
  if (c == ' ') { usage = 0x2c; return true; }
  const char normal[] = "-=[]\\;'\x60,./";
  const char shifted[] = "_+{}|:\"~<>?";
  const uint8_t usages[] = {0x2d,0x2e,0x2f,0x30,0x31,0x33,0x34,0x35,0x36,0x37,0x38};
  for (size_t i = 0; i < sizeof(usages); ++i) {
    if (c == normal[i]) { usage = usages[i]; return true; }
    if (c == shifted[i]) { usage = usages[i]; modifiers = 2; return true; }
  }
  const char symbols[] = "!@#$%^&*()";
  for (unsigned i = 0; i < 10; ++i) {
    if (c == symbols[i]) {
      usage = static_cast<uint8_t>(i == 9 ? 0x27 : 0x1e + i);
      modifiers = 2;
      return true;
    }
  }
  return false;
}

class ServerCallbacks : public BLEServerCallbacks {
public:
  explicit ServerCallbacks(BleHidService& svc) : svc_(svc) {}
  void onConnect(BLEServer* pServer) override {
    (void)pServer;
    svc_.onClientConnected();
  }
  void onDisconnect(BLEServer* pServer) override {
    (void)pServer;
    svc_.onClientDisconnected();
  }
private:
  BleHidService& svc_;
};
} // namespace
#endif

void BleHidService::begin() {
  enabled_ = false;
  state_ = BleConnectionState::Disabled;
  typedCount_ = 0;
  serverPtr_ = nullptr;
  hidPtr_ = nullptr;
  inputReportPtr_ = nullptr;
}

bool BleHidService::setEnabled(bool enable, AppContext& ctx) {
  if (enable == enabled_) return true;

  if (enable) {
    if (!ctx.resources.acquireWiFi(ResourceOwner::BleKeyboard)) {
      return false;
    }
#if defined(ARDUINO_ARCH_ESP32) && !defined(NIBRU_DESKTOP_SIMULATOR) && !defined(UNIT_TEST)
    BLEDevice::init("NibruOS Keyboard");
    BLEServer* pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks(*this));
    serverPtr_ = pServer;

    BLEHIDDevice* pHid = new BLEHIDDevice(pServer);
    BLECharacteristic* pInput = pHid->inputReport(1);
    hidPtr_ = pHid;
    inputReportPtr_ = pInput;

    pHid->manufacturer("NibruOS");
    pHid->pnp(0x02, 0xe502, 0xa111, 0x0210);
    pHid->hidInfo(0x00, 0x01);

    BLESecurity* pSecurity = new BLESecurity();
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);
    pSecurity->setCapability(ESP_IO_CAP_NONE);
    pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
    pSecurity->setRespEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);

    pHid->reportMap(const_cast<uint8_t*>(hidReportMap), sizeof(hidReportMap));
    pHid->startServices();

    BLEAdvertising* pAdvertising = pServer->getAdvertising();
    pAdvertising->setAppearance(HID_KEYBOARD);
    pAdvertising->addServiceUUID(pHid->hidService()->getUUID());
    pAdvertising->setScanResponse(true);
    pAdvertising->start();
    pHid->setBatteryLevel(100);
#endif
    enabled_ = true;
    state_ = BleConnectionState::Advertising;
    return true;
  } else {
#if defined(ARDUINO_ARCH_ESP32) && !defined(NIBRU_DESKTOP_SIMULATOR) && !defined(UNIT_TEST)
    if (serverPtr_) {
      BLEDevice::deinit(false);
      serverPtr_ = nullptr;
      hidPtr_ = nullptr;
      inputReportPtr_ = nullptr;
    }
#endif
    ctx.resources.releaseWiFi(ResourceOwner::BleKeyboard);
    enabled_ = false;
    state_ = BleConnectionState::Disabled;
    return true;
  }
}

bool BleHidService::sendKey(uint8_t usage, uint8_t modifiers) {
  if (!enabled_ || state_ != BleConnectionState::Connected) return false;
#if defined(ARDUINO_ARCH_ESP32) && !defined(NIBRU_DESKTOP_SIMULATOR) && !defined(UNIT_TEST)
  if (!inputReportPtr_) return false;
  BLECharacteristic* pInput = static_cast<BLECharacteristic*>(inputReportPtr_);

  KeyReport8 report{};
  report.modifiers = modifiers;
  report.keys[0] = usage;
  pInput->setValue((uint8_t*)&report, sizeof(report));
  pInput->notify();
  delay(12);

  KeyReport8 empty{};
  pInput->setValue((uint8_t*)&empty, sizeof(empty));
  pInput->notify();
  delay(12);
#else
  (void)usage;
  (void)modifiers;
#endif
  typedCount_++;
  return true;
}

bool BleHidService::typeText(const char* text) {
  if (!enabled_ || state_ != BleConnectionState::Connected || !text) return false;
#if defined(ARDUINO_ARCH_ESP32) && !defined(NIBRU_DESKTOP_SIMULATOR) && !defined(UNIT_TEST)
  for (size_t i = 0; text[i] != '\0'; ++i) {
    uint8_t usage = 0;
    uint8_t modifiers = 0;
    if (!encodeChar(text[i], usage, modifiers)) continue;
    sendKey(usage, modifiers);
  }
#else
  // Desktop simulation
  typedCount_ += static_cast<uint32_t>(strlen(text));
#endif
  return true;
}

void BleHidService::onClientConnected() {
  state_ = BleConnectionState::Connected;
}

void BleHidService::onClientDisconnected() {
  state_ = BleConnectionState::Advertising;
#if defined(ARDUINO_ARCH_ESP32) && !defined(NIBRU_DESKTOP_SIMULATOR) && !defined(UNIT_TEST)
  if (serverPtr_) {
    BLEServer* pServer = static_cast<BLEServer*>(serverPtr_);
    pServer->startAdvertising();
  }
#endif
}

void BleHidService::tick(uint32_t nowMs) {
  (void)nowMs;
}
