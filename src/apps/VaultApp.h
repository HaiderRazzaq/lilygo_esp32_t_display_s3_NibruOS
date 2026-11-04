#pragma once
#include "core/App.h"
#include "services/LocalizationService.h"
#include <stdint.h>

/**
 * @brief Password Vault Application for NibruOS.
 * Provides encrypted account credential browsing (title, username, masked password, notes),
 * confirmed two-step USB HID credential transmission, and on-device credential management.
 */
class VaultApp final : public App {
public:
  const char* title() const override { return LocalizationService::instance().get(TextId::AppVault); }
  const char* shortTitle() const override { return "PASS"; }
  const char* subtitle() const override { return LocalizationService::instance().get(TextId::SubVault); }

  void onEnter(AppContext& ctx) override;
  void onExit(AppContext& ctx) override;
  void onTick(AppContext& ctx, uint32_t nowMs) override;
  void onTouch(AppContext& ctx, const TouchEvent& ev) override;
  void render(AppContext& ctx) override;
  bool onBack(AppContext& ctx) override;
  void onPrimaryAction(AppContext& ctx) override;
  void onDisplayRestored(AppContext& ctx) override;
  void clearSession(AppContext& ctx);

private:
  enum class View : uint8_t {
    Card,
    EditTitle,
    EditUser,
    EditPass,
    EditNotes,
    ConfirmDelete
  };

  enum class ArmedAction : uint8_t {
    None,
    TypeUser,
    TypePass
  };

  uint8_t entryIndex_ = 0;
  uint8_t categoryFilter_ = 0; // 0: ALL, 1: WORK, 2: PERS, 3: FIN
  View view_ = View::Card;
  ArmedAction armedAction_ = ArmedAction::None;
  uint32_t armedContactId_ = 0;

  bool showPassword_ = false;
  bool attemptedLoad_ = false;
  bool sending_ = false;
  uint32_t feedbackUntilMs_ = 0;
  uint32_t confirmAfterMs_ = 0;
  const char* status_ = "";

  // Dynamic redraw flags
  bool fullRedraw_ = true;
  bool cardDirty_ = true;
  bool keyboardDirty_ = true;

  // Editor working buffers (zeroed immediately on exit/cancel/commit)
  char editTitle_[24]{};
  char editUser_[32]{};
  char editPass_[48]{};
  char editNotes_[24]{};
  bool capitals_ = true;
  bool symbols_ = false;

  void ensureSession(AppContext& ctx);
  void clearEditor();
  void renderCard(AppContext& ctx);
  void renderEditor(AppContext& ctx);
  void renderConfirmDelete(AppContext& ctx);
  void handleKeyboardTouch(AppContext& ctx, const TouchEvent& ev);
};
