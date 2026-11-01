# Current state

Snapshot: 2026-11-01, Asia/Baghdad. Implementation and physical acceptance are
separate; [history](DEVELOPMENT_LOG.md) is append-only.

```json
{
  "record_type": "state",
  "date": "2026-11-01",
  "runtime_version": "v0.3",
  "source_patch_label": "v0.1.1-source-patch (legacy documentation label)",
  "branch_at_baseline": "main",
  "baseline_commit": "8239bb6",
  "working_tree": "Clean at discovery; Architecture audit and modular refactoring completed",
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
    "FEAT-0015"
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
    "BUG-0008"
  ],
  "latest_build": "VER-0028",
  "latest_upload": "HWT-0014",
  "documentation_verification": "VER-0027",
  "current_host_attempt": "VER-0027",
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
