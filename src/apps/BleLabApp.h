#pragma once
#include "core/App.h"
#include "services/BleAdvertisingPayload.h"

class BleLabApp final : public App {
public:
  const char* title() const override { return "BLE Lab"; }
  const char* shortTitle() const override { return "BLE"; }
  const char* subtitle() const override { return "Advertising"; }
  void onEnter(AppContext& ctx) override;
  void onExit(AppContext& ctx) override;
  void onTick(AppContext& ctx, uint32_t nowMs) override;
  void onTouch(AppContext& ctx, const TouchEvent& touch) override;
  void onPrimaryAction(AppContext& ctx) override;
  void onActivate(AppContext& ctx) override;
  bool onBack(AppContext& ctx) override;
  void render(AppContext& ctx) override;
  bool isSelectingTarget() const { return selectingTarget_; }
  BleAdvertisingProfile selectedProfile() const { return profile_; }
private:
  void selectTarget(uint8_t index);
  void setPickerPage(uint8_t page);
  void renderTargets(AppContext& ctx);
  bool selectingTarget_ = false;
  uint8_t targetIndex_ = 0;
  uint8_t pickerPage_ = 0;
  uint32_t pickerContactId_ = 0;
  uint8_t count_ = 5;
  uint8_t seconds_ = 30;
  BleAdvertisingProfile profile_ = BleAdvertisingProfile::Android;
  uint8_t drawnCount_ = 0;
  bool drawnActive_ = false;
  const char* drawnError_ = nullptr;
};
