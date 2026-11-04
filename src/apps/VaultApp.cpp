#include "apps/VaultApp.h"
#include "core/AppContext.h"
#include "hardware/DisplayManager.h"
#include "services/VaultService.h"
#include "services/UsbHidService.h"
#include "services/BleHidService.h"
#include "services/SecurityService.h"
#include "services/CryptoService.h"
#include "ui/TextKeyboard.h"
#include <Arduino.h>
#include <cstring>
#include <cstdio>

static const char* kCategories[] = {"ALL", "WORK", "PERS", "FIN"};

static bool entryMatchesCategory(const VaultEntry* entry, uint8_t cat) {
  if (!entry || cat == 0) return true;
  if (!entry->notes[0]) return false;
  const char* tag = kCategories[cat];
  return (strstr(entry->notes, tag) != nullptr);
}

void VaultApp::clearEditor() {
  CryptoService::secureZero(editTitle_, sizeof(editTitle_));
  CryptoService::secureZero(editUser_, sizeof(editUser_));
  CryptoService::secureZero(editPass_, sizeof(editPass_));
  CryptoService::secureZero(editNotes_, sizeof(editNotes_));
}

void VaultApp::clearSession(AppContext& ctx) {
  clearEditor();
  ctx.vault.lock();
  ctx.usbHid.cancel();
  attemptedLoad_ = false;
  sending_ = false;
  armedAction_ = ArmedAction::None;
  showPassword_ = false;
  status_ = "";
  feedbackUntilMs_ = 0;
  view_ = View::Card;
  fullRedraw_ = true;
  cardDirty_ = true;
  keyboardDirty_ = true;
  invalidate();
}

void VaultApp::onEnter(AppContext& ctx) {
  clearSession(ctx);
  entryIndex_ = 0;
  if (ctx.security.isPinEnabled() && !ctx.security.hasActiveKey()) ctx.security.lock();
  ensureSession(ctx);
}

void VaultApp::onExit(AppContext& ctx) {
  clearSession(ctx);
}

void VaultApp::ensureSession(AppContext& ctx) {
  if (ctx.display.isSleeping() || !ctx.security.isPinEnabled() || !ctx.security.hasActiveKey()) return;
  if (!ctx.vault.isLoaded()) {
    attemptedLoad_ = true;
    if (!ctx.vault.loadVault(ctx.security.getActiveVaultKey())) {
      status_ = "Vault unreadable";
    }
    fullRedraw_ = true;
    cardDirty_ = true;
    invalidate();
  }
}

void VaultApp::onDisplayRestored(AppContext& ctx) {
  ensureSession(ctx);
  fullRedraw_ = true;
  cardDirty_ = true;
  keyboardDirty_ = true;
  invalidate();
}

void VaultApp::onTick(AppContext& ctx, uint32_t nowMs) {
  if (!ctx.security.hasActiveKey() || ctx.display.isSleeping()) {
    if (attemptedLoad_ || view_ != View::Card) clearSession(ctx);
    return;
  }
  ensureSession(ctx);

  if (sending_) {
    const UsbTransferState state = ctx.usbHid.transferState();
    if (state == UsbTransferState::Sent || state == UsbTransferState::Failed) {
      sending_ = false;
      status_ = (state == UsbTransferState::Sent ? "Typed via USB!" : "USB send failed");
      feedbackUntilMs_ = nowMs + 2000;
      cardDirty_ = true;
      invalidate();
    }
  } else if (feedbackUntilMs_ && static_cast<int32_t>(nowMs - feedbackUntilMs_) >= 0) {
    feedbackUntilMs_ = 0;
    status_ = "";
    cardDirty_ = true;
    invalidate();
  }
}

bool VaultApp::onBack(AppContext& ctx) {
  if (view_ == View::Card) return false;
  clearEditor();
  ctx.usbHid.cancel();
  view_ = View::Card;
  armedAction_ = ArmedAction::None;
  status_ = "";
  fullRedraw_ = true;
  cardDirty_ = true;
  invalidate();
  return true;
}

void VaultApp::onPrimaryAction(AppContext& ctx) {
  if (view_ == View::Card && ctx.vault.count()) {
    ctx.usbHid.cancel();
    sending_ = false;
    armedAction_ = ArmedAction::None;
    showPassword_ = false;
    status_ = "";
    entryIndex_ = (entryIndex_ + 1) % ctx.vault.count();
    cardDirty_ = true;
    invalidate();
  }
}

