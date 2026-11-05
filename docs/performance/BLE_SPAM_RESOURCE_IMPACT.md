# BLE spam/advertising modes — resource impact

```text
RESOURCE IMPACT REPORT

Change ID: DEV-0070
Feature: FEAT-0027 BLE Lab advertising (Apple Action, Apple Not Your Device,
         BLE Beacon spam, page-two picker)
Firmware Version: v0.20.0
Git Commit: WORKING_TREE
Build Environment: Windows, PlatformIO 6.2.0, espressif32@6.5.0,
                   Arduino-ESP32 2.0.14, TFT_eSPI 2.5.43

FLASH:
Before: 1,590,992 B (v0.19.0)
After: 1,592,096 B (v0.20.0)
Delta: +1,104 B (+0.069%)
Application Partition Capacity: 6,553,600 B (app0)
Remaining Headroom: 4,961,504 B

INTERNAL RAM:
Free Before: NOT MEASURED
Free After: NOT MEASURED
Minimum Free: NOT MEASURED
Largest Free Block: NOT MEASURED
Observed Delta: NOT MEASURED; linker static RAM is 86,264 B before and after

PSRAM:
Free Before: NOT MEASURED
Free After: NOT MEASURED
Minimum Observed Free: NOT MEASURED
Observed Delta: NOT MEASURED; no PSRAM buffer added

RUNTIME:
Application Startup: NOT MEASURED; reuses the existing checked controller start
Maximum Observed Loop/Update Time: NOT MEASURED
UI Responsiveness: Host logic only; physical touch latency NOT MEASURED
Resource Cleanup: Host fake-transport stop/teardown and 20 cycles PASS
Repeated Entry/Exit Test: 20 host cycles PASS; physical 20 cycles NOT RUN

OTHER:
Stack Impact: No task or stack added; high-water marks NOT MEASURED
Peripheral Resource Impact: Same ResourceOwner::BleAdvertising lease, same
  Bluedroid controller, same 31-byte payload buffers
Storage Impact: None; no NVS, file or settings schema change
GPIO Impact: None; no pin added or reassigned
Power Impact: Same 400 ms identity windows and session cap; active current
  NOT MEASURED

VERIFICATION:
Static Analysis: Ads-length bounds reviewed; all builders cap at 31 bytes
Unit Tests: VER-0230 PASS (Apple Action, Not Your Device, Beacon, paging,
  connectable routing, cleanup, 20 cycles)
PlatformIO Build: VER-0231 PASS; app0 fit 1,592,096 / 6,553,600 B
Physical Hardware Test: HWT-0085 upload and write-hash verified; HWT-0086
  runtime boot BLOCKED in ROM download mode pending a physical reset;
  HWT-0084 receiver behavior NOT RUN; no heap, latency or power measurement

RISK ASSESSMENT:
Regression Findings: Existing first-page targets, All cycling and host
  lifecycle assertions are unchanged. Image growth warning is retained.
Known Limitations: Advertisement-only demonstrations; no GATT, pairing,
  authentication, anti-spoofing, TX-power change or faster advertising rhythm.
Outstanding Verification: Physical payload reception, receiver notifications,
  post-start heap/largest block, stack high-water marks, latency, power and
  resource scenarios A-E.

STATUS: WARNING (image-growth trigger retained; runtime metrics NOT MEASURED)

EVIDENCE:
docs/evidence/2026-11-05-v0200-host.txt
docs/evidence/2026-11-05-v0200-build.txt
docs/evidence/2026-11-05-v0200-docs.txt
docs/VERIFICATION_REPORT.md (VER-0230, VER-0231, VER-0232, VER-0233)
docs/HARDWARE_TEST_LOG.md (HWT-0084, HWT-0085, HWT-0086)
```

## Notes

The new Apple action and beacon builders add only constant tables and a few
stack locals. No dynamic allocation, background task, queue, persistence or
radio owner was introduced. Picker page two draws the same eight card records;
it adds no framebuffer.

The provisional image-growth review trigger remains active because the image is
still compared against the older accepted baseline. It is not a partition or
build failure.
