#pragma once
#include <stdint.h>
#include <stddef.h>
#include <random>

inline uint32_t esp_random() {
  static std::mt19937 s_gen(1337);
  return s_gen();
}

inline void esp_fill_random(void* buf, size_t len) {
  uint8_t* p = static_cast<uint8_t*>(buf);
  for (size_t i = 0; i < len; ++i) {
    p[i] = static_cast<uint8_t>(esp_random() & 0xFF);
  }
}
