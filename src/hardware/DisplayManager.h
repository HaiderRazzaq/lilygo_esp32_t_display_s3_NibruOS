#pragma once
#include <TFT_eSPI.h>
#include <stdint.h>
#ifndef NIBRU_BACKLIGHT_LEDC
#define NIBRU_BACKLIGHT_LEDC 0
#endif

class DisplayManager {
public:
  void begin();
  TFT_eSPI& gfx() { return tft_; }
  void clear(uint16_t color = 0x0841);
  void header(const char* title, bool showBack);
  void footer(const char* message);
  void label(int x, int y, const char* text, uint16_t color = 0xFFFF,
             uint8_t size = 1, uint16_t background = 0x0841);

  // Brightness management
  uint8_t brightness() const { return brightnessPercent_; }
  void setBrightness(uint8_t percent);

  // Theme Accent
  uint16_t themeColor() const { return themeAccent_; }
  void setThemeColor(uint16_t color) { themeAccent_ = color; }

  // Sleep & Power management
  void sleep();
  void wake();
  bool isSleeping() const { return sleeping_; }
  void setSleepTimeoutSeconds(uint16_t seconds);
  uint16_t sleepTimeoutSeconds() const { return sleepTimeoutSec_; }
  void notifyActivity(uint32_t nowMs);
  void tick(uint32_t nowMs);

private:
  TFT_eSPI tft_;
  bool sleeping_ = false;
  uint8_t brightnessPercent_ = 100;
  uint16_t sleepTimeoutSec_ = 30;
  uint32_t lastActivityMs_ = 0;
  uint16_t themeAccent_ = 0x05FF; // Default Cyan
#if NIBRU_BACKLIGHT_LEDC
  bool pwmReady_ = false;
#endif

  void applyBacklight(uint8_t percent);
};

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
