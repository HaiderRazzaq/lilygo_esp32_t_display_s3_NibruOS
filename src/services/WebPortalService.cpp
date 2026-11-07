#include "services/WebPortalService.h"
#include <Arduino.h>
#include "core/AppContext.h"
#include "core/ResourceManager.h"
#include "services/VaultService.h"
#include "services/TotpService.h"
#include "services/SecurityService.h"
#include "services/CryptoService.h"
#include "services/SettingsService.h"
#include "services/TimeService.h"
#include "services/BleHidService.h"
#include "hardware/DisplayManager.h"
#include "NibruConfig.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

#if defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include "services/WebPortalHtml.h"

namespace {
WebServer server(WebPortalService::kHttpPort);
DNSServer dnsServer;
static char s_apiResp[3072];
static AppContext* s_appContext = nullptr;
static bool s_routesRegistered = false;
}
#endif

namespace {

static bool extractJsonString(const char* json, const char* key, char* out, size_t maxLen) {
  if (!json || !key || !out || maxLen == 0) return false;
  out[0] = '\0';
  char needle[36];
  snprintf(needle, sizeof(needle), "\"%s\"", key);
  const char* pos = strstr(json, needle);
  if (!pos) return false;
  pos += strlen(needle);
  while (*pos && (*pos == ' ' || *pos == '\t' || *pos == '\r' || *pos == '\n' || *pos == ':')) ++pos;
  if (*pos != '"') return false;
  ++pos;
  size_t idx = 0;
  while (*pos && *pos != '"' && idx + 1 < maxLen) {
    if (*pos == '\\' && *(pos + 1)) ++pos;
    out[idx++] = *pos++;
  }
  out[idx] = '\0';
  return true;
}

static int extractJsonInt(const char* json, const char* key, int defaultValue = 0) {
  if (!json || !key) return defaultValue;
  char needle[36];
  snprintf(needle, sizeof(needle), "\"%s\"", key);
  const char* pos = strstr(json, needle);
  if (!pos) return defaultValue;
  pos += strlen(needle);
  while (*pos && (*pos == ' ' || *pos == '\t' || *pos == '\r' || *pos == '\n' || *pos == ':')) ++pos;
  if (*pos == '"') ++pos;
  return atoi(pos);
}

static void toHex(const uint8_t* in, size_t inLen, char* out, size_t maxOut) {
  static const char kHexDigits[] = "0123456789abcdef";
  size_t o = 0;
  for (size_t i = 0; i < inLen && (o + 3) <= maxOut; ++i) {
    out[o++] = kHexDigits[(in[i] >> 4) & 0x0F];
    out[o++] = kHexDigits[in[i] & 0x0F];
  }
  out[o] = '\0';
}

static size_t fromHex(const char* in, uint8_t* out, size_t maxOut) {
  if (!in) return 0;
  size_t len = 0;
  auto hexVal = [](char c) -> int {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
  };
  while (*in && *(in + 1) && len < maxOut) {
    int h1 = hexVal(*in++);
    int h2 = hexVal(*in++);
    if (h1 < 0 || h2 < 0) break;
    out[len++] = static_cast<uint8_t>((h1 << 4) | h2);
  }
  return len;
}

// kIndexHtml replaced by gzipped PROGMEM payload in WebPortalHtml.h

} // namespace

void WebPortalService::generateCredentials() {
  uint32_t randVal = 0;
#if defined(ARDUINO_ARCH_ESP32)
  randVal = esp_random();
#else
  static uint32_t seed = 0x5a1f29b3;
  seed = seed * 1664525U + 1013904223U;
  randVal = seed;
#endif

  // SSID: NibruOS-XXXX
  snprintf(ssid_, sizeof(ssid_), "NibruOS-%04X", static_cast<unsigned int>(randVal & 0xFFFF));

  // WPA2 Password: nb + 6 digits (e.g. nb749215)
  snprintf(password_, sizeof(password_), "nb%06u", static_cast<unsigned int>((randVal >> 4) % 1000000U));

  // Web PIN: 4 digits (e.g. 4819)
  snprintf(webPin_, sizeof(webPin_), "%04u", static_cast<unsigned int>((randVal >> 12) % 10000U));
}

