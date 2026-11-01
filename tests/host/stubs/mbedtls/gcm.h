#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <initializer_list>
#include "tests/support/NativeCrypto.h"
#define MBEDTLS_GCM_ENCRYPT 1
#define MBEDTLS_GCM_DECRYPT 0
#define MBEDTLS_CIPHER_ID_AES 1
typedef int mbedtls_cipher_id_t;
struct mbedtls_gcm_context { uint8_t key[32]{}; };
inline void mbedtls_gcm_init(mbedtls_gcm_context* c) { memset(c, 0, sizeof(*c)); }
inline void mbedtls_gcm_free(mbedtls_gcm_context* c) { memset(c, 0, sizeof(*c)); }
inline int mbedtls_gcm_setkey(mbedtls_gcm_context* c, mbedtls_cipher_id_t,
                             const unsigned char* key, unsigned bits) {
  if (bits != 256) return -1;
  memcpy(c->key, key, 32); return 0;
}
#if !defined(_WIN32)
// Portable synthetic double: rejection/control flow only, NOT AES-GCM.
inline void syntheticTag(mbedtls_gcm_context* c, const uint8_t* iv, size_t ivLen,
    const uint8_t* aad, size_t aadLen, const uint8_t* data, size_t len, uint8_t* tag, size_t tagLen) {
  uint32_t h = 2166136261U;
  for (auto part : {0, 1, 2, 3}) {
    const uint8_t* p = part == 0 ? c->key : part == 1 ? iv : part == 2 ? aad : data;
    size_t n = part == 0 ? 32 : part == 1 ? ivLen : part == 2 ? aadLen : len;
    for (size_t i = 0; i < n; ++i) h = (h ^ p[i]) * 16777619U;
  }
  for (size_t i = 0; i < tagLen; ++i) { h = h * 1664525U + 1013904223U; tag[i] = h >> 24; }
}
#endif
inline int mbedtls_gcm_crypt_and_tag(mbedtls_gcm_context* c, int, size_t len,
    const uint8_t* iv, size_t ivLen, const uint8_t* aad, size_t aadLen,
    const uint8_t* input, uint8_t* output, size_t tagLen, uint8_t* tag) {
#if defined(_WIN32)
  return NativeCrypto::gcm(true, c->key, 32, iv, ivLen, aad, aadLen, input, len, output, tag, tagLen);
#else
  for (size_t i = 0; i < len; ++i) output[i] = input[i] ^ c->key[i % 32] ^ 0x5A;
  syntheticTag(c, iv, ivLen, aad, aadLen, output, len, tag, tagLen); return 0;
#endif
}
inline int mbedtls_gcm_auth_decrypt(mbedtls_gcm_context* c, size_t len,
    const uint8_t* iv, size_t ivLen, const uint8_t* aad, size_t aadLen,
    const uint8_t* tag, size_t tagLen, const uint8_t* input, uint8_t* output) {
#if defined(_WIN32)
  uint8_t copy[16]{};
  if (tagLen != sizeof(copy)) return -1;
  memcpy(copy, tag, sizeof(copy));
  return NativeCrypto::gcm(false, c->key, 32, iv, ivLen, aad, aadLen, input, len, output, copy, tagLen);
#else
  uint8_t expected[16]{};
  if (tagLen != sizeof(expected)) return -1;
  syntheticTag(c, iv, ivLen, aad, aadLen, input, len, expected, tagLen);
  if (memcmp(expected, tag, tagLen)) return -1;
  for (size_t i = 0; i < len; ++i) output[i] = input[i] ^ c->key[i % 32] ^ 0x5A;
  return 0;
#endif
}
