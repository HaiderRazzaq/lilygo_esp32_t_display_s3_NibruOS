#pragma once
#include <stdint.h>
#include <stddef.h>

enum class ScanState : uint8_t { Idle, Preparing, Scanning, Complete, Failed };
enum class ConnectState : uint8_t { Disconnected, Connecting, Connected, ConnectFailed };

struct NetworkRecord {
  char ssid[33];
  int16_t rssi;
  int8_t channel;
  bool secured;
};

struct SavedNetwork {
  char ssid[33];
  char password[65];
};

static constexpr uint8_t kMaxSavedNetworks = 5;

// Single owner: WiFiScannerApp through ResourceManager. No Wi-Fi calls in UI code.
class WiFiService {
public:
  void begin(uint32_t nowMs = 0);
  bool startScan(uint32_t nowMs);
  void stopScan();
  void tick(uint32_t nowMs);
  void shutdown();
  bool setBleCoexistence(bool required);
  static constexpr uint8_t kBeaconMaxCount = 40;
  static constexpr uint8_t kBeaconMinSeconds = 5;
  static constexpr uint8_t kBeaconMaxSeconds = 60;
  bool startBeaconLab(uint8_t openCount, uint8_t securedCount, uint8_t seconds, uint32_t nowMs);
  void tickBeaconLab(uint32_t nowMs);
  void stopBeaconLab();
  bool beaconActive() const { return beaconCount_ != 0; }
  uint8_t beaconAcceptedNames() const { return beaconAcceptedNames_; }
  ScanState state() const { return state_; }
  uint8_t count() const { return count_; }
  const NetworkRecord* network(uint8_t idx) const {
    return idx < count_ ? &networks_[idx] : nullptr;
  }
  uint32_t scanStartedAt() const { return stageStartedAt_; }
  uint32_t lastScanDurationMs() const { return lastScanDurationMs_; }
  uint8_t attempt() const { return attempt_; }
  const char* lastError() const { return error_; }

  // Connection support
  bool connect(const char* ssid, const char* password, uint32_t nowMs);
  void disconnect();
  ConnectState connectState() const { return connectState_; }
  bool isConnected() const { return connectState_ == ConnectState::Connected; }
  const char* connectedSSID() const { return connectedSsid_; }
  const char* targetSSID() const { return targetSsid_; }
  const char* connectError() const { return connectErrorBuf_; }
  const char* statusName() const;
  uint32_t connectStartedAt() const { return connectStartedAt_; }
  void getLocalIP(char* buffer, size_t maxLen) const;
  void getGatewayIP(char* buffer, size_t maxLen) const;
  void getMacAddress(char* buffer, size_t maxLen) const;
  int8_t currentChannel() const;
  int16_t currentRSSI() const;

  // Saved networks & persistence
  void loadSavedNetworks();
  void saveSavedNetworks();
  uint8_t savedCount() const { return savedCount_; }
  const SavedNetwork* savedNetwork(uint8_t idx) const {
    return idx < savedCount_ ? &saved_[idx] : nullptr;
  }
  const char* getSavedPassword(const char* ssid) const;
  bool isSaved(const char* ssid) const;
  bool saveNetwork(const char* ssid, const char* pass);
  bool forgetNetwork(uint8_t idx);
  bool forgetNetwork(const char* ssid);
  bool autoConnect(uint32_t nowMs);
  bool connectSaved(uint8_t idx, uint32_t nowMs);

private:
  NetworkRecord networks_[12]{};
  ScanState state_ = ScanState::Idle;
  uint8_t count_ = 0;
  uint8_t attempt_ = 0;
  uint32_t stageStartedAt_ = 0;
  uint32_t lastScanDurationMs_ = 0;
  const char* error_ = "";
  bool bleCoexistence_ = false;
  uint32_t beaconStarted_ = 0;
  uint32_t nextBeacon_ = 0;
  uint32_t beaconSeed_ = 0;
  uint32_t beaconDurationMs_ = 0;
  uint8_t beaconCount_ = 0;
  uint8_t beaconOpenCount_ = 0;
  uint8_t beaconAcceptedNames_ = 0;
  uint8_t beaconIndex_ = 0;
  uint8_t beaconChannel_ = 0;
  bool beaconOwnsStation_ = false;

  // Connect state
  ConnectState connectState_ = ConnectState::Disconnected;
  char targetSsid_[33]{};
  char targetPass_[65]{};
  char connectedSsid_[33]{};
  char connectErrorBuf_[48]{};
  uint32_t connectStartedAt_ = 0;

  // Saved networks & auto-connect
  SavedNetwork saved_[kMaxSavedNetworks]{};
  uint8_t savedCount_ = 0;
  int8_t autoConnectIdx_ = -1;
  bool autoConnectActive_ = false;

  bool prepareStation(uint32_t nowMs);
  void startHardwareScan(uint32_t nowMs);
  void onFailure(uint32_t nowMs, const char* why);
  void stopHardwareScan();
  void collectResults(int n);
  bool transmitBeacon();
};
