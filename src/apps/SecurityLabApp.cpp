#include "apps/SecurityLabApp.h"
#include "apps/BleLabApp.h"
#include "apps/BeaconLabApp.h"
#include "apps/WifiDefenseApp.h"
#include "apps/WiFiChannelsApp.h"
#include "core/AppContext.h"
#include "hardware/DisplayManager.h"
#include "services/LocalizationService.h"
#include "ui/Theme.h"
#include <Arduino.h>

SecurityLabApp::SecurityLabApp(BleLabApp& bleLab, BeaconLabApp& beaconLab,
                               WifiDefenseApp& wifiDefense, WiFiChannelsApp& wifiChannels)
    : bleLab_(bleLab), beaconLab_(beaconLab), wifiDefense_(wifiDefense), wifiChannels_(wifiChannels) {
  tools_[0] = &bleLab_;
  tools_[1] = &beaconLab_;
  tools_[2] = &wifiDefense_;
  tools_[3] = &wifiChannels_;
}

void SecurityLabApp::onEnter(AppContext& ctx) {
  if (activeTool_) {
    activeTool_->onExit(ctx);
    activeTool_ = nullptr;
  }
  fullRedraw_ = true;
  invalidate();
}

void SecurityLabApp::onExit(AppContext& ctx) {
  closeTool(ctx);
  fullRedraw_ = true;
}

void SecurityLabApp::openTool(uint8_t index, AppContext& ctx) {
  if (index >= 4 || !tools_[index]) return;
  activeTool_ = tools_[index];
  activeTool_->onEnter(ctx);
  activeTool_->invalidate();
  activeTool_->render(ctx);
}

void SecurityLabApp::closeTool(AppContext& ctx) {
  if (activeTool_) {
    activeTool_->onExit(ctx);
    activeTool_ = nullptr;
  }
  fullRedraw_ = true;
  invalidate();
}

void SecurityLabApp::onTick(AppContext& ctx, uint32_t nowMs) {
  if (activeTool_) {
    activeTool_->onTick(ctx, nowMs);
    if (activeTool_->consumeDirty()) {
      activeTool_->render(ctx);
    }
  }
}

void SecurityLabApp::onTouch(AppContext& ctx, const TouchEvent& touch) {
  if (activeTool_) {
    activeTool_->onTouch(ctx, touch);
    return;
  }

  // Handle Menu Card Taps:
  if (touch.gesture == TouchGesture::Tap || touch.gesture == TouchGesture::Unknown) {
    // Card 0: Col 0, Row 0: x: 10..155, y: 36..92
    if (touch.x >= 10 && touch.x <= 155 && touch.y >= 36 && touch.y <= 92) {
      selectedIndex_ = 0;
      openTool(0, ctx);
      return;
    }
    // Card 1: Col 1, Row 0: x: 165..310, y: 36..92
    if (touch.x >= 165 && touch.x <= 310 && touch.y >= 36 && touch.y <= 92) {
      selectedIndex_ = 1;
      openTool(1, ctx);
      return;
    }
    // Card 2: Col 0, Row 1: x: 10..155, y: 98..154
    if (touch.x >= 10 && touch.x <= 155 && touch.y >= 98 && touch.y <= 154) {
      selectedIndex_ = 2;
      openTool(2, ctx);
      return;
    }
    // Card 3: Col 1, Row 1: x: 165..310, y: 98..154
    if (touch.x >= 165 && touch.x <= 310 && touch.y >= 98 && touch.y <= 154) {
      selectedIndex_ = 3;
      openTool(3, ctx);
      return;
    }
  }

  // Gestures for cycling selection
  if (touch.gesture == TouchGesture::SwipeRight || touch.gesture == TouchGesture::SwipeDown) {
    selectedIndex_ = static_cast<uint8_t>((selectedIndex_ + 1) % 4);
    invalidate();
    return;
  }
  if (touch.gesture == TouchGesture::SwipeLeft || touch.gesture == TouchGesture::SwipeUp) {
    selectedIndex_ = static_cast<uint8_t>((selectedIndex_ + 3) % 4);
    invalidate();
    return;
  }
}

