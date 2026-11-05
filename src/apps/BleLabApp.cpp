#include "apps/BleLabApp.h"
#include "core/AppContext.h"
#include "services/BleAdvertisingService.h"
#include "hardware/DisplayManager.h"
#include <Arduino.h>
#include <cstdio>

namespace {
struct TargetOption {
  BleAdvertisingProfile profile;
  const char* family;
  const char* protocol;
};
const TargetOption targets[] = {
  {BleAdvertisingProfile::Android, "Android", "Google Fast Pair"},
  {BleAdvertisingProfile::Windows, "Windows", "Swift Pair"},
  {BleAdvertisingProfile::Apple, "iPhone / iPad", "Apple AirPods"},
  {BleAdvertisingProfile::SamsungBuds, "Samsung", "Galaxy Buds"},
  {BleAdvertisingProfile::SamsungWatch, "Samsung", "Galaxy Watch"},
  {BleAdvertisingProfile::All, "All types", "Cycle five profiles"},
  {BleAdvertisingProfile::Generic, "BLE scanner", "LabBLE identities"},
  {BleAdvertisingProfile::Random, "Random", "Random type + model"},
  {BleAdvertisingProfile::AppleAction, "Apple Action", "iPhone action popup"},
  {BleAdvertisingProfile::AppleNotYourDevice, "Apple alert", "Not Your Device"},
  {BleAdvertisingProfile::Beacon, "Beacon spam", "Eddystone names"}
};
constexpr uint8_t kTargetCount = sizeof(targets) / sizeof(targets[0]);
constexpr uint8_t kPageSize = 8;
constexpr uint8_t kPageCount = (kTargetCount + kPageSize - 1) / kPageSize;
}

void BleLabApp::onEnter(AppContext&) {
  selectingTarget_ = false;
  pickerPage_ = 0;
  invalidate();
}

void BleLabApp::onExit(AppContext& ctx) {
  if (ctx.bleAdvertising) ctx.bleAdvertising->stop(millis(), ctx);
  selectingTarget_ = false;
}

void BleLabApp::onTick(AppContext& ctx, uint32_t) {
  if (!ctx.bleAdvertising) return;
  if (drawnCount_ != ctx.bleAdvertising->advertisedCount() ||
      drawnActive_ != ctx.bleAdvertising->isActive() ||
      drawnError_ != ctx.bleAdvertising->lastError()) invalidate();
}

void BleLabApp::onPrimaryAction(AppContext& ctx) {
  if (selectingTarget_) {
    targetIndex_ = (targetIndex_ + 1) % kTargetCount;
    pickerPage_ = targetIndex_ / kPageSize;
    invalidate();
    return;
  }
  if (!ctx.bleAdvertising) return;
  if (ctx.bleAdvertising->isActive()) ctx.bleAdvertising->stop(millis(), ctx);
  else ctx.bleAdvertising->start(profile_ == BleAdvertisingProfile::Random ? 1 : count_,
                                  seconds_, millis(), ctx, profile_);
  invalidate();
}

void BleLabApp::selectTarget(uint8_t index) {
  if (index >= kTargetCount) return;
  targetIndex_ = index;
  pickerPage_ = index / kPageSize;
  profile_ = targets[index].profile;
  selectingTarget_ = false;
  invalidate();
}

void BleLabApp::setPickerPage(uint8_t page) {
  if (page >= kPageCount || page == pickerPage_) return;
  pickerPage_ = page;
  // Keep the BOOT highlight on a visible card when switching pages.
  if (targetIndex_ / kPageSize != page) targetIndex_ = page * kPageSize;
  invalidate();
}

void BleLabApp::onActivate(AppContext& ctx) {
  if (selectingTarget_) selectTarget(targetIndex_);
  else onPrimaryAction(ctx);
}

bool BleLabApp::onBack(AppContext&) {
  if (!selectingTarget_) return false;
  selectingTarget_ = false;
  invalidate();
  return true;
}

void BleLabApp::onTouch(AppContext& ctx, const TouchEvent& touch) {
  if (touch.gesture != TouchGesture::Tap && touch.gesture != TouchGesture::DoubleTap) return;
  if (touch.x < 13 || touch.x >= 307) return;
  if (selectingTarget_) {
    if (touch.contactId != 0 && touch.contactId == pickerContactId_) return;
    if (touch.y < 33) {
      if (touch.x >= 278) setPickerPage(pickerPage_ + 1);
      else if (touch.x >= 200) setPickerPage(pickerPage_ - 1);
      return;
    }
    if (touch.y < 39 || touch.y >= 148 || (touch.x >= 157 && touch.x < 163)) return;
    const uint8_t row = (touch.y - 39) / 28;
    if ((touch.y - 39) % 28 >= 25) return;
    const uint8_t index = static_cast<uint8_t>(pickerPage_ * kPageSize + row * 2 +
                                               (touch.x >= 163 ? 1 : 0));
    if (index < kTargetCount) selectTarget(index);
    return;
  }
  if (touch.y >= 123 && touch.y < 150) {
    onPrimaryAction(ctx);
    return;
  }
  if (!ctx.bleAdvertising || ctx.bleAdvertising->isActive()) return;
  if (touch.y >= 36 && touch.y < 62) {
    selectingTarget_ = true;
    pickerContactId_ = touch.contactId;
    for (uint8_t i = 0; i < kTargetCount; ++i)
      if (targets[i].profile == profile_) targetIndex_ = i;
    pickerPage_ = targetIndex_ / kPageSize;
    invalidate();
    return;
  }
  if (touch.y < 65 || touch.y >= 118 || (touch.x >= 57 && touch.x < 264)) return;
  const bool increase = touch.x >= 264;
  if (touch.y < 93) {
    if (profile_ == BleAdvertisingProfile::Random) return;
    if (increase && count_ < 10) ++count_;
    if (!increase && count_ > 1) --count_;
  } else {
    if (increase && seconds_ < 60) seconds_ += 5;
    if (!increase && seconds_ > 5) seconds_ -= 5;
  }
  invalidate();
}

