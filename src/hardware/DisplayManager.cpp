#include "hardware/DisplayManager.h"
#include "services/ArabicReshaper.h"
#include "ui/ArabicFont.h"
#include "BoardConfig.h"
#include <Arduino.h>

void DisplayManager::begin() {
  pinMode(Board::kDisplayPower, OUTPUT);
  pinMode(Board::kDisplayBacklight, OUTPUT);
  digitalWrite(Board::kDisplayPower, HIGH);
  digitalWrite(Board::kDisplayBacklight, HIGH);

  tft_.init();
  tft_.setRotation(rotation_);
  tft_.fillScreen(Colors::Bg);

#if NIBRU_BACKLIGHT_LEDC
  ledcSetup(0, 5000, 8);
  ledcAttachPin(Board::kDisplayBacklight, 0);
  pwmReady_ = true;
#endif

  applyBacklight(brightnessPercent_);
  lastActivityMs_ = millis();
}

void DisplayManager::clear(uint16_t color) {
  tft_.fillScreen(color);
}

void DisplayManager::drawArabic(int x, int y, const char* text, uint16_t color,
                                uint8_t size, uint16_t background) {
  uint16_t glyphs[64];
  const size_t count = ArabicReshaper::reshape(text, glyphs, 64);
  int curX = x;

  for (size_t i = 0; i < count; ++i) {
    uint16_t code = glyphs[i];
    if (code == ' ') {
      if (background != 0 && background != Colors::Bg) {
        tft_.fillRect(curX, y, 5 * size, 14 * size, background);
      }
      curX += 5 * size;
      continue;
    }
    const ArabicGlyph* g = ArabicFont::getGlyph(code);
    if (g) {
      const uint8_t gw = g->width;
      for (uint8_t row = 0; row < 14; ++row) {
        uint16_t rowBits = g->bitmap[row];
        for (uint8_t col = 0; col < gw; ++col) {
          const bool on = (rowBits & (1 << (15 - col))) != 0;
          const int px = curX + col * size;
          const int py = y + row * size;
          if (on) {
            if (size == 1) tft_.drawPixel(px, py, color);
            else tft_.fillRect(px, py, size, size, color);
          } else if (background != 0) {
            if (size == 1) tft_.drawPixel(px, py, background);
            else tft_.fillRect(px, py, size, size, background);
          }
        }
      }
      curX += (gw + 1) * size;
    } else if (code < 128) {
      tft_.setTextSize(size);
      tft_.setTextColor(color, background);
      tft_.setCursor(curX, y + (size == 1 ? 3 : 0));
      tft_.print(static_cast<char>(code));
      curX += (size == 1 ? 6 : 12);
    }
  }
}

void DisplayManager::label(int x, int y, const char* text, uint16_t color,
                           uint8_t size, uint16_t background) {
  if (ArabicReshaper::hasArabic(text)) {
    drawArabic(x, y, text, color, size, background);
    return;
  }
  tft_.setTextSize(size);
  tft_.setTextColor(color, background);
  tft_.setCursor(x, y);
  tft_.print(text);
}

void DisplayManager::labelPadded(int x, int y, const char* text, uint8_t minChars,
                                 uint16_t color, uint8_t size, uint16_t background) {
  if (ArabicReshaper::hasArabic(text)) {
    drawArabic(x, y, text, color, size, background);
    return;
  }
  char buf[64];
  const size_t len = strlen(text);
  if (minChars >= sizeof(buf)) minChars = sizeof(buf) - 1;
  memcpy(buf, text, len);
  if (minChars > len) {
    memset(buf + len, ' ', minChars - len);
    buf[minChars] = 0;
  } else {
    buf[len] = 0;
  }
  label(x, y, buf, color, size, background);
}

void DisplayManager::header(const char* title, bool showBack) {
  tft_.fillRect(0, 0, 320, 33, Colors::Panel);
  tft_.drawFastHLine(0, 32, 320, Colors::Border);
  if (showBack) {
    tft_.drawRoundRect(5, 5, 31, 23, 5, Colors::Border);
    label(14, 9, "<", themeAccent_, 2, Colors::Panel);
    label(43, 11, title, Colors::White, 2, Colors::Panel);
  } else {
    label(9, 11, title, themeAccent_, 2, Colors::Panel);
  }
}

