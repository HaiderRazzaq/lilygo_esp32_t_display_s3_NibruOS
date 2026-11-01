#pragma once
#include <stdint.h>
#include <stddef.h>

/**
 * @brief Native ESP32-S3 USB HID Keyboard emulator for safe hardware keystroke injection.
 */
enum class UsbTransferState : uint8_t { Idle, Sending, Sent, Failed };
// Platform transport; reports a USB HID usage and modifier byte (zero = release).
struct UsbHidTransport {
  UsbHidTransport(bool (*readiness)() = nullptr,
                  bool (*sender)(uint8_t, uint8_t) = nullptr,
                  bool (*connection)() = nullptr)
      : ready(readiness), sendKey(sender), connected(connection) {}
  bool (*ready)();
  bool (*sendKey)(uint8_t usage, uint8_t modifiers);
  bool (*connected)(); // Optional: distinguish disconnect from endpoint busy.
};

class UsbHidService {
public:
  explicit UsbHidService(const UsbHidTransport* transport = nullptr) : transport_(transport) {}
  void begin();
  bool isReady() const;
  void tick(uint32_t nowMs);
  void cancel();
  UsbTransferState transferState() const { return state_; }

  // Types given text and optionally presses Enter
  bool typeText(const char* text, bool pressEnter = false);

  // Types numeric code (e.g. 6-digit TOTP) with precise pacing
  bool typeCode(const char* code, bool pressEnter = false);

private:
  bool ready_ = false;
  const UsbHidTransport* transport_ = nullptr;
  UsbTransferState state_ = UsbTransferState::Idle;
  char pending_[65]{};
  uint8_t length_ = 0, position_ = 0;
  bool pressed_ = false;
  bool releasePending_ = false;
  uint32_t lastReportMs_ = 0;
};

