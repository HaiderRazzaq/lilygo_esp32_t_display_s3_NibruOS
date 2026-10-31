#include "core/AppManager.h"
#include "hardware/DisplayManager.h"
#include <Arduino.h>
#include <cstdio>

AppManager::AppManager(AppContext& ctx) : ctx_(ctx) {}
bool AppManager::registerApp(App& app) {
  if (count_ >= kMaxApps) return false;
  apps_[count_++] = &app;
  return true;
}
void AppManager::begin() { goHome(); }
void AppManager::goHome() {
  if (active_) {
    active_->onExit(ctx_);  // release app-owned resources before home rendering
    active_ = nullptr;
  }
  homeDirty_ = true;
}
bool AppManager::open(uint8_t index) {
  if (index >= count_) return false;
  if (active_) active_->onExit(ctx_);
  active_ = apps_[index];
  selected_ = index;
  active_->onEnter(ctx_);
  return true;
}
void AppManager::selectNext() {
  if (count_) selected_ = static_cast<uint8_t>((selected_ + 1) % count_);
  homeDirty_ = true;
}
void AppManager::handleButton(ButtonAction action) {
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
      else active_->onPrimaryAction(ctx_);
      break;
    default: break;
  }
}
void AppManager::handleTouch(const TouchEvent& ev) {
  if (active_) {
    // Header Back zone is reserved by the shell (all apps share it).
    if ((ev.gesture == TouchGesture::Tap || ev.gesture == TouchGesture::DoubleTap)
        && ev.y < 33 && ev.x < 40) {
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
    homeDirty_ = true; return;
  }
  if (ev.gesture == TouchGesture::SwipeRight) {
    if (count_) {
      const uint8_t pages = static_cast<uint8_t>((count_ + 2) / 3);
      selected_ = static_cast<uint8_t>(((selected_ / 3 + pages - 1) % pages) * 3);
    }
    homeDirty_ = true; return;
  }
  if (ev.gesture == TouchGesture::SwipeUp) { selectNext(); return; }
  if (ev.gesture == TouchGesture::SwipeDown) {
    if (count_) selected_ = static_cast<uint8_t>((selected_ + count_ - 1) % count_);
    homeDirty_ = true; return;
  }
  if (ev.gesture != TouchGesture::Tap && ev.gesture != TouchGesture::DoubleTap) return;
  if (ev.y >= 44 && ev.y < 146 && ev.x >= 8 && ev.x < 316) {
    if (count_ == 2) {
      if (ev.x < 158) open(0);
      else open(1);
      return;
    }
    const int col = (ev.x - 8) / 102;
    const int index = (selected_ / 3) * 3 + col;
    if (col >= 0 && col < 3 && index < count_) open(static_cast<uint8_t>(index));
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
  DisplayManager& screen = ctx_.display;
  TFT_eSPI& t = screen.gfx();
  screen.clear();
  screen.header("NibruOS v0.1", false);
  const uint16_t accents[] = {Colors::Cyan, Colors::Green, Colors::Amber};
  const uint8_t page = selected_ / 3;
  const uint8_t pages = static_cast<uint8_t>((count_ + 2) / 3);
  char pageLabel[16];
  snprintf(pageLabel, sizeof(pageLabel), "%u/%u", page + 1, pages ? pages : 1);
  screen.label(277, 14, pageLabel, Colors::Muted, 1, Colors::Panel);

  if (count_ == 2) {
    // 2-card balanced landscape layout
    for (uint8_t i = 0; i < 2; ++i) {
      const int x = 14 + i * 150;
      const uint16_t accent = accents[i % 3];
      t.fillRoundRect(x, 46, 142, 96, 8, Colors::Panel);
      t.drawRoundRect(x, 46, 142, 96, 8, i == selected_ ? accent : Colors::Border);

      // Centered short icon badge
      t.fillRoundRect(x + 51, 56, 40, 26, 4, accent);
      const char* shortT = apps_[i]->shortTitle();
      const int shortW = static_cast<int>(strlen(shortT)) * 6;
      screen.label(x + 51 + (40 - shortW) / 2, 65, shortT, Colors::Bg, 1, accent);

      // Centered title with overflow prevention
      const char* title = apps_[i]->title();
      const int titleLen = static_cast<int>(strlen(title));
      uint8_t tSize = 2;
      int titleW = titleLen * 12;
      if (titleW > 124) {
        tSize = 1;
        titleW = titleLen * 6;
      }
      const int titleX = x + (142 - titleW) / 2;
      screen.label(titleX > x + 4 ? titleX : x + 4, 92, title, Colors::White, tSize, Colors::Panel);

      // Centered subtitle
      const char* sub = apps_[i]->subtitle();
      const int subW = static_cast<int>(strlen(sub)) * 6;
      const int subX = x + (142 - subW) / 2;
      screen.label(subX > x + 4 ? subX : x + 4, 118, sub, Colors::Muted, 1, Colors::Panel);
    }
  } else {
    // 3-card paginated grid
    for (uint8_t col = 0; col < 3; ++col) {
      const uint8_t i = static_cast<uint8_t>(page * 3 + col);
      if (i >= count_) break;
      const int x = 8 + col * 102;
      const uint16_t accent = accents[col % 3];
      t.fillRoundRect(x, 48, 94, 93, 9, Colors::Panel);
      t.drawRoundRect(x, 48, 94, 93, 9, i == selected_ ? accent : Colors::Border);

      // Centered badge
      t.fillRoundRect(x + 29, 58, 36, 26, 4, accent);
      const char* shortT = apps_[i]->shortTitle();
      const int shortW = static_cast<int>(strlen(shortT)) * 6;
      screen.label(x + 29 + (36 - shortW) / 2, 67, shortT, Colors::Bg, 1, accent);

      // Centered title
      const char* title = apps_[i]->title();
      const int titleW = static_cast<int>(strlen(title)) * 6;
      const int titleX = x + (94 - titleW) / 2;
      screen.label(titleX > x + 3 ? titleX : x + 3, 94, title, Colors::White, 1, Colors::Panel);

      // Centered subtitle
      const char* sub = apps_[i]->subtitle();
      const int subW = static_cast<int>(strlen(sub)) * 6;
      const int subX = x + (94 - subW) / 2;
      screen.label(subX > x + 3 ? subX : x + 3, 117, sub, Colors::Muted, 1, Colors::Panel);
    }
  }
  screen.footer("Tap app | BOOT next/hold open");
}
