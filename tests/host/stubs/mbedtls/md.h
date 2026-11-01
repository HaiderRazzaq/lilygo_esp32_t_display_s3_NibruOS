#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>

typedef enum {
  MBEDTLS_MD_NONE = 0,
  MBEDTLS_MD_MD5,
  MBEDTLS_MD_SHA1,
  MBEDTLS_MD_SHA256
} mbedtls_md_type_t;

typedef struct {
  mbedtls_md_type_t type;
} mbedtls_md_info_t;

typedef struct {
  const mbedtls_md_info_t *md_info;
} mbedtls_md_context_t;

inline const mbedtls_md_info_t *mbedtls_md_info_from_type(mbedtls_md_type_t md_type) {
  static const mbedtls_md_info_t info = { md_type };
  return &info;
}

inline void mbedtls_md_init(mbedtls_md_context_t *ctx) {
  if (ctx) ctx->md_info = NULL;
}

inline void mbedtls_md_free(mbedtls_md_context_t *ctx) {
  if (ctx) ctx->md_info = NULL;
}

inline int mbedtls_md_setup(mbedtls_md_context_t *ctx, const mbedtls_md_info_t *md_info, int) {
  if (ctx) ctx->md_info = md_info;
  return 0;
}

inline int mbedtls_md(const mbedtls_md_info_t *, const unsigned char *input, size_t ilen, unsigned char *output) {
  // Mock hash for host test
  for (size_t i = 0; i < 32; ++i) {
    output[i] = (unsigned char)((i < ilen ? input[i] : 0) ^ (i * 7));
  }
  return 0;
}

inline int mbedtls_md_hmac(const mbedtls_md_info_t *, const unsigned char *key, size_t keylen,
                           const unsigned char *input, size_t ilen, unsigned char *output) {
  for (size_t i = 0; i < 32; ++i) {
    unsigned char k = (i < keylen ? key[i] : 0x36);
    unsigned char d = (i < ilen ? input[i] : 0x5c);
    output[i] = (unsigned char)(k ^ d ^ (i * 13));
  }
  return 0;
}

