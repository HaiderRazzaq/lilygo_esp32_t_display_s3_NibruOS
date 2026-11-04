#include "apps/ClockApp.h"
#include "core/AppContext.h"
#include "hardware/DisplayManager.h"
#include "services/TimeService.h"
#include "services/SettingsService.h"
#include "ui/Theme.h"
#include <Arduino.h>
#include <cstdio>
#include <cstring>

void ClockApp::onEnter(AppContext& ctx) {
  (void)ctx;
  lastEpoch_ = 0;
  fullRedraw_ = true;
  invalidate();
}

void ClockApp::onExit(AppContext& ctx) {
  (void)ctx;
  lastEpoch_ = 0;
}

void ClockApp::onTick(AppContext& ctx, uint32_t nowMs) {
  (void)nowMs;
  const uint32_t ep = ctx.time.getEpoch();
  if (ep != lastEpoch_) {
    lastEpoch_ = ep;
    invalidate();
  }
}

bool ClockApp::onBack(AppContext& ctx) {
  (void)ctx;
  return false; // return to Home launcher
}

void ClockApp::onTouch(AppContext& ctx, const TouchEvent& ev) {
  if (ev.gesture != TouchGesture::Tap) return;

  // Header Back button touch
  if (ev.x < 45 && ev.y < 35) {
    onBack(ctx);
    return;
  }

  // Tapping Screensaver toggle pill: x: 160..300, y: 114..138
  if (ev.x >= 160 && ev.x <= 300 && ev.y >= 114 && ev.y <= 138) {
    const bool newSaver = !ctx.settings.clockScreensaver();
    ctx.settings.setClockScreensaver(newSaver);
    ctx.display.setScreensaverEnabled(newSaver);
    fullRedraw_ = true;
    invalidate();
    return;
  }

  // Tapping card cycles accent theme
  themeIdx_ = static_cast<uint8_t>((themeIdx_ + 1) % 4);
  fullRedraw_ = true;
  invalidate();
}

void ClockApp::render(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();

  const uint16_t accents[] = {Colors::Cyan, Colors::Amber, Colors::Green, Colors::Magenta};
  const uint16_t accent = accents[themeIdx_ % 4];

  if (fullRedraw_) {
    screen.clear();
    screen.header(title(), true);
    fullRedraw_ = false;
  }

  // Main Cyber Clock Frame
  t.fillRoundRect(12, 36, 296, 122, 10, Colors::Panel);
  t.drawRoundRect(12, 36, 296, 122, 10, accent);

  // Time & Date strings from TimeService
  char timeStr[16]{};
  char dateStr[16]{};
  ctx.time.formatTime(timeStr, sizeof(timeStr));
  ctx.time.formatDate(dateStr, sizeof(dateStr));

  // Big digital time centered (Size 4 = 24px wide per char, 8 chars = 192px width)
  const int timeW = static_cast<int>(strlen(timeStr)) * 24;
  const int timeX = 12 + (296 - timeW) / 2;
  screen.label(timeX, 50, timeStr, accent, 4, Colors::Panel);

  // Date and Timezone label
  char subLabel[40]{};
  snprintf(subLabel, sizeof(subLabel), "%s  |  UTC%+d", dateStr, ctx.time.timezoneOffset());
  const int subW = static_cast<int>(strlen(subLabel)) * 6;
  const int subX = 12 + (296 - subW) / 2;
  screen.label(subX, 94, subLabel, Colors::White, 1, Colors::Panel);

  // Dual Badges on y: 116
  // Left: Sync state pill badge
  const bool synced = ctx.time.isSynced();
  const char* syncTxt = synced ? "NTP SYNCED" : "LOCAL RTC";
  const uint16_t syncColor = synced ? Colors::Green : Colors::Amber;
  t.fillRoundRect(24, 116, 120, 18, 4, syncColor);
  const int sOff = (120 - static_cast<int>(strlen(syncTxt)) * 6) / 2;
  screen.label(24 + sOff, 121, syncTxt, Colors::Bg, 1, syncColor);

  // Right: Screensaver Mode Toggle Pill
  const bool saverOn = ctx.settings.clockScreensaver();
  const char* saverTxt = saverOn ? "SAVER: ON" : "SAVER: OFF";
  const uint16_t saverBg = saverOn ? Colors::Cyan : Colors::Bg;
  const uint16_t saverTc = saverOn ? Colors::Bg : Colors::Muted;
  t.fillRoundRect(176, 116, 120, 18, 4, saverBg);
  t.drawRoundRect(176, 116, 120, 18, 4, saverOn ? Colors::Cyan : Colors::Border);
  const int rOff = (120 - static_cast<int>(strlen(saverTxt)) * 6) / 2;
  screen.label(176 + rOff, 121, saverTxt, saverTc, 1, saverBg);

  // Animated seconds progress bar across bottom of panel
  const uint32_t ep = ctx.time.getEpoch();
  const int sec = static_cast<int>(ep % 60);
  t.fillRect(24, 146, 272, 3, Colors::Bg);
  const int barW = (sec * 272) / 59;
  if (barW > 0) {
    t.fillRect(24, 146, barW, 3, accent);
  }
}