void SecurityLabApp::onPrimaryAction(AppContext& ctx) {
  if (activeTool_) {
    activeTool_->onPrimaryAction(ctx);
  } else {
    selectedIndex_ = static_cast<uint8_t>((selectedIndex_ + 1) % 4);
    invalidate();
  }
}

void SecurityLabApp::onActivate(AppContext& ctx) {
  if (activeTool_) {
    activeTool_->onActivate(ctx);
  } else {
    openTool(selectedIndex_, ctx);
  }
}

bool SecurityLabApp::onBack(AppContext& ctx) {
  if (activeTool_) {
    // If sub-tool has an internal modal/picker (e.g. BleLab target picker), let it handle it
    if (activeTool_->onBack(ctx)) {
      return true;
    }
    // Otherwise, close the tool and return to SecurityLabApp menu
    closeTool(ctx);
    return true; // stay in SecurityLabApp
  }
  // At menu level: return false so AppManager exits to Home screen
  return false;
}

void SecurityLabApp::onDisplayRestored(AppContext& ctx) {
  if (activeTool_) {
    activeTool_->onDisplayRestored(ctx);
  } else {
    fullRedraw_ = true;
    invalidate();
  }
}

void SecurityLabApp::drawMenu(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();

  if (fullRedraw_) {
    screen.clear();
    screen.header("SECURITY LAB", true);
    fullRedraw_ = false;
  }

  struct ToolDef {
    const char* name;
    const char* sub;
    const char* badge;
    uint16_t badgeColor;
  };

  const ToolDef kTools[4] = {
    {"BLE Lab", "Adv & Pairing Demo", "BLE", Colors::Cyan},
    {"Beacon Lab", "Wi-Fi Beacon Flood", "WIFI", Colors::Amber},
    {"WiFi Defense", "Deauth & IDS Monitor", "IDS", Colors::Green},
    {"WiFi Channels", "2.4GHz Spectrum", "SPEC", Colors::Magenta}
  };

  const int16_t xs[4] = {10, 165, 10, 165};
  const int16_t ys[4] = {36, 36, 98, 98};
  constexpr int16_t kW = 145;
  constexpr int16_t kH = 56;

  for (uint8_t i = 0; i < 4; ++i) {
    const bool isSel = (i == selectedIndex_);
    const int16_t x = xs[i];
    const int16_t y = ys[i];

    // Card background & border
    t.fillRoundRect(x, y, kW, kH, 5, isSel ? Colors::Panel : Colors::Bg);
    t.drawRoundRect(x, y, kW, kH, 5, isSel ? Colors::Cyan : Colors::Border);

    // Title
    screen.label(x + 8, y + 8, kTools[i].name, isSel ? Colors::White : Colors::Cyan, 1, isSel ? Colors::Panel : Colors::Bg);

    // Badge
    t.fillRoundRect(x + kW - 42, y + 6, 36, 15, 3, Colors::Bg);
    t.drawRoundRect(x + kW - 42, y + 6, 36, 15, 3, kTools[i].badgeColor);
    screen.label(x + kW - 38, y + 8, kTools[i].badge, kTools[i].badgeColor, 1, Colors::Bg);

    // Subtitle
    screen.label(x + 8, y + 30, kTools[i].sub, Colors::Muted, 1, isSel ? Colors::Panel : Colors::Bg);
  }

  // Footer prompt
  const bool ar = LocalizationService::instance().isArabic();
  screen.footer(ar ? "اختر أداة | زر BOOT للتنقل" : "Select tool | BOOT: Next | Tap: Launch");
}

void SecurityLabApp::render(AppContext& ctx) {
  if (activeTool_) {
    activeTool_->render(ctx);
  } else {
    drawMenu(ctx);
  }
}