void VaultApp::handleKeyboardTouch(AppContext& ctx, const TouchEvent& ev) {
  char* target = nullptr;
  size_t maxLen = 0;
  if (view_ == View::EditTitle) { target = editTitle_; maxLen = sizeof(editTitle_) - 1; }
  else if (view_ == View::EditUser) { target = editUser_; maxLen = sizeof(editUser_) - 1; }
  else if (view_ == View::EditPass) { target = editPass_; maxLen = sizeof(editPass_) - 1; }
  else if (view_ == View::EditNotes) { target = editNotes_; maxLen = sizeof(editNotes_) - 1; }
  if (!target) return;

  const TextKeyboard::Key key = TextKeyboard::hit(ev.x, ev.y, capitals_, symbols_);
  size_t len = strlen(target);

  if (key.action == TextKeyboard::Action::Shift) {
    capitals_ = !capitals_;
    keyboardDirty_ = true;
    invalidate();
  } else if (key.action == TextKeyboard::Action::Symbols) {
    symbols_ = !symbols_;
    keyboardDirty_ = true;
    invalidate();
  } else if (key.action == TextKeyboard::Action::Delete) {
    if (len > 0) {
      target[len - 1] = '\0';
      invalidate();
    }
  } else if (key.action == TextKeyboard::Action::Space) {
    if (len < maxLen) {
      target[len] = ' ';
      target[len + 1] = '\0';
      invalidate();
    }
  } else if (key.action == TextKeyboard::Action::Character && key.character) {
    if (len < maxLen) {
      target[len] = key.character;
      target[len + 1] = '\0';
      invalidate();
    }
  } else if (key.action == TextKeyboard::Action::Cancel) {
    onBack(ctx);
  } else if (key.action == TextKeyboard::Action::Accept) {
    if (view_ == View::EditTitle) {
      if (editTitle_[0] == '\0') strncpy(editTitle_, "Account", sizeof(editTitle_) - 1);
      view_ = View::EditUser;
      fullRedraw_ = true;
      keyboardDirty_ = true;
      invalidate();
    } else if (view_ == View::EditUser) {
      view_ = View::EditPass;
      fullRedraw_ = true;
      keyboardDirty_ = true;
      invalidate();
    } else if (view_ == View::EditPass) {
      view_ = View::EditNotes;
      fullRedraw_ = true;
      keyboardDirty_ = true;
      invalidate();
    } else if (view_ == View::EditNotes) {
      if (ctx.vault.addEntry(ctx.security.getActiveVaultKey(), editTitle_, editUser_, editPass_, editNotes_)) {
        entryIndex_ = ctx.vault.count() - 1;
        status_ = "Saved!";
      } else {
        status_ = "Save failed";
      }
      clearEditor();
      view_ = View::Card;
      armedAction_ = ArmedAction::None;
      fullRedraw_ = true;
      cardDirty_ = true;
      invalidate();
    }
  }
}

