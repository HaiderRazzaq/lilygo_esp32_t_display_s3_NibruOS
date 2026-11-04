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

```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.3 screen orientation",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "Windows / espressif32@6.5.0 / Arduino-ESP32 2.0.14 / esptool 4.5.1",
  "wiring": "Existing USB-C COM5 and integrated LCD/touch/buttons; no external changes",
  "id": "HWT-0020",
  "verification_type": "HARDWARE",
  "scope": "Authorized full flash erase and firmware re-flash",
  "hardware": "ESP32-S3 revision v0.2, COM5 VID 303A PID 1001 MAC 44:1b:f6:d3:aa:bc",
  "procedure": "pio run -e lilygo-t-display-s3 -t erase --upload-port COM5 followed by pio run -e lilygo-t-display-s3 -t upload --upload-port COM5 per user explicit instruction",
  "expected": "Full 16MB flash erase in 34.4s, write bootloader, partition table, boot flags, application image 832,464 B, verify flashed data hashes, and send hard reset via RTS",
  "actual": "Chip erase completed successfully in 34.4s. Application 832,464 B (538,631 compressed) written to 0x10000 in 7.3s. Bootloader (15,104 B at 0x00000000), partitions (3,072 B at 0x00008000), and boot metadata (8,192 B at 0x0000e000) written and verified. Hash of data verified on all segments. Hard reset via RTS sent.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-full-erase-flash-upload-com5.txt"
  ],
  "evidence_kind": "Captured PlatformIO/esptool terminal output for full erase and upload",
  "limitations": "Write/hash/reset verification only; physical on-screen interaction and touch verification on real device pending user testing"
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.4.1",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "Windows / espressif32@6.5.0 / Arduino-ESP32 2.0.14 / esptool 4.5.1",
  "wiring": "Existing USB-C COM5 and integrated LCD/touch/buttons",
  "id": "HWT-0021",
  "verification_type": "HARDWARE",
  "scope": "Authorized v0.4.1 firmware upload with pairing code broadcast and Web Serial CDC stability fix",
  "hardware": "ESP32-S3 revision v0.2, COM5 VID 303A PID 1001 MAC 44:1b:f6:d3:aa:bc",
  "procedure": "pio run -e lilygo-t-display-s3 -t upload --upload-port COM5",
  "expected": "Write intended v0.4.1 image (832,800 B), verify data hashes, reset device via RTS pin",
  "actual": "SUCCESS in 19.58s. Application 832,800 B (538,964 compressed) written to 0x10000 in 7.3s. Bootloader, partitions, and boot metadata written and verified. Hash of data verified on all segments. Hard reset via RTS sent.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v041-upload-com5.txt"
  ],
  "evidence_kind": "Captured PlatformIO/esptool terminal output for v0.4.1 firmware upload",
  "limitations": "Flashing and hardware reset verified; end-to-end pairing test with browser companion to be verified by user"
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.4.2",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "PlatformIO 6.1.18 / Arduino-ESP32 2.0.14 / esptool 4.5.1",
  "wiring": "Existing USB-C COM5 and integrated LCD/touch/buttons",
  "id": "HWT-0022",
  "verification_type": "HARDWARE",
  "scope": "Authorized v0.4.2 firmware upload with flicker-free label rendering and live redraw optimization",
  "hardware": "ESP32-S3 revision v0.2, COM5 VID 303A PID 1001 MAC 44:1b:f6:d3:aa:bc",
  "procedure": "pio run -e lilygo-t-display-s3 -t upload --upload-port COM5",
  "expected": "Write intended v0.4.2 image (833,056 B), verify data hashes, reset device via RTS pin",
  "actual": "SUCCESS in 20.47s. Application 833,056 B (539,197 compressed) written to 0x10000 in 7.3s. Bootloader, partitions, and boot metadata written and verified. Hash of data verified on all segments. Hard reset via RTS sent.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v042-upload-com5.txt"
  ],
  "evidence_kind": "Captured PlatformIO/esptool terminal output for v0.4.2 firmware upload",
  "limitations": "Flashing and hardware reset verified; physical screen inspection of flicker-free timers to be observed by user"
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.4.3",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "PlatformIO 6.1.18 / Arduino-ESP32 2.0.14 / esptool 4.5.1",
  "wiring": "Existing USB-C COM5 and integrated LCD/touch/buttons",
  "id": "HWT-0023",
  "verification_type": "HARDWARE",
  "scope": "Authorized v0.4.3 firmware upload with flicker-free PIN entry and security keypad optimization",
  "hardware": "ESP32-S3 revision v0.2, COM5 VID 303A PID 1001 MAC 44:1b:f6:d3:aa:bc",
  "procedure": "pio run -e lilygo-t-display-s3 -t upload --upload-port COM5",
  "expected": "Write intended v0.4.3 image (833,280 B), verify data hashes, reset device via RTS pin",
  "actual": "SUCCESS in 19.59s. Application 833,280 B (539,411 compressed) written to 0x10000 in 7.3s. Bootloader, partitions, and boot metadata written and verified. Hash of data verified on all segments. Hard reset via RTS sent.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v043-upload-com5.txt"
  ],
  "evidence_kind": "Captured PlatformIO/esptool terminal output for v0.4.3 firmware upload",
  "limitations": "Flashing and hardware reset verified; physical PIN entry interaction to be observed on device"
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.5.0",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "PlatformIO 6.1.18 / Arduino-ESP32 2.0.14 / esptool 4.5.1",
  "wiring": "Existing USB-C COM5 and integrated LCD/touch/buttons",
  "id": "HWT-0024",
  "verification_type": "HARDWARE",
  "scope": "Authorized v0.5.0 firmware upload with system-wide Arabic & English localization and Arabic typography engine",
  "hardware": "ESP32-S3 revision v0.2, COM5 VID 303A PID 1001 MAC 44:1b:f6:d3:aa:bc",
  "procedure": "pio run -e lilygo-t-display-s3 -t upload --upload-port COM5",
  "expected": "Write intended v0.5.0 image (843,872 B), verify data hashes, reset device via RTS pin",
  "actual": "SUCCESS in 19.69s. Application 843,872 B (544,974 compressed) written to 0x10000 in 7.4s. Bootloader, partitions, and boot metadata written and verified. Hash of data verified on all segments. Hard reset via RTS sent.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v050-upload-com5.txt"
  ],
  "evidence_kind": "Captured PlatformIO/esptool terminal output for v0.5.0 firmware upload",
  "limitations": "Flashing and hardware reset verified; physical observation of Arabic UI rendering to be confirmed by user on device"
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.5.1",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "PlatformIO 6.1.18 / Arduino-ESP32 2.0.14 / esptool 4.5.1",
  "wiring": "Existing USB-C COM5 and integrated LCD/touch/buttons",
  "id": "HWT-0025",
  "verification_type": "HARDWARE",
  "scope": "Authorized v0.5.1 firmware upload with Select-then-Confirm touch stability navigation",
  "hardware": "ESP32-S3 revision v0.2, COM5 VID 303A PID 1001 MAC 44:1b:f6:d3:aa:bc",
  "procedure": "pio run -e lilygo-t-display-s3 -t upload --upload-port COM5",
  "expected": "Write intended v0.5.1 image (844,416 B), verify data hashes, reset device via RTS pin",
  "actual": "SUCCESS in 19.69s. Application 844,416 B (545,229 compressed) written to 0x10000 in 7.4s. Bootloader, partitions, and boot metadata written and verified. Hash of data verified on all segments. Hard reset via RTS sent.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v051-upload-com5.txt"
  ],
  "evidence_kind": "Captured PlatformIO/esptool terminal output for v0.5.1 firmware upload",
  "limitations": "Flashing and hardware reset verified; physical touch selection and confirmation to be experienced by user on device"
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.5.2",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "PlatformIO 6.1.18 / Arduino-ESP32 2.0.14 / esptool 4.5.1",
  "wiring": "Existing USB-C COM5 and integrated LCD/touch/buttons",
  "id": "HWT-0026",
  "verification_type": "HARDWARE",
  "scope": "Authorized v0.5.2 firmware upload with wake lock screen redraw fix and settings menu border ghosting fix",
  "hardware": "ESP32-S3 revision v0.2, COM5 VID 303A PID 1001 MAC 44:1b:f6:d3:aa:bc",
  "procedure": "pio run -e lilygo-t-display-s3 -t upload --upload-port COM5",
  "expected": "Write intended v0.5.2 image (844,512 B), verify data hashes, reset device via RTS pin",
  "actual": "SUCCESS in 19.74s. Application 844,512 B (545,283 compressed) written to 0x10000 in 7.4s. Bootloader, partitions, and boot metadata written and verified. Hash of data verified on all segments. Hard reset via RTS sent.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v052-upload-com5.txt"
  ],
  "evidence_kind": "Captured PlatformIO/esptool terminal output for v0.5.2 firmware upload",
  "limitations": "Flashing and hardware reset verified; physical lock screen appearance on wake and single menu border to be verified by user on physical screen"
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.6.0",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "PlatformIO 6.1.18 / Arduino-ESP32 2.0.14 / esptool 4.5.1",
  "wiring": "Existing USB-C COM5 and integrated LCD/touch/buttons",
  "id": "HWT-0027",
  "verification_type": "HARDWARE",
  "scope": "Authorized v0.6.0 firmware upload with Time Management Engine, USB Serial time sync, and TOTP validity display",
  "hardware": "ESP32-S3 revision v0.2, COM5 VID 303A PID 1001 MAC 44:1b:f6:d3:aa:bc",
  "procedure": "Pre-erase app0 region via esptool and flash complete v0.6.0 binary segments (845,936 B) plus bootloader and partition table",
  "expected": "Write intended v0.6.0 image (845,936 B), verify data hashes, reset device via RTS pin",
  "actual": "SUCCESS. Application 845,936 B (544,177 compressed) written to 0x10000 in segments. Bootloader, partitions, and boot metadata written and verified. Hash of data verified on all segments. Hard reset via RTS sent.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v060-upload-com5.txt"
  ],
  "evidence_kind": "Captured PlatformIO/esptool terminal output for v0.6.0 firmware upload",
  "limitations": "Flashing and hardware reset verified; physical observation of time sync, USB time command, and TOTP code generation to be confirmed by user on device"
}
```


