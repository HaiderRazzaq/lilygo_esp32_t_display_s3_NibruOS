#pragma once
#include <stdint.h>
#include <stddef.h>

#define MBEDTLS_GCM_ENCRYPT 1
#define MBEDTLS_GCM_DECRYPT 0
#define MBEDTLS_CIPHER_ID_AES 1

typedef int mbedtls_cipher_id_t;

typedef struct {
  int cipher;
} mbedtls_gcm_context;

inline void mbedtls_gcm_init(mbedtls_gcm_context *ctx) {
  if (ctx) ctx->cipher = 0;
}

inline void mbedtls_gcm_free(mbedtls_gcm_context *ctx) {
  if (ctx) ctx->cipher = 0;
}

inline int mbedtls_gcm_setkey(mbedtls_gcm_context *ctx, mbedtls_cipher_id_t cipher, const unsigned char *, unsigned int) {
  if (ctx) ctx->cipher = cipher;
  return 0;
}

inline int mbedtls_gcm_crypt_and_tag(mbedtls_gcm_context *, int, size_t length,
                                     const unsigned char *, size_t,
                                     const unsigned char *, size_t,
                                     const unsigned char *input, unsigned char *output,
                                     size_t tag_len, unsigned char *tag) {
  for (size_t i = 0; i < length; ++i) {
    output[i] = (unsigned char)(input[i] ^ 0x5A);
  }
  for (size_t i = 0; i < tag_len; ++i) {
    tag[i] = (unsigned char)(0xA0 + (i & 0x0F));
  }
  return 0;
}

inline int mbedtls_gcm_auth_decrypt(mbedtls_gcm_context *, size_t length,
                                    const unsigned char *, size_t,
                                    const unsigned char *, size_t,
                                    const unsigned char *, size_t,
                                    const unsigned char *input, unsigned char *output) {
  for (size_t i = 0; i < length; ++i) {
    output[i] = (unsigned char)(input[i] ^ 0x5A);
  }
  return 0;
}

