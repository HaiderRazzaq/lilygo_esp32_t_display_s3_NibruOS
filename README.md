# NibruOS — T-Display-S3 Touch Glass

One ESP32-S3 Arduino/PlatformIO firmware with Wi-Fi Hub, 2FA Authenticator and
Essential Settings (runtime v0.3). This change completes existing Settings and
establishes resource acceptance procedures. Physical tests of this changed image
remain pending; see [current state](docs/CURRENT_STATE.md),
[verification](docs/VERIFICATION_REPORT.md) and [resource report](docs/performance/SETTINGS_RESOURCE_IMPACT.md).

Start with [AGENTS.md](AGENTS.md) and the [documentation index](docs/README.md).
Every runtime-impacting change must follow the permanent
[resource policy](docs/RESOURCE_BUDGET.md).

## Setup / تشغيل سريع

1. Open the directory in VS Code/PlatformIO; retain the pinned framework/libraries.
2. Build: `pio run -e lilygo-t-display-s3` (also checks actual app0 image capacity).
3. Upload only when intended, with the observed COM port; procedures are in
   [BUILD_AND_TEST](docs/BUILD_AND_TEST.md). Latest repaired COM5 upload passed (HWT-0013); runtime acceptance remains pending.
4. Touch opens apps; BOOT short selects, hold activates. Settings scrolls with
   vertical swipes and supports physical focus/activation. GPIO14/header Back
   cancels a confirmation, returns to menu, then Home.
5. [Settings](docs/SETTINGS.md) explains brightness, timeout, accent, diagnostics,
   confirmed reboot/reset, preservation scope and the opt-in LEDC build.
6. [Profiling](docs/MEMORY_PROFILING.md) explains sparse serial instrumentation
   and pending hardware scenarios A–E. Runtime resource use is NOT MEASURED.

Existing Wi-Fi credential/security/vault functionality predates this scope.
Settings reset preserves that data; do not use production secrets in development
captures. Current implementation is not proof of secure hardware storage or
physical acceptance. No new BLE, SD, OTA or Deep Sleep implementation is added.

## Tested hardware baseline retained

From the user's working example (not guessed):

```cpp
#define TOUCH_SDA 18
#define TOUCH_SCL 17
#define TOUCH_RST 21
#define TOUCH_INT 16
CST816S touch(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_INT);
// touch.begin(FALLING) after LOW / HIGH reset pulse
// LCD GPIO15 HIGH, BL GPIO38 HIGH, TFT rotation 1
// landscape mapped touchX = rawY; touchY = 169 - rawX
// swipe direction IDs translated into landscape directions
// GPIO0 BOOT + GPIO14 User button = INPUT_PULLUP
```

LCD is ST7789 170x320 native, **8-bit parallel**. TFT_eSPI setup is supplied entirely as build flags in `platformio.ini`, matching `Setup206_LilyGo_T_Display_S3.h`; **do not modify installed libraries manually**. Board `lilygo-t-display-s3` uses ESP32-S3 with QIO Flash and OPI PSRAM.

## Code structure

```text
platformio.ini
include/BoardConfig.h
src/
  main.cpp                    ← setup and loop only
  core/App.h                  ← app lifecycle interface
  core/AppContext.h           ← typed dependency access
  core/AppManager.{h,cpp}     ← home/menu/router
  core/ResourceManager.*      ← scanner ownership
  core/MemoryProfiler.*       ← bounded resource snapshots
  core/WakeInputGate.h        ← consume wake contacts
  hardware/DisplayManager.*  ← TFT_eSPI & common UI
  hardware/TouchManager.*    ← CST816S + coordinates
  hardware/ButtonManager.*   ← debounced GPIO0/14
  services/WiFiService.*      ← async scan / saved connections
  services/SettingsService.*  ← typed user preferences
  services/SecurityService.*  ← existing PIN/session state
  services/{Crypto,Time,Totp,UsbHid}Service.* ← existing 2FA services
  apps/SystemInfoApp.*
  apps/TouchTestApp.*
  apps/WiFiScannerApp.*
  apps/WiFiApp.*
  apps/TotpApp.*
  apps/SettingsApp.*
tests/host/                    ← desktop mock smoke tests
docs/                          ← architecture, build, adding new apps, acceptance checks
```

## Extend with a new app

1. Implement `App`'s `title/onEnter/onExit/onTick/onTouch/render` methods in a dedicated app class.
2. Access shared services through `AppContext`, not by independently reconfiguring the LCD or touch pins.
3. Acquire/release shared resources in enter/exit. Prefer asynchronous operations; return from `onTick()` promptly.
4. Register the new instance in `src/main.cpp`. Launcher supports up to **12 apps** across paginated screens (3 per page). Page change: horizontal swipe; selection changes: BOOT short or vertical swipe.
5. Add host tests and perform on-device acceptance tests.

## Boundaries / الحدود

- Firmware applications are **compiled into one image**; it does not hot-load standalone `.bin` programs or Bruce/GhostESP firmware.
- Existing Security/2FA/vault code is retained; it has not received an independent security or physical acceptance audit in this change.
- No OTA/filesystem/SD service, battery percentage estimate or multi-radio scheduler exists. No new task is introduced by Settings.
- The board's physical Touch Glass panel might differ across production variants. If `TFT_eSPI` works but touch does not, verify the CST816S chip/driver against the working original sketch.

**References**
- LILYGO pinout / examples: https://github.com/Xinyuan-LilyGO/T-Display-S3
- LILYGO display profile: https://github.com/Xinyuan-LilyGO/T-Display-S3/blob/main/lib/TFT_eSPI/User_Setups/Setup206_LilyGo_T_Display_S3.h
- PlatformIO board: https://docs.platformio.org/en/stable/boards/espressif32/lilygo-t-display-s3.html
- TFT_eSPI user setup flags: https://github.com/Bodmer/TFT_eSPI/blob/master/docs/PlatformIO/Configuring%20options.txt
- CST816S (fbiego): https://github.com/fbiego/CST816S

## v0.1.1 Wi-Fi reliability update + AI IDE handoff

If your earlier scanner showed **Scan timed out**, this package includes an updated asynchronous Wi-Fi state machine, one automatic retry, explicit low-level scan cancellation, and additional serial diagnostics. Hardware verification is still required. See [Wi-Fi debugging](docs/WIFI_DIAGNOSTICS.md).

**Start every AI IDE session with [`AGENTS.md`](AGENTS.md), then [`docs/PROJECT_CONTEXT.md`](docs/PROJECT_CONTEXT.md), [`docs/CURRENT_STATE.md`](docs/CURRENT_STATE.md) and [`docs/HARDWARE_PINOUT.md`](docs/HARDWARE_PINOUT.md).** Full onboarding and prompts: [`docs/AI_IDE_QUICKSTART.md`](docs/AI_IDE_QUICKSTART.md). Canonical changelog: [`CHANGELOG.md`](CHANGELOG.md); [original package history](docs/CHANGELOG.md) is preserved.