void VaultApp::onTouch(AppContext& ctx, const TouchEvent& ev) {
  if (ev.gesture != TouchGesture::Tap && ev.gesture != TouchGesture::SwipeLeft &&
      ev.gesture != TouchGesture::SwipeRight) return;

  if (view_ == View::EditTitle || view_ == View::EditUser ||
      view_ == View::EditPass || view_ == View::EditNotes) {
    if (ev.gesture == TouchGesture::Tap) {
      handleKeyboardTouch(ctx, ev);
    }
    return;
  }

  if (view_ == View::ConfirmDelete) {
    if (ev.gesture != TouchGesture::Tap || ev.y < 95 || ev.y > 135) return;
    if (ev.x < 155) {
      view_ = View::Card;
      fullRedraw_ = true;
      cardDirty_ = true;
      invalidate();
      return;
    }
    const uint32_t start = ev.contactId ? ev.contactStartedMs : millis();
    if (static_cast<int32_t>(start - confirmAfterMs_) < 0) return;
    if (ctx.vault.removeEntry(ctx.security.getActiveVaultKey(), entryIndex_)) {
      if (entryIndex_ > 0) entryIndex_--;
      status_ = "Deleted";
    } else {
      status_ = "Delete failed";
    }
    view_ = View::Card;
    armedAction_ = ArmedAction::None;
    fullRedraw_ = true;
    cardDirty_ = true;
    invalidate();
    return;
  }

  // View::Card handling
  if (sending_) return;
  const uint8_t count = ctx.vault.count();
  const bool unlocked = ctx.security.isPinEnabled() && ctx.security.hasActiveKey() && ctx.vault.isLoaded();
  if (!unlocked) return;

  // Swipe or Arrow navigation (respects category filter)
  if (ev.gesture == TouchGesture::SwipeLeft || (ev.gesture == TouchGesture::Tap && ev.x >= 284 && ev.y >= 35 && ev.y < 144)) {
    for (int next = entryIndex_ + 1; next < count; ++next) {
      if (entryMatchesCategory(ctx.vault.getEntry(static_cast<uint8_t>(next)), categoryFilter_)) {
        entryIndex_ = static_cast<uint8_t>(next);
        showPassword_ = false;
        armedAction_ = ArmedAction::None;
        cardDirty_ = true;
        invalidate();
        break;
      }
    }
    return;
  }
  if (ev.gesture == TouchGesture::SwipeRight || (ev.gesture == TouchGesture::Tap && ev.x <= 36 && ev.y >= 35 && ev.y < 144)) {
    for (int prev = entryIndex_ - 1; prev >= 0; --prev) {
      if (entryMatchesCategory(ctx.vault.getEntry(static_cast<uint8_t>(prev)), categoryFilter_)) {
        entryIndex_ = static_cast<uint8_t>(prev);
        showPassword_ = false;
        armedAction_ = ArmedAction::None;
        cardDirty_ = true;
        invalidate();
        break;
      }
    }
    return;
  }

  if (ev.gesture != TouchGesture::Tap) return;

  // Category filter pill tap: x: 210..280, y: 35..55
  if (ev.x >= 210 && ev.x <= 280 && ev.y >= 35 && ev.y <= 55) {
    categoryFilter_ = static_cast<uint8_t>((categoryFilter_ + 1) % 4);
    // Find first entry matching new filter
    for (uint8_t i = 0; i < count; ++i) {
      if (entryMatchesCategory(ctx.vault.getEntry(i), categoryFilter_)) {
        entryIndex_ = i;
        break;
      }
    }
    showPassword_ = false;
    armedAction_ = ArmedAction::None;
    cardDirty_ = true;
    invalidate();
    return;
  }

  // Bottom action bar
  if (ev.y >= 146) {
    if (ev.x >= 80 && ev.x < 160 && count < kMaxVaultEntries) {
      clearEditor();
      view_ = View::EditTitle;
      status_ = "";
      fullRedraw_ = true;
      keyboardDirty_ = true;
      invalidate();
    } else if (ev.x >= 160 && ev.x < 240 && count) {
      view_ = View::ConfirmDelete;
      confirmAfterMs_ = millis() + 300;
      status_ = "";
      fullRedraw_ = true;
      invalidate();
    }
    return;
  }

  if (!count) return;

  // Show/Hide password toggle
  if (ev.x >= 220 && ev.x <= 280 && ev.y >= 64 && ev.y <= 85) {
    showPassword_ = !showPassword_;
    cardDirty_ = true;
    invalidate();
    return;
  }

  // Two-step confirmed USB typing:
  // Button 1: Type User (x: 42..158, y: 86..116)
  if (ev.x >= 42 && ev.x <= 158 && ev.y >= 86 && ev.y <= 116) {
    if (armedAction_ != ArmedAction::TypeUser) {
      // Step 1: Arm action
      armedAction_ = ArmedAction::TypeUser;
      armedContactId_ = ev.contactId;
      cardDirty_ = true;
      invalidate();
    } else {
      // Step 2: Confirm action (finger lifted between touches)
      if (ev.contactId != armedContactId_) {
        const VaultEntry* entry = ctx.vault.getEntry(entryIndex_);
        if (entry && entry->username[0]) {
          if (ctx.bleHid.isEnabled() && ctx.bleHid.isConnected()) {
            ctx.bleHid.typeText(entry->username);
            sending_ = true;
            status_ = "Typed user via BLE!";
          } else if (!ctx.usbHid.isReady()) {
            status_ = "USB/BLE unavailable";
          } else if (ctx.usbHid.typeText(entry->username, false)) {
            sending_ = true;
            status_ = "Typing user...";
          } else {
            status_ = "USB send failed";
          }
        } else {
          status_ = "User empty";
        }
        armedAction_ = ArmedAction::None;
        cardDirty_ = true;
        invalidate();
      }
    }
    return;
  }

  // Button 2: Type Password (x: 162..278, y: 86..116)
  if (ev.x >= 162 && ev.x <= 278 && ev.y >= 86 && ev.y <= 116) {
    if (armedAction_ != ArmedAction::TypePass) {
      // Step 1: Arm action
      armedAction_ = ArmedAction::TypePass;
      armedContactId_ = ev.contactId;
      cardDirty_ = true;
      invalidate();
    } else {
      // Step 2: Confirm action (finger lifted between touches)
      if (ev.contactId != armedContactId_) {
        const VaultEntry* entry = ctx.vault.getEntry(entryIndex_);
        if (entry && entry->password[0]) {
          if (ctx.bleHid.isEnabled() && ctx.bleHid.isConnected()) {
            ctx.bleHid.typeText(entry->password);
            sending_ = true;
            status_ = "Typed pass via BLE!";
          } else if (!ctx.usbHid.isReady()) {
            status_ = "USB/BLE unavailable";
          } else if (ctx.usbHid.typeText(entry->password, false)) {
            sending_ = true;
            status_ = "Typing password...";
          } else {
            status_ = "USB send failed";
          }
        } else {
          status_ = "Pass empty";
        }
        armedAction_ = ArmedAction::None;
        cardDirty_ = true;
        invalidate();
      }
    }
    return;
  }

  // Tapping anywhere else inside card disarms
  if (armedAction_ != ArmedAction::None) {
    armedAction_ = ArmedAction::None;
    cardDirty_ = true;
    invalidate();
  }
}

