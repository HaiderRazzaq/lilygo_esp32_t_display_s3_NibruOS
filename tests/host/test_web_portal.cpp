#include "services/WebPortalService.h"
#include "services/VaultService.h"
#include "services/TotpService.h"
#include "services/SecurityService.h"
#include "services/SettingsService.h"
#include "services/TimeService.h"
#include "services/WiFiService.h"
#include "services/UsbHidService.h"
#include "services/AccountImportService.h"
#include "hardware/DisplayManager.h"
#include "core/ResourceManager.h"
#include "core/AppContext.h"
#include <Preferences.h>
#include <cassert>
#include <cstdio>
#include <cstring>

void runWebPortalHostTests() {
  std::printf("[TEST] Running Web Portal Service Host Tests...\n");
  Preferences::resetMock();

  DisplayManager display;
  ResourceManager resources;
  WiFiService wifi;
  SecurityService security;
  SettingsService settings;
  TimeService timeService;
  TotpService totp;
  UsbHidService usbHid;
  AccountImportService accountImport;
  VaultService vault;
  WebPortalService webPortal;

  AppContext ctx{display, resources, wifi, security, settings, timeService, totp, usbHid, accountImport, vault, webPortal};

  // 1. Initial State
  assert(!webPortal.isActive());
  assert(webPortal.state() == WebPortalState::Idle);

  // 2. Start Portal
  const uint32_t nowMs = 5000;
  assert(webPortal.start(nowMs, ctx));
  assert(webPortal.isActive());
  assert(webPortal.state() == WebPortalState::Active);
  assert(resources.ownsWiFi(ResourceOwner::WebPortal));

  // Check generated credentials
  assert(std::strncmp(webPortal.ssid(), "NibruOS-", 8) == 0);
  assert(std::strlen(webPortal.password()) == 8);
  assert(std::strlen(webPortal.webPin()) == 4);
  assert(webPortal.remainingSeconds(nowMs) == 180);

  // 3. Status Endpoint
  char resp[256]{};
  int code = webPortal.simulateRequest(ctx, "GET", "/api/status", "", resp, sizeof(resp));
  assert(code == 200);
  assert(std::strstr(resp, "\"ok\":true") != nullptr);

  // 4. Test Vault Add with Invalid PIN
  code = webPortal.simulateRequest(ctx, "POST", "/api/vault/add",
    "{\"pin\":\"9999\",\"title\":\"Google\",\"username\":\"user@gmail.com\",\"password\":\"p@ss123\"}",
    resp, sizeof(resp));
  assert(code == 401);
  assert(std::strstr(resp, "Invalid Web PIN") != nullptr);
  assert(vault.count() == 0);

  // 5. Test Vault Add while device locked
  char validPayload[256]{};
  std::snprintf(validPayload, sizeof(validPayload),
    "{\"pin\":\"%s\",\"title\":\"Google\",\"username\":\"user@gmail.com\",\"password\":\"secretPass123\",\"notes\":\"Work\"}",
    webPortal.webPin());

  // Security is locked by default
  code = webPortal.simulateRequest(ctx, "POST", "/api/vault/add", validPayload, resp, sizeof(resp));
  assert(code == 403);
  assert(std::strstr(resp, "Device locked") != nullptr);

  // Unlock device with PIN
  security.begin();
  assert(security.setPin("654321"));
  assert(security.setPinEnabled(true));
  assert(security.verifyPin("654321"));
  security.unlock();
  assert(security.hasActiveKey());

  vault.begin();
  assert(vault.loadVault(security.getActiveVaultKey()));

  // 6. Test Vault Add with valid Web PIN and active device key
  code = webPortal.simulateRequest(ctx, "POST", "/api/vault/add", validPayload, resp, sizeof(resp));
  assert(code == 200);
  assert(std::strstr(resp, "\"success\":true") != nullptr);
  assert(vault.count() == 1);

  const VaultEntry* entry = vault.getEntry(0);
  assert(entry != nullptr);
  assert(std::strcmp(entry->title, "Google") == 0);
  assert(std::strcmp(entry->username, "user@gmail.com") == 0);
  assert(std::strcmp(entry->password, "secretPass123") == 0);
  assert(std::strcmp(entry->notes, "Work") == 0);

  // 7. Test TOTP Add with invalid PIN
  totp.begin();
  assert(totp.loadVault(security.getActiveVaultKey()));
  code = webPortal.simulateRequest(ctx, "POST", "/api/totp/add",
    "{\"pin\":\"0000\",\"name\":\"GitHub\",\"secret\":\"JBSWY3DPEHPK3PXP\"}",
    resp, sizeof(resp));
  assert(code == 401);
  assert(totp.accountCount() == 0);

  // 8. Test TOTP Add with valid Web PIN
  char totpPayload[256]{};
  std::snprintf(totpPayload, sizeof(totpPayload),
    "{\"pin\":\"%s\",\"name\":\"GitHub\",\"secret\":\"JBSWY3DPEHPK3PXP\"}",
    webPortal.webPin());

  code = webPortal.simulateRequest(ctx, "POST", "/api/totp/add", totpPayload, resp, sizeof(resp));
  assert(code == 200);
  assert(std::strstr(resp, "\"success\":true") != nullptr);
  assert(totp.accountCount() == 1);

  // 9. Stop Portal
  webPortal.stop(ctx);
  assert(!webPortal.isActive());
  assert(!resources.ownsWiFi(ResourceOwner::WebPortal));

  code = webPortal.simulateRequest(ctx, "GET", "/api/status", "", resp, sizeof(resp));
  assert(code == 503);

  std::printf("  -> PASS: Web Portal Service Host Tests verified successfully.\n");
}
