#include "apps/TotpApp.h"
#include "core/AppContext.h"
#include "hardware/DisplayManager.h"
#include "services/TimeService.h"
#include "services/TotpService.h"
#include "services/UsbHidService.h"
#include "services/SecurityService.h"
#include "services/CryptoService.h"
#include "ui/TextKeyboard.h"
#include "services/AccountImportService.h"
#include "services/WiFiService.h"
#include "core/ResourceManager.h"
#include <Arduino.h>
#include <cstring>
#include <cstdio>

void TotpApp::clearEditor() {
  CryptoService::secureZero(name_, sizeof(name_));
  CryptoService::secureZero(secret_, sizeof(secret_));
}

void TotpApp::endImport(AppContext& ctx) {
  ctx.accountImport.stop();
  ctx.resources.releaseWiFi(ResourceOwner::AuthenticatorImport);
  importSent_ = false;
}
void TotpApp::clearSession(AppContext& ctx) {
  endImport(ctx);
  clearEditor();
  ctx.totp.lock();
  ctx.usbHid.cancel();
  attemptedLoad_ = sending_ = false;
  status_ = "";
  capitals_ = true; symbols_ = false; keyboardDirty_ = true;
  feedbackUntilMs_ = 0;
  view_ = View::Card;
  fullRedraw_ = true;
  cardDirty_ = true;
  invalidate();
}

void TotpApp::ensureSession(AppContext& ctx) {
  if (ctx.display.isSleeping() || !ctx.security.isPinEnabled() || !ctx.security.hasActiveKey()) return;
  if (!attemptedLoad_) {
    attemptedLoad_ = true;
    if (!ctx.totp.loadVault(ctx.security.getActiveVaultKey())) status_ = "Vault unreadable";
    fullRedraw_ = true;
  cardDirty_ = true;
    invalidate();
  }
}

void TotpApp::onEnter(AppContext& ctx) {
  clearSession(ctx);
  accountIndex_ = 0;
  lastSecond_ = 0;
  if (ctx.security.isPinEnabled() && !ctx.security.hasActiveKey()) ctx.security.lock();
  ensureSession(ctx);
}
void TotpApp::onExit(AppContext& ctx) { clearSession(ctx); }

void TotpApp::onTick(AppContext& ctx, uint32_t nowMs) {
  if (!ctx.security.hasActiveKey() || ctx.display.isSleeping()) {
    if (attemptedLoad_ || view_ != View::Card) clearSession(ctx);
    return;
  }
  ensureSession(ctx);
  if (view_ == View::ImportUsb || view_ == View::ImportWifi || view_ == View::ImportReview) {
    if (view_ == View::ImportWifi && !ctx.wifi.isConnected()) {
      endImport(ctx); view_ = View::Methods; status_ = "Wi-Fi disconnected";
      fullRedraw_ = true;
  cardDirty_ = true; invalidate();
    }
    ctx.accountImport.tick(nowMs);
    const auto state = ctx.accountImport.state();
    if (state == ImportState::Review && view_ != View::ImportReview) {
      ctx.resources.releaseWiFi(ResourceOwner::AuthenticatorImport);
  importSent_ = false;
      view_ = View::ImportReview; confirmAfterMs_ = nowMs + 350;
      fullRedraw_ = true;
  cardDirty_ = true; status_ = ""; invalidate();
    } else if (state == ImportState::Expired) {
      endImport(ctx); view_ = View::Methods; status_ = "Import expired - retry";
      fullRedraw_ = true;
  cardDirty_ = true; invalidate();
    } else ctx.display.notifyActivity(nowMs); // Explicit session only, max120s.
    const uint32_t seconds = ctx.accountImport.remainingSeconds(nowMs);
    if (seconds != importRemaining_) { importRemaining_ = seconds; invalidate(); }
    if (view_ == View::ImportUsb && !importSent_ && ctx.accountImport.state() == ImportState::Waiting) {
      ctx.accountImport.broadcastPairingCode();
      importSent_ = true;
    }
  }
  if (sending_ && !ctx.time.getEpoch()) {
    ctx.usbHid.cancel(); sending_ = false;
    status_ = "Sync time via Wi-Fi"; invalidate();
  }
  if (sending_) {
    const UsbTransferState state = ctx.usbHid.transferState();
    if (state == UsbTransferState::Sent || state == UsbTransferState::Failed) {
      sending_ = false;
      status_ = state == UsbTransferState::Sent ? "HID reports sent" : "USB send failed";
      feedbackUntilMs_ = nowMs + 2000;
      invalidate();
    }
  } else if (feedbackUntilMs_ && static_cast<int32_t>(nowMs - feedbackUntilMs_) >= 0) {
    feedbackUntilMs_ = 0; status_ = ""; invalidate();
  }
  const uint32_t epoch = ctx.time.getEpoch();
  if (epoch != lastSecond_) { lastSecond_ = epoch; invalidate(); }
}

