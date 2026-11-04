# Resource Budget & Engineering Limits

Permanent project-wide engineering resource policy and budget specifications for **NibruOS** on the **LILYGO T-Display-S3 Touch Glass** (ESP32-S3R8).

---

## 1. Controls and evidence

Every runtime, memory, storage, timing, peripheral or power change needs the report
below, including fixes, refactors, libraries, flags, tasks and hardware additions.
Documentation-only changes use a short impact assessment. Read
[baseline](performance/BASELINE.md) first; measure before/after where possible.
See [Architecture resource impact](performance/ARCHITECTURE_RESOURCE_IMPACT.md).

| Resource | Control | Evidence / acceptance |
|---|---|---|
| Flash | Image must fit actual app0; currently 6,553,600 B (6.25 MiB) | Generated partitions.bin + firmware.bin; post-build check |
| Internal SRAM | >=150 KiB free internal byte-addressable heap | Physical capability-specific runtime measurement, NOT static linker remainder |
| Largest internal block | Track before/after fragmentation | Physical; >10% decrease requires review, no justified absolute baseline yet |
| PSRAM | Verify detection and usable allocations | Total/free/min/largest measured on device; no assumed 8 MB free |
| FreeRTOS stack | Track important task HWM (ESP-IDF bytes) | Current loop-task snapshot; other tasks need separate instrumentation |
| UI/CPU | Responsive Back/touch; no multi-second work in loop | Record loop/update/render/input timings on device |
| Wi-Fi/BLE | Existing WiFiService and cooperative ownership only | No new radio task/manager; BLE remains unimplemented |
| Storage | Bounded NVS records and checked commits | Settings uses one 5-byte blob; no SD integration |
| GPIO/buses | No pin conflicts, 3.3V logic | Existing LCD/touch/buttons; GPIO38 driver owns backlight |
| Power | Preserve GPIO15 and supply constraints | Current and thermal effects NOT MEASURED; no Deep Sleep |

Image growth >5%, internal minimum heap or largest block drop >10%, repeatable
latency degradation, large retained buffers/tasks/radio allocations all require
engineering review. These percentages are provisional, not universal safety
limits. Missing measurements never constitute PASS. Replace provisional targets
only after representative device evidence and documented justification.

## 2. Blocking conditions

Partition overflow, exhaustion, repeatable allocation/init failures, watchdog
failures, pin/electrical conflicts, verified regressions and unbalanced resource
ownership block acceptance. Investigate persistent lifecycle loss over >=20
cycles: allocator caching or intentionally retained services are not automatically
leaks. The >=150 KiB internal headroom requirement remains a device acceptance
condition until explicitly revised from evidence.

## 3. Ownership and cleanup

| Resource | Owner / lifetime | Cleanup |
|---|---|---|
| Display, touch, buttons | main-owned singleton drivers | Apps reset local state, never reinitialize shared drivers |
| GPIO38 backlight | DisplayManager; AW9364 default | Sleep LOW; wake restores configured step; optional LEDC channel0 |
| Settings NVS | SettingsService, nibru_settings | Close each Preferences handle; never clear unrelated namespaces |
| Saved Wi-Fi/security/vault | Existing owning services, separate namespaces | Settings reset preserves them; no duplicate persistence in UI |
| Wi-Fi scanning | Scanner under ResourceOwner::WiFiScanner | Scanner cancels its job/releases lease; Settings diagnostics is read-only |
| Account import | AccountImportService + explicit TotpApp session; AuthenticatorImport Wi-Fi lease | Close UDP on first candidate/cancel/timeout; wipe frame/key/code/candidate on session exit, lock or sleep; release lease |
| Background connection/NTP | Existing shared services ticked in main | May intentionally survive app exit; reboot explicitly shuts Wi-Fi down |
| Profiler | Fixed snapshots/counters; no new task | No stored history, callback, file or heap buffer to release |

Applications cancel owned jobs, close files, free temporary buffers, unregister
callbacks, stop owned tasks safely and release their locks on exit. Never stop a
shared service owned elsewhere merely because a diagnostics view closes. No new
SPI/I2C/USB/BLE/storage owner is introduced by Settings.

## 4. Standard Resource Impact Report Format

```text
RESOURCE IMPACT REPORT

Change ID:
Feature:
Firmware Version:
Git Commit:
Build Environment:

FLASH:
Before:
After:
Delta:
Application Partition Capacity:
Remaining Headroom:

INTERNAL RAM:
Free Before:
Free After:
Minimum Free:
Largest Free Block:
Observed Delta:

PSRAM:
Free Before:
Free After:
Minimum Observed Free:
Observed Delta:

RUNTIME:
Application Startup:
Maximum Observed Loop/Update Time:
UI Responsiveness:
Resource Cleanup:
Repeated Entry/Exit Test:

OTHER:
Stack Impact:
Peripheral Resource Impact:
Storage Impact:
GPIO Impact:
Power Impact:

VERIFICATION:
Static Analysis:
Unit Tests:
PlatformIO Build:
Physical Hardware Test:

RISK ASSESSMENT:
Regression Findings:
Known Limitations:
Outstanding Verification:

STATUS:
PASS / WARNING / FAIL / NOT MEASURED

EVIDENCE:
Logs, measurements, or relevant test references.
```

