# Physical hardware test log

Append-only. HWT-0001/0002 import earlier owner observations; dates and binary
identity are unknown. Their record-creation time is not the test date. No
physical tests were performed in the 2026-11-01 documentation session. Pending
procedures below are never automatically promoted to PASS. Append a new result
referencing the procedure ID; preserve these entries.

See [build/device instructions](BUILD_AND_TEST.md), [matrix](TEST_MATRIX.md),
and [record rules](ENGINEERING_WORKFLOW.md).

```json
{
  "record_type": "test",
  "id": "HWT-0001",
  "date": "UNKNOWN",
  "recorded_at": "2026-11-01T00:14:58+03:00",
  "firmware_version": "UNKNOWN (earlier independent owner sketch, not current NibruOS)",
  "commit": "UNKNOWN",
  "verification_type": "HARDWARE",
  "scope": "Independent-sketch display and CST816S baseline only",
  "hardware": "Owner Touch Glass board, integrated LCD and CST816S",
  "wiring": "SDA18/SCL17/RST21/INT16; LCD PWR15 HIGH/BL38 HIGH; reset LOW10ms/HIGH wait50ms; begin(FALLING), rotation1, x=rawY/y=169-rawX.",
  "procedure": "Owner tested independent sketch; complete original test steps and raw serial capture are unavailable.",
  "expected": "Display and mapped touch operate in landscape.",
  "actual": "Owner reported display and CST816S operation working in the independent sketch.",
  "result": "PASS",
  "evidence": [
    "AGENTS.md",
    "docs/PROJECT_CONTEXT.md"
  ],
  "evidence_kind": "Imported owner physical-test report, preserved in initial Git commit; no new test by this agent.",
  "environment": "Power configuration UNKNOWN; no device test performed this session.",
  "limitations": "Event date, sketch hash, power and raw logs UNKNOWN. Does not verify patched NibruOS, gestures, navigation or Wi-Fi."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0002",
  "date": "UNKNOWN",
  "recorded_at": "2026-11-01T00:14:58+03:00",
  "firmware_version": "v0.1 (owner report; exact image UNKNOWN)",
  "commit": "UNKNOWN",
  "verification_type": "HARDWARE",
  "scope": "Earlier NibruOS Wi-Fi Scanner timeout",
  "hardware": "LILYGO T-Display-S3 Touch Glass; external modules UNKNOWN",
  "wiring": "Integrated radio; no external radio pins; antenna routing UNKNOWN.",
  "procedure": "Owner flashed earlier firmware and opened Wi-Fi Scanner.",
  "expected": "Async scan returns networks, empty result or bounded error.",
  "actual": "Owner reported Scanner displayed Scan timed out.",
  "result": "FAIL",
  "evidence": [
    "docs/WIFI_DIAGNOSTICS.md",
    "docs/PROJECT_CONTEXT.md"
  ],
  "evidence_kind": "Imported owner physical-test failure report; no raw serial attachment.",
  "environment": "Power configuration UNKNOWN; no device test performed this session.",
  "limitations": "No original binary or separate pre-patch commit, event date, power information or established root cause."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0003",
  "date": "UNKNOWN",
  "recorded_at": "2026-11-01T00:14:58+03:00",
  "firmware_version": "v0.1 runtime / v0.1.1-source-patch documentation",
  "commit": "ccbd0e625837196ce547f16788022be6465786cb",
  "verification_type": "HARDWARE",
  "scope": "Current firmware boot/display/touch/buttons/navigation",
  "hardware": "LILYGO T-Display-S3 Touch Glass; external modules UNKNOWN",
  "wiring": "All reservations in HARDWARE_PINOUT; no external module required.",
  "procedure": "Record actual flashed commit/image hash, power and modules. Follow BUILD_AND_TEST hardware/Touch Test/System Info checks: boot, corners, four swipes, BOOT short/long, GPIO14, header Back, flash/PSRAM values.",
  "expected": "320x170 Home; x0..319/y0..169; correct landscape gestures; buttons/Back work; stable boot and actual memory reports.",
  "actual": "NOT RUN",
  "result": "NOT RUN",
  "evidence": [],
  "evidence_kind": "None; pending physical procedure",
  "environment": "Power configuration UNKNOWN; no device test performed this session.",
  "limitations": "NOT RUN; baseline commit is the intended source, not proof it has been flashed. Independent-sketch PASS does not cover this test."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0004",
  "date": "UNKNOWN",
  "recorded_at": "2026-11-01T00:14:58+03:00",
  "firmware_version": "v0.1 runtime / v0.1.1-source-patch documentation",
  "commit": "ccbd0e625837196ce547f16788022be6465786cb",
  "verification_type": "HARDWARE",
  "scope": "Wi-Fi reliability fix, repeated scans, cancellation and radio cleanup",
  "hardware": "LILYGO T-Display-S3 Touch Glass; external modules UNKNOWN",
  "wiring": "Integrated radio and display/input; known nearby 2.4GHz AP; record antenna/power.",
  "procedure": "Follow WIFI_DIAGNOSTICS real-board steps: warmup, result/empty result, 3 rescans, Home during Preparing/Scanning, reopen. If both attempts fail capture complete sanitized [WiFi] 115200 log; use standalone diagnostic only after backup and restore NibruOS.",
  "expected": "Successful completed scans near known AP; responsive Back; no stale radio ownership; bounded 11s per scan plus 650ms warmup, at most 2 attempts.",
  "actual": "NOT RUN",
  "result": "NOT RUN",
  "evidence": [],
  "evidence_kind": "None; pending physical procedure",
  "environment": "Power configuration UNKNOWN; no device test performed this session.",
  "limitations": "NOT RUN; physical resolution of BUG-0001 remains pending."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0005",
  "date": "UNKNOWN",
  "recorded_at": "2026-11-01T00:14:58+03:00",
  "firmware_version": "v0.1 runtime / v0.1.1-source-patch documentation",
  "commit": "ccbd0e625837196ce547f16788022be6465786cb",
  "verification_type": "HARDWARE",
  "scope": "Memory stability, power and repeated lifecycle cleanup",
  "hardware": "LILYGO T-Display-S3 Touch Glass; external modules UNKNOWN",
  "wiring": "Integrated board; USB first, safe compatible battery only after polarity/power checks.",
  "procedure": "After HWT-0003/HWT-0004, run at least 20 Home/Wi-Fi/Home cycles; note free heap/free PSRAM before/after each cycle and after 10 minutes idle. Capture exceptions/brownouts/boot loops. Repeat on suitable non-USB power if available.",
  "expected": "No resets/freezes or persistent downward free-memory trend after radio cleanup. Record exact observations; no invented threshold.",
  "actual": "NOT RUN",
  "result": "NOT RUN",
  "evidence": [],
  "evidence_kind": "None; pending physical procedure",
  "environment": "Power configuration UNKNOWN; no device test performed this session.",
  "limitations": "NOT RUN; linker RAM figures are not heap-leak, PSRAM or electrical stability tests."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0006",
  "date": "UNKNOWN",
  "recorded_at": "2026-11-01T00:14:58+03:00",
  "firmware_version": "UNKNOWN",
  "commit": "UNKNOWN",
  "verification_type": "HARDWARE",
  "scope": "Future external MicroSD initialization/read/write",
  "hardware": "LILYGO T-Display-S3 Touch Glass; external modules UNKNOWN",
  "wiring": "PROPOSED/UNVERIFIED: CS10/MOSI11/SCK12/MISO13, compatible 3.3V supply and GND. Not confirmed installed.",
  "procedure": "First identify module/card and power requirements, confirm wiring and implement FEAT-0007 with bounded I/O. Then mount; write/read a temporary non-sensitive file; compare contents; remove test file; test missing card and Back cleanup. Never format user media as a test.",
  "expected": "Future driver reports capacity/errors, round-trips data, and remains responsive.",
  "actual": "NOT RUN",
  "result": "BLOCKED",
  "evidence": [],
  "evidence_kind": "None; pending physical procedure",
  "environment": "Power configuration UNKNOWN; no device test performed this session.",
  "limitations": "No SD driver or confirmed connection; no SD access attempted. Timing/speed/current budget unknown."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0007",
  "date": "2026-11-01",
  "recorded_at": "2026-11-01T03:51:00+03:00",
  "firmware_version": "v0.3 runtime",
  "commit": "ccbd0e625837196ce547f16788022be6465786cb (uncommitted working tree)",
  "verification_type": "HARDWARE",
  "scope": "Physical firmware upload and ROM bootloader execution on COM3",
  "hardware": "LILYGO T-Display-S3 Touch Glass (ESP32-S3R8, 16 MB flash, 8 MB PSRAM, ST7789V parallel LCD, CST816S touch)",
  "wiring": "Connected via USB-C to host on COM3 (ROM Bootloader mode VID:303A PID:1001)",
  "procedure": "esptool.py v4.5.1 write_flash at 460800 baud on COM3 via PlatformIO",
  "expected": "Successful chip detection (ESP32-S3 revision v0.2, 40MHz crystal), flash erase, compression, flashing, verification of SHA-256 hash, and RTS reset.",
  "actual": "PASS: Successfully detected ESP32-S3 (revision v0.2, MAC 44:1b:f6:d3:aa:bc), erased flash regions, flashed 841712 bytes in 8.2 seconds (819.3 kbit/s), verified hash, and executed hard reset via RTS pin.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-target-build.txt"
  ],
  "evidence_kind": "esptool v4.5.1 flash output",
  "environment": "Windows 11, USB CDC, PlatformIO Core 6.2.0, esptool 4.5.1",
  "limitations": "Physical runtime verification of touch taps on the 46px chevrons and Settings PIN change."
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-01",
  "firmware_version": "v0.3",
  "commit": "8239bb6 + uncommitted Settings completion",
  "hardware": "Target LILYGO T-Display-S3 Touch Glass; software checks only",
  "wiring": "No external GPIO change; no device accessed",
  "expected": "Actual current image observations; >=150 KiB internal headroom, bounded latency and balanced cleanup; no unintended confirmation; no unexpected credential/secret loss",
  "evidence_kind": "Pending reproducible procedure; no observed physical result",
  "environment": "Windows; Python 3.14; PlatformIO 6.2.0; local Zig 0.16.0 for HOST",
  "limitations": "Physical heap/PSRAM/stack/latency/power/brightness/reception remain NOT MEASURED",
  "id": "HWT-0008",
  "verification_type": "HARDWARE",
  "scope": "Essential Settings and resource profiling scenarios A–E",
  "procedure": "Use identified default/profile images; boot checkpoints; test all brightness/timeout/accent values, restart persistence, corrupt/unavailable NVS recovery, reset preservation, BOOT/touch/Back, held wake and raw double taps; record 20 entry/exit cycles and supported Wi-Fi+display/touch workloads. Compare USB and suitable battery supply. LEDC requires separately observed compatibility; do not replace AW9364 blindly.",
  "actual": "NOT RUN: no upload, serial capture or physical observation performed in this change.",
  "result": "NOT RUN",
  "evidence": []
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0009",
  "date": "2026-11-01",
  "firmware_version": "v0.3 runtime, Essential Settings completion",
  "commit": "8239bb6 + uncommitted Settings changes",
  "verification_type": "HARDWARE",
  "scope": "User-authorized default AW9364 firmware upload to connected ESP32-S3 on COM5 only",
  "hardware": "Connected ESP32-S3 USB device VID303A:PID1001, detected by PlatformIO/esptool",
  "wiring": "USB-C serial COM5, upload460800 baud; firmware pins/partitions unchanged",
  "procedure": "pio device list; pio run -e lilygo-t-display-s3; python tools/check_resources.py; pio run -e lilygo-t-display-s3 -t upload --upload-port COM5",
  "expected": "Upload latest default firmware, verify written data and send reset; preserve NVS partitions",
  "actual": "SUCCESS in19.734s. Wrote842848 bytes (546464 compressed) at0x00010000 in7.3s. Hash of data verified; hard reset via RTS requested. Firmware SHA-256 f170c68ce03ee3e8967f648598942c5c162067d69e454060e7a384e08a04fb9b. No unrestricted erase, reset-preferences action or serial runtime observation performed.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-settings-upload-com5.txt"
  ],
  "evidence_kind": "Captured actual PlatformIO/esptool physical upload output; device MAC redacted",
  "environment": "Windows, PlatformIO6.2.0, espressif32@6.5.0/Arduino2.0.14, lilygo-t-display-s3",
  "limitations": "PASS is limited to flash write/hash/reset command. No boot/UI/touch/brightness/Wi-Fi reception or runtime resource validation; HWT-0008 remains NOT RUN."
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-01",
  "firmware_version": "v0.3",
  "commit": "8239bb6 + uncommitted Settings/touch repair",
  "hardware": "LILYGO T-Display-S3 Touch Glass; physical observation only when explicitly indicated",
  "wiring": "Unchanged SDA18/SCL17/RST21/IRQ16, power15/backlight38/buttons0/14",
  "expected": "One action per physical press; supported swipes/Back/wake and safe confirmations; no per-touch full-screen wipe",
  "environment": "Windows, PlatformIO6.2.0/espressif32@6.5.0/core2.0.14; Zig0.16.0 for host mocks",
  "limitations": "Mock/build results do not validate physical touch timing, brightness, heap, stack, latency or power",
  "id": "HWT-0010",
  "verification_type": "HARDWARE",
  "scope": "Owner-reported Settings repeated touch actions and screen flicker after latest upload",
  "procedure": "User operated Settings on their device after HWT-0009 and reported behavior in chat.",
  "actual": "Owner reports touch repeats several times per second and screen flashes with every touch. No raw contact trace, video or independently read running image identifier; latest known upload was HWT-0009.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-01-touch-owner-report.txt"
  ],
  "evidence_kind": "Direct owner report in this chat, preserved verbatim"
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-01",
  "firmware_version": "v0.3",
  "commit": "8239bb6 + uncommitted Settings/touch repair",
  "hardware": "LILYGO T-Display-S3 Touch Glass; physical observation only when explicitly indicated",
  "wiring": "Unchanged SDA18/SCL17/RST21/IRQ16, power15/backlight38/buttons0/14",
  "expected": "One action per physical press; supported swipes/Back/wake and safe confirmations; no per-touch full-screen wipe",
  "environment": "Windows, PlatformIO6.2.0/espressif32@6.5.0/core2.0.14; Zig0.16.0 for host mocks",
  "limitations": "Mock/build results do not validate physical touch timing, brightness, heap, stack, latency or power",
  "id": "HWT-0012",
  "verification_type": "HARDWARE",
  "scope": "Pending physical acceptance of touch-repeat and Settings redraw repair",
  "procedure": "On repaired default image: hold each control2s (one action), release and tap again; repeat10 separate taps/keypad digits; swipe all directions and scroll Settings without opening rows; test same-value brightness/timeout/accent touches and focus without whole-screen flash. Test raw wake consumption and PIN lock/unlock restoring the previous Settings page; Back and reset/reboot Cancel; confirmation requires a distinct press begun>=500ms after dialog. Run20 Settings entry/exit cycles and A–E resources. Record image hash, raw trace if any failures, and sanitized observations.",
  "actual": "NOT RUN: repaired UI/touch behavior has not been physically observed by the agent or confirmed by owner.",
  "result": "NOT RUN",
  "evidence": [],
  "evidence_kind": "Pending reproducible physical procedure; no PASS observation"
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0011",
  "date": "2026-11-01",
  "firmware_version": "v0.3, repaired touch and Settings repaint",
  "commit": "8239bb6 + uncommitted focused repair",
  "verification_type": "HARDWARE",
  "scope": "Physical upload of repaired default AW9364 image; flash verification only",
  "hardware": "Detected ESP32-S3 USB VID303A:PID1001 on COM5",
  "wiring": "USB-C COM5; source pins/partition table unchanged",
  "procedure": "pio device list; python tools/check_docs.py; python tools/check_resources.py; pio run -e lilygo-t-display-s3 -t upload --upload-port COM5",
  "expected": "Write repaired default firmware, verify flash data and send reset without full-flash erase",
  "actual": "SUCCESS19.893s; wrote843888 B (547349 compressed) at0x10000 in7.4s; data hash verified, RTS reset command sent. Image SHA-256 2d68c7dd233c5404626eee4d8a6aa4613b9ad50323e321e3a7b0a3de6dcacdd1.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-touch-upload-com5.txt",
    "docs/evidence/2026-11-01-touch-artifacts.txt"
  ],
  "evidence_kind": "Captured actual PlatformIO/esptool device-write output, MAC redacted",
  "environment": "Windows, PlatformIO6.2.0, espressif32@6.5.0/core2.0.14",
  "limitations": "PASS means flash write/hash/reset command only. Repaired UI/touch/brightness/heap/stack/power and radio reception not observed; owner recheck HWT-0012 required."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0013",
  "date": "2026-11-01",
  "firmware_version": "v0.3 repaired touch/Settings",
  "commit": "8239bb6 + uncommitted repair",
  "verification_type": "HARDWARE",
  "scope": "Repeated user-requested upload of latest default AW9364 firmware",
  "hardware": "ESP32-S3 USB VID303A:PID1001 detected on COM5",
  "wiring": "USB-C COM5; firmware wiring/partitions unchanged",
  "procedure": "pio device list; python tools/check_docs.py; pio run -e lilygo-t-display-s3 -t upload --upload-port COM5",
  "expected": "Write current default image, verify hash and send reset",
  "actual": "SUCCESS20.013s. Wrote843888 bytes (547349 compressed) at0x10000 in7.4s; hash verified and RTS reset sent. Unchanged image SHA-256 2d68c7dd233c5404626eee4d8a6aa4613b9ad50323e321e3a7b0a3de6dcacdd1.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-touch-repeat-upload-com5.txt"
  ],
  "evidence_kind": "Actual PlatformIO/esptool output with MAC redacted",
  "environment": "Windows/PlatformIO6.2.0/espressif32@6.5.0/core2.0.14",
  "limitations": "Flash-write/hash/reset verification only; physical UI/touch/heap/latency/power acceptance HWT-0012 still pending."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0014",
  "date": "2026-11-01",
  "firmware_version": "v0.3 modular refactored",
  "commit": "8239bb6 + modular architecture refactoring",
  "verification_type": "HARDWARE",
  "scope": "Physical upload of modular architecture refactored NibruOS image to COM5",
  "hardware": "ESP32-S3 (revision v0.2, VID:303A PID:1001) on COM5",
  "wiring": "USB-C COM5; ST7789 8-bit parallel, CST816S I2C; MicroSD not connected",
  "procedure": "pio run -e lilygo-t-display-s3 -t upload --upload-port COM5",
  "expected": "Write modular refactored firmware, verify partition and application hashes, execute RTS reset",
  "actual": "SUCCESS 32.056s. Flashed bootloader, partitions, and application (843,936 bytes, 547,299 compressed) at 0x10000 in 7.4s (915.2 kbit/s). Hash verified, hard reset via RTS pin.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-modular-architecture-upload-com5.txt"
  ],
  "evidence_kind": "Captured PlatformIO/esptool v4.5.1 device flashing output, MAC sanitized",
  "environment": "Windows 11, PlatformIO Core 6.5.0, espressif32@6.5.0, esptool 4.5.1",
  "limitations": "Physical verification confirms successful flash write, hash validation, and bootloader reset. Interactive runtime UI/touch observation on physical screen remains pending owner testing."
}
```