bool TotpApp::onBack(AppContext& ctx) {
  if (view_ == View::Card) return false;
  endImport(ctx);
  clearEditor();
  ctx.usbHid.cancel();
  view_ = View::Card; status_ = ""; fullRedraw_ = true; invalidate();
  return true;
}

void TotpApp::onPrimaryAction(AppContext& ctx) {
  if (view_ == View::ImportUsb && ctx.accountImport.state() == ImportState::Waiting) {
    ctx.accountImport.broadcastPairingCode();
    if (ctx.usbHid.isReady()) {
      ctx.usbHid.typeText(ctx.accountImport.pairingCode(), false);
    }
    status_ = "Code sent to PC!";
    fullRedraw_ = true;
  cardDirty_ = true;
    invalidate();
    return;
  }
  if (view_ == View::Card && ctx.totp.accountCount()) {
    ctx.usbHid.cancel(); sending_ = false; status_ = "";
    accountIndex_ = (accountIndex_ + 1) % ctx.totp.accountCount();
    cardDirty_ = true;
    invalidate();
  }
}

void TotpApp::onTouch(AppContext& ctx, const TouchEvent& ev) {
  if (ev.gesture != TouchGesture::Tap && ev.gesture != TouchGesture::SwipeLeft &&
      ev.gesture != TouchGesture::SwipeRight) return;
  if (!ctx.security.isPinEnabled() || !ctx.security.hasActiveKey() || !ctx.totp.isLoaded()) return;
  if (view_ == View::Methods) {
    if (ev.gesture != TouchGesture::Tap) return;
    if (ev.y >= 38 && ev.y <= 69) {
      clearEditor(); capitals_ = true; symbols_ = false;
      keyboardDirty_ = true; view_ = View::Name; status_ = "";
    } else if ((ev.y >= 75 && ev.y <= 106) || (ev.y >= 112 && ev.y <= 143)) {
      const bool wifi = ev.y >= 112;
      if (wifi && (!ctx.wifi.isConnected() || !ctx.resources.acquireWiFi(ResourceOwner::AuthenticatorImport))) {
        status_ = "Connect Wi-Fi first"; invalidate(); return;
      }
      if (!ctx.accountImport.start(wifi ? ImportMode::Wifi : ImportMode::Usb, millis())) {
        ctx.resources.releaseWiFi(ResourceOwner::AuthenticatorImport);
  importSent_ = false;
        status_ = "Import unavailable"; invalidate(); return;
      }
      view_ = wifi ? View::ImportWifi : View::ImportUsb;
      importRemaining_ = 120; status_ = ""; ctx.display.notifyActivity(millis());
    } else return;
    fullRedraw_ = true;
  cardDirty_ = true; invalidate(); return;
  }
  if (view_ == View::ImportUsb || view_ == View::ImportWifi) {
    if (ev.gesture == TouchGesture::Tap && view_ == View::ImportUsb && ctx.accountImport.state() == ImportState::Waiting) {
      ctx.accountImport.broadcastPairingCode();
      if (ctx.usbHid.isReady()) {
        ctx.usbHid.typeText(ctx.accountImport.pairingCode(), false);
      }
      status_ = "Code sent to PC!";
      fullRedraw_ = true;
  cardDirty_ = true;
      invalidate();
    }
    return;
  }
  if (view_ == View::ImportReview) {
    if (ev.gesture != TouchGesture::Tap || ev.y < 108 || ev.y > 140) return;
    if (ev.x < 155) { onBack(ctx); return; }
    const uint32_t start = ev.contactId ? ev.contactStartedMs : millis();
    if (static_cast<int32_t>(start - confirmAfterMs_) < 0) return;
    // Touch dispatch precedes onTick in the main loop. Check the absolute
    // session deadline here as well, before any persistent write.
    ctx.accountImport.tick(millis());
    if (ctx.accountImport.state() != ImportState::Review) {
      endImport(ctx); view_ = View::Methods; status_ = "Import expired - retry";
      fullRedraw_ = true;
  cardDirty_ = true; invalidate(); return;
    }
    const ImportedAccount* account = ctx.accountImport.candidate();
    if (account && ctx.totp.addAccount(ctx.security.getActiveVaultKey(), account->name, "", account->secret, account->digits, account->period)) {
      accountIndex_ = ctx.totp.accountCount() - 1;
      endImport(ctx); view_ = View::Card; status_ = "Saved"; fullRedraw_ = true;
    } else status_ = "Save failed - retry";
    invalidate(); return;
  }
  if (view_ == View::Name || view_ == View::Secret) {
    if (ev.gesture != TouchGesture::Tap) return;
    char* input = view_ == View::Name ? name_ : secret_;
    const size_t capacity = view_ == View::Name ? sizeof(name_) : sizeof(secret_);
    const size_t length = strlen(input);
    const TextKeyboard::Key pressed = TextKeyboard::hit(ev.x, ev.y, capitals_, symbols_);
    char key = pressed.character;
    if (view_ == View::Secret && key >= 'a' && key <= 'z') key -= 'a' - 'A';
    const bool valid = view_ == View::Name || (key >= 'A' && key <= 'Z') || (key >= '2' && key <= '7');
    if (!valid) key = 0;
    if (pressed.action == TextKeyboard::Action::Shift) { capitals_ = !capitals_; keyboardDirty_ = true; }
    else if (pressed.action == TextKeyboard::Action::Symbols) { symbols_ = !symbols_; keyboardDirty_ = true; }
    else if (pressed.action == TextKeyboard::Action::Delete) { if (length) input[length - 1] = 0; }
    else if (pressed.action == TextKeyboard::Action::Cancel) { onBack(ctx); return; }
    else if (key && length + 1 < capacity) { input[length] = key; input[length + 1] = 0; status_ = ""; }
    else if (pressed.action == TextKeyboard::Action::Space && view_ == View::Name) {
      if (length && length + 1 < capacity) { input[length] = ' '; input[length + 1] = 0; }
    } else if (pressed.action == TextKeyboard::Action::Accept) {
      if (view_ == View::Name) {
        if (!length) status_ = "Enter a name";
        else { view_ = View::Secret; status_ = ""; fullRedraw_ = true; }
      } else {
        uint8_t decoded[32]{};
        const int size = TotpService::decodeBase32(secret_, decoded, sizeof(decoded));
        CryptoService::secureZero(decoded, sizeof(decoded));
        if (size <= 0) status_ = "Invalid Base32 secret";
        else if (!ctx.totp.addAccount(ctx.security.getActiveVaultKey(), name_, "", secret_))
          status_ = "Save failed - retry";
        else {
          accountIndex_ = ctx.totp.accountCount() - 1;
          clearEditor(); view_ = View::Card; status_ = "Saved"; fullRedraw_ = true;
        }
      }
    }
    invalidate();
    return;
  }
  if (view_ == View::ConfirmDelete) {
    if (ev.gesture != TouchGesture::Tap || ev.y < 96 || ev.y > 130) return;
    if (ev.x < 155) { onBack(ctx); return; }
    const uint32_t start = ev.contactId ? ev.contactStartedMs : millis();
    if (static_cast<int32_t>(start - confirmAfterMs_) < 0) return;
    if (ctx.totp.removeAccount(ctx.security.getActiveVaultKey(), accountIndex_)) {
      accountIndex_ = 0; view_ = View::Card; status_ = "Deleted"; fullRedraw_ = true;
    } else status_ = "Delete failed - retry";
    invalidate();
    return;
  }
  if (sending_) return;
  const uint8_t count = ctx.totp.accountCount();
  if (ev.gesture == TouchGesture::SwipeLeft || (ev.gesture == TouchGesture::Tap && ev.x >= 266 && ev.y >= 28 && ev.y < 144)) {
    if (accountIndex_ + 1 < count) { ++accountIndex_; cardDirty_ = true; }
  } else if (ev.gesture == TouchGesture::SwipeRight || (ev.gesture == TouchGesture::Tap && ev.x <= 54 && ev.y >= 28 && ev.y < 144)) {
    if (accountIndex_) { --accountIndex_; cardDirty_ = true; }
  } else if (ev.gesture == TouchGesture::Tap && ev.y >= 146) {
    if (ev.x >= 80 && ev.x < 160 && count < kMaxTotpAccounts) {
      clearEditor(); view_ = View::Methods; status_ = ""; fullRedraw_ = true;
    } else if (ev.x >= 160 && ev.x < 240 && count) {
      view_ = View::ConfirmDelete; confirmAfterMs_ = millis() + 350;
      status_ = ""; fullRedraw_ = true;
    }
  } else if (ev.gesture == TouchGesture::Tap && count && ev.x >= 58 && ev.x <= 262 && ev.y >= 65 && ev.y <= 98 && !ctx.time.getEpoch()) {
    if (ctx.wifi.isConnected()) {
      ctx.time.requestNtpSync();
      status_ = "NTP sync requested...";
    } else {
      status_ = "Connect Wi-Fi or USB";
    }
    cardDirty_ = true;
  } else if (ev.gesture == TouchGesture::Tap && count && ev.x >= 58 && ev.x <= 262 && ev.y >= 102 && ev.y <= 134) {
    const uint32_t epoch = ctx.time.getEpoch();
    char code[10]{}; uint8_t remaining = 0;
    if (!epoch) status_ = "Sync: USB (TIME:x) or WiFi";
    else if (!ctx.usbHid.isReady()) status_ = "USB unavailable";
    else if (!ctx.totp.getAccountCode(accountIndex_, epoch, code, sizeof(code), remaining))
      status_ = "Code unavailable";
    else if (ctx.usbHid.typeCode(code)) { sending_ = true; status_ = "Sending..."; }
    else status_ = "USB send failed";
    CryptoService::secureZero(code, sizeof(code));
    cardDirty_ = true;
  }
  invalidate();
}

