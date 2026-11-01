# Memory profiling and resource verification

The [permanent policy](RESOURCE_BUDGET.md) applies to every meaningful change.
Start from an identified image/build and a sanitized serial capture; follow
[baseline](performance/BASELINE.md) and keep results by verification level.

## Instrumentation and commands

`MemoryProfiler::capture()` is available for diagnostics in every build. On the
ESP32 it uses INTERNAL|8BIT heap free/min/largest, PSRAM total/free/min/largest,
ESP sketch/flash APIs, reset reason, uptime and current loop-task stack HWM.
ESP-IDF's stack HWM unit is **bytes**. These are capability-specific pools;
ESP.getFreeHeap() and unused linker RAM must not be labeled free internal SRAM.
The host returns `available=false`, no invented device numbers, and a HOST label.

```powershell
pio run -e lilygo-t-display-s3
python tools/check_resources.py
pio run -e lilygo-t-display-s3-profile
# Upload only when intended; replace COM7 with the observed port.
pio run -e lilygo-t-display-s3-profile -t upload --upload-port COM7
pio device monitor -p COM7 -b 115200
```

NIBRU_PROFILE defaults to 0; the profile environment enables it. Fixed snapshots
are printed at startup-basic, display-touch-ready, home-services-ready,
before-entry, after-entry and after-exit. App entry/exit durations are logged.
Main loop work maximum excludes delay(12) and includes input, service ticks and
rendering; no per-loop serial output or unbounded history. Snapshot formatting
and serial transport perturb entry/exit and loop timing: compare both profile
and ordinary builds before accepting latency. Current instrumentation measures
only the task executing it, not all FreeRTOS stacks or CPU utilization. Touch
latency and full redraw latency require targeted physical measurement. No new
FreeRTOS task, file or filesystem exporter exists. Capture serial to the host;
SD export is deferred until a real storage service exists.

## Record with every session

Date/time, board/revision, supply, connected peripherals, exact image SHA-256,
firmware version/commit/dirty state, build environment/flags/dependencies,
partition allocation, active Wi-Fi/BLE, user brightness/timeout/accent, and reset
reason. Never log credentials, tokens, PINs or secrets. Record instrument overhead
and compare equivalent workload/configuration before and after.

## Scenario A — Startup

Capture startup-basic, display-touch-ready, and home-services-ready. Inspect
actual flash/PSRAM detection and free/min/largest internal and PSRAM pools;
collect loop stack HWM. Note initialization failures, brownouts and watchdogs.
Basic checkpoint is after Serial.begin; earlier startup allocations are outside
its observation. Boot Wi-Fi auto-connect may change later measurements.

## Scenario B — Application entry/workload

Capture before-entry and after-entry for Settings. Visit display/theme,
system/memory pages, Wi-Fi diagnostics and device controls, plus preserved
subviews if used. Record normal/heaviest workload memory and observed max update,
redraw/touch latency. Memory page samples current APIs once per second. Capture
Wi-Fi async scan separately, without inventing concurrent active applications.

## Scenario C — Exit

Exit via Back to menu then Home; also test app replacement. Capture after-exit
and compare to pre-entry. Settings owns no radio lease/task/file/buffer.
Scanner releases its scan lease; the existing shared Wi-Fi connection and NTP
may intentionally persist and must be recorded, not misclassified as a leak.

## Scenario D — Repeated lifecycle

At least 20 entry/workload/exit cycles for Settings and relevant Wi-Fi workload.
Record every cycle's free/min/largest internal and PSRAM values, stack HWM,
resource owner and max loop time. Investigate repeatable losses and caches;
a one-time free-heap reduction alone is not proof of leakage. Host 20-cycle
assertions only validate control flow/ownership, never physical leak freedom.

## Scenario E — Combined workloads

Display/touch/buttons plus supported background Wi-Fi connection/NTP while in
Settings; separately display/touch plus asynchronous Scanner. Check Back/wake,
allocation/init failures, radio ownership and >=150 KiB internal headroom.
No SD/BLE combination exists; do not claim it tested. Observe power and thermal
behavior on USB then suitable battery supply when physically available.

## Acceptance and unavailable metrics

Image must fit actual app0. Run artifact checks; >5% image growth is a warning
requiring review. Internal minimum/largest drops >10% and repeatable latency
regression require investigation. Allocation/watchdog/ownership failures block
acceptance. All device scenarios remain NOT RUN until captured on the actual
image. Separate STATIC, UNIT/DOCUMENTATION, HOST SIMULATION, BUILD and HARDWARE
results; unknown heap/stack/power data remains NOT MEASURED.
