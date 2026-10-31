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

  // If CST816S reports points == 0 or event == 1 (Up), finger was lifted
  if (touch_.data.points == 0 || touch_.data.event == 1) {
    touchDown_ = false;
    return false;
  }

  // Minimum debounce gap between any accepted touch reports
  if (now - lastTouchTime_ < Board::kTouchDebounceMs) return false;

  TouchGesture gesture = TouchGesture::Unknown;
  switch (touch_.data.gestureID) {
    case SWIPE_UP: gesture = TouchGesture::SwipeLeft; break;
    case SWIPE_DOWN: gesture = TouchGesture::SwipeRight; break;
    case SWIPE_LEFT: gesture = TouchGesture::SwipeDown; break;
    case SWIPE_RIGHT: gesture = TouchGesture::SwipeUp; break;
    case DOUBLE_CLICK: gesture = TouchGesture::DoubleTap; break;
    case LONG_PRESS: gesture = TouchGesture::LongPress; break;
    case SINGLE_CLICK:
      gesture = TouchGesture::Tap;
      break;
    case NONE:
      // When CST816S reports NONE during touch contact:
      // Only fire Tap once when the touch first begins; suppress continuous hold repeats!
      if (touchDown_) {
        return false;
      }
      gesture = TouchGesture::Tap;
      break;
    default: break;
  }

  if (gesture == TouchGesture::Unknown) return false;

  touchDown_ = true;
  lastTouchTime_ = now;

  ev.rawX = touch_.data.x;
  ev.rawY = touch_.data.y;
  ev.x = constrain(static_cast<int>(ev.rawY), 0, Board::kScreenWidth - 1);
  ev.y = constrain(Board::kScreenHeight - 1 - static_cast<int>(ev.rawX),
                   0, Board::kScreenHeight - 1);
  ev.gesture = gesture;
  return true;
}
