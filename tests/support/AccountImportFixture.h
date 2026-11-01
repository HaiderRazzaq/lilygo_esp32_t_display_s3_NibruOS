#pragma once
#include "services/AccountImportService.h"
#include "services/CryptoService.h"
#include <cassert>
#include <cstring>
#include <string>
namespace ImportFixture {
inline std::string input;
inline unsigned closes = 0;
inline bool opened = false;
inline bool openImport(ImportMode) { opened = true; return true; }
inline void closeImport() { opened = false; ++closes; }
inline int readImport(ImportMode, uint8_t* output, size_t capacity) {
  const size_t n = input.size() < capacity ? input.size() : capacity;
  memcpy(output, input.data(), n); input.erase(0, n); return static_cast<int>(n);
}
inline std::string hex(const uint8_t* p, size_t n) {
  const char* alphabet = "0123456789abcdef";
  std::string result;
  for (size_t i = 0; i < n; ++i) { result += alphabet[p[i] >> 4]; result += alphabet[p[i] & 15]; }
  return result;
}
inline std::string frameForCode(const char* pairing, const char* plain) {
  uint8_t key[32]{}, iv[12]{}, cipher[96]{}, tag[16]{};
  assert(CryptoService::sha256(reinterpret_cast<const uint8_t*>(pairing), 16, key));
  iv[0] = 19;
  const char* aad = "NibruOS/account-import/v1";
  assert(CryptoService::aesGcmEncrypt(key, 256, iv, 12, reinterpret_cast<const uint8_t*>(aad), strlen(aad),
      reinterpret_cast<const uint8_t*>(plain), strlen(plain), cipher, tag, 16));
  return "NI1:" + hex(iv, 12) + ":" + hex(cipher, strlen(plain)) + ":" + hex(tag, 16) + "\n";
}
inline std::string frame(AccountImportService& service, const char* plain) { return frameForCode(service.pairingCode(), plain); }
inline void pump(AccountImportService& service, uint32_t now = 100) {
  for (int i = 0; i < 8; ++i) service.tick(now + i);
}
}

