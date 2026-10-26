#include "hardware/TouchManager.h"
#include "BoardConfig.h"
#include <Arduino.h>

TouchManager::TouchManager()
 : touch_(Board::kTouchSda, Board::kTouchScl,
          Board::kTouchReset, Board::kTouchInterrupt) {}

void TouchManager::begin() {
  // Keep the working sketch's reset/IRQ/begin sequence unchanged.
  pinMode(Board::kTouchInterrupt, INPUT_PULLUP);
  pinMode(Board::kTouchReset, OUTPUT);
  digitalWrite(Board::kTouchReset, LOW);
  delay(10);
  digitalWrite(Board::kTouchReset, HIGH);
  delay(50);
  touch_.begin(FALLING);
  Serial.println("[Touch] CST816S configured SDA=18 SCL=17 RST=21 INT=16");
}

bool TouchManager::poll(TouchEvent& ev) {
  if (!touch_.available()) return false;
  const uint32_t now = millis();
  if (now - lastTouchTime_ < Board::kTouchDebounceMs) return false;
  lastTouchTime_ = now;

  ev.rawX = touch_.data.x;
  ev.rawY = touch_.data.y;
  // Exact tested mapping for display rotation 1 / 320x170 landscape.
  ev.x = constrain(static_cast<int>(ev.rawY), 0, Board::kScreenWidth - 1);
  ev.y = constrain(Board::kScreenHeight - 1 - static_cast<int>(ev.rawX),
                   0, Board::kScreenHeight - 1);

  // CST816S gesture IDs use the panel's native portrait axes.
  // The landscape coordinate transform is (x, y) = (rawY, 169 - rawX).
  // Apply the SAME rotation to directional swipe gestures so the home
  // launcher and network list move in the direction seen by the user.
  switch (touch_.data.gestureID) {
    case SWIPE_UP: ev.gesture = TouchGesture::SwipeLeft; break;
    case SWIPE_DOWN: ev.gesture = TouchGesture::SwipeRight; break;
    case SWIPE_LEFT: ev.gesture = TouchGesture::SwipeDown; break;
    case SWIPE_RIGHT: ev.gesture = TouchGesture::SwipeUp; break;
    case DOUBLE_CLICK: ev.gesture = TouchGesture::DoubleTap; break;
    case LONG_PRESS: ev.gesture = TouchGesture::LongPress; break;
    case NONE:
    case SINGLE_CLICK: ev.gesture = TouchGesture::Tap; break;
    default: ev.gesture = TouchGesture::Unknown; break;
  }
  return true;
}
