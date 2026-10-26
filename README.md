# NibruOS v0.1 — T-Display-S3 Touch Glass

**First deliverable:** a small, extensible Arduino/PlatformIO firmware shell for the owner's **LILYGO T-Display-S3 Touch Glass**, with a Home Launcher and three apps: **System Info**, **Touch Test**, and **Wi-Fi Scanner**.

> **Status:** Source files are complete and the host-side C++ smoke tests pass. **Not compiled against actual PlatformIO Arduino dependencies here and not flashed or verified on physical hardware.** You must run PlatformIO Build and then test on the device. See `docs/BUILD_AND_TEST.md`.

## Setup / تشغيل سريع

1. Install **VS Code** and **PlatformIO IDE** extension. Open this entire directory (the directory containing `platformio.ini`) with VS Code.
2. Keep internet connected on your Windows PC during the first build; PlatformIO will download its platform and dependencies.
3. Run **PlatformIO: Build** (`pio run -e lilygo-t-display-s3`).
4. Connect the board via data-capable USB-C, select the device COM port, and run **Upload** (`pio run -e lilygo-t-display-s3 -t upload`).
5. Use the touch screen to choose an app. Physical buttons: **BOOT (GPIO0)** short = select next, hold ~650 ms = open; **GPIO14** short or long = return Home. Touch the **`<`** in the upper-left corner to return Home.

### Apps included

| App | Function | Resource behaviour |
|---|---|---|
| System Info | ESP32 model / CPU, flash, PSRAM, free heap, uptime | No radio required |
| Touch Test | Live mapped and raw coordinates; CST816S gesture reporting | No radio required |
| Wi-Fi Scanner | Nonblocking 2.4 GHz network discovery; name, channel, RSSI, security | Acquires radio on entry, turns Wi-Fi off on exit; scan timeout 16 s |

**No Wi-Fi password or account secret is embedded in this firmware.** This is a read-only scan of networks. No packet injection, capture, portal, or HID automation in v0.1.

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
  core/ResourceManager.*      ← Wi-Fi ownership
  hardware/DisplayManager.*  ← TFT_eSPI & common UI
  hardware/TouchManager.*    ← CST816S + coordinates
  hardware/ButtonManager.*   ← debounced GPIO0/14
  services/WiFiService.*      ← async scan and cleanup
  apps/SystemInfoApp.*
  apps/TouchTestApp.*
  apps/WiFiScannerApp.*
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
- This project intentionally does not include Password Vault or TOTP; their security architecture and time handling require a separate implementation.
- No OTA, filesystem, task scheduler, battery percentage estimate, or multi-radio scheduler in v0.1.
- The board's physical Touch Glass panel might differ across production variants. If `TFT_eSPI` works but touch does not, verify the CST816S chip/driver against the working original sketch.

**References**
- LILYGO pinout / examples: https://github.com/Xinyuan-LilyGO/T-Display-S3
- LILYGO display profile: https://github.com/Xinyuan-LilyGO/T-Display-S3/blob/main/lib/TFT_eSPI/User_Setups/Setup206_LilyGo_T_Display_S3.h
- PlatformIO board: https://docs.platformio.org/en/stable/boards/espressif32/lilygo-t-display-s3.html
- TFT_eSPI user setup flags: https://github.com/Bodmer/TFT_eSPI/blob/master/docs/PlatformIO/Configuring%20options.txt
- CST816S (fbiego): https://github.com/fbiego/CST816S

## v0.1.1 Wi-Fi reliability update + AI IDE handoff

If your earlier scanner showed **Scan timed out**, this package includes an updated asynchronous Wi-Fi state machine, one automatic retry, explicit low-level scan cancellation, and additional serial diagnostics. Hardware verification is still required. See [Wi-Fi debugging](docs/WIFI_DIAGNOSTICS.md).

**Start every AI IDE session with [`AGENTS.md`](AGENTS.md), then [`docs/PROJECT_CONTEXT.md`](docs/PROJECT_CONTEXT.md) and [`docs/HARDWARE_PINOUT.md`](docs/HARDWARE_PINOUT.md).** Full onboarding and prompts: [`docs/AI_IDE_QUICKSTART.md`](docs/AI_IDE_QUICKSTART.md). Change log: [`docs/CHANGELOG.md`](docs/CHANGELOG.md).
