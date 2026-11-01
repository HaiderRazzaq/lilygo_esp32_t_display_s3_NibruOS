#include "services/WiFiService.h"
#include "BoardConfig.h"
#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>
#ifndef UNIT_TEST
#include <esp_wifi.h>

namespace {
uint8_t s_lastDisconnectReason = 0;
const char* s_lastDisconnectReasonName = "";
bool s_eventHookInstalled = false;

void installWiFiEvents() {
  if (s_eventHookInstalled) return;
  s_eventHookInstalled = true;
  WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
    if (event == ARDUINO_EVENT_WIFI_STA_DISCONNECTED) {
      s_lastDisconnectReason = info.wifi_sta_disconnected.reason;
      s_lastDisconnectReasonName = WiFi.disconnectReasonName(static_cast<wifi_err_reason_t>(s_lastDisconnectReason));
      Serial.printf("[WiFiEvent] Disconnected, reason %u: %s\n",
                    s_lastDisconnectReason, s_lastDisconnectReasonName);
    } else if (event == ARDUINO_EVENT_WIFI_STA_CONNECTED) {
      Serial.println("[WiFiEvent] Associated with AP successfully");
    } else if (event == ARDUINO_EVENT_WIFI_STA_GOT_IP) {
      Serial.printf("[WiFiEvent] Acquired IP: %s\n", WiFi.localIP().toString().c_str());
    }
  });
}
}
#endif

void WiFiService::begin(uint32_t nowMs) {
  loadSavedNetworks();
  if (savedCount_ > 0) {
    autoConnect(nowMs);
  }
}

void WiFiService::loadSavedNetworks() {
  Preferences prefs;
  if (prefs.begin("nibru_wifi", true)) {
    savedCount_ = prefs.getUChar("count", 0);
    if (savedCount_ > kMaxSavedNetworks) savedCount_ = kMaxSavedNetworks;
    for (uint8_t i = 0; i < savedCount_; ++i) {
      char sKey[8], pKey[8];
      snprintf(sKey, sizeof(sKey), "s%u", i);
      snprintf(pKey, sizeof(pKey), "p%u", i);
      String s = prefs.getString(sKey, "");
      String p = prefs.getString(pKey, "");
      strncpy(saved_[i].ssid, s.c_str(), sizeof(saved_[i].ssid) - 1);
      saved_[i].ssid[sizeof(saved_[i].ssid) - 1] = '\0';
      strncpy(saved_[i].password, p.c_str(), sizeof(saved_[i].password) - 1);
      saved_[i].password[sizeof(saved_[i].password) - 1] = '\0';
    }
    prefs.end();
    Serial.printf("[WiFi] Loaded %u saved network(s) from NVS\n", savedCount_);
  }
}

void WiFiService::saveSavedNetworks() {
  Preferences prefs;
  if (prefs.begin("nibru_wifi", false)) {
    prefs.putUChar("count", savedCount_);
    for (uint8_t i = 0; i < savedCount_; ++i) {
      char sKey[8], pKey[8];
      snprintf(sKey, sizeof(sKey), "s%u", i);
      snprintf(pKey, sizeof(pKey), "p%u", i);
      prefs.putString(sKey, saved_[i].ssid);
      prefs.putString(pKey, saved_[i].password);
    }
    prefs.end();
    Serial.printf("[WiFi] Saved %u network(s) to NVS\n", savedCount_);
  }
}

const char* WiFiService::getSavedPassword(const char* ssid) const {
  if (!ssid || ssid[0] == '\0') return nullptr;
  for (uint8_t i = 0; i < savedCount_; ++i) {
    if (strcmp(saved_[i].ssid, ssid) == 0) {
      return saved_[i].password;
    }
  }
  return nullptr;
}

bool WiFiService::isSaved(const char* ssid) const {
  return getSavedPassword(ssid) != nullptr;
}

bool WiFiService::saveNetwork(const char* ssid, const char* pass) {
  if (!ssid || ssid[0] == '\0') return false;

  int existingIdx = -1;
  for (uint8_t i = 0; i < savedCount_; ++i) {
    if (strcmp(saved_[i].ssid, ssid) == 0) {
      existingIdx = i;
      break;
    }
  }

  SavedNetwork item{};
  strncpy(item.ssid, ssid, sizeof(item.ssid) - 1);
  if (pass) {
    strncpy(item.password, pass, sizeof(item.password) - 1);
  }

  if (existingIdx >= 0) {
    for (int i = existingIdx; i > 0; --i) {
      saved_[i] = saved_[i - 1];
    }
    saved_[0] = item;
  } else {
    if (savedCount_ < kMaxSavedNetworks) {
      savedCount_++;
    }
    for (int i = static_cast<int>(savedCount_) - 1; i > 0; --i) {
      saved_[i] = saved_[i - 1];
    }
    saved_[0] = item;
  }

  saveSavedNetworks();
  return true;
}

