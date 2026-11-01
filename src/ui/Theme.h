#pragma once
#include <stdint.h>

/**
 * @brief System UI Theme, Color Palette, and Layout Metrics for NibruOS.
 *
 * RGB565 color format matching the ST7789 display controller.
 */
namespace Colors {
  constexpr uint16_t Bg = 0x0841;
  constexpr uint16_t Panel = 0x10A3;
  constexpr uint16_t Border = 0x2945;
  constexpr uint16_t Cyan = 0x05FF;
  constexpr uint16_t White = 0xFFFF;
  constexpr uint16_t Muted = 0xB596;
  constexpr uint16_t Green = 0x05E7;
  constexpr uint16_t Amber = 0xFEA0;
  constexpr uint16_t Red = 0xF945;
  constexpr uint16_t Magenta = 0xF9FF;
}

namespace Theme {
  // Theme accents matching SettingsService indices
  constexpr uint16_t Accents[] = {
    Colors::Cyan,    // 0: Cyber Cyan
    Colors::Green,   // 1: Emerald Green
    Colors::Amber,   // 2: Amber Gold
    Colors::Magenta  // 3: Neon Magenta
  };
  constexpr uint8_t AccentCount = 4;

  inline uint16_t getAccentColor(uint8_t index) {
    return (index < AccentCount) ? Accents[index] : Colors::Cyan;
  }

  // Standard UI metrics
  constexpr int16_t HeaderHeight = 33;
  constexpr int16_t FooterHeight = 18;
  constexpr int16_t ScreenWidth = 320;
  constexpr int16_t ScreenHeight = 170;
}
