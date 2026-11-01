#include "services/SettingsService.h"
#include "hardware/DisplayManager.h"
#include <Arduino.h>
#include <Preferences.h>

namespace {
constexpr const char* kNvsNamespace = "nibru_settings";
constexpr const char* kLegacyNamespace = "nibru_cfg";
}

void SettingsService::begin(DisplayManager* display) {
  display_ = display;
  load();
  applyToDisplay();
  Serial.printf("[Settings] Loaded: Bright=%u%% Timeout=%us Theme=%s\n",
                brightness_, screenTimeoutSec_, themeName());
}

void SettingsService::load() {
  Preferences prefs;
  if (prefs.begin(kNvsNamespace, true)) {
    brightness_ = prefs.getUChar("bright", 100);
    screenTimeoutSec_ = prefs.getUShort("timeout", 30);
    theme_ = prefs.getUChar("theme", 0);
    prefs.end();
  } else if (prefs.begin(kLegacyNamespace, true)) {
    // Migration from older v0.2/v0.3 config
    brightness_ = prefs.getUChar("bright", 100);
    screenTimeoutSec_ = prefs.getUShort("sleep_sec", 30);
    theme_ = 0;
    prefs.end();
  } else {
    brightness_ = 100;
    screenTimeoutSec_ = 30;
    theme_ = 0;
  }

  // Validate inputs
  if (brightness_ != 25 && brightness_ != 50 && brightness_ != 75 && brightness_ != 100) {
    brightness_ = 100;
  }
  if (screenTimeoutSec_ != 30 && screenTimeoutSec_ != 60 &&
      screenTimeoutSec_ != 180 && screenTimeoutSec_ != 300 && screenTimeoutSec_ != 0) {
    screenTimeoutSec_ = 30;
  }
  if (theme_ > 3) {
    theme_ = 0;
  }
}

void SettingsService::save() {
  Preferences prefs;
  if (prefs.begin(kNvsNamespace, false)) {
    prefs.putUChar("bright", brightness_);
    prefs.putUShort("timeout", screenTimeoutSec_);
    prefs.putUChar("theme", theme_);
    prefs.end();
    Serial.println("[Settings] Saved to NVS");
  }
}

void SettingsService::resetToDefaults() {
  brightness_ = 100;
  screenTimeoutSec_ = 30;
  theme_ = 0;
  save();
  applyToDisplay();
  Serial.println("[Settings] Reset to defaults");
}

bool SettingsService::setBrightness(uint8_t percent) {
  if (percent != 25 && percent != 50 && percent != 75 && percent != 100) {
    return false;
  }
  if (brightness_ == percent) return true;
  brightness_ = percent;
  save();
  applyToDisplay();
  return true;
}

bool SettingsService::setScreenTimeoutSec(uint16_t sec) {
  if (sec != 30 && sec != 60 && sec != 180 && sec != 300 && sec != 0) {
    return false;
  }
  if (screenTimeoutSec_ == sec) return true;
  screenTimeoutSec_ = sec;
  save();
  applyToDisplay();
  return true;
}

bool SettingsService::setTheme(uint8_t themeIndex) {
  if (themeIndex > 3) return false;
  if (theme_ == themeIndex) return true;
  theme_ = themeIndex;
  save();
  applyToDisplay();
  return true;
}

uint16_t SettingsService::themeColor() const {
  switch (theme_) {
    case 1: return Colors::Green;
    case 2: return Colors::Amber;
    case 3: return Colors::Magenta;
    default: return Colors::Cyan;
  }
}

const char* SettingsService::themeName() const {
  switch (theme_) {
    case 1: return "Emerald Green";
    case 2: return "Amber Gold";
    case 3: return "Neon Magenta";
    default: return "Cyber Cyan";
  }
}

void SettingsService::applyToDisplay() {
  if (!display_) return;
  display_->setBrightness(brightness_);
  display_->setSleepTimeoutSeconds(screenTimeoutSec_);
  display_->setThemeColor(themeColor());
}

