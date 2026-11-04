#pragma once
#include "core/App.h"

class WifiDefenseApp final : public App {
public:
  const char* title() const override { return "WiFi Defense"; }
  const char* shortTitle() const override { return "IDS"; }
  const char* subtitle() const override { return "Attack Monitor"; }

  void onEnter(AppContext& ctx) override;
  void onExit(AppContext& ctx) override;
  void onTick(AppContext& ctx, uint32_t nowMs) override;
  void onTouch(AppContext& ctx, const TouchEvent& touch) override;
  void onPrimaryAction(AppContext& ctx) override;
  void render(AppContext& ctx) override;
  bool onBack(AppContext& ctx) override;

private:
  uint32_t lastRenderMs_ = 0;
  bool fullRedraw_ = true;
  bool resourceAcquired_ = false;
};
