#include "services/BleHidService.h"
#include "services/WiFiService.h"
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
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_heap_caps.h>

namespace {
bool startBleStack(const char* name) {
  constexpr uint32_t internalCaps = MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT;
  Serial.printf("[BLE] Starting controller (internal heap=%u, largest=%u bytes)\n",
                static_cast<unsigned>(heap_caps_get_free_size(internalCaps)),
                static_cast<unsigned>(heap_caps_get_largest_free_block(internalCaps)));

  // Arduino-ESP32 2.0.14's btStart() spins while IDLE if controller init fails.
  // Initialize explicitly so the error can be handled before calling BLEDevice.
  if (esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_IDLE) {
    esp_bt_controller_config_t config = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    const esp_err_t result = esp_bt_controller_init(&config);
    if (result != ESP_OK) {
      Serial.printf("[BLE] Controller init failed: %s\n", esp_err_to_name(result));
      return false;
    }
  }

  Serial.println("[BLE] Starting Bluedroid host...");
  BLEDevice::init(name);
  if (esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_ENABLED ||
      esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
    Serial.println("[BLE] Controller or Bluedroid failed to start");
    BLEDevice::deinit(false);
    return false;
  }
  return true;
}

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

  void configure(uint16_t hidServiceHandle) {
    hidServiceHandle_ = hidServiceHandle;
    subscriptionHandle_ = 0;
    gattIf_ = ESP_GATT_IF_NONE;
    connected_ = false;
    pendingAuth_ = false;
    pendingCode_ = BleHidService::kNoPairingCode;
  }

  void setSubscriptionHandle(uint16_t handle) { subscriptionHandle_ = handle; }
  esp_gatt_if_t gattInterface() const { return gattIf_.load(); }

  void onConnect(BLEServer*, esp_ble_gatts_cb_param_t* event) override {
    if (connected_) return;
    connId_ = event->connect.conn_id;
    memcpy(peer_, event->connect.remote_bda, sizeof(peer_));
    connected_ = true;
    svc_.onClientConnected(connId_);
    if (memcmp(peer_, pendingCodePeer_, sizeof(peer_)) == 0) {
      svc_.onPairingCode(pendingCode_);
    }
    pendingCode_ = BleHidService::kNoPairingCode;
    if (pendingAuth_ && memcmp(peer_, pendingPeer_, sizeof(peer_)) == 0) {
      svc_.onLinkAuthenticated(true);
    }
    pendingAuth_ = false;
    Serial.printf("[BLE] Connected (id=%u)\n", static_cast<unsigned>(connId_));
  }

  void onDisconnect(BLEServer*, esp_ble_gatts_cb_param_t* event) override {
    if (!connected_ || connId_ != event->disconnect.conn_id ||
        memcmp(peer_, event->disconnect.remote_bda, sizeof(peer_)) != 0) return;
    connected_ = false;
    pendingAuth_ = false;
    pendingCode_ = BleHidService::kNoPairingCode;
    svc_.onClientDisconnected();
    Serial.printf("[BLE] Disconnected (id=%u, reason=0x%02x)\n",
                  static_cast<unsigned>(connId_), static_cast<unsigned>(event->disconnect.reason));
  }

  void onAuthentication(const esp_ble_auth_cmpl_t& auth) {
    if (connected_ && memcmp(peer_, auth.bd_addr, sizeof(peer_)) == 0) {
      svc_.onLinkAuthenticated(auth.success);
    } else if (!connected_) {
      memcpy(pendingPeer_, auth.bd_addr, sizeof(pendingPeer_));
      pendingAuth_ = auth.success;
      pendingCode_ = BleHidService::kNoPairingCode;
    }
  }

  void onPasskey(const uint8_t* address, uint32_t code) {
    if (connected_ && memcmp(peer_, address, sizeof(peer_)) == 0) {
      svc_.onPairingCode(code);
    } else if (!connected_) {
      memcpy(pendingCodePeer_, address, sizeof(pendingCodePeer_));
      pendingCode_ = code;
    }
  }

