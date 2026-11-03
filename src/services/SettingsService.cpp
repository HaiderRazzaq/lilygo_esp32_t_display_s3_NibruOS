#include "services/SettingsService.h"
#include "services/LocalizationService.h"
#include "hardware/DisplayManager.h"
#include "hardware/TouchManager.h"
#include <Arduino.h>
#include <Preferences.h>
#include <cstring>

namespace {
constexpr const char* kNvsNamespace = "nibru_settings";
constexpr const char* kLegacyNamespace = "nibru_cfg";
}

void SettingsService::begin(DisplayManager* display, TouchManager* touch) {
  display_ = display;
  touch_ = touch;
  load();
  Serial.printf("[Settings] Loaded: Bright=%u%% Timeout=%us Theme=%s Rot=%u Lang=%s\n",
                brightness_, screenTimeoutSec_, themeName(), rotation_, languageName());
}

void SettingsService::load() {
  brightness_ = 100; screenTimeoutSec_ = 30; theme_ = 0; rotation_ = 1; language_ = 0;
  writable_ = true; lastSaveOk_ = true;
  Preferences prefs;
  bool migrated = false;
  // Opening read-write distinguishes an absent namespace (created empty) from
  // an unavailable NVS backend. Never allow writes after an unread load.
  if (!prefs.begin(kNvsNamespace, false)) {
    writable_ = false; lastSaveOk_ = false;
    applyToDisplay();
    return;
  }
  {
    if (prefs.isKey("config")) {
      uint8_t data[5]{};
      const bool valid = prefs.getBytesLength("config") == sizeof(data) &&
                         prefs.getBytes("config", data, sizeof(data)) == sizeof(data);
      if (!valid || data[0] != 1) {
        prefs.end();
        // Never silently overwrite a corrupt/unknown schema. Explicit reset may.
        writable_ = false; lastSaveOk_ = false;
        applyToDisplay();
        return;
      }
      brightness_ = data[1];
      screenTimeoutSec_ = static_cast<uint16_t>(data[2] | (data[3] << 8));
      theme_ = data[4];
      if (prefs.isKey("rot")) {
        rotation_ = prefs.getUChar("rot", 1);
      }
      if (prefs.isKey("lang")) {
        language_ = prefs.getUChar("lang", 0);
      }
      prefs.end();
    } else if (prefs.isKey("bright") || prefs.isKey("timeout") || prefs.isKey("theme") || prefs.isKey("rot") || prefs.isKey("lang")) {
      brightness_ = prefs.getUChar("bright", 100);
      screenTimeoutSec_ = prefs.getUShort("timeout", 30);
      theme_ = prefs.getUChar("theme", 0);
      rotation_ = prefs.getUChar("rot", 1);
      language_ = prefs.getUChar("lang", 0);
      prefs.end();
      migrated = true;
    } else {
      prefs.end();
      if (prefs.begin(kLegacyNamespace, true)) {
        brightness_ = prefs.getUChar("bright", 100);
        screenTimeoutSec_ = prefs.getUShort("sleep_sec", 30);
        prefs.end();
        migrated = true;
      }
    }
  }
  if (brightness_ != 25 && brightness_ != 50 && brightness_ != 75 && brightness_ != 100)
    brightness_ = 100;
  if (screenTimeoutSec_ != 0 && screenTimeoutSec_ != 30 && screenTimeoutSec_ != 60 &&
      screenTimeoutSec_ != 180 && screenTimeoutSec_ != 300) screenTimeoutSec_ = 30;
  if (theme_ > 3) theme_ = 0;
  if (rotation_ != 1 && rotation_ != 3) rotation_ = 1;
  if (language_ > 1) language_ = 0;
  LocalizationService::instance().setLanguage(static_cast<Language>(language_));
  if (migrated) save();
  applyToDisplay();
}

