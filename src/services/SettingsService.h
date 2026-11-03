#pragma once
#include <stdint.h>
#include <stddef.h>

class DisplayManager;
class TouchManager;

class SettingsService {
public:
  void begin(DisplayManager* display = nullptr, TouchManager* touch = nullptr);
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

  // Screen Orientation: 1 = Landscape Normal (0 deg), 3 = Landscape Inverted (180 deg)
  uint8_t rotation() const { return rotation_; }
  bool setRotation(uint8_t rot);
  const char* rotationName() const;

  // System Language: 0 = English, 1 = Arabic
  uint8_t language() const { return language_; }
  bool setLanguage(uint8_t lang);
  const char* languageName() const;

  // Timezone Offset: -12 to +14 hours (default 3 = UTC+3)
  int8_t timezoneOffset() const { return timezoneOffset_; }
  bool setTimezoneOffset(int8_t offsetHours);

private:
  DisplayManager* display_ = nullptr;
  TouchManager* touch_ = nullptr;
  bool writable_ = true;
  bool lastSaveOk_ = true;
  uint8_t brightness_ = 100;
  uint16_t screenTimeoutSec_ = 30;
  uint8_t theme_ = 0;
  uint8_t rotation_ = 1;
  uint8_t language_ = 0;
  int8_t timezoneOffset_ = 3;

  void applyToDisplay();
};