void TotpApp::renderEditor(AppContext& ctx) {
  auto& screen = ctx.display;
  auto& t = screen.gfx();
  if (fullRedraw_) {
    screen.clear();
    screen.header(view_ == View::Name ? "Account name" : "Base32 secret", true);
    keyboardDirty_ = true;
    fullRedraw_ = false;
  }
  if (keyboardDirty_) {
    TextKeyboard::draw(screen, capitals_, symbols_, view_ == View::Name ? "NEXT" : "SAVE", view_ == View::Secret);
    keyboardDirty_ = false;
  }
  t.fillRoundRect(3, 33, 314, 17, 3, Colors::Panel);
  t.drawRoundRect(3, 33, 314, 17, 3, Colors::Border);
  char text[54]{};
  if (view_ == View::Name) snprintf(text, sizeof(text), "%.23s", name_);
  else memset(text, '*', strlen(secret_) > 26 ? 26 : strlen(secret_));
  const char* placeholder = view_ == View::Name ? "Type account name..." : "Base32 key (A-Z, 2-7)";
  screen.label(8, 37, status_[0] ? status_ : text[0] ? text : placeholder,
      status_[0] ? Colors::Amber : text[0] ? Colors::White : Colors::Muted, 1, Colors::Panel);
  char count[12]{};
  snprintf(count, sizeof(count), "%u/%u", static_cast<unsigned>(view_ == View::Name ? strlen(name_) : strlen(secret_)),
      view_ == View::Name ? 23U : 52U);
  screen.label(275, 37, count, Colors::Muted, 1, Colors::Panel);
  CryptoService::secureZero(text, sizeof(text));

}

