#pragma once
#include "core/App.h"
#include "services/WiFiService.h"

enum class WiFiView : uint8_t {
  List,
  Keyboard,
  Connecting,
  Connected
};

enum class KeyboardMode : uint8_t {
  Lowercase,
  Uppercase,
  Symbols
};

class WiFiScannerApp final : public App {
public:
  const char* title() const override { return "Scan Networks"; }
  const char* shortTitle() const override { return "SCAN"; }
  const char* subtitle() const override { return "2.4GHz"; }
  void onEnter(AppContext& ctx) override;
  void onExit(AppContext& ctx) override;
  void onTick(AppContext& ctx, uint32_t nowMs) override;
  void onTouch(AppContext& ctx, const TouchEvent& ev) override;
  void render(AppContext& ctx) override;
  void onPrimaryAction(AppContext& ctx) override;
  bool onBack(AppContext& ctx) override;

private:
  bool acquired_ = false;
  uint8_t offset_ = 0;
  uint32_t lastAnimMs_ = 0;
  uint32_t viewChangeMs_ = 0;
  uint32_t lastKeyMs_ = 0;
  bool fullRedraw_ = true;

  ScanState lastScanState_ = ScanState::Idle;
  ConnectState lastConnState_ = ConnectState::Disconnected;

  WiFiView view_ = WiFiView::List;
  KeyboardMode kbMode_ = KeyboardMode::Lowercase;

  NetworkRecord selectedNet_{};
  char password_[64]{};
  uint8_t passwordLen_ = 0;
  char errorMsg_[32]{};

  void setView(WiFiView newView);
  void refresh(AppContext& ctx);
  void handleListTouch(AppContext& ctx, const TouchEvent& ev);
  void handleKeyboardTouch(AppContext& ctx, const TouchEvent& ev);
  void handleConnectingTouch(AppContext& ctx, const TouchEvent& ev);
  void handleConnectedTouch(AppContext& ctx, const TouchEvent& ev);

  void renderList(AppContext& ctx);
  void renderKeyboard(AppContext& ctx);
  void renderConnecting(AppContext& ctx);
  void renderConnected(AppContext& ctx);

  void renderPasswordBox(AppContext& ctx);
  void startConnection(AppContext& ctx);
};
