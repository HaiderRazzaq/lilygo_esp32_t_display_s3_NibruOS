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
- [BLE spam/advertising modes resource impact](performance/BLE_SPAM_RESOURCE_IMPACT.md)

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

### Wi-Fi Channels v0.16.0 (FEAT-0026)

Resource impact report for working tree based on 0e005b9, Windows PlatformIO release target, espressif32 6.5.0 and Arduino-ESP32 2.0.14.

| Metric | Before v0.15.0 | After v0.16.0 | Assessment |
|---|---|---|---|
| Actual firmware.bin | 1,583,328 B | 1,584,960 B | +1,632 B (+0.103%) |
| Linker static RAM | 86,088 B | 86,152 B | +64 B, includes fixed summaries and app state |
| Generated app0 | 6,553,600 B | 6,553,600 B | 4,968,640 B remaining |
| Free/min/largest internal heap | NOT MEASURED | NOT MEASURED | 150 KiB acceptance pending |
| PSRAM and stack HWM | NOT MEASURED | NOT MEASURED | No new task or PSRAM buffer |
| Entry, scan completion and touch latency | NOT MEASURED | NOT MEASURED | Post-scan aggregation is linear in result count |
| Current draw and power | NOT MEASURED | NOT MEASURED | Uses existing explicit async scan |

Only fourteen four-byte summaries and a result count are retained by WiFiService.
The chart has fixed render buffers and loops over fourteen channels only when
invalidated. Existing scan buffers remain owned by the driver and are freed by
the existing scanDelete path. The full-result aggregation reads existing records
without copying additional SSIDs or allocating another AP list. Dense-scan RSSI
and channel reads use the bundled driver's integer-index record accessor because
the Arduino scalar accessors accept only eight-bit indices.

No GPIO, bus, partition, persistence schema or dependency was changed. Wi-Fi is
leased on app entry and a pending scan is cancelled on exit. An established
station connection survives exit. Desktop source-list repairs add the existing
WiFi Defense implementation only to desktop targets.

VER-0207 covers twenty HOST cancellation cycles and standalone/blocked/faulted
control flow. VER-0208 and VER-0209 cover build and image capacity. Physical
startup, entry, exit, twenty cycles and combined connected workloads are NOT RUN
(HWT-0075). No physical leak, headroom, responsiveness or power claim follows
from host results. The accepted baseline and its existing 88.295% image growth
warning are retained.

Status is IMPLEMENTED, verification incomplete for physical resource acceptance.

### BLE Lab v0.17.0 (FEAT-0027)

Resource impact report for the current working tree, Windows release target,
espressif32 6.5.0 and Arduino-ESP32 2.0.14.

| Metric | Before v0.16.0 | After v0.17.0 | Assessment |
|---|---|---|---|
| Actual firmware.bin | 1,584,960 B | 1,588,688 B | +3,728 B |
| Linker static RAM | 86,152 B | 86,248 B | +96 B |
| Generated app0 | 6,553,600 B | 6,553,600 B | 4,964,912 B remaining |
| Internal heap and largest block | NOT MEASURED | NOT MEASURED | Physical acceptance pending |
| PSRAM and stack HWM | NOT MEASURED | NOT MEASURED | No added task or PSRAM buffer |
| UI latency and current draw | NOT MEASURED | NOT MEASURED | Active radio effects pending |

The service uses fixed address/payload storage and one atomic event mailbox.
The controller starts once per session and the checked startup is shared with
the keyboard. Identity rotation queues GAP operations without a new task,
GATT server or per-identity controller restart. Main ticks the service and
stops it on lock, sleep and screensaver. A failed teardown retains ownership.
No GPIO, partition, persistence schema, library or framework was changed.

Startup rejects advertising if internal byte-addressable heap is below 150 KiB
after controller initialization. This gate does not measure the minimum over
the full session. Physical startup, entry, exit, twenty cycles, Wi-Fi combined
workloads, stack, fragmentation, latency and power are NOT RUN (HWT-0076).
VER-0213 covers host lifecycle and failures. VER-0214/VER-0215 cover compilation
and partition fit. Simulator work is explicitly SKIPPED (VER-0216).

The accepted baseline and its existing image-growth warning are retained.
Status is IMPLEMENTED, verification incomplete for physical resource acceptance.

### Pairing profiles v0.18.0 (FEAT-0027)

Actual firmware is 1,589,984 B, +1,296 B from v0.17.0, leaving 4,963,616 B in
the generated 6,553,600 B app0 partition. Linker RAM remains 86,248 B.
Model tables reside in read-only storage. Each payload is bounded to 31 bytes.
The scheduler, radio lease, startup heap gate and session limit are retained.

Pairing modes use connectable advertising, which may introduce controller
connection allocations if a receiver connects. An All session rotates up to
fifty identities but retains one active advertising window and fixed buffers.
No additional task, GPIO, persistence, framework or library is introduced.

Internal free/min/largest heap, PSRAM, stack, responsiveness, active power and
twenty physical lifecycle cycles are NOT MEASURED. HWT-0078 includes these
measurements and connected Wi-Fi coexistence. Host format and control-flow
tests do not establish hardware notifications or memory stability.
The existing accepted-baseline growth warning remains.

### Target picker and Random v0.19.0 (FEAT-0027)

Actual firmware is 1,590,992 B, +1,008 B from v0.18.0, leaving 4,962,608 B in
app0. Linker RAM is 86,264 B, +16 B. The target picker uses eight fixed records
and redraws on selection. Its Back and BOOT handling introduces no task or
storage allocation. Random retains one active advertising window and the
existing timing, duration and teardown controls, while refreshing its random
address and protocol/model fields between windows.

HWT-0080 captured v0.18.0 before-controller internal free heap of 232,452 B and
largest block of 221,172 B. These are one checkpoint, not a complete runtime
baseline. Post-start minima, stacks, PSRAM, latency, power and twenty physical
cycles remain NOT MEASURED for v0.19.0. Random's receiver cache and notification
effects require separate testing. HWT-0081 specifies physical scenarios A-E.
Host tests and image fit pass; simulator validation is explicitly skipped.
The accepted baseline and existing growth warning remain.

### BLE spam/advertising modes v0.20.0 (FEAT-0027)

Actual firmware is 1,592,096 B, +1,104 B from v0.19.0, leaving 4,961,504 B in
app0. Linker RAM is 86,264 B, unchanged. The additional Apple action flags and
five beacon name pointers are read-only data; payload buffers remain the same
fixed 31 bytes. No task, queue, dynamic allocation or second radio stack was
added. Beacon uses the existing non-connectable path; Apple alert profiles use
the existing connectable path. The second picker page reuses the same eight
card records and draws only the current page.

Radio ownership, 400 ms identity windows, 100 ms nominal advertising interval,
two-second GAP operation timeouts and teardown retry are unchanged. HWT-0080's
v0.18.0 heap checkpoint remains the latest physical heap observation. HWT-0085
uploaded v0.20.0 and esptool verified the write hash. HWT-0086 could not start
the application because the board stayed in the ROM USB download mode; a
physical reset is required. For v0.20.0, post-start minima, stack high-water
marks, PSRAM, latency, power and twenty physical cycles are NOT MEASURED.
HWT-0084 defines the pending receiver procedure. Host tests and image fit pass;
simulator validation is skipped by the owner. The accepted baseline and existing
growth warning remain.
