#pragma once
#include <stdint.h>
#include <stddef.h>

struct AppContext;
struct TouchEvent;

class SecurityService {
public:
  void begin();
  bool isPinEnabled() const { return pinEnabled_; }
  void setPinEnabled(bool en);
  bool isLocked() const { return locked_; }
  void lock();
  void unlock();
  bool verifyPin(const char* pin) const;
  bool setPin(const char* newPin);

  void tick(uint32_t nowMs);
  void handleTouch(AppContext& ctx, const TouchEvent& ev);
  void render(AppContext& ctx);
  bool consumeDirty() { bool d = dirty_; dirty_ = false; return d; }
  void invalidate() { dirty_ = true; }

private:
  bool pinEnabled_ = false;
  bool locked_ = false;
  bool dirty_ = true;
  char pinCode_[8] = "1234";

  // Lock screen input buffer
  char inputPin_[8]{};
  uint8_t inputLen_ = 0;
  bool errorState_ = false;
  uint32_t errorUntilMs_ = 0;

  void load();
  void save();
};

