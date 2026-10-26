#include "hardware/DisplayManager.h"
#include "BoardConfig.h"
#include <Arduino.h>
void DisplayManager::begin() {
  // Exact sequence from owner's working Arduino sketch.
  pinMode(Board::kDisplayPower, OUTPUT);
  digitalWrite(Board::kDisplayPower, HIGH);
  pinMode(Board::kDisplayBacklight, OUTPUT);
  digitalWrite(Board::kDisplayBacklight, HIGH);
  tft_.init();
  tft_.setRotation(Board::kRotation);
  tft_.setTextWrap(false);
  clear();
  Serial.printf("[Display] size=%dx%d expected=320x170\n", tft_.width(), tft_.height());
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
    label(14, 9, "<", Colors::Cyan, 2, Colors::Panel);
    label(43, 11, title, Colors::White, 2, Colors::Panel);
  } else {
    label(9, 11, title, Colors::Cyan, 2, Colors::Panel);
  }
}
void DisplayManager::footer(const char* message) {
  tft_.drawFastHLine(0, 151, 320, Colors::Border);
  tft_.fillRect(0, 152, 320, 18, Colors::Bg);
  label(7, 156, message, Colors::Muted, 1);
}
