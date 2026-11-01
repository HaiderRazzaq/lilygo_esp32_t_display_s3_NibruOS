#define SDL_MAIN_HANDLED
#include "platform/desktop/SDLDisplay.h"
#include "platform/desktop/glcdfont.h"
#include <SDL.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>

SDLDisplay& SDLDisplay::instance() {
  static SDLDisplay s_instance;
  return s_instance;
}

SDLDisplay::~SDLDisplay() {
  destroy();
}

bool SDLDisplay::init(uint8_t scale, bool headless) {
  if (initialized_) return true;
  scale_ = (scale >= 1 && scale <= 4) ? scale : 3;
  headless_ = headless;
  std::memset(buffer_, 0, sizeof(buffer_));

  if (headless_) {
    initialized_ = true;
    return true;
  }

  SDL_SetMainReady();
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::printf("[SDLDisplay] SDL_Init failed: %s\n", SDL_GetError());
    return false;
  }

  const int winWidth = kLogicalWidth * scale_;
  const int winHeight = kLogicalHeight * scale_;

  window_ = SDL_CreateWindow(
      "NibruOS Desktop Simulator (320x170 @ 3x)",
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      winWidth, winHeight,
      SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI
  );

  if (!window_) {
    std::printf("[SDLDisplay] Window creation failed: %s\n", SDL_GetError());
    SDL_Quit();
    return false;
  }

  renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!renderer_) {
    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_SOFTWARE);
  }
  if (!renderer_) {
    std::printf("[SDLDisplay] Renderer creation failed: %s\n", SDL_GetError());
    SDL_DestroyWindow(window_);
    window_ = nullptr;
    SDL_Quit();
    return false;
  }

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"); // Nearest neighbor
  SDL_RenderSetLogicalSize(renderer_, kLogicalWidth, kLogicalHeight);

  texture_ = SDL_CreateTexture(
      renderer_,
      SDL_PIXELFORMAT_RGB565,
      SDL_TEXTUREACCESS_STREAMING,
      kLogicalWidth, kLogicalHeight
  );

  if (!texture_) {
    std::printf("[SDLDisplay] Texture creation failed: %s\n", SDL_GetError());
    SDL_DestroyRenderer(renderer_);
    renderer_ = nullptr;
    SDL_DestroyWindow(window_);
    window_ = nullptr;
    SDL_Quit();
    return false;
  }

  updateWindowTitle();
  initialized_ = true;
  std::printf("[SDLDisplay] Initialized: 320x170, scale=%ux (%dx%d window)\n",
              scale_, winWidth, winHeight);
  return true;
}

void SDLDisplay::destroy() {
  if (texture_) {
    SDL_DestroyTexture(texture_);
    texture_ = nullptr;
  }
  if (renderer_) {
    SDL_DestroyRenderer(renderer_);
    renderer_ = nullptr;
  }
  if (window_) {
    SDL_DestroyWindow(window_);
    window_ = nullptr;
  }
  if (initialized_ && !headless_) {
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
  }
  initialized_ = false;
}

void SDLDisplay::setScale(uint8_t scale) {
  if (scale < 1) scale = 1;
  if (scale > 4) scale = 4;
  if (scale_ == scale) return;
  scale_ = scale;
  if (window_ && !headless_) {
    SDL_SetWindowSize(window_, kLogicalWidth * scale_, kLogicalHeight * scale_);
    SDL_SetWindowPosition(window_, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    updateWindowTitle();
  }
}

void SDLDisplay::cycleScale() {
  uint8_t next = scale_ + 1;
  if (next > 4) next = 1;
  setScale(next);
}

void SDLDisplay::updateWindowTitle() {
  if (!window_) return;
  char title[96];
  std::snprintf(title, sizeof(title), "NibruOS Desktop Simulator (320x170 @ %ux) | %s",
                scale_, sleeping_ ? "SLEEP" : "AWAKE");
  SDL_SetWindowTitle(window_, title);
}

void SDLDisplay::writeCommand(uint8_t cmd) {
  if (cmd == 0x28) {
    sleeping_ = true;
    updateWindowTitle();
  } else if (cmd == 0x29) {
    sleeping_ = false;
    updateWindowTitle();
  }
}

void SDLDisplay::setBrightness(uint8_t percent) {
  if (percent > 100) percent = 100;
  brightness_ = percent;
}

void SDLDisplay::present() {
  if (headless_ || !renderer_ || !texture_) return;

  if (sleeping_) {
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);
    SDL_RenderPresent(renderer_);
    return;
  }

  const uint8_t mod = static_cast<uint8_t>((static_cast<uint32_t>(brightness_) * 255) / 100);
  SDL_SetTextureColorMod(texture_, mod, mod, mod);
  SDL_UpdateTexture(texture_, nullptr, buffer_, kLogicalWidth * sizeof(uint16_t));
  SDL_RenderClear(renderer_);
  SDL_RenderCopy(renderer_, texture_, nullptr, nullptr);
  SDL_RenderPresent(renderer_);
}

