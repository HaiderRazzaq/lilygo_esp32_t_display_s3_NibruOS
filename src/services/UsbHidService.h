#pragma once
#include <stdint.h>
#include <stddef.h>

/**
 * @brief Native ESP32-S3 USB HID Keyboard emulator for safe hardware keystroke injection.
 */
class UsbHidService {
public:
  void begin();
  bool isReady() const { return ready_; }

  // Types given text and optionally presses Enter
  bool typeText(const char* text, bool pressEnter = false);

  // Types numeric code (e.g. 6-digit TOTP) with precise pacing
  bool typeCode(const char* code, bool pressEnter = false);

private:
  bool ready_ = false;
};

