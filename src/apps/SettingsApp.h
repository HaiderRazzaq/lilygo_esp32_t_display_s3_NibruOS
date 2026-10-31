#pragma once
#include "core/App.h"
#include "apps/SystemInfoApp.h"
#include "apps/TouchTestApp.h"

enum class SettingsSubView : uint8_t {
  Menu,
  SystemInfo,
  TouchTest
};

class SettingsApp final : public App {
public:
  const char* title() const override { return "Settings"; }
  const char* shortTitle() const override { return "SET"; }
  const char* subtitle() const override { return "Config"; }

  void onEnter(AppContext& ctx) override;
  void onExit(AppContext& ctx) override;
  void onTick(AppContext& ctx, uint32_t nowMs) override;
  void onTouch(AppContext& ctx, const TouchEvent& ev) override;
  void onPrimaryAction(AppContext& ctx) override;
  bool onBack(AppContext& ctx) override;
  void render(AppContext& ctx) override;

private:
  SettingsSubView subView_ = SettingsSubView::Menu;
  SystemInfoApp systemInfo_;
  TouchTestApp touchTest_;

  void renderMenu(AppContext& ctx);
};

