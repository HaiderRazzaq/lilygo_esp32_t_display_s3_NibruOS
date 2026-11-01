#pragma once
#include "core/App.h"
#include <stdint.h>

/**
 * @brief 2FA Authenticator application for NibruOS.
 * Displays one 2FA account per screen with clear, compact typography,
 * live countdown timer, swipe/button browsing, and instant USB HID injection.
 */
class TotpApp : public App {
public:
  const char* title() const override { return "2FA Auth"; }
  const char* shortTitle() const override { return "2FA"; }
  const char* subtitle() const override { return "Tokens"; }

  void onEnter(AppContext& ctx) override;
  void onExit(AppContext& ctx) override;
  void onTick(AppContext& ctx, uint32_t nowMs) override;
  void onTouch(AppContext& ctx, const TouchEvent& ev) override;
  void render(AppContext& ctx) override;
  bool onBack(AppContext& ctx) override;
  void onPrimaryAction(AppContext& ctx) override;
  void onDisplayRestored(AppContext&) override { fullRedraw_ = true; invalidate(); }
  void clearSession(AppContext& ctx);

private:
  uint8_t accountIndex_ = 0;
  uint32_t lastSecond_ = 0;
  enum class View : uint8_t { Card, Methods, Name, Secret, ImportUsb, ImportWifi, ImportReview, ConfirmDelete };
  View view_ = View::Card;
  char name_[24]{};
  char secret_[53]{};
  const char* status_ = "";
  bool attemptedLoad_ = false;
  bool sending_ = false;
  uint32_t feedbackUntilMs_ = 0;
  uint32_t confirmAfterMs_ = 0;
  bool fullRedraw_ = true;
  bool keyboardDirty_ = true;
  bool capitals_ = true;
  bool symbols_ = false;
  uint32_t importRemaining_ = 0;

  void ensureSession(AppContext& ctx);
  void clearEditor();
  void renderEditor(AppContext& ctx);
  void renderImport(AppContext& ctx);
  void endImport(AppContext& ctx);
};