```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.6.1",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "PlatformIO 6.1.18 / Arduino-ESP32 2.0.14 / esptool 4.5.1",
  "wiring": "Existing USB-C COM5 and integrated LCD/touch/buttons",
  "id": "HWT-0028",
  "verification_type": "HARDWARE",
  "scope": "Authorized v0.6.1 firmware upload with lock screen wake freeze and display restoration fix",
  "hardware": "ESP32-S3 revision v0.2, COM5 VID 303A PID 1001 MAC 44:1b:f6:d3:aa:bc",
  "procedure": "Upload complete v0.6.1 binary (845,952 B) via PlatformIO esptool directly to COM5 at 460800 baud",
  "expected": "Write intended v0.6.1 image (845,952 B), verify data hashes, reset device via RTS pin",
  "actual": "SUCCESS. Application 845,952 B (546,103 compressed) written to 0x10000 in 7.4 seconds (908.5 kbit/s). Bootloader and partition tables written. Hash of data verified. Hard reset via RTS sent.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v061-upload-com5.txt"
  ],
  "evidence_kind": "Captured PlatformIO/esptool terminal output for v0.6.1 firmware upload",
  "limitations": "Flashing and hardware reset verified; physical observation of lock screen wake redraw and display restoration to be verified by user on device"
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0029",
  "date": "2026-11-03",
  "firmware_version": "v0.7.0",
  "commit": "WORKING_TREE",
  "environment": "Physical LILYGO T-Display-S3 on COM5 via esptool.py",
  "wiring": "Factory ST7789V 8-bit parallel, CST816S capacitive touch, AW9364 backlight",
  "verification_type": "HARDWARE",
  "scope": "Authorized v0.7.0 firmware upload with Encrypted Password Vault Storage and Backup Engine",
  "hardware": "ESP32-S3 revision v0.2, COM5 VID 303A PID 1001 MAC 44:1b:f6:d3:aa:bc",
  "procedure": "Upload complete v0.7.0 binary (846,352 B) via PlatformIO esptool directly to COM5 at 460800 baud",
  "expected": "Write intended v0.7.0 image (846,352 B), verify data hashes, reset device via RTS pin",
  "actual": "SUCCESS. Application 846,352 B (546,219 compressed) written to 0x10000 in 7.5 seconds (903.4 kbit/s). Bootloader and partition tables written. Hash of data verified. Hard reset via RTS sent.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v070-upload-com5.txt"
  ],
  "evidence_kind": "Captured PlatformIO/esptool terminal output for v0.7.0 firmware upload",
  "limitations": "Flashing and hardware reset verified; physical user interaction with upcoming VaultApp UI to follow in Phase 3 Step 2"
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0030",
  "date": "2026-11-04",
  "firmware_version": "v0.8.0",
  "commit": "WORKING_TREE",
  "environment": "Physical LILYGO T-Display-S3 on COM5 via esptool.py",
  "wiring": "Factory ST7789V 8-bit parallel, CST816S capacitive touch, AW9364 backlight",
  "verification_type": "HARDWARE",
  "scope": "Authorized v0.8.0 firmware upload with Password Vault UI (VaultApp) and Confirmed USB HID Credential Transmission",
  "hardware": "ESP32-S3 revision v0.2, COM5 VID 303A PID 1001 MAC 44:1b:f6:d3:aa:bc",
  "procedure": "Upload complete v0.8.0 binary (852,864 B) via PlatformIO esptool directly to COM5 at 460800 baud",
  "expected": "Write intended v0.8.0 image (852,864 B), verify data hashes, reset device via RTS pin, observe boot v0.8.0 banner",
  "actual": "SUCCESS. Application 852,864 B (550,346 compressed) written to 0x10000 in 7.4 seconds (922.0 kbit/s). Hash of data verified. Hard reset via RTS sent. Serial monitor confirms '[NibruOS] boot v0.8.0'.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v080-upload-com5.txt"
  ],
  "evidence_kind": "Captured PlatformIO/esptool terminal output and serial boot banner for v0.8.0 firmware upload",
  "limitations": "Flashing and clean serial boot verified; interactive user validation of password unmasking and USB typing in progress"
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0031",
  "date": "2026-11-04",
  "firmware_version": "v0.8.1",
  "commit": "WORKING_TREE",
  "environment": "Physical LILYGO T-Display-S3 on COM5 via esptool.py",
  "wiring": "Factory ST7789V 8-bit parallel, CST816S capacitive touch, AW9364 backlight",
  "verification_type": "HARDWARE",
  "scope": "Authorized v0.8.1 firmware upload with native TinyUSB composite HID keyboard and VaultApp wake session reloading",
  "hardware": "ESP32-S3 revision v0.2, COM5 VID 303A PID 1001 MAC 44:1b:f6:d3:aa:bc",
  "procedure": "Upload complete v0.8.1 binary (888,352 B) with native TinyUSB composite enabled via PlatformIO esptool directly to COM5 at 460800 baud",
  "expected": "Write intended v0.8.1 image (888,352 B), verify data hashes, reset device via RTS pin",
  "actual": "SUCCESS in 34.52s. Application 888,352 B (574,589 compressed) written to 0x10000 in 7.7 seconds (919.1 kbit/s). Hash of data verified on all segments. Hard reset via RTS sent.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v081-upload-com5.txt"
  ],
  "evidence_kind": "Captured PlatformIO/esptool terminal output for v0.8.1 firmware upload",
  "limitations": "Flashing and hardware reset verified; physical observation of USB typing into PC Notepad and VaultApp PIN re-unlocking requested from user"
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0032",
  "date": "2026-11-04",
  "firmware_version": "v0.9.0",
  "commit": "WORKING_TREE",
  "environment": "Physical LILYGO T-Display-S3 on COM5 via esptool.py",
  "wiring": "Factory ST7789V 8-bit parallel, CST816S capacitive touch, AW9364 backlight",
  "verification_type": "HARDWARE",
  "scope": "Authorized v0.9.0 firmware upload with Web Portal Service and on-device Web Portal UI",
  "hardware": "ESP32-S3 revision v0.2, COM5 VID 303A PID 1001 MAC 44:1b:f6:d3:aa:bc",
  "procedure": "Upload complete v0.9.0 binary (942,800 B) via PlatformIO esptool directly to COM5 at 460800 baud",
  "expected": "Write intended v0.9.0 image (942,800 B), verify data hashes, reset device via RTS pin",
  "actual": "SUCCESS in 22.72s. Application 942,800 B (604,776 compressed) written to 0x10000 in 8.2 seconds (921.0 kbit/s). Hash of data verified on all segments. Hard reset via RTS sent.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v090-upload-com5.txt"
  ],
  "evidence_kind": "Captured PlatformIO/esptool terminal output for v0.9.0 firmware upload",
  "limitations": "Flashing and hardware reset verified; interactive validation of SoftAP connection, Web Portal UI, and [TYPE USB] requested from user"
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0033",
  "date": "2026-11-04",
  "firmware_version": "v0.9.1",
  "commit": "WORKING_TREE",
  "environment": "Physical LILYGO T-Display-S3 on COM5 via esptool.py",
  "wiring": "Factory ST7789V 8-bit parallel, CST816S capacitive touch, AW9364 backlight",
  "verification_type": "HARDWARE",
  "scope": "Authorized v0.9.1 firmware upload with redesigned branded Web Portal",
  "hardware": "ESP32-S3 revision v0.2, COM5 VID 303A PID 1001 MAC 44:1b:f6:d3:aa:bc",
  "procedure": "Upload complete v0.9.1 binary (956,976 B) via PlatformIO esptool directly to COM5 at 460800 baud",
  "expected": "Write intended v0.9.1 image (956,976 B), verify data hashes, reset device via RTS pin",
  "actual": "SUCCESS in 35.22s. Application 956,976 B (608,634 compressed) written to 0x10000 in 8.3 seconds (924.2 kbit/s). Hash of data verified on all segments. Hard reset via RTS sent.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v091-upload-com5.txt"
  ],
  "evidence_kind": "Captured PlatformIO/esptool terminal output for v0.9.1 firmware upload",
  "limitations": "Flashing and hardware reset verified; interactive validation of redesigned Web Portal visual identity and browser interaction requested from user"
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0034",
  "date": "2026-11-04",
  "firmware_version": "v0.9.2",
  "commit": "WORKING_TREE",
  "environment": "Physical LILYGO T-Display-S3 on COM5 via esptool.py",
  "wiring": "Factory ST7789V 8-bit parallel, CST816S capacitive touch, AW9364 backlight",
  "verification_type": "HARDWARE",
  "scope": "Authorized v0.9.2 firmware upload with LinkedIn links integrated into Web Portal",
  "hardware": "ESP32-S3 revision v0.2, COM5 VID 303A PID 1001 MAC 44:1b:f6:d3:aa:bc",
  "procedure": "Upload complete v0.9.2 binary (958,896 B) via PlatformIO esptool directly to COM5 at 460800 baud",
  "expected": "Write intended v0.9.2 image (958,896 B), verify data hashes, reset device via RTS pin",
  "actual": "SUCCESS in 35.15s. Application 958,896 B (609,720 compressed) written to 0x10000 in 8.3 seconds (924.5 kbit/s). Hash of data verified on all segments. Hard reset via RTS sent.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v092-upload-com5.txt"
  ],
  "evidence_kind": "Captured PlatformIO/esptool terminal output for v0.9.2 firmware upload",
  "limitations": "Flashing and hardware reset verified; interactive validation of Web Portal in browser requested from user"
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0035",
  "date": "2026-11-04",
  "firmware_version": "v0.9.3",
  "commit": "WORKING_TREE",
  "environment": "Physical LILYGO T-Display-S3 on COM5 via esptool.py",
  "wiring": "Factory ST7789V 8-bit parallel, CST816S capacitive touch, AW9364 backlight",
  "verification_type": "HARDWARE",
  "scope": "Authorized v0.9.3 firmware upload with 2FA Authenticator UI restoration on cancel",
  "hardware": "ESP32-S3 revision v0.2, COM5 VID 303A PID 1001 MAC 44:1b:f6:d3:aa:bc",
  "procedure": "Upload complete v0.9.3 binary (958,928 B) via PlatformIO esptool directly to COM5 at 460800 baud",
  "expected": "Write intended v0.9.3 image (958,928 B), verify data hashes, reset device via RTS pin",
  "actual": "SUCCESS in 34.98s. Application 958,928 B (609,112 compressed) written to 0x10000 in 8.3 seconds (924.3 kbit/s). Hash of data verified on all segments. Hard reset via RTS sent.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v093-upload-com5.txt"
  ],
  "evidence_kind": "Captured PlatformIO/esptool terminal output for v0.9.3 firmware upload",
  "limitations": "Flashing and hardware reset verified; physical observation of 2FA cancel UI requested from user"
}
```