uint32_t WebPortalService::remainingSeconds(uint32_t nowMs) const {
  if (state_ != WebPortalState::Active) return 0;
  const uint32_t elapsedSec = (nowMs - startedMs_) / 1000U;
  if (elapsedSec >= timeoutSec_) return 0;
  return timeoutSec_ - elapsedSec;
}

bool WebPortalService::start(uint32_t nowMs, AppContext& ctx) {
  if (state_ == WebPortalState::Active) return true;

  // If BLE Keyboard is currently holding the radio lease or enabled, suspend it temporarily
  // so that SoftAP gets dedicated, non-interfering radio bandwidth and full heap headroom.
  if (ctx.bleHid.isEnabled() || ctx.resources.ownsWiFi(ResourceOwner::BleKeyboard)) {
    bleSuspended_ = true;
    ctx.bleHid.setEnabled(false, ctx);
  } else {
    bleSuspended_ = false;
  }

  if (!ctx.resources.acquireWiFi(ResourceOwner::WebPortal)) {
    if (bleSuspended_) {
      ctx.bleHid.setEnabled(true, ctx);
      bleSuspended_ = false;
    }
    state_ = WebPortalState::Error;
    return false;
  }
  generateCredentials();
  startedMs_ = nowMs;
  timeoutSec_ = kDefaultTimeoutSec;

#if defined(ARDUINO_ARCH_ESP32)
  WiFi.mode(WIFI_AP);
  const bool apOk = WiFi.softAP(ssid_, password_);
  IPAddress apIp = WiFi.softAPIP();
  if (!apOk) {
    Serial.println("[WebPortal] Failed to start SoftAP!");
    ctx.resources.releaseWiFi(ResourceOwner::WebPortal);
    if (bleSuspended_) {
      ctx.bleHid.setEnabled(true, ctx);
      bleSuspended_ = false;
    }
    state_ = WebPortalState::Error;
    return false;
  }
  Serial.printf("[WebPortal] SoftAP started successfully. IP: %s\n", apIp.toString().c_str());

  s_appContext = &ctx;
  dnsServer.start(53, "*", apIp);

  if (!s_routesRegistered) {
    s_routesRegistered = true;

    server.on("/", HTTP_GET, []() {
      server.sendHeader("Content-Encoding", "gzip");
      server.send_P(200, "text/html", reinterpret_cast<PGM_P>(kIndexHtmlGz), sizeof(kIndexHtmlGz));
    });

    // Captive portal probes for Android, iOS, Windows
    auto handleRedirect = []() {
      server.sendHeader("Location", String("http://") + WiFi.softAPIP().toString() + "/", true);
      server.send(302, "text/plain", "");
    };

    server.on("/generate_204", HTTP_GET, handleRedirect);
    server.on("/gen_204", HTTP_GET, handleRedirect);
    server.on("/hotspot-detect.html", HTTP_GET, handleRedirect);
    server.on("/canonical.html", HTTP_GET, handleRedirect);
    server.on("/ncsi.txt", HTTP_GET, handleRedirect);
    server.on("/connecttest.txt", HTTP_GET, handleRedirect);

    server.on("/api/status", HTTP_GET, [this]() {
      if (!s_appContext) { server.send(500, "application/json", "{\"error\":\"No context\"}"); return; }
      handleStatus(*s_appContext, s_apiResp, sizeof(s_apiResp));
      server.send(200, "application/json", s_apiResp);
    });

    server.on("/api/totp/list", HTTP_GET, [this]() {
      if (!s_appContext) { server.send(500, "application/json", "{\"error\":\"No context\"}"); return; }
      int code = handleTotpList(*s_appContext, s_apiResp, sizeof(s_apiResp));
      server.send(code, "application/json", s_apiResp);
    });

    server.on("/api/totp/add", HTTP_POST, [this]() {
      if (!s_appContext) { server.send(500, "application/json", "{\"error\":\"No context\"}"); return; }
      String body = server.arg("plain");
      int code = handleAddTotp(*s_appContext, body.c_str(), s_apiResp, sizeof(s_apiResp));
      server.send(code, "application/json", s_apiResp);
    });

    server.on("/api/totp/remove", HTTP_POST, [this]() {
      if (!s_appContext) { server.send(500, "application/json", "{\"error\":\"No context\"}"); return; }
      String body = server.arg("plain");
      int code = handleRemoveTotp(*s_appContext, body.c_str(), s_apiResp, sizeof(s_apiResp));
      server.send(code, "application/json", s_apiResp);
    });

    server.on("/api/totp/next_hotp", HTTP_POST, [this]() {
      if (!s_appContext) { server.send(500, "application/json", "{\"error\":\"No context\"}"); return; }
      String body = server.arg("plain");
      int code = handleNextHotp(*s_appContext, body.c_str(), s_apiResp, sizeof(s_apiResp));
      server.send(code, "application/json", s_apiResp);
    });

    server.on("/api/vault/list", HTTP_GET, [this]() {
      if (!s_appContext) { server.send(500, "application/json", "{\"error\":\"No context\"}"); return; }
      int code = handleVaultList(*s_appContext, s_apiResp, sizeof(s_apiResp));
      server.send(code, "application/json", s_apiResp);
    });

    server.on("/api/vault/add", HTTP_POST, [this]() {
      if (!s_appContext) { server.send(500, "application/json", "{\"error\":\"No context\"}"); return; }
      String body = server.arg("plain");
      int code = handleAddVault(*s_appContext, body.c_str(), s_apiResp, sizeof(s_apiResp));
      server.send(code, "application/json", s_apiResp);
    });

    server.on("/api/vault/remove", HTTP_POST, [this]() {
      if (!s_appContext) { server.send(500, "application/json", "{\"error\":\"No context\"}"); return; }
      String body = server.arg("plain");
      int code = handleRemoveVault(*s_appContext, body.c_str(), s_apiResp, sizeof(s_apiResp));
      server.send(code, "application/json", s_apiResp);
    });

    server.on("/api/vault/backup", HTTP_POST, [this]() {
      if (!s_appContext) { server.send(500, "application/json", "{\"error\":\"No context\"}"); return; }
      String body = server.arg("plain");
      int code = handleBackup(*s_appContext, body.c_str(), s_apiResp, sizeof(s_apiResp));
      server.send(code, "application/json", s_apiResp);
    });

    server.on("/api/vault/restore", HTTP_POST, [this]() {
      if (!s_appContext) { server.send(500, "application/json", "{\"error\":\"No context\"}"); return; }
      String body = server.arg("plain");
      int code = handleRestore(*s_appContext, body.c_str(), s_apiResp, sizeof(s_apiResp));
      server.send(code, "application/json", s_apiResp);
    });

    server.on("/api/device/display", HTTP_POST, [this]() {
      if (!s_appContext) { server.send(500, "application/json", "{\"error\":\"No context\"}"); return; }
      String body = server.arg("plain");
      int code = handleDeviceDisplay(*s_appContext, body.c_str(), s_apiResp, sizeof(s_apiResp));
      server.send(code, "application/json", s_apiResp);
    });

    server.on("/api/device/lock", HTTP_POST, [this]() {
      if (!s_appContext) { server.send(500, "application/json", "{\"error\":\"No context\"}"); return; }
      String body = server.arg("plain");
      int code = handleDeviceLock(*s_appContext, body.c_str(), s_apiResp, sizeof(s_apiResp));
      server.send(code, "application/json", s_apiResp);
    });

    server.onNotFound([]() {
      String uri = server.uri();
      if (uri != "/" && !uri.startsWith("/api/")) {
        server.sendHeader("Location", String("http://") + WiFi.softAPIP().toString() + "/", true);
        server.send(302, "text/plain", "");
        return;
      }
      server.sendHeader("Content-Encoding", "gzip");
      server.send_P(200, "text/html", reinterpret_cast<PGM_P>(kIndexHtmlGz), sizeof(kIndexHtmlGz));
    });
  }

  server.begin();
#endif

  state_ = WebPortalState::Active;
  return true;
}