void TotpApp::renderImport(AppContext& ctx) {
  auto& screen = ctx.display;
  auto& t = screen.gfx();
  const bool initialDraw = fullRedraw_;
  if (fullRedraw_) {
    screen.clear();
    screen.header(view_ == View::Methods ? "Add account" : view_ == View::ImportReview ? "Review account" :
                  view_ == View::ImportWifi ? "Wi-Fi paste" : "USB paste", true);
    fullRedraw_ = false;
  }
  if (view_ == View::Methods) {
    TextKeyboard::button(screen, 12, 38, 296, 31, "TYPE ON DEVICE", Colors::White);
    TextKeyboard::button(screen, 12, 75, 296, 31, "PASTE FROM COMPUTER - USB", Colors::Cyan);
    TextKeyboard::button(screen, 12, 112, 296, 31, "PASTE FROM COMPUTER - WI-FI", Colors::Cyan);
    screen.footer(status_[0] ? status_ : "Paste a setup key or otpauth link");
  } else if (view_ == View::ImportReview) {
    const ImportedAccount* account = ctx.accountImport.candidate();
    if (!account) return;
    t.fillRect(0, 33, 320, 118, Colors::Bg);
    screen.label(16, 43, account->name, Colors::Cyan, 1);
    char details[48]{};
    snprintf(details, sizeof(details), "TOTP SHA1 | %u digits | %us", account->digits, account->period);
    screen.label(16, 65, details, Colors::White, 1);
    screen.label(16, 85, status_[0] ? status_ : "Secret hidden. Save this account?", Colors::Amber, 1);
    TextKeyboard::button(screen, 25, 108, 120, 32, "CANCEL", Colors::Amber);
    TextKeyboard::button(screen, 175, 108, 120, 32, "SAVE", Colors::Cyan);
    screen.footer("Nothing is saved until you confirm");
  } else {
    if (initialDraw) {
      t.fillRect(0, 33, 320, 118, Colors::Bg);
      char address[48]{};
      if (view_ == View::ImportWifi) {
        char ip[24]{}; ctx.wifi.getLocalIP(ip, sizeof(ip));
        snprintf(address, sizeof(address), "Device IP: %s", ip);
      } else snprintf(address, sizeof(address), "Connect the data cable to computer");
      screen.label(10, 43, address, Colors::White, 1);
      screen.label(10, 61, "Open Account Setup. Paste the key.", Colors::Muted, 1);
      screen.label(10, 80, "One-time pairing code:", Colors::Cyan, 1);
      screen.label(42, 97, ctx.accountImport.pairingCode(), Colors::White, 2);
      screen.footer(status_[0] ? status_ : (view_ == View::ImportUsb ? "Tap code or press BTN to send to PC" : "Encrypted transfer. Confirm on device."));
    }
    char remaining[40]{};
    snprintf(remaining, sizeof(remaining), "Expires in %lus | Back cancels", static_cast<unsigned long>(importRemaining_));
    screen.labelPadded(40, 128, remaining, 36, Colors::Amber, 1, Colors::Bg);
  }
}

