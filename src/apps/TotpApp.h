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

private:
  uint8_t accountIndex_ = 0;
  uint32_t lastSecond_ = 0;
  bool injectionFeedback_ = false;
  uint32_t feedbackUntilMs_ = 0;
  bool fullRedraw_ = true;

  void renderCard(AppContext& ctx, uint8_t index, const char* name,
                  const char* user, const char* code, uint8_t remainingSec);
};
