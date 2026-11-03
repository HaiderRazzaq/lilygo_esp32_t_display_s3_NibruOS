#include "services/UsbHidService.h"
#include "services/CryptoService.h"
#include <Arduino.h>
#include <string.h>

#if defined(ARDUINO_ARCH_ESP32) && defined(ARDUINO_USB_MODE) && ARDUINO_USB_MODE == 0
#include "USB.h"
#include "USBHIDKeyboard.h"
#include "tusb.h"
namespace {
USBHIDKeyboard keyboard;
USBHID hid;
bool nativeReady() { return hid.ready(); }
bool nativeConnected() { return tud_mounted() && !tud_suspended(); }
bool nativeSend(uint8_t usage, uint8_t modifiers) {
  KeyReport report{};
  report.modifiers = modifiers;
  report.keys[0] = usage;
  // Arduino SendReport waits synchronously even with timeout=0. Enqueue directly;
  // this service is the sole HID writer and polls endpoint readiness in tick().
  return tud_hid_n_report(0, HID_REPORT_ID_KEYBOARD, &report, sizeof(report));
}
const UsbHidTransport nativeTransport{nativeReady, nativeSend, nativeConnected};
}
#endif

namespace {
bool encode(char c, uint8_t& usage, uint8_t& modifiers) {
  modifiers = 0;
  if (c >= 'a' && c <= 'z') { usage = c - 'a' + 4; return true; }
  if (c >= 'A' && c <= 'Z') { usage = c - 'A' + 4; modifiers = 2; return true; }
  if (c >= '1' && c <= '9') { usage = c - '1' + 0x1e; return true; }
  if (c == '0') { usage = 0x27; return true; }
  if (c == '\n') { usage = 0x28; return true; }
  if (c == '\t') { usage = 0x2b; return true; }
  const char normal[] = " -=[]\\;'\x60,./";
  const char shifted[] = " _+{}|:\"~<>?";
  const uint8_t usages[] = {0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x33,0x34,0x35,0x36,0x37,0x38};
  for (size_t i = 0; i < sizeof(usages); ++i) {
    if (c == normal[i]) { usage = usages[i]; return true; }
    if (i && c == shifted[i]) { usage = usages[i]; modifiers = 2; return true; }
  }
  const char symbols[] = "!@#$%^&*()";
  for (unsigned i = 0; i < 10; ++i) if (c == symbols[i]) {
    usage = static_cast<uint8_t>(i == 9 ? 0x27 : 0x1e + i); modifiers = 2; return true;
  }
  return false;
}
}

void UsbHidService::begin() {
  cancel();
  if (transport_) { ready_ = transport_->ready && transport_->sendKey; return; }
#if defined(ARDUINO_ARCH_ESP32) && defined(ARDUINO_USB_MODE) && ARDUINO_USB_MODE == 0
  keyboard.begin();
  ready_ = USB.begin();
  transport_ = &nativeTransport;
#else
  // USB Serial/JTAG mode (default) cannot also own the PHY as TinyUSB HID.
  ready_ = false;
#endif
}

bool UsbHidService::isReady() const {
  return ready_ && transport_ && transport_->ready && transport_->sendKey &&
      (!transport_->connected || transport_->connected()) && transport_->ready();
}

void UsbHidService::cancel() {
  releasePending_ = releasePending_ || pressed_;
  if (releasePending_ && isReady() && transport_->sendKey(0, 0)) releasePending_ = false;
  CryptoService::secureZero(pending_, sizeof(pending_));
  length_ = position_ = 0;
  pressed_ = false;
  state_ = UsbTransferState::Idle;
}

bool UsbHidService::typeText(const char* text, bool pressEnter) {
  if (!text || !isReady() || releasePending_ || state_ == UsbTransferState::Sending) return false;
  const size_t len = strlen(text);
  if (!len || len + (pressEnter ? 1U : 0U) >= sizeof(pending_)) return false;
  for (size_t i = 0; i < len; ++i) {
    uint8_t usage = 0, modifiers = 0;
    if (!encode(text[i], usage, modifiers)) return false;
  }
  cancel();
  memcpy(pending_, text, len);
  length_ = static_cast<uint8_t>(len);
  if (pressEnter) pending_[length_++] = '\n';
  state_ = UsbTransferState::Sending;
  lastReportMs_ = millis() - 15U;
  return true; // Accepted for asynchronous transmission, not yet sent.
}

bool UsbHidService::typeCode(const char* code, bool pressEnter) {
  if (!code) return false;
  const size_t len = strlen(code);
  if (len < 6 || len > 8) return false;
  for (size_t i = 0; i < len; ++i) if (code[i] < '0' || code[i] > '9') return false;
  return typeText(code, pressEnter);
}

void UsbHidService::tick(uint32_t nowMs) {
  if (releasePending_) {
    if (isReady() && transport_->sendKey(0, 0)) releasePending_ = false;
    return;
  }
  if (state_ != UsbTransferState::Sending || nowMs - lastReportMs_ < 15U) return;
  if (transport_->connected && !transport_->connected()) {
    cancel(); state_ = UsbTransferState::Failed; return;
  }
  // Busy is not a failed transfer. Await completion without blocking the UI.
  if (!isReady()) {
    if (nowMs - lastReportMs_ > 500U) { cancel(); state_ = UsbTransferState::Failed; }
    return;
  }
  if (position_ >= length_) {
    CryptoService::secureZero(pending_, sizeof(pending_));
    length_ = position_ = 0;
    state_ = UsbTransferState::Sent;
    return;
  }
  lastReportMs_ = nowMs;
  uint8_t usage = 0, modifiers = 0;
  const bool encoded = pressed_ || encode(pending_[position_], usage, modifiers);
  if (!encoded || !isReady() || !transport_->sendKey(usage, modifiers)) {
    cancel();
    state_ = UsbTransferState::Failed;
    return;
  }
  if (!pressed_) pressed_ = true;
  else {
    pressed_ = false;
    ++position_;
  }
}
