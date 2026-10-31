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
- WiFiService prepares STA for 650ms, then starts an async active scan at 300ms/channel. It polls completion before the 11-second per-scan deadline, stops/deletes failed scans, and restarts once (2 total scan attempts). Initial STA initialization rejection enters Failed directly. RESCAN is ignored during Preparing/Scanning; retryable final errors remain visible. Physical timing and responsiveness need device testing.
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
- The provided host tests use stand-in framework classes. A real target build passed on 2026-11-01; physical acceptance remains pending. Current coverage and blockers are in [TEST_MATRIX](TEST_MATRIX.md).

## Communication, errors, memory and storage

`main.cpp` owns static driver/service/app instances and passes references in
`AppContext`. There is no message bus, interprocess communication or per-app task.
Touch/buttons route through AppManager; only the active app ticks/renders. Shared
resources are a cooperative Wi-Fi lease, not a scheduler or thread-safe lock.
No BLE owner/service exists; add one deliberately before introducing BLE apps.

WiFiScannerApp checks lease acquisition and renders a busy state if unavailable.
WiFiService exposes states/error strings and serial diagnostics, returning control
between stages. On exit the app calls shutdown and releases only its own lease.
The radio can remain in STA after results until exit or rescan. Display/touch begin
have no success-return check in main; registration returns bool but setup does not
check it for these three fixed apps. These are implementation limits, not proof
of runtime failures. No watchdog/recovery supervisor is implemented.

AppManager holds 12 pointers; WiFiService holds 12 fixed NetworkRecord entries
(33-byte SSID field each) and copies driver results before deletion. Temporary
Arduino String allocation still occurs when reading SSIDs; fixed app storage is
not proof of zero heap allocations or leak freedom. Rendering uses direct TFT
calls, no application framebuffer or PSRAM allocator. Runtime memory stability
requires [HWT-0005](HARDWARE_TEST_LOG.md).

No SDCardService, SPI storage initialization, Preferences/NVS user data, filesystem
mount, OTA updater or persistent settings exists. FS/SPIFFS dependencies and OTA/
SPIFFS partitions alone do not implement these features. External MicroSD is
**proposed**, FEAT-0007; ownership, bounded I/O and card-removal behavior need design.

## Adding and removing apps

Follow [ADDING_AN_APP](ADDING_AN_APP.md) and [engineering workflow](ENGINEERING_WORKFLOW.md).
Registration is in main, hardware remains in shared drivers, state stays app-local,
and onExit releases resources. To remove an app, remove its registration/unused
instance/includes and deliberately handle shared service dependencies, then keep
its registry identity as REMOVED with date/reason and append history/changelog.
No dynamic loader or runtime uninstall exists. Record major architecture changes
under [decisions](decisions/README.md).