void WebPortalService::stop(AppContext& ctx) {
  if (state_ == WebPortalState::Idle) return;
#if defined(ARDUINO_ARCH_ESP32)
  server.stop();
  dnsServer.stop();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);
#endif
  ctx.resources.releaseWiFi(ResourceOwner::WebPortal);
  state_ = WebPortalState::Idle;
  CryptoService::secureZero(password_, sizeof(password_));
  CryptoService::secureZero(webPin_, sizeof(webPin_));

  // Automatically restore BLE Keyboard if it was suspended for Web Portal
  if (bleSuspended_) {
    bleSuspended_ = false;
    ctx.bleHid.setEnabled(true, ctx);
    Serial.println("[WebPortal] Restored BLE Keyboard after Web Portal closed");
  }
}

void WebPortalService::tick(uint32_t nowMs, AppContext& ctx) {
  if (state_ != WebPortalState::Active) return;
  if (remainingSeconds(nowMs) == 0) {
    stop(ctx);
    return;
  }
#if defined(ARDUINO_ARCH_ESP32)
  dnsServer.processNextRequest();
  server.handleClient();
#endif
}

int WebPortalService::handleStatus(AppContext& ctx, char* outResponse, size_t maxLen) {
  const bool unlocked = ctx.security.hasActiveKey();
  const uint32_t epoch = ctx.time.getEpoch();
  const uint8_t keyCount = ctx.totp.isLoaded() ? ctx.totp.accountCount() : 0;
  const uint8_t vaultCount = ctx.vault.isLoaded() ? ctx.vault.count() : 0;
  snprintf(outResponse, maxLen,
           "{\"ok\":true,\"device\":\"NibruOS\",\"version\":\"%s\",\"remaining\":%u,\"unlocked\":%s,\"epoch\":%lu,\"keysCount\":%u,\"vaultCount\":%u}",
           Config::kVersion,
           static_cast<unsigned int>(remainingSeconds(millis())),
           unlocked ? "true" : "false",
           static_cast<unsigned long>(epoch),
           keyCount, vaultCount);
  return 200;
}

