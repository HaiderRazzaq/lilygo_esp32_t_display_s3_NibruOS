#include "apps/SystemInfoApp.h"
#include "core/AppContext.h"
#include "services/WiFiService.h"
#include "hardware/DisplayManager.h"
#include <Arduino.h>
#include <WiFi.h>
#include <cstdio>

void SystemInfoApp::onEnter(AppContext&) { lastUpdateMs_ = millis(); invalidate(); }
void SystemInfoApp::onExit(AppContext&) {}
void SystemInfoApp::onTick(AppContext&, uint32_t now) {
  if (now - lastUpdateMs_ >= 1000) { lastUpdateMs_ = now; invalidate(); }
}
void SystemInfoApp::onTouch(AppContext&, const TouchEvent&) {}
void SystemInfoApp::render(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();
  screen.clear();
  screen.header(title(), true);
  char line[64];

  // 1. Chip and CPU
  const char* chip = ESP.getChipModel();
  snprintf(line, sizeof(line), "Chip:  %s @ %d MHz", chip, ESP.getCpuFreqMHz());
  screen.label(10, 36, line, Colors::White, 1);

  // 2. Flash and PSRAM
  snprintf(line, sizeof(line), "Flash: %lu MB  |  PSRAM: %lu MB",
           static_cast<unsigned long>(ESP.getFlashChipSize() / 1048576UL),
           static_cast<unsigned long>(ESP.getPsramSize() / 1048576UL));
  screen.label(10, 52, line, Colors::Cyan);

  // 3. Memory & Uptime
  snprintf(line, sizeof(line), "Heap:  %lu KB  |  Up: %lu s",
           static_cast<unsigned long>(ESP.getFreeHeap() / 1024UL),
           static_cast<unsigned long>(millis() / 1000UL));
  screen.label(10, 68, line, Colors::Cyan);

  // 4. MAC Address
  String mac = WiFi.macAddress();
  snprintf(line, sizeof(line), "MAC:   %s", mac.c_str());
  screen.label(10, 84, line, Colors::White);

  // 5. IP Address
  char ipBuf[24]{};
  ctx.wifi.getLocalIP(ipBuf, sizeof(ipBuf));
  const bool hasIp = (ipBuf[0] != '\0');
  snprintf(line, sizeof(line), "IP:    %s", hasIp ? ipBuf : "(Not connected)");
  screen.label(10, 100, line, hasIp ? Colors::Green : Colors::Muted);

  // 6. Wi-Fi Link
  if (ctx.wifi.connectState() == ConnectState::Connected) {
    snprintf(line, sizeof(line), "Wi-Fi: %s (%d dBm)",
             ctx.wifi.connectedSSID(), ctx.wifi.currentRSSI());
    screen.label(10, 116, line, Colors::Amber);
  } else {
    screen.label(10, 116, "Wi-Fi: Disconnected", Colors::Muted);
  }

  // Right status badge card
  t.drawRoundRect(210, 36, 100, 94, 6, Colors::Border);
  t.fillRoundRect(212, 38, 96, 22, 4, Colors::Panel);
  screen.label(225, 45, "NibruOS", Colors::Cyan, 1, Colors::Panel);

  const bool online = (ctx.wifi.connectState() == ConnectState::Connected);
  t.fillCircle(224, 76, 5, online ? Colors::Green : Colors::Muted);
  screen.label(236, 72, online ? "ONLINE" : "OFFLINE", online ? Colors::Green : Colors::Muted, 1);

  snprintf(line, sizeof(line), "PSRAM %luK", static_cast<unsigned long>(ESP.getFreePsram() / 1024UL));
  screen.label(222, 94, line, Colors::Muted, 1);

  snprintf(line, sizeof(line), "v0.1.1");
  screen.label(222, 112, line, Colors::Border, 1);

  screen.footer("GPIO14 = Back  |  BOOT = Select");
}