```json
{
  "record_type": "test",
  "id": "HWT-0015",
  "date": "2026-11-01",
  "firmware_version": "v0.3 authenticator phase one",
  "commit": "9513925 + uncommitted changes",
  "verification_type": "HARDWARE",
  "scope": "Pending authenticator device acceptance",
  "hardware": "LILYGO T-Display-S3 Touch Glass ESP32-S3R8 N16R8; not observed in this task",
  "wiring": "Existing LCD/touch/buttons/native USB; no connected SD/RTC/BLE module assumed",
  "procedure": "Preserve existing data first; identify intended image/port and flash only when requested. Configure own PIN, add public fixture, sync time and compare code, power-cycle/unlock/reopen. Check invalid/lost time, USB busy/failure/disconnect/cancel/reconnect and final key release; default USB unavailable, opt-in HID CDC/keyboard enumeration and US layout. Check Back, masked editing, failed writes and delete/PIN guard. Run startup, entry, exit, >=20 cycles and combined Wi-Fi/NTP/TOTP/HID; measure >=150 KiB free internal heap, largest block, PSRAM, stack, input/loop latency and power.",
  "expected": "Behavior matches the phase-one design and applicable test contract",
  "actual": "NOT RUN. Default image SHA256 00e74d8689773ea90efc9cfb707fa48aeb593c2cc10d2b8d5edfdb5b8cd0ea8b; optional HID SHA256 07beb7ab38c7670e896e8b5c2ea930550a685b42bc2de19cdf266c87a60f0c55.",
  "result": "NOT RUN",
  "evidence": [],
  "evidence_kind": "Pending procedure; no physical outcome",
  "environment": "Windows / Python 3.12.6 / Zig0.15.2 / Arduino-ESP32 2.0.14 espressif32@6.5.0",
  "limitations": "No physical security, USB, RF, timing or resource claim."
}
```


