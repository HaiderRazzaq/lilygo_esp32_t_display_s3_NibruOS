#include "apps/WifiDefenseApp.h"
#include <Arduino.h>
#include "core/AppContext.h"
#include "core/ResourceManager.h"
#include "services/WifiDefenseService.h"
#include "hardware/DisplayManager.h"
#include "ui/Theme.h"
#include <cstdio>
#include <cstring>

void WifiDefenseApp::onEnter(AppContext& ctx) {
  resourceAcquired_ = ctx.resources.acquireWiFi(ResourceOwner::WifiDefense);
  if (resourceAcquired_) {
    ctx.wifiDefense.start(1, true, millis());
  }
  fullRedraw_ = true;
  lastRenderMs_ = 0;
}

void WifiDefenseApp::onExit(AppContext& ctx) {
  ctx.wifiDefense.stop();
  if (resourceAcquired_) {
    ctx.resources.releaseWiFi(ResourceOwner::WifiDefense);
    resourceAcquired_ = false;
  }
}

void WifiDefenseApp::onTick(AppContext& ctx, uint32_t nowMs) {
  ctx.wifiDefense.tick(nowMs);
  if (nowMs - lastRenderMs_ >= 250) {
    lastRenderMs_ = nowMs;
    render(ctx);
  }
}

void WifiDefenseApp::onTouch(AppContext& ctx, const TouchEvent& touch) {
  if (touch.gesture != TouchGesture::Tap) return;

  const int x = touch.x;
  const int y = touch.y;

  // Bottom Toolbar Action Buttons (y >= 138)
  if (y >= 138) {
    // Button 1: Channel Mode (10..105)
    if (x >= 10 && x <= 105) {
      ctx.wifiDefense.cycleChannelMode();
      render(ctx);
      return;
    }
    // Button 2: Reset Stats (112..207)
    if (x >= 112 && x <= 207) {
      ctx.wifiDefense.resetStats();
      render(ctx);
      return;
    }
    // Button 3: Pause / Resume (214..309)
    if (x >= 214 && x <= 309) {
      ctx.wifiDefense.setPaused(!ctx.wifiDefense.isPaused());
      render(ctx);
      return;
    }
  }
}

void WifiDefenseApp::onPrimaryAction(AppContext& ctx) {
  ctx.wifiDefense.cycleChannelMode();
  render(ctx);
}

bool WifiDefenseApp::onBack(AppContext&) {
  return false; // Allow AppManager to return Home
}

