#pragma once
#include <stdint.h>

struct AppContext;

// Input events are physical screen coordinates after Rotation 1 mapping.
enum class TouchGesture : uint8_t {
  Tap, SwipeUp, SwipeDown, SwipeLeft, SwipeRight, DoubleTap, LongPress, Unknown
};
struct TouchEvent {
  int16_t x;
  int16_t y;
  int16_t rawX;
  int16_t rawY;
  TouchGesture gesture;
  uint32_t contactId = 0; // TouchManager contact identity; not a timestamp.
  uint32_t contactStartedMs = 0;
};

class App {
public:
  virtual ~App() = default;
  virtual const char* title() const = 0;
  virtual const char* shortTitle() const { return "APP"; }
  virtual const char* subtitle() const { return "Utility"; }
  virtual void onEnter(AppContext& ctx) = 0;
  virtual void onExit(AppContext& ctx) = 0;
  virtual void onTick(AppContext& ctx, uint32_t nowMs) = 0;
  virtual void onTouch(AppContext& ctx, const TouchEvent& touch) = 0;
  // Optional physical BOOT button action while this app is active.
  virtual void onPrimaryAction(AppContext&) {}
  // Hold activation defaults to legacy primary action for existing apps.
  virtual void onActivate(AppContext& ctx) { onPrimaryAction(ctx); }
  // Optional Back handler; returns true if handled internally, false to go Home.
  virtual bool onBack(AppContext&) { return false; }
  virtual void render(AppContext& ctx) = 0;
  // A shared overlay (e.g. PIN lock) replaced the display contents.
  virtual void onDisplayRestored(AppContext&) { invalidate(); }

  bool consumeDirty() { const bool d = dirty_; dirty_ = false; return d; }
  void invalidate() { dirty_ = true; }

private:
  bool dirty_ = true;
};
