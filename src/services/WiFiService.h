#pragma once
#include <stdint.h>

enum class ScanState : uint8_t { Idle, Preparing, Scanning, Complete, Failed };
struct NetworkRecord {
  char ssid[33];
  int16_t rssi;
  int8_t channel;
  bool secured;
};

// Single owner: WiFiScannerApp through ResourceManager. No Wi-Fi calls in UI code.
class WiFiService {
public:
  bool startScan(uint32_t nowMs);
  void tick(uint32_t nowMs);
  void shutdown();
  ScanState state() const { return state_; }
  uint8_t count() const { return count_; }
  const NetworkRecord* network(uint8_t idx) const {
    return idx < count_ ? &networks_[idx] : nullptr;
  }
  uint32_t scanStartedAt() const { return stageStartedAt_; }
  uint8_t attempt() const { return attempt_; }
  const char* lastError() const { return error_; }
private:
  NetworkRecord networks_[12]{};
  ScanState state_ = ScanState::Idle;
  uint8_t count_ = 0;
  uint8_t attempt_ = 0;
  uint32_t stageStartedAt_ = 0;
  const char* error_ = "";
  bool prepareStation(uint32_t nowMs);
  void startHardwareScan(uint32_t nowMs);
  void onFailure(uint32_t nowMs, const char* why);
  void stopHardwareScan();
  void collectResults(int n);
};
