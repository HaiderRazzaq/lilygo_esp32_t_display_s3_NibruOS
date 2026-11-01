#pragma once
#include "core/App.h"
#include "core/AppContext.h"
#include "core/EventTypes.h"
#include <stdint.h>

class AppManager {
public:
  explicit AppManager(AppContext& ctx);
  bool registerApp(App& app);
  void begin();
  void tick(uint32_t nowMs);
  void handleTouch(const TouchEvent& ev);
  void handleButton(ButtonAction action);
  bool open(uint8_t index);
  void goHome();
  void invalidate() {
    if (active_) active_->onDisplayRestored(ctx_);
    else homeDirty_ = true;
  }
  bool isHome() const { return active_ == nullptr; }
  uint8_t selectedIndex() const { return selected_; }
  uint8_t appCount() const { return count_; }
private:
  static constexpr uint8_t kMaxApps = 12;
  AppContext& ctx_;
  App* apps_[kMaxApps]{};
  uint8_t count_ = 0;
  uint8_t selected_ = 0;
  App* active_ = nullptr;
  bool homeDirty_ = true;
  void drawHome();
  void selectNext();
};
