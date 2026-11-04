#pragma once
#include "core/App.h"
#include "services/LocalizationService.h"
#include <stdint.h>

class ClockApp final : public App {
public:
  const char* title() const override { return LocalizationService::instance().get(TextId::AppClock); }
  const char* shortTitle() const override { return "CLK"; }
  const char* subtitle() const override { return LocalizationService::instance().get(TextId::SubClock); }

  void onEnter(AppContext& ctx) override;
  void onExit(AppContext& ctx) override;
  void onTick(AppContext& ctx, uint32_t nowMs) override;
  void onTouch(AppContext& ctx, const TouchEvent& ev) override;
  bool onBack(AppContext& ctx) override;
  void render(AppContext& ctx) override;
  void renderScreensaver(AppContext& ctx, uint32_t nowMs);

private:
  uint32_t lastEpoch_ = 0;
  uint32_t lastScreensaverEpoch_ = 0;
  uint8_t themeIdx_ = 0;
  bool fullRedraw_ = true;
};
