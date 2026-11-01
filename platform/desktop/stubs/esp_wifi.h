#pragma once
#include <stdint.h>

using esp_err_t = int32_t;
static constexpr esp_err_t ESP_OK = 0;
static constexpr esp_err_t ESP_ERR_WIFI_NOT_STARTED = 0x3001;

inline esp_err_t esp_wifi_scan_stop() { return ESP_OK; }
inline const char* esp_err_to_name(esp_err_t) { return "ESP_OK"; }