  void onGattEvent(esp_gatts_cb_event_t event, esp_gatt_if_t gattIf, esp_ble_gatts_cb_param_t* params) {
    if (event == ESP_GATTS_START_EVT && params->start.service_handle == hidServiceHandle_) {
      if (params->start.status == ESP_GATT_OK) gattIf_ = gattIf;
      Serial.printf("[BLE] HID service start result=%u\n", static_cast<unsigned>(params->start.status));
    }
    if (event != ESP_GATTS_WRITE_EVT || !connected_ || !subscriptionHandle_) return;
    const auto& write = params->write;
    if (write.conn_id != connId_ || write.handle != subscriptionHandle_ ||
        write.is_prep || write.offset != 0 || write.len != 2) return;
    const bool subscribed = (write.value[0] & 1) != 0;
    svc_.onInputSubscription(subscribed);
    Serial.printf("[BLE] Keyboard notifications %s\n", subscribed ? "on" : "off");
  }

private:
  BleHidService& svc_;
  esp_bd_addr_t peer_{};
  esp_bd_addr_t pendingPeer_{};
  esp_bd_addr_t pendingCodePeer_{};
  uint32_t pendingCode_ = BleHidService::kNoPairingCode;
  uint16_t connId_ = 0;
  uint16_t hidServiceHandle_ = 0;
  uint16_t subscriptionHandle_ = 0;
  std::atomic<esp_gatt_if_t> gattIf_{ESP_GATT_IF_NONE};
  bool connected_ = false;
  bool pendingAuth_ = false;
};

ServerCallbacks* serverCallbacks = nullptr;

void handleGattEvent(esp_gatts_cb_event_t event, esp_gatt_if_t gattIf, esp_ble_gatts_cb_param_t* params) {
  serverCallbacks->onGattEvent(event, gattIf, params);
}

void logGapEvent(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t* params) {
  if (event == ESP_GAP_BLE_AUTH_CMPL_EVT) {
    const auto& auth = params->ble_security.auth_cmpl;
    serverCallbacks->onAuthentication(auth);
    Serial.printf("[BLE] Authentication %s (reason=0x%02x)\n",
                  auth.success ? "succeeded" : "failed",
                  auth.success ? 0u : static_cast<unsigned>(auth.fail_reason));
  } else if (event == ESP_GAP_BLE_PASSKEY_NOTIF_EVT) {
    const auto& notification = params->ble_security.key_notif;
    serverCallbacks->onPasskey(notification.bd_addr, notification.passkey);
    Serial.println("[BLE] Pairing code shown on device");
  } else if (event == ESP_GAP_BLE_ADV_START_COMPLETE_EVT) {
    Serial.printf("[BLE] Advertising result=%u\n",
                  static_cast<unsigned>(params->adv_start_cmpl.status));
  }
}
} // namespace
#endif

void BleHidService::begin() {
  state_ = BleConnectionState::Disabled;
  typedCount_ = 0;
  linkToken_ = 0;
  pairingCode_ = kNoPairingCode;
  authenticated_ = subscribed_ = false;
  serverPtr_ = nullptr;
  hidPtr_ = nullptr;
  inputReportPtr_ = nullptr;
}

