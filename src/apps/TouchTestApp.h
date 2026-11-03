#include "services/LocalizationService.h"
#pragma once
#include "core/App.h"

class TouchTestApp final : public App {
public:
  const char* title() const override { return LocalizationService::instance().get(TextId::AppTouchTest); }
  const char* shortTitle() const override { return "TAP"; }
  const char* subtitle() const override { return LocalizationService::instance().get(TextId::SubTouchTest); }
  void onEnter(AppContext& ctx) override;
  void onExit(AppContext& ctx) override;
  void onTick(AppContext& ctx, uint32_t nowMs) override;
  void onTouch(AppContext& ctx, const TouchEvent& ev) override;
  void render(AppContext& ctx) override;
  void onDisplayRestored(AppContext&) override { fullRedraw_ = true; invalidate(); }
private:
  bool touched_ = false;
  bool fullRedraw_ = true;
  TouchEvent last_{};
};