void VaultApp::renderEditor(AppContext& ctx) {
  auto& screen = ctx.display;
  auto& t = screen.gfx();
  if (fullRedraw_) {
    screen.clear();
    const char* hTitle = "Entry Title";
    if (view_ == View::EditUser) hTitle = "Username / Email";
    else if (view_ == View::EditPass) hTitle = "Password";
    else if (view_ == View::EditNotes) hTitle = "Notes (Optional)";
    screen.header(hTitle, true);
    keyboardDirty_ = true;
    fullRedraw_ = false;
  }
  if (keyboardDirty_) {
    const char* acceptLabel = (view_ == View::EditNotes ? "SAVE" : "NEXT");
    TextKeyboard::draw(screen, capitals_, symbols_, acceptLabel, false);
    keyboardDirty_ = false;
  }
  t.fillRoundRect(3, 33, 314, 17, 3, Colors::Panel);
  t.drawRoundRect(3, 33, 314, 17, 3, Colors::Border);

  char text[54]{};
  if (view_ == View::EditTitle) snprintf(text, sizeof(text), "%.23s", editTitle_);
  else if (view_ == View::EditUser) snprintf(text, sizeof(text), "%.31s", editUser_);
  else if (view_ == View::EditPass) {
    size_t plen = strlen(editPass_);
    if (plen > 28) plen = 28;
    for (size_t i = 0; i < plen; ++i) text[i] = '*';
    text[plen] = '\0';
  } else if (view_ == View::EditNotes) snprintf(text, sizeof(text), "%.23s", editNotes_);

  screen.label(8, 38, text, Colors::White, 1, Colors::Panel);
}

void VaultApp::renderConfirmDelete(AppContext& ctx) {
  auto& screen = ctx.display;
  auto& t = screen.gfx();
  t.fillRect(0, 33, 320, 137, Colors::Bg);
  screen.label(35, 50, "Delete this credential?", Colors::White, 2);
  screen.label(35, 76, status_[0] ? status_ : "This action cannot be undone.", Colors::Amber, 1);
  screen.label(45, 110, "CANCEL", Colors::Cyan, 2);
  screen.label(190, 110, "DELETE", Colors::Red, 2);
}

