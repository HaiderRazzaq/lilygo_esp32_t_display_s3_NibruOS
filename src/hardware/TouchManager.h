#pragma once
#include "core/App.h"
#include <CST816S.h>

class TouchManager {
public:
  TouchManager();
  void begin();
  bool poll(TouchEvent& event);
private:
  CST816S touch_;
  uint32_t lastTouchTime_ = 0;
};
