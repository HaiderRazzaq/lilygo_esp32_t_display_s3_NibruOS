#include "services/BleAdvertisingService.h"
#include "services/BleStack.h"
#include "services/WiFiService.h"
#include "core/AppContext.h"
#include "core/ResourceManager.h"
#include <cstdio>
#include <cstring>
#include <atomic>
#if defined(ARDUINO_ARCH_ESP32) && !defined(UNIT_TEST) && !defined(NIBRU_DESKTOP_SIMULATOR)
#include <esp_random.h>
#endif

namespace {
#if defined(ARDUINO_ARCH_ESP32) && !defined(UNIT_TEST) && !defined(NIBRU_DESKTOP_SIMULATOR)
class EspBleAdvertiser final : public BleAdvertisingTransport {
public:
  bool begin() override;
  bool setAddress(const uint8_t* address) override {
    return esp_ble_gap_set_rand_addr(const_cast<uint8_t*>(address)) == ESP_OK;
  }
  bool setData(const uint8_t* bytes, size_t length) override {
    return esp_ble_gap_config_adv_data_raw(const_cast<uint8_t*>(bytes), length) == ESP_OK;
  }
  bool start(bool connectable) override {
    esp_ble_adv_params_t params{};
    params.adv_int_min = 160; // 100 ms, in units of 0.625 ms.
    params.adv_int_max = 160;
    params.adv_type = connectable ? ADV_TYPE_IND : ADV_TYPE_NONCONN_IND;
    params.own_addr_type = BLE_ADDR_TYPE_RANDOM;
    params.channel_map = ADV_CHNL_ALL;
    params.adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;
    return esp_ble_gap_start_advertising(&params) == ESP_OK;
  }
  bool stop() override { return esp_ble_gap_stop_advertising() == ESP_OK; }
  bool end() override {
    BLEDevice::setCustomGapHandler(nullptr);
    BLEDevice::deinit(false);
    // Also clean up an incomplete startup for which BLEDevice never set its
    // initialized flag. Keep the radio leased if any layer refuses teardown.
    if (esp_bluedroid_get_status() == ESP_BLUEDROID_STATUS_ENABLED) esp_bluedroid_disable();
    if (esp_bluedroid_get_status() == ESP_BLUEDROID_STATUS_INITIALIZED) esp_bluedroid_deinit();
    if (esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_ENABLED) esp_bt_controller_disable();
    if (esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_INITED) esp_bt_controller_deinit();
    pending_.store(0);
    return esp_bluedroid_get_status() == ESP_BLUEDROID_STATUS_UNINITIALIZED &&
           esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_IDLE;
  }
  uint32_t randomSeed() override { return esp_random(); }
  BleAdvertisingEvent poll() override {
    const uint8_t value = pending_.exchange(0);
    return {static_cast<BleAdvertisingOperation>(value >> 1), (value & 1) != 0};
  }
  void complete(BleAdvertisingOperation operation, bool success) {
    pending_.store((static_cast<uint8_t>(operation) << 1) | (success ? 1 : 0));
  }
private:
  std::atomic<uint8_t> pending_{0};
};

EspBleAdvertiser advertiser;
void onGapEvent(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t* params) {
  switch (event) {
    case ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT:
      advertiser.complete(BleAdvertisingOperation::Address, params->set_rand_addr_cmpl.status == ESP_BT_STATUS_SUCCESS);
      break;
    case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
      advertiser.complete(BleAdvertisingOperation::Data, params->adv_data_raw_cmpl.status == ESP_BT_STATUS_SUCCESS);
      break;
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
      advertiser.complete(BleAdvertisingOperation::Start, params->adv_start_cmpl.status == ESP_BT_STATUS_SUCCESS);
      break;
    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
      advertiser.complete(BleAdvertisingOperation::Stop, params->adv_stop_cmpl.status == ESP_BT_STATUS_SUCCESS);
      break;
    default: break;
  }
}

bool EspBleAdvertiser::begin() {
  pending_.store(0);
  if (!BleStack::start("")) return false;
  if (heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT) < 150 * 1024) return false;
  BLEDevice::setCustomGapHandler(onGapEvent);
  return true;
}
#endif

BleAdvertisingTransport* defaultTransport() {
#if defined(ARDUINO_ARCH_ESP32) && !defined(UNIT_TEST) && !defined(NIBRU_DESKTOP_SIMULATOR)
  return &advertiser;
#else
  return nullptr;
#endif
}
}

