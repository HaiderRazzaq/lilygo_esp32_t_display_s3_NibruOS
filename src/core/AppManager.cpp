#include "core/AppManager.h"
#include "ui/HomeScreen.h"
#include "BoardConfig.h"
#include "core/MemoryProfiler.h"
#include <Arduino.h>

AppManager::AppManager(AppContext& ctx) : ctx_(ctx) {}
bool AppManager::registerApp(App& app) {
  if (count_ >= kMaxApps) return false;
  apps_[count_++] = &app;
  return true;
}
void AppManager::begin() { goHome(); }
void AppManager::goHome() {
  if (active_) {
    {
      ProfileScope timer("app-exit");
      active_->onExit(ctx_);  // release app-owned resources before home rendering
    }
    active_ = nullptr;
    MemoryProfiler::checkpoint("after-exit");
  }
  armedIndex_ = 0xFF;
  armedContactId_ = 0;
  homeDirty_ = true;
}
bool AppManager::open(uint8_t index) {
  if (index >= count_) return false;
  if (active_) goHome();
  MemoryProfiler::checkpoint("before-entry");
  active_ = apps_[index];
  selected_ = index;
  armedIndex_ = 0xFF;
  armedContactId_ = 0;
  {
    ProfileScope timer("app-entry");
    active_->onEnter(ctx_);
  }
  MemoryProfiler::checkpoint("after-entry");
  return true;
}
void AppManager::selectNext() {
  if (count_) selected_ = static_cast<uint8_t>((selected_ + 1) % count_);
  armedIndex_ = 0xFF;
  armedContactId_ = 0;
  homeDirty_ = true;
}
void AppManager::handleButton(ButtonAction action) {
  armedIndex_ = 0xFF;
  armedContactId_ = 0;
  switch (action) {
    case ButtonAction::Back:
      if (active_ && active_->onBack(ctx_)) return;
      goHome();
      break;
    case ButtonAction::SelectNext:
      if (isHome()) selectNext();
      else active_->onPrimaryAction(ctx_);
      break;
    case ButtonAction::Activate:
      if (isHome()) open(selected_);
      else active_->onActivate(ctx_);
      break;
    default: break;
  }
}
void AppManager::handleTouch(const TouchEvent& ev) {
  if (active_) {
    // Header Back zone is reserved by the shell (all apps share it).
    if ((ev.gesture == TouchGesture::Tap || ev.gesture == TouchGesture::DoubleTap)
        && ev.y < 38 && ev.x < 55) {
      if (active_->onBack(ctx_)) return;
      goHome();
      return;
    }
    active_->onTouch(ctx_, ev);
    return;
  }
  if (ev.gesture == TouchGesture::SwipeLeft) {
    if (count_) {
      const uint8_t pages = static_cast<uint8_t>((count_ + 2) / 3);
      selected_ = static_cast<uint8_t>(((selected_ / 3 + 1) % pages) * 3);
    }
    armedIndex_ = 0xFF;
    armedContactId_ = 0;
    homeDirty_ = true; return;
  }
  if (ev.gesture == TouchGesture::SwipeRight) {
    if (count_) {
      const uint8_t pages = static_cast<uint8_t>((count_ + 2) / 3);
      selected_ = static_cast<uint8_t>(((selected_ / 3 + pages - 1) % pages) * 3);
    }
    armedIndex_ = 0xFF;
    armedContactId_ = 0;
    homeDirty_ = true; return;
  }
  if (ev.gesture == TouchGesture::SwipeUp) { selectNext(); return; }
  if (ev.gesture == TouchGesture::SwipeDown) {
    if (count_) selected_ = static_cast<uint8_t>((selected_ + count_ - 1) % count_);
    armedIndex_ = 0xFF;
    armedContactId_ = 0;
    homeDirty_ = true; return;
  }
  if (ev.gesture != TouchGesture::Tap && ev.gesture != TouchGesture::DoubleTap) return;
  if (ev.y >= 44 && ev.y < 146 && ev.x >= 8 && ev.x < 316) {
    int target = -1;
    if (count_ == 2) {
      target = (ev.x < 158) ? 0 : 1;
    } else {
      const int col = (ev.x - 8) / 102;
      const int idx = (selected_ / 3) * 3 + col;
      if (col >= 0 && col < 3 && idx < count_) target = idx;
    }
    if (target >= 0) {
      const uint8_t targetIdx = static_cast<uint8_t>(target);
      // Select-then-Confirm:
      // Touch 1: If card not armed, select and arm it, show prominent highlight.
      // Touch 2: If card already armed and contact is separate, confirm & open.
      // DoubleTap: Direct fast launch immediately.
      if (ev.gesture == TouchGesture::DoubleTap ||
          (armedIndex_ == targetIdx && ev.contactId != armedContactId_ && ev.contactId != 0)) {
        armedIndex_ = 0xFF;
        armedContactId_ = 0;
        open(targetIdx);
      } else {
        selected_ = targetIdx;
        armedIndex_ = targetIdx;
        armedContactId_ = ev.contactId;
        homeDirty_ = true;
      }
    }
  }
}
void AppManager::tick(uint32_t nowMs) {
  if (active_) {
    active_->onTick(ctx_, nowMs);
    if (active_->consumeDirty()) active_->render(ctx_);
  } else if (homeDirty_) {
    drawHome();
    homeDirty_ = false;
  }
}
void AppManager::drawHome() {
  HomeScreen::draw(ctx_, apps_, count_, selected_, armedIndex_ == selected_);
}
