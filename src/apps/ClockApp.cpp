#include "apps/ClockApp.h"
#include "core/AppContext.h"
#include "hardware/DisplayManager.h"
#include "services/TimeService.h"
#include "services/SettingsService.h"
#include "services/WebPortalService.h"
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

  const uint32_t ep = ctx.time.getEpoch();
  const bool synced = ctx.time.isSynced();
  const bool saverOn = ctx.settings.clockScreensaver();
  const uint32_t curDay = ep / 86400;

  if (fullRedraw_) {
    screen.clear();
    screen.header(title(), true);

    // Main Cyber Clock Frame
    t.fillRoundRect(12, 36, 296, 122, 10, Colors::Panel);
    t.drawRoundRect(12, 36, 296, 122, 10, accent);

    // Date and Timezone label
    char dateStr[16]{};
    ctx.time.formatDate(dateStr, sizeof(dateStr));
    char subLabel[40]{};
    snprintf(subLabel, sizeof(subLabel), "%s  |  UTC%+d", dateStr, ctx.time.timezoneOffset());
    const int subW = static_cast<int>(strlen(subLabel)) * 6;
    const int subX = 12 + (296 - subW) / 2;
    screen.label(subX, 94, subLabel, Colors::White, 1, Colors::Panel);

    // Left: Sync state pill badge
    const char* syncTxt = synced ? "NTP SYNCED" : "LOCAL RTC";
    const uint16_t syncColor = synced ? Colors::Green : Colors::Amber;
    t.fillRoundRect(24, 116, 120, 18, 4, syncColor);
    const int sOff = (120 - static_cast<int>(strlen(syncTxt)) * 6) / 2;
    screen.label(24 + sOff, 121, syncTxt, Colors::Bg, 1, syncColor);

    // Right: Screensaver Mode Toggle Pill
    const char* saverTxt = saverOn ? "SAVER: ON" : "SAVER: OFF";
    const uint16_t saverBg = saverOn ? Colors::Cyan : Colors::Bg;
    const uint16_t saverTc = saverOn ? Colors::Bg : Colors::Muted;
    t.fillRoundRect(176, 116, 120, 18, 4, saverBg);
    t.drawRoundRect(176, 116, 120, 18, 4, saverOn ? Colors::Cyan : Colors::Border);
    const int rOff = (120 - static_cast<int>(strlen(saverTxt)) * 6) / 2;
    screen.label(176 + rOff, 121, saverTxt, saverTc, 1, saverBg);

    // Initial seconds progress bar track
    t.fillRect(24, 146, 272, 3, Colors::Bg);

    lastClockSynced_ = synced;
    lastClockSaverOn_ = saverOn;
    lastClockDay_ = curDay;
    fullRedraw_ = false;
  } else {
    // Dynamic updates for badges/labels if changed
    if (curDay != lastClockDay_) {
      lastClockDay_ = curDay;
      t.fillRect(24, 94, 272, 10, Colors::Panel);
      char dateStr[16]{};
      ctx.time.formatDate(dateStr, sizeof(dateStr));
      char subLabel[40]{};
      snprintf(subLabel, sizeof(subLabel), "%s  |  UTC%+d", dateStr, ctx.time.timezoneOffset());
      const int subW = static_cast<int>(strlen(subLabel)) * 6;
      const int subX = 12 + (296 - subW) / 2;
      screen.label(subX, 94, subLabel, Colors::White, 1, Colors::Panel);
    }
    if (synced != lastClockSynced_) {
      lastClockSynced_ = synced;
      const char* syncTxt = synced ? "NTP SYNCED" : "LOCAL RTC";
      const uint16_t syncColor = synced ? Colors::Green : Colors::Amber;
      t.fillRoundRect(24, 116, 120, 18, 4, syncColor);
      const int sOff = (120 - static_cast<int>(strlen(syncTxt)) * 6) / 2;
      screen.label(24 + sOff, 121, syncTxt, Colors::Bg, 1, syncColor);
    }
    if (saverOn != lastClockSaverOn_) {
      lastClockSaverOn_ = saverOn;
      const char* saverTxt = saverOn ? "SAVER: ON" : "SAVER: OFF";
      const uint16_t saverBg = saverOn ? Colors::Cyan : Colors::Bg;
      const uint16_t saverTc = saverOn ? Colors::Bg : Colors::Muted;
      t.fillRoundRect(176, 116, 120, 18, 4, saverBg);
      t.drawRoundRect(176, 116, 120, 18, 4, saverOn ? Colors::Cyan : Colors::Border);
      const int rOff = (120 - static_cast<int>(strlen(saverTxt)) * 6) / 2;
      screen.label(176 + rOff, 121, saverTxt, saverTc, 1, saverBg);
    }
  }

  // Big digital time centered (Size 4 = 24px wide per char, 8 chars = 192px width)
  char timeStr[16]{};
  ctx.time.formatTime(timeStr, sizeof(timeStr));
  const int timeW = static_cast<int>(strlen(timeStr)) * 24;
  const int timeX = 12 + (296 - timeW) / 2;
  screen.label(timeX, 50, timeStr, accent, 4, Colors::Panel);

  // Animated seconds progress bar across bottom of panel
  const int sec = static_cast<int>(ep % 60);
  if (sec == 0) {
    t.fillRect(24, 146, 272, 3, Colors::Bg);
  }
  const int barW = (sec * 272) / 59;
  if (barW > 0) {
    t.fillRect(24, 146, barW, 3, accent);
  }
}

void ClockApp::resetScreensaver() {
  screensaverFullRedraw_ = true;
  lastScreensaverEpoch_ = 0;
}

