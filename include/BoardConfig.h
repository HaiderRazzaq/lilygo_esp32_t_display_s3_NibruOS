#pragma once
#include <stdint.h>

namespace Board {
// Firmware runtime versioning
constexpr const char* kVersion = "v0.3";
constexpr const char* kSystemTitle = "NibruOS v0.3";

// WORKING baseline supplied by the owner; confirmed on their Touch Glass board.
constexpr int kTouchSda = 18;
constexpr int kTouchScl = 17;
constexpr int kTouchReset = 21;
constexpr int kTouchInterrupt = 16;
constexpr int kButtonBoot = 0;
constexpr int kButtonUser = 14;
constexpr int kDisplayPower = 15;
constexpr int kDisplayBacklight = 38;
constexpr int kScreenWidth = 320;
constexpr int kScreenHeight = 170;
constexpr uint8_t kRotation = 1;
constexpr uint32_t kTouchDebounceMs = 180;
constexpr uint32_t kButtonLongPressMs = 650;
constexpr uint8_t kMaxScanResults = 12;
// Measured in milliseconds; MAX_ATTEMPTS includes the first attempt.
constexpr uint32_t kWiFiWarmupMs = 650;
constexpr uint32_t kScanTimeoutMs = 11000;
constexpr uint32_t kScanMsPerChannel = 300;
constexpr uint8_t kScanMaxAttempts = 2;
constexpr uint32_t kConnectTimeoutMs = 20000;
}