## 5. Cross-Platform Desktop Simulator Impact

The Native Desktop Simulator (`platform/desktop/`) introduces zero memory or flash overhead to the physical ESP32-S3 firmware image.
- Desktop-only headers and SDL2 link libraries are isolated strictly to the CMake desktop build target.
- The embedded PlatformIO target (`lilygo-t-display-s3`) compiles exclusively from `src/` and official libraries.
- Firmware size before and after simulator additions remains constant (Flash: 843,521 bytes [12.9%], static RAM: 61,240 bytes [18.7%]).
- Desktop process memory (~25-35 MB) reflects native host CRT, SDL2, and graphics surfaces, and must never be conflated with embedded SRAM or PSRAM metrics.

## 6. Resource Impact Reports
- [Settings resource impact](performance/SETTINGS_RESOURCE_IMPACT.md)
- [Touch fix resource impact](performance/TOUCH_FIX_RESOURCE_IMPACT.md)
- [Architecture resource impact](performance/ARCHITECTURE_RESOURCE_IMPACT.md)

- [Authenticator resource impact](performance/AUTHENTICATOR_RESOURCE_IMPACT.md)
- [Account entry resource impact](performance/ACCOUNT_ENTRY_RESOURCE_IMPACT.md)
- [BLE startup resource impact](performance/BLE_STARTUP_RESOURCE_IMPACT.md)

### Beacon Lab v0.13.0 (FEAT-0024)

- **Flash:** 1,575,856 B versus 1,572,864 B in v0.12.1 (+2,992 B). Generated app0 is 6,553,600 B with 4,977,744 B remaining. The existing warning against the accepted baseline remains.
- **Internal RAM:** PlatformIO static use is 85,968 B versus 85,928 B (+40 B). Free internal heap, largest block and the 150 KiB threshold are NOT MEASURED.
- **PSRAM and stack:** No PSRAM buffer or additional task. One fixed-size frame and SSID buffer are local to the existing app tick. Stack high-water marks and PSRAM headroom are NOT MEASURED.
- **Timing, radio and power:** At most one transmit request per tick, at least 25 ms between requests, for up to ten seconds. Wi-Fi lease is exclusive. Idle checks add no periodic transmit work. Device latency, airtime and power are NOT MEASURED.
- **Acceptance:** Host/desktop, ESP32 build and partition capacity passed. Device scenarios A–E, 20 session cycles, receiver confirmation and radio recovery are NOT RUN. Start with the count at 10, then verify 40 on the same channel and observe that frames stop at timeout, Stop and Back.

### Beacon Lab v0.14.0 (FEAT-0024)

- **Flash:** 1,577,744 B (+1,888 B over v0.13.0), leaving 4,975,856 B in app0. The existing baseline growth warning persists.
- **SRAM and PSRAM:** Static RAM remains 85,968 B. No new task, dynamic allocation or PSRAM buffer. Free internal heap, largest block, stack and PSRAM are NOT MEASURED.
- **Timing and power:** Same minimum transmit spacing of 25 ms, with selectable maximum session duration increased to 60 seconds. Longer sessions may consume more airtime and energy. Latency and power are NOT MEASURED. Radio owner, teardown and GPIO assignments are unchanged.
- **Acceptance:** Host, simulator, ESP32 build and capacity passed. Capture SSIDs and RSN flags using an isolated receiver, and run scenarios A–E including 20 lifecycle cycles and the 150 KiB internal heap check.

### Beacon Lab v0.14.1 (FEAT-0024)

- **Flash:** 1,578,032 B (+288 B over v0.14.0). app0 has 4,975,568 B available. The existing warning against the accepted baseline persists.
- **SRAM/PSRAM and stack:** Linker RAM remains 85,968 B. The existing frame buffer is reused sequentially through the transmit function, with no new task or PSRAM buffer. Internal heap and stack high-water marks were NOT MEASURED.
- **Timing and power:** Three requests per 25 ms batch at most, up to 120 requests per second nominally. Active radio load can rise. Transmit errors terminate the session. Latency, airtime and power were NOT MEASURED.
- **Acceptance:** Host, desktop, ESP32 build and partition capacity passed. Physical capture and resource scenarios A–E including 20 cycles were NOT RUN.
