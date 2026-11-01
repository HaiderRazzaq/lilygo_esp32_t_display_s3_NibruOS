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
  const bool wasPressed = contactActive_;
  contactActive_ = touch_.data.points != 0 && touch_.data.event != 1;
  const bool newDown = contactActive_ && touch_.data.event == 0 && lastPhase_ != 0;
  lastPhase_ = touch_.data.event;
  if (!wasPressed && !contactActive_) return false; // duplicate/orphan release
  if (contactActive_ && (!wasPressed || newDown)) {
    ++contactId_;
    gestureSent_ = false;
    moved_ = false;
    downAt_ = now;
    startX_ = touch_.data.x;
    startY_ = touch_.data.y;
  }
  if (contactActive_) {
    pending_.rawX = touch_.data.x;
    pending_.rawY = touch_.data.y;
    pending_.x = constrain(static_cast<int>(pending_.rawY), 0, Board::kScreenWidth - 1);
    pending_.y = constrain(Board::kScreenHeight - 1 - static_cast<int>(pending_.rawX),
                           0, Board::kScreenHeight - 1);
    const int dx = touch_.data.x - startX_, dy = touch_.data.y - startY_;
    if (dx > 12 || dx < -12 || dy > 12 || dy < -12) moved_ = true;
  }
  if (gestureSent_) return false; // one semantic event per physical contact

  TouchGesture gesture = TouchGesture::Unknown;
  switch (touch_.data.gestureID) {
    case SWIPE_UP: gesture = TouchGesture::SwipeLeft; break;
    case SWIPE_DOWN: gesture = TouchGesture::SwipeRight; break;
    case SWIPE_LEFT: gesture = TouchGesture::SwipeDown; break;
    case SWIPE_RIGHT: gesture = TouchGesture::SwipeUp; break;
    case DOUBLE_CLICK: gesture = TouchGesture::DoubleTap; break;
    case LONG_PRESS: gesture = TouchGesture::LongPress; break;
    case SINGLE_CLICK: gesture = TouchGesture::Tap; break;
    case NONE:
      // A Down/NONE may become a swipe. Commit a fallback tap only on release.
      if (!contactActive_ && !moved_) {
        gesture = now - downAt_ >= Board::kButtonLongPressMs ?
                    TouchGesture::LongPress : TouchGesture::Tap;
      }
      break;
    default: break;
  }
  if (gesture == TouchGesture::Unknown) return false;
  gestureSent_ = true;
  ev = pending_;
  ev.gesture = gesture;
  ev.contactId = contactId_;
  ev.contactStartedMs = downAt_;
  return true;
}
