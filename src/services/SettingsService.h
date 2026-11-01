#pragma once
#include <stdint.h>
#include <stddef.h>

class DisplayManager;

class SettingsService {
public:
  void begin(DisplayManager* display = nullptr);
  void load();
  bool save();
  bool resetToDefaults();
  bool lastSaveOk() const { return lastSaveOk_; }

  // Screen Brightness: 25, 50, 75, 100 (%)
  uint8_t brightness() const { return brightness_; }
  bool setBrightness(uint8_t percent);

  // Screen Timeout: 30, 60, 180, 300, 0 (seconds)
  uint16_t screenTimeoutSec() const { return screenTimeoutSec_; }
  bool setScreenTimeoutSec(uint16_t sec);

  // Theme Accent: 0=Cyan, 1=Green, 2=Amber, 3=Magenta
  uint8_t theme() const { return theme_; }
  bool setTheme(uint8_t themeIndex);
  uint16_t themeColor() const;
  const char* themeName() const;

private:
  DisplayManager* display_ = nullptr;
  bool writable_ = true;
  bool lastSaveOk_ = true;
  uint8_t brightness_ = 100;
  uint16_t screenTimeoutSec_ = 30;
  uint8_t theme_ = 0;

  void applyToDisplay();
};

