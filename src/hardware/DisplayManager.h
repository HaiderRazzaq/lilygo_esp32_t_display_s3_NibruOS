#pragma once
#include <TFT_eSPI.h>
#include <stdint.h>
#include "ui/Theme.h"

#ifndef NIBRU_BACKLIGHT_LEDC
#define NIBRU_BACKLIGHT_LEDC 0
#endif

class DisplayManager {
public:
  void begin();
  TFT_eSPI& gfx() { return tft_; }
  void clear(uint16_t color = Colors::Bg);
  void header(const char* title, bool showBack);
  void footer(const char* message);
  void label(int x, int y, const char* text, uint16_t color = Colors::White,
             uint8_t size = 1, uint16_t background = Colors::Bg);
  void labelPadded(int x, int y, const char* text, uint8_t minChars,
                   uint16_t color = Colors::White, uint8_t size = 1,
                   uint16_t background = Colors::Bg);

  // Brightness management
  uint8_t brightness() const { return brightnessPercent_; }
  void setBrightness(uint8_t percent);

  // Screen Orientation: 1 = Landscape Normal (0 deg), 3 = Landscape Inverted (180 deg)
  uint8_t rotation() const { return rotation_; }
  void setRotation(uint8_t r);

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
  uint8_t rotation_ = 1;
  uint8_t brightnessPercent_ = 100;
  uint16_t sleepTimeoutSec_ = 30;
  uint32_t lastActivityMs_ = 0;
  uint16_t themeAccent_ = Colors::Cyan;
#if NIBRU_BACKLIGHT_LEDC
  bool pwmReady_ = false;
#endif

  void applyBacklight(uint8_t percent);
};