bool SettingsService::save() {
  if (!writable_) return lastSaveOk_ = false;
  Preferences prefs;
  if (!prefs.begin(kNvsNamespace, false)) return lastSaveOk_ = false;
  // One NVS blob commit prevents mixed preferences after an interrupted write.
  const uint8_t data[] = {1, brightness_, static_cast<uint8_t>(screenTimeoutSec_),
                         static_cast<uint8_t>(screenTimeoutSec_ >> 8), theme_};
  uint8_t stored[sizeof(data)]{};
  const bool unchanged = prefs.getBytesLength("config") == sizeof(data) &&
      prefs.getBytes("config", stored, sizeof(stored)) == sizeof(stored) &&
      memcmp(data, stored, sizeof(data)) == 0;
  bool ok = unchanged || prefs.putBytes("config", data, sizeof(data)) == sizeof(data);
  if (prefs.isKey("rot")) {
    const uint8_t storedRot = prefs.getUChar("rot", 1);
    if (storedRot != rotation_) {
      if (prefs.putUChar("rot", rotation_) != sizeof(uint8_t)) ok = false;
    }
  } else if (rotation_ != 1) {
    if (prefs.putUChar("rot", rotation_) != sizeof(uint8_t)) ok = false;
  }
  if (prefs.isKey("lang")) {
    const uint8_t storedLang = prefs.getUChar("lang", 0);
    if (storedLang != language_) {
      if (prefs.putUChar("lang", language_) != sizeof(uint8_t)) ok = false;
    }
  } else if (language_ != 0) {
    if (prefs.putUChar("lang", language_) != sizeof(uint8_t)) ok = false;
  }
  prefs.end();
  lastSaveOk_ = ok;
  if (!lastSaveOk_) Serial.println("[Settings] NVS write failed; preferences unchanged");
  else LocalizationService::instance().setLanguage(static_cast<Language>(language_));
  return lastSaveOk_;
}

bool SettingsService::resetToDefaults() {
  const uint8_t oldB = brightness_, oldT = theme_, oldL = language_;
  const uint16_t oldS = screenTimeoutSec_;
  const bool oldWritable = writable_;
  brightness_ = 100; screenTimeoutSec_ = 30; theme_ = 0; rotation_ = 1; language_ = 0; writable_ = true;
  if (!save()) {
    brightness_ = oldB; screenTimeoutSec_ = oldS; theme_ = oldT; language_ = oldL; writable_ = oldWritable;
    return false;
  }
  applyToDisplay();
  return true;
}

bool SettingsService::setBrightness(uint8_t percent) {
  if (percent != 25 && percent != 50 && percent != 75 && percent != 100) return false;
  if (brightness_ == percent) return lastSaveOk_;
  const uint8_t old = brightness_;
  brightness_ = percent;
  if (!save()) { brightness_ = old; return false; }
  applyToDisplay();
  return true;
}

bool SettingsService::setScreenTimeoutSec(uint16_t sec) {
  if (sec != 0 && sec != 30 && sec != 60 && sec != 180 && sec != 300) return false;
  if (screenTimeoutSec_ == sec) return lastSaveOk_;
  const uint16_t old = screenTimeoutSec_;
  screenTimeoutSec_ = sec;
  if (!save()) { screenTimeoutSec_ = old; return false; }
  applyToDisplay();
  return true;
}

bool SettingsService::setTheme(uint8_t themeIndex) {
  if (themeIndex > 3) return false;
  if (theme_ == themeIndex) return lastSaveOk_;
  const uint8_t old = theme_;
  theme_ = themeIndex;
  if (!save()) { theme_ = old; return false; }
  applyToDisplay();
  return true;
}

uint16_t SettingsService::themeColor() const {
  switch (theme_) {
    case 1:  return 0x07E0; // Matrix Green
    case 2:  return 0xFD20; // Amber Gold
    case 3:  return 0xF81F; // Neon Magenta
    default: return 0x07FF; // Cyber Cyan
  }
}

const char* SettingsService::themeName() const {
  switch (theme_) {
    case 1:  return "Matrix Green";
    case 2:  return "Amber Gold";
    case 3:  return "Neon Magenta";
    default: return "Cyber Cyan";
  }
}

bool SettingsService::setRotation(uint8_t rot) {
  if (rot != 1 && rot != 3) return false;
  if (rotation_ == rot) return lastSaveOk_;
  const uint8_t old = rotation_;
  rotation_ = rot;
  if (!save()) { rotation_ = old; return false; }
  applyToDisplay();
  return true;
}

const char* SettingsService::rotationName() const {
  switch (rotation_) {
    case 3: return "Inverted (180)";
    default: return "Landscape (0)";
  }
}

bool SettingsService::setLanguage(uint8_t lang) {
  if (lang > 1) return false;
  if (language_ == lang) return lastSaveOk_;
  const uint8_t old = language_;
  language_ = lang;
  if (!save()) { language_ = old; return false; }
  return true;
}

const char* SettingsService::languageName() const {
  switch (language_) {
    case 1:  return "Arabic";
    default: return "English";
  }
}

void SettingsService::applyToDisplay() {
  if (display_) {
    display_->setBrightness(brightness_);
    display_->setSleepTimeoutSeconds(screenTimeoutSec_);
    display_->setThemeColor(themeColor());
    display_->setRotation(rotation_);
  }
  if (touch_) {
    touch_->setRotation(rotation_);
  }
}
