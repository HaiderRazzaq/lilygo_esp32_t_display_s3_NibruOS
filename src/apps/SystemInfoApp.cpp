#include "apps/SystemInfoApp.h"
#include "core/AppContext.h"
#include "hardware/DisplayManager.h"
#include <Arduino.h>
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
  char line[56];
  const char* chip = ESP.getChipModel();
  snprintf(line, sizeof(line), "Chip: %s  %d MHz", chip, ESP.getCpuFreqMHz());
  screen.label(10, 41, line, Colors::White, 1);
  snprintf(line, sizeof(line), "Flash: %lu MB", static_cast<unsigned long>(ESP.getFlashChipSize() / 1048576UL));
  screen.label(10, 59, line, Colors::Cyan);
  snprintf(line, sizeof(line), "PSRAM: %lu / %lu KB free",
           static_cast<unsigned long>(ESP.getFreePsram() / 1024UL),
           static_cast<unsigned long>(ESP.getPsramSize() / 1024UL));
  screen.label(10, 77, line, Colors::Cyan);
  snprintf(line, sizeof(line), "Free heap: %lu KB", static_cast<unsigned long>(ESP.getFreeHeap() / 1024UL));
  screen.label(10, 95, line);
  snprintf(line, sizeof(line), "Uptime: %lu s", static_cast<unsigned long>(millis() / 1000UL));
  screen.label(10, 113, line);
  t.drawRoundRect(212, 51, 96, 72, 8, Colors::Border);
  t.setTextColor(Colors::Green, Colors::Bg);
  t.setTextSize(2);
  t.setCursor(225, 62);
  t.print("ESP32");
  t.setTextSize(1);
  t.setCursor(228, 93);
  t.print("S3 / R8");
  screen.footer("GPIO14 = Back  |  BOOT = Select");
}
