#pragma once
#include <stdint.h>

/**
 * @brief Input event definitions for NibruOS.
 *
 * Provides normalized, platform-independent event types for touch gestures,
 * touch coordinates, and physical button actions.
 */

// Normalized gestures in display coordinate space (after Rotation 1 mapping).
enum class TouchGesture : uint8_t {
  Tap,
  SwipeUp,
  SwipeDown,
  SwipeLeft,
  SwipeRight,
  DoubleTap,
  LongPress,
  Unknown
};

// Physical screen coordinates and gesture payload.
struct TouchEvent {
  int16_t x;
  int16_t y;
  int16_t rawX;
  int16_t rawY;
  TouchGesture gesture;
  uint32_t contactId = 0; // TouchManager contact identity; not a timestamp.
  uint32_t contactStartedMs = 0;
};

// Physical hardware button actions (BOOT and USER button events).
enum class ButtonAction : uint8_t {
  None,
  SelectNext,
  Activate,
  Back
};