void WifiDefenseApp::render(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();
  WifiDefenseService& def = ctx.wifiDefense;

  if (fullRedraw_) {
    screen.clear();
    fullRedraw_ = false;
  }

  // 1. Header (y: 0..28)
  screen.header("WiFi Defense IDS", true);

  // Top-Right Alert Pill (x: 185..312, y: 6..24)
  const bool underAttack = def.isUnderAttack();
  const uint16_t alertBg = underAttack ? Colors::Red : Colors::Bg;
  const uint16_t alertBorder = underAttack ? Colors::White : Colors::Green;
  const uint16_t alertText = underAttack ? Colors::White : Colors::Green;

  t.fillRoundRect(188, 6, 122, 19, 4, alertBg);
  t.drawRoundRect(188, 6, 122, 19, 4, alertBorder);
  if (underAttack) {
    char rateBuf[20];
    snprintf(rateBuf, sizeof(rateBuf), "! ATTACK: %u/s !", def.deauthRate());
    screen.label(192, 10, rateBuf, alertText, 1, alertBg);
  } else if (def.isPaused()) {
    screen.label(218, 10, "[ PAUSED ]", Colors::Amber, 1, alertBg);
  } else {
    screen.label(208, 10, "[ SHIELD: OK ]", alertText, 1, alertBg);
  }

  // If Wi-Fi resource conflict
  if (!resourceAcquired_) {
    t.fillRoundRect(10, 34, 300, 100, 6, Colors::Panel);
    t.drawRoundRect(10, 34, 300, 100, 6, Colors::Red);
    screen.label(20, 50, "Radio in use by another app", Colors::Red, 1, Colors::Panel);
    screen.label(20, 75, "Please stop other Wi-Fi features first.", Colors::White, 1, Colors::Panel);
    return;
  }

  // 2. Left Panel: Detection Metrics (x: 10, y: 32, w: 145, h: 102)
  t.fillRoundRect(10, 32, 145, 102, 5, Colors::Panel);
  t.drawRoundRect(10, 32, 145, 102, 5, Colors::Border);

  char chBuf[24];
  if (def.autoHop()) {
    snprintf(chBuf, sizeof(chBuf), "Hop Ch: %u/13", def.channel());
  } else {
    snprintf(chBuf, sizeof(chBuf), "Lock Ch: %u", def.channel());
  }
  screen.label(16, 38, chBuf, Colors::Cyan, 1, Colors::Panel);

  char buf[32];
  snprintf(buf, sizeof(buf), "Deauth: %lu (%u/s)", static_cast<unsigned long>(def.deauthCount()), def.deauthRate());
  screen.label(16, 56, buf, def.deauthCount() > 0 ? Colors::Red : Colors::White, 1, Colors::Panel);

  snprintf(buf, sizeof(buf), "Disassoc: %lu", static_cast<unsigned long>(def.disassocCount()));
  screen.label(16, 74, buf, def.disassocCount() > 0 ? Colors::Amber : Colors::White, 1, Colors::Panel);

  snprintf(buf, sizeof(buf), "EAPOL HS: %lu", static_cast<unsigned long>(def.eapolCount()));
  screen.label(16, 92, buf, def.eapolCount() > 0 ? Colors::Green : Colors::Muted, 1, Colors::Panel);

  snprintf(buf, sizeof(buf), "Pkts: %lu (%u/s)", static_cast<unsigned long>(def.totalPackets()), def.packetRate());
  screen.label(16, 110, buf, Colors::Muted, 1, Colors::Panel);

  // 3. Right Panel: Incident Inspector (x: 162, y: 32, w: 148, h: 102)
  t.fillRoundRect(162, 32, 148, 102, 5, Colors::Panel);
  t.drawRoundRect(162, 32, 148, 102, 5, underAttack ? Colors::Red : Colors::Border);

  if (def.hasIncident()) {
    const IncidentRecord& inc = def.lastIncident();
    screen.label(168, 38, inc.isDeauth ? "Deauth Target Event" : "Disassoc Event", Colors::Amber, 1, Colors::Panel);

    char macBuf[20];
    WifiDefenseService::formatMac(inc.sourceMac, macBuf, sizeof(macBuf));
    char lineBuf[32];
    snprintf(lineBuf, sizeof(lineBuf), "S: %s", macBuf);
    screen.label(168, 56, lineBuf, Colors::White, 1, Colors::Panel);

    if (inc.targetMac[0] == 0xFF && inc.targetMac[1] == 0xFF && inc.targetMac[2] == 0xFF) {
      screen.label(168, 74, "T: BROADCAST", Colors::Red, 1, Colors::Panel);
    } else {
      WifiDefenseService::formatMac(inc.targetMac, macBuf, sizeof(macBuf));
      snprintf(lineBuf, sizeof(lineBuf), "T: %s", macBuf);
      screen.label(168, 74, lineBuf, Colors::White, 1, Colors::Panel);
    }

    snprintf(lineBuf, sizeof(lineBuf), "Rsn: %s", WifiDefenseService::reasonString(inc.reasonCode));
    screen.label(168, 92, lineBuf, Colors::Muted, 1, Colors::Panel);

    snprintf(lineBuf, sizeof(lineBuf), "RSSI: %d dBm | Ch %u", inc.rssi, inc.channel);
    screen.label(168, 110, lineBuf, Colors::Cyan, 1, Colors::Panel);
  } else {
    screen.label(168, 45, "Listening on air...", Colors::Cyan, 1, Colors::Panel);
    screen.label(168, 68, "Monitoring 802.11", Colors::Muted, 1, Colors::Panel);
    screen.label(168, 88, "Deauth, Disassoc &", Colors::Muted, 1, Colors::Panel);
    screen.label(168, 108, "EAPOL Handshakes.", Colors::Muted, 1, Colors::Panel);
  }

  // 4. Bottom Toolbar (y: 138..166)
  // Button 1: Channel Mode (10..105)
  t.fillRoundRect(10, 138, 95, 26, 4, Colors::Panel);
  t.drawRoundRect(10, 138, 95, 26, 4, Colors::Border);
  screen.label(18, 144, def.autoHop() ? "Ch: AutoHop" : chBuf, Colors::White, 1, Colors::Panel);

  // Button 2: Reset Stats (112..207)
  t.fillRoundRect(112, 138, 95, 26, 4, Colors::Panel);
  t.drawRoundRect(112, 138, 95, 26, 4, Colors::Border);
  screen.label(130, 144, "Reset Stats", Colors::White, 1, Colors::Panel);

  // Button 3: Pause / Resume (214..309)
  const bool paused = def.isPaused();
  t.fillRoundRect(214, 138, 95, 26, 4, paused ? Colors::Amber : Colors::Panel);
  t.drawRoundRect(214, 138, 95, 26, 4, paused ? Colors::White : Colors::Border);
  screen.label(238, 144, paused ? "Resume" : "Pause", paused ? Colors::Bg : Colors::White, 1, paused ? Colors::Amber : Colors::Panel);
}
