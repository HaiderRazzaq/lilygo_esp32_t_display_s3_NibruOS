#pragma once
#include <stdint.h>
#include <stddef.h>
#include <cstdlib>
#if defined(_WIN32)
#include "tests/support/NativeCrypto.h"
#else
#include <random>
#endif

inline void esp_fill_random(void* buffer, size_t len) {
  auto* p = static_cast<uint8_t*>(buffer);
#if defined(_WIN32)
  if (len && BCryptGenRandom(nullptr, p, static_cast<ULONG>(len), BCRYPT_USE_SYSTEM_PREFERRED_RNG) < 0)
    std::abort(); // Never fall back to a repeatable nonce/key stream.
#else
  std::random_device source;
  for (size_t i = 0; i < len; ++i) p[i] = static_cast<uint8_t>(source());
#endif
}
inline uint32_t esp_random() { uint32_t value; esp_fill_random(&value, sizeof(value)); return value; }
