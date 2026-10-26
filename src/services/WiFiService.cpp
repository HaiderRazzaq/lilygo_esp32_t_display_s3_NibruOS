#include "services/WiFiService.h"
#include "BoardConfig.h"
#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>

bool WiFiService::prepareStation(uint32_t nowMs) {
  // Do not start a scan in the same instant that the driver is enabled.
  WiFi.persistent(false);
  if (!WiFi.mode(WIFI_STA)) {
    state_ = ScanState::Failed;
    error_ = "STA init failed";
    Serial.println("[WiFi] STA initialization failed");
    return false;
  }
  WiFi.setSleep(false); // Keep the station awake during a short foreground scan.
  WiFi.disconnect(false, false); // No attempt to join any remembered AP.
  WiFi.scanDelete();
  stageStartedAt_ = nowMs;
  state_ = ScanState::Preparing;
  Serial.printf("[WiFi] attempt %u/%u: STA preparing (%u ms warmup)\n",
                attempt_, Board::kScanMaxAttempts, Board::kWiFiWarmupMs);
  return true;
}

bool WiFiService::startScan(uint32_t nowMs) {
  if (state_ == ScanState::Preparing || state_ == ScanState::Scanning) return false;
  count_ = 0;
  error_ = "";
  attempt_ = 1;
  return prepareStation(nowMs);
}

void WiFiService::startHardwareScan(uint32_t nowMs) {
  // Bounded active scan; WiFiScan starts async and reports completion in scanComplete().
  const int result = WiFi.scanNetworks(true, false, false, Board::kScanMsPerChannel);
  Serial.printf("[WiFi] scanNetworks(async)=%d, freeHeap=%lu\n",
                result, static_cast<unsigned long>(ESP.getFreeHeap()));
  if (result == WIFI_SCAN_FAILED) {
    onFailure(nowMs, "Scan start rejected");
    return;
  }
  stageStartedAt_ = nowMs;
  state_ = ScanState::Scanning;
  // A cached/instant result is retrieved through the next tick.
}

void WiFiService::stopHardwareScan() {
  // scanDelete() only frees Arduino result memory; it does NOT stop a scan.
  // Stop the underlying IDF scan first to permit safe retries and app exit.
  const esp_err_t err = esp_wifi_scan_stop();
  if (err != ESP_OK && err != ESP_ERR_WIFI_NOT_STARTED) {
    Serial.printf("[WiFi] esp_wifi_scan_stop: %s (%d)\n",
                  esp_err_to_name(err), static_cast<int>(err));
  }
  WiFi.scanDelete();
}

void WiFiService::onFailure(uint32_t nowMs, const char* why) {
  Serial.printf("[WiFi] attempt %u/%u failed: %s (elapsed %lu ms)\n",
                attempt_, Board::kScanMaxAttempts, why,
                static_cast<unsigned long>(nowMs - stageStartedAt_));
  stopHardwareScan();
  WiFi.mode(WIFI_OFF);
  if (attempt_ < Board::kScanMaxAttempts) {
    ++attempt_;
    // A fresh radio/STA startup is intentional after a driver failure.
    if (prepareStation(nowMs)) return;
  }
  state_ = ScanState::Failed;
  error_ = why;
  Serial.println("[WiFi] scan failed after retries; check 115200 serial log");
}

void WiFiService::collectResults(int n) {
  count_ = static_cast<uint8_t>(n < Board::kMaxScanResults ? n : Board::kMaxScanResults);
  for (uint8_t i = 0; i < count_; ++i) {
    const String ssid = WiFi.SSID(i);
    ssid.toCharArray(networks_[i].ssid, sizeof(networks_[i].ssid));
    networks_[i].rssi = static_cast<int16_t>(WiFi.RSSI(i));
    networks_[i].channel = static_cast<int8_t>(WiFi.channel(i));
    networks_[i].secured = WiFi.encryptionType(i) != WIFI_AUTH_OPEN;
  }
  WiFi.scanDelete();
  state_ = ScanState::Complete;
  Serial.printf("[WiFi] complete: %u stored / %d reported, attempt %u\n",
                count_, n, attempt_);
}

void WiFiService::tick(uint32_t nowMs) {
  if (state_ == ScanState::Preparing) {
    if (nowMs - stageStartedAt_ >= Board::kWiFiWarmupMs) startHardwareScan(nowMs);
    return;
  }
  if (state_ != ScanState::Scanning) return;
  // Poll completion BEFORE local timeout to avoid falsely discarding finished results.
  const int n = WiFi.scanComplete();
  if (n >= 0) { collectResults(n); return; }
  if (n == WIFI_SCAN_FAILED) { onFailure(nowMs, "Driver reported scan failure"); return; }
  if (nowMs - stageStartedAt_ >= Board::kScanTimeoutMs) {
    onFailure(nowMs, "Scan timed out");
    return;
  }
}

void WiFiService::shutdown() {
  if (state_ == ScanState::Scanning) stopHardwareScan();
  else WiFi.scanDelete();
  WiFi.mode(WIFI_OFF);
  count_ = 0;
  attempt_ = 0;
  state_ = ScanState::Idle;
  error_ = "";
}
