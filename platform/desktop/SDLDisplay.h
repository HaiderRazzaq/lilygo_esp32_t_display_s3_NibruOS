#pragma once
#include <stdint.h>
#include <cstddef>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

class SDLDisplay {
public:
  static constexpr int kLogicalWidth = 320;
  static constexpr int kLogicalHeight = 170;

  static SDLDisplay& instance();

  bool init(uint8_t scale = 3, bool headless = false);
  void destroy();

  // Resolution
  int width() const { return kLogicalWidth; }
  int height() const { return kLogicalHeight; }
  uint8_t scale() const { return scale_; }
  void setScale(uint8_t scale);
  void cycleScale();

  // Rendering Primitives
  void fillScreen(uint16_t color);
  void fillRect(int x, int y, int w, int h, uint16_t color);
  void drawFastHLine(int x, int y, int w, uint16_t color);
  void drawFastVLine(int x, int y, int h, uint16_t color);
  void drawPixel(int x, int y, uint16_t color);
  void drawRoundRect(int x, int y, int w, int h, int r, uint16_t color);
  void fillRoundRect(int x, int y, int w, int h, int r, uint16_t color);
  void drawCircle(int x0, int y0, int r, uint16_t color);
  void fillCircle(int x0, int y0, int r, uint16_t color);
  void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color);
  void fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color);

  // Text / Font Primitives (using 5x7 GLCD bitmap font)
  void drawChar(int x, int y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
  void setCursor(int x, int y) { cursorX_ = x; cursorY_ = y; }
  void setTextSize(uint8_t size) { textSize_ = size ? size : 1; }
  void setTextColor(uint16_t color, uint16_t bg) { textColor_ = color; textBg_ = bg; hasBg_ = true; }
  void setTextColor(uint16_t color) { textColor_ = color; hasBg_ = false; }
  void setTextWrap(bool wrap) { textWrap_ = wrap; }
  void print(const char* str);
  void print(int val);
  void print(char c);

  // Hardware emulation hooks
  void writeCommand(uint8_t cmd);
  void setBrightness(uint8_t percent);
  void setRotation(uint8_t r) { (void)r; } // Device is fixed to Rotation 1 (320x170)

  // Buffer presentation to SDL
  void present();
  bool isSleeping() const { return sleeping_; }
  bool isHeadless() const { return headless_; }

  const uint16_t* framebuffer() const { return &buffer_[0][0]; }

private:
  SDLDisplay() = default;
  ~SDLDisplay();
  SDLDisplay(const SDLDisplay&) = delete;
  SDLDisplay& operator=(const SDLDisplay&) = delete;

  uint16_t buffer_[kLogicalHeight][kLogicalWidth]{};
  uint8_t scale_ = 3;
  bool headless_ = false;
  bool initialized_ = false;
  bool sleeping_ = false;
  uint8_t brightness_ = 100;

  int cursorX_ = 0;
  int cursorY_ = 0;
  uint8_t textSize_ = 1;
  uint16_t textColor_ = 0xFFFF;
  uint16_t textBg_ = 0x0000;
  bool hasBg_ = false;
  bool textWrap_ = false;

  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;
  SDL_Texture* texture_ = nullptr;

  void drawCircleHelper(int x0, int y0, int r, uint8_t cornername, uint16_t color);
  void fillCircleHelper(int x0, int y0, int r, uint8_t cornername, int delta, uint16_t color);
  void updateWindowTitle();
};
