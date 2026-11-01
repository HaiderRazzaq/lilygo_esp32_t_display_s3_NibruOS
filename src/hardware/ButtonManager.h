#pragma once
#include <stdint.h>
#include "core/EventTypes.h"

class ButtonManager {
public:
  void begin();
  ButtonAction poll(uint32_t nowMs);
  bool anyPressed() const;
private:
  struct State {
    bool previousDown = false;
    bool longHandled = false;
    uint32_t downAt = 0;
    uint32_t lastEdge = 0;
  } boot_, user_;
  ButtonAction pollOne(State& state, int pin, uint32_t nowMs,
                       ButtonAction shortAction, ButtonAction longAction);
};
