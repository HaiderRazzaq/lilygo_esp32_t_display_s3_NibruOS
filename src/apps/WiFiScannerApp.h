#pragma once
#include "core/App.h"
#include "services/WiFiService.h"

class WiFiScannerApp final : public App {
public:
  const char* title() const override { return "Wi-Fi Scanner"; }
  void onEnter(AppContext& ctx) override;
  void onExit(AppContext& ctx) override;
  void onTick(AppContext& ctx, uint32_t nowMs) override;
  void onTouch(AppContext& ctx, const TouchEvent& ev) override;
  void render(AppContext& ctx) override;
  void onPrimaryAction(AppContext& ctx) override;
private:
  bool acquired_ = false;
  uint8_t offset_ = 0;
  uint32_t lastAnimMs_ = 0;
  ScanState lastState_ = ScanState::Idle;
  void refresh(AppContext& ctx);
};
