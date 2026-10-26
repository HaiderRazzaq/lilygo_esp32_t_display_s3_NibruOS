#pragma once
#include "core/App.h"

class SystemInfoApp final : public App {
public:
  const char* title() const override { return "System Info"; }
  void onEnter(AppContext& ctx) override;
  void onExit(AppContext& ctx) override;
  void onTick(AppContext& ctx, uint32_t nowMs) override;
  void onTouch(AppContext& ctx, const TouchEvent& ev) override;
  void render(AppContext& ctx) override;
private:
  uint32_t lastUpdateMs_ = 0;
};
