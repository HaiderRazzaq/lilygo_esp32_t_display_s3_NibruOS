# Architecture — NibruOS v0.1

```text
main.cpp
 ├─ DisplayManager: one TFT_eSPI instance, GPIO15 power, GPIO38 backlight
 ├─ TouchManager: one CST816S instance, reset IRQ and Rotation 1 mapping
 ├─ ButtonManager: GPIO0 and GPIO14 debounce/press events
 ├─ AppManager: single active App, Home shell, touch/buttons/router
 │    ├─ SystemInfoApp
 │    ├─ TouchTestApp
 │    └─ WiFiScannerApp
 └─ AppContext: DisplayManager + ResourceManager + WiFiService
                                     └─ WiFiService: async discovery
```

## Constraints

- Exactly **one active app** receives update/touch/render. No concurrent app task is created by the manager.
- The shell owns back navigation and Home Launcher. Header's back-touch region (x<40, y<33) is reserved; physical GPIO14 always works, including when an app is scanning.
- `App::onEnter()` acquires resources and initializes state; `App::onExit()` releases them. The Home screen never leaves the Wi-Fi radio on from the Wi-Fi app.
- `App::onTick()` must not block; WiFiService calls `scanNetworks(true)` and `scanComplete()` asynchronously and times out.
- Apps have an explicit invalidation flag. The shell redraws when invalidated and does not busy-render the screen every loop.
- WiFiService handles radio-init, scan-start, scan-completion failures and a 16-second timeout by entering a retryable error state. While an active scan runs, a second RESCAN is ignored.
- WiFiService caps memory use to 12 records of fixed size, copying SSID/RSSI/channel/security out of the Arduino scan results before deleting them.
- The launcher supports 12 registered app instances in a fixed-size table; the home layout shows three icons per page. New app instances must be registered in `main.cpp`.
- No user script execution, no dynamic firmware loader, no external flash modifications.

## Orientation and touch gestures

The owner's verified code uses native touch coordinates rotated into landscape as
`x = rawY; y = 169 - rawX`. **Gesture IDs also follow the native portrait axes**,
so `TouchManager` rotates swipe directions for the visible 320×170 interface:
raw UP → screen LEFT; raw DOWN → screen RIGHT; raw LEFT → screen DOWN;
raw RIGHT → screen UP. This preserves the tested wiring and coordinate mapping.
Validate the direction on the physical touchscreen during acceptance testing.

## Input rules

| Input | Home | Inside app |
|---|---|---|
| Touch on tile | Launch corresponding app | App-specific touches |
| Horizontal swipe | Change launcher page | App-specific (Touch Test reports it) |
| Vertical swipe | Previous/next selection | App-specific (Wi-Fi scrolls) |
| BOOT (GPIO0) short | Next app selection | App-defined primary action (rescan in Wi-Fi) |
| BOOT long (650ms) | Launch selected app | App-defined primary action |
| GPIO14 short/long | Stay Home | Exit and return Home |
| Header `<` top-left | N/A | Exit and return Home |

## Reasons for design decisions

- **Arduino instead of migrating to ESP-IDF on day one:** preserves the user's known-good TFT_eSPI / CST816S example and reduces unknown variables.
- **No LVGL dependency yet:** on a 320×170 screen, simple TFT_eSPI-based navigation suffices for the foundation. Consider LVGL after devices have passed touch/OTA tests.
- **No Preferences use yet:** first version has no writable persistent user data, so no premature storage abstractions are needed.
- **Wi-Fi single owner:** an explicit lease prevents future apps from using the same radio without coordination. This is a guard, not an OS-level lock or Wi-Fi/BLE co-existence scheduler.
- **No app references concrete sibling apps:** actions dispatched via a virtual hook (`onPrimaryAction`).

## Known limitations

- Data shown in System Info is approximate runtime reporting, not a diagnostic certification.
- Wi-Fi scan results display up to 12 records, ordered by the library's returned order; not sorted by RSSI.
- Fixed header and text sizes may not fit unusually long names when adding new apps; introduce ellipsis/scrolling or alternate UI before shipping many apps.
- The provided host tests use stand-in framework classes; a PlatformIO device-target build is still required.