void VaultApp::renderCard(AppContext& ctx) {
  auto& screen = ctx.display;
  auto& t = screen.gfx();
  if (fullRedraw_) {
    screen.clear();
    screen.header(title(), true);
    fullRedraw_ = false;
  }

  const uint8_t count = ctx.vault.count();
  const bool unlocked = ctx.security.isPinEnabled() && ctx.security.hasActiveKey() && ctx.vault.isLoaded();

  if (cardDirty_) {
    cardDirty_ = false;
    t.fillRect(0, 33, 320, 112, Colors::Bg);

    if (!unlocked) {
      screen.label(30, 65, ctx.vault.hasStorageError() ? "Vault unreadable" : "Vault Locked", Colors::Amber, 2);
      screen.label(30, 95, ctx.vault.hasStorageError() ? "Back and retry. Data preserved." : "Unlock with your PIN to access.", Colors::Muted, 1);
    } else if (!count) {
      screen.label(85, 65, "Vault empty", Colors::White, 2);
      screen.label(40, 98, "Tap ADD below to store passwords.", Colors::Muted, 1);
    } else {
      if (entryIndex_ >= count) entryIndex_ = count - 1;
      const VaultEntry* entry = ctx.vault.getEntry(entryIndex_);

      // Arrows
      screen.label(10, 75, "<", entryIndex_ > 0 ? Colors::Cyan : Colors::Muted, 2);
      screen.label(298, 75, ">", entryIndex_ + 1 < count ? Colors::Cyan : Colors::Muted, 2);

      // Card container
      t.fillRoundRect(34, 33, 252, 108, 6, Colors::Panel);
      t.drawRoundRect(34, 33, 252, 108, 6, Colors::Border);

      // Title
      screen.label(42, 38, entry->title, Colors::Cyan, strlen(entry->title) > 14 ? 1 : 2, Colors::Panel);

      // Category Pill Button
      static const uint16_t kCatColors[] = {Colors::White, Colors::Cyan, Colors::Green, Colors::Amber};
      const char* catLabel = kCategories[categoryFilter_ % 4];
      const uint16_t catColor = kCatColors[categoryFilter_ % 4];
      t.fillRoundRect(224, 38, 54, 16, 3, Colors::Bg);
      t.drawRoundRect(224, 38, 54, 16, 3, catColor);
      screen.label(234, 42, catLabel, catColor, 1, Colors::Bg);

      // Username
      screen.label(42, 54, "User:", Colors::Muted, 1, Colors::Panel);
      screen.label(76, 54, entry->username, Colors::White, 1, Colors::Panel);

      // Password (masked or visible)
      screen.label(42, 69, "Pass:", Colors::Muted, 1, Colors::Panel);
      if (showPassword_) {
        screen.label(76, 69, entry->password, Colors::Amber, 1, Colors::Panel);
      } else {
        screen.label(76, 69, "••••••••••••", Colors::Amber, 1, Colors::Panel);
      }

      // Show/Hide toggle button
      t.fillRoundRect(224, 65, 54, 16, 3, Colors::Bg);
      t.drawRoundRect(224, 65, 54, 16, 3, showPassword_ ? Colors::Amber : Colors::Cyan);
      screen.label(234, 69, showPassword_ ? "HIDE" : "SHOW", showPassword_ ? Colors::Amber : Colors::Cyan, 1, Colors::Bg);

      // Action Buttons for USB HID
      // Button 1: Type User
      if (armedAction_ == ArmedAction::TypeUser) {
        t.fillRoundRect(42, 86, 114, 25, 4, Colors::Cyan);
        screen.label(48, 94, "CONFIRM SEND", Colors::Bg, 1, Colors::Cyan);
      } else {
        t.fillRoundRect(42, 86, 114, 25, 4, Colors::Bg);
        t.drawRoundRect(42, 86, 114, 25, 4, Colors::Cyan);
        screen.label(54, 94, "TYPE USER", Colors::Cyan, 1, Colors::Bg);
      }

      // Button 2: Type Password
      if (armedAction_ == ArmedAction::TypePass) {
        t.fillRoundRect(164, 86, 114, 25, 4, Colors::Amber);
        screen.label(170, 94, "CONFIRM SEND", Colors::Bg, 1, Colors::Amber);
      } else {
        t.fillRoundRect(164, 86, 114, 25, 4, Colors::Bg);
        t.drawRoundRect(164, 86, 114, 25, 4, Colors::Amber);
        screen.label(176, 94, "TYPE PASS", Colors::Amber, 1, Colors::Bg);
      }

      // Status or Notes row
      if (status_[0] != '\0') {
        screen.label(42, 119, status_, Colors::Green, 1, Colors::Panel);
      } else if (entry->notes[0] != '\0') {
        char nbuf[28]{};
        snprintf(nbuf, sizeof(nbuf), "Note: %.18s", entry->notes);
        screen.label(42, 119, nbuf, Colors::Muted, 1, Colors::Panel);
      }
    }

    // Bottom Navigation Bar
    t.fillRect(0, 146, 320, 24, Colors::Panel);
    char counter[12]{};
    snprintf(counter, sizeof(counter), "%u/%u", count ? entryIndex_ + 1 : 0, count);
    screen.label(12, 154, counter, Colors::Muted, 1, Colors::Panel);
    screen.label(100, 154, "ADD", unlocked && count < kMaxVaultEntries ? Colors::Cyan : Colors::Muted, 1, Colors::Panel);
    screen.label(175, 154, "DELETE", unlocked && count ? Colors::Red : Colors::Muted, 1, Colors::Panel);
  }
}

void VaultApp::render(AppContext& ctx) {
  if (view_ == View::EditTitle || view_ == View::EditUser ||
      view_ == View::EditPass || view_ == View::EditNotes) {
    renderEditor(ctx);
    return;
  }
  if (view_ == View::ConfirmDelete) {
    renderConfirmDelete(ctx);
    return;
  }
  renderCard(ctx);
}
