#include "platform/desktop/MockWiFiService.h"
#include <cstdio>
#include <cstring>

static constexpr int MOCK_WIFI_STA = 1;
static constexpr int MOCK_WIFI_SCAN_RUNNING = -1;
static constexpr int MOCK_WIFI_SCAN_FAILED = -2;

static constexpr int MOCK_WL_IDLE_STATUS = 0;
static constexpr int MOCK_WL_NO_SSID_AVAIL = 1;
static constexpr int MOCK_WL_SCAN_COMPLETED = 2;
static constexpr int MOCK_WL_CONNECTED = 3;
static constexpr int MOCK_WL_CONNECT_FAILED = 4;
static constexpr int MOCK_WL_DISCONNECTED = 6;

MockWiFiService& MockWiFiService::instance() {
  static MockWiFiService s_instance;
  return s_instance;
}

MockWiFiService::MockWiFiService() {
  resetDefaultNetworks();
}

void MockWiFiService::setSimulationMode(WiFiSimMode mode) {
  mode_ = mode;
  std::printf("[MockWiFi] Simulation mode set to: %s\n", modeName());
}

const char* MockWiFiService::modeName() const {
  switch (mode_) {
    case WiFiSimMode::Normal: return "Normal (Success)";
    case WiFiSimMode::Timeout: return "Timeout (11s delay -> retry)";
    case WiFiSimMode::ScanFailed: return "ScanFailed (Immediate error)";
    case WiFiSimMode::StaError: return "StaError (Mode init fail)";
    case WiFiSimMode::Empty: return "Empty (0 APs found)";
    default: return "Unknown";
  }
}

void MockWiFiService::resetDefaultNetworks() {
  networks_ = {
    {"NibruOS_Lab", -48, 1, true},
    {"Home_5G_Ext", -58, 6, true},
    {"CoffeeShop_Guest", -74, 11, false},
    {"ESP32_Test_AP", -65, 3, true},
    {"Office_IoT", -82, 9, true}
  };
}

void MockWiFiService::setCustomNetworks(const std::vector<SimulatedAP>& aps) {
  networks_ = aps;
}

bool MockWiFiService::onMode(int mode) {
  if (mode == MOCK_WIFI_STA && mode_ == WiFiSimMode::StaError) {
    std::printf("[MockWiFi] Simulating STA mode init failure\n");
    return false;
  }
  return true;
}

int MockWiFiService::onScanNetworks(bool async, uint32_t nowMs) {
  (void)async;
  if (mode_ == WiFiSimMode::ScanFailed) {
    std::printf("[MockWiFi] Simulating immediate scan start failure\n");
    scanning_ = false;
    return MOCK_WIFI_SCAN_FAILED;
  }
  scanning_ = true;
  scanStartedAt_ = nowMs;
  std::printf("[MockWiFi] Scan started at %u ms (mode: %s)\n", nowMs, modeName());
  return MOCK_WIFI_SCAN_RUNNING;
}

int MockWiFiService::onScanComplete(uint32_t nowMs) {
  if (!scanning_) return MOCK_WIFI_SCAN_FAILED;

  if (mode_ == WiFiSimMode::Timeout) {
    // Hangs indefinitely; let WiFiService::tick trigger timeout and retry logic
    return MOCK_WIFI_SCAN_RUNNING;
  }

  if (mode_ == WiFiSimMode::ScanFailed) {
    scanning_ = false;
    return MOCK_WIFI_SCAN_FAILED;
  }

  // Normal or Empty scan: completes in 1200ms
  if (nowMs - scanStartedAt_ < 1200) {
    return MOCK_WIFI_SCAN_RUNNING;
  }

  scanning_ = false;
  if (mode_ == WiFiSimMode::Empty) {
    std::printf("[MockWiFi] Scan completed with 0 APs\n");
    return 0;
  }

  std::printf("[MockWiFi] Scan completed with %zu APs\n", networks_.size());
  return static_cast<int>(networks_.size());
}

void MockWiFiService::onScanDelete() {
  scanning_ = false;
}

int MockWiFiService::getNetworkCount() const {
  return static_cast<int>(networks_.size());
}

const char* MockWiFiService::getSSID(int idx) const {
  if (idx >= 0 && idx < static_cast<int>(networks_.size())) {
    return networks_[idx].ssid.c_str();
  }
  return "";
}

int MockWiFiService::getRSSI(int idx) const {
  if (idx >= 0 && idx < static_cast<int>(networks_.size())) {
    return networks_[idx].rssi;
  }
  return -99;
}

int MockWiFiService::getChannel(int idx) const {
  if (idx >= 0 && idx < static_cast<int>(networks_.size())) {
    return networks_[idx].channel;
  }
  return 1;
}

int MockWiFiService::getEncryptionType(int idx) const {
  if (idx >= 0 && idx < static_cast<int>(networks_.size())) {
    return networks_[idx].secured ? 3 : 0; // 0 = open
  }
  return 0;
}

void MockWiFiService::onConnectBegin(const char* ssid, const char* pass, uint32_t nowMs) {
  targetSsid_ = ssid ? ssid : "";
  targetPass_ = pass ? pass : "";
  connecting_ = true;
  connectStartedAt_ = nowMs;
  connectStatus_ = MOCK_WL_IDLE_STATUS;
  std::printf("[MockWiFi] Connecting to '%s' at %u ms\n", targetSsid_.c_str(), nowMs);
}

int MockWiFiService::getConnectStatus(uint32_t nowMs) {
  if (!connecting_) return connectStatus_;

  // Simulate ~1500ms handshake
  if (nowMs - connectStartedAt_ < 1500) {
    return MOCK_WL_IDLE_STATUS;
  }

  connecting_ = false;
  // If open network or password matches standard simulator test pass "nibru123", connect!
  if (targetSsid_ == "CoffeeShop_Guest" || targetPass_ == "nibru123" || targetPass_.empty()) {
    connectStatus_ = MOCK_WL_CONNECTED;
    std::printf("[MockWiFi] Connected successfully to '%s'\n", targetSsid_.c_str());
  } else {
    connectStatus_ = MOCK_WL_CONNECT_FAILED;
    std::printf("[MockWiFi] Auth failed for '%s'\n", targetSsid_.c_str());
  }
  return connectStatus_;
}

void MockWiFiService::onDisconnect() {
  connecting_ = false;
  connectStatus_ = MOCK_WL_DISCONNECTED;
  std::printf("[MockWiFi] Disconnected\n");
}
