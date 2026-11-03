#include "services/CryptoService.h"
#include <esp_random.h>
#include <mbedtls/md.h>
#include <mbedtls/pkcs5.h>
#include <mbedtls/gcm.h>
#include <mbedtls/platform_util.h>
#include <string.h>
#if defined(ARDUINO_ARCH_ESP32)
#include <bootloader_random.h>
#include <esp_wifi.h>
#endif

void CryptoService::fillRandom(uint8_t* buf, size_t len) {
  if (!buf || len == 0) return;
#if defined(ARDUINO_ARCH_ESP32)
  // Current firmware has no BLE/ADC service. With Wi-Fi off, briefly own the
  // documented SAR entropy source; release it before later radio/ADC use.
  wifi_mode_t mode = WIFI_MODE_NULL;
  const bool ownEntropy = esp_wifi_get_mode(&mode) != ESP_OK || mode == WIFI_MODE_NULL;
  if (ownEntropy) bootloader_random_enable();
#endif
  esp_fill_random(buf, len);
#if defined(ARDUINO_ARCH_ESP32)
  if (ownEntropy) bootloader_random_disable();
#endif
}

void CryptoService::secureZero(void* ptr, size_t len) {
  if (!ptr || len == 0) return;
  mbedtls_platform_zeroize(ptr, len);
}

bool CryptoService::constantTimeEquals(const uint8_t* a, const uint8_t* b, size_t len) {
  if (!a || !b) return false;
  uint8_t diff = 0;
  for (size_t i = 0; i < len; ++i) {
    diff |= (a[i] ^ b[i]);
  }
  return diff == 0;
}

bool CryptoService::sha256(const uint8_t* data, size_t len, uint8_t* outHash) {
  if (!data || !outHash) return false;
  const mbedtls_md_info_t* mdInfo = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
  if (!mdInfo) return false;
  return (mbedtls_md(mdInfo, data, len, outHash) == 0);
}

bool CryptoService::hmacSha256(const uint8_t* key, size_t keyLen,
                               const uint8_t* msg, size_t msgLen,
                               uint8_t* outMac) {
  if (!key || !msg || !outMac) return false;
  const mbedtls_md_info_t* mdInfo = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
  if (!mdInfo) return false;
  return (mbedtls_md_hmac(mdInfo, key, keyLen, msg, msgLen, outMac) == 0);
}

bool CryptoService::hmacSha1(const uint8_t* key, size_t keyLen,
                             const uint8_t* msg, size_t msgLen,
                             uint8_t* outMac) {
  if (!key || !msg || !outMac) return false;
  const mbedtls_md_info_t* mdInfo = mbedtls_md_info_from_type(MBEDTLS_MD_SHA1);
  if (!mdInfo) return false;
  return (mbedtls_md_hmac(mdInfo, key, keyLen, msg, msgLen, outMac) == 0);
}

bool CryptoService::deriveKey(const char* password, size_t passLen,
                              const uint8_t* salt, size_t saltLen,
                              uint32_t iterations,
                              uint8_t* outKey, size_t outKeyLen) {
  if (!password || passLen == 0 || !salt || saltLen == 0 || !outKey || outKeyLen == 0) {
    return false;
  }
  const mbedtls_md_info_t* mdInfo = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
  if (!mdInfo) return false;

  mbedtls_md_context_t ctx;
  mbedtls_md_init(&ctx);
  int ret = mbedtls_md_setup(&ctx, mdInfo, 1);
  if (ret != 0) {
    mbedtls_md_free(&ctx);
    return false;
  }

  ret = mbedtls_pkcs5_pbkdf2_hmac(&ctx,
                                  reinterpret_cast<const unsigned char*>(password), passLen,
                                  salt, saltLen,
                                  iterations, static_cast<uint32_t>(outKeyLen), outKey);
  mbedtls_md_free(&ctx);
  return (ret == 0);
}

bool CryptoService::aesGcmEncrypt(const uint8_t* key, size_t keyBits,
                                  const uint8_t* iv, size_t ivLen,
                                  const uint8_t* aad, size_t aadLen,
                                  const uint8_t* plaintext, size_t inLen,
                                  uint8_t* ciphertext,
                                  uint8_t* tag, size_t tagLen) {
  if (!key || !iv || !tag || (inLen > 0 && (!plaintext || !ciphertext))) return false;
  mbedtls_gcm_context ctx;
  mbedtls_gcm_init(&ctx);
  int ret = mbedtls_gcm_setkey(&ctx, MBEDTLS_CIPHER_ID_AES, key, static_cast<unsigned int>(keyBits));
  if (ret != 0) {
    mbedtls_gcm_free(&ctx);
    return false;
  }

  ret = mbedtls_gcm_crypt_and_tag(&ctx, MBEDTLS_GCM_ENCRYPT, inLen,
                                  iv, ivLen,
                                  aad, aadLen,
                                  plaintext, ciphertext,
                                  tagLen, tag);
  mbedtls_gcm_free(&ctx);
  return (ret == 0);
}

bool CryptoService::aesGcmDecrypt(const uint8_t* key, size_t keyBits,
                                  const uint8_t* iv, size_t ivLen,
                                  const uint8_t* aad, size_t aadLen,
                                  const uint8_t* ciphertext, size_t inLen,
                                  const uint8_t* tag, size_t tagLen,
                                  uint8_t* plaintext) {
  if (!key || !iv || !tag || (inLen > 0 && (!ciphertext || !plaintext))) return false;
  mbedtls_gcm_context ctx;
  mbedtls_gcm_init(&ctx);
  int ret = mbedtls_gcm_setkey(&ctx, MBEDTLS_CIPHER_ID_AES, key, static_cast<unsigned int>(keyBits));
  if (ret != 0) {
    mbedtls_gcm_free(&ctx);
    return false;
  }

  ret = mbedtls_gcm_auth_decrypt(&ctx, inLen,
                                 iv, ivLen,
                                 aad, aadLen,
                                 tag, tagLen,
                                 ciphertext, plaintext);
  mbedtls_gcm_free(&ctx);
  return (ret == 0);
}