```json
{
  "record_type": "test",
  "date": "2026-11-02",
  "firmware_version": "v0.3 authenticator phase one",
  "commit": "9513925 + uncommitted implementation",
  "environment": "Windows / PlatformIO espressif32@6.5.0 / Arduino-ESP32 2.0.14 / esptool4.5.1",
  "id": "HWT-0016",
  "verification_type": "HARDWARE",
  "scope": "Authorized default firmware upload to COM3",
  "hardware": "ESP32-S3 revision v0.2, VID303A PID1001 on COM3; identifier redacted",
  "wiring": "USB-C; existing LCD/CST816S/buttons, no external peripheral changes",
  "procedure": "Read 3072-byte device table at0x8000 and compare; pio run -e lilygo-t-display-s3 -t upload --upload-port COM3",
  "expected": "Write intended default image, verify hashes, send RTS reset; preserve partition layout/NVS range",
  "actual": "SUCCESS26.18s. Preflight table exact match bd0f7954aca2ef7d925ee21aaa1f3dc8822d1d6ce5cbbd26a135e5886bfff6ce. Application822912 B written at0x10000 in8.0s; bootloader/table/boot metadata/application hashes verified; RTS reset sent. Image SHA256 00e74d8689773ea90efc9cfb707fa48aeb593c2cc10d2b8d5edfdb5b8cd0ea8b. No full-chip erase; NVS0x9000..0xdfff excluded from flashed ranges.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-02-authenticator-preflight.txt",
    "docs/evidence/2026-11-02-authenticator-upload-com3.txt"
  ],
  "evidence_kind": "Sanitized captured esptool/PlatformIO output and partition-byte comparison",
  "limitations": "Upload/write/hash/reset only. No observation of boot completion, UI/touch/time/PIN persistence, RF, USB HID or runtime resources; HWT-0015 remains NOT RUN."
}
```


