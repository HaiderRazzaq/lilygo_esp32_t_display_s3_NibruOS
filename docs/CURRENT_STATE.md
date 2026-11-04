# Current state

Snapshot: 2026-11-03, Asia/Baghdad. Implementation and physical acceptance are
separate; [history](DEVELOPMENT_LOG.md) is append-only.

```json
{
  "record_type": "state",
  "date": "2026-11-04",
  "runtime_version": "v0.9.3",
  "source_patch_label": "v0.1.1-source-patch (legacy documentation label)",
  "branch_at_baseline": "main",
  "baseline_commit": "8239bb6",
  "working_tree": "Screen orientation control (0 deg/180 deg), architecture audit, layer boundary hardening, single-source-of-truth configuration alignment (ADR-0003), zero compiler warnings",
  "implemented_features": [
    "FEAT-0001",
    "FEAT-0002",
    "FEAT-0003",
    "FEAT-0004",
    "FEAT-0005",
    "FEAT-0006",
    "FEAT-0008",
    "FEAT-0009",
    "FEAT-0010",
    "FEAT-0011",
    "FEAT-0012",
    "FEAT-0013",
    "FEAT-0014",
    "FEAT-0015",
    "FEAT-0016",
    "FEAT-0017",
    "FEAT-0018",
    "FEAT-0019"
  ],
  "planned_features": [
    "FEAT-0007"
  ],
  "active_issues": [
    "BUG-0001",
    "BUG-0002",
    "BUG-0003",
    "BUG-0004",
    "BUG-0005",
    "BUG-0007",
    "BUG-0008",
    "BUG-0009",
    "BUG-0010",
    "BUG-0011",
    "BUG-0015",
    "BUG-0016"
  ],
  "latest_build": "VER-0115",
  "latest_upload": "HWT-0035",
  "documentation_verification": "VER-0116",
  "current_host_attempt": "VER-0113",
  "source_constants": {
    "kTouchSda": 18,
    "kTouchScl": 17,
    "kTouchReset": 21,
    "kTouchInterrupt": 16,
    "kButtonBoot": 0,
    "kButtonUser": 14,
    "kDisplayPower": 15,
    "kDisplayBacklight": 38,
    "kScreenWidth": 320,
    "kScreenHeight": 170,
    "kRotation": 1,
    "kTouchDebounceMs": 180,
    "kWiFiWarmupMs": 650,
    "kScanTimeoutMs": 11000,
    "kScanMsPerChannel": 300,
    "kScanMaxAttempts": 2,
    "kConnectTimeoutMs": 20000
  }
}
```

One ESP32-S3 Arduino firmware, runtime **v0.3**. Current source registers Wi-Fi
Hub, 2FA Authenticator and Settings. Settings contains System/memory diagnostics,
Touch Test, brightness/timeout/accent, Wi-Fi status and confirmed reboot/reset;
existing Security/PIN subviews are retained. No new advanced settings implemented.
[Settings behavior](SETTINGS.md), [architecture](ARCHITECTURE.md).

Pre-change build at 8239bb6: linked Flash 841,377 B; static RAM 61,168 B;
firmware.bin 841,744 B. Actual app0 is 6,553,600 B. Current costs and exact image
hashes are in [Touch repair resource report](performance/TOUCH_FIX_RESOURCE_IMPACT.md).
The old approximate physical heap/PSRAM baseline is withdrawn as unsupported.
All free/min/largest heap, stack, latency and power acceptance remains
**NOT MEASURED** until the current image is observed on hardware.

Settings uses one validated versioned NVS blob; failed writes preserve previous
runtime settings, unread/corrupt/unknown schema is write-protected until reload
or explicit reset. Reset touches only the three user preferences, preserving
credentials/security/vault/unrelated NVS. GPIO38 AW9364 default retained; optional
LEDC environment is not physically accepted. No pins, partitions or framework
versions changed. Profiling is off by default and available as a separate build.

The owner verified an older independent display/touch sketch (HWT-0001). Earlier
NibruOS scan timed out (HWT-0002); BUG-0001 remains FIXED — UNVERIFIED physically.
Latest repaired default firmware re-upload on explicit request succeeded (HWT-0013):
843,888 B written, hash verified, RTS reset sent. Current build is VER-0024.
No repaired serial runtime/UI acceptance performed; HWT-0012 and full profiling remain NOT RUN.
No commit or push performed. Host cycles
are control-flow tests and do not establish leak freedom or radio reception.

No SD driver/file manager, BLE settings or OTA implementation exists. SPIFFS/OTA
partition reservations do not implement features. External MicroSD reader is owned, but the owner confirmed it is NOT connected to the device; GPIOs 10–13 remain unallocated and floating.

Next: perform HWT-0008 and profiling scenarios A–E; >=20 Settings/Wi-Fi cycles,
brightness, timeout/wake/confirmation behavior, NVS restart/reset preservation,
actual heap/stack/PSRAM/latency and power. Do not mark hardware readiness from
build or mock PASS. Use the [permanent budget](RESOURCE_BUDGET.md).

