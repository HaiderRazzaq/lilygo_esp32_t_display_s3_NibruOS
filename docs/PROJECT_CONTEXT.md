# NibruOS — Complete project context for AI IDEs

## Purpose and scope
A **single ESP32-S3 firmware** containing a touch-controlled home launcher and embedded modular applications, designed to grow from three apps into a portable multifunction tool. It is **not** an OS with processes, Linux, dynamic executables, and it **does not flash or launch Bruce/GhostESP binaries**. Adding a new native C++ app requires rebuilding/flashing the combined firmware.

## Device
LILYGO T-Display-S3 **Touch Glass** board; ESP32-S3R8 (dual-core), **16MB external Flash, 8MB PSRAM**, 1.9-inch **ST7789V** IPS 170x320, **CST816S** touch, Wi-Fi 2.4GHz, BLE, USB-C, GPIO buttons 0/14, battery sense GPIO4, U.FL footprint for an external antenna arrangement. **Verify the antenna routing and required matching/modification on this specific PCB before switching antenna paths.** Not every radio feature is present without extra modules: no 5GHz Wi-Fi, no onboard NFC/SubGHz/SD slot.

Critical board data: see `docs/HARDWARE_PINOUT.md` including a full GPIO map, expansion header and strict reserved-pin policy. The earlier owner-tested Arduino sketch established display + touch behavior. The existing NibruOS firmware itself was loaded by the user and **Wi-Fi Scanner showed “Scan timed out”**. All other individual functions of the newly modified release still need explicit hardware validation.

## Current release / status
Project baseline v0.1, updated Wi-Fi reliability patch (**v0.1.1 source revision**). Apps: System Info, Touch Test, Wi-Fi Scanner. Windows workflow: VS Code + PlatformIO. **Host-based simulated C++ tests passed**, but the patched firmware has **not** been built against real PlatformIO dependencies in the assistant environment and **not** been flashed/validated on actual hardware. Do not misrepresent it.

Wi-Fi incident: original `WiFiService` called `WiFi.mode(WIFI_STA)` and immediately `WiFi.scanNetworks(true)`, without a STA warmup; used `WiFi.scanDelete()` during shutdown but did not explicitly stop underlying driver scan. `WiFi.scanComplete()` apparently did not deliver results before the application's 16-second timeout. Root cause may still be Arduino-ESP32 event/driver, RF, power or board configuration, and is **not proven**. Patch: `Preparing` stage with 650ms radio warmup, scan polling before timeout, bounded asynchronous scan (300ms/channel), `esp_wifi_scan_stop()` when aborting, radio reset & one retry, status display + log at 115200. Standalone blocking diagnostic sketch under `tools/wifi_scan_diagnostic/` isolates radio independently of launcher. See `docs/WIFI_DIAGNOSTICS.md`.

## Complete code map
- `platformio.ini`: toolchain, board, Arduino build flags, TFT_eSPI parallel bus, libraries, USB CDC.
- `include/BoardConfig.h`: pins, rotation, timeouts, scan limits. All pin values must originate here or TFT setup flags.
- `src/main.cpp`: global singleton objects; begin/loop; register 3 apps; poll buttons/touch/tick in main loop.
- `src/core/App.h`: pure virtual lifecycle and dirty render state; inspect exact methods before implementing.
- `src/core/AppManager.*`: register apps, home paging (3 per page), route gestures, open/close & release on exit, header back action, render.
- `src/core/AppContext.h`: pass display, resources, Wi-Fi service references without global dependencies in apps.
- `src/core/ResourceManager.*`: enforce exclusive Wi-Fi owner; expand conservatively for Bluetooth/USB/storage.
- `src/hardware/DisplayManager.*`: TFT_eSPI init, drawing primitives and styles.
- `src/hardware/TouchManager.*`: CST816S exact wiring, reset sequence, raw/landscape coordinate mapping, rotated swipe decoding.
- `src/hardware/ButtonManager.*`: GPIO0/14 active low, short/long press detection, always-accessible Back.
- `src/services/WiFiService.*`: non-blocking scan state machine and results in fixed-capacity array.
- `src/apps/SystemInfoApp.*`: ESP/heap/flash/PSRAM/uptime stats.
- `src/apps/TouchTestApp.*`: raw + converted touch coordinates and gesture.
- `src/apps/WiFiScannerApp.*`: resource owner, scan UI, rescan, scroll, exit.
- `tests/host/*`: stubs and local smoke suite, not a full ESP/driver emulator.
- `tools/wifi_scan_diagnostic/*`: independent temporary diagnostic (reflashing required).

## Runtime flow
1. `setup()` starts Serial at 115200, initializes display (`GPIO15 HIGH`, `GPIO38 HIGH`, `tft.init()`, rotation 1), then touch (GPIO16 pull-up, GPIO21 reset pulse, `touch.begin(FALLING)`), then buttons.
2. `AppManager` registers and draws Home with the three apps.
3. `loop()` reads touch/button events; passes events to AppManager, then calls current app `onTick()` and redraws only on invalidate; delay(12) yields.
4. On app opening, manager invokes `onEnter(ctx)`. Wi-Fi Scanner acquires radio then begins state-machine init; on each tick calls WiFiService `tick`. On Back calls `onExit` which stops/releases radio.

## UI rules
- Display 320×170 pixels, landscape, `tft.setRotation(1)`.
- Home 3 app tiles per page. Tap tile to open; BOOT short select next, BOOT hold activate; GPIO14 always returns home. Tap top-left back arrow inside apps.
- Touch transform screenX=rawY, screenY=169-rawX, clipped to screen bounds; swipe directions rotated in driver. Verify actual perceived swipe on board; the earlier sketch confirmed coordinate mapping, not necessarily every rotated swipe enum.
- Never let scan, file I/O, sensor reads, or network connect block > a few milliseconds per `loop()` iteration.

## Build / dependencies / tests
`platformio.ini`: `platform=espressif32@6.5.0`, board `lilygo-t-display-s3`, framework `arduino`, TFT_eSPI `2.5.43`, CST816S GitHub source. `board_build.arduino.memory_type=qio_opi`, `board_build.partitions=default_16MB.csv`, native USB CDC flags, PSRAM flag. Confirm outputs after build; these settings were inherited from the earlier project and must not be changed casually. See `docs/BUILD_AND_TEST.md` for Windows build/flash and test procedure. Host C++ tests use stubs and check navigation, input, Wi-Fi scan states/retries, resource ownership.

## Next work / roadmap
1. User flashes patched build and attaches serial log if Wi-Fi scan still fails. Verify touch gestures separately and confirm button input works after USB reconnection.
2. Validate build, `.bin` size, flash/PSRAM reports; check PSRAM test, network scan, exit/re-enter and repeated rescans.
3. Add app-specific error reporting, richer Wi-Fi results, lazy rendering of large lists.
4. Optional safe BLE scanner / sensor apps / NVS-backed settings. Integrate any secret vault only after dedicated security design (device lock, at-rest confidentiality, key management, safe provisioning, threat model).
5. As more apps arrive, review flash partition, heap/PSRAM, radio coexistence, task scheduling, USB HID security, and OTA integrity/rollback.

## AI agent contract
Preserve user-confirmed pins. Inspect entire affected flow before editing. Never randomly transplant firmware code; port features via cohesive App + service architecture. Avoid global and duplicate hardware init. Keep resources balanced. Update docs/tests and report what was actually executed. If you require new sensors/modules, specify exact pin wiring and power/voltage/current considerations first.
