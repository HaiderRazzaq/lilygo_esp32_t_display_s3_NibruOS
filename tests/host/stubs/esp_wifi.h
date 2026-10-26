#pragma once
#include <WiFi.h>
using esp_err_t = int;
static constexpr int ESP_OK=0;
static constexpr int ESP_ERR_WIFI_NOT_STARTED=0x3002;
inline const char* esp_err_to_name(int) { return "TEST"; }
inline int esp_wifi_scan_stop() { ++WiFi.stopped; return ESP_OK; }