```json
{
  "record_type": "test",
  "date": "2026-11-02",
  "firmware_version": "v0.3 easier account entry",
  "commit": "9513925 + uncommitted changes",
  "hardware": "Owner LILYGO T-Display-S3 Touch Glass, prior HWT-0016 image",
  "wiring": "No GPIO/wiring/framework/partition change",
  "environment": "Windows / Python3.12.6 / Zig0.15.2 / SDL2 2.30.10 / Node24.8.0 / espressif32@6.5.0 Arduino2.0.14",
  "id": "HWT-0017",
  "verification_type": "HARDWARE",
  "scope": "Owner-reported account-entry usability on prior image",
  "procedure": "Owner opens 2FA and adds a new account",
  "expected": "Practical long-key entry and familiar keyboard ordering",
  "actual": "Owner reached Base32 secret prompt, reported long manual typing cumbersome and requested conventional number/letter ordering plus multiple alternate methods.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-02-account-entry-owner-report.txt"
  ],
  "evidence_kind": "Owner report, imported from current chat; no independent visual observation",
  "limitations": "Usability feedback and reachability only, not a boot/touch/TOTP/persistence/RF/resource acceptance suite."
}
```


```json
{
  "record_type": "test",
  "date": "2026-11-02",
  "firmware_version": "v0.3 easier account entry",
  "commit": "9513925 + uncommitted changes",
  "hardware": "LILYGO T-Display-S3 Touch Glass; new image not interactively observed",
  "wiring": "No GPIO/wiring/framework/partition change",
  "environment": "Windows / Python3.12.6 / Zig0.15.2 / SDL2 2.30.10 / Node24.8.0 / espressif32@6.5.0 Arduino2.0.14",
  "id": "HWT-0018",
  "verification_type": "HARDWARE",
  "scope": "Pending easier account-entry and resource acceptance",
  "procedure": "Use public disposable seed: manual QWERTY/digits/Shift/symbols; Web Serial USB paste then device SAVE; existing-network Wi-Fi paste/SAVE; no autosave/early contact; timeout/Back/lock/sleep/disconnect and reboot persistence; run profiling Startup/Entry/Exit/>=20 cycles/combined Wi-Fi-NTP-TOTP-USB and measure >=150KiB internal byte-addressable heap, largest block, PSRAM, task stack, UI latency and power.",
  "expected": "Correct explicit provisioning, saved metadata and codes, responsive Back, bounded cleanup and resource acceptance",
  "actual": "NOT RUN. Host/simulator/build results are separately recorded, no native USB/UDP/touch/resource evidence.",
  "result": "NOT RUN",
  "evidence": [],
  "evidence_kind": "Pending reproducible physical procedure",
  "limitations": "All runtime resource metrics NOT MEASURED. Default USB serial import differs from optional keyboard HID delivery."
}
```