int WebPortalService::handleTotpList(AppContext& ctx, char* outResponse, size_t maxLen) {
  if (!ctx.security.hasActiveKey()) {
    snprintf(outResponse, maxLen, "{\"ok\":false,\"error\":\"Device locked\",\"keys\":[]}");
    return 403;
  }
  if (!ctx.totp.isLoaded()) {
    ctx.totp.loadVault(ctx.security.getActiveVaultKey());
  }
  const uint8_t count = ctx.totp.accountCount();
  const uint32_t epoch = ctx.time.getEpoch();
  size_t offset = snprintf(outResponse, maxLen, "{\"ok\":true,\"epoch\":%lu,\"keys\":[", static_cast<unsigned long>(epoch));
  for (uint8_t i = 0; i < count; ++i) {
    const TotpAccount* acc = ctx.totp.getAccount(i);
    if (!acc) continue;
    char code[10] = "------";
    uint8_t remaining = 0;
    ctx.totp.getAccountCode(i, epoch, code, sizeof(code), remaining);
    const bool isHotp = (static_cast<OtpType>(acc->type) == OtpType::Hotp);
    const char* algoStr = (acc->algo == static_cast<uint8_t>(OtpAlgo::Sha256)) ? "sha256" :
                          ((acc->algo == static_cast<uint8_t>(OtpAlgo::Sha512)) ? "sha512" : "sha1");
    char item[256];
    snprintf(item, sizeof(item),
             "%s{\"index\":%u,\"name\":\"%s\",\"user\":\"%s\",\"code\":\"%s\",\"timer\":%u,\"period\":%u,\"type\":\"%s\",\"algo\":\"%s\",\"digits\":%u,\"counter\":%lu}",
             (i > 0 ? "," : ""),
             i, acc->name, acc->user, code,
             isHotp ? 0 : remaining,
             acc->period,
             isHotp ? "hotp" : "totp",
             algoStr,
             acc->digits,
             static_cast<unsigned long>(acc->counter));
    CryptoService::secureZero(code, sizeof(code));
    const size_t itemLen = strlen(item);
    if (offset + itemLen + 4 < maxLen) {
      memcpy(outResponse + offset, item, itemLen);
      offset += itemLen;
      outResponse[offset] = '\0';
    }
  }
  if (offset + 3 < maxLen) {
    outResponse[offset++] = ']';
    outResponse[offset++] = '}';
    outResponse[offset] = '\0';
  }
  return 200;
}