void SDLDisplay::drawPixel(int x, int y, uint16_t color) {
  if (x >= 0 && x < kLogicalWidth && y >= 0 && y < kLogicalHeight) {
    buffer_[y][x] = color;
  }
}

void SDLDisplay::fillScreen(uint16_t color) {
  for (int y = 0; y < kLogicalHeight; ++y) {
    for (int x = 0; x < kLogicalWidth; ++x) {
      buffer_[y][x] = color;
    }
  }
}

void SDLDisplay::fillRect(int x, int y, int w, int h, uint16_t color) {
  if (w <= 0 || h <= 0) return;
  const int x1 = std::max(0, x);
  const int y1 = std::max(0, y);
  const int x2 = std::min(kLogicalWidth, x + w);
  const int y2 = std::min(kLogicalHeight, y + h);

  for (int py = y1; py < y2; ++py) {
    for (int px = x1; px < x2; ++px) {
      buffer_[py][px] = color;
    }
  }
}

void SDLDisplay::drawFastHLine(int x, int y, int w, uint16_t color) {
  if (y < 0 || y >= kLogicalHeight || w <= 0) return;
  const int x1 = std::max(0, x);
  const int x2 = std::min(kLogicalWidth, x + w);
  for (int px = x1; px < x2; ++px) {
    buffer_[y][px] = color;
  }
}

void SDLDisplay::drawFastVLine(int x, int y, int h, uint16_t color) {
  if (x < 0 || x >= kLogicalWidth || h <= 0) return;
  const int y1 = std::max(0, y);
  const int y2 = std::min(kLogicalHeight, y + h);
  for (int py = y1; py < y2; ++py) {
    buffer_[py][x] = color;
  }
}

void SDLDisplay::drawCircle(int x0, int y0, int r, uint16_t color) {
  int f = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x = 0;
  int y = r;

  drawPixel(x0, y0 + r, color);
  drawPixel(x0, y0 - r, color);
  drawPixel(x0 + r, y0, color);
  drawPixel(x0 - r, y0, color);

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    drawPixel(x0 + x, y0 + y, color);
    drawPixel(x0 - x, y0 + y, color);
    drawPixel(x0 + x, y0 - y, color);
    drawPixel(x0 - x, y0 - y, color);
    drawPixel(x0 + y, y0 + x, color);
    drawPixel(x0 - y, y0 + x, color);
    drawPixel(x0 + y, y0 - x, color);
    drawPixel(x0 - y, y0 - x, color);
  }
}

void SDLDisplay::drawCircleHelper(int x0, int y0, int r, uint8_t cornername, uint16_t color) {
  int f = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x = 0;
  int y = r;

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    if (cornername & 0x4) {
      drawPixel(x0 + x, y0 + y, color);
      drawPixel(x0 + y, y0 + x, color);
    }
    if (cornername & 0x2) {
      drawPixel(x0 + x, y0 - y, color);
      drawPixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      drawPixel(x0 - y, y0 + x, color);
      drawPixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      drawPixel(x0 - y, y0 - x, color);
      drawPixel(x0 - x, y0 - y, color);
    }
  }
}

void SDLDisplay::fillCircle(int x0, int y0, int r, uint16_t color) {
  drawFastVLine(x0, y0 - r, 2 * r + 1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
}

void SDLDisplay::fillCircleHelper(int x0, int y0, int r, uint8_t cornername, int delta, uint16_t color) {
  int f = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x = 0;
  int y = r;

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    if (cornername & 0x1) {
      drawFastVLine(x0 + x, y0 - y, 2 * y + 1 + delta, color);
      drawFastVLine(x0 + y, y0 - x, 2 * x + 1 + delta, color);
    }
    if (cornername & 0x2) {
      drawFastVLine(x0 - x, y0 - y, 2 * y + 1 + delta, color);
      drawFastVLine(x0 - y, y0 - x, 2 * x + 1 + delta, color);
    }
  }
}

