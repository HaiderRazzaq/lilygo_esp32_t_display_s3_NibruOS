#include "apps/WiFiChannelsApp.h"
#include "core/AppContext.h"
#include "core/ResourceManager.h"
#include "hardware/DisplayManager.h"
#include <Arduino.h>
#include <cstdio>

void WiFiChannelsApp::onEnter(AppContext& ctx) {
  acquired_ = ctx.resources.acquireWiFi(ResourceOwner::WiFiChannels);
  lastState_ = ctx.wifi.state();
  invalidate();
}

void WiFiChannelsApp::onExit(AppContext& ctx) {
  if (!acquired_) return;
  if (ctx.wifi.state() == ScanState::Preparing || ctx.wifi.state() == ScanState::Scanning)
    ctx.wifi.stopScan();
  if (ctx.wifi.connectState() == ConnectState::Disconnected) ctx.wifi.shutdown();
  ctx.resources.releaseWiFi(ResourceOwner::WiFiChannels);
  acquired_ = false;
}

void WiFiChannelsApp::onTick(AppContext& ctx, uint32_t nowMs) {
  if (!acquired_) return;
  ctx.wifi.tick(nowMs);
  if (lastState_ != ctx.wifi.state()) {
    lastState_ = ctx.wifi.state();
    invalidate();
  }
}

void WiFiChannelsApp::onPrimaryAction(AppContext& ctx) {
  if (!acquired_) return;
  ctx.wifi.startScan(millis());
  invalidate();
}

void WiFiChannelsApp::onTouch(AppContext& ctx, const TouchEvent& touch) {
  if (touch.gesture == TouchGesture::Tap || touch.gesture == TouchGesture::DoubleTap) {
    if (touch.x >= 229 && touch.x < 310 && touch.y >= 36 && touch.y < 63) {
      onPrimaryAction(ctx);
      return;
    }
    if (touch.x >= 13 && touch.x < 307 && touch.y >= 85 && touch.y < 146) {
      selectedChannel_ = static_cast<uint8_t>((touch.x - 13) / 21 + 1);
      invalidate();
    }
  } else if (touch.gesture == TouchGesture::SwipeLeft && selectedChannel_ > 1) {
    --selectedChannel_;
    invalidate();
  } else if (touch.gesture == TouchGesture::SwipeRight && selectedChannel_ < WiFiService::kChannelCount) {
    ++selectedChannel_;
    invalidate();
  }
}

void WiFiChannelsApp::render(AppContext& ctx) {
  auto& screen = ctx.display;
  auto& gfx = screen.gfx();
  screen.clear();
  screen.header(title(), true);
  const auto state = ctx.wifi.state();
  const bool scanning = state == ScanState::Preparing || state == ScanState::Scanning;
  const auto buttonColor = acquired_ && !scanning ? Colors::Cyan : Colors::Panel;
  gfx.fillRoundRect(229, 36, 81, 27, 5, buttonColor);
  screen.label(248, 46, "REFRESH", Colors::White, 1, buttonColor);
  screen.footer("AP counts | Tap channel | BOOT scan");

  if (!acquired_) {
    screen.label(13, 76, "Radio busy", Colors::Amber, 2);
    return;
  }
  if (scanning) {
    screen.label(13, 76, state == ScanState::Preparing ? "Preparing scan..." : "Scanning...",
                 Colors::Cyan, 2);
    return;
  }
  if (state == ScanState::Failed) {
    screen.label(13, 76, "Scan failed", Colors::Red, 2);
    screen.label(13, 105, ctx.wifi.lastError(), Colors::Muted);
    return;
  }
  if (state != ScanState::Complete) {
    screen.label(13, 76, "Tap REFRESH to scan", Colors::Muted, 2);
    return;
  }

  char line[48];
  snprintf(line, sizeof(line), "Visible APs: %u", static_cast<unsigned>(ctx.wifi.scanResultCount()));
  screen.label(13, 46, line, Colors::White);
  const auto& selected = *ctx.wifi.channelSummary(selectedChannel_);
  if (selected.accessPoints) {
    snprintf(line, sizeof(line), "Ch %u  APs %u  Peak %d dBm", selectedChannel_,
             static_cast<unsigned>(selected.accessPoints), selected.strongestRssi);
  } else {
    snprintf(line, sizeof(line), "Ch %u  APs 0  Peak --", selectedChannel_);
  }
  screen.label(13, 70, line, Colors::Cyan);

  uint16_t maximum = 1;
  for (uint8_t channel = 1; channel <= WiFiService::kChannelCount; ++channel) {
    const auto count = ctx.wifi.channelSummary(channel)->accessPoints;
    if (count > maximum) maximum = count;
  }
  for (uint8_t channel = 1; channel <= WiFiService::kChannelCount; ++channel) {
    const int x = 13 + (channel - 1) * 21;
    const auto count = ctx.wifi.channelSummary(channel)->accessPoints;
    const auto color = channel == selectedChannel_ ? Colors::Cyan : Colors::Muted;
    const int height = count == 0 ? 0 : 1 + (static_cast<uint32_t>(count) * 35 / maximum);
    gfx.drawRoundRect(x, 85, 17, 43, 2, channel == selectedChannel_ ? Colors::Cyan : Colors::Border);
    if (height) gfx.fillRect(x + 2, 125 - height, 13, height, color);
    char label[3];
    snprintf(label, sizeof(label), "%u", channel);
    screen.label(x + (channel < 10 ? 5 : 2), 136, label, color);
  }
}