int WebPortalService::handleAddTotp(AppContext& ctx, const char* body, char* outResponse, size_t maxLen) {
  char pin[16]{}, name[24]{}, user[32]{}, secret[65]{}, typeStr[16]{}, algoStr[16]{};
  extractJsonString(body, "pin", pin, sizeof(pin));
  extractJsonString(body, "name", name, sizeof(name));
  extractJsonString(body, "user", user, sizeof(user));
  extractJsonString(body, "secret", secret, sizeof(secret));
  extractJsonString(body, "type", typeStr, sizeof(typeStr));
  extractJsonString(body, "algo", algoStr, sizeof(algoStr));
  int digits = extractJsonInt(body, "digits", 6);
  int period = extractJsonInt(body, "period", 30);
  int counter = extractJsonInt(body, "counter", 0);

  if (strcmp(pin, webPin_) != 0) {
    snprintf(outResponse, maxLen, "{\"error\":\"Invalid Web PIN\"}");
    return 401;
  }

  if (!ctx.security.hasActiveKey()) {
    snprintf(outResponse, maxLen, "{\"error\":\"Device locked. Unlock on device first.\"}");
    return 403;
  }

  if (!name[0] || !secret[0]) {
    snprintf(outResponse, maxLen, "{\"error\":\"Service name and Secret key are required\"}");
    return 400;
  }

  if (!ctx.totp.isLoaded()) {
    if (!ctx.totp.loadVault(ctx.security.getActiveVaultKey())) {
      snprintf(outResponse, maxLen, "{\"error\":\"Failed to load 2FA storage\"}");
      return 500;
    }
  }

  OtpType type = (strcmp(typeStr, "hotp") == 0) ? OtpType::Hotp : OtpType::Totp;
  OtpAlgo algo = OtpAlgo::Sha1;
  if (strcmp(algoStr, "sha256") == 0) algo = OtpAlgo::Sha256;
  else if (strcmp(algoStr, "sha512") == 0) algo = OtpAlgo::Sha512;

  if (!ctx.totp.addAccount(ctx.security.getActiveVaultKey(), name, user[0] ? user : "Account", secret,
                           static_cast<uint8_t>(digits), static_cast<uint8_t>(period),
                           type, algo, static_cast<uint32_t>(counter))) {
    snprintf(outResponse, maxLen, "{\"error\":\"Failed to save 2FA account (invalid Base32 or storage full)\"}");
    return 400;
  }

  snprintf(outResponse, maxLen, "{\"success\":true,\"count\":%u}", static_cast<unsigned int>(ctx.totp.accountCount()));
  return 200;
}

int WebPortalService::handleRemoveTotp(AppContext& ctx, const char* body, char* outResponse, size_t maxLen) {
  char pin[16]{};
  extractJsonString(body, "pin", pin, sizeof(pin));
  if (strcmp(pin, webPin_) != 0) {
    snprintf(outResponse, maxLen, "{\"error\":\"Invalid Web PIN\"}");
    return 401;
  }
  if (!ctx.security.hasActiveKey()) {
    snprintf(outResponse, maxLen, "{\"error\":\"Device locked\"}");
    return 403;
  }
  int index = extractJsonInt(body, "index", -1);
  if (index < 0 || index >= ctx.totp.accountCount()) {
    snprintf(outResponse, maxLen, "{\"error\":\"Invalid index\"}");
    return 400;
  }
  if (!ctx.totp.removeAccount(ctx.security.getActiveVaultKey(), static_cast<uint8_t>(index))) {
    snprintf(outResponse, maxLen, "{\"error\":\"Failed to remove account\"}");
    return 500;
  }
  snprintf(outResponse, maxLen, "{\"success\":true,\"count\":%u}", static_cast<unsigned int>(ctx.totp.accountCount()));
  return 200;
}

int WebPortalService::handleNextHotp(AppContext& ctx, const char* body, char* outResponse, size_t maxLen) {
  char pin[16]{};
  extractJsonString(body, "pin", pin, sizeof(pin));
  if (strcmp(pin, webPin_) != 0) {
    snprintf(outResponse, maxLen, "{\"error\":\"Invalid Web PIN\"}");
    return 401;
  }
  if (!ctx.security.hasActiveKey()) {
    snprintf(outResponse, maxLen, "{\"error\":\"Device locked\"}");
    return 403;
  }
  int index = extractJsonInt(body, "index", -1);
  if (index < 0 || index >= ctx.totp.accountCount()) {
    snprintf(outResponse, maxLen, "{\"error\":\"Invalid index\"}");
    return 400;
  }
  if (!ctx.totp.incrementHotp(ctx.security.getActiveVaultKey(), static_cast<uint8_t>(index))) {
    snprintf(outResponse, maxLen, "{\"error\":\"Failed to advance counter\"}");
    return 500;
  }
  const TotpAccount* acc = ctx.totp.getAccount(static_cast<uint8_t>(index));
  char code[10]{};
  uint8_t rem = 0;
  ctx.totp.getAccountCode(static_cast<uint8_t>(index), 0, code, sizeof(code), rem);
  snprintf(outResponse, maxLen, "{\"success\":true,\"code\":\"%s\",\"counter\":%lu}",
           code, static_cast<unsigned long>(acc ? acc->counter : 0));
  CryptoService::secureZero(code, sizeof(code));
  return 200;
}

