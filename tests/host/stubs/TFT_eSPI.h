#pragma once
#include <stdint.h>
#include <cstdarg>
#include <vector>
#include <string>
struct TFT_eSPI {
  std::vector<std::string> printed;
  unsigned fullClears = 0, drawCalls = 0;
  void init(){}
  void writecommand(uint8_t){}
  void setRotation(uint8_t){}
  void setTextWrap(bool){}
  int width() const { return 320; }
  int height() const { return 170; }
  void fillScreen(uint16_t){ ++fullClears; ++drawCalls; }
  void fillRect(int,int,int,int,uint16_t){ ++drawCalls; }
  void drawFastHLine(int,int,int,uint16_t){}
  void drawFastVLine(int,int,int,uint16_t){}
  void setTextColor(uint16_t,uint16_t){}
  void setTextSize(uint8_t){}
  void setCursor(int,int){}
  void print(const char* text){ ++drawCalls; printed.emplace_back(text); }
  void print(int){}
  void drawRoundRect(int,int,int,int,int,uint16_t){ ++drawCalls; }
  void fillRoundRect(int,int,int,int,int,uint16_t){ ++drawCalls; }
  void drawCircle(int,int,int,uint16_t){}
  void fillCircle(int,int,int,uint16_t){}
  void fillTriangle(int,int,int,int,int,int,uint16_t){}
  void drawTriangle(int,int,int,int,int,int,uint16_t){}
};
