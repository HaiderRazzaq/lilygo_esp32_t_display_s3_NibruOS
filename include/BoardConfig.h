#pragma once
#include <stdint.h>
#include "PinDefinitions.h"
#include "NibruConfig.h"

/**
 * @brief Board configuration bridge for NibruOS.
 *
 * Sourced directly from and statically verified against PinDefinitions.h and NibruConfig.h.
 * Preserved for full backward compatibility across legacy code, tests, and tools.
 */
namespace Board {
// Firmware runtime versioning
constexpr const char* kVersion = "v0.21.0";
constexpr const char* kSystemTitle = "NibruOS v0.21.0";

// Hardware pin mapping (statically verified against PinDefinitions.h)
constexpr int kTouchSda = 18;
constexpr int kTouchScl = 17;
constexpr int kTouchReset = 21;
constexpr int kTouchInterrupt = 16;
constexpr int kButtonBoot = 0;
constexpr int kButtonUser = 14;
constexpr int kDisplayPower = 15;
constexpr int kDisplayBacklight = 38;

// Display and input configuration (statically verified against NibruConfig.h)
constexpr int kScreenWidth = 320;
constexpr int kScreenHeight = 170;
constexpr uint8_t kRotation = 1;
constexpr uint32_t kTouchDebounceMs = 180;
constexpr uint32_t kButtonLongPressMs = 650;
constexpr uint8_t kMaxScanResults = 12;

// Wi-Fi subsystem configuration (statically verified against NibruConfig.h)
constexpr uint32_t kWiFiWarmupMs = 650;
constexpr uint32_t kScanTimeoutMs = 11000;
constexpr uint32_t kScanMsPerChannel = 300;
constexpr uint8_t kScanMaxAttempts = 2;
constexpr uint32_t kConnectTimeoutMs = 20000;

// Compile-time static assertions ensuring single source of truth alignment
static_assert(kTouchSda == Pins::TouchSda, "TouchSda mismatch");
static_assert(kTouchScl == Pins::TouchScl, "TouchScl mismatch");
static_assert(kTouchReset == Pins::TouchReset, "TouchReset mismatch");
static_assert(kTouchInterrupt == Pins::TouchInterrupt, "TouchInterrupt mismatch");
static_assert(kButtonBoot == Pins::ButtonBoot, "ButtonBoot mismatch");
static_assert(kButtonUser == Pins::ButtonUser, "ButtonUser mismatch");
static_assert(kDisplayPower == Pins::DisplayPower, "DisplayPower mismatch");
static_assert(kDisplayBacklight == Pins::DisplayBacklight, "DisplayBacklight mismatch");

static_assert(kScreenWidth == Config::ScreenWidth, "ScreenWidth mismatch");
static_assert(kScreenHeight == Config::ScreenHeight, "ScreenHeight mismatch");
static_assert(kRotation == Config::Rotation, "Rotation mismatch");
static_assert(kTouchDebounceMs == Config::TouchDebounceMs, "TouchDebounceMs mismatch");
static_assert(kButtonLongPressMs == Config::ButtonLongPressMs, "ButtonLongPressMs mismatch");
static_assert(kMaxScanResults == Config::MaxScanResults, "MaxScanResults mismatch");
static_assert(kWiFiWarmupMs == Config::WiFiWarmupMs, "WiFiWarmupMs mismatch");
static_assert(kScanTimeoutMs == Config::ScanTimeoutMs, "ScanTimeoutMs mismatch");
static_assert(kScanMsPerChannel == Config::ScanMsPerChannel, "ScanMsPerChannel mismatch");
static_assert(kScanMaxAttempts == Config::ScanMaxAttempts, "ScanMaxAttempts mismatch");
static_assert(kConnectTimeoutMs == Config::ConnectTimeoutMs, "ConnectTimeoutMs mismatch");

} // namespace Board
