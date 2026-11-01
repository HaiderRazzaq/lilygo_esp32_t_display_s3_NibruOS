#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <ctime>
using byte = uint8_t;
static constexpr int HIGH=1, LOW=0, OUTPUT=1, INPUT_PULLUP=2, FALLING=3;
extern uint32_t fakeMs;
extern int fakePins[64];
inline uint32_t millis(){return fakeMs;}
inline void delay(uint32_t ms){fakeMs+=ms;}
inline void delayMicroseconds(uint32_t){}
inline uint32_t micros(){return fakeMs * 1000U;}
inline void configTime(long,int,const char*){}
#ifdef _WIN32
inline tm* localtime_r(const time_t* t, tm* out) { *out = *std::localtime(t); return out; }
#endif
inline void pinMode(int,int){}
inline void digitalWrite(int pin,int value){fakePins[pin]=value;}
inline int digitalRead(int pin){return fakePins[pin];}
inline bool fakePwmOk = true;
inline uint32_t fakeDuty = 255;
inline uint32_t ledcSetup(uint8_t, uint32_t freq, uint8_t){return fakePwmOk ? freq : 0;}
inline void ledcAttachPin(uint8_t, uint8_t){}
inline void ledcWrite(uint8_t, uint32_t duty){fakeDuty = duty;}
template<class T> T constrain(T x, T low, T high){return x<low?low:(x>high?high:x);}
class String {
public:
  String(const char* s="") : data_(s) {}
  const char* c_str() const { return data_.c_str(); }
  size_t length() const { return data_.length(); }
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
  void flush(){}
  template<class... Args> void printf(const char*, Args...) {}
};
struct MockESP {
  unsigned restarts = 0;
  void restart() { ++restarts; }
  const char* getChipModel() const {return "ESP32-S3";}
  int getCpuFreqMHz() const {return 240;}
  uint32_t getFlashChipSize() const {return 16U*1024U*1024U;}
  uint32_t getFreePsram() const {return 6U*1024U*1024U;}
  uint32_t getPsramSize() const {return 8U*1024U*1024U;}
  uint32_t getFreeHeap() const {return 200000U;}
};
extern MockSerial Serial;
extern MockESP ESP;
