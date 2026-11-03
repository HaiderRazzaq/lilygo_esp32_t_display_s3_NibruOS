#pragma once
#include "core/App.h"
#include "apps/TouchTestApp.h"
#include <stdint.h>

enum class SettingsSubView : uint8_t {
  Menu,
  Display,
  Theme,
  Memory,
  Security,
  WiFiDiag,
  Device,
  TouchTest,
  SetPin,
  Language,
  WebPortal
};

class SettingsApp final : public App {
public:
  const char* title() const override;
  const char* shortTitle() const override { return "SET"; }
  const char* subtitle() const override;

  void onEnter(AppContext& ctx) override;
  void onExit(AppContext& ctx) override;
  void onTick(AppContext& ctx, uint32_t nowMs) override;
  void onTouch(AppContext& ctx, const TouchEvent& ev) override;
  void onPrimaryAction(AppContext& ctx) override;
  void onActivate(AppContext& ctx) override;
  bool onBack(AppContext& ctx) override;
  void render(AppContext& ctx) override;
  void onDisplayRestored(AppContext& ctx) override;

private:
  SettingsSubView subView_ = SettingsSubView::Menu;
  uint8_t focusedIndex_ = 0;
  uint8_t menuIndex_ = 0, scrollOffset_ = 0, metricPage_ = 0;
  uint8_t deviceIndex_ = 0;
  TouchTestApp touchTest_;
  uint32_t lastMetricTickMs_ = 0;
  bool fullRedraw_ = true;
  bool hasRendered_ = false, drawnSaveOk_ = true;
  SettingsSubView drawnView_ = SettingsSubView::Menu;
  uint8_t drawnFocus_ = 255, drawnMenu_ = 255, drawnScroll_ = 255;
  uint8_t drawnBrightness_ = 0, drawnTheme_ = 255, drawnRotation_ = 255;
  uint8_t drawnLanguage_ = 255;
  uint16_t drawnTimeout_ = 65535;
  uint8_t armedMenuIndex_ = 255;
  uint8_t drawnArmedMenu_ = 255;
  uint32_t armedMenuContactId_ = 0;

  // Confirmation dialog state for Device actions
  enum class ConfirmAction : uint8_t { None, Reboot, ResetSettings };
  ConfirmAction confirmAction_ = ConfirmAction::None;
  ConfirmAction drawnConfirmation_ = ConfirmAction::None;
  uint32_t confirmContactId_ = 0, confirmOpenedMs_ = 0;

  // Security & PIN Setup state
  char pinInputBuffer_[10]{};
  uint8_t pinInputLen_ = 0;
  bool pinError_ = false;
  uint8_t drawnPinLen_ = 255;
  bool drawnPinError_ = false;
  bool drawnPinEnabled_ = false;

  void renderMenu(AppContext& ctx);
  void renderDisplay(AppContext& ctx);
  void renderTheme(AppContext& ctx);
  void renderMemory(AppContext& ctx);
  void renderSecurity(AppContext& ctx);
  void renderWiFiDiag(AppContext& ctx);
  void renderDevice(AppContext& ctx);
  void renderSetPin(AppContext& ctx);
  void renderLanguage(AppContext& ctx);
  void renderWebPortal(AppContext& ctx);

  void handleMenuTouch(AppContext& ctx, const TouchEvent& ev);
  void handleDisplayTouch(AppContext& ctx, const TouchEvent& ev);
  void handleThemeTouch(AppContext& ctx, const TouchEvent& ev);
  void handleMemoryTouch(AppContext& ctx, const TouchEvent& ev);
  void handleSecurityTouch(AppContext& ctx, const TouchEvent& ev);
  void handleWiFiDiagTouch(AppContext& ctx, const TouchEvent& ev);
  void handleDeviceTouch(AppContext& ctx, const TouchEvent& ev);
  void handleSetPinTouch(AppContext& ctx, const TouchEvent& ev);
  void handleLanguageTouch(AppContext& ctx, const TouchEvent& ev);
  void handleWebPortalTouch(AppContext& ctx, const TouchEvent& ev);

  uint32_t drawnWebPortalSec_ = 9999;
  bool webPortalTyped_ = false;
};
