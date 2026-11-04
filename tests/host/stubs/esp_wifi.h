#pragma once
#include <WiFi.h>
using esp_err_t = int;
static constexpr int ESP_OK=0;
static constexpr int ESP_ERR_WIFI_NOT_STARTED=0x3002;
inline const char* esp_err_to_name(int) { return "TEST"; }
inline int esp_wifi_scan_stop() { ++WiFi.stopped; return ESP_OK; }
inline int esp_wifi_connect() { return ESP_OK; }
enum wifi_ps_type_t { WIFI_PS_NONE, WIFI_PS_MIN_MODEM };
inline esp_err_t esp_wifi_get_ps(wifi_ps_type_t* mode) {
  *mode = WiFi.sleepEnabled ? WIFI_PS_MIN_MODEM : WIFI_PS_NONE;
  return ESP_OK;
}
