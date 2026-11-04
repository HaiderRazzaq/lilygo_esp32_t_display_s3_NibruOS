#include "services/AccountImportService.h"
#include "services/CryptoService.h"
#include "services/SecurityService.h"
#include "services/TotpService.h"
#include "services/SettingsService.h"
#include "services/TimeService.h"
#include "services/VaultService.h"
#include "services/WebPortalService.h"
#include "services/BleHidService.h"
#include "services/WifiDefenseService.h"
#include "services/UsbHidService.h"
#include "services/WiFiService.h"
#include "hardware/DisplayManager.h"
#include "core/ResourceManager.h"
#include "tests/support/AuthenticatorFlow.h"
#include <Preferences.h>
#include <WiFi.h>
#include <cassert>
#include <cstring>
#include <string>
#include <cstdio>

#include "tests/support/AccountImportFixture.h"
using namespace ImportFixture;

void testAccountImport() {
#if defined(_WIN32)
  // Deterministic PUBLIC fixture only: CNG output cross-checked by WebCrypto.
  const std::string vector = frameForCode("ABCDEFGHIJKLMNOP", "Fixture\tGEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQ\t6\t30");
  FILE* fixture = std::fopen(".testbuild/import-vector.txt", "wb");
  assert(fixture); std::fwrite(vector.data(), 1, vector.size(), fixture); std::fclose(fixture);
#endif
  AccountImportTransport transport(openImport, closeImport, readImport);
  AccountImportService service(&transport);
  assert(service.state() == ImportState::Idle && !service.candidate());
  assert(service.start(ImportMode::Usb, 0));
  assert(strlen(service.pairingCode()) == 16);
  input = frame(service, "Fixture\tGEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQ\t6\t30");
  const std::string replay = input;
  pump(service);
  assert(service.state() == ImportState::Review && !opened);
  assert(service.candidate() && strcmp(service.candidate()->name, "Fixture") == 0);
  assert(service.candidate()->digits == 6 && service.candidate()->period == 30);
  assert(service.pairingCode()[0] == 0); // Receive consumes and wipes session key/code.
  input = replay;
  pump(service); // No second candidate or automatic persistence.
  assert(strcmp(service.candidate()->name, "Fixture") == 0);
  service.stop();
  assert(!service.candidate() && service.state() == ImportState::Idle);
  input.clear();
  assert(service.start(ImportMode::Wifi, 0));
  input = replay; // Key changed; previous envelope cannot be replayed.
  pump(service);
  assert(service.state() == ImportState::Waiting && !service.candidate());
  input = frame(service, "Fixture\tMY\t6\t30");
  input[input.size() - 3] = input[input.size() - 3] == 'a' ? 'b' : 'a';
  pump(service);
  assert(!service.candidate());
  input = std::string(800, 'A') + "\n";
  for (unsigned i = 0; i < 20; ++i) service.tick(200 + i);
  assert(!service.candidate());
  service.stop(); input.clear();
  assert(service.start(ImportMode::Wifi, 0xfffffff0U));
  service.tick(120000U - 16U);
  assert(service.state() == ImportState::Expired && !service.candidate());
  assert(service.pairingCode()[0] == 0 && !opened);
  assert(service.start(ImportMode::Usb, 0));
  input = frame(service, "Fixture\tMY\t7\t30");
  pump(service);
  assert(!service.candidate()); // Unsupported URI settings never silently downgrade.
  service.stop(); input.clear();
  Preferences::resetMock();
  SecurityService security; security.setCredentialChangeGuard(TotpService::canChangeCredentials); security.begin();
  assert(security.setPin("739126") && security.setPinEnabled(true));
  assert(security.verifyPin("739126")); security.unlock();
  DisplayManager display; display.begin();
  ResourceManager resources;
  WiFiService wifi;
  SettingsService settings;
  TimeService time;
  TotpService totp; totp.begin();
  UsbHidService usb;
  VaultService vault;
  WebPortalService webPortal;
  BleHidService bleHid;
  WifiDefenseService wifiDefense;
  AppContext ctx{display, resources, wifi, security, settings, time, totp, usb, service, vault, webPortal, bleHid, wifiDefense};
  TotpApp app;
  app.onEnter(ctx);
  authTap(app, ctx, 110, 158); // ADD > methods
  authTap(app, ctx, 160, 125); // Offline Wi-Fi import rejected.
  assert(service.state() == ImportState::Idle && resources.wifiOwner() == ResourceOwner::None);
  assert(wifi.connect("PublicFixture", "", millis()));
  WiFi.statusValue = WL_CONNECTED;
  wifi.tick(millis() + 50);
  assert(wifi.isConnected());
  assert(resources.acquireWiFi(ResourceOwner::WiFiScanner));
  authTap(app, ctx, 160, 125); // Another owner must not be displaced.
  assert(service.state() == ImportState::Idle && resources.wifiOwner() == ResourceOwner::WiFiScanner);
  resources.releaseWiFi(ResourceOwner::WiFiScanner);
  authTap(app, ctx, 160, 125);
  assert(service.state() == ImportState::Waiting && resources.wifiOwner() == ResourceOwner::AuthenticatorImport);
  assert(app.onBack(ctx));
  assert(service.state() == ImportState::Idle && resources.wifiOwner() == ResourceOwner::None);
  authTap(app, ctx, 110, 158); authTap(app, ctx, 160, 125);
  wifi.disconnect();
  app.onTick(ctx, millis());
  assert(service.state() == ImportState::Idle && resources.wifiOwner() == ResourceOwner::None);
  authTap(app, ctx, 160, 88); // USB import
  assert(service.state() == ImportState::Waiting);
  authTap(app, ctx, 160, 97); // Tap pairing code to send
  assert(service.state() == ImportState::Waiting);
  app.onPrimaryAction(ctx); // Primary button to send
  assert(service.state() == ImportState::Waiting);
  input = frame(service, "Imported\tGEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQ\t8\t60");
  for (unsigned i = 0; i < 6; ++i) app.onTick(ctx, millis() + i);
  assert(service.state() == ImportState::Review && totp.accountCount() == 0);
  authTap(app, ctx, 240, 125); // Early confirmation cannot save.
  assert(totp.accountCount() == 0);
  fakeMs += 400;
  Preferences::failWrite = true;
  authTap(app, ctx, 240, 125);
  assert(totp.accountCount() == 0 && service.candidate());
  Preferences::failWrite = false;
  authTap(app, ctx, 240, 125);
  assert(totp.accountCount() == 1 && !service.candidate());
  assert(totp.getAccount(0)->digits == 8 && totp.getAccount(0)->period == 60);
  for (unsigned i = 0; i < 20; ++i) {
    authTap(app, ctx, 110, 158); authTap(app, ctx, 160, 88);
    assert(service.state() == ImportState::Waiting);
    assert(app.onBack(ctx));
    assert(service.state() == ImportState::Idle && service.pairingCode()[0] == 0);
    assert(resources.wifiOwner() == ResourceOwner::None);
  }
  authTap(app, ctx, 110, 158); authTap(app, ctx, 160, 88);
  input = frame(service, "Expired\tMY\t6\t30");
  for (unsigned i = 0; i < 8; ++i) app.onTick(ctx, millis() + i);
  assert(service.state() == ImportState::Review);
  fakeMs += 120001;
  authTap(app, ctx, 240, 125); // Real loop dispatches touch before onTick.
  assert(totp.accountCount() == 1); // Expired candidate must never commit.
  assert(app.onBack(ctx));
  authTap(app, ctx, 110, 158); authTap(app, ctx, 160, 88);
  display.sleep(); app.onTick(ctx, millis());
  assert(service.state() == ImportState::Idle && totp.accountCount() == 0);
  display.wake(); app.onExit(ctx);
  Preferences::resetMock();
  std::puts("PASS: bounded paired account import");
}