```json
{
  "record_type": "test",
  "id": "HWT-0036",
  "date": "2026-11-04",
  "firmware_version": "v0.9.4",
  "commit": "WORKING_TREE",
  "environment": "Physical LILYGO T-Display-S3 on COM5 via esptool.py",
  "wiring": "Factory ST7789V 8-bit parallel, CST816S capacitive touch, AW9364 backlight",
  "verification_type": "HARDWARE",
  "scope": "Authorized v0.9.4 firmware upload with 2FA USB Cyber Action Capsule UI, touch long-press gating, and physical BOOT button trigger",
  "hardware": "ESP32-S3 revision v0.2, COM5 VID 303A PID 1001 MAC 44:1b:f6:d3:aa:bc",
  "procedure": "Upload complete v0.9.4 binary (959,440 B) via PlatformIO esptool directly to COM5 at 460800 baud",
  "expected": "Write intended v0.9.4 image (959,440 B), verify data hashes, reset device via RTS pin",
  "actual": "SUCCESS in 35.97s. Application 959,440 B (609,379 compressed) written to 0x10000 in 8.3 seconds (926.1 kbit/s). Hash of data verified on all segments. Hard reset via RTS sent.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v094-upload-com5.txt"
  ],
  "evidence_kind": "Captured PlatformIO/esptool terminal output for v0.9.4 firmware upload",
  "limitations": "Flashing and hardware reset verified; physical observation of Cyber USB Action Capsule, long-press touch sending, and BOOT button typing requested from user"
}
```


