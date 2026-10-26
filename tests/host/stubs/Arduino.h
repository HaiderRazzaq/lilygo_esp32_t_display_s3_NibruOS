#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
using byte = uint8_t;
static constexpr int HIGH=1, LOW=0, OUTPUT=1, INPUT_PULLUP=2, FALLING=3;
extern uint32_t fakeMs;
extern int fakePins[64];
inline uint32_t millis(){return fakeMs;}
inline void delay(uint32_t ms){fakeMs+=ms;}
inline void pinMode(int,int){}
inline void digitalWrite(int pin,int value){fakePins[pin]=value;}
inline int digitalRead(int pin){return fakePins[pin];}
template<class T> T constrain(T x, T low, T high){return x<low?low:(x>high?high:x);}
class String {
public:
  String(const char* s="") : data_(s) {}
  void toCharArray(char* buffer, size_t size) const {
    if (!size) return;
    std::strncpy(buffer,data_.c_str(),size-1);
    buffer[size-1]='\0';
  }
private:
  std::string data_;
};
struct MockSerial {
  void begin(unsigned long){}
  void println(const char*){}
  template<class... Args> void printf(const char*, Args...) {}
};
struct MockESP {
  const char* getChipModel() const {return "ESP32-S3";}
  int getCpuFreqMHz() const {return 240;}
  uint32_t getFlashChipSize() const {return 16U*1024U*1024U;}
  uint32_t getFreePsram() const {return 6U*1024U*1024U;}
  uint32_t getPsramSize() const {return 8U*1024U*1024U;}
  uint32_t getFreeHeap() const {return 200000U;}
};
extern MockSerial Serial;
extern MockESP ESP;
