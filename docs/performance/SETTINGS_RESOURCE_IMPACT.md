# RESOURCE IMPACT REPORT

Change ID: FEAT-0013 / DEV-0014 / DEV-0015
Feature: Essential Settings completion and permanent resource controls
Firmware Version: v0.3 (unchanged)
Git Commit: 8239bb62d28ae5ac9c736033c7636c4e82ac8f30 + uncommitted changes
Build Environment: Windows; PlatformIO lilygo-t-display-s3; espressif32@6.5.0;
Arduino-ESP32 2.0.14; Xtensa GCC 8.4.0; release -Os; TFT_eSPI 2.5.43; CST816S Git.

## FLASH

Before: firmware.bin **841,744 B**; linked program **841,377 B**.
After: firmware.bin **842,848 B**; linked program **842,481 B**.
Delta: image **+1,104 B (+0.1312%)**; linked program **+1,104 B**.
Application Partition Capacity: **6,553,600 B**, read from generated app0 OTA0.
Remaining Headroom: **5,710,752 B** (image capacity, not entire Flash).
No 5% growth warning triggered; this is not runtime acceptance.

| Build | Image bytes | Linked Flash bytes | Static RAM bytes | Remaining app0 |
|---|---:|---:|---:|---:|
| Before (8239bb6) | 841744 | 841377 | 61168 | 5711856 |
| Default AW9364 | 842848 | 842481 | 61200 | 5710752 |
| Profiling enabled | 843424 | 843061 | 61200 | 5710176 |
| Opt-in LEDC | 848608 | 848241 | 61272 | 5704992 |

Default SHA-256: `f170c68ce03ee3e8967f648598942c5c162067d69e454060e7a384e08a04fb9b`.
Profile SHA-256: `33891c46b181e5a653e613da04712c7c933d964cdfcc32f019a0fc4e03dab0d6`.
LEDC SHA-256: `2b1999426ad92cd1ef0d648032a211b88acc923818f3c04c0ae010e04d00b4cf`.

## INTERNAL RAM

Free Before: NOT MEASURED.
Free After: NOT MEASURED.
Minimum Free: NOT MEASURED.
Largest Free Block: NOT MEASURED.
Observed Delta: NOT MEASURED at runtime. Static data/BSS **+32 B** default;
this does not establish the >=150 KiB internal free-heap requirement.

## PSRAM

Free Before: NOT MEASURED.
Free After: NOT MEASURED.
Minimum Observed Free: NOT MEASURED.
Observed Delta: NOT MEASURED. No application PSRAM buffer/allocator introduced.

## RUNTIME

Application Startup: NOT MEASURED on device; sparse startup/entry checkpoints exist.
Maximum Observed Loop/Update Time: NOT MEASURED on device; profile build tracks
main loop work maximum, excluding delay(12), with no continuous loop logging.
UI Responsiveness: HOST control-flow assertions PASS; physical touch/redraw and
USB/battery responsiveness NOT MEASURED.
Resource Cleanup: HOST verifies Settings owns no scanner lease, Back/Home and
reboot cleanup; shared existing Wi-Fi/NTP may intentionally outlive app exit.
Repeated Entry/Exit Test: 20 Settings cycles HOST PASS; physical leak/fragmentation
scenario D is NOT RUN. No runtime memory delta can be inferred from these mocks.

## OTHER

Stack Impact: No new FreeRTOS task. Small fixed local snapshots/format buffers;
actual high-water/latency effects NOT MEASURED. Current-task HWM instrumentation
uses ESP-IDF byte units; it does not measure every task.
Peripheral Resource Impact: No new radio manager/bus/driver allocation; diagnostics
reads existing Wi-Fi. LEDC channel0 reserved only by opt-in environment.
Storage Impact: one versioned 5-byte NVS blob, handles scoped; unchanged values
skip writes. No SD/filesystem/OTA. Atomic NVS fault behavior is host-simulated,
not a physical power-loss test.
GPIO Impact: Existing reservations preserved; touch adds contact state only.
GPIO38 AW9364 default retained. Optional LEDC requires physical compatibility.
Power Impact: No Deep Sleep or supply change; backlight off during inactivity.
Actual current/thermal/battery effects NOT MEASURED.

## VERIFICATION

Static Analysis: diff/resource/lifecycle review; independent source review
identified NVS load/migration, held-wake and raw-double-tap cases; fixes have
host regression coverage. Source review is not a full static-analysis tool run.
Unit Tests: six artifact tests and 17 documentation tests; see VER-0018.
Host Simulation: full firmware mocks plus main compile, default and combined
profile/LEDC modes PASS (VER-0017). Crypto mocks do not validate cryptography.
PlatformIO Build: all three target environments PASS with actual image/partition
post-checks (VER-0020). Existing TFT parallel DMA/TOUCH_CS warnings retained.
Physical Hardware Test: NOT RUN (HWT-0008); no upload/serial session in this change.

## RISK ASSESSMENT

Regression Findings: failed intermediate attempts and corrections retained in
[failed-attempt evidence](../evidence/2026-11-01-settings-failed-attempts.txt).
Known Limitations: no device baseline or A–E data; actual backlight steps,
capacitive contact/release timing, radio timeout, NVS power-loss recovery,
heap/PSRAM/stack/latency/power remain unverified. Existing Security/2FA behavior
is retained without expansion or independent security validation. CST816S remains
unpinned (BUG-0002). Serial profiling perturbs timing.
Outstanding Verification: HWT-0008 and [scenarios A–E](../MEMORY_PROFILING.md).
The previous approximate runtime baseline is withdrawn, not accepted or erased
from history. See [baseline correction](BASELINE.md).

STATUS: **WARNING — IMPLEMENTED, verification incomplete**.
Software/build gates pass; runtime resource acceptance remains NOT MEASURED.

## EVIDENCE

[Before build](../evidence/2026-11-01-settings-before-build.txt),
[final builds](../evidence/2026-11-01-settings-target-builds-final.txt),
[artifact reports](../evidence/2026-11-01-settings-artifact-final.txt),
[host default](../evidence/2026-11-01-settings-host.txt),
[host profile/LEDC](../evidence/2026-11-01-settings-host-ledc-profile.txt),
[resource tests](../evidence/2026-11-01-settings-resource-tests.txt).

## Subsequent authorized upload — 2026-11-01

User requested uploading the latest change. Fresh default build/capacity/hash
check passed (VER-0021); the identical 842,848-byte AW9364 image was flashed on
COM5 with data hash verified and RTS reset command sent (HWT-0009). No firmware
source/configuration or resource cost changed during upload. No full-flash erase,
preferences reset, runtime serial capture or app interaction was performed.
The earlier NOT RUN hardware statement refers to implementation-stage testing;
physical flash writing is now verified, while HWT-0008 and all runtime memory,
stack, latency, power, touch/brightness and radio acceptance remain pending.
