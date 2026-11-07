#pragma once
// Windows HOST/SIMULATOR adapter: real CNG primitives behind the mbedTLS API.
// This file is never on the embedded include path. It does not emulate ESP32
// entropy, flash, timing, or hardware acceleration.
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOUSER
#define NOGDI
#include <windows.h>
#include <bcrypt.h>
#include <cstdint>
#include <cstddef>
#include <cstring>

namespace NativeCrypto {
inline int hash(int algo, const uint8_t* key, size_t keyLen,
                const uint8_t* data, size_t size, uint8_t* out) {
  LPCWSTR algId = BCRYPT_SHA256_ALGORITHM;
  ULONG outLen = 32;
  if (algo == 1) {
    algId = BCRYPT_SHA1_ALGORITHM;
    outLen = 20;
  } else if (algo == 2) {
    algId = BCRYPT_SHA512_ALGORITHM;
    outLen = 64;
  }
  BCRYPT_ALG_HANDLE alg = nullptr;
  BCRYPT_HASH_HANDLE hash = nullptr;
  NTSTATUS status = BCryptOpenAlgorithmProvider(&alg, algId, nullptr,
      key ? BCRYPT_ALG_HANDLE_HMAC_FLAG : 0);
  if (status >= 0) status = BCryptCreateHash(alg, &hash, nullptr, 0,
      const_cast<PUCHAR>(key), static_cast<ULONG>(keyLen), 0);
  if (status >= 0) status = BCryptHashData(hash, const_cast<PUCHAR>(data), static_cast<ULONG>(size), 0);
  if (status >= 0) status = BCryptFinishHash(hash, out, outLen, 0);
  if (hash) BCryptDestroyHash(hash);
  if (alg) BCryptCloseAlgorithmProvider(alg, 0);
  return status >= 0 ? 0 : -1;
}
inline int derive(const uint8_t* password, size_t passLen, const uint8_t* salt,
                  size_t saltLen, unsigned iterations, uint8_t* out, size_t len) {
  BCRYPT_ALG_HANDLE alg = nullptr;
  NTSTATUS status = BCryptOpenAlgorithmProvider(&alg, BCRYPT_SHA256_ALGORITHM,
                                               nullptr, BCRYPT_ALG_HANDLE_HMAC_FLAG);
  if (status >= 0) status = BCryptDeriveKeyPBKDF2(alg, const_cast<PUCHAR>(password),
      static_cast<ULONG>(passLen), const_cast<PUCHAR>(salt), static_cast<ULONG>(saltLen),
      iterations, out, static_cast<ULONG>(len), 0);
  if (alg) BCryptCloseAlgorithmProvider(alg, 0);
  return status >= 0 ? 0 : -1;
}
inline int gcm(bool encrypt, const uint8_t* key, size_t keyLen,
               const uint8_t* iv, size_t ivLen, const uint8_t* aad, size_t aadLen,
               const uint8_t* input, size_t len, uint8_t* output,
               uint8_t* tag, size_t tagLen) {
  BCRYPT_ALG_HANDLE alg = nullptr;
  BCRYPT_KEY_HANDLE handle = nullptr;
  NTSTATUS status = BCryptOpenAlgorithmProvider(&alg, BCRYPT_AES_ALGORITHM, nullptr, 0);
  if (status >= 0) status = BCryptSetProperty(alg, BCRYPT_CHAINING_MODE,
      reinterpret_cast<PUCHAR>(const_cast<wchar_t*>(BCRYPT_CHAIN_MODE_GCM)), sizeof(BCRYPT_CHAIN_MODE_GCM), 0);
  if (status >= 0) status = BCryptGenerateSymmetricKey(alg, &handle, nullptr, 0,
      const_cast<PUCHAR>(key), static_cast<ULONG>(keyLen), 0);
  BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO info;
  BCRYPT_INIT_AUTH_MODE_INFO(info);
  info.pbNonce = const_cast<PUCHAR>(iv); info.cbNonce = static_cast<ULONG>(ivLen);
  info.pbAuthData = const_cast<PUCHAR>(aad); info.cbAuthData = static_cast<ULONG>(aadLen);
  info.pbTag = tag; info.cbTag = static_cast<ULONG>(tagLen);
  ULONG written = 0;
  if (status >= 0) {
    status = encrypt ? BCryptEncrypt(handle, const_cast<PUCHAR>(input), static_cast<ULONG>(len),
        &info, nullptr, 0, output, static_cast<ULONG>(len), &written, 0)
      : BCryptDecrypt(handle, const_cast<PUCHAR>(input), static_cast<ULONG>(len),
        &info, nullptr, 0, output, static_cast<ULONG>(len), &written, 0);
  }
  if (handle) BCryptDestroyKey(handle);
  if (alg) BCryptCloseAlgorithmProvider(alg, 0);
  return status >= 0 && written == len ? 0 : -1;
}
}
#endif
