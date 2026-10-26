#pragma once
#include <stdint.h>
#include <cstdarg>
struct TFT_eSPI {
  void init(){}
  void setRotation(uint8_t){}
  void setTextWrap(bool){}
  int width() const { return 320; }
  int height() const { return 170; }
  void fillScreen(uint16_t){}
  void fillRect(int,int,int,int,uint16_t){}
  void drawFastHLine(int,int,int,uint16_t){}
  void drawFastVLine(int,int,int,uint16_t){}
  void setTextColor(uint16_t,uint16_t){}
  void setTextSize(uint8_t){}
  void setCursor(int,int){}
  void print(const char*){}
  void print(int){}
  void drawRoundRect(int,int,int,int,int,uint16_t){}
  void fillRoundRect(int,int,int,int,int,uint16_t){}
  void drawCircle(int,int,int,uint16_t){}
};
