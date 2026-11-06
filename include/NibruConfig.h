#pragma once
#include <stdint.h>
#include "PinDefinitions.h"

/**
 * @brief Global Firmware Configuration and Constants for NibruOS.
 */
namespace Config {

constexpr const char* kVersion = "v0.23.6";
constexpr const char* kSystemTitle = "NibruOS v0.23.6";

// Display geometry (landscape mode)
constexpr int ScreenWidth = 320;
constexpr int ScreenHeight = 170;
constexpr uint8_t Rotation = 1;

// Input & Timing governance (ms)
constexpr uint32_t TouchDebounceMs = 180;
constexpr uint32_t ButtonLongPressMs = 650;
constexpr uint8_t MaxScanResults = 12;
constexpr uint32_t WiFiWarmupMs = 650;
constexpr uint32_t ScanTimeoutMs = 11000;
constexpr uint32_t ScanMsPerChannel = 300;
constexpr uint8_t ScanMaxAttempts = 2;
constexpr uint32_t ConnectTimeoutMs = 20000;

} // namespace Config
