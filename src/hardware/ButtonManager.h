#pragma once
#include <stdint.h>

enum class ButtonAction : uint8_t { None, SelectNext, Activate, Back };
class ButtonManager {
public:
  void begin();
  ButtonAction poll(uint32_t nowMs);
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