void BleLabApp::render(AppContext& ctx) {
  if (selectingTarget_) {
    renderTargets(ctx);
    return;
  }
  auto& screen = ctx.display;
  auto& gfx = screen.gfx();
  auto* service = ctx.bleAdvertising;
  screen.clear();
  screen.header(title(), true);
  if (!service) {
    screen.label(13, 74, "BLE service unavailable", Colors::Muted);
    return;
  }
  drawnActive_ = service->isActive();
  drawnCount_ = service->advertisedCount();
  drawnError_ = service->lastError();
  const auto row = [&](int y, const char* name, uint8_t value) {
    char label[24];
    snprintf(label, sizeof(label), "%s: %u", name, static_cast<unsigned>(value));
    screen.label(92, y + 8, label, Colors::White);
    if (drawnActive_) return;
    gfx.fillRoundRect(13, y, 44, 26, 4, Colors::Panel);
    gfx.fillRoundRect(264, y, 43, 26, 4, Colors::Panel);
    screen.label(32, y + 8, "-", Colors::White, 1, Colors::Panel);
    screen.label(283, y + 8, "+", Colors::White, 1, Colors::Panel);
  };
  gfx.fillRoundRect(13, 36, 294, 26, 4, Colors::Panel);
  screen.label(23, 44, bleAdvertisingProfileName(profile_), Colors::Cyan, 1, Colors::Panel);
  if (!drawnActive_) screen.label(291, 44, ">", Colors::Cyan, 1, Colors::Panel);
  const char* countLabel = "Devices";
  if (profile_ == BleAdvertisingProfile::All) countLabel = "Per type";
  else if (profile_ == BleAdvertisingProfile::Beacon) countLabel = "Names";
  if (profile_ == BleAdvertisingProfile::Random)
    screen.label(65, 74, "Random type + model", Colors::White);
  else row(65, countLabel, count_);
  row(93, "Seconds", seconds_);
  char status[40];
  snprintf(status, sizeof(status), "First pass %u/%u", drawnCount_, service->identityCount());
  if (profile_ == BleAdvertisingProfile::Random)
    snprintf(status, sizeof(status), "Random: %s", drawnActive_ ? "running" : "ready");
  const auto color = drawnActive_ ? Colors::Red : Colors::Cyan;
  gfx.fillRoundRect(13, 123, 294, 26, 5, color);
  screen.label(136, 132, drawnActive_ ? "STOP" : "START", Colors::Bg, 1, color);
  screen.footer(drawnError_[0] ? drawnError_ : status);
}

void BleLabApp::renderTargets(AppContext& ctx) {
  auto& screen = ctx.display;
  auto& gfx = screen.gfx();
  screen.clear();
  char title[20];
  snprintf(title, sizeof(title), "Targets %u/%u",
           static_cast<unsigned>(pickerPage_ + 1), static_cast<unsigned>(kPageCount));
  screen.header(title, true);
  screen.label(203, 9, "<", pickerPage_ ? Colors::Cyan : Colors::Muted, 2, Colors::Panel);
  screen.label(287, 9, ">",
               pickerPage_ + 1 < kPageCount ? Colors::Cyan : Colors::Muted, 2,
               Colors::Panel);
  for (uint8_t slot = 0; slot < kPageSize; ++slot) {
    const uint8_t i = static_cast<uint8_t>(pickerPage_ * kPageSize + slot);
    if (i >= kTargetCount) break;
    const int x = 13 + (slot % 2) * 150;
    const int y = 39 + (slot / 2) * 28;
    gfx.fillRoundRect(x, y, 144, 25, 4, Colors::Panel);
    gfx.drawRoundRect(x, y, 144, 25, 4, i == targetIndex_ ? Colors::Cyan : Colors::Border);
    screen.label(x + 7, y + 3, targets[i].family, Colors::White, 1, Colors::Panel);
    screen.label(x + 7, y + 15, targets[i].protocol, Colors::Muted, 1, Colors::Panel);
  }
  screen.footer("Tap pick | BOOT next | header pages");
}
