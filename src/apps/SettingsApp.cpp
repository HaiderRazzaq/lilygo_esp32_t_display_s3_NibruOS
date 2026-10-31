#include "apps/SettingsApp.h"
#include "core/AppContext.h"
#include "hardware/DisplayManager.h"
#include <Arduino.h>

void SettingsApp::onEnter(AppContext&) {
  subView_ = SettingsSubView::Menu;
  invalidate();
}

void SettingsApp::onExit(AppContext& ctx) {
  if (subView_ == SettingsSubView::SystemInfo) {
    systemInfo_.onExit(ctx);
  } else if (subView_ == SettingsSubView::TouchTest) {
    touchTest_.onExit(ctx);
  }
  subView_ = SettingsSubView::Menu;
}

void SettingsApp::onTick(AppContext& ctx, uint32_t nowMs) {
  if (subView_ == SettingsSubView::SystemInfo) {
    systemInfo_.onTick(ctx, nowMs);
    if (systemInfo_.consumeDirty()) systemInfo_.render(ctx);
  } else if (subView_ == SettingsSubView::TouchTest) {
    touchTest_.onTick(ctx, nowMs);
    if (touchTest_.consumeDirty()) touchTest_.render(ctx);
  }
}

bool SettingsApp::onBack(AppContext& ctx) {
  if (subView_ != SettingsSubView::Menu) {
    if (subView_ == SettingsSubView::SystemInfo) {
      systemInfo_.onExit(ctx);
    } else if (subView_ == SettingsSubView::TouchTest) {
      touchTest_.onExit(ctx);
    }
    subView_ = SettingsSubView::Menu;
    invalidate();
    return true;  // Handled internally; remain inside Settings
  }
  return false;  // At top-level Settings menu; let shell go Home
}

void SettingsApp::onPrimaryAction(AppContext& ctx) {
  if (subView_ == SettingsSubView::SystemInfo) {
    systemInfo_.onPrimaryAction(ctx);
  } else if (subView_ == SettingsSubView::TouchTest) {
    touchTest_.onPrimaryAction(ctx);
  }
}

void SettingsApp::onTouch(AppContext& ctx, const TouchEvent& ev) {
  if (subView_ == SettingsSubView::Menu) {
    if (ev.gesture != TouchGesture::Tap && ev.gesture != TouchGesture::DoubleTap) return;

    // Item 0: System Info (Y: 40..88, X: 10..310)
    if (ev.y >= 40 && ev.y <= 88 && ev.x >= 10 && ev.x <= 310) {
      subView_ = SettingsSubView::SystemInfo;
      systemInfo_.onEnter(ctx);
      systemInfo_.render(ctx);
      return;
    }

    // Item 1: Touch Test (Y: 94..142, X: 10..310)
    if (ev.y >= 92 && ev.y <= 142 && ev.x >= 10 && ev.x <= 310) {
      subView_ = SettingsSubView::TouchTest;
      touchTest_.onEnter(ctx);
      touchTest_.render(ctx);
      return;
    }
  } else if (subView_ == SettingsSubView::SystemInfo) {
    // Header Back zone tap returns to Settings Menu
    if ((ev.gesture == TouchGesture::Tap || ev.gesture == TouchGesture::DoubleTap) &&
        ev.x < 45 && ev.y < 35) {
      onBack(ctx);
      return;
    }
    systemInfo_.onTouch(ctx, ev);
  } else if (subView_ == SettingsSubView::TouchTest) {
    // Header Back zone tap returns to Settings Menu
    if ((ev.gesture == TouchGesture::Tap || ev.gesture == TouchGesture::DoubleTap) &&
        ev.x < 45 && ev.y < 35) {
      onBack(ctx);
      return;
    }
    touchTest_.onTouch(ctx, ev);
  }
}

void SettingsApp::render(AppContext& ctx) {
  if (subView_ == SettingsSubView::Menu) {
    renderMenu(ctx);
  } else if (subView_ == SettingsSubView::SystemInfo) {
    systemInfo_.render(ctx);
  } else if (subView_ == SettingsSubView::TouchTest) {
    touchTest_.render(ctx);
  }
}

void SettingsApp::renderMenu(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();
  screen.clear();
  screen.header("Settings", true);

  // Item 0: System Info card (Y: 42, H: 46)
  t.fillRoundRect(10, 42, 300, 46, 6, Colors::Panel);
  t.drawRoundRect(10, 42, 300, 46, 6, Colors::Border);
  t.fillRoundRect(18, 48, 36, 34, 4, Colors::Cyan);
  screen.label(24, 60, "SYS", Colors::Bg, 1, Colors::Cyan);
  screen.label(64, 50, "System Info", Colors::White, 2, Colors::Panel);
  screen.label(64, 71, "Chip, IP, MAC & Memory", Colors::Muted, 1, Colors::Panel);
  screen.label(292, 58, ">", Colors::Cyan, 2, Colors::Panel);

  // Item 1: Touch Test card (Y: 96, H: 46)
  t.fillRoundRect(10, 96, 300, 46, 6, Colors::Panel);
  t.drawRoundRect(10, 96, 300, 46, 6, Colors::Border);
  t.fillRoundRect(18, 102, 36, 34, 4, Colors::Green);
  screen.label(24, 114, "TAP", Colors::Bg, 1, Colors::Green);
  screen.label(64, 104, "Touch Test", Colors::White, 2, Colors::Panel);
  screen.label(64, 125, "Screen & Gesture Diagnostics", Colors::Muted, 1, Colors::Panel);
  screen.label(292, 112, ">", Colors::Green, 2, Colors::Panel);

  screen.footer("Tap item to open  |  GPIO14 Back");
}

