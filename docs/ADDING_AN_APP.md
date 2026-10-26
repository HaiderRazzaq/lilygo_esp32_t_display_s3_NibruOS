# Adding a new app to NibruOS (v0.1)

Applications are **compile-time C++ modules**, not ELF plugins or separate `.bin` files.
Only one application is active at a time, and the shell owns the display, touch
sensor, physical buttons, and the upper-left Back action.

## Minimal development path

1. Create `src/apps/MyApp.h` and `src/apps/MyApp.cpp`.
2. Derive `MyApp` from `App` (defined in `src/core/App.h`). Implement
   `title()`, `onEnter()`, `onExit()`, `onTick()`, `onTouch()`, and `render()`.
3. In `src/main.cpp`, include your header, create one **static** instance
   alongside the other apps, and call `launcher.registerApp(myApp)` **before**
   `launcher.begin()`.
4. Put transient application initialization in `onEnter()` and release any
   services, callbacks, scan state, or memory it owns in `onExit()`.
5. Keep `onTick()` fast. Do not call infinite retry loops or block on network
   operations. Mark changed data using `invalidate()` so redraws occur when needed.
6. Render using `ctx.display.gfx()` and common helpers. Call
   `ctx.display.header(title(), true)` and `ctx.display.footer(...)`; reserve
   the 0..32 pixel header for the shell's Back button.
7. Extend the `ResourceOwner` enum / resource manager if you introduce shared
   peripherals that can conflict with another app. Only release a resource you own.
8. Add tests in `tests/host/test_main.cpp` and run the desktop smoke tests;
   afterwards run a **real PlatformIO Build** and the device acceptance tests.

## Header example (not registered by default)

```cpp
#pragma once
#include "core/App.h"

class MyApp final : public App {
public:
    const char* title() const override { return "My App"; }
    void onEnter(AppContext&) override { invalidate(); }
    void onExit(AppContext&) override {}
    void onTick(AppContext&, uint32_t) override {}
    void onTouch(AppContext&, const TouchEvent&) override {}
    void render(AppContext& ctx) override;
};
```

In `MyApp.cpp`, include `"core/AppContext.h"` and `"hardware/DisplayManager.h"`,
then draw into `ctx.display`. Do not instantiate another TFT_eSPI or CST816S
object and do not change the shared GPIO configuration.

## Design rules

- Keep passwords, TOTP secrets and Wi-Fi credentials **out of firmware source**.
- Do not assume the entire 16 MB Flash is available for application code;
  check the selected partition table and PlatformIO's linker usage report.
- A future Wi-Fi/BLE combination may require resource coordination beyond
  the simple single-owner Wi-Fi lease used in v0.1.
- If a new app adds a driver or library, pin a compatible version and test
  against the working TFT_eSPI + ESP32 core setup before upgrading either.
