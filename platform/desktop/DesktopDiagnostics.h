#pragma once
#include <stdint.h>
#include <cstddef>

class AppManager;
class ResourceManager;
class WiFiService;

class DesktopDiagnostics {
public:
  static DesktopDiagnostics& instance();

  void init();
  void tick(uint32_t nowMs, AppManager& launcher, ResourceManager& resources, WiFiService& wifi);
  void renderOverlay();

  float currentFps() const { return fps_; }
  uint32_t loopDurationUs() const { return lastLoopUs_; }
  size_t hostProcessMemoryKb() const;

private:
  DesktopDiagnostics() = default;

  uint32_t lastSecMs_ = 0;
  uint32_t frameCount_ = 0;
  float fps_ = 0.0f;
  uint32_t lastLoopUs_ = 0;
};
