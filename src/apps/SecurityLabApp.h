#pragma once
#include "core/App.h"
#include <stdint.h>

class BleLabApp;
class BeaconLabApp;
class WifiDefenseApp;
class WiFiChannelsApp;

/**
 * @brief Security Lab Hub - Unified wireless security and audit laboratory.
 *
 * Consolidates BLE Lab, Beacon Lab, WiFi Defense, and WiFi Channels
 * into a single top-level application with nested sub-tool navigation.
 */
class SecurityLabApp final : public App {
public:
  SecurityLabApp(BleLabApp& bleLab, BeaconLabApp& beaconLab,
                 WifiDefenseApp& wifiDefense, WiFiChannelsApp& wifiChannels);

  const char* title() const override { return "Security Lab"; }
  const char* shortTitle() const override { return "SECLAB"; }
  const char* subtitle() const override { return "Audit & Defense"; }

  void onEnter(AppContext& ctx) override;
  void onExit(AppContext& ctx) override;
  void onTick(AppContext& ctx, uint32_t nowMs) override;
  void onTouch(AppContext& ctx, const TouchEvent& touch) override;
  void onPrimaryAction(AppContext& ctx) override;
  void onActivate(AppContext& ctx) override;
  bool onBack(AppContext& ctx) override;
  void render(AppContext& ctx) override;
  void onDisplayRestored(AppContext& ctx) override;

  uint8_t selectedIndex() const { return selectedIndex_; }
  App* activeTool() const { return activeTool_; }
  void openTool(uint8_t index, AppContext& ctx);
  void closeTool(AppContext& ctx);

private:
  void drawMenu(AppContext& ctx);

  BleLabApp& bleLab_;
  BeaconLabApp& beaconLab_;
  WifiDefenseApp& wifiDefense_;
  WiFiChannelsApp& wifiChannels_;

  App* tools_[4]{};
  App* activeTool_ = nullptr;
  uint8_t selectedIndex_ = 0;
  bool fullRedraw_ = true;
};
