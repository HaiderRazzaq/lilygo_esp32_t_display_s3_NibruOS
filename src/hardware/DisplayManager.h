#pragma once
#include <TFT_eSPI.h>
#include <stdint.h>

class DisplayManager {
public:
  void begin();
  TFT_eSPI& gfx() { return tft_; }
  void clear(uint16_t color = 0x0841);
  void header(const char* title, bool showBack);
  void footer(const char* message);
  void label(int x, int y, const char* text, uint16_t color = 0xFFFF,
             uint8_t size = 1, uint16_t background = 0x0841);
private:
  TFT_eSPI tft_;
};
namespace Colors {
  constexpr uint16_t Bg = 0x0841;
  constexpr uint16_t Panel = 0x10A3;
  constexpr uint16_t Border = 0x2945;
  constexpr uint16_t Cyan = 0x05FF;
  constexpr uint16_t White = 0xFFFF;
  constexpr uint16_t Muted = 0xB596;
  constexpr uint16_t Green = 0x05E7;
  constexpr uint16_t Amber = 0xFEA0;
  constexpr uint16_t Red = 0xF945;
}
