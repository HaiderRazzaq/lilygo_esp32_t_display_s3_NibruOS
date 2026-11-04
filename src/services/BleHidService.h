#pragma once
#include <stdint.h>
#include <stddef.h>

struct AppContext;

enum class BleConnectionState : uint8_t {
  Disabled = 0,
  Advertising = 1,
  Connected = 2
};

class BleHidService {
public:
  void begin();
  bool isEnabled() const { return enabled_; }
  bool isConnected() const { return state_ == BleConnectionState::Connected; }
  BleConnectionState state() const { return state_; }
  const char* deviceName() const { return "NibruOS Keyboard"; }

  bool setEnabled(bool enable, AppContext& ctx);
  bool typeText(const char* text);
  bool sendKey(uint8_t usage, uint8_t modifiers = 0);

  void onClientConnected();
  void onClientDisconnected();
  void tick(uint32_t nowMs);
  uint32_t typedCount() const { return typedCount_; }

  // Simulation controls for host tests
  void simulateConnected(bool connected) {
    if (enabled_) state_ = connected ? BleConnectionState::Connected : BleConnectionState::Advertising;
  }

private:
  bool enabled_ = false;
  BleConnectionState state_ = BleConnectionState::Disabled;
  uint32_t typedCount_ = 0;
  void* serverPtr_ = nullptr;
  void* hidPtr_ = nullptr;
  void* inputReportPtr_ = nullptr;
};