bool WiFiService::forgetNetwork(uint8_t idx) {
  if (idx >= savedCount_) return false;
  for (uint8_t i = idx; i + 1 < savedCount_; ++i) {
    saved_[i] = saved_[i + 1];
  }
  savedCount_--;
  saveSavedNetworks();
  return true;
}

bool WiFiService::forgetNetwork(const char* ssid) {
  if (!ssid) return false;
  for (uint8_t i = 0; i < savedCount_; ++i) {
    if (strcmp(saved_[i].ssid, ssid) == 0) {
      return forgetNetwork(i);
    }
  }
  return false;
}

bool WiFiService::autoConnect(uint32_t nowMs) {
  if (savedCount_ == 0) return false;
  if (connectState_ == ConnectState::Connecting || connectState_ == ConnectState::Connected) {
    return false;
  }
  autoConnectActive_ = true;
  autoConnectIdx_ = 0;
  Serial.printf("[WiFi] Auto-connecting to saved network '%s' (1/%u)...\n",
                saved_[0].ssid, savedCount_);
  return connect(saved_[0].ssid, saved_[0].password, nowMs);
}

bool WiFiService::connectSaved(uint8_t idx, uint32_t nowMs) {
  if (idx >= savedCount_) return false;
  autoConnectActive_ = false;
  autoConnectIdx_ = -1;
  return connect(saved_[idx].ssid, saved_[idx].password, nowMs);
}

bool WiFiService::prepareStation(uint32_t nowMs) {
  WiFi.persistent(false);
  if (!WiFi.mode(WIFI_STA)) {
    state_ = ScanState::Failed;
    error_ = "STA init failed";
    Serial.println("[WiFi] STA initialization failed");
    return false;
  }
  WiFi.setSleep(false);
  if (connectState_ != ConnectState::Connected && connectState_ != ConnectState::Connecting) {
    WiFi.disconnect(false, false);
  }
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
  const int result = WiFi.scanNetworks(true, false, false, Board::kScanMsPerChannel);
  Serial.printf("[WiFi] scanNetworks(async)=%d, freeHeap=%lu\n",
                result, static_cast<unsigned long>(ESP.getFreeHeap()));
  if (result == WIFI_SCAN_FAILED) {
    onFailure(nowMs, "Scan start rejected");
    return;
  }
  stageStartedAt_ = nowMs;
  state_ = ScanState::Scanning;
}

void WiFiService::stopHardwareScan() {
#ifndef UNIT_TEST
  const esp_err_t err = esp_wifi_scan_stop();
  if (err != ESP_OK && err != ESP_ERR_WIFI_NOT_STARTED) {
    Serial.printf("[WiFi] esp_wifi_scan_stop: %s (%d)\n",
                  esp_err_to_name(err), static_cast<int>(err));
  }
#endif
  WiFi.scanDelete();
}

void WiFiService::stopScan() {
  if (state_ == ScanState::Scanning) stopHardwareScan();
  else WiFi.scanDelete();
  state_ = ScanState::Idle;
  attempt_ = 0;
  error_ = "";
}

void WiFiService::onFailure(uint32_t nowMs, const char* why) {
  stopHardwareScan();
  error_ = why;
  Serial.printf("[WiFi] attempt %u/%u failed: %s\n", attempt_, Board::kScanMaxAttempts, why);
  if (attempt_ < Board::kScanMaxAttempts) {
    attempt_++;
    Serial.printf("[WiFi] retrying scan (attempt %u/%u)...\n", attempt_, Board::kScanMaxAttempts);
    prepareStation(nowMs);
    return;
  }
  state_ = ScanState::Failed;
  if (connectState_ == ConnectState::Disconnected) {
    WiFi.mode(WIFI_OFF);
  }
}

