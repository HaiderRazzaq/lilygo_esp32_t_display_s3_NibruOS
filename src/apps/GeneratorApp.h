#pragma once
#include "core/App.h"
#include "services/LocalizationService.h"
#include <stdint.h>

class GeneratorApp final : public App {
public:
  const char* title() const override { return LocalizationService::instance().get(TextId::AppGenerator); }
  const char* shortTitle() const override { return "GEN"; }
  const char* subtitle() const override { return LocalizationService::instance().get(TextId::SubGenerator); }

  void onEnter(AppContext& ctx) override;
  void onExit(AppContext& ctx) override;
  void onTick(AppContext& ctx, uint32_t nowMs) override;
  void onTouch(AppContext& ctx, const TouchEvent& ev) override;
  bool onBack(AppContext& ctx) override;
  void onDisplayRestored(AppContext& ctx) override;
  void render(AppContext& ctx) override;

private:
  void generatePassword(AppContext& ctx);
  void executeSave(AppContext& ctx);

  char currentPass_[33]{};
  uint8_t length_ = 16;
  bool includeSymbols_ = true;
  bool includeDigits_ = true;
  bool pendingSave_ = false;
  const char* status_ = "";
  uint32_t feedbackUntilMs_ = 0;
  bool fullRedraw_ = true;
};
