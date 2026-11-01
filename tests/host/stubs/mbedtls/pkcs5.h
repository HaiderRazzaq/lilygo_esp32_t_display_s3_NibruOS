#pragma once
#include "md.h"

inline int mbedtls_pkcs5_pbkdf2_hmac(mbedtls_md_context_t *,
                                     const unsigned char *password, size_t plen,
                                     const unsigned char *salt, size_t slen,
                                     unsigned int,
                                     uint32_t key_length,
                                     unsigned char *output) {
  for (uint32_t i = 0; i < key_length; ++i) {
    unsigned char p = (i < plen ? password[i] : 0x11);
    unsigned char s = (i < slen ? salt[i] : 0x22);
    output[i] = (unsigned char)(p ^ s ^ (i * 31));
  }
  return 0;
}

