#include "services/UsbHidService.h"
#include <Arduino.h>

#if defined(ARDUINO_USB_MODE) && (ARDUINO_USB_MODE == 1)
#include "USB.h"
#include "USBHIDKeyboard.h"

namespace {
USBHIDKeyboard s_keyboard;
}

void UsbHidService::begin() {
  s_keyboard.begin();
  USB.begin();
  ready_ = true;
  Serial.println("[UsbHidService] Native USB HID Keyboard initialized.");
}

bool UsbHidService::typeText(const char* text, bool pressEnter) {
  if (!ready_ || !text) return false;
  s_keyboard.print(text);
  if (pressEnter) {
    s_keyboard.write(KEY_RETURN);
  }
  return true;
}

bool UsbHidService::typeCode(const char* code, bool pressEnter) {
  if (!ready_ || !code) return false;
  for (size_t i = 0; code[i] != '\0'; ++i) {
    s_keyboard.write(static_cast<uint8_t>(code[i]));
    delay(15);
  }
  if (pressEnter) {
    delay(20);
    s_keyboard.write(KEY_RETURN);
  }
  return true;
}

#else

void UsbHidService::begin() {
  ready_ = false;
}

bool UsbHidService::typeText(const char*, bool) {
  return false;
}

bool UsbHidService::typeCode(const char*, bool) {
  return false;
}

#endif

