# Current state

Snapshot: 2026-11-05, Asia/Baghdad. Implementation and physical acceptance are
separate; [history](DEVELOPMENT_LOG.md) is append-only.

```json
{
  "record_type": "state",
  "date": "2026-11-05",
  "runtime_version": "v0.21.0",
  "source_patch_label": "v0.1.1-source-patch (legacy documentation label)",
  "branch_at_baseline": "main",
  "baseline_commit": "8239bb6",
  "working_tree": "Security Lab Hub consolidating BLE Lab, Beacon Lab, WiFi Defense and WiFi Channels into a unified launcher",
  "implemented_features": [
    "FEAT-0024",
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
    "FEAT-0019",
    "FEAT-0020",
    "FEAT-0021",
    "FEAT-0022",
    "FEAT-0023",
    "FEAT-0025",
    "FEAT-0026",
    "FEAT-0027",
    "FEAT-0028"
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
    "BUG-0016",
    "BUG-0018"
  ],
  "latest_build": "VER-0231",
  "latest_upload": "HWT-0085",
  "documentation_verification": "VER-0232",
  "current_host_attempt": "VER-0230",
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

## 2FA USB Long-Press & Hardware Button Trigger — 2026-11-04
Redesigned the 2FA Authenticator USB button with a modern Cyber USB Action Capsule (Cyan border, USB badge, instructional prompt, and status feedback). Added touch hold (long press) gating so short taps guide the user without accidental keystroke injection, and wired the physical BOOT hardware button (GPIO 0) via onPrimaryAction and onActivate to directly trigger USB HID OTP typing. Built and uploaded v0.9.4 to COM5 (HWT-0036); owner confirmed physical test PASS on device (HWT-0037).

## BLE keyboard startup interruption — 2026-11-04

The owner reproduced the interruption during BLE startup on v0.10.5 and
v0.10.6 (HWT-0044, HWT-0048). The v0.10.6 trace identified an ESP-IDF
coexistence panic in `coex_core_enable`. v0.10.7 enables Wi-Fi modem sleep
for the BLE lifecycle and retains the controller error handling from v0.10.6.
The authorized app0 upload passed (HWT-0050). A physical activation reached
BLE advertising with native USB still connected (HWT-0051). Pairing, typing
and extended resource acceptance are tracked in HWT-0049. See the
[resource impact report](performance/BLE_STARTUP_RESOURCE_IMPACT.md).

## Windows BLE link after pairing — 2026-11-04

Windows successfully paired with v0.10.7. The owner then reported
`BLE not connected` in Generator while the board remained on (HWT-0052).
v0.10.8 adds Serial connection, authentication, disconnect reason and
advertising completion messages. The uploaded image (HWT-0054) captured
remote termination and an unspecified authentication failure (HWT-0055).
Peer identity and Generator typing remain unverified.

v0.11.0 adds Bluetooth beside Wi-Fi on the first Home page, with the
same view reachable from Settings. BLE requests link encryption and shows
keyboard readiness after the host subscribes to the encrypted input report.
Home, Settings and readiness tests passed in software. The application
image was flashed and verified (HWT-0057). Native USB returned, and the
capture session contained no BLE activation. Windows reconnect, typing
and resource acceptance are pending HWT-0056.

HWT-0058 then reproduced successful v0.11.0 authentication without an HID
subscription. Windows showed the Battery Service but no HID child. v0.11.1
makes HID report metadata and the CCCD available during enumeration, while
keystrokes require the authenticated and subscribed link. Device acceptance
was attempted on the authorized image (HWT-0060). HID started on v0.11.1,
but Windows disconnected before subscription (HWT-0061). A phone stayed
connected and subscribed, with failed authentication (HWT-0062).
v0.12.0 supplies DisplayOnly pairing and shows the one-time code on the
Bluetooth screen. HWT-0064 verified the upload. Windows authenticated
once but did not subscribe (HWT-0065). The phone reported incorrect
PIN/passkey before offering a code entry field (HWT-0066). Keyboard
delivery and extended resource acceptance are unverified.

### BLE PnP ID correction in v0.12.1

The Device Information PnP ID now uses little-endian fields, matching the NimBLE HID implementation used by Bruce. Display-assisted pairing and the authentication and subscription gates are unchanged. This corrects a known metadata error. Its effect on Windows HID subscription and the phone pairing failure is unverified. The v0.12.1 image was written and hash-verified. Runtime COM6 returned after reset (HWT-0067).

### Beacon Lab in v0.13.0

Beacon Lab is a Home app on ESP32 and desktop. Select 10, 15, 20, 25, 30, 35 or 40 synthetic Orchid SSIDs. Start requires an idle radio. A session stays on the current radio channel and ends after ten seconds or on Stop, Back, lock, screen sleep or screensaver. Hardware reception and resource scenarios are pending. The v0.13.0 image was written and hash-verified, and runtime COM6 returned (HWT-0070).

### Beacon Lab controls in v0.14.0

Open and Secured are separate counts, with a combined range of 1 to 40. Seconds selects a 5 to 60 second broadcast window in steps of 5 seconds. Each session selects distinct names from a pool of 48 fictional SSIDs. Secured adds WPA2-PSK metadata to the beacon. All entries are advertisements only. Receivers may retain them in their Wi-Fi list after the transmission ends. Physical receiver validation is pending. The v0.14.0 image was written and hash-verified, and runtime COM6 returned (HWT-0071).

### Beacon Lab scan visibility in v0.14.1

Beacon Lab sends at most three different names per scheduled tick, with at least 25 ms between batches. TX accepted shows distinct names accepted by the driver. It is not a receiver count. A failed raw TX request stops the session and logs its error and progress. The board remains on v0.14.0 pending upload.

### Wi-Fi channel analysis in v0.16.0

Wi-Fi Channels is registered after the existing Home apps on ESP32 and desktop. It opens the latest completed scan, if available, and scans only through REFRESH or BOOT. The chart covers channels 1–14, with the number of visible access point records and strongest RSSI on the selected channel. Statistics aggregate the entire scan before the existing twelve-entry connection list is retained. Counts describe detected APs and do not measure channel airtime or interference. Regulatory scan coverage is determined by the existing driver configuration.

The owner confirmed Beacon Lab works (HWT-0074), without reporting counts, exact image or physical resource measurements. Channel reception and device resource scenarios A–E remain NOT RUN (HWT-0075). The latest recorded physical upload remains v0.15.0 (HWT-0073); v0.16.0 is a source/build version.

### BLE Lab in v0.17.0

BLE Lab is a physical Home app. Defaults are five identities for thirty seconds,
adjustable from one to ten and five to sixty seconds. Names are LabBLE01 through
LabBLE10, with stable per-session random addresses and a custom service UUID.
The Advertised count records successful GAP starts for unique identities.
Each operation has a two-second timeout. Startup reuses the checked HID path
and requires 150 KiB free internal heap after controller initialization.

Radio leasing excludes other tools and the keyboard. Connected Wi-Fi uses the
existing modem-sleep coexistence policy. Back, Stop, expiry, lock and sleep end
the session. Failed teardown retains the lease and retries once per second.
Physical reception, keyboard recovery and resource scenarios are pending.
The v0.17.0 image was written and hash-verified, and runtime COM6 returned
(HWT-0077). Receiver and resource acceptance remain pending. Simulator integration and validation are
SKIPPED at the owner request (VER-0216). See [BLE Lab](BLE_LAB.md).

### Pairing discovery profiles in v0.18.0

BLE Lab includes Android Fast Pair, Windows Swift Pair, Apple AirPods and
Samsung Buds/Watch advertisement profiles adapted from Bruce. The generic
scanner mode is retained. Tap the top row to select a mode. Defaults are
Android, five identities and thirty seconds. All pairing types uses the
selected number per protocol across five protocols.

Pairing profiles use connectable advertising. The transmitted layouts and
model identifiers follow the reference; receiver notification behavior still
depends on software, discovery settings and model support. HWT-0078 tracks
physical notification and resource acceptance. Simulator work remains paused.

The v0.18.0 image was written and hash-verified and runtime COM6 returned
(HWT-0079). Receiver notification behavior remains unverified.

### Target picker and Random in v0.19.0

The owner confirmed Windows Swift Pair notifications displaying Classroom Buds
after the matching profile was selected (HWT-0080). The Windows observer
independently received ten Microsoft manufacturer payloads and five Android
FE2C payloads in separate sessions. Android notification display is unverified.

Tapping BLE Lab's profile row now opens a target picker for Android, Windows,
iPhone/iPad, Samsung Buds/Watch, All types, BLE scanner and Random. Back returns
to configuration. BOOT selects a card and hold activates it. The contact that
opens the picker is ignored for selection to avoid touch bleed-through.

Random chooses a pairing protocol and model for every advertising window and
refreshes the random address. Duration remains bounded. Fixed profiles retain
their configured identity counts. Random's count row describes automatic type
and model selection instead of a fixed identity count. The session stops on
Back, expiry, lock and sleep. Physical picker/Random acceptance remains pending
(HWT-0081). Simulator work remains paused.

The v0.19.0 image was written and hash-verified and runtime COM6 returned
(HWT-0082). New picker and Random receiver acceptance are still pending.

### BLE spam/advertising modes in v0.20.0

BLE Lab now includes a second target-picker page for Apple Action, Apple Not
Your Device and Beacon spam. Header arrows switch pages; the first page, its
touch coordinates, BOOT navigation and selection behavior are unchanged. The
picker's Back behavior still returns to configuration before leaving the app.

Apple Action emits the Continuity NearbyAction record with one of Bruce's
fifteen action type codes and fresh trailing bytes per window. Apple Not Your
Device reuses the ProximityPair layout with prefix 0x01. Beacon spam emits
Bruce's UUID 0x1812, appearance 0x0180 and one of five preset names, capped at
31 bytes, with non-connectable advertising. Random now chooses among all seven
profile types.

The ESP32 build passed at 1,592,096 B of 6,553,600 B app0 capacity, 4,961,504 B
remaining, with unchanged 86,264 B static RAM (VER-0231). Host payload, paging
and cleanup tests passed (VER-0230). The image was uploaded on the second
attempt (COM5) and esptool verified the write hash (HWT-0085). Post-upload
probing could not return the board to normal boot: every software reset sampled
the USB download strap and the serial TIME probe returned no response, so a
physical RESET press or USB re-plug is required (HWT-0086). No receiver was
tested and no heap, latency, power or twenty-cycle physical result exists, so
Apple Action, Not Your Device and Beacon receiver behavior is NOT RUN
(HWT-0084). Simulator work remains paused by the owner.

