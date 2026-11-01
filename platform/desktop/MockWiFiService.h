#pragma once
#include <stdint.h>
#include <vector>
#include <string>

enum class WiFiSimMode : uint8_t {
  Normal,      // Discovers 5 realistic networks in ~1200ms
  Timeout,     // Scan hangs indefinitely, exercising 11s timeout & retry
  ScanFailed,  // Scan fails immediately (WIFI_SCAN_FAILED)
  StaError,    // WiFi.mode(WIFI_STA) fails to initialize
  Empty        // Scan succeeds but discovers 0 APs
};

struct SimulatedAP {
  std::string ssid;
  int16_t rssi;
  int8_t channel;
  bool secured;
};

class MockWiFiService {
public:
  static MockWiFiService& instance();

  void setSimulationMode(WiFiSimMode mode);
  WiFiSimMode simulationMode() const { return mode_; }
  const char* modeName() const;

  // Called by Arduino MockWiFi
  bool onMode(int mode);
  int onScanNetworks(bool async, uint32_t nowMs);
  int onScanComplete(uint32_t nowMs);
  void onScanDelete();

  int getNetworkCount() const;
  const char* getSSID(int idx) const;
  int getRSSI(int idx) const;
  int getChannel(int idx) const;
  int getEncryptionType(int idx) const;

  void onConnectBegin(const char* ssid, const char* pass, uint32_t nowMs);
  int getConnectStatus(uint32_t nowMs);
  void onDisconnect();

  void setCustomNetworks(const std::vector<SimulatedAP>& aps);
  void resetDefaultNetworks();

private:
  MockWiFiService();

  WiFiSimMode mode_ = WiFiSimMode::Normal;
  std::vector<SimulatedAP> networks_;

  bool scanning_ = false;
  uint32_t scanStartedAt_ = 0;

  bool connecting_ = false;
  uint32_t connectStartedAt_ = 0;
  std::string targetSsid_;
  std::string targetPass_;
  int connectStatus_ = 0; // WL_IDLE_STATUS
};
