#pragma once
#include "core/App.h"
#include "core/EventTypes.h"
#include <stdint.h>
#include <queue>

class DesktopInput {
public:
  static DesktopInput& instance();

  void processEvents(uint32_t nowMs, uint8_t scale = 3);

  bool pollTouch(TouchEvent& ev);
  ButtonAction pollButtons(uint32_t nowMs);

  bool isTouchPressed() const { return touchPressed_; }
  bool anyButtonPressed() const { return bootPressed_ || userPressed_; }
  bool shouldQuit() const { return shouldQuit_; }
  void requestQuit() { shouldQuit_ = true; }

  bool consumeRebootRequest() {
    bool r = rebootRequested_;
    rebootRequested_ = false;
    return r;
  }

  bool isHudVisible() const { return showHud_; }
  void toggleHud() { showHud_ = !showHud_; }

  // Programmatic simulation triggers for testing
  void injectTouch(int16_t x, int16_t y, TouchGesture gesture);
  void injectButton(ButtonAction action);

private:
  DesktopInput() = default;

  bool shouldQuit_ = false;
  bool rebootRequested_ = false;
  bool showHud_ = false;

  bool touchPressed_ = false;
  int16_t startX_ = 0;
  int16_t startY_ = 0;
  int16_t curX_ = 0;
  int16_t curY_ = 0;
  uint32_t downAt_ = 0;
  bool moved_ = false;
  uint32_t contactId_ = 0;

  uint32_t lastTapTime_ = 0;
  int16_t lastTapX_ = 0;
  int16_t lastTapY_ = 0;

  bool bootPressed_ = false;
  bool userPressed_ = false;

  std::queue<TouchEvent> touchQueue_;
  std::queue<ButtonAction> buttonQueue_;
};
