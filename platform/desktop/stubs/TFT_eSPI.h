#pragma once
#include "platform/desktop/SDLDisplay.h"
#include <stdint.h>

class TFT_eSPI {
public:
  void init() { SDLDisplay::instance().init(); }
  void writecommand(uint8_t cmd) { SDLDisplay::instance().writeCommand(cmd); }
  void setRotation(uint8_t r) { SDLDisplay::instance().setRotation(r); }
  void setTextWrap(bool w) { SDLDisplay::instance().setTextWrap(w); }
  int width() const { return SDLDisplay::instance().width(); }
  int height() const { return SDLDisplay::instance().height(); }

  void fillScreen(uint16_t color) { SDLDisplay::instance().fillScreen(color); }
  void fillRect(int x, int y, int w, int h, uint16_t color) {
    SDLDisplay::instance().fillRect(x, y, w, h, color);
  }
  void drawFastHLine(int x, int y, int w, uint16_t color) {
    SDLDisplay::instance().drawFastHLine(x, y, w, color);
  }
  void drawFastVLine(int x, int y, int h, uint16_t color) {
    SDLDisplay::instance().drawFastVLine(x, y, h, color);
  }

  void setTextColor(uint16_t fg, uint16_t bg) { SDLDisplay::instance().setTextColor(fg, bg); }
  void setTextColor(uint16_t fg) { SDLDisplay::instance().setTextColor(fg); }
  void setTextSize(uint8_t size) { SDLDisplay::instance().setTextSize(size); }
  void setCursor(int x, int y) { SDLDisplay::instance().setCursor(x, y); }
  void print(const char* text) { SDLDisplay::instance().print(text); }
  void print(int val) { SDLDisplay::instance().print(val); }
  void print(char c) { SDLDisplay::instance().print(c); }

  void drawRoundRect(int x, int y, int w, int h, int r, uint16_t color) {
    SDLDisplay::instance().drawRoundRect(x, y, w, h, r, color);
  }
  void fillRoundRect(int x, int y, int w, int h, int r, uint16_t color) {
    SDLDisplay::instance().fillRoundRect(x, y, w, h, r, color);
  }
  void drawCircle(int x, int y, int r, uint16_t color) {
    SDLDisplay::instance().drawCircle(x, y, r, color);
  }
  void fillCircle(int x, int y, int r, uint16_t color) {
    SDLDisplay::instance().fillCircle(x, y, r, color);
  }
  void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color) {
    SDLDisplay::instance().drawTriangle(x0, y0, x1, y1, x2, y2, color);
  }
  void fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color) {
    SDLDisplay::instance().fillTriangle(x0, y0, x1, y1, x2, y2, color);
  }
};