```json
{
  "record_type": "test",
  "id": "HWT-0037",
  "date": "2026-11-04",
  "firmware_version": "v0.9.4",
  "commit": "WORKING_TREE",
  "environment": "Physical LILYGO T-Display-S3 on COM5 via esptool.py",
  "wiring": "Factory ST7789V 8-bit parallel, CST816S capacitive touch, AW9364 backlight, USB HID composite",
  "verification_type": "HARDWARE",
  "scope": "Owner physical verification of 2FA USB Cyber Action Capsule, touch long-press gating, and physical BOOT button trigger",
  "hardware": "ESP32-S3 revision v0.2, COM5 VID 303A PID 1001 MAC 44:1b:f6:d3:aa:bc",
  "procedure": "Owner opens 2FA Authenticator on device, verifies the new Cyber USB Action Capsule UI layout, verifies that short tap shows 'Hold touch or press BTN', tests long press on the screen to type OTP code into PC Notepad, and presses the physical BOOT hardware button (GPIO 0) to type OTP code into PC Notepad",
  "expected": "Cyber USB UI renders cleanly without missing elements; short tap shows guidance without sending; long-press touch sends active OTP digits via USB HID keyboard emulation; physical BOOT button press sends active OTP digits via USB HID",
  "actual": "Owner confirmed physical test PASS ('نجح'): Cyber USB Action Capsule rendered cleanly, long press successfully typed 6-digit OTP code into PC via USB HID, and physical BOOT button successfully typed 6-digit OTP code into PC via USB HID.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v094-owner-hardware-confirmation.txt"
  ],
  "evidence_kind": "Owner report from interactive device session",
  "limitations": "Physical observation confirms UI appearance, long-press touch gating, and BOOT hardware button USB HID typing; RF Wi-Fi and long-term memory profiling scenarios A-E remain ongoing"
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0038",
  "date": "2026-11-04",
  "firmware_version": "v0.10.0",
  "commit": "WORKING_TREE",
  "environment": "Physical LILYGO T-Display-S3 on COM5 via esptool.py",
  "wiring": "Factory ST7789V 8-bit parallel, CST816S capacitive touch, AW9364 backlight, USB HID composite",
  "verification_type": "HARDWARE",
  "scope": "Authorized v0.10.0 firmware upload with MVP features suite (Generator, Clock, BLE, Vault categories, Web Portal Backup & Restore)",
  "hardware": "ESP32-S3 revision v0.2, COM5 VID 303A PID 1001 MAC 44:1b:f6:d3:aa:bc",
  "procedure": "Upload complete v0.10.0 binary (975,664 B) via PlatformIO esptool directly to COM5 at 460800 baud",
  "expected": "Write intended v0.10.0 image (975,664 B), verify data hashes, reset device via RTS pin",
  "actual": "SUCCESS in 36.02s. Application 975,664 B (618,309 compressed) written to 0x10000 in 8.5 seconds (913.9 kbit/s). Hash of data verified on all segments. Hard reset via RTS sent.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0100-upload-com5.txt"
  ],
  "evidence_kind": "Captured PlatformIO/esptool terminal output for v0.10.0 firmware upload",
  "limitations": "Flashing and hardware reset verified; physical observation of Password Generator, Desk Clock, and Web Portal Backup/Restore requested from user"
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0039",
  "date": "2026-11-04",
  "firmware_version": "v0.10.1",
  "commit": "WORKING_TREE",
  "environment": "Physical LILYGO T-Display-S3 on COM5 via esptool.py",
  "wiring": "Factory ST7789V 8-bit parallel, CST816S capacitive touch, AW9364 backlight, USB HID composite",
  "verification_type": "HARDWARE",
  "scope": "Authorized v0.10.1 firmware upload with Password Generator auto-save and Cyber Clock Screensaver",
  "hardware": "ESP32-S3 revision v0.2, COM5 VID 303A PID 1001 MAC 44:1b:f6:d3:aa:bc",
  "procedure": "Upload complete v0.10.1 binary (977,664 B) via PlatformIO esptool directly to COM5 at 460800 baud",
  "expected": "Write intended v0.10.1 image (977,664 B), verify data hashes, reset device via RTS pin",
  "actual": "SUCCESS in 22.43s. Application 977,664 B (619,704 compressed) written to 0x10000 in 8.6 seconds (911.3 kbit/s). Hash of data verified on all segments. Hard reset via RTS sent.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0101-upload-com5.txt"
  ],
  "evidence_kind": "Captured PlatformIO/esptool terminal output for v0.10.1 firmware upload",
  "limitations": "Flashing and hardware reset verified; physical observation of Clock screensaver and generator save flow requested from user"
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0040",
  "date": "2026-11-04",
  "firmware_version": "v0.10.2",
  "commit": "WORKING_TREE",
  "environment": "Physical LILYGO T-Display-S3 on COM5 via esptool.py",
  "wiring": "Factory ST7789V 8-bit parallel, CST816S capacitive touch, AW9364 backlight, ESP32-S3 BLE radio",
  "verification_type": "HARDWARE",
  "scope": "Authorized v0.10.2 firmware upload with Bluetooth LE HID Keyboard integration",
  "hardware": "ESP32-S3 revision v0.2, COM5 VID 303A PID 1001 MAC 44:1b:f6:d3:aa:bc",
  "procedure": "Upload complete v0.10.2 binary (1,454,448 B) via PlatformIO esptool directly to COM5 at 460800 baud",
  "expected": "Write intended v0.10.2 image (1,454,448 B), verify data hashes, reset device via RTS pin",
  "actual": "SUCCESS in 27.87s. Application 1,454,448 B (905,431 compressed) written to 0x10000 in 12.4 seconds (939.9 kbit/s). Hash of data verified on all segments. Hard reset via RTS sent.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0102-upload-com5.txt"
  ],
  "evidence_kind": "Captured PlatformIO/esptool terminal output for v0.10.2 firmware upload",
  "limitations": "Flashing and hardware reset verified; physical pairing and typing over BLE requested from user"
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0041",
  "date": "2026-11-04",
  "firmware_version": "v0.10.3",
  "commit": "WORKING_TREE",
  "environment": "Physical LILYGO T-Display-S3 on COM5 via esptool.py",
  "wiring": "Factory ST7789V 8-bit parallel, CST816S capacitive touch, AW9364 backlight, ESP32-S3 BLE radio",
  "verification_type": "HARDWARE",
  "scope": "Authorized v0.10.3 firmware upload with Settings Bluetooth touch routing and primary action fix",
  "hardware": "ESP32-S3 revision v0.2, COM5 VID 303A PID 1001 MAC 44:1b:f6:d3:aa:bc",
  "procedure": "Upload complete v0.10.3 binary (1,564,432 B) via PlatformIO esptool directly to COM5 at 460800 baud",
  "expected": "Write intended v0.10.3 image (1,564,432 B), verify data hashes, reset device via RTS pin",
  "actual": "SUCCESS in 29.11s. Application 1,564,432 B (978,652 compressed) written to 0x10000 in 13.6 seconds (919.8 kbit/s). Hash of data verified on all segments. Hard reset via RTS sent.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0103-upload-com5.txt"
  ],
  "evidence_kind": "Captured PlatformIO/esptool terminal output for v0.10.3 firmware upload",
  "limitations": "Flashing and hardware reset verified; physical pairing and typing over BLE requested from user"
}
```