void ClockApp::renderScreensaver(AppContext& ctx, uint32_t nowMs) {
  (void)nowMs;
  const uint32_t ep = ctx.time.getEpoch();
  if (ep == lastScreensaverEpoch_ && !screensaverFullRedraw_) return;
  lastScreensaverEpoch_ = ep;

  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();

  const uint16_t accents[] = {Colors::Cyan, Colors::Amber, Colors::Green, Colors::Magenta};
  const uint16_t accent = accents[themeIdx_ % 4];

  const bool synced = ctx.time.isSynced();
  const uint32_t curDay = ep / 86400;

  if (screensaverFullRedraw_) {
    screen.clear();

    // Fullscreen cyber frame
    t.fillRoundRect(8, 6, 304, 158, 12, Colors::Bg);
    t.drawRoundRect(8, 6, 304, 158, 12, accent);
    t.drawRoundRect(10, 8, 300, 154, 10, Colors::Panel);

    // Top header pill: SCREENSAVER
    t.fillRoundRect(80, 14, 160, 16, 4, Colors::Panel);
    const bool portalOn = ctx.webPortal.isActive();
    screen.label(portalOn ? 86 : 95, 18, portalOn ? "PORTAL BROADCASTING" : "CLOCK SCREENSAVER", portalOn ? Colors::Green : Colors::Cyan, 1, Colors::Panel);

    // Date and Timezone label
    char dateStr[16]{};
    ctx.time.formatDate(dateStr, sizeof(dateStr));
    char subLabel[40]{};
    snprintf(subLabel, sizeof(subLabel), "%s  |  UTC%+d", dateStr, ctx.time.timezoneOffset());
    const int subW = static_cast<int>(strlen(subLabel)) * 6;
    const int subX = (320 - subW) / 2;
    screen.label(subX, 86, subLabel, Colors::White, 1, Colors::Bg);

    // NTP / RTC status + Tap to wake badge or Web Portal Active Badge
    const uint32_t portalRem = portalOn ? ctx.webPortal.remainingSeconds(millis()) : 0;
    char infoStr[48]{};
    if (portalOn) {
      snprintf(infoStr, sizeof(infoStr), "[ PORTAL AP ACTIVE (%us) * TAP ]",
               static_cast<unsigned int>(portalRem));
    } else {
      snprintf(infoStr, sizeof(infoStr), "[ %s  *  TAP TO WAKE ]", synced ? "NTP SYNCED" : "RTC ACTIVE");
    }
    const int infoW = static_cast<int>(strlen(infoStr)) * 6;
    const int infoX = (320 - infoW) / 2;
    screen.label(infoX, 108, infoStr, portalOn ? Colors::Green : Colors::Muted, 1, Colors::Bg);

    // Initial seconds progress bar track
    t.fillRect(20, 134, 280, 4, Colors::Panel);

    lastScreensaverSynced_ = synced;
    lastScreensaverDay_ = curDay;
    lastScreensaverPortalOn_ = portalOn;
    lastScreensaverPortalRem_ = portalRem;
    screensaverFullRedraw_ = false;
  } else {
    // Differential updates for day or sync status change
    if (curDay != lastScreensaverDay_) {
      lastScreensaverDay_ = curDay;
      t.fillRect(20, 86, 280, 12, Colors::Bg);
      char dateStr[16]{};
      ctx.time.formatDate(dateStr, sizeof(dateStr));
      char subLabel[40]{};
      snprintf(subLabel, sizeof(subLabel), "%s  |  UTC%+d", dateStr, ctx.time.timezoneOffset());
      const int subW = static_cast<int>(strlen(subLabel)) * 6;
      const int subX = (320 - subW) / 2;
      screen.label(subX, 86, subLabel, Colors::White, 1, Colors::Bg);
    }
    const bool portalOn = ctx.webPortal.isActive();
    const uint32_t portalRem = portalOn ? ctx.webPortal.remainingSeconds(millis()) : 0;
    if (synced != lastScreensaverSynced_ || portalOn != lastScreensaverPortalOn_ || portalRem != lastScreensaverPortalRem_) {
      lastScreensaverSynced_ = synced;
      lastScreensaverPortalOn_ = portalOn;
      lastScreensaverPortalRem_ = portalRem;
      t.fillRect(16, 108, 288, 12, Colors::Bg);
      char infoStr[48]{};
      if (portalOn) {
        snprintf(infoStr, sizeof(infoStr), "[ PORTAL AP ACTIVE (%us) * TAP ]",
                 static_cast<unsigned int>(portalRem));
      } else {
        snprintf(infoStr, sizeof(infoStr), "[ %s  *  TAP TO WAKE ]", synced ? "NTP SYNCED" : "RTC ACTIVE");
      }
      const int infoW = static_cast<int>(strlen(infoStr)) * 6;
      const int infoX = (320 - infoW) / 2;
      screen.label(infoX, 108, infoStr, portalOn ? Colors::Green : Colors::Muted, 1, Colors::Bg);
    }
  }

  // Big digital time centered (Size 4 = 24px wide per char, 8 chars = 192px width)
  char timeStr[16]{};
  ctx.time.formatTime(timeStr, sizeof(timeStr));
  const int timeW = static_cast<int>(strlen(timeStr)) * 24;
  const int timeX = (320 - timeW) / 2;
  screen.label(timeX, 42, timeStr, accent, 4, Colors::Bg);

  // Animated seconds progress bar across bottom of panel
  const int sec = static_cast<int>(ep % 60);
  if (sec == 0) {
    t.fillRect(20, 134, 280, 4, Colors::Panel);
  }
  const int barW = (sec * 280) / 59;
  if (barW > 0) {
    t.fillRect(20, 134, barW, 4, accent);
  }
}
