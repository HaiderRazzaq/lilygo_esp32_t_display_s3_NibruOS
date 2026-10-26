#include "hardware/ButtonManager.h"
#include "BoardConfig.h"
#include <Arduino.h>

void ButtonManager::begin() {
  pinMode(Board::kButtonBoot, INPUT_PULLUP);
  pinMode(Board::kButtonUser, INPUT_PULLUP);
}
ButtonAction ButtonManager::pollOne(State& s, int pin, uint32_t now,
                                   ButtonAction shortAction, ButtonAction longAction) {
  const bool down = digitalRead(pin) == LOW;
  // Debounce only state transitions; duration measurement remains monotonic.
  if (down != s.previousDown && now - s.lastEdge >= 35) {
    s.lastEdge = now;
    s.previousDown = down;
    if (down) {
      s.downAt = now;
      s.longHandled = false;
    } else if (!s.longHandled) {
      return shortAction;
    }
  }
  if (s.previousDown && !s.longHandled &&
      now - s.downAt >= Board::kButtonLongPressMs) {
    s.longHandled = true;
    return longAction;
  }
  return ButtonAction::None;
}
ButtonAction ButtonManager::poll(uint32_t now) {
  // GPIO14 always takes priority, allowing escape from all apps.
  ButtonAction action = pollOne(user_, Board::kButtonUser, now,
                                ButtonAction::Back, ButtonAction::Back);
  if (action != ButtonAction::None) return action;
  return pollOne(boot_, Board::kButtonBoot, now,
                 ButtonAction::SelectNext, ButtonAction::Activate);
}
