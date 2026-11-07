#pragma once
#include "core/App.h"

class BeaconLabApp final : public App {
public:
  const char* title() const override { return "Beacon Lab"; }
  const char* shortTitle() const override { return "LAB"; }
  const char* subtitle() const override { return "Wi-Fi test"; }
  void onEnter(AppContext&) override;
  void onExit(AppContext& ctx) override;
  void onTick(AppContext& ctx, uint32_t nowMs) override;
  void onTouch(AppContext& ctx, const TouchEvent& touch) override;
  void onPrimaryAction(AppContext& ctx) override;
  void onActivate(AppContext& ctx) override;
  void forceStart(AppContext& ctx);
  void render(AppContext& ctx) override;
  uint8_t openCount() const { return openCount_; }
  uint8_t securedCount() const { return securedCount_; }
  uint8_t durationSeconds() const { return durationSeconds_; }

private:
  uint8_t openCount_ = 10;
  uint8_t securedCount_ = 0;
  uint8_t durationSeconds_ = 10;
  const char* status_ = "";
};