## Latest owner-reported regression and repair

Owner reported repeated touch actions and Settings flashes after HWT-0009
(HWT-0010 FAIL, BUG-0008). Per-contact dispatch/release handling and incremental
Settings paint are implemented; final host and3 target builds pass
(VER-0023/VER-0024). Current default image843888 B, static RAM61240 B;
[focused resource impact](performance/TOUCH_FIX_RESOURCE_IMPACT.md).
Do not equate software PASS or upload with the repaired physical UI outcome;
HWT-0012 and complete memory/stack/latency/power scenarios remain NOT RUN.

## Authenticator phase one — 2026-11-01
This supersedes older current-build counts above without changing historical
device/upload outcomes. FEAT-0014 now provides empty-first manual name/masked
secret entry, authenticated atomic persistence/legacy reads, valid-time gating
and cancelable USB. New installations require user PIN provisioning; credential
changes are blocked while TOTP storage exists (BUG-0010).
Current default image822912 B/static RAM49248 B; opt-in HID858256 B/static61368 B.
All four builds PASS VER-0035; host/CNG and simulator PASS VER-0033/VER-0034.
Latest physical upload remains historical HWT-0014: this source was NOT flashed.
HWT-0015 and heap/PSRAM/stack/latency/power remain NOT RUN / NOT MEASURED.
See [operation](AUTHENTICATOR.md) and [impact](performance/AUTHENTICATOR_RESOURCE_IMPACT.md).

## Authorized default upload — 2026-11-02
Default phase-one image822912 B uploaded to COM3 (HWT-0016); hash verification
and RTS reset succeeded. This supersedes the earlier not-flashed statement.
Device partition table matched exactly; no full-chip erase or NVS-range write.
Source/image content and resource report unchanged. Default USB remains Serial/JTAG.
Runtime boot/UI/touch/PIN/TOTP/RF/HID and resource acceptance remain pending HWT-0015.

## Easier account entry — 2026-11-02
This supersedes previous current-image counts: FEAT-0014 offers ADD methods for
manual QWERTY or paired USB/Wi-Fi paste of a key/otpauth URI. The same keyboard
is reused by Wi-Fi password entry. A localhost-only Python/browser helper
provides authenticated encrypted transfer; later physical SAVE commits the
candidate. Absolute120s expiry, Back/lock/sleep/exit and Wi-Fi lease cleanup
are bounded. BUG-0011 remains FIXED — UNVERIFIED on the physical board.
Default image831408 B/static SRAM50056 B (+8496/+808 B versus phase one).
Four target builds PASS VER-0045, host/simulator/helper PASS VER-0042..VER-0044,
capacity PASS VER-0046. No GPIO/framework/partition/backlight changes.
Latest upload is still HWT-0016 until separately recorded; HWT-0018 native
USB/UDP/touch and profiling A–E remain NOT RUN / NOT MEASURED.
See [account setup](ACCOUNT_SETUP.md) and
[resource impact](performance/ACCOUNT_ENTRY_RESOURCE_IMPACT.md).

## Current default uploaded — 2026-11-02
HWT-0019 supersedes earlier latest-upload statements: default831408 B image
uploaded COM3, hashes verified and RTS reset sent in26.376s. VER-0049 confirms
matching partition tables. No full erase or NVS-range write. Interactive native
USB/UDP/touch/persistence and resources remain pending HWT-0018; this is upload
evidence only. No commit or push performed.

## Architecture Audit & Screen Orientation Control — 2026-11-03
Architecture audit completed under ADR-0003: decoupled Touch HAL from App interface, established compile-time static alignment across configuration headers, resolved MSVC 64-bit warnings, and added service-level MemoryProfiler forwarder.
Runtime screen orientation control is implemented across DisplayManager, TouchManager (with coordinate transform x=319-rawY, y=rawX and inverted swipe gestures), SettingsService (dedicated 'rot' NVS key preserving 5-byte config schema), and SettingsApp UI (Normal 0° vs Invert 180°).
PlatformIO image build passes with 832,464 B (-1.10% vs baseline, 12.7% flash, 15.3% RAM). Host smoke tests, desktop simulator suite, and documentation checks pass (VER-0053..0055). Physical hardware verification with inverted orientation is pending.

## Lock Screen Wake Freeze and Display Restoration Fix — 2026-11-03
Fixed lock screen freeze on wake: removed ST7789 DISPOFF (0x28) / DISPON (0x29) command hazards from DisplayManager (which caused 120ms hardware stabilization drops and bus desynchronization), suppressed rendering while display.isSleeping() is active so dirty/fullRedraw flags are preserved until wake, enforced invalidateFull() upon display wake, and added onDisplayRestored overrides to WiFiApp and WiFiScannerApp to ensure full UI redraw upon unlock.