void WiFiService::collectResults(int n) {
  const int found = n < 0 ? 0 : n;
  count_ = static_cast<uint8_t>(found > 12 ? 12 : found);
  for (uint8_t i = 0; i < count_; ++i) {
    String s = WiFi.SSID(i);
    strncpy(networks_[i].ssid, s.c_str(), sizeof(networks_[i].ssid) - 1);
    networks_[i].ssid[sizeof(networks_[i].ssid) - 1] = '\0';
    networks_[i].rssi = static_cast<int16_t>(WiFi.RSSI(i));
    networks_[i].channel = static_cast<int8_t>(WiFi.channel(i));
    networks_[i].secured = (WiFi.encryptionType(i) != WIFI_AUTH_OPEN);
  }
  WiFi.scanDelete();
  state_ = ScanState::Complete;
  lastScanDurationMs_ = (stageStartedAt_ > 0 && millis() >= stageStartedAt_) ? (millis() - stageStartedAt_) : 0;
  Serial.printf("[WiFi] Scan complete: %u APs discovered in %lu ms\n", count_, static_cast<unsigned long>(lastScanDurationMs_));
}

const char* WiFiService::statusName() const {
  switch (WiFi.status()) {
    case WL_IDLE_STATUS: return "IDLE";
    case WL_NO_SSID_AVAIL: return "NO_SSID";
    case WL_SCAN_COMPLETED: return "SCAN_DONE";
    case WL_CONNECTED: return "CONNECTED";
    case WL_CONNECT_FAILED: return "AUTH_FAILED";
    case WL_CONNECTION_LOST: return "LOST";
    case WL_DISCONNECTED: return "DISCONNECTED";
    default: return "UNKNOWN";
  }
}

bool WiFiService::connect(const char* ssid, const char* password, uint32_t nowMs) {
  if (!ssid || ssid[0] == '\0') {
    connectState_ = ConnectState::ConnectFailed;
    snprintf(connectErrorBuf_, sizeof(connectErrorBuf_), "Empty SSID");
    return false;
  }

#ifndef UNIT_TEST
  installWiFiEvents();
  s_lastDisconnectReason = 0;
  s_lastDisconnectReasonName = "";
#endif

  WiFi.persistent(false);
  if (!WiFi.mode(WIFI_STA)) {
    connectState_ = ConnectState::ConnectFailed;
    snprintf(connectErrorBuf_, sizeof(connectErrorBuf_), "STA init failed");
    Serial.println("[WiFi] STA initialization failed for connect");
    return false;
  }
  WiFi.setSleep(false);

#ifndef UNIT_TEST
  WiFi.setAutoReconnect(true);
  WiFi.setMinSecurity(WIFI_AUTH_OPEN);
  WiFi.setScanMethod(WIFI_ALL_CHANNEL_SCAN);
  WiFi.setSortMethod(WIFI_CONNECT_AP_BY_SIGNAL);
#endif

  strncpy(targetSsid_, ssid, sizeof(targetSsid_) - 1);
  targetSsid_[sizeof(targetSsid_) - 1] = '\0';
  if (password) {
    strncpy(targetPass_, password, sizeof(targetPass_) - 1);
    targetPass_[sizeof(targetPass_) - 1] = '\0';
  } else {
    targetPass_[0] = '\0';
  }
  connectedSsid_[0] = '\0';
  connectErrorBuf_[0] = '\0';
  connectStartedAt_ = nowMs;
  connectState_ = ConnectState::Connecting;

  wl_status_t beginRes = WL_IDLE_STATUS;
  if (password && password[0] != '\0') {
    Serial.printf("[WiFi] Connecting to '%s' (password length %u)...\n",
                  targetSsid_, static_cast<unsigned>(strlen(password)));
    beginRes = WiFi.begin(targetSsid_, password);
  } else {
    Serial.printf("[WiFi] Connecting to open network '%s'...\n", targetSsid_);
    beginRes = WiFi.begin(targetSsid_);
  }
  Serial.printf("[WiFi] WiFi.begin() returned %d\n", static_cast<int>(beginRes));

#ifndef UNIT_TEST
  if (beginRes == WL_CONNECT_FAILED) {
    Serial.println("[WiFi] begin reported WL_CONNECT_FAILED; triggering esp_wifi_connect");
    esp_err_t err = esp_wifi_connect();
    if (err != ESP_OK) {
      Serial.printf("[WiFi] esp_wifi_connect: %s (%d)\n", esp_err_to_name(err), err);
    }
  }
#endif

  connectStartedAt_ = nowMs;
  return true;
}

