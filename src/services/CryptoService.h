#pragma once
#include <stdint.h>
#include <stddef.h>

/**
 * @brief Hardware-accelerated cryptographic service utilizing ESP32-S3 mbedTLS.
 * Provides PBKDF2 key derivation, AES-256-GCM encryption/decryption, HMAC-SHA1/SHA256,
 * TRNG hardware randomness, and secure memory zeroization.
 */
class CryptoService {
public:
  // Fills buffer with hardware true random numbers (ESP32-S3 TRNG)
  static void fillRandom(uint8_t* buf, size_t len);

  // Securely wipes sensitive memory to prevent cold-boot and memory inspection
  static void secureZero(void* ptr, size_t len);

  // Constant-time memory comparison to prevent timing attacks
  static bool constantTimeEquals(const uint8_t* a, const uint8_t* b, size_t len);

  // SHA-256 hash (outHash must be 32 bytes)
  static bool sha256(const uint8_t* data, size_t len, uint8_t* outHash);

  // HMAC-SHA256 calculation (outMac must be 32 bytes)
  static bool hmacSha256(const uint8_t* key, size_t keyLen,
                         const uint8_t* msg, size_t msgLen,
                         uint8_t* outMac);

  // HMAC-SHA1 calculation (required for RFC 6238 TOTP; outMac must be 20 bytes)
  static bool hmacSha1(const uint8_t* key, size_t keyLen,
                       const uint8_t* msg, size_t msgLen,
                       uint8_t* outMac);

  // HMAC-SHA512 calculation (required for SHA-512 TOTP/HOTP; outMac must be 64 bytes)
  static bool hmacSha512(const uint8_t* key, size_t keyLen,
                         const uint8_t* msg, size_t msgLen,
                         uint8_t* outMac);

  // PBKDF2-HMAC-SHA256 key derivation from PIN / passphrase
  static bool deriveKey(const char* password, size_t passLen,
                        const uint8_t* salt, size_t saltLen,
                        uint32_t iterations,
                        uint8_t* outKey, size_t outKeyLen);

  // AES-256-GCM authenticated encryption (keyBits = 256, tagLen = 16)
  static bool aesGcmEncrypt(const uint8_t* key, size_t keyBits,
                            const uint8_t* iv, size_t ivLen,
                            const uint8_t* aad, size_t aadLen,
                            const uint8_t* plaintext, size_t inLen,
                            uint8_t* ciphertext,
                            uint8_t* tag, size_t tagLen);

  // AES-256-GCM authenticated decryption with tag verification
  static bool aesGcmDecrypt(const uint8_t* key, size_t keyBits,
                            const uint8_t* iv, size_t ivLen,
                            const uint8_t* aad, size_t aadLen,
                            const uint8_t* ciphertext, size_t inLen,
                            const uint8_t* tag, size_t tagLen,
                            uint8_t* plaintext);
};