void DisplayManager::footer(const char* message) {
  tft_.drawFastHLine(0, 151, 320, Colors::Border);
  tft_.fillRect(0, 152, 320, 18, Colors::Bg);
  label(7, 156, message, Colors::Muted, 1);
}

void DisplayManager::applyBacklight(uint8_t percent) {
#if NIBRU_BACKLIGHT_LEDC
  if (pwmReady_) ledcWrite(0, (static_cast<uint32_t>(percent) * 255 + 50) / 100);
  else digitalWrite(Board::kDisplayBacklight, percent ? HIGH : LOW);
#else
  pinMode(Board::kDisplayBacklight, OUTPUT);

  if (percent == 0) {
    digitalWrite(Board::kDisplayBacklight, LOW);
    return;
  }

  // 1. Reset AW9364 to Step 0 by holding LOW for 3ms (> 2.5ms T_SHDN)
  digitalWrite(Board::kDisplayBacklight, LOW);
  delay(3);

  // 2. Rising edge brings AW9364 to Step 1 (100% full brightness)
  digitalWrite(Board::kDisplayBacklight, HIGH);
  delayMicroseconds(60); // Latch Step 1

  // 3. AW9364 16-step dimming protocol:
  // Each low pulse steps the current sink down by 1 step (~6.25%).
  // Step 1: 0 pulses (100%)
  // Step 5: 4 pulses (75%)
  // Step 9: 8 pulses (50%)
  // Step 13: 12 pulses (25%)
  uint8_t pulses = 0;
  if (percent <= 25) {
    pulses = 12;
  } else if (percent <= 50) {
    pulses = 8;
  } else if (percent <= 75) {
    pulses = 4;
  } else {
    pulses = 0; // 100%
  }

  for (uint8_t i = 0; i < pulses; ++i) {
    digitalWrite(Board::kDisplayBacklight, LOW);
    delayMicroseconds(20);
    digitalWrite(Board::kDisplayBacklight, HIGH);
    delayMicroseconds(20);
  }
#endif
}

void DisplayManager::setBrightness(uint8_t percent) {
  if (percent < 10) percent = 10;
  if (percent > 100) percent = 100;
  brightnessPercent_ = percent;

  if (!sleeping_) {
    applyBacklight(brightnessPercent_);
  }
  Serial.printf("[Display] Brightness set to %u%%\n", brightnessPercent_);
}

void DisplayManager::setRotation(uint8_t r) {
  if (r != 1 && r != 3) r = 1;
  rotation_ = r;
  tft_.setRotation(rotation_);
  Serial.printf("[Display] Rotation set to %u\n", rotation_);
}

void DisplayManager::setSleepTimeoutSeconds(uint16_t seconds) {
  sleepTimeoutSec_ = seconds;
  lastActivityMs_ = millis();
  Serial.printf("[Display] Sleep timeout set to %u s\n", sleepTimeoutSec_);
}

void DisplayManager::notifyActivity(uint32_t nowMs) {
  lastActivityMs_ = nowMs;
}

void DisplayManager::startScreensaver() {
  if (screensaverActive_ || sleeping_) return;
  screensaverActive_ = true;
  screensaverStartedMs_ = millis();
  Serial.println("[Display] Clock Screensaver activated");
}

void DisplayManager::stopScreensaver() {
  if (!screensaverActive_) return;
  screensaverActive_ = false;
  lastActivityMs_ = millis();
  Serial.println("[Display] Clock Screensaver deactivated");
}

void DisplayManager::sleep() {
  if (screensaverActive_) stopScreensaver();
  if (sleeping_) return;
  sleeping_ = true;
  applyBacklight(0);
  Serial.println("[Display] Sleep mode entered (backlight OFF)");
}

void DisplayManager::wake() {
  if (screensaverActive_) stopScreensaver();
  if (!sleeping_) return;
  applyBacklight(brightnessPercent_);
  sleeping_ = false;
  lastActivityMs_ = millis();
  Serial.println("[Display] Woke up (backlight restored)");
}

void DisplayManager::tick(uint32_t nowMs) {
  if (sleeping_) return;
  if (sleepTimeoutSec_ == 0) return; // 0 = Never sleep

  if (!screensaverActive_) {
    if (nowMs - lastActivityMs_ >= (static_cast<uint32_t>(sleepTimeoutSec_) * 1000UL)) {
      if (screensaverEnabled_) {
        startScreensaver();
      } else {
        sleep();
      }
    }
  }
}