void WiFiService::disconnect() {
  autoConnectActive_ = false;
  autoConnectIdx_ = -1;
  if (connectState_ != ConnectState::Disconnected) {
    Serial.println("[WiFi] Disconnecting from AP...");
    WiFi.disconnect(false, false);
    connectState_ = ConnectState::Disconnected;
    connectedSsid_[0] = '\0';
    connectErrorBuf_[0] = '\0';
  }
}

void WiFiService::getLocalIP(char* buffer, size_t maxLen) const {
  if (!buffer || maxLen == 0) return;
  if (connectState_ == ConnectState::Connected) {
    const String ip = WiFi.localIP().toString();
    ip.toCharArray(buffer, maxLen);
  } else {
    buffer[0] = '\0';
  }
}

void WiFiService::getGatewayIP(char* buffer, size_t maxLen) const {
  if (!buffer || maxLen == 0) return;
  if (connectState_ == ConnectState::Connected) {
    const String gw = WiFi.gatewayIP().toString();
    gw.toCharArray(buffer, maxLen);
  } else {
    buffer[0] = '\0';
  }
}

int8_t WiFiService::currentChannel() const {
  return (connectState_ == ConnectState::Connected) ? static_cast<int8_t>(WiFi.channel()) : 0;
}

int16_t WiFiService::currentRSSI() const {
  return (connectState_ == ConnectState::Connected) ? static_cast<int16_t>(WiFi.RSSI()) : 0;
}

