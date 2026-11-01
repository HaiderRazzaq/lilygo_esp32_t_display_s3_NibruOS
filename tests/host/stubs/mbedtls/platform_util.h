#pragma once
#include <string.h>

inline void mbedtls_platform_zeroize(void *buf, size_t len) {
  if (buf && len) {
    volatile unsigned char *p = (volatile unsigned char *)buf;
    while (len--) *p++ = 0;
  }
}

