#include "ui/HomeScreen.h"
#include "core/AppContext.h"
#include "core/App.h"
#include "hardware/DisplayManager.h"
#include "services/LocalizationService.h"
#include "services/ArabicReshaper.h"
#include "services/WiFiService.h"
#include "services/BleHidService.h"
#include "ui/Theme.h"
#include "BoardConfig.h"
#include <cstdio>
#include <cstring>

void HomeScreen::draw(AppContext& ctx, App* const* apps, uint8_t count, uint8_t selected, bool armed) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();
  screen.clear();
  screen.header(Board::kSystemTitle, false);
  const uint16_t accents[] = {Colors::Cyan, Colors::Green, Colors::Amber};
  const uint8_t page = selected / 3;
  const uint8_t pages = static_cast<uint8_t>((count + 2) / 3);

  // Live Status Bar Indicators in Header (x: 180..315)
  if (ctx.wifi.isConnected()) {
    t.fillRoundRect(182, 8, 36, 17, 3, Colors::Bg);
    t.drawRoundRect(182, 8, 36, 17, 3, Colors::Green);
    screen.label(186, 12, "WiFi", Colors::Green, 1, Colors::Bg);
  } else if (ctx.wifi.state() == ScanState::Scanning || ctx.wifi.state() == ScanState::Preparing) {
    t.fillRoundRect(182, 8, 36, 17, 3, Colors::Bg);
    t.drawRoundRect(182, 8, 36, 17, 3, Colors::Amber);
    screen.label(186, 12, "SCAN", Colors::Amber, 1, Colors::Bg);
  }

  if (ctx.bleHid.isConnected()) {
    t.fillRoundRect(222, 8, 30, 17, 3, Colors::Bg);
    t.drawRoundRect(222, 8, 30, 17, 3, Colors::Green);
    screen.label(226, 12, "BLE", Colors::Green, 1, Colors::Bg);
  } else if (ctx.bleHid.isEnabled()) {
    t.fillRoundRect(222, 8, 30, 17, 3, Colors::Bg);
    t.drawRoundRect(222, 8, 30, 17, 3, Colors::Cyan);
    screen.label(226, 12, "BLE", Colors::Cyan, 1, Colors::Bg);
  }

  // Modern Page Counter Pill
  char pageLabel[16];
  snprintf(pageLabel, sizeof(pageLabel), "%u/%u", page + 1, pages ? pages : 1);
  t.fillRoundRect(256, 8, 56, 17, 3, Colors::Bg);
  t.drawRoundRect(256, 8, 56, 17, 3, Colors::Border);
  screen.label(270, 12, pageLabel, Colors::Muted, 1, Colors::Bg);

  if (count == 2) {
    // 2-card balanced landscape layout
    for (uint8_t i = 0; i < 2; ++i) {
      const int x = 14 + i * 150;
      const uint16_t accent = accents[i % 3];
      t.fillRoundRect(x, 46, 142, 94, 8, Colors::Panel);
      t.drawRoundRect(x, 46, 142, 94, 8, i == selected ? accent : Colors::Border);
      if (i == selected && armed) {
        t.drawRoundRect(x + 1, 47, 140, 92, 7, accent);
        t.fillRect(x + 20, 136, 102, 2, accent);
      }

      // Centered short icon badge
      t.fillRoundRect(x + 51, 56, 40, 26, 4, accent);
      const char* shortT = apps[i]->shortTitle();
      const int shortW = static_cast<int>(strlen(shortT)) * 6;
      screen.label(x + 51 + (40 - shortW) / 2, 65, shortT, Colors::Bg, 1, accent);

      // Centered title with overflow prevention
      const char* title = apps[i]->title();
      char safeTitle[32]{};
      strncpy(safeTitle, title, sizeof(safeTitle) - 1);
      int titleW = 0;
      if (ArabicReshaper::hasArabic(safeTitle)) {
        titleW = static_cast<int>(strlen(safeTitle)) * 4;
      } else {
        titleW = static_cast<int>(strlen(safeTitle)) * 6;
      }
      const int titleX = x + (142 - titleW) / 2;
      screen.label(titleX > x + 4 ? titleX : x + 4, 90, safeTitle, Colors::White, 1, Colors::Panel);

      // Centered subtitle
      const char* sub = apps[i]->subtitle();
      int subW = 0;
      if (ArabicReshaper::hasArabic(sub)) {
        subW = static_cast<int>(strlen(sub)) * 4;
      } else {
        subW = static_cast<int>(strlen(sub)) * 6;
      }
      const int subX = x + (142 - subW) / 2;
      screen.label(subX > x + 4 ? subX : x + 4, 116, sub, Colors::Muted, 1, Colors::Panel);
    }
  } else {
    // 3-card paginated grid
    for (uint8_t col = 0; col < 3; ++col) {
      const uint8_t i = static_cast<uint8_t>(page * 3 + col);
      if (i >= count) break;
      const int x = 8 + col * 102;
      const uint16_t accent = i == selected ? screen.themeColor() : accents[col % 3];
      t.fillRoundRect(x, 46, 94, 93, 8, Colors::Panel);
      t.drawRoundRect(x, 46, 94, 93, 8, i == selected ? accent : Colors::Border);
      if (i == selected && armed) {
        t.drawRoundRect(x + 1, 47, 92, 91, 7, accent);
        t.fillRect(x + 14, 134, 66, 2, accent);
      }

      // Centered badge with overflow guard
      t.fillRoundRect(x + 29, 54, 36, 24, 4, accent);
      char safeShort[16]{};
      strncpy(safeShort, apps[i]->shortTitle(), sizeof(safeShort) - 1);
      const int shortW = static_cast<int>(strlen(safeShort)) * 6;
      screen.label(x + 29 + (36 - shortW) / 2, 62, safeShort, Colors::Bg, 1, accent);

      // Centered title with overflow guard
      char safeTitle[32]{};
      strncpy(safeTitle, apps[i]->title(), sizeof(safeTitle) - 1);
      int titleW = 0;
      if (ArabicReshaper::hasArabic(safeTitle)) {
        titleW = static_cast<int>(strlen(safeTitle)) * 4;
      } else {
        titleW = static_cast<int>(strlen(safeTitle)) * 6;
      }
      const int titleX = x + (94 - titleW) / 2;
      screen.label(titleX > x + 3 ? titleX : x + 3, 88, safeTitle, Colors::White, 1, Colors::Panel);

      // Centered subtitle
      const char* sub = apps[i]->subtitle();
      int subW = 0;
      if (ArabicReshaper::hasArabic(sub)) {
        subW = static_cast<int>(strlen(sub)) * 4;
      } else {
        subW = static_cast<int>(strlen(sub)) * 6;
      }
      const int subX = x + (94 - subW) / 2;
      screen.label(subX > x + 3 ? subX : x + 3, 112, sub, Colors::Muted, 1, Colors::Panel);
    }
  }

  // Modern Pagination Dots Indicator (Centered below cards at y=143)
  if (pages > 1) {
    const int dotStartX = (320 - pages * 16) / 2;
    for (uint8_t p = 0; p < pages; ++p) {
      if (p == page) {
        t.fillRoundRect(dotStartX + p * 16, 143, 12, 4, 2, screen.themeColor());
      } else {
        t.fillRoundRect(dotStartX + p * 16 + 4, 143, 4, 4, 2, Colors::Border);
      }
    }
    // Interactive Side Chevrons as swipe navigation cues
    if (page > 0) {
      screen.label(2, 88, "<", screen.themeColor(), 1, Colors::Bg);
    }
    if (page + 1 < pages) {
      screen.label(313, 88, ">", screen.themeColor(), 1, Colors::Bg);
    }
  }

  const auto& loc = LocalizationService::instance();
  if (armed) {
    screen.footer(loc.isArabic() ? "المس مجدداً أو BOOT للفتح | اسحب" : "Tap again or BOOT to open | Swipe");
  } else {
    screen.footer(loc.isArabic() ? "المس للتحديد أو انقر مرتين للفتح" : "Tap to select | Double-tap to open");
  }
}
