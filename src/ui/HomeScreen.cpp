#include "ui/HomeScreen.h"
#include "core/AppContext.h"
#include "core/App.h"
#include "hardware/DisplayManager.h"
#include "ui/Theme.h"
#include "BoardConfig.h"
#include <cstdio>
#include <cstring>

void HomeScreen::draw(AppContext& ctx, App* const* apps, uint8_t count, uint8_t selected) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();
  screen.clear();
  screen.header(Board::kSystemTitle, false);
  const uint16_t accents[] = {Colors::Cyan, Colors::Green, Colors::Amber};
  const uint8_t page = selected / 3;
  const uint8_t pages = static_cast<uint8_t>((count + 2) / 3);
  char pageLabel[16];
  snprintf(pageLabel, sizeof(pageLabel), "%u/%u", page + 1, pages ? pages : 1);
  screen.label(277, 14, pageLabel, Colors::Muted, 1, Colors::Panel);

  if (count == 2) {
    // 2-card balanced landscape layout
    for (uint8_t i = 0; i < 2; ++i) {
      const int x = 14 + i * 150;
      const uint16_t accent = accents[i % 3];
      t.fillRoundRect(x, 46, 142, 96, 8, Colors::Panel);
      t.drawRoundRect(x, 46, 142, 96, 8, i == selected ? accent : Colors::Border);

      // Centered short icon badge
      t.fillRoundRect(x + 51, 56, 40, 26, 4, accent);
      const char* shortT = apps[i]->shortTitle();
      const int shortW = static_cast<int>(strlen(shortT)) * 6;
      screen.label(x + 51 + (40 - shortW) / 2, 65, shortT, Colors::Bg, 1, accent);

      // Centered title with overflow prevention
      const char* title = apps[i]->title();
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
      const char* sub = apps[i]->subtitle();
      const int subW = static_cast<int>(strlen(sub)) * 6;
      const int subX = x + (142 - subW) / 2;
      screen.label(subX > x + 4 ? subX : x + 4, 118, sub, Colors::Muted, 1, Colors::Panel);
    }
  } else {
    // 3-card paginated grid
    for (uint8_t col = 0; col < 3; ++col) {
      const uint8_t i = static_cast<uint8_t>(page * 3 + col);
      if (i >= count) break;
      const int x = 8 + col * 102;
      const uint16_t accent = i == selected ? screen.themeColor() : accents[col % 3];
      t.fillRoundRect(x, 48, 94, 93, 9, Colors::Panel);
      t.drawRoundRect(x, 48, 94, 93, 9, i == selected ? accent : Colors::Border);

      // Centered badge with overflow guard
      t.fillRoundRect(x + 29, 58, 36, 26, 4, accent);
      char safeShort[6]{};
      strncpy(safeShort, apps[i]->shortTitle(), sizeof(safeShort) - 1);
      const int shortW = static_cast<int>(strlen(safeShort)) * 6;
      screen.label(x + 29 + (36 - shortW) / 2, 67, safeShort, Colors::Bg, 1, accent);

      // Centered title with overflow guard
      char safeTitle[15]{};
      strncpy(safeTitle, apps[i]->title(), sizeof(safeTitle) - 1);
      const int titleW = static_cast<int>(strlen(safeTitle)) * 6;
      const int titleX = x + (94 - titleW) / 2;
      screen.label(titleX > x + 3 ? titleX : x + 3, 94, safeTitle, Colors::White, 1, Colors::Panel);

      // Centered subtitle
      const char* sub = apps[i]->subtitle();
      const int subW = static_cast<int>(strlen(sub)) * 6;
      const int subX = x + (94 - subW) / 2;
      screen.label(subX > x + 3 ? subX : x + 3, 117, sub, Colors::Muted, 1, Colors::Panel);
    }
  }
  screen.footer("Tap app | BOOT next/hold open");
}