void TotpApp::render(AppContext& ctx) {
  if (view_ == View::Methods || view_ == View::ImportUsb || view_ == View::ImportWifi || view_ == View::ImportReview) {
    renderImport(ctx); return;
  }
  if (view_ == View::Name || view_ == View::Secret) { renderEditor(ctx); return; }
  auto& screen = ctx.display;
  auto& t = screen.gfx();
  if (fullRedraw_) { screen.clear(); screen.header("2FA Authenticator", true); fullRedraw_ = false; }
  if (view_ == View::ConfirmDelete) {
    t.fillRect(0, 33, 320, 137, Colors::Bg);
    screen.label(40, 50, "Delete this account?", Colors::White, 2);
    screen.label(35, 76, status_[0] ? status_ : "Keep a recovery copy elsewhere.", Colors::Amber, 1);
    screen.label(45, 110, "CANCEL", Colors::Cyan, 2);
    screen.label(190, 110, "DELETE", Colors::Red, 2);
    return;
  }
  const uint8_t count = ctx.totp.accountCount();
  const bool unlocked = ctx.security.isPinEnabled() && ctx.security.hasActiveKey() && ctx.totp.isLoaded();
  const uint32_t epoch = ctx.time.getEpoch();

  if (cardDirty_) {
    cardDirty_ = false;
    t.fillRect(0, 33, 320, 112, Colors::Bg);
    if (!unlocked) {
      screen.label(30, 65, ctx.totp.hasStorageError() ? "Vault unreadable" : "Enable PIN in Settings", Colors::Amber, 2);
      screen.label(30, 95, ctx.totp.hasStorageError() ? "Back and retry. Data preserved." : "Unlock with your PIN to continue.", Colors::Muted, 1);
    } else if (!count) {
      screen.label(85, 65, "Vault empty", Colors::White, 2);
      screen.label(40, 98, "Tap ADD to enter an account.", Colors::Muted, 1);
    } else {
      if (accountIndex_ >= count) accountIndex_ = count - 1;
      const TotpAccount* account = ctx.totp.getAccount(accountIndex_);
      screen.label(60, 39, account->name, Colors::Cyan, strlen(account->name) > 16 ? 1 : 2);
      t.fillRoundRect(58, 102, 204, 32, 5, epoch && ctx.usbHid.isReady() ? Colors::Cyan : Colors::Panel);
      screen.label(76, 114, status_[0] ? status_ : "TYPE CODE (USB)", Colors::White, 1, Colors::Panel);
      screen.label(12, 75, "<", Colors::Cyan, 2);
      screen.label(290, 75, ">", Colors::Cyan, 2);
    }
    t.fillRect(0, 146, 320, 24, Colors::Panel);
    char counter[12]{}; snprintf(counter, sizeof(counter), "%u/%u", count ? accountIndex_ + 1 : 0, count);
    screen.label(12, 154, counter, Colors::Muted, 1, Colors::Panel);
    screen.label(100, 154, "ADD", unlocked && count < kMaxTotpAccounts ? Colors::Cyan : Colors::Muted, 1, Colors::Panel);
    screen.label(175, 154, "DELETE", unlocked && count ? Colors::Red : Colors::Muted, 1, Colors::Panel);
  }

  // Live dynamic update on every tick - ZERO fillRect, ZERO flicker!
  if (unlocked && count) {
    if (accountIndex_ >= count) accountIndex_ = count - 1;
    char code[10] = "------"; uint8_t remaining = 0;
    if (epoch) ctx.totp.getAccountCode(accountIndex_, epoch, code, sizeof(code), remaining);
    screen.label(105, 69, code, Colors::White, 2, Colors::Bg);
    CryptoService::secureZero(code, sizeof(code));
    char timer[32]{};
    if (epoch) {
      snprintf(timer, sizeof(timer), "%2u sec  [%s]", remaining, ctx.time.sourceName());
      screen.labelPadded(85, 90, timer, 20, ctx.time.source() == TimeSource::Ntp ? Colors::Green : Colors::Cyan, 1, Colors::Bg);
    } else {
      snprintf(timer, sizeof(timer), "Waiting for time (Sync)");
      screen.labelPadded(65, 90, timer, 26, Colors::Amber, 1, Colors::Bg);
    }
  }
}
