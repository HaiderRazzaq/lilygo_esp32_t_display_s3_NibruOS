#pragma once
#include <Arduino.h>
#include "apps/TotpApp.h"
#include "core/AppContext.h"
#include "services/TotpService.h"
#include "services/SecurityService.h"
#include <cassert>
#include <cstring>

inline void authTap(TotpApp& app, AppContext& ctx, int x, int y) {
  app.onTouch(ctx, TouchEvent{static_cast<int16_t>(x), static_cast<int16_t>(y), 0, 0,
                            TouchGesture::Tap, 0, millis()});
}
inline void authLongPress(TotpApp& app, AppContext& ctx, int x, int y) {
  app.onTouch(ctx, TouchEvent{static_cast<int16_t>(x), static_cast<int16_t>(y), 0, 0,
                            TouchGesture::LongPress, 0, millis()});
}
inline void authAdd(TotpApp& app, AppContext& ctx) {
  authTap(app, ctx, 110, 158);
  authTap(app, ctx, 160, 50);
}
inline void authType(TotpApp& app, AppContext& ctx, const char* text) {
  const char* rows[] = {"1234567890", "QWERTYUIOP", "ASDFGHJKL", "ZXCVBNM"};
  for (const char* p = text; *p; ++p) {
    bool found = false;
    for (int row = 0; row < 4; ++row) {
      const char* match = std::strchr(rows[row], *p);
      if (!match) continue;
      const int index = static_cast<int>(match - rows[row]);
      const int x = row < 2 ? 18 + index * 32 : row == 2 ? 24 + index * 34 : 59 + index * 31;
      authTap(app, ctx, x, 60 + row * 20);
      found = true; break;
    }
    assert(found);
  }
}
inline void testAuthenticatorEntryFlow(AppContext& ctx, TotpApp& app) {
  constexpr const char* fixture = "GEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQ"; // Public RFC fixture.
  app.onEnter(ctx);
  app.render(ctx);
  assert(ctx.totp.accountCount() == 0);
  authAdd(app, ctx); // ADD > TYPE ON DEVICE
  authType(app, ctx, "TEST");
  authTap(app, ctx, 270, 158); // NEXT
  authType(app, ctx, fixture);
  authTap(app, ctx, 270, 158); // SAVE
  assert(ctx.totp.accountCount() == 1);
  assert(std::strcmp(ctx.totp.getAccount(0)->name, "TEST") == 0);
  app.render(ctx);
  app.onExit(ctx);
  assert(ctx.totp.accountCount() == 0); // Exit wipes plaintext, not persisted data.
  for (unsigned i = 0; i < 20; ++i) {
    app.onEnter(ctx);
    assert(ctx.totp.accountCount() == 1);
    app.onExit(ctx);
    assert(ctx.totp.accountCount() == 0);
  }
  app.onEnter(ctx);
  authAdd(app, ctx);
  authType(app, ctx, "CANCEL");
  assert(app.onBack(ctx)); // Back abandons editor, does not append an account.
  assert(ctx.totp.accountCount() == 1);
  assert(!app.onBack(ctx)); // At card, shell may return Home.
  app.onExit(ctx);
}
