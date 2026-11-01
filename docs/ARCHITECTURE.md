# Architecture — current firmware

One in-process Arduino firmware. main.cpp owns singleton drivers and services,
AppContext passes references, AppManager routes one active app. Apps are WiFiApp,
TotpApp and SettingsApp; SystemInfoApp/TouchTestApp/WiFiScannerApp code is retained,
with scanner and touch diagnostics delegated by their parent views.

DisplayManager alone owns TFT_eSPI, GPIO15 power and GPIO38 brightness/sleep;
TouchManager alone owns CST816S initialization and raw/landscape transformation;
ButtonManager owns GPIO0/14 debounce. The tested pin/reset/rotation setup is
preserved. Raw UP -> visible LEFT, DOWN -> RIGHT, LEFT -> DOWN, RIGHT -> UP;
actual gesture acceptance still requires physical testing.

AppManager has 12 fixed app slots and 3 Home tiles per page. App::onEnter runs on
entry, onExit before replacement/Home, onTick/render only for the active app.
BOOT short dispatches onPrimaryAction; hold dispatches onActivate (defaults to
legacy primary action for other apps). Settings uses separate focus/activation.
Header/GPIO14 Back first lets onBack consume nested navigation/cancel, then Home.
No new task, scheduler, framebuffer or dynamic application loader exists.

main ticks WiFiService and TimeService independently of the active app. Existing
saved-network auto-connect/NTP may survive app exit. ResourceManager is a
cooperative scanner lease, not an OS lock or a complete radio lifecycle policy.
WiFiScannerApp owns its asynchronous scan and releases it on exit; Settings
Wi-Fi diagnostics only reads existing service state. Reboot explicitly stops
Wi-Fi, releases scanner ownership, locks an enabled security session, flushes
Serial and restarts. Settings reset preserves Wi-Fi/security/2FA namespaces.

SettingsService is the single owner of brightness/timeout/accent persistence.
An atomic 5-byte versioned blob replaces the older multi-key layout; legacy
imports are non-destructive. Updates persist before applying to affected display
controls; unchanged values avoid writes and failed writes leave runtime values
unchanged. No duplicate Preferences access in SettingsApp. Other preexisting
services retain their own unrelated namespaces. See [Settings](SETTINGS.md).

Timeout is non-blocking uint32 arithmetic. Wake consumes the initiating event,
held contact/button and follow-up gestures until released plus 350ms quiet.
TouchManager tracks raw contact separately from semantic dispatch. One action is
latched per contact; NONE/down waits for a classified gesture or release. New
Down phase recovers a missed Up, and contact identity/start time protect delayed
confirmations. A confirm press must start at least500ms after the dialog opens.
No Deep Sleep. AppManager invalidate calls onDisplayRestored; Settings and its
Touch Test invalidate their static drawing caches when the PIN overlay is removed.
Same-page Settings updates use partial redraw; no framebuffer is allocated.

MemoryProfiler uses capability-specific internal byte-addressable heap and PSRAM
APIs, sketch/flash/reset/uptime and current-task HWM. Host returns unavailable,
not device metrics. Sparse NIBRU_PROFILE logs startup/app transitions and fixed
max loop timing; no history buffers, new callbacks/tasks or continuous telemetry.
The profile environment is separate; [profiling](MEMORY_PROFILING.md) records its
measurement overhead and unsupported metrics.

No storage bus driver, SD mount/exporter, BLE service or OTA updater exists.
The platform supports two build targets sharing identical application code:
the ESP32-S3 firmware target (`pio run -e lilygo-t-display-s3`) and the Windows
Desktop Simulator target (`cmake --build build`). The desktop target pairs SDL2
with a 320x170 RGB565 framebuffer, nearest-neighbor scaling, and mock hardware
services (`MockWiFiService`, `DesktopStorage`, `DesktopInput`). See
[Desktop Simulator](DESKTOP_SIMULATOR.md) and [Simulator Architecture](SIMULATOR_ARCHITECTURE.md).
Any future app must follow [adding an app](ADDING_AN_APP.md), ownership cleanup,
[resource controls](RESOURCE_BUDGET.md) and [engineering workflow](ENGINEERING_WORKFLOW.md).
Firmware build/mock tests cannot establish runtime headroom, physical wiring,
radio reception, touch latency or power safety.