bool BleHidService::setEnabled(bool enable, AppContext& ctx) {
  if (enable == isEnabled()) return true;

  if (enable) {
    if (!ctx.resources.acquireWiFi(ResourceOwner::BleKeyboard)) {
      return false;
    }
    if (!ctx.wifi.setBleCoexistence(true)) {
      Serial.println("[BLE] Wi-Fi modem sleep could not be enabled");
      ctx.resources.releaseWiFi(ResourceOwner::BleKeyboard);
      return false;
    }
#if defined(ARDUINO_ARCH_ESP32) && !defined(NIBRU_DESKTOP_SIMULATOR) && !defined(UNIT_TEST)
    if (!startBleStack(deviceName())) {
      if (esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        ctx.wifi.setBleCoexistence(false);
      }
      ctx.resources.releaseWiFi(ResourceOwner::BleKeyboard);
      return false;
    }

    Serial.println("[BLE] Creating BLE Server...");
    BLEServer* pServer = BLEDevice::createServer();
    serverPtr_ = pServer;

    Serial.println("[BLE] Creating BLE HID Device...");
    BLEHIDDevice* pHid = new BLEHIDDevice(pServer);
    BLECharacteristic* pInput = pHid->inputReport(1);
    auto* inputCccd = pInput->getDescriptorByUUID(BLEUUID(static_cast<uint16_t>(0x2902)));
    BLECharacteristic* pOutput = pHid->outputReport(1);
    const BLEUUID reportReference(static_cast<uint16_t>(0x2908));

    // Windows enumerates the report metadata and CCCD before it may encrypt.
    // The readable input attribute always contains a neutral report.
    pInput->setAccessPermissions(ESP_GATT_PERM_READ);
    pInput->getDescriptorByUUID(reportReference)->setAccessPermissions(ESP_GATT_PERM_READ);
    inputCccd->setAccessPermissions(ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE);
    pOutput->setAccessPermissions(ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE);
    pOutput->getDescriptorByUUID(reportReference)->setAccessPermissions(ESP_GATT_PERM_READ);
    KeyReport8 idleReport{};
    pInput->setValue(reinterpret_cast<uint8_t*>(&idleReport), sizeof(idleReport));
    uint8_t leds = 0;
    pOutput->setValue(&leds, sizeof(leds));

    static ServerCallbacks callbacks(*this);
    callbacks.configure(pHid->hidService()->getHandle());
    serverCallbacks = &callbacks;
    pServer->setCallbacks(&callbacks);
    BLEDevice::setCustomGapHandler(logGapEvent);
    BLEDevice::setCustomGattsHandler(handleGattEvent);
    hidPtr_ = pHid;
    inputReportPtr_ = pInput;

    Serial.println("[BLE] Setting HID Metadata...");
    pHid->manufacturer()->setValue("NibruOS");
    // PnP ID uses little-endian fields. Arduino-ESP32 2.0.14's pnp() writes
    // them in big-endian order, so set the existing characteristic directly.
    uint8_t pnpId[] = {0x02, 0x02, 0xe5, 0x11, 0xa1, 0x10, 0x02};
    pHid->deviceInfo()->getCharacteristic(BLEUUID(static_cast<uint16_t>(0x2a50)))
        ->setValue(pnpId, sizeof(pnpId));
    pHid->hidInfo(0x00, 0x01);

    Serial.println("[BLE] Setting Security...");
    BLESecurity security;
    security.setCapability(ESP_IO_CAP_OUT);
    security.setAuthenticationMode(ESP_LE_AUTH_REQ_SC_MITM_BOND);
    BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT_MITM);

    Serial.println("[BLE] Setting Report Map & Services...");
    pHid->reportMap(const_cast<uint8_t*>(hidReportMap), sizeof(hidReportMap));
    pHid->startServices();
    callbacks.setSubscriptionHandle(inputCccd->getHandle());

    Serial.println("[BLE] Starting Advertising...");
    BLEAdvertising* pAdvertising = pServer->getAdvertising();
    // Keep flags, appearance, HID UUID and the full name in one 29-byte packet.
    BLEAdvertisementData advertisement;
    advertisement.setFlags(ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);
    advertisement.setAppearance(HID_KEYBOARD);
    advertisement.setCompleteServices(pHid->hidService()->getUUID());
    advertisement.setName(deviceName());
    pAdvertising->setAdvertisementData(advertisement);
    pAdvertising->setScanResponse(false);
    state_ = BleConnectionState::Advertising;
    pAdvertising->start();

    Serial.println("[BLE] Setting Battery Level...");
    pHid->setBatteryLevel(100);
    Serial.println("[BLE] BLE Advertising started successfully!");
#else
    state_ = BleConnectionState::Advertising;
#endif
    return true;
  } else {
    state_ = BleConnectionState::Disabled;
    pairingCode_ = kNoPairingCode;
    bool controllerStopped = true;
#if defined(ARDUINO_ARCH_ESP32) && !defined(NIBRU_DESKTOP_SIMULATOR) && !defined(UNIT_TEST)
    if (serverPtr_) {
      BLEDevice::setCustomGapHandler(nullptr);
      BLEDevice::setCustomGattsHandler(nullptr);
      BLEDevice::deinit(false);
      serverPtr_ = nullptr;
      hidPtr_ = nullptr;
      inputReportPtr_ = nullptr;
    }
    controllerStopped = esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_ENABLED;
#endif
    if (controllerStopped) ctx.wifi.setBleCoexistence(false);
    ctx.resources.releaseWiFi(ResourceOwner::BleKeyboard);
    return true;
  }
}