int WebPortalService::handleVaultList(AppContext& ctx, char* outResponse, size_t maxLen) {
  if (!ctx.security.hasActiveKey()) {
    snprintf(outResponse, maxLen, "{\"ok\":false,\"error\":\"Device locked\",\"passwords\":[]}");
    return 403;
  }
  if (!ctx.vault.isLoaded()) {
    ctx.vault.loadVault(ctx.security.getActiveVaultKey());
  }
  const uint8_t count = ctx.vault.count();
  size_t offset = snprintf(outResponse, maxLen, "{\"ok\":true,\"passwords\":[");
  for (uint8_t i = 0; i < count; ++i) {
    const VaultEntry* entry = ctx.vault.getEntry(i);
    if (!entry) continue;
    char item[256];
    snprintf(item, sizeof(item),
             "%s{\"index\":%u,\"title\":\"%s\",\"username\":\"%s\",\"password\":\"%s\",\"notes\":\"%s\"}",
             (i > 0 ? "," : ""),
             i, entry->title, entry->username, entry->password, entry->notes);
    const size_t itemLen = strlen(item);
    if (offset + itemLen + 4 < maxLen) {
      memcpy(outResponse + offset, item, itemLen);
      offset += itemLen;
      outResponse[offset] = '\0';
    }
  }
  if (offset + 3 < maxLen) {
    outResponse[offset++] = ']';
    outResponse[offset++] = '}';
    outResponse[offset] = '\0';
  }
  return 200;
}

int WebPortalService::handleAddVault(AppContext& ctx, const char* body, char* outResponse, size_t maxLen) {
  char pin[16]{}, title[24]{}, user[32]{}, pass[48]{}, notes[24]{};
  extractJsonString(body, "pin", pin, sizeof(pin));
  extractJsonString(body, "title", title, sizeof(title));
  extractJsonString(body, "username", user, sizeof(user));
  extractJsonString(body, "password", pass, sizeof(pass));
  extractJsonString(body, "notes", notes, sizeof(notes));

  if (strcmp(pin, webPin_) != 0) {
    snprintf(outResponse, maxLen, "{\"error\":\"Invalid Web PIN\"}");
    return 401;
  }

  if (!ctx.security.hasActiveKey()) {
    snprintf(outResponse, maxLen, "{\"error\":\"Device locked. Unlock on device first.\"}");
    return 403;
  }

  if (!title[0] || !pass[0]) {
    snprintf(outResponse, maxLen, "{\"error\":\"Title and Password are required\"}");
    return 400;
  }

  if (!ctx.vault.isLoaded()) {
    if (!ctx.vault.loadVault(ctx.security.getActiveVaultKey())) {
      snprintf(outResponse, maxLen, "{\"error\":\"Failed to load vault\"}");
      return 500;
    }
  }

  if (!ctx.vault.addEntry(ctx.security.getActiveVaultKey(), title, user, pass, notes)) {
    snprintf(outResponse, maxLen, "{\"error\":\"Vault storage full or write error\"}");
    return 500;
  }

  snprintf(outResponse, maxLen, "{\"success\":true,\"count\":%u}", static_cast<unsigned int>(ctx.vault.count()));
  return 200;
}

int WebPortalService::handleRemoveVault(AppContext& ctx, const char* body, char* outResponse, size_t maxLen) {
  char pin[16]{};
  extractJsonString(body, "pin", pin, sizeof(pin));
  if (strcmp(pin, webPin_) != 0) {
    snprintf(outResponse, maxLen, "{\"error\":\"Invalid Web PIN\"}");
    return 401;
  }
  if (!ctx.security.hasActiveKey()) {
    snprintf(outResponse, maxLen, "{\"error\":\"Device locked\"}");
    return 403;
  }
  int index = extractJsonInt(body, "index", -1);
  if (index < 0 || index >= ctx.vault.count()) {
    snprintf(outResponse, maxLen, "{\"error\":\"Invalid index\"}");
    return 400;
  }
  if (!ctx.vault.removeEntry(ctx.security.getActiveVaultKey(), static_cast<uint8_t>(index))) {
    snprintf(outResponse, maxLen, "{\"error\":\"Failed to remove password\"}");
    return 500;
  }
  snprintf(outResponse, maxLen, "{\"success\":true,\"count\":%u}", static_cast<unsigned int>(ctx.vault.count()));
  return 200;
}