void WiFiService::tick(uint32_t nowMs) {
  // 1. Scan state machine
  if (state_ == ScanState::Preparing) {
    if (nowMs < stageStartedAt_) return;
    if (nowMs - stageStartedAt_ >= Board::kWiFiWarmupMs) startHardwareScan(nowMs);
    return;
  }
  if (state_ == ScanState::Scanning) {
    const int n = WiFi.scanComplete();
    if (n >= 0) { collectResults(n); return; }
    if (n == WIFI_SCAN_FAILED) { onFailure(nowMs, "Driver reported scan failure"); return; }
    if (nowMs >= stageStartedAt_ && (nowMs - stageStartedAt_ >= Board::kScanTimeoutMs)) {
      onFailure(nowMs, "Scan timed out");
      return;
    }
  }

  // 2. Connection state machine
  if (connectState_ == ConnectState::Connecting) {
    if (nowMs < connectStartedAt_) return;

    const wl_status_t status = WiFi.status();
    const uint32_t elapsed = nowMs - connectStartedAt_;

    // Check if connected
    IPAddress ip = WiFi.localIP();
    if (status == WL_CONNECTED || (ip[0] != 0 && ip[0] != 255)) {
      connectState_ = ConnectState::Connected;
      strncpy(connectedSsid_, targetSsid_, sizeof(connectedSsid_) - 1);
      connectedSsid_[sizeof(connectedSsid_) - 1] = '\0';
      connectErrorBuf_[0] = '\0';
      autoConnectActive_ = false;
      autoConnectIdx_ = -1;
      saveNetwork(connectedSsid_, targetPass_);
      Serial.printf("[WiFi] Successfully connected to '%s'! IP: %s\n",
                    connectedSsid_, WiFi.localIP().toString().c_str());
      return;
    }

    // Check overall connection timeout
    if (elapsed >= Board::kConnectTimeoutMs) {
      connectState_ = ConnectState::ConnectFailed;
#ifndef UNIT_TEST
      if (s_lastDisconnectReason != 0) {
        snprintf(connectErrorBuf_, sizeof(connectErrorBuf_), "%s (%u)",
                 s_lastDisconnectReasonName, s_lastDisconnectReason);
      } else {
        snprintf(connectErrorBuf_, sizeof(connectErrorBuf_), "Timed out (%u s)",
                 static_cast<unsigned>(elapsed / 1000));
      }
#else
      snprintf(connectErrorBuf_, sizeof(connectErrorBuf_), "Connection timed out");
#endif
      Serial.printf("[WiFi] Connection failed: %s after %lu ms\n",
                    connectErrorBuf_, static_cast<unsigned long>(elapsed));
      WiFi.disconnect(false, false);

      if (autoConnectActive_) {
        autoConnectIdx_++;
        if (autoConnectIdx_ < savedCount_) {
          Serial.printf("[WiFi] Auto-connect trying next saved network: '%s' (%d/%u)...\n",
                        saved_[autoConnectIdx_].ssid, autoConnectIdx_ + 1, savedCount_);
          connect(saved_[autoConnectIdx_].ssid, saved_[autoConnectIdx_].password, nowMs);
          autoConnectActive_ = true;
          return;
        }
        Serial.println("[WiFi] Auto-connect exhausted all saved networks");
        autoConnectActive_ = false;
        autoConnectIdx_ = -1;
      }
      return;
    }

    // Evaluate failure statuses after 8 seconds of connection attempt
    if (elapsed > 8000) {
#ifndef UNIT_TEST
      if (s_lastDisconnectReason == WIFI_REASON_AUTH_FAIL ||
          s_lastDisconnectReason == 14 ||
          s_lastDisconnectReason == 204) {
        connectState_ = ConnectState::ConnectFailed;
        snprintf(connectErrorBuf_, sizeof(connectErrorBuf_), "Wrong Pass (%u)", s_lastDisconnectReason);
        Serial.printf("[WiFi] Fatal auth failure: %s\n", connectErrorBuf_);
        WiFi.disconnect(false, false);

        if (autoConnectActive_) {
          autoConnectIdx_++;
          if (autoConnectIdx_ < savedCount_) {
            connect(saved_[autoConnectIdx_].ssid, saved_[autoConnectIdx_].password, nowMs);
            autoConnectActive_ = true;
            return;
          }
          autoConnectActive_ = false;
          autoConnectIdx_ = -1;
        }
        return;
      }
      if (s_lastDisconnectReason == WIFI_REASON_NO_AP_FOUND && status == WL_NO_SSID_AVAIL) {
        connectState_ = ConnectState::ConnectFailed;
        snprintf(connectErrorBuf_, sizeof(connectErrorBuf_), "AP Not Found (201)");
        Serial.println("[WiFi] Network not in range");
        WiFi.disconnect(false, false);

        if (autoConnectActive_) {
          autoConnectIdx_++;
          if (autoConnectIdx_ < savedCount_) {
            connect(saved_[autoConnectIdx_].ssid, saved_[autoConnectIdx_].password, nowMs);
            autoConnectActive_ = true;
            return;
          }
          autoConnectActive_ = false;
          autoConnectIdx_ = -1;
        }
        return;
      }
#endif
      if (status == WL_CONNECT_FAILED) {
        connectState_ = ConnectState::ConnectFailed;
#ifndef UNIT_TEST
        if (s_lastDisconnectReason != 0) {
          snprintf(connectErrorBuf_, sizeof(connectErrorBuf_), "%s (%u)",
                   s_lastDisconnectReasonName, s_lastDisconnectReason);
        } else {
          snprintf(connectErrorBuf_, sizeof(connectErrorBuf_), "Auth rejected");
        }
#else
        snprintf(connectErrorBuf_, sizeof(connectErrorBuf_), "Wrong pass or auth rejected");
#endif
        Serial.printf("[WiFi] Connection failed (WL_CONNECT_FAILED): %s\n", connectErrorBuf_);
        WiFi.disconnect(false, false);

        if (autoConnectActive_) {
          autoConnectIdx_++;
          if (autoConnectIdx_ < savedCount_) {
            connect(saved_[autoConnectIdx_].ssid, saved_[autoConnectIdx_].password, nowMs);
            autoConnectActive_ = true;
            return;
          }
          autoConnectActive_ = false;
          autoConnectIdx_ = -1;
        }
        return;
      }
      if (status == WL_NO_SSID_AVAIL) {
        connectState_ = ConnectState::ConnectFailed;
        snprintf(connectErrorBuf_, sizeof(connectErrorBuf_), "Network not in range");
        Serial.println("[WiFi] Network not in range (WL_NO_SSID_AVAIL)");
        WiFi.disconnect(false, false);

        if (autoConnectActive_) {
          autoConnectIdx_++;
          if (autoConnectIdx_ < savedCount_) {
            connect(saved_[autoConnectIdx_].ssid, saved_[autoConnectIdx_].password, nowMs);
            autoConnectActive_ = true;
            return;
          }
          autoConnectActive_ = false;
          autoConnectIdx_ = -1;
        }
        return;
      }
    }
  }
}

void WiFiService::shutdown() {
  disconnect();
  stopScan();
  WiFi.mode(WIFI_OFF);
  count_ = 0;
  attempt_ = 0;
  state_ = ScanState::Idle;
  error_ = "";
}
