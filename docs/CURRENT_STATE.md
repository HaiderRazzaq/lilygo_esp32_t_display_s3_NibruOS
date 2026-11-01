# Current state

Snapshot recorded **2026-11-01, Asia/Baghdad (UTC+03:00)**. This is a mutable
snapshot; [development history](DEVELOPMENT_LOG.md) is append-only.

```json
{
  "record_type": "state",
  "date": "2026-11-01",
  "runtime_version": "v0.3",
  "source_patch_label": "v0.1.1-source-patch (legacy documentation label)",
  "branch_at_baseline": "main",
  "baseline_commit": "ccbd0e625837196ce547f16788022be6465786cb",
  "working_tree": "Clean at discovery; engineering-memory changes are uncommitted",
  "implemented_features": ["FEAT-0001", "FEAT-0002", "FEAT-0003", "FEAT-0004", "FEAT-0005", "FEAT-0006", "FEAT-0008", "FEAT-0009", "FEAT-0010", "FEAT-0011", "FEAT-0012", "FEAT-0013"],
  "planned_features": ["FEAT-0007"],
  "active_issues": ["BUG-0001", "BUG-0002", "BUG-0003", "BUG-0004", "BUG-0005"],
  "latest_build": "VER-0010",
  "documentation_verification": "VER-0005",
  "current_host_attempt": "VER-0002",
  "source_constants": {
    "kTouchSda": 18, "kTouchScl": 17, "kTouchReset": 21,
    "kTouchInterrupt": 16, "kButtonBoot": 0, "kButtonUser": 14,
    "kDisplayPower": 15, "kDisplayBacklight": 38,
    "kScreenWidth": 320, "kScreenHeight": 170, "kRotation": 1,
    "kTouchDebounceMs": 180,
    "kWiFiWarmupMs": 650, "kScanTimeoutMs": 11000,
    "kScanMsPerChannel": 300, "kScanMaxAttempts": 2,
    "kConnectTimeoutMs": 20000
  }
}
```

## Firmware and repository baseline

Single Arduino/PlatformIO ESP32-S3 firmware; target LILYGO T-Display-S3 **Touch
Glass**, ESP32-S3R8, 16 MB flash/8 MB PSRAM. Configured platform 6.5.0/core
2.0.14; [resolved dependencies](DEPENDENCIES.md). Runtime Home and serial say
**v0.1**. No centralized version constant, version manifest or Git tag exists.
The Wi-Fi patch is present despite older titles; no version bump made here.

Git has two commits, is not shallow, and provides no earlier development
history: `d6cf029` (2026-10-26, complete imported source including Wi-Fi patch)
and `ccbd0e6` (2026-10-27, schematic PDF only). No claim that the `stable`
commit title proves device stability. New documentation describes an uncommitted
working-tree change on the baseline, not a new code revision or release.

## Features and evidence

System Info, Touch Test, Wi-Fi Scanner with AP connection and virtual on-screen keyboard,
Home/navigation/input and the independent radio diagnostic are implemented.
See the [feature registry](FEATURE_REGISTRY.md) for exact modules, resource contracts
and test references. The target build has passed; firmware was uploaded to ESP32-S3 on COM3.
Documentation tooling has its own tests; it is not a firmware feature or a physical test.

Owner-reported independent-sketch display/touch success is [HWT-0001](HARDWARE_TEST_LOG.md).
An earlier flashed NibruOS scan timed out ([HWT-0002](HARDWARE_TEST_LOG.md)).
That failure's root cause is unconfirmed. The patched Wi-Fi code, rotated
gestures, buttons, launcher responsiveness, actual flash/PSRAM reports and
memory stability all await explicit tests of this firmware on the board.
Nothing here establishes that Wi-Fi reception currently works.

No SD driver/service, storage app, persistence, OTA, BLE service, vault, TOTP,
NFC or SubGHz feature exists. SPIFFS/FS being linked by TFT_eSPI and partition
space reserved for SPIFFS/OTA do not implement storage or OTA functionality.

## Hardware allocation and installation

Integrated LCD/touch, buttons, radio, flash/PSRAM, USB and battery ADC are
reserved as detailed in [pinout](HARDWARE_PINOUT.md). No external module is
confirmed physically connected. Owner reports owning an SPI MicroSD module;
CS10/MOSI11/SCK12/MISO13 at 3.3V are **proposed/unverified**, not configured in
firmware. Antenna ownership/routing and battery configuration need confirmation;
see [inventory](HARDWARE_INVENTORY.md). GPIO10–13 remain expansion candidates
pending actual allocation and SD/shield conflict review.

## Latest actual verification

Target build on this Windows machine: PASS; RAM 45,976/327,680 bytes, linked
flash 741,117/6,553,600 bytes (the app partition is 6.25 MiB, not all 16 MB).
Recorded in [VER-0006](VERIFICATION_REPORT.md). Device upload performed on COM3.
These are build-time figures, not measured free heap/PSRAM or a memory soak test.

## Next recommended work

1. Follow [physical acceptance](BUILD_AND_TEST.md) and HWT-0003: boot, exact
   display/touch mapping, four swipe directions, buttons and Home escape.
2. Run HWT-0004 and the Wi-Fi diagnostics; save sanitized serial output and
   exact image hash, then update BUG-0001 only from that evidence.
3. Test virtual keyboard touch input and Wi-Fi connection with visible password.
4. Run HWT-0005: 20 Wi-Fi/Home cycles and sustained memory observation on USB,
   then appropriate battery power. Do not equate linker RAM usage with stability.
5. Confirm MicroSD module model, voltage/current and physical wiring before
   implementing FEAT-0007 or running HWT-0006.
