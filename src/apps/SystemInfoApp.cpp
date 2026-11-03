#include "services/TimeService.h"
#include "apps/SystemInfoApp.h"
#include "core/AppContext.h"
#include "services/WiFiService.h"
#include "hardware/DisplayManager.h"
#include "BoardConfig.h"
#include <Arduino.h>

#include <cstdio>

void SystemInfoApp::onEnter(AppContext&) {
  lastUpdateMs_ = millis();
  fullRedraw_ = true;
  lastOnlineSet_ = false;
  invalidate();
}

void SystemInfoApp::onExit(AppContext&) {
  fullRedraw_ = true;
  lastOnlineSet_ = false;
}

void SystemInfoApp::onTick(AppContext&, uint32_t now) {
  if (now - lastUpdateMs_ >= 1000) {
    lastUpdateMs_ = now;
    invalidate();
  }
}

void SystemInfoApp::onTouch(AppContext&, const TouchEvent&) {}

void SystemInfoApp::render(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();
  char line[64];

  if (fullRedraw_) {
    screen.clear();
    screen.header(title(), true);

    // 1. Chip and CPU (static)
    const char* chip = ESP.getChipModel();
    snprintf(line, sizeof(line), "Chip:  %s @ %d MHz", chip, ESP.getCpuFreqMHz());
    screen.label(10, 36, line, Colors::White, 1);

    // 2. Flash and PSRAM (static)
    snprintf(line, sizeof(line), "Flash: %lu MB  |  PSRAM: %lu MB",
             static_cast<unsigned long>(ESP.getFlashChipSize() / 1048576UL),
             static_cast<unsigned long>(ESP.getPsramSize() / 1048576UL));
    screen.label(10, 52, line, Colors::Cyan);

    // 4. MAC Address (static)
    char macBuf[24]{};
    ctx.wifi.getMacAddress(macBuf, sizeof(macBuf));
    snprintf(line, sizeof(line), "MAC:   %s", macBuf);
    screen.label(10, 84, line, Colors::White);

    // Right status badge card border and static labels
    t.drawRoundRect(210, 36, 100, 94, 6, Colors::Border);
    t.fillRoundRect(212, 38, 96, 22, 4, Colors::Panel);
    screen.label(225, 45, "NibruOS", Colors::Cyan, 1, Colors::Panel);
    screen.label(225, 112, Board::kVersion, Colors::Muted, 1, Colors::Bg);

    screen.footer("GPIO14 = Back  |  BOOT = Select");
    fullRedraw_ = false;
  }

  // --- Dynamic updates below: NO screen.clear(), NO full redraw! ---

  // 3. Memory & Uptime (changes every second)

  snprintf(line, sizeof(line), "Heap:  %lu KB  |  Up: %lu s",
           static_cast<unsigned long>(ESP.getFreeHeap() / 1024UL),
           static_cast<unsigned long>(millis() / 1000UL));
  screen.labelPadded(10, 68, line, 30, Colors::Cyan, 1, Colors::Bg);

  // 5. IP Address (updated in-place)

  char ipBuf[24]{};
  ctx.wifi.getLocalIP(ipBuf, sizeof(ipBuf));
  const bool hasIp = (ipBuf[0] != '\0');
  snprintf(line, sizeof(line), "IP:    %s", hasIp ? ipBuf : "(Not connected)");
  screen.labelPadded(10, 100, line, 28, hasIp ? Colors::Green : Colors::Muted, 1, Colors::Bg);

  // 6. Wi-Fi Link (updated in-place)

  if (ctx.wifi.connectState() == ConnectState::Connected) {
    snprintf(line, sizeof(line), "Wi-Fi: %s (%d dBm)",
             ctx.wifi.connectedSSID(), ctx.wifi.currentRSSI());
    screen.labelPadded(10, 116, line, 30, Colors::Amber, 1, Colors::Bg);
  } else {
    screen.labelPadded(10, 116, "Wi-Fi: Disconnected", 30, Colors::Muted, 1, Colors::Bg);
  }

  // Right card: ONLINE/OFFLINE indicator
  const bool online = (ctx.wifi.connectState() == ConnectState::Connected);

  if (!lastOnlineSet_ || online != lastOnline_) {
    lastOnline_ = online;
    lastOnlineSet_ = true;
    t.fillCircle(224, 76, 5, online ? Colors::Green : Colors::Muted);
    screen.labelPadded(236, 72, online ? "ONLINE" : "OFFLINE", 8, online ? Colors::Green : Colors::Muted, 1, Colors::Bg);
  }


  // Right card: PSRAM free

  snprintf(line, sizeof(line), "PSRAM %luK", static_cast<unsigned long>(ESP.getFreePsram() / 1024UL));
  screen.labelPadded(222, 94, line, 11, Colors::Muted, 1, Colors::Bg);
}
