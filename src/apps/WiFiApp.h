#include "services/LocalizationService.h"
#pragma once
#include "core/App.h"
#include "apps/WiFiScannerApp.h"

enum class WiFiSubView : uint8_t {
  Menu,
  Scanner,
  Saved
};

class WiFiApp final : public App {
public:
  const char* title() const override { return LocalizationService::instance().get(TextId::AppWiFi); }
  const char* shortTitle() const override { return "WIFI"; }
  const char* subtitle() const override { return LocalizationService::instance().get(TextId::SubWiFi); }

  void onEnter(AppContext& ctx) override;
  void onExit(AppContext& ctx) override;
  void onTick(AppContext& ctx, uint32_t nowMs) override;
  void onTouch(AppContext& ctx, const TouchEvent& ev) override;
  void onPrimaryAction(AppContext& ctx) override;
  bool onBack(AppContext& ctx) override;
  void render(AppContext& ctx) override;
  void onDisplayRestored(AppContext& ctx) override {
    menuFullRedraw_ = true;
    if (subView_ == WiFiSubView::Scanner) {
      scannerApp_.onDisplayRestored(ctx);
    }
    invalidate();
  }

private:
  WiFiSubView subView_ = WiFiSubView::Menu;
  WiFiScannerApp scannerApp_;
  uint8_t savedOffset_ = 0;
  ConnectState lastConnState_ = ConnectState::Disconnected;
  bool menuFullRedraw_ = true;
  uint8_t armedCard_ = 255;
  uint32_t armedCardContactId_ = 0;

  void renderMenu(AppContext& ctx);
  void renderSaved(AppContext& ctx);
  void handleMenuTouch(AppContext& ctx, const TouchEvent& ev);
  void handleSavedTouch(AppContext& ctx, const TouchEvent& ev);
};

