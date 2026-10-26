#pragma once
#include "core/App.h"

class TouchTestApp final : public App {
public:
  const char* title() const override { return "Touch Test"; }
  void onEnter(AppContext& ctx) override;
  void onExit(AppContext& ctx) override;
  void onTick(AppContext& ctx, uint32_t nowMs) override;
  void onTouch(AppContext& ctx, const TouchEvent& ev) override;
  void render(AppContext& ctx) override;
private:
  bool touched_ = false;
  TouchEvent last_{};
};
