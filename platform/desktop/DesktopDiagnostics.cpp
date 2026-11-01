#include "platform/desktop/DesktopDiagnostics.h"
#include "platform/desktop/SDLDisplay.h"
#include "platform/desktop/DesktopInput.h"
#include "platform/desktop/MockWiFiService.h"
#include "core/AppManager.h"
#include "core/ResourceManager.h"
#include "services/WiFiService.h"
#include <cstdio>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
#endif

DesktopDiagnostics& DesktopDiagnostics::instance() {
  static DesktopDiagnostics s_instance;
  return s_instance;
}

void DesktopDiagnostics::init() {
  lastSecMs_ = 0;
  frameCount_ = 0;
  fps_ = 0.0f;
  lastLoopUs_ = 0;
}

size_t DesktopDiagnostics::hostProcessMemoryKb() const {
#ifdef _WIN32
  PROCESS_MEMORY_COUNTERS pmc{};
  if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
    return pmc.WorkingSetSize / 1024;
  }
#endif
  return 0;
}

void DesktopDiagnostics::tick(uint32_t nowMs, AppManager& /*launcher*/, ResourceManager& /*resources*/, WiFiService& /*wifi*/) {
  frameCount_++;
  if (nowMs - lastSecMs_ >= 1000) {
    fps_ = static_cast<float>(frameCount_ * 1000) / static_cast<float>(nowMs - lastSecMs_);
    frameCount_ = 0;
    lastSecMs_ = nowMs;
  }
}

void DesktopDiagnostics::renderOverlay() {
  if (!DesktopInput::instance().isHudVisible()) return;

  SDLDisplay& screen = SDLDisplay::instance();
  // Draw small transparent-style HUD banner at bottom of logical screen
  screen.fillRect(4, 134, 312, 32, 0x18E3); // Dark slate
  screen.drawRoundRect(4, 134, 312, 32, 3, 0x05FF); // Cyan border

  char line1[64];
  std::snprintf(line1, sizeof(line1), "SIM HUD: FPS: %.1f | Host: %zu KB | Scale: %ux",
                fps_, hostProcessMemoryKb(), screen.scale());
  screen.setCursor(8, 138);
  screen.setTextSize(1);
  screen.setTextColor(0xFFFF, 0x18E3);
  screen.print(line1);

  char line2[64];
  std::snprintf(line2, sizeof(line2), "WiFi Mode: %s | F2:Norm F3:TO F4:Err F6:HUD",
                MockWiFiService::instance().modeName());
  screen.setCursor(8, 150);
  screen.setTextSize(1);
  screen.setTextColor(0x05FF, 0x18E3);
  screen.print(line2);
}
