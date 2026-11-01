#pragma once
#include "core/App.h"
#include <CST816S.h>

class TouchManager {
public:
  TouchManager();
  void begin();
  bool poll(TouchEvent& event);
  bool isPressed() const { return contactActive_; }
private:
  CST816S touch_;
  TouchEvent pending_{};
  uint32_t downAt_ = 0;
  int16_t startX_ = 0, startY_ = 0;
  bool gestureSent_ = false, moved_ = false;
  uint8_t lastPhase_ = 1; // Up; repeated Down frames do not start extra contacts.
  bool contactActive_ = false;
  uint32_t contactId_ = 0;
};
