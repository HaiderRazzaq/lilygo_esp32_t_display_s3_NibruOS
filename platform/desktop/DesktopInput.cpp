#define SDL_MAIN_HANDLED
#include "platform/desktop/DesktopInput.h"
#include "platform/desktop/SDLDisplay.h"
#include "platform/desktop/MockWiFiService.h"
#include <SDL.h>
#include <cstdlib>
#include <algorithm>
#include <cstdio>

DesktopInput& DesktopInput::instance() {
  static DesktopInput s_instance;
  return s_instance;
}

void DesktopInput::processEvents(uint32_t nowMs, uint8_t scale) {
  if (scale == 0) scale = 1;

  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
      case SDL_QUIT:
        shouldQuit_ = true;
        break;

      case SDL_MOUSEBUTTONDOWN:
        if (e.button.button == SDL_BUTTON_LEFT) {
          touchPressed_ = true;
          startX_ = static_cast<int16_t>(std::clamp(e.button.x / scale, 0, SDLDisplay::kLogicalWidth - 1));
          startY_ = static_cast<int16_t>(std::clamp(e.button.y / scale, 0, SDLDisplay::kLogicalHeight - 1));
          curX_ = startX_;
          curY_ = startY_;
          downAt_ = nowMs;
          moved_ = false;
        }
        break;

      case SDL_MOUSEMOTION:
        if (touchPressed_) {
          curX_ = static_cast<int16_t>(std::clamp(e.motion.x / scale, 0, SDLDisplay::kLogicalWidth - 1));
          curY_ = static_cast<int16_t>(std::clamp(e.motion.y / scale, 0, SDLDisplay::kLogicalHeight - 1));
          const int dx = curX_ - startX_;
          const int dy = curY_ - startY_;
          if (std::abs(dx) > 12 || std::abs(dy) > 12) {
            moved_ = true;
          }
        }
        break;

      case SDL_MOUSEBUTTONUP:
        if (e.button.button == SDL_BUTTON_LEFT && touchPressed_) {
          touchPressed_ = false;
          const int16_t endX = static_cast<int16_t>(std::clamp(e.button.x / scale, 0, SDLDisplay::kLogicalWidth - 1));
          const int16_t endY = static_cast<int16_t>(std::clamp(e.button.y / scale, 0, SDLDisplay::kLogicalHeight - 1));
          TouchGesture gesture = TouchGesture::Tap;

          if (moved_) {
            const int dx = endX - startX_;
            const int dy = endY - startY_;
            if (std::abs(dx) >= std::abs(dy)) {
              gesture = (dx > 0) ? TouchGesture::SwipeRight : TouchGesture::SwipeLeft;
            } else {
              gesture = (dy > 0) ? TouchGesture::SwipeDown : TouchGesture::SwipeUp;
            }
          } else {
            if (nowMs - downAt_ >= 650) {
              gesture = TouchGesture::LongPress;
            } else if ((nowMs - lastTapTime_ < 300) &&
                       std::abs(endX - lastTapX_) < 12 &&
                       std::abs(endY - lastTapY_) < 12) {
              gesture = TouchGesture::DoubleTap;
              lastTapTime_ = 0;
            } else {
              gesture = TouchGesture::Tap;
              lastTapTime_ = nowMs;
              lastTapX_ = endX;
              lastTapY_ = endY;
            }
          }

          TouchEvent ev{};
          ev.x = endX;
          ev.y = endY;
          ev.rawX = static_cast<int16_t>(SDLDisplay::kLogicalHeight - 1 - endY);
          ev.rawY = endX;
          ev.gesture = gesture;
          ev.contactId = ++contactId_;
          touchQueue_.push(ev);
        }
        break;

      case SDL_KEYDOWN:
        switch (e.key.keysym.sym) {
          case SDLK_ESCAPE:
          case SDLK_BACKSPACE:
          case SDLK_u:
            userPressed_ = true;
            buttonQueue_.push(ButtonAction::Back);
            break;

          case SDLK_SPACE:
          case SDLK_TAB:
          case SDLK_b:
            bootPressed_ = true;
            buttonQueue_.push(ButtonAction::SelectNext);
            break;

          case SDLK_RETURN:
          case SDLK_KP_ENTER:
          case SDLK_o:
            bootPressed_ = true;
            buttonQueue_.push(ButtonAction::Activate);
            break;

          case SDLK_LEFT:
            injectTouch(160, 85, TouchGesture::SwipeLeft);
            break;

          case SDLK_RIGHT:
            injectTouch(160, 85, TouchGesture::SwipeRight);
            break;

          case SDLK_UP:
            injectTouch(160, 85, TouchGesture::SwipeUp);
            break;

          case SDLK_DOWN:
            injectTouch(160, 85, TouchGesture::SwipeDown);
            break;

          // Function Keys: Simulator Diagnostics & Controls
          case SDLK_F1:
            SDLDisplay::instance().cycleScale();
            break;

          case SDLK_F2:
            MockWiFiService::instance().setSimulationMode(WiFiSimMode::Normal);
            std::printf("[Simulator] Wi-Fi Mode set to: Normal (deterministic discovery)\n");
            break;

          case SDLK_F3:
            MockWiFiService::instance().setSimulationMode(WiFiSimMode::Timeout);
            std::printf("[Simulator] Wi-Fi Mode set to: Timeout (simulating scan hang)\n");
            break;

          case SDLK_F4:
            MockWiFiService::instance().setSimulationMode(WiFiSimMode::ScanFailed);
            std::printf("[Simulator] Wi-Fi Mode set to: ScanFailed (immediate error)\n");
            break;

          case SDLK_F5:
            rebootRequested_ = true;
            std::printf("[Simulator] Device reboot requested via F5\n");
            break;

          case SDLK_F6:
            toggleHud();
            std::printf("[Simulator] Diagnostic HUD %s\n", showHud_ ? "ENABLED" : "DISABLED");
            break;

          case SDLK_F9:
            MockWiFiService::instance().setSimulationMode(WiFiSimMode::StaError);
            std::printf("[Simulator] Wi-Fi Mode set to: StaError (STA init failure)\n");
            break;

          default:
            break;
        }
        break;

      case SDL_KEYUP:
        switch (e.key.keysym.sym) {
          case SDLK_SPACE:
          case SDLK_TAB:
          case SDLK_RETURN:
          case SDLK_KP_ENTER:
          case SDLK_b:
          case SDLK_o:
            bootPressed_ = false;
            break;
          case SDLK_ESCAPE:
          case SDLK_BACKSPACE:
          case SDLK_u:
            userPressed_ = false;
            break;
          default:
            break;
        }
        break;

      default:
        break;
    }
  }
}

bool DesktopInput::pollTouch(TouchEvent& ev) {
  if (touchQueue_.empty()) return false;
  ev = touchQueue_.front();
  touchQueue_.pop();
  return true;
}

ButtonAction DesktopInput::pollButtons(uint32_t /*nowMs*/) {
  if (buttonQueue_.empty()) return ButtonAction::None;
  ButtonAction act = buttonQueue_.front();
  buttonQueue_.pop();
  return act;
}

void DesktopInput::injectTouch(int16_t x, int16_t y, TouchGesture gesture) {
  TouchEvent ev{};
  ev.x = std::clamp<int16_t>(x, 0, SDLDisplay::kLogicalWidth - 1);
  ev.y = std::clamp<int16_t>(y, 0, SDLDisplay::kLogicalHeight - 1);
  ev.rawX = static_cast<int16_t>(SDLDisplay::kLogicalHeight - 1 - ev.y);
  ev.rawY = ev.x;
  ev.gesture = gesture;
  ev.contactId = ++contactId_;
  touchQueue_.push(ev);
}

void DesktopInput::injectButton(ButtonAction action) {
  buttonQueue_.push(action);
}