void SDLDisplay::drawRoundRect(int x, int y, int w, int h, int r, uint16_t color) {
  if (w <= 0 || h <= 0) return;
  if (r <= 0) {
    drawFastHLine(x, y, w, color);
    drawFastHLine(x, y + h - 1, w, color);
    drawFastVLine(x, y, h, color);
    drawFastVLine(x + w - 1, y, h, color);
    return;
  }
  drawFastHLine(x + r, y, w - 2 * r, color);
  drawFastHLine(x + r, y + h - 1, w - 2 * r, color);
  drawFastVLine(x, y + r, h - 2 * r, color);
  drawFastVLine(x + w - 1, y + r, h - 2 * r, color);

  drawCircleHelper(x + r, y + r, r, 1, color);
  drawCircleHelper(x + w - r - 1, y + r, r, 2, color);
  drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
  drawCircleHelper(x + r, y + h - r - 1, r, 8, color);
}

void SDLDisplay::fillRoundRect(int x, int y, int w, int h, int r, uint16_t color) {
  if (w <= 0 || h <= 0) return;
  if (r <= 0) {
    fillRect(x, y, w, h, color);
    return;
  }
  fillRect(x + r, y, w - 2 * r, h, color);
  fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
  fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1, color);
}

void SDLDisplay::drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color) {
  auto drawLine = [this](int lx0, int ly0, int lx1, int ly1, uint16_t c) {
    int dx = std::abs(lx1 - lx0), sx = lx0 < lx1 ? 1 : -1;
    int dy = -std::abs(ly1 - ly0), sy = ly0 < ly1 ? 1 : -1;
    int err = dx + dy, e2;
    while (true) {
      drawPixel(lx0, ly0, c);
      if (lx0 == lx1 && ly0 == ly1) break;
      e2 = 2 * err;
      if (e2 >= dy) { err += dy; lx0 += sx; }
      if (e2 <= dx) { err += dx; ly0 += sy; }
    }
  };
  drawLine(x0, y0, x1, y1, color);
  drawLine(x1, y1, x2, y2, color);
  drawLine(x2, y2, x0, y0, color);
}

void SDLDisplay::fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color) {
  int a, b, y, last;
  if (y0 > y1) { std::swap(y0, y1); std::swap(x0, x1); }
  if (y1 > y2) { std::swap(y2, y1); std::swap(x2, x1); }
  if (y0 > y1) { std::swap(y0, y1); std::swap(x0, x1); }

  if (y0 == y2) {
    a = b = x0;
    if (x1 < a) a = x1;
    else if (x1 > b) b = x1;
    if (x2 < a) a = x2;
    else if (x2 > b) b = x2;
    drawFastHLine(a, y0, b - a + 1, color);
    return;
  }

  int dx01 = x1 - x0, dy01 = y1 - y0;
  int dx02 = x2 - x0, dy02 = y2 - y0;
  int dx12 = x2 - x1, dy12 = y2 - y1;
  int sa = 0, sb = 0;

  if (y1 == y2) last = y1;
  else last = y1 - 1;

  for (y = y0; y <= last; y++) {
    a = x0 + sa / dy01;
    b = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    if (a > b) std::swap(a, b);
    drawFastHLine(a, y, b - a + 1, color);
  }

  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for (; y <= y2; y++) {
    a = x1 + sa / dy12;
    b = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    if (a > b) std::swap(a, b);
    drawFastHLine(a, y, b - a + 1, color);
  }
}

void SDLDisplay::drawChar(int x, int y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) {
  if (c >= 128) return;
  for (int8_t i = 0; i < 5; i++) {
    uint8_t line = glcd_font[c * 5 + i];
    for (int8_t j = 0; j < 8; j++, line >>= 1) {
      if (line & 1) {
        if (size == 1) drawPixel(x + i, y + j, color);
        else fillRect(x + i * size, y + j * size, size, size, color);
      } else if (hasBg_) {
        if (size == 1) drawPixel(x + i, y + j, bg);
        else fillRect(x + i * size, y + j * size, size, size, bg);
      }
    }
  }
  if (hasBg_) {
    // 6th column spacing
    if (size == 1) drawFastVLine(x + 5, y, 8, bg);
    else fillRect(x + 5 * size, y, size, 8 * size, bg);
  }
}

void SDLDisplay::print(char c) {
  if (c == '\n') {
    cursorX_ = 0;
    cursorY_ += 8 * textSize_;
  } else if (c == '\r') {
    // ignore
  } else {
    if (textWrap_ && ((cursorX_ + 6 * textSize_) > kLogicalWidth)) {
      cursorX_ = 0;
      cursorY_ += 8 * textSize_;
    }
    drawChar(cursorX_, cursorY_, static_cast<unsigned char>(c), textColor_, textBg_, textSize_);
    cursorX_ += 6 * textSize_;
  }
}

void SDLDisplay::print(const char* str) {
  if (!str) return;
  while (*str) {
    print(*str++);
  }
}

void SDLDisplay::print(int val) {
  char buf[32];
  std::snprintf(buf, sizeof(buf), "%d", val);
  print(buf);
}