bool BleAdvertisingService::start(uint8_t count, uint8_t seconds, uint32_t nowMs, AppContext& ctx,
                                  BleAdvertisingProfile profile) {
  if (isActive()) return false;
  error_ = "";
  if (count < 1 || count > 10 || seconds < 5 || seconds > 60 ||
      profile >= BleAdvertisingProfile::Count) {
    error_ = "Invalid count or duration";
    return false;
  }
  if (!transport_) transport_ = defaultTransport();
  if (!transport_) {
    error_ = "BLE radio unavailable";
    return false;
  }
  if (!ctx.resources.acquireWiFi(ResourceOwner::BleAdvertising)) {
    error_ = "Radio busy";
    return false;
  }
  if (!ctx.wifi.setBleCoexistence(true)) {
    ctx.resources.releaseWiFi(ResourceOwner::BleAdvertising);
    error_ = "Wi-Fi coexistence failed";
    return false;
  }
  if (!transport_->begin()) {
    fail("BLE startup or heap limit", nowMs, ctx);
    return false;
  }
  const auto seed = transport_->randomSeed();
  address_[0] = 0xc2; // Static random address, with a distinct suffix per identity.
  for (uint8_t i = 0; i < 4; ++i) address_[i + 1] = static_cast<uint8_t>(seed >> (i * 8));
  profile_ = profile;
  count_ = profile == BleAdvertisingProfile::All ? count * 5 : count;
  advertisedCount_ = index_ = 0;
  startedMs_ = nowMs;
  durationMs_ = seconds * 1000UL;
  configureDevice(nowMs, ctx);
  return isActive() && !error_[0];
}

void BleAdvertisingService::configureDevice(uint32_t nowMs, AppContext& ctx) {
  if (profile_ == BleAdvertisingProfile::Random) {
    const auto seed = transport_->randomSeed();
    for (uint8_t i = 0; i < 4; ++i) address_[i + 1] = static_cast<uint8_t>(seed >> (i * 8));
  }
  address_[5] = index_ + 1;
  payloadLength_ = buildBleAdvertisingPayload(profile_, index_, transport_->randomSeed(), payload_);
  if (!payloadLength_) {
    fail("Invalid advertising profile", nowMs, ctx);
    return;
  }
  phase_ = Phase::Address;
  phaseMs_ = nowMs;
  if (!transport_->setAddress(address_)) fail("Address request rejected", nowMs, ctx);
}

void BleAdvertisingService::fail(const char* error, uint32_t nowMs, AppContext& ctx) {
  error_ = error;
  stop(nowMs, ctx);
}

void BleAdvertisingService::stop(uint32_t nowMs, AppContext& ctx) {
  if (!ctx.resources.ownsWiFi(ResourceOwner::BleAdvertising)) return;
  if (phase_ == Phase::Cleanup && nowMs - phaseMs_ < 1000) return;
  if (!transport_->end()) {
    phase_ = Phase::Cleanup;
    phaseMs_ = nowMs;
    error_ = "BLE shutdown failed";
    return;
  }
  ctx.wifi.setBleCoexistence(false);
  ctx.resources.releaseWiFi(ResourceOwner::BleAdvertising);
  phase_ = Phase::Idle;
}

void BleAdvertisingService::tick(uint32_t nowMs, AppContext& ctx) {
  if (!isActive()) return;
  if (phase_ == Phase::Cleanup) {
    if (nowMs - phaseMs_ >= 1000) stop(nowMs, ctx);
    return;
  }
  if (nowMs - startedMs_ >= durationMs_) {
    stop(nowMs, ctx);
    return;
  }
  if (phase_ == Phase::Advertising) {
    if (nowMs - phaseMs_ < 400) return;
    phase_ = Phase::Stop;
    phaseMs_ = nowMs;
    if (!transport_->stop()) fail("Stop request rejected", nowMs, ctx);
    return;
  }
  const auto event = transport_->poll();
  if (event.operation == BleAdvertisingOperation::None) {
    if (nowMs - phaseMs_ >= 2000) fail("BLE operation timed out", nowMs, ctx);
    return;
  }
  // Only one GAP operation is outstanding. Ignore unrelated completion events.
  const bool expected =
      (phase_ == Phase::Address && event.operation == BleAdvertisingOperation::Address) ||
      (phase_ == Phase::Data && event.operation == BleAdvertisingOperation::Data) ||
      (phase_ == Phase::Start && event.operation == BleAdvertisingOperation::Start) ||
      (phase_ == Phase::Stop && event.operation == BleAdvertisingOperation::Stop);
  if (!expected) return;
  if (!event.success) {
    fail("BLE operation failed", nowMs, ctx);
    return;
  }
  phaseMs_ = nowMs;
  switch (phase_) {
    case Phase::Address:
      phase_ = Phase::Data;
      if (!transport_->setData(payload_, payloadLength_)) fail("Payload request rejected", nowMs, ctx);
      break;
    case Phase::Data:
      phase_ = Phase::Start;
      if (!transport_->start(bleAdvertisingProfileConnectable(profile_)))
        fail("Start request rejected", nowMs, ctx);
      break;
    case Phase::Start:
      phase_ = Phase::Advertising;
      if (advertisedCount_ < index_ + 1) advertisedCount_ = index_ + 1;
      break;
    case Phase::Stop:
      index_ = static_cast<uint8_t>((index_ + 1) % count_);
      configureDevice(nowMs, ctx);
      break;
    default: break;
  }
}