bool BleHidService::sendKey(uint8_t usage, uint8_t modifiers) {
  return sendKeyForLink(usage, modifiers, linkToken_.load());
}

bool BleHidService::sendKeyForLink(uint8_t usage, uint8_t modifiers, uint32_t link) {
  if (!isConnected() || link != linkToken_.load()) return false;
#if defined(ARDUINO_ARCH_ESP32) && !defined(NIBRU_DESKTOP_SIMULATOR) && !defined(UNIT_TEST)
  if (!inputReportPtr_) return false;
  auto* input = static_cast<BLECharacteristic*>(inputReportPtr_);
  const auto gattIf = serverCallbacks->gattInterface();
  if (gattIf == ESP_GATT_IF_NONE) return false;

  KeyReport8 report{};
  report.modifiers = modifiers;
  report.keys[0] = usage;
  const auto connId = static_cast<uint16_t>(link);
  if (esp_ble_gatts_send_indicate(gattIf, connId, input->getHandle(),
      sizeof(report), reinterpret_cast<uint8_t*>(&report), false) != ESP_OK) return false;
  delay(12);

  if (!isConnected() || link != linkToken_.load()) return false;
  KeyReport8 empty{};
  if (esp_ble_gatts_send_indicate(gattIf, connId, input->getHandle(),
      sizeof(empty), reinterpret_cast<uint8_t*>(&empty), false) != ESP_OK) return false;
  delay(12);
  if (!isConnected() || link != linkToken_.load()) return false;
#else
  (void)usage;
  (void)modifiers;
#endif
  typedCount_++;
  return true;
}

bool BleHidService::typeText(const char* text) {
  if (!isConnected() || !text) return false;
#if defined(ARDUINO_ARCH_ESP32) && !defined(NIBRU_DESKTOP_SIMULATOR) && !defined(UNIT_TEST)
  const uint32_t link = linkToken_.load();
  for (size_t i = 0; text[i] != '\0'; ++i) {
    uint8_t usage = 0;
    uint8_t modifiers = 0;
    if (!encodeChar(text[i], usage, modifiers)) continue;
    if (!sendKeyForLink(usage, modifiers, link)) return false;
  }
#else
  // Desktop simulation
  typedCount_ += static_cast<uint32_t>(strlen(text));
#endif
  return true;
}

void BleHidService::onClientConnected(uint16_t connectionId) {
  if (state() != BleConnectionState::Advertising) return;
  authenticated_ = subscribed_ = false;
  pairingCode_ = kNoPairingCode;
  const uint32_t generation = (linkToken_.load() >> 16) + 1;
  linkToken_.store((generation << 16) | connectionId);
  auto expected = BleConnectionState::Advertising;
  state_.compare_exchange_strong(expected, BleConnectionState::Linking);
}

void BleHidService::onLinkAuthenticated(bool authenticated) {
  pairingCode_ = kNoPairingCode;
  authenticated_ = authenticated;
  updateReadiness();
}

void BleHidService::onPairingCode(uint32_t code) {
  if (state() == BleConnectionState::Linking && code <= 999999) pairingCode_ = code;
}

void BleHidService::onInputSubscription(bool subscribed) {
  subscribed_ = subscribed;
  updateReadiness();
}

void BleHidService::updateReadiness() {
  const bool ready = authenticated_ && subscribed_;
  auto expected = ready ? BleConnectionState::Linking : BleConnectionState::Connected;
  const auto next = ready ? BleConnectionState::Connected : BleConnectionState::Linking;
  state_.compare_exchange_strong(expected, next);
}

void BleHidService::onClientDisconnected() {
  auto expected = state();
  if ((expected != BleConnectionState::Linking && expected != BleConnectionState::Connected) ||
      !state_.compare_exchange_strong(expected, BleConnectionState::Advertising)) return;
  pairingCode_ = kNoPairingCode;
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
