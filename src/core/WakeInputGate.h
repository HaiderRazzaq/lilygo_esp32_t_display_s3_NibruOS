#pragma once
#include <stdint.h>

// Suppress the entire wake press/gesture, including a delayed button release or
// follow-up double tap. No input dispatch until a quiet, fully released interval.
class WakeInputGate {
public:
  void arm(uint32_t now) { armed_ = true; quietSince_ = now; }
  bool consume(bool touch, bool action, bool contactOrButtonDown, uint32_t now) {
    if (!armed_) return false;
    if (touch || action || contactOrButtonDown) quietSince_ = now;
    if (now - quietSince_ >= 350) armed_ = false;
    return true;
  }
private:
  bool armed_ = false;
  uint32_t quietSince_ = 0;
};