int WebPortalService::handleBackup(AppContext& ctx, const char* body, char* outResponse, size_t maxLen) {
  char pin[16]{}, pass[48]{};
  extractJsonString(body, "pin", pin, sizeof(pin));
  extractJsonString(body, "passphrase", pass, sizeof(pass));

  if (strcmp(pin, webPin_) != 0) {
    snprintf(outResponse, maxLen, "{\"error\":\"Invalid Web PIN\"}");
    return 401;
  }
  if (!pass[0]) {
    snprintf(outResponse, maxLen, "{\"error\":\"Passphrase is required\"}");
    return 400;
  }
  if (!ctx.vault.isLoaded()) {
    if (!ctx.vault.loadVault(ctx.security.getActiveVaultKey())) {
      snprintf(outResponse, maxLen, "{\"error\":\"Failed to load vault\"}");
      return 500;
    }
  }

  uint8_t payload[2048]{};
  size_t outLen = 0;
  if (!ctx.vault.exportBackup(pass, payload, sizeof(payload), outLen)) {
    snprintf(outResponse, maxLen, "{\"error\":\"Backup export failed\"}");
    return 500;
  }

  snprintf(outResponse, maxLen, "{\"success\":true,\"payload\":\"");
  size_t prefixLen = strlen(outResponse);
  toHex(payload, outLen, outResponse + prefixLen, maxLen - prefixLen - 3);
  size_t end = strlen(outResponse);
  if (end + 3 < maxLen) {
    outResponse[end++] = '"';
    outResponse[end++] = '}';
    outResponse[end] = '\0';
  }
  CryptoService::secureZero(payload, sizeof(payload));
  return 200;
}

int WebPortalService::handleRestore(AppContext& ctx, const char* body, char* outResponse, size_t maxLen) {
  char pin[16]{}, pass[48]{};
  extractJsonString(body, "pin", pin, sizeof(pin));
  extractJsonString(body, "passphrase", pass, sizeof(pass));

  if (strcmp(pin, webPin_) != 0) {
    snprintf(outResponse, maxLen, "{\"error\":\"Invalid Web PIN\"}");
    return 401;
  }
  if (!ctx.security.hasActiveKey()) {
    snprintf(outResponse, maxLen, "{\"error\":\"Device locked. Unlock on device first.\"}");
    return 403;
  }
  if (!pass[0]) {
    snprintf(outResponse, maxLen, "{\"error\":\"Passphrase is required\"}");
    return 400;
  }

  const char* pKey = "\"payload\"";
  const char* pPos = strstr(body, pKey);
  if (!pPos) {
    snprintf(outResponse, maxLen, "{\"error\":\"Missing payload field\"}");
    return 400;
  }
  pPos += strlen(pKey);
  while (*pPos && (*pPos == ' ' || *pPos == '\t' || *pPos == ':')) ++pPos;
  if (*pPos != '"') {
    snprintf(outResponse, maxLen, "{\"error\":\"Invalid payload format\"}");
    return 400;
  }
  ++pPos;

  uint8_t binPayload[2048]{};
  size_t binLen = fromHex(pPos, binPayload, sizeof(binPayload));
  if (binLen < 48) {
    snprintf(outResponse, maxLen, "{\"error\":\"Payload corrupted or too short\"}");
    return 400;
  }

  if (!ctx.vault.importBackup(pass, binPayload, binLen, ctx.security.getActiveVaultKey())) {
    CryptoService::secureZero(binPayload, sizeof(binPayload));
    snprintf(outResponse, maxLen, "{\"error\":\"Restore failed: wrong passphrase or corrupted data\"}");
    return 400;
  }

  CryptoService::secureZero(binPayload, sizeof(binPayload));
  snprintf(outResponse, maxLen, "{\"success\":true,\"count\":%u}", static_cast<unsigned int>(ctx.vault.count()));
  return 200;
}