void ClockApp::renderScreensaver(AppContext& ctx, uint32_t nowMs) {
  (void)nowMs;
  const uint32_t ep = ctx.time.getEpoch();
  if (ep == lastScreensaverEpoch_) return;
  lastScreensaverEpoch_ = ep;

  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();

  const uint16_t accents[] = {Colors::Cyan, Colors::Amber, Colors::Green, Colors::Magenta};
  const uint16_t accent = accents[themeIdx_ % 4];

  // Fullscreen cyber frame
  t.fillRoundRect(8, 6, 304, 158, 12, Colors::Bg);
  t.drawRoundRect(8, 6, 304, 158, 12, accent);
  t.drawRoundRect(10, 8, 300, 154, 10, Colors::Panel);

  // Top header pill: SCREENSAVER
  t.fillRoundRect(80, 14, 160, 16, 4, Colors::Panel);
  screen.label(95, 18, "CLOCK SCREENSAVER", Colors::Cyan, 1, Colors::Panel);

  // Big digital time centered (Size 4 = 24px wide per char, 8 chars = 192px width)
  char timeStr[16]{};
  char dateStr[16]{};
  ctx.time.formatTime(timeStr, sizeof(timeStr));
  ctx.time.formatDate(dateStr, sizeof(dateStr));

  const int timeW = static_cast<int>(strlen(timeStr)) * 24;
  const int timeX = (320 - timeW) / 2;
  screen.label(timeX, 42, timeStr, accent, 4, Colors::Bg);

  // Date and Timezone label
  char subLabel[40]{};
  snprintf(subLabel, sizeof(subLabel), "%s  |  UTC%+d", dateStr, ctx.time.timezoneOffset());
  const int subW = static_cast<int>(strlen(subLabel)) * 6;
  const int subX = (320 - subW) / 2;
  screen.label(subX, 86, subLabel, Colors::White, 1, Colors::Bg);

  // NTP / RTC status + Tap to wake badge
  const bool synced = ctx.time.isSynced();
  char infoStr[48]{};
  snprintf(infoStr, sizeof(infoStr), "[ %s  *  TAP TO WAKE ]", synced ? "NTP SYNCED" : "RTC ACTIVE");
  const int infoW = static_cast<int>(strlen(infoStr)) * 6;
  const int infoX = (320 - infoW) / 2;
  screen.label(infoX, 108, infoStr, Colors::Muted, 1, Colors::Bg);

  // Animated seconds progress bar across bottom of panel
  const int sec = static_cast<int>(ep % 60);
  t.fillRect(20, 134, 280, 4, Colors::Panel);
  const int barW = (sec * 280) / 59;
  if (barW > 0) {
    t.fillRect(20, 134, barW, 4, accent);
  }
}
