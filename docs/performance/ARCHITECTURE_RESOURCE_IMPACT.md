# Architecture & Modular Refactoring Resource Impact Report

```text
RESOURCE IMPACT REPORT

Change ID: DEV-0021 / ADR-0002
Feature: FEAT-0001, FEAT-0005, FEAT-0006, FEAT-0015
Firmware Version: v0.3
Git Commit: 8239bb6 + modular refactor
Build Environment: PlatformIO lilygo-t-display-s3, espressif32@6.5.0, Arduino-ESP32 2.0.14, default release -Os

FLASH:
Before: 843,521 bytes (linked), 843,888 bytes (firmware.bin)
After:  843,565 bytes (linked), 843,936 bytes (firmware.bin)
Delta:  +44 bytes linked (+0.005%), +48 bytes binary (+0.005%)
Application Partition Capacity: 6,553,600 bytes (app0)
Remaining Headroom: 5,709,664 bytes (87.1% free partition space)

INTERNAL RAM:
Before: 61,240 bytes static data/BSS (18.7%)
After:  61,240 bytes static data/BSS (18.7%)
Free Before: NOT MEASURED on physical hardware (static remainder 266,440 B)
Free After:  NOT MEASURED on physical hardware (static remainder 266,440 B)
Minimum Free: NOT MEASURED (device acceptance pending)
Largest Free Block: NOT MEASURED (device acceptance pending)
Observed Delta: 0 bytes static RAM change

PSRAM:
Free Before: NOT MEASURED
Free After:  NOT MEASURED
Minimum Observed Free: NOT MEASURED
Observed Delta: 0 bytes (no PSRAM buffers allocated by architecture refactor)

RUNTIME:
Application Startup: Expected non-blocking boot sequence preserved
Maximum Observed Loop/Update Time: NOT MEASURED on hardware; desktop simulator runs 60 FPS
UI Responsiveness: Maintained; HomeScreen rendering is zero-allocation and delegates directly
Resource Cleanup: Guaranteed; onExit() lifecycle releases all acquired leases
Repeated Entry/Exit Test: 25 cycles verified in desktop automated test suite with zero leaks

OTHER:
Stack Impact: Minimal; stack variables for HomeScreen tile math are bounded primitives
Peripheral Resource Impact: None; ST7789, CST816S, and BOOT/USER buttons unchanged
Storage Impact: None; NVS schema unchanged (5-byte blob)
GPIO Impact: Zero extra GPIO allocated; PinDefinitions centralizes verified pins and marks SPI MicroSD as unverified
Power Impact: NOT MEASURED on physical hardware; GPIO15 and AW9364 pulse modulation preserved

VERIFICATION:
Static Analysis: check_docs.py PASS; check_resources.py PASS (0.26% growth vs baseline, limit 5%)
Unit Tests: 17 documentation tests PASS; 6 resource tests PASS; host Zig smoke tests PASS
PlatformIO Build: SUCCESS across lilygo-t-display-s3, profile, and ledc environments
Physical Hardware Test: NOT RUN (pending device observation HWT-0012)

RISK ASSESSMENT:
Regression Findings: Zero compilation errors, zero link errors, zero test regressions.
Known Limitations: Host tests and desktop simulator verify control flow, UI geometry, and state persistence, but cannot substitute for physical RF Wi-Fi reception or AW9364 pulse dimming on real hardware.
Outstanding Verification: Hardware acceptance test HWT-0012.

STATUS:
PASS for compile, software, and simulation scopes; NOT MEASURED for physical runtime metrics.

EVIDENCE:
Build evidence in docs/evidence/2026-11-01-architecture-builds.txt and docs/evidence/2026-11-01-architecture-refactor-tests.txt.
```
