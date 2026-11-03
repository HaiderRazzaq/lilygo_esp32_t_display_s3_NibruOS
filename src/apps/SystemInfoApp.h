#include "services/LocalizationService.h"
#pragma once
#include "core/App.h"

class SystemInfoApp final : public App {
public:
  const char* title() const override { return LocalizationService::instance().get(TextId::AppSystemInfo); }
  const char* shortTitle() const override { return "SYS"; }
  const char* subtitle() const override { return LocalizationService::instance().get(TextId::SubSystemInfo); }
  void onEnter(AppContext& ctx) override;
  void onExit(AppContext& ctx) override;
  void onTick(AppContext& ctx, uint32_t nowMs) override;
  void onTouch(AppContext& ctx, const TouchEvent& ev) override;
  void render(AppContext& ctx) override;
private:
  uint32_t lastUpdateMs_ = 0;
  bool fullRedraw_ = true;
  bool lastOnline_ = false;
  bool lastOnlineSet_ = false;
};
