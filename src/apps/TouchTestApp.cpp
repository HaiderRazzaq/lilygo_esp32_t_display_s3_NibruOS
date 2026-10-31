#include "apps/TouchTestApp.h"
#include "core/AppContext.h"
#include "hardware/DisplayManager.h"
#include <cstdio>

static const char* gestureName(TouchGesture g) {
  switch(g) {
    case TouchGesture::Tap: return "Tap";
    case TouchGesture::SwipeUp: return "Swipe Up";
    case TouchGesture::SwipeDown: return "Swipe Down";
    case TouchGesture::SwipeLeft: return "Swipe Left";
    case TouchGesture::SwipeRight: return "Swipe Right";
    case TouchGesture::DoubleTap: return "Double Tap";
    case TouchGesture::LongPress: return "Long Press";
    default: return "Unknown";
  }
}

void TouchTestApp::onEnter(AppContext&) {
  touched_ = false;
  fullRedraw_ = true;
  invalidate();
}

void TouchTestApp::onExit(AppContext&) {
  fullRedraw_ = true;
}

void TouchTestApp::onTick(AppContext&, uint32_t) {}

void TouchTestApp::onTouch(AppContext&, const TouchEvent& ev) {
  last_ = ev;
  touched_ = true;
  invalidate();
}

void TouchTestApp::render(AppContext& ctx) {
  DisplayManager& screen = ctx.display;
  TFT_eSPI& t = screen.gfx();

  if (fullRedraw_) {
    screen.clear();
    screen.header(title(), true);
    t.drawRoundRect(7, 37, 306, 110, 5, Colors::Border);
    screen.footer("Test rotation 1 + CST816S gestures");
    fullRedraw_ = false;
  }

  // Clear only the inside of the test box without touching header or footer
  t.fillRect(9, 39, 302, 106, Colors::Bg);

  if (!touched_) {
    screen.label(28, 67, "Tap or swipe anywhere in the box", Colors::Cyan);
    screen.label(28, 88, "Back: top-left / GPIO14", Colors::Muted);
  } else {
    char line[52];
    snprintf(line, sizeof(line), "Screen X:%d Y:%d", last_.x, last_.y);
    screen.label(17, 43, line, Colors::White);
    snprintf(line, sizeof(line), "Raw X:%d Y:%d", last_.rawX, last_.rawY);
    screen.label(17, 60, line, Colors::Muted);
    screen.label(17, 78, gestureName(last_.gesture), Colors::Green, 2);

    int x = last_.x < 17 ? 17 : (last_.x > 302 ? 302 : last_.x);
    int y = last_.y < 48 ? 48 : (last_.y > 135 ? 135 : last_.y);
    t.drawCircle(x, y, 5, Colors::Cyan);
    t.drawFastHLine(x - 9, y, 19, Colors::Cyan);
    t.drawFastVLine(x, y - 9, 19, Colors::Cyan);
  }
}
