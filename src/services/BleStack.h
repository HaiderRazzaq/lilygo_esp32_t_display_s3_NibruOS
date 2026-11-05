#pragma once
#if defined(ARDUINO_ARCH_ESP32) && !defined(UNIT_TEST) && !defined(NIBRU_DESKTOP_SIMULATOR)
#include <Arduino.h>
#include <BLEDevice.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_heap_caps.h>

namespace BleStack {
inline bool start(const char* name) {
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
}
#endif
