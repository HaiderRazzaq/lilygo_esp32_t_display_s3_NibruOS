#include "hardware/DisplayManager.h"
#include "BoardConfig.h"
#include <Arduino.h>
#ifndef NIBRU_BACKLIGHT_LEDC
#define NIBRU_BACKLIGHT_LEDC 0
#endif

void DisplayManager::begin() {
  pinMode(Board::kDisplayPower, OUTPUT);
  digitalWrite(Board::kDisplayPower, HIGH);
  pinMode(Board::kDisplayBacklight, OUTPUT);
  digitalWrite(Board::kDisplayBacklight, HIGH);

  // Initialize display controller first
  tft_.init();
  tft_.setRotation(rotation_);
  tft_.setTextWrap(false);
  clear();

  // Apply backlight AFTER tft_.init() so TFT_eSPI doesn't override pin state
#if NIBRU_BACKLIGHT_LEDC
  // Arduino-ESP32 2.0.14 API. Channel 0 reserved here; no other LEDC user.
  pwmReady_ = ledcSetup(0, 5000, 8) != 0;
  if (pwmReady_) ledcAttachPin(Board::kDisplayBacklight, 0);
  else Serial.println("[Display] LEDC unavailable; visible HIGH fallback");
#endif
  applyBacklight(brightnessPercent_);

  Serial.printf("[Display] size=%dx%d expected=320x170 bright=%u%% sleep=%us\n",
                tft_.width(), tft_.height(), brightnessPercent_, sleepTimeoutSec_);

  lastActivityMs_ = millis();
}

void DisplayManager::clear(uint16_t c) { tft_.fillScreen(c); }

void DisplayManager::label(int x, int y, const char* text, uint16_t color,
                           uint8_t size, uint16_t background) {
  tft_.setTextSize(size);
  tft_.setTextColor(color, background);
  tft_.setCursor(x, y);
  tft_.print(text);
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

void DisplayManager::sleep() {
  if (sleeping_) return;
  sleeping_ = true;
  applyBacklight(0);
  tft_.writecommand(0x28); // ST7789 DISPOFF
  Serial.println("[Display] Sleep mode entered (backlight OFF)");
}

void DisplayManager::wake() {
  if (!sleeping_) return;
  tft_.writecommand(0x29); // ST7789 DISPON
  applyBacklight(brightnessPercent_);
  sleeping_ = false;
  lastActivityMs_ = millis();
  Serial.println("[Display] Woke up (backlight restored)");
}

void DisplayManager::tick(uint32_t nowMs) {
  if (sleeping_) return;
  if (sleepTimeoutSec_ == 0) return; // 0 = Never sleep

  if (nowMs - lastActivityMs_ >= (static_cast<uint32_t>(sleepTimeoutSec_) * 1000UL)) {
    sleep();
  }
}
