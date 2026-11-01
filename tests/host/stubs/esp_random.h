#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

inline void esp_fill_random(void *buf, size_t len) {
  uint8_t *p = (uint8_t *)buf;
  for (size_t i = 0; i < len; ++i) {
    p[i] = (uint8_t)(rand() & 0xFF);
  }
}

