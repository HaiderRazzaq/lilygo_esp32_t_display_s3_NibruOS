#include "services/AccountImportService.h"
#include "services/TotpService.h"
#include "services/SecurityService.h"
#include "services/CryptoService.h"
#include "services/UsbHidService.h"
#include "services/TimeService.h"
#include "services/VaultService.h"
#include "services/WebPortalService.h"
#include "services/SettingsService.h"
#include "services/WiFiService.h"
#include "hardware/DisplayManager.h"
#include "core/ResourceManager.h"
#include "tests/support/AuthenticatorFlow.h"
#include "ui/Base32Keyboard.h"
#include "ui/TextKeyboard.h"
#include <Preferences.h>
#include <cassert>
#include <cstdio>
#include <cstring>

namespace {
constexpr const char* kTestSecret = "GEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQ"; // RFC 6238 public fixture
time_t testEpoch = 0;
time_t testClock(time_t*) { return testEpoch; }
bool connected = true, mounted = true, acceptReport = true;
unsigned reportCount = 0;
bool usbReady() { return connected; }
bool usbConnected() { return mounted; }
bool usbReport(uint8_t, uint8_t) { ++reportCount; return acceptReport; }
}

void testAuthenticator() {
  assert(Base32Keyboard::keyAt(18, 60) == '1');
  assert(Base32Keyboard::keyAt(18, 80) == 'Q');
  assert(Base32Keyboard::keyAt(25, 100) == 'A');
  assert(Base32Keyboard::keyAt(60, 120) == 'Z');
  assert(TextKeyboard::hit(18, 80, false, false).character == 'q');
  assert(TextKeyboard::hit(18, 80, false, true).character == '@');
  assert(TextKeyboard::hit(20, 120).action == TextKeyboard::Action::Shift);
  assert(TextKeyboard::hit(280, 120).action == TextKeyboard::Action::Delete);
  assert(TextKeyboard::hit(-1, 80).action == TextKeyboard::Action::None);
  TotpService totpSvc;
  totpSvc.begin();
  // Boot must not provision public demo secrets as if they were user accounts.
  assert(totpSvc.accountCount() == 0);
  Preferences::resetMock();
  uint8_t key[32]{}; key[0] = 73;
  assert(totpSvc.loadVault(key)); // An absent vault is writable empty, not an error.
  assert(totpSvc.addAccount(key, "Test only", "fixture", kTestSecret));
  assert(totpSvc.accountCount() == 1);
  TotpService reopened;
  reopened.begin();
  assert(reopened.loadVault(key));
  assert(reopened.accountCount() == 1);
  assert(std::strcmp(reopened.getAccount(0)->name, "Test only") == 0);
  assert(reopened.getAccount(0)->secretLen == 20);
  // Previous firmware used a scalar U8 count, not a blob. Match native NVS types.
  TotpAccount old = *reopened.getAccount(0);
  uint8_t oldIv[12]{}, oldTag[16]{}, oldCipher[sizeof(old)]{};
  assert(CryptoService::aesGcmEncrypt(key, 256, oldIv, 12, nullptr, 0,
      reinterpret_cast<const uint8_t*>(&old), sizeof(old), oldCipher, oldTag, 16));
  Preferences legacy;
  assert(legacy.begin("nibru_totp", false));
  assert(legacy.clear());
  legacy.putUChar("cnt", 1);
  assert(legacy.getBytesLength("cnt") == 0); // nvs_get_blob on NVS U8 fails.
  legacy.putBytes("iv", oldIv, 12); legacy.putBytes("tag", oldTag, 16);
  legacy.putBytes("data", oldCipher, sizeof(oldCipher)); legacy.end();
  assert(reopened.loadVault(key) && reopened.accountCount() == 1);
  assert(reopened.saveVault(key)); // Upgrade only after authenticated read.

#if defined(_WIN32)
  // Independent RFC 6238 Appendix B, SHA-1. Windows uses real CNG primitives.
  const uint32_t epochs[] = {59, 1111111109, 1111111111, 1234567890, 2000000000};
  const char* expected[] = {"94287082", "07081804", "14050471", "89005924", "69279037"};
  for (unsigned i = 0; i < 5; ++i) {
    char code[10]{}; uint8_t remaining = 0;
    assert(TotpService::computeTotp(reopened.getAccount(0)->secret, 20, epochs[i], 30, 8, code, sizeof(code), remaining));
    assert(std::strcmp(code, expected[i]) == 0);
  }
#endif
  uint8_t decoded[32]{};
  assert(TotpService::decodeBase32(kTestSecret, decoded, sizeof(decoded)) == 20);
  assert(TotpService::decodeBase32("A", decoded, sizeof(decoded)) == -1);
  assert(TotpService::decodeBase32("MZ", decoded, sizeof(decoded)) == -1); // Non-zero trailing bits.
  assert(TotpService::decodeBase32("MY======X", decoded, sizeof(decoded)) == -1);
  char oversized[80]; std::memset(oversized, 'A', 79); oversized[79] = 0;
  assert(!reopened.addAccount(key, "too long", "", oversized));
  assert(!reopened.addAccount(key, "", "", kTestSecret));
  assert(reopened.accountCount() == 1);

  Preferences::failWrite = true;
  assert(!reopened.addAccount(key, "not committed", "", kTestSecret));
  assert(reopened.accountCount() == 1);
  assert(!reopened.removeAccount(key, 0));
  assert(reopened.accountCount() == 1);
  Preferences::failWrite = false;
  TotpService afterFailure; afterFailure.begin();
  assert(afterFailure.loadVault(key) && afterFailure.accountCount() == 1);

  auto saved = Preferences::store["nibru_totp"]["vault"];
  auto corrupted = saved; corrupted.back() ^= 1;
  Preferences::store["nibru_totp"]["vault"] = corrupted;
  assert(!afterFailure.loadVault(key));
  assert(afterFailure.accountCount() == 0);
  assert(!afterFailure.addAccount(key, "no overwrite", "", kTestSecret));
  assert(Preferences::store["nibru_totp"]["vault"] == corrupted);
  Preferences::store["nibru_totp"]["vault"] = saved;
  key[0] ^= 1;
  assert(!afterFailure.loadVault(key));
  key[0] ^= 1;
  assert(afterFailure.loadVault(key));
  afterFailure.lock();
  assert(afterFailure.accountCount() == 0 && afterFailure.getAccount(0) == nullptr);
  assert(!afterFailure.addAccount(key, "locked", "", kTestSecret));

  SecurityService security;
  security.setCredentialChangeGuard(TotpService::canChangeCredentials);
  // Existing default security metadata absent: don't invent a new PIN over a vault.
  Preferences::store.erase("nibru_totp");
  security.begin();
  assert(!security.setPinEnabled(true)); // A fresh device has no shared default PIN.
  assert(security.setPin("739126"));
  assert(security.setPinEnabled(true));
  assert(security.verifyPin("739126")); security.unlock();
  assert(afterFailure.loadVault(security.getActiveVaultKey()));
  assert(afterFailure.addAccount(security.getActiveVaultKey(), "protected", "", kTestSecret));
  assert(!security.setPin("826139"));
  assert(!security.setPinEnabled(false));
  assert(security.isPinEnabled());
  assert(security.verifyPin("739126"));
  assert(afterFailure.removeAccount(security.getActiveVaultKey(), 0));
  assert(security.setPin("826139"));
  Preferences::failWrite = true;
  assert(!security.setPin("111122"));
  Preferences::failWrite = false;
  assert(security.verifyPin("826139"));
  security.lock();
  assert(!security.hasActiveKey());
  Preferences::resetMock();

  TimeService clock(testClock);
  clock.begin();
  assert(clock.getEpoch() == 0 && !clock.isSynced());
  testEpoch = 1800000000;
  clock.tick(1000, false);
  assert(clock.getEpoch() == 1800000000 && clock.isSynced());
  testEpoch = 0;
  clock.tick(2000, false);
  assert(clock.getEpoch() == 0 && !clock.isSynced());

  // Test USB Serial time sync and timezone formatting
  TimeService usbClock;
  assert(usbClock.processSerialCommand("TIME:1730648400"));
  assert(usbClock.isSynced());
  assert(usbClock.source() == TimeSource::UsbSerial);
  assert(std::strcmp(usbClock.sourceName(), "USB") == 0);
  assert(usbClock.lastSyncEpoch() == 1730648400);

  // Timezone formatting test (UTC+3)
  usbClock.setTimezoneOffset(3);
  char tBuf[16]{};
  char dBuf[16]{};
  usbClock.formatTime(tBuf, sizeof(tBuf));
  usbClock.formatDate(dBuf, sizeof(dBuf));
    assert(std::strcmp(tBuf, "18:40:00") == 0);
  assert(std::strcmp(dBuf, "2024-11-03") == 0);

  // Character-by-character serial input test
  const char* cmd = "TIME:1730650000\n";
  bool gotSync = false;
  for (const char* p = cmd; *p; ++p) {
    if (usbClock.processSerialInput(*p)) gotSync = true;
  }
  assert(gotSync);
  assert(usbClock.lastSyncEpoch() == 1730650000);

  UsbHidTransport transport{usbReady, usbReport, usbConnected};
  UsbHidService usb(&transport);
  usb.begin();
  connected = false;
  assert(!usb.typeCode("123456"));
  connected = true;
  assert(!usb.typeCode("12345x"));
  assert(usb.typeCode("123456"));
  assert(!usb.typeCode("654321")); // Never queue another code behind an active transfer.
  assert(usb.transferState() == UsbTransferState::Sending);
  for (unsigned i = 0; i < 12; ++i) usb.tick(millis() + i * 20);
  assert(usb.transferState() == UsbTransferState::Sending); // Final release still in flight.
  connected = false; usb.tick(millis() + 240);
  assert(usb.transferState() == UsbTransferState::Sending);
  connected = true; usb.tick(millis() + 260);
  assert(usb.transferState() == UsbTransferState::Sent && reportCount == 12);
  assert(usb.typeCode("123456"));
  acceptReport = false; usb.tick(millis());
  assert(usb.transferState() == UsbTransferState::Failed);
  acceptReport = true;
  assert(usb.typeCode("123456"));
  usb.cancel();
  assert(usb.transferState() == UsbTransferState::Idle);
  const unsigned beforeCancel = reportCount;
  usb.tick(millis() + 1000);
  assert(reportCount == beforeCancel);
  assert(usb.typeCode("123456"));
  mounted = false;
  usb.tick(millis() + 20);
  assert(usb.transferState() == UsbTransferState::Failed);
  const unsigned beforeReconnect = reportCount;
  mounted = true;
  usb.tick(millis() + 40);
  assert(reportCount == beforeReconnect); // Reconnecting never resumes queued digits.
  Preferences::resetMock();
  SecurityService session;
  session.setCredentialChangeGuard(TotpService::canChangeCredentials);
  session.begin();
  assert(session.setPin("739126"));
  assert(session.setPinEnabled(true));
  assert(session.verifyPin("739126")); session.unlock();
  DisplayManager display; display.begin();
  ResourceManager resources;
  WiFiService wifi;
  SettingsService settings;
  TotpService accounts; accounts.begin();
  AccountImportService accountImport;
  VaultService vault;
  WebPortalService webPortal;
  AppContext ctx{display, resources, wifi, session, settings, clock, accounts, usb, accountImport, vault, webPortal};
  TotpApp app;
  testAuthenticatorEntryFlow(ctx, app);
  app.onEnter(ctx);
  testEpoch = 0;
  const unsigned noTimeReports = reportCount;
  authTap(app, ctx, 160, 115);
  usb.tick(millis() + 1000);
  assert(reportCount == noTimeReports); // No code can be sent with invalid time.
  app.render(ctx);
  auto textShown = [&](const char* text) {
    for (const auto& label : display.gfx().printed) if (label == text) return true;
    return false;
  };
  assert(textShown("------"));
  display.gfx().printed.clear();
  testEpoch = 1800000000;
  connected = false;
  authTap(app, ctx, 160, 115); app.render(ctx);
  assert(textShown("USB unavailable") && !textShown("HID reports sent"));
  display.gfx().printed.clear();
  connected = true; acceptReport = false;
  authTap(app, ctx, 160, 115);
  usb.tick(millis()); app.onTick(ctx, millis()); app.render(ctx);
  assert(textShown("USB send failed") && !textShown("HID reports sent"));
  display.gfx().printed.clear();
  acceptReport = true;
  authTap(app, ctx, 160, 115);
  for (unsigned i = 0; i < 13; ++i) usb.tick(millis() + i * 20);
  app.onTick(ctx, millis()); app.render(ctx);
  assert(textShown("HID reports sent"));
  // A clock loss after queuing also cancels the transfer, before another digit.
  authTap(app, ctx, 160, 115);
  testEpoch = 0;
  app.onTick(ctx, millis());
  assert(usb.transferState() == UsbTransferState::Idle);
  const unsigned stopped = reportCount;
  usb.tick(millis() + 1000);
  assert(reportCount == stopped);
  // Failed save retains entered secret for retry; it never appends a ghost record.
  authAdd(app, ctx); authType(app, ctx, "SECOND");
  authTap(app, ctx, 270, 158); authType(app, ctx, kTestSecret);
  Preferences::failWrite = true;
  authTap(app, ctx, 270, 158);
  assert(accounts.accountCount() == 1);
  Preferences::failWrite = false;
  authTap(app, ctx, 270, 158);
  assert(accounts.accountCount() == 2);
  authAdd(app, ctx); authType(app, ctx, "ABANDON");
  authTap(app, ctx, 270, 158); authType(app, ctx, "GEZDG");
  display.sleep(); app.onTick(ctx, millis());
  assert(accounts.accountCount() == 0);
  display.wake();
  app.onTick(ctx, millis());
  assert(accounts.accountCount() == 2 && !app.onBack(ctx)); // Editor was abandoned on sleep.
  authTap(app, ctx, 185, 158); // Confirm deletion, never act on an old/held contact.
  const uint32_t opened = millis();
  fakeMs += 400;
  app.onTouch(ctx, TouchEvent{210, 110, 0, 0, TouchGesture::Tap, 123, opened + 10});
  assert(accounts.accountCount() == 2);
  authTap(app, ctx, 210, 110);
  assert(accounts.accountCount() == 1);
  app.onExit(ctx);
  Preferences::resetMock();
  std::puts("PASS: authenticator lifecycle");
}