```json
{
  "record_type": "test",
  "date": "2026-11-02",
  "firmware_version": "v0.3 easier account entry",
  "commit": "9513925 + uncommitted changes",
  "environment": "Windows / espressif32@6.5.0 / Arduino-ESP32 2.0.14 / esptool4.5.1",
  "wiring": "Existing USB-C COM3 and integrated LCD/touch/buttons; no external changes",
  "id": "HWT-0019",
  "verification_type": "HARDWARE",
  "scope": "Authorized easier-account-entry default firmware upload",
  "hardware": "ESP32-S3 revisionv0.2, COM3 VID303A PID1001; identifier redacted",
  "procedure": "pio run -e lilygo-t-display-s3 -t upload --upload-port COM3 after all software/documentation checks and matched partition preflight",
  "expected": "Write intended image, verify flashed data hashes, reset; no full-chip erase/NVS-range writes",
  "actual": "SUCCESS26.376s. Application831408 B (537975 compressed) written0x10000 in8.2s; bootloader/table/boot-metadata/application hashes verified and RTS reset sent. App SHA256 2b43fd32b2d8fbac27263b9bd9226c22c497d331187b384397593e59bb4661f2. Device/generated partitions match VER-0049. No full-chip erase; NVS0x9000..0xdfff not targeted.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-02-account-entry-preflight.txt",
    "docs/evidence/2026-11-02-account-entry-upload-com3.txt"
  ],
  "evidence_kind": "Sanitized captured PlatformIO/esptool output and byte/hash identity",
  "limitations": "Write/hash/reset only; native UI/USB/UDP/persistence and profiling acceptance HWT-0018 NOT RUN."
}
```
