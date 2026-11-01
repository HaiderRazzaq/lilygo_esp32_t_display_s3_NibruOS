#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <chrono>
#include <thread>
#include <ctime>

using byte = uint8_t;
static constexpr int HIGH = 1, LOW = 0, OUTPUT = 1, INPUT = 0, INPUT_PULLUP = 2, FALLING = 3;

namespace ArduinoTiming {
  inline const auto s_epoch = std::chrono::steady_clock::now();
  inline bool s_useCustomClock = false;
  inline uint32_t s_customMs = 0;
}

inline void setCustomMillis(uint32_t ms) {
  ArduinoTiming::s_useCustomClock = true;
  ArduinoTiming::s_customMs = ms;
}

inline void clearCustomMillis() {
  ArduinoTiming::s_useCustomClock = false;
}

inline uint32_t millis() {
  if (ArduinoTiming::s_useCustomClock) return ArduinoTiming::s_customMs;
  const auto now = std::chrono::steady_clock::now();
  return static_cast<uint32_t>(
      std::chrono::duration_cast<std::chrono::milliseconds>(now - ArduinoTiming::s_epoch).count()
  );
}

inline uint32_t micros() {
  if (ArduinoTiming::s_useCustomClock) return ArduinoTiming::s_customMs * 1000U;
  const auto now = std::chrono::steady_clock::now();
  return static_cast<uint32_t>(
      std::chrono::duration_cast<std::chrono::microseconds>(now - ArduinoTiming::s_epoch).count()
  );
}

inline void delay(uint32_t ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

inline void delayMicroseconds(uint32_t us) {
  std::this_thread::sleep_for(std::chrono::microseconds(us));
}

inline void configTime(long, int, const char*) {}

#ifdef _WIN32
inline tm* localtime_r(const time_t* t, tm* out) {
  if (!t || !out) return nullptr;
  *out = *std::localtime(t);
  return out;
}
#endif

inline int s_fakePins[64]{};
inline void pinMode(int, int) {}
inline void digitalWrite(int pin, int value) {
  if (pin >= 0 && pin < 64) s_fakePins[pin] = value;
}
inline int digitalRead(int pin) {
  return (pin >= 0 && pin < 64) ? s_fakePins[pin] : HIGH;
}

inline uint32_t ledcSetup(uint8_t, uint32_t freq, uint8_t) { return freq; }
inline void ledcAttachPin(uint8_t, uint8_t) {}
inline void ledcWrite(uint8_t, uint32_t) {}

template<class T> T constrain(T x, T low, T high) {
  return x < low ? low : (x > high ? high : x);
}

class String {
public:
  String(const char* s = "") : data_(s ? s : "") {}
  String(int v) : data_(std::to_string(v)) {}
  String(unsigned int v) : data_(std::to_string(v)) {}
  String(long v) : data_(std::to_string(v)) {}
  String(unsigned long v) : data_(std::to_string(v)) {}
  const char* c_str() const { return data_.c_str(); }
  size_t length() const { return data_.length(); }
  char& operator[](size_t index) { return data_[index]; }
  bool isEmpty() const { return data_.empty(); }
  void toCharArray(char* buffer, size_t size) const {
    if (!size || !buffer) return;
    std::strncpy(buffer, data_.c_str(), size - 1);
    buffer[size - 1] = '\0';
  }
  bool operator==(const String& o) const { return data_ == o.data_; }
  bool operator==(const char* o) const { return data_ == (o ? o : ""); }
private:
  std::string data_;
};

struct DesktopSerial {
  void begin(unsigned long) {}
  void println(const char* s = "") {
    std::printf("%s\n", s ? s : "");
    std::fflush(stdout);
  }
  void flush() { std::fflush(stdout); }
  template<class... Args>
  void printf(const char* fmt, Args... args) {
    std::printf(fmt, args...);
    std::fflush(stdout);
  }
};

struct DesktopESP {
  unsigned restarts = 0;
  void restart() {
    ++restarts;
    std::printf("[ESP] restart requested\n");
  }
  const char* getChipModel() const { return "ESP32-S3 (Simulated)"; }
  int getCpuFreqMHz() const { return 240; }
  uint32_t getFlashChipSize() const { return 16U * 1024U * 1024U; }
  uint32_t getFreePsram() const { return 6U * 1024U * 1024U; }
  uint32_t getPsramSize() const { return 8U * 1024U * 1024U; }
  uint32_t getFreeHeap() const { return 215000U; }
};

inline DesktopSerial Serial;
inline DesktopESP ESP;
