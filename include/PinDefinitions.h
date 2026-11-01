#pragma once
#include <stdint.h>

/**
 * @brief Hardware Pin Definitions for LILYGO T-Display-S3 Touch Glass (ESP32-S3R8).
 *
 * Sourced from LilyGO official schematic and owner-verified hardware baseline.
 * DO NOT change without physical hardware validation.
 */
namespace Pins {

// Capacitive Touch Controller (CST816S on shared I2C bus)
constexpr int TouchSda = 18;
constexpr int TouchScl = 17;
constexpr int TouchReset = 21;
constexpr int TouchInterrupt = 16;

// Physical Buttons (active LOW with internal pull-up)
constexpr int ButtonBoot = 0;   // BOOT Button (GPIO0)
constexpr int ButtonUser = 14;  // USER Button (GPIO14)

// ST7789 8-bit Parallel Display Control & Power
constexpr int DisplayPower = 15;     // Active HIGH peripheral/display power
constexpr int DisplayBacklight = 38; // Backlight pulse (AW9364) or LEDC PWM
constexpr int DisplayCs = 6;
constexpr int DisplayDc = 7;
constexpr int DisplayRst = 5;
constexpr int DisplayWr = 8;
constexpr int DisplayRd = 9;

// ST7789 8-bit Parallel Data Bus (D0..D7)
constexpr int DisplayD0 = 39;
constexpr int DisplayD1 = 40;
constexpr int DisplayD2 = 41;
constexpr int DisplayD3 = 42;
constexpr int DisplayD4 = 45;
constexpr int DisplayD5 = 46;
constexpr int DisplayD6 = 47;
constexpr int DisplayD7 = 48;

// Proposed External 3.3V SPI MicroSD Card Interface
// NOTE: NOT CONNECTED (Confirmed by user 2026-11-01). Unallocated, no hardware attached.
constexpr int SdCs   = 10;
constexpr int SdMosi = 11;
constexpr int SdSck  = 12;
constexpr int SdMiso = 13;

} // namespace Pins