int WebPortalService::handleDeviceDisplay(AppContext& ctx, const char* body, char* outResponse, size_t maxLen) {
  char pin[16]{};
  extractJsonString(body, "pin", pin, sizeof(pin));
  if (strcmp(pin, webPin_) != 0) {
    snprintf(outResponse, maxLen, "{\"error\":\"Invalid Web PIN\"}");
    return 401;
  }
  int brightness = extractJsonInt(body, "brightness", -1);
  int timeout = extractJsonInt(body, "timeout", -1);
  int theme = extractJsonInt(body, "theme", -1);

  if (brightness >= 0 && brightness <= 100) {
    ctx.settings.setBrightness(static_cast<uint8_t>(brightness));
  }
  if (timeout >= 0) {
    ctx.settings.setScreenTimeoutSec(static_cast<uint16_t>(timeout));
  }
  if (theme >= 0 && theme <= 3) {
    ctx.settings.setTheme(static_cast<uint8_t>(theme));
  }
  snprintf(outResponse, maxLen, "{\"success\":true}");
  return 200;
}

int WebPortalService::handleDeviceLock(AppContext& ctx, const char* body, char* outResponse, size_t maxLen) {
  char pin[16]{};
  extractJsonString(body, "pin", pin, sizeof(pin));
  if (strcmp(pin, webPin_) != 0) {
    snprintf(outResponse, maxLen, "{\"error\":\"Invalid Web PIN\"}");
    return 401;
  }
  ctx.security.lock();
  ctx.totp.lock();
  ctx.vault.lock();
  snprintf(outResponse, maxLen, "{\"success\":true,\"locked\":true}");
  return 200;
}

int WebPortalService::simulateRequest(AppContext& ctx, const char* method, const char* path, const char* body, char* outResponse, size_t maxLen) {
  if (state_ != WebPortalState::Active) {
    snprintf(outResponse, maxLen, "{\"error\":\"Portal not active\"}");
    return 503;
  }
  if (strcmp(path, "/api/status") == 0) {
    return handleStatus(ctx, outResponse, maxLen);
  } else if (strcmp(path, "/api/totp/list") == 0 && strcmp(method, "GET") == 0) {
    return handleTotpList(ctx, outResponse, maxLen);
  } else if (strcmp(path, "/api/totp/add") == 0 && strcmp(method, "POST") == 0) {
    return handleAddTotp(ctx, body, outResponse, maxLen);
  } else if (strcmp(path, "/api/totp/remove") == 0 && strcmp(method, "POST") == 0) {
    return handleRemoveTotp(ctx, body, outResponse, maxLen);
  } else if (strcmp(path, "/api/totp/next_hotp") == 0 && strcmp(method, "POST") == 0) {
    return handleNextHotp(ctx, body, outResponse, maxLen);
  } else if (strcmp(path, "/api/vault/list") == 0 && strcmp(method, "GET") == 0) {
    return handleVaultList(ctx, outResponse, maxLen);
  } else if (strcmp(path, "/api/vault/add") == 0 && strcmp(method, "POST") == 0) {
    return handleAddVault(ctx, body, outResponse, maxLen);
  } else if (strcmp(path, "/api/vault/remove") == 0 && strcmp(method, "POST") == 0) {
    return handleRemoveVault(ctx, body, outResponse, maxLen);
  } else if (strcmp(path, "/api/vault/backup") == 0 && strcmp(method, "POST") == 0) {
    return handleBackup(ctx, body, outResponse, maxLen);
  } else if (strcmp(path, "/api/vault/restore") == 0 && strcmp(method, "POST") == 0) {
    return handleRestore(ctx, body, outResponse, maxLen);
  } else if (strcmp(path, "/api/device/display") == 0 && strcmp(method, "POST") == 0) {
    return handleDeviceDisplay(ctx, body, outResponse, maxLen);
  } else if (strcmp(path, "/api/device/lock") == 0 && strcmp(method, "POST") == 0) {
    return handleDeviceLock(ctx, body, outResponse, maxLen);
  } else if (strcmp(path, "/") == 0) {
    snprintf(outResponse, maxLen, "HTML_PORTAL");
    return 200;
  }
  snprintf(outResponse, maxLen, "{\"error\":\"Not found\"}");
  return 404;
}
