#pragma once
#include "core/App.h"
#include "services/WiFiService.h"

class WiFiChannelsApp final : public App {
public:
  const char* title() const override { return "Wi-Fi Channels"; }
  const char* shortTitle() const override { return "CH"; }
  const char* subtitle() const override { return "AP counts"; }
  void onEnter(AppContext& ctx) override;
  void onExit(AppContext& ctx) override;
  void onTick(AppContext& ctx, uint32_t nowMs) override;
  void onTouch(AppContext& ctx, const TouchEvent& touch) override;
  void onPrimaryAction(AppContext& ctx) override;
  void render(AppContext& ctx) override;
  uint8_t selectedChannel() const { return selectedChannel_; }

private:
  bool acquired_ = false;
  uint8_t selectedChannel_ = 1;
  ScanState lastState_ = ScanState::Idle;
};
