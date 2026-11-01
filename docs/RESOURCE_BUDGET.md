# Resource Budget & Engineering Limits

Permanent project-wide engineering resource policy and budget specifications for **NibruOS** on the **LILYGO T-Display-S3 Touch Glass** (ESP32-S3R8).

---

## 1. Controls and evidence

Every runtime, memory, storage, timing, peripheral or power change needs the report
below, including fixes, refactors, libraries, flags, tasks and hardware additions.
Documentation-only changes use a short impact assessment. Read
[baseline](performance/BASELINE.md) first; measure before/after where possible.

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

## 4. Cross-Platform Desktop Simulator Impact

The Native Desktop Simulator (`platform/desktop/`) introduces zero memory or flash overhead to the physical ESP32-S3 firmware image.
- Desktop-only headers and SDL2 link libraries are isolated strictly to the CMake desktop build target.
- The embedded PlatformIO target (`lilygo-t-display-s3`) compiles exclusively from `src/` and official libraries.
- Firmware size before and after simulator additions remains constant (Flash: 843,521 bytes [12.9%], static RAM: 61,240 bytes [18.7%]).
- Desktop process memory (~25-35 MB) reflects native host CRT, SDL2, and graphics surfaces, and must never be conflated with embedded SRAM or PSRAM metrics.
