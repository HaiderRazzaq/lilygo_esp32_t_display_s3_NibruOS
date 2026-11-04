#pragma once
#include <stdint.h>
#include <stddef.h>
#include <atomic>

struct AppContext;

enum class BleConnectionState : uint32_t {
  Disabled = 0,
  Advertising = 1,
  Linking = 2,
  Connected = 3
};

class BleHidService {
public:
  static constexpr uint32_t kNoPairingCode = UINT32_MAX;
  void begin();
  bool isEnabled() const { return state() != BleConnectionState::Disabled; }
  bool isConnected() const { return state() == BleConnectionState::Connected; }
  BleConnectionState state() const { return state_.load(); }
  uint32_t pairingCode() const { return pairingCode_.load(); }
  const char* deviceName() const { return "NibruOS Keyboard"; }

  bool setEnabled(bool enable, AppContext& ctx);
  bool typeText(const char* text);
  bool sendKey(uint8_t usage, uint8_t modifiers = 0);

  void onClientConnected(uint16_t connectionId = 0);
  void onLinkAuthenticated(bool authenticated);
  void onPairingCode(uint32_t code);
  void onInputSubscription(bool subscribed);
  void onClientDisconnected();
  void tick(uint32_t nowMs);
  uint32_t typedCount() const { return typedCount_; }

  // Simulation controls for host tests
  void simulateConnected(bool connected) {
    if (connected) {
      onClientConnected();
      onLinkAuthenticated(true);
      onInputSubscription(true);
    } else {
      onClientDisconnected();
    }
  }

private:
  bool sendKeyForLink(uint8_t usage, uint8_t modifiers, uint32_t link);
  void updateReadiness();
  std::atomic<BleConnectionState> state_{BleConnectionState::Disabled};
  std::atomic<uint32_t> linkToken_{0};
  std::atomic<uint32_t> pairingCode_{kNoPairingCode};
  // The BLE event task owns these flags. The UI reads the published state.
  bool authenticated_ = false;
  bool subscribed_ = false;
  uint32_t typedCount_ = 0;
  void* serverPtr_ = nullptr;
  void* hidPtr_ = nullptr;
  void* inputReportPtr_ = nullptr;
};
