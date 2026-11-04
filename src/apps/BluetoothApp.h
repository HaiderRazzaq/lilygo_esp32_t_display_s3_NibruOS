#pragma once
#include "core/App.h"
#include "services/BleHidService.h"

class BluetoothApp final : public App {
public:
  const char* title() const override;
  const char* shortTitle() const override { return "BLE"; }
  const char* subtitle() const override;
  void onEnter(AppContext& ctx) override;
  void onExit(AppContext& ctx) override;
  void onTick(AppContext& ctx, uint32_t nowMs) override;
  void onTouch(AppContext& ctx, const TouchEvent& ev) override;
  void onPrimaryAction(AppContext& ctx) override;
  void render(AppContext& ctx) override;

private:
  void toggle(AppContext& ctx);
  void testKeyboard(AppContext& ctx);
  const char* feedback_ = nullptr;
  uint32_t feedbackSinceMs_ = 0;
  BleConnectionState drawnState_ = BleConnectionState::Disabled;
  uint32_t drawnPairingCode_ = BleHidService::kNoPairingCode;
};
