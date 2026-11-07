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

```json
{
  "record_type": "test",
  "id": "HWT-0042",
  "date": "2026-11-04",
  "firmware_version": "v0.10.4",
  "commit": "WORKING_TREE",
  "environment": "Physical LILYGO T-Display-S3 on COM5 via esptool.py",
  "wiring": "Factory ST7789V 8-bit parallel, CST816S capacitive touch, AW9364 backlight, ESP32-S3 BLE radio",
  "verification_type": "HARDWARE",
  "scope": "Authorized v0.10.4 firmware upload with BLE HID null pointer initialization crash fix",
  "hardware": "ESP32-S3 revision v0.2, COM5 VID 303A PID 1001 MAC 44:1b:f6:d3:aa:bc",
  "procedure": "Upload complete v0.10.4 binary (1,565,488 B) via PlatformIO esptool directly to COM5 at 460800 baud",
  "expected": "Write intended v0.10.4 image (1,565,488 B), verify data hashes, reset device via RTS pin",
  "actual": "SUCCESS in 42.21s. Application 1,565,488 B (979,039 compressed) written to 0x10000 in 13.6 seconds (923.1 kbit/s). Hash of data verified on all segments. Hard reset via RTS sent.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0104-upload-com5.txt"
  ],
  "evidence_kind": "Captured PlatformIO/esptool terminal output for v0.10.4 firmware upload",
  "limitations": "Flashing and hardware reset verified; physical pairing and typing over BLE requested from user"
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0043",
  "date": "2026-11-04",
  "firmware_version": "v0.10.5",
  "commit": "WORKING_TREE",
  "environment": "Physical LILYGO T-Display-S3 on COM5/COM6 via esptool.py",
  "wiring": "Factory ST7789V 8-bit parallel, CST816S capacitive touch, AW9364 backlight, ESP32-S3 BLE radio",
  "verification_type": "HARDWARE",
  "scope": "Authorized v0.10.5 firmware upload with standardized BLE HID stack and watchdog recovery",
  "hardware": "ESP32-S3 revision v0.2, COM5/COM6 VID 303A PID 1001 MAC 44:1b:f6:d3:aa:bc",
  "procedure": "Upload complete v0.10.5 binary via PlatformIO esptool directly to device port at 460800 baud and issue watchdog reset",
  "expected": "Write intended v0.10.5 image, verify data hashes, reset device into active runtime mode (COM6)",
  "actual": "SUCCESS in 45.37s. Application 1,565,408 B (978,880 compressed) written to 0x10000 in 13.6s (921.8 kbit/s). Hash verified on all segments. Watchdog reset recovered board into active runtime mode (COM6).",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0105-upload-com5.txt"
  ],
  "evidence_kind": "Captured PlatformIO/esptool terminal output for v0.10.5 firmware upload",
  "limitations": "Flashing and hardware reset verified; physical pairing and typing over BLE requested from user"
}
```


```json
{
  "record_type": "test",
  "id": "HWT-0044",
  "date": "2026-11-04",
  "firmware_version": "v0.10.5",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Reproduce reported BLE activation interruption",
  "hardware": "LILYGO T-Display-S3 Touch Glass",
  "wiring": "Factory touch/display and native USB connection",
  "procedure": "Listen on COM6 while owner taps Settings > Bluetooth > ENABLE BLE KEYBOARD.",
  "expected": "Board stays active and shows BLE advertising.",
  "actual": "Owner reported the device turned off. Serial reached [BLE] Initializing BLE Device..., then the port reopened before the next BLE startup message.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-04-v0105-ble-repro.txt"
  ],
  "evidence_kind": "Captured output or identified transcript",
  "environment": "Physical board, Windows native USB CDC",
  "limitations": "No reset reason, timing measurement, or panic trace captured. The installed version is identified by the preceding upload record and the local artifact, not a captured boot banner."
}
```


```json
{
  "record_type": "test",
  "id": "HWT-0045",
  "date": "2026-11-04",
  "firmware_version": "v0.10.6",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "BLE startup and resource acceptance",
  "hardware": "LILYGO T-Display-S3 Touch Glass",
  "wiring": "Factory touch/display and native USB connection",
  "procedure": "After authorized upload, attach CDC at 115200. Enable BLE and capture controller/host logs and any reset reason. If advertising starts, pair with a phone/PC and use TEST KEYBOARD in a blank editor. Check Back, repeat enable/disable at least 20 times, and repeat with background Wi-Fi active. Run profiling scenarios A–E and measure headroom, stack, latency and current.",
  "expected": "No interruption; startup errors are visible and leave Settings usable. Successful startup supports pairing and typing. Maintain resource budgets.",
  "actual": "Pending upload and physical interaction.",
  "result": "NOT RUN",
  "evidence": [],
  "evidence_kind": "Pending procedure",
  "environment": "Physical board",
  "limitations": "Build and fault injection cannot verify the physical reset or pairing."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0046",
  "date": "2026-11-04",
  "firmware_version": "v0.10.6",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Initial authorized upload attempt using runtime COM6",
  "hardware": "LILYGO T-Display-S3 Touch Glass / ESP32-S3 revision v0.2",
  "wiring": "Factory touch/display and native USB connection",
  "procedure": "pio run -e lilygo-t-display-s3 -t upload --upload-port COM6",
  "expected": "Write the identified application and return to runtime USB mode.",
  "actual": "Exit 1 during connection. ClearCommError while the board changed from runtime COM6 to ROM COM5. Flash writing did not start.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-04-v0106-upload-com6-attempt.txt"
  ],
  "evidence_kind": "Captured esptool output and observed USB port enumeration",
  "environment": "Windows / PlatformIO / native USB",
  "limitations": "Retried on the enumerated ROM port."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0047",
  "date": "2026-11-04",
  "firmware_version": "v0.10.6",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Authorized v0.10.6 firmware upload",
  "hardware": "LILYGO T-Display-S3 Touch Glass / ESP32-S3 revision v0.2",
  "wiring": "Factory touch/display and native USB connection",
  "procedure": "pio run -e lilygo-t-display-s3 -t upload --upload-port COM5; after RTS left COM5 present, request esptool 5.6.0 watchdog reset on the ROM port.",
  "expected": "Write the identified application and return to runtime USB mode.",
  "actual": "Exit 0 for upload. Image 1,565,824 B at 0x10000; all written segments hash-verified. No NVS-range write or full-chip erase. The watchdog command lost COM5 after issuing reset and exited 1; runtime COM6 appeared and accepted the serial monitor.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0106-upload.txt",
    "docs/evidence/2026-11-04-v0106-reset.txt"
  ],
  "evidence_kind": "Captured esptool output and observed USB port enumeration",
  "environment": "Windows / PlatformIO / native USB",
  "limitations": "PASS is for flash write/hash verification and observed return of runtime port. BLE behavior is recorded separately."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0048",
  "date": "2026-11-04",
  "firmware_version": "v0.10.6",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Reproduction and crash localization after firmware upload",
  "hardware": "LILYGO T-Display-S3 Touch Glass / ESP32-S3 revision v0.2",
  "wiring": "Factory touch/display and native USB",
  "procedure": "Tap ENABLE BLE KEYBOARD while monitoring COM6. Read the reset reason from System info and decode the dedicated crash partition using the matching application ELF.",
  "expected": "Remain responsive through BLE enable/disable, coexist with Wi-Fi, and advertise and type after pairing.",
  "actual": "Owner confirmed the same interruption and System info showed Software Crash/Panic. COM6 dropped after controller initialization. Sanitized backtrace identifies abort in coex_core_enable via esp_bt_controller_enable. Internal heap before initialization was 232,972 B, largest block 221,172 B.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-04-v0106-ble-runtime.txt",
    "docs/evidence/2026-11-04-v0106-coredump-backtrace.txt"
  ],
  "evidence_kind": "Owner report, timestamped native USB log and sanitized crash backtrace",
  "environment": "Physical board / native USB",
  "limitations": "Heap snapshot before BLE controller startup only. No complete memory, latency, or power profiling. Raw task snapshots were removed after local decoding."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0049",
  "date": "2026-11-04",
  "firmware_version": "v0.10.7",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Wi-Fi/BLE coexistence physical acceptance",
  "hardware": "LILYGO T-Display-S3 Touch Glass / ESP32-S3 revision v0.2",
  "wiring": "Factory touch/display and native USB",
  "procedure": "After upload, enable BLE while Wi-Fi STA is connected. Pair with a phone/PC, use TEST KEYBOARD in a blank editor, then disable BLE and check Wi-Fi remains connected. Repeat with BLE enabled before Wi-Fi and perform at least 20 activation cycles and resource profiling.",
  "expected": "Remain responsive through BLE enable/disable, coexist with Wi-Fi, and advertise and type after pairing.",
  "actual": "Pending v0.10.7 retest.",
  "result": "NOT RUN",
  "evidence": [],
  "evidence_kind": "Pending procedure",
  "environment": "Physical board / native USB",
  "limitations": "Track immediate smoke verification separately from complete pairing and lifecycle profiling."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0050",
  "date": "2026-11-04",
  "firmware_version": "v0.10.7",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Authorized application-only upload",
  "hardware": "LILYGO T-Display-S3 Touch Glass / ESP32-S3 revision v0.2",
  "wiring": "Factory touch/display and native USB",
  "procedure": "Check image SHA-256. Use native USB 1200 baud transition from COM6 to ROM COM5, then esptool 5.6.0 write-flash 0x10000 with watchdog reset.",
  "expected": "Write and verify the identified app0 image, then observe runtime port.",
  "actual": "Wrote 1,566,096 B to app0 and verified data hash. COM5 disappeared after watchdog reset, causing a serial exception and exit 1 after successful verification. Runtime COM6 returned and accepted the serial monitor.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0107-upload.txt"
  ],
  "evidence_kind": "Captured esptool output and runtime USB enumeration",
  "environment": "Windows / native USB / esptool 5.6.0",
  "limitations": "PASS applies to application write, hash verification and observed runtime USB return. BLE behavior has separate evidence."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0051",
  "date": "2026-11-04",
  "firmware_version": "v0.10.7",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Original BLE activation restart regression",
  "hardware": "LILYGO T-Display-S3 Touch Glass / ESP32-S3 revision v0.2",
  "wiring": "Factory touch/display and native USB",
  "procedure": "Press ENABLE BLE KEYBOARD with runtime COM6 monitor attached after installing HWT-0050.",
  "expected": "Controller and host start, HID startup reaches advertising, and USB remains enumerated.",
  "actual": "BLE initialization completed through the advertising and battery setup log at 21:40:21+03:00. COM6 remained connected. Controller pre-init internal heap was 232,988 B with a 221,172 B largest block.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0107-ble-runtime.txt"
  ],
  "evidence_kind": "Timestamped native USB serial capture",
  "environment": "Physical board / native USB",
  "limitations": "Single activation smoke test. The log shows firmware advertising calls. RF discovery, pairing, typing, Wi-Fi link state and full lifecycle acceptance are tracked separately in HWT-0049."
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-04",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "hardware": "LILYGO T-Display-S3 Touch Glass and Windows PC",
  "wiring": "Factory wiring and native USB",
  "environment": "Physical board / Windows",
  "procedure": "Capture Bluetooth connection, auth, and disconnect while moving from Settings to Generator and testing BLE typing in a blank local editor.",
  "expected": "Windows remains connected and receives HID input.",
  "id": "HWT-0052",
  "firmware_version": "v0.10.7",
  "scope": "Windows BLE pairing and Generator typing",
  "actual": "Owner reported the board stayed on, paired with Windows, and Generator showed BLE not connected. Windows BTHUSB event 8 and SecureConnectionPaired confirm pairing succeeded.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-04-v0107-windows-ble.txt"
  ],
  "evidence_kind": "Owner report and Windows pairing event/property",
  "limitations": "No controller disconnect reason was logged by v0.10.7."
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-04",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "hardware": "LILYGO T-Display-S3 Touch Glass and Windows PC",
  "wiring": "Factory wiring and native USB",
  "environment": "Physical board / Windows",
  "procedure": "Capture Bluetooth connection, auth, and disconnect while moving from Settings to Generator and testing BLE typing in a blank local editor.",
  "expected": "Windows remains connected and receives HID input.",
  "id": "HWT-0053",
  "firmware_version": "v0.10.8",
  "scope": "Capture post-pairing BLE link loss on Windows",
  "actual": "Pending authorized upload and owner reproduction.",
  "result": "NOT RUN",
  "evidence": [],
  "evidence_kind": "Pending procedure",
  "limitations": "Requires the paired Windows host, board and USB serial monitor."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0054",
  "date": "2026-11-04",
  "firmware_version": "v0.10.8",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Authorized app-only diagnostic upload",
  "hardware": "LILYGO T-Display-S3 Touch Glass",
  "wiring": "Factory wiring and native USB",
  "procedure": "Verify local image hash and write app0 via esptool 5.6.0 on COM5.",
  "expected": "Verify written image and see runtime COM6.",
  "actual": "Wrote 1,566,464 bytes at 0x10000 and verified the data hash. COM5 disappeared during watchdog reset, producing a serial error after verified write. COM6 returned and serial monitoring opened.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0108-upload.txt",
    "docs/evidence/2026-11-04-v0108-ble-runtime.txt"
  ],
  "evidence_kind": "esptool output and USB CDC observation",
  "environment": "Windows / esptool 5.6.0",
  "limitations": "Upload result covers flash write and runtime enumeration. BLE results tracked separately."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0055",
  "date": "2026-11-04",
  "firmware_version": "v0.10.8",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Windows BLE disconnect and reported connected state",
  "hardware": "LILYGO T-Display-S3 Touch Glass and Windows PC",
  "wiring": "Factory wiring and native USB",
  "procedure": "Capture v0.10.8 BLE events during existing Windows pairing and after a Windows re-pair attempt. Compare Windows connected endpoints.",
  "expected": "Windows maintains the authenticated HID link and Generator can deliver to the intended host.",
  "actual": "Multiple remote disconnects 0x13 with advertising restarted successfully. The re-pair interval included authentication failure 0x55 and another remote disconnect. A later GATT Connected event lacked a logged authentication result and was shown green on the board. No matching connected Windows endpoint was returned by the subsequent query.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-04-v0108-ble-runtime.txt"
  ],
  "evidence_kind": "Timestamped USB serial trace and owner report, with Windows connection-selector observation",
  "environment": "Physical board / Windows",
  "limitations": "No peer identity captured. Missing Windows endpoint does not identify another device. The serial log does not prove key delivery or the underlying authentication cause."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0056",
  "date": "2026-11-04",
  "firmware_version": "v0.11.0",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Bluetooth Home, Windows pairing and HID stability",
  "hardware": "LILYGO T-Display-S3 Touch Glass and Windows PC",
  "wiring": "Factory wiring and native USB",
  "procedure": "Open Bluetooth from Home, enable it with Wi-Fi active, re-pair from Windows as needed and monitor encryption, CCCD and disconnect. Test TEST KEYBOARD in an empty local editor, return Home and test Generator only with a disposable value. Repeat reconnect and at least 20 cycles for resource acceptance.",
  "expected": "Pairing and ready states reflect the link, test input arrives, and connection remains usable across app navigation.",
  "actual": "Pending physical test.",
  "result": "NOT RUN",
  "evidence": [],
  "evidence_kind": "Pending procedure",
  "environment": "Physical board / Windows / native USB",
  "limitations": "Host mocks cannot confirm encryption, Windows HID behavior or runtime resources."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0057",
  "date": "2026-11-04",
  "firmware_version": "v0.11.0",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Authorized app-only upload",
  "hardware": "LILYGO T-Display-S3 Touch Glass",
  "wiring": "Factory wiring and native USB",
  "procedure": "Check local image SHA-256 and write app0 at 0x10000 using esptool 5.6.0 on COM5.",
  "expected": "Hash verification and return to runtime COM6.",
  "actual": "Wrote 1,567,296 B and verified data hash. Serial exception after watchdog reset as COM5 disappeared. Runtime COM6 returned and monitor opened.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0110-upload.txt",
    "docs/evidence/2026-11-04-v0110-ble-runtime.txt"
  ],
  "evidence_kind": "esptool transcript and native USB capture",
  "environment": "Windows / esptool / USB CDC",
  "limitations": "Write and runtime enumeration only. Pairing and HID results recorded separately."
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-04",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "hardware": "LILYGO T-Display-S3 Touch Glass and Windows PC",
  "wiring": "Factory hardware and USB CDC",
  "environment": "Physical board / Windows / native USB",
  "id": "HWT-0058",
  "firmware_version": "v0.11.0",
  "scope": "Reproduce Windows HID enumeration failure",
  "procedure": "Restart BLE on the board and then remove and pair the Windows device again while monitoring USB. Inspect Windows PnP child service UUIDs.",
  "expected": "Windows enumerates the HID service and subscribes after connecting.",
  "actual": "Fresh pairing authenticated successfully without a keyboard subscription. Board stayed at SETTING UP KEYBOARD. Windows enumerated 0x1800, 0x1801, 0x180A and 0x180F children and no 0x1812 child.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-04-v0110-retest-ble-runtime.txt",
    "docs/evidence/2026-11-04-v0110-windows-services.txt"
  ],
  "evidence_kind": "Timestamped serial capture, Windows PnP metadata and owner report",
  "limitations": "Windows PnP children are host enumeration evidence, not a raw GATT database dump. Startup heap checkpoints at 192,832 and 179,604 B differ by 13,228 B after a toggle; full repeat-cycle profile needed."
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-04",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "hardware": "LILYGO T-Display-S3 Touch Glass and Windows PC",
  "wiring": "Factory hardware and USB CDC",
  "environment": "Physical board / Windows / native USB",
  "id": "HWT-0059",
  "firmware_version": "v0.11.1",
  "scope": "Test Windows enumeration and encrypted BLE typing",
  "procedure": "Enable Bluetooth and add the keyboard in Windows after removing its existing Windows pairing if present. Confirm HID service start, GATT connection, authentication, CCCD on, and receipt of TEST KEYBOARD text in an empty local editor. Retest a reconnect and the 20-cycle resource scenarios.",
  "expected": "Windows enumerates HID, authenticates, subscribes and receives test text; unready links cannot send.",
  "actual": "Pending authorized device test.",
  "result": "NOT RUN",
  "evidence": [],
  "evidence_kind": "Pending procedure",
  "limitations": "Software validation cannot establish HID delivery or Windows bonded reconnect."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0060",
  "date": "2026-11-04",
  "firmware_version": "v0.11.1",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Authorized app0 upload",
  "hardware": "LILYGO T-Display-S3 Touch Glass",
  "wiring": "Factory wiring and USB CDC",
  "procedure": "Verify SHA-256 and flash app0 at 0x10000 from COM5 using esptool.",
  "expected": "Verify flash write and recover runtime COM6.",
  "actual": "1,572,160 B written and data hash verified. COM5 disappeared on watchdog reset and esptool returned a post-write serial error. Runtime COM6 returned and BLE connection events were captured.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0111-upload.txt",
    "docs/evidence/2026-11-04-v0111-ble-runtime.txt"
  ],
  "evidence_kind": "esptool transcript and runtime serial capture",
  "environment": "Windows / esptool 5.6.0 / USB",
  "limitations": "Flash and runtime-port evidence only."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0061",
  "date": "2026-11-04",
  "firmware_version": "v0.11.1",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Windows BLE keyboard startup and reconnect",
  "hardware": "LILYGO T-Display-S3 Touch Glass and Windows PC",
  "wiring": "Factory wiring and USB CDC",
  "procedure": "Observe Windows re-pairing and BLE disable/enable with serial capture.",
  "expected": "HID starts, Windows subscribes and test keys can be sent.",
  "actual": "HID service start result 0 and advertising result 0. User observed SETTING UP KEYBOARD and WAITING alternating. Multiple 0x13 disconnects before subscription. Authentication success occurred on initial link, with failures 0x55 and 0x66 on subsequent attempts. No keyboard notifications on event. USB remained connected.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-04-v0111-ble-runtime.txt"
  ],
  "evidence_kind": "Owner report and timestamped serial capture",
  "environment": "Physical board / Windows / native USB",
  "limitations": "The status confirms service registration and peer termination; the underlying reason for Windows terminating and HID delivery remain unverified."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0062",
  "date": "2026-11-04",
  "firmware_version": "v0.11.1",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Second-host isolation after Windows failure",
  "hardware": "LILYGO T-Display-S3 Touch Glass and user's phone",
  "wiring": "Factory hardware and USB CDC",
  "procedure": "Owner isolated Windows Bluetooth and tried a phone connection while serial monitoring remained active.",
  "expected": "HID starts, the phone authenticates, subscribes and NibruOS shows KEYBOARD READY.",
  "actual": "Owner reported the phone stayed connected and the board remained at SETTING UP KEYBOARD. Serial capture recorded HID start 0, authentication failure 0x55, then keyboard notifications on. Test input was unavailable.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-04-v0111-ble-runtime.txt"
  ],
  "evidence_kind": "Owner report and timestamped serial capture",
  "environment": "Physical board / phone / USB CDC",
  "limitations": "Phone platform was not reported. The trace shows subscription with failed authentication. It does not establish the precise SMP rejection cause."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0063",
  "date": "2026-11-04",
  "firmware_version": "v0.12.0",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Display-assisted authentication on Windows and phone",
  "hardware": "LILYGO T-Display-S3 Touch Glass and user's phone",
  "wiring": "Factory hardware and USB CDC",
  "procedure": "With only the test host's Bluetooth on, remove the prior host-side NibruOS pairing if present. Pair while the Bluetooth card is visible and enter the displayed code on the host. Verify authentication, subscription, TEST KEYBOARD in an empty editor, then reconnect.",
  "expected": "A one-time code is visible only during pairing, the authenticated and subscribed link becomes ready, and test text arrives.",
  "actual": "Pending authorized device test.",
  "result": "NOT RUN",
  "evidence": [],
  "evidence_kind": "Pending procedure",
  "environment": "Physical board / phone / USB CDC",
  "limitations": "Test Windows and phone separately; 20-cycle resource acceptance remains a separate requirement."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0064",
  "date": "2026-11-04",
  "firmware_version": "v0.12.0",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Authorized app0 upload",
  "hardware": "LILYGO T-Display-S3 Touch Glass",
  "wiring": "Factory wiring and native USB",
  "procedure": "Verify local image SHA-256 and write app0 at 0x10000 with esptool.",
  "expected": "Verify written data and recover runtime COM6.",
  "actual": "1,572,800 B written and hash verified. COM5 disappeared after watchdog reset, causing an esptool serial error after verification. Runtime COM6 returned and serial monitoring opened.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0120-upload.txt",
    "docs/evidence/2026-11-04-v0120-ble-runtime.txt"
  ],
  "evidence_kind": "Captured esptool output and USB serial observation",
  "environment": "Windows / esptool / USB CDC",
  "limitations": "Flash write and runtime-port evidence only."
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-04",
  "firmware_version": "v0.12.0",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "hardware": "LILYGO T-Display-S3 Touch Glass",
  "wiring": "Factory wiring and native USB",
  "evidence": [
    "docs/evidence/2026-11-04-v0120-ble-runtime.txt"
  ],
  "environment": "Physical ESP32-S3 / USB CDC",
  "id": "HWT-0065",
  "scope": "Windows passkey and HID subscription",
  "procedure": "Monitor display-assisted pairing and compare the USB serial result with Windows BTHUSB event 8.",
  "expected": "Successful authentication followed by keyboard subscription and test text delivery.",
  "actual": "Passkey notification, then successful authentication at 22:39:14 +03:00, coinciding with the NibruOS pairing event on Windows. No keyboard subscription event followed. The link closed with reason 0x13.",
  "result": "FAIL",
  "evidence_kind": "Timestamped USB serial and Windows event match",
  "limitations": "PASS for the individual authentication attempt only. Windows HID typing remains unverified."
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-04",
  "firmware_version": "v0.12.0",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "hardware": "LILYGO T-Display-S3 Touch Glass",
  "wiring": "Factory wiring and native USB",
  "evidence": [
    "docs/evidence/2026-11-04-v0120-ble-runtime.txt"
  ],
  "environment": "Physical ESP32-S3 / USB CDC",
  "id": "HWT-0066",
  "scope": "Phone passkey prompt and keyboard acceptance",
  "procedure": "Owner attempted pairing on the phone while monitoring the NibruOS BLE events.",
  "expected": "The phone prompts for the displayed code, then enables the input report and receives test text.",
  "actual": "Owner observed the incorrect PIN or passkey error immediately, before an entry field. The capture had passkey notifications, repeated 0x51/0x55 authentication failures and no subscription. Windows Bluetooth was still on at host status checks.",
  "result": "FAIL",
  "evidence_kind": "Owner report and timestamped USB serial",
  "limitations": "The trace omits peer addresses and Windows was not confirmed off throughout. A specific failure line cannot be attributed to the phone. No PIN or passkey value was recorded."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0067",
  "date": "2026-11-04",
  "firmware_version": "v0.12.1",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Authorized app0 upload and runtime USB enumeration",
  "hardware": "LILYGO T-Display-S3 Touch Glass ESP32-S3",
  "wiring": "Native USB",
  "procedure": "Verify the build hash, write only app0 at 0x10000 via ROM COM5, reset and open runtime COM6.",
  "expected": "The image verifies and runtime USB returns.",
  "actual": "Wrote 1,572,864 B and verified the flash hash. esptool exited 1 after the reset detached COM5. Runtime COM6 returned and opened at 115200.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0121-upload.txt",
    "docs/evidence/2026-11-04-v0121-runtime.txt"
  ],
  "evidence_kind": "Captured esptool output and USB serial observation",
  "environment": "Windows / esptool 5.6.0",
  "limitations": "The boot banner was not captured. Pairing, typing and physical resource metrics were not tested."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0068",
  "date": "2026-11-04",
  "firmware_version": "v0.12.1",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Windows reconnect with existing pairing",
  "hardware": "LILYGO T-Display-S3 Touch Glass and Windows Bluetooth host",
  "wiring": "USB for serial capture",
  "procedure": "Toggle BLE on NibruOS with the existing Windows pairing. Inspect COM6 logs and present Windows Keyboard devices.",
  "expected": "Authenticated peer subscribes to HID input and keyboard becomes ready.",
  "actual": "HID service started and authentication succeeded. No input subscription appeared in the capture. Owner confirmed SETTING UP KEYBOARD. Windows listed NibruOS under Bluetooth but no Bluetooth keyboard input device.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-04-v0121-ble-runtime.txt"
  ],
  "evidence_kind": "Serial output, Windows PnP inspection and owner report",
  "environment": "Windows and ESP32-S3",
  "limitations": "Existing pairing only. Fresh pairing and typing not verified by this record."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0069",
  "date": "2026-11-04",
  "firmware_version": "v0.12.1",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Windows pairing retry and HID enumeration",
  "hardware": "LILYGO T-Display-S3 Touch Glass and Windows Bluetooth host",
  "wiring": "Native USB for serial",
  "procedure": "Owner retries pairing after removing the Windows entry. Inspect serial and present PnP children of NibruOS Keyboard.",
  "expected": "Windows exposes a HID keyboard, enables notifications and NibruOS shows KEYBOARD READY.",
  "actual": "Owner reported no readiness. Present Windows children included 1800, 1801, 180A and 180F; none showed HID service 1812. The serial capture showed repeated disconnects and authentication failures and no keyboard subscription.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-04-v0121-ble-runtime.txt"
  ],
  "evidence_kind": "Serial output, live Windows PnP inspection and owner report",
  "environment": "Windows and ESP32-S3",
  "limitations": "PnP children do not show raw GATT transactions. Individual serial events were not correlated to the exact pairing click."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0070",
  "date": "2026-11-04",
  "firmware_version": "v0.13.0",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Authorized app0 upload",
  "hardware": "LILYGO T-Display-S3 Touch Glass",
  "wiring": "Native USB",
  "procedure": "Verify image SHA-256, write app0 at 0x10000 via COM5, then check runtime COM6.",
  "expected": "Written data verifies and runtime port opens.",
  "actual": "1,575,856 bytes written and hash verified. COM5 detached during watchdog reset and esptool returned a post-write serial error. Runtime COM6 returned and opened at 115200.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0130-upload.txt",
    "docs/evidence/2026-11-04-v0130-runtime.txt"
  ],
  "evidence_kind": "esptool output and runtime USB observation",
  "environment": "Windows / native USB",
  "limitations": "Boot banner, UI, RF transmission and resource metrics were not captured."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0071",
  "date": "2026-11-04",
  "firmware_version": "v0.14.0",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Authorized app0 upload",
  "hardware": "LILYGO T-Display-S3 Touch Glass",
  "wiring": "Native USB",
  "procedure": "Verify image SHA-256, write app0 at 0x10000 via COM5, then check runtime COM6.",
  "expected": "Written data verifies and runtime port opens.",
  "actual": "1,577,744 bytes written and hash verified. COM5 detached during watchdog reset and esptool returned a post-write serial error. Runtime COM6 returned and opened at 115200.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0140-upload.txt",
    "docs/evidence/2026-11-04-v0140-runtime.txt"
  ],
  "evidence_kind": "esptool output and runtime USB observation",
  "environment": "Windows / native USB",
  "limitations": "Boot banner, UI, RF transmission and resource metrics were not captured."
}
```


```json
{
  "record_type": "test",
  "id": "HWT-0072",
  "date": "2026-11-04",
  "firmware_version": "v0.14.2",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Authorized app0 upload for v0.14.2",
  "hardware": "LILYGO T-Display-S3 Touch Glass",
  "wiring": "Native USB",
  "procedure": "Verify image SHA-256, write app0 at 0x10000 via COM5, then check runtime port.",
  "expected": "Written data verifies, hash is confirmed, and device resets successfully.",
  "actual": "1,578,656 bytes written and hash verified in 13.6 seconds. Hard reset via RTS succeeded and runtime COM5 opened at 115200.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0142-upload.txt",
    "docs/evidence/2026-11-04-v0142-runtime.txt"
  ],
  "evidence_kind": "esptool output and runtime USB observation",
  "environment": "Windows / native USB",
  "limitations": "Physical touch accuracy and thumb comfort on real capacitive glass not yet measured."
}
```


```json
{
  "record_type": "test",
  "id": "HWT-0073",
  "date": "2026-11-04",
  "firmware_version": "v0.15.0",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Authorized app0 upload for v0.15.0 with Wi-Fi Defense IDS",
  "hardware": "LILYGO T-Display-S3 Touch Glass",
  "wiring": "Native USB",
  "procedure": "Verify image SHA-256, write app0 at 0x10000 via COM5, then check runtime port.",
  "expected": "Written data verifies, hash is confirmed, and device resets successfully into v0.15.0.",
  "actual": "1,583,328 bytes written and hash verified in 13.6 seconds. Hard reset via RTS succeeded and runtime COM5 opened at 115200.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0150-upload.txt",
    "docs/evidence/2026-11-04-v0150-runtime.txt"
  ],
  "evidence_kind": "esptool output and runtime USB observation",
  "environment": "Windows / native USB",
  "limitations": "Physical RF reception and real over-air deauth packet observation require laboratory radio emissions."
}
```
```json
{
  "record_type": "test",
  "id": "HWT-0074",
  "date": "2026-11-05",
  "firmware_version": "UNKNOWN; latest recorded upload v0.15.0 (HWT-0073)",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Owner-reported Beacon Lab operation after scheduling update",
  "hardware": "Owner ESP32-S3 board with phone/PC receivers described earlier in the conversation",
  "wiring": "Owner setup; not independently inspected",
  "procedure": "Owner ran Beacon Lab and reported its result after the proposed receiver check.",
  "expected": "Synthetic network advertisements are visible to the receiver.",
  "actual": "Owner confirmed the feature works successfully. Exact selected and received counts were not provided.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-05-beacon-owner-report.txt"
  ],
  "evidence_kind": "Owner report in this conversation",
  "environment": "Owner hardware",
  "limitations": "Limited functional confirmation. Exact running image, all forty names, WPA2 display, timing and resource scenarios are not independently verified."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0075",
  "date": "2026-11-05",
  "firmware_version": "v0.16.0",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Wi-Fi Channels receiver, UI and resource acceptance",
  "hardware": "LILYGO T-Display-S3 Touch Glass",
  "wiring": "Native USB for serial; baseline display/touch wiring",
  "procedure": "After an authorized upload, open Wi-Fi Channels, refresh, select channels by touch and swipe, compare counts and RSSI with known APs, rescan while connected, and use Back during a scan. Capture MemoryProfiler startup, entry, exit, twenty cycles and connected Wi-Fi combined workloads.",
  "expected": "Responsive UI, correct known-channel summaries, clean ownership release, preserved connection and at least 150 KiB free internal heap.",
  "actual": "Not run. v0.16.0 has not been uploaded in this task, and physical UI/RF/heap/stack/power measurements are unavailable.",
  "result": "NOT RUN",
  "evidence": [],
  "evidence_kind": "Pending physical procedure",
  "environment": "Physical ESP32-S3 required",
  "limitations": "Host cancellation cycles do not establish physical leak freedom or RF accuracy."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0076",
  "date": "2026-11-05",
  "firmware_version": "v0.17.0",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "BLE Lab receiver and resource acceptance",
  "hardware": "LILYGO T-Display-S3 Touch Glass and a BLE scanner receiver",
  "wiring": "Native USB for serial diagnostics",
  "procedure": "After authorized upload, run five identities for thirty seconds, inspect names, addresses and service UUID. Repeat with ten identities. Check Stop, Back, expiry, lock, sleep, subsequent keyboard activation and connected Wi-Fi. Capture resource scenarios A-E including twenty lifecycle cycles.",
  "expected": "Requested identities are observed, radio cleanup succeeds and internal heap remains at least 150 KiB.",
  "actual": "NOT RUN. The feature image has not been uploaded and no physical receiver or resource measurements were taken.",
  "result": "NOT RUN",
  "evidence": [],
  "evidence_kind": "Pending physical acceptance procedure",
  "environment": "ESP32-S3 hardware required",
  "limitations": "Host mocks and GAP start events cannot establish over-air reception or physical lifecycle stability."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0077",
  "date": "2026-11-05",
  "firmware_version": "v0.17.0",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Authorized app0 upload and runtime serial return",
  "hardware": "LILYGO T-Display-S3 Touch Glass",
  "wiring": "Native USB",
  "procedure": "Verify the image hash, write app0 at 0x10000 through COM5 and check runtime serial after watchdog reset.",
  "expected": "Written flash data verifies and the runtime serial port returns.",
  "actual": "1,588,688 bytes written in 11.3 seconds and flash hash verified. COM5 detached during watchdog reset, producing an esptool post-write serial exception. Runtime COM6 returned and opened at 115200.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-05-v0170-upload.txt",
    "docs/evidence/2026-11-05-v0170-runtime.txt"
  ],
  "evidence_kind": "Captured esptool output and USB serial observation",
  "environment": "Windows / esptool 5.6.0",
  "limitations": "The boot banner was not captured. BLE receiver discovery, touch and runtime resources were not tested."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0078",
  "date": "2026-11-05",
  "firmware_version": "v0.18.0",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Accessory discovery notifications and resources",
  "hardware": "ESP32-S3 with Android/Windows receiver; Apple/Samsung receivers for their profiles",
  "wiring": "Native USB",
  "procedure": "Select a matching profile, five identities and thirty seconds. Check receiver discovery settings and observe notifications. Record receiver model and OS version. Test stop, timeout, Back, twenty cycles, Wi-Fi coexistence and A-E resource scenarios.",
  "expected": "Reference-compatible receivers may show accessory discovery notifications; cleanup and resource limits remain satisfied.",
  "actual": "NOT RUN. Payload format tests do not establish receiver notifications.",
  "result": "NOT RUN",
  "evidence": [],
  "evidence_kind": "Pending receiver acceptance procedure",
  "environment": "Physical receivers required",
  "limitations": "Availability of registered models, platform filtering and notification settings affect results."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0079",
  "date": "2026-11-05",
  "firmware_version": "v0.18.0",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Authorized app0 upload and runtime USB return",
  "hardware": "LILYGO T-Display-S3 Touch Glass",
  "wiring": "Native USB",
  "procedure": "Verify build SHA-256, write app0 at 0x10000 via COM5, reset and check runtime serial.",
  "expected": "Written flash data verifies and runtime serial returns.",
  "actual": "1,589,984 bytes written in 11.8 seconds and flash hash verified. COM5 detached during watchdog reset, causing a post-write serial exception. Runtime COM6 returned and opened at 115200.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-05-v0180-upload.txt",
    "docs/evidence/2026-11-05-v0180-runtime.txt"
  ],
  "evidence_kind": "Captured esptool output and USB observation",
  "environment": "Windows / esptool 5.6.0",
  "limitations": "Boot banner and receiver notifications were not captured. Physical resource acceptance remains pending."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0080",
  "date": "2026-11-05",
  "firmware_version": "v0.18.0 (latest upload HWT-0079)",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Windows notification and received BLE payloads",
  "hardware": "Owner ESP32-S3 and Windows Bluetooth receiver",
  "wiring": "Native USB for serial",
  "procedure": "Observe BLE packets in Windows while the owner runs Android and Windows profiles, then record the owner notification report.",
  "expected": "Matching payloads are received and Windows may show Swift Pair notification.",
  "actual": "Captured five FE2C model records followed by ten Microsoft records with Classroom Buds01-10 names, at reported RSSI -10 to -12 dBm. The owner confirmed notifications appeared and described Classroom Buds text. Serial recorded 232452 B free internal heap and 221172 B largest block before controller start.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-05-v0180-ble-observation.txt",
    "docs/evidence/2026-11-05-v0180-ble-serial.txt",
    "docs/evidence/2026-11-05-swift-pair-owner-report.txt"
  ],
  "evidence_kind": "Captured Bleak 3.0.2 receiver output, serial output and owner report",
  "environment": "Windows receiver; observer dependencies isolated under .testbuild",
  "limitations": "Notification display is owner-reported. The initial no-notification report preceded matched Windows profile reception. No controlled comparison proves a single cause. Android notification display and post-start resource minima were not measured."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0081",
  "date": "2026-11-05",
  "firmware_version": "v0.19.0",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Target picker, Random and resource acceptance",
  "hardware": "ESP32-S3 and matching receiver devices",
  "wiring": "Native USB",
  "procedure": "Choose each target via touch and BOOT, exercise Back and same-contact suppression, run Random with bounded duration, and test stop, timeout, lock and sleep. Measure resource scenarios A-E and twenty cycles.",
  "expected": "Correct target selection, random data/address rotation and clean radio recovery.",
  "actual": "NOT RUN. Host tests do not establish physical touch or Random notification behavior.",
  "result": "NOT RUN",
  "evidence": [],
  "evidence_kind": "Pending physical procedure",
  "environment": "Physical board and receivers required",
  "limitations": "Windows v0.18.0 success does not validate the new picker or Random mode."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0082",
  "date": "2026-11-05",
  "firmware_version": "v0.19.0",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Authorized app0 upload and runtime USB return",
  "hardware": "LILYGO T-Display-S3 Touch Glass",
  "wiring": "Native USB",
  "procedure": "Verify the build hash, write app0 at 0x10000 through COM5, reset and open runtime serial.",
  "expected": "Written data verifies and runtime serial returns.",
  "actual": "1,590,992 bytes written in 11.8 seconds and flash hash verified. COM5 detached during watchdog reset and esptool reported a post-write serial exception. Runtime COM6 returned and opened at 115200.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-05-v0190-upload.txt",
    "docs/evidence/2026-11-05-v0190-runtime.txt"
  ],
  "evidence_kind": "Captured esptool output and USB observation",
  "environment": "Windows / esptool 5.6.0",
  "limitations": "Boot banner, new picker touch behavior, Random receiver notifications and runtime resource acceptance were not captured."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0083",
  "date": "2026-11-05",
  "firmware_version": "v0.19.0",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Owner-requested repeat upload of verified v0.19.0",
  "hardware": "LILYGO T-Display-S3 Touch Glass",
  "wiring": "Native USB",
  "procedure": "Check the existing image hash and write only app0 at 0x10000 through COM5; check runtime COM6.",
  "expected": "Flash data verifies and runtime serial returns.",
  "actual": "The same 1,590,992 B image was written in 11.8 seconds and its flash hash verified. COM5 detached during watchdog reset, producing the post-write serial exception. Runtime COM6 returned and opened at 115200.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-05-v0190-reupload.txt",
    "docs/evidence/2026-11-05-v0190-reupload-runtime.txt"
  ],
  "evidence_kind": "Captured esptool output and USB observation",
  "environment": "Windows / esptool 5.6.0",
  "limitations": "Repeat installation only. Physical UI, receiver notifications and resource acceptance remain separately scoped."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0084",
  "date": "2026-11-05",
  "firmware_version": "v0.20.0",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Apple Action, Apple Not Your Device and Beacon advertisement reception",
  "hardware": "ESP32-S3 and Apple/Android/Windows receivers",
  "wiring": "Native USB for serial",
  "procedure": "Build and upload v0.20.0 under explicit owner instruction. Open BLE Lab, switch to picker page 2, run each new profile with one identity and 60 seconds, observe receiver notification behavior, then test Back, lock, sleep and twenty cycles.",
  "expected": "The board advertises bounded 31-byte payloads, ends sessions cleanly and the matching receiver may display its alert or discovery UI.",
  "actual": "NOT RUN. No v0.20.0 image was uploaded and no receiver was tested. Build, host payload and documentation checks do not establish physical behavior.",
  "result": "NOT RUN",
  "evidence": [],
  "evidence_kind": "Pending physical procedure",
  "environment": "Physical board and matching receivers required",
  "limitations": "Receiver filtering varies by OS, vendor software, location permission and nearby-device settings."
}
```

Correction (2026-11-05, after HWT-0084): v0.20.0 was subsequently uploaded and
hash-verified under HWT-0085. HWT-0084's receiver procedure remains NOT RUN; its
statement that no image had been uploaded was accurate when that record was
written.

```json
{
  "record_type": "test",
  "id": "HWT-0085",
  "date": "2026-11-05",
  "firmware_version": "v0.20.0",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Upload and flash hash verification only",
  "hardware": "LILYGO T-Display-S3 Touch Glass",
  "wiring": "Native USB",
  "procedure": "Write app0 through PlatformIO/esptool and verify the write hash. First attempt used the stale COM6 port; retry used the re-enumerated COM5 port.",
  "expected": "The 1,592,096 B image writes completely and esptool verifies its hash.",
  "actual": "The COM6 attempt failed while Connecting with ClearCommError and wrote no data (upload-attempt evidence). COM5 then reported Writing to 100 percent and 'Hash of data verified'. Upload finished SUCCESS in 48.43 seconds. Runtime receiver behavior was not exercised. A post-upload DTR/RTS probe entered ROM download mode; esptool chip_id with --after hard_reset restored normal boot, but the boot banner was not captured.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-05-v0200-upload.txt",
    "docs/evidence/2026-11-05-v0200-upload-attempt.txt",
    "docs/evidence/2026-11-05-v0200-build.txt",
    "docs/evidence/2026-11-05-v0200-reset-attempt.txt"
  ],
  "evidence_kind": "Captured esptool output and USB port observation",
  "environment": "Windows / PlatformIO 6.2.0 / esptool 4.5.1",
  "limitations": "Installation only. Runtime banner, BLE receiver alerts, UI touch acceptance, heap/latency/power and twenty cycles remain NOT RUN."
}
```

Correction (2026-11-05, after HWT-0085): the statement that a post-upload
esptool reset "restored normal boot" was wrong. Later no-reset synchronization
and ROM-banner captures proved the chip stayed in the ROM USB download mode. See
HWT-0086 and `docs/evidence/2026-11-05-v0200-runtime-probe.txt`.

```json
{
  "record_type": "test",
  "id": "HWT-0086",
  "date": "2026-11-05",
  "firmware_version": "v0.20.0",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Post-upload runtime boot and serial response",
  "hardware": "LILYGO T-Display-S3 Touch Glass",
  "wiring": "Native USB",
  "procedure": "After HWT-0085, test whether the chip runs the application: no-reset esptool sync, DTR/RTS strap experiments, stub reboot command and a TIME serial probe over COM5.",
  "expected": "The chip leaves the bootloader, the v0.20.0 application runs and the TIME serial command returns a TimeService acknowledgement.",
  "actual": "BLOCKED. Every software reset sampled boot:0x21 DOWNLOAD(USB/UART0) and printed 'waiting for download'. no_reset esptool sync kept reporting 'Staying in bootloader'. The stub reboot and soft-reset attempts did not start the app. TIME:1762340000 returned NO_RESPONSE. A physical RESET press or USB re-plug is required.",
  "result": "BLOCKED",
  "evidence": [
    "docs/evidence/2026-11-05-v0200-runtime-probe.txt",
    "docs/evidence/2026-11-05-v0200-reset-attempt.txt"
  ],
  "evidence_kind": "Captured ROM banner, esptool output and serial probe",
  "environment": "Windows / PlatformIO 6.2.0 / esptool 4.5.1 / pyserial",
  "limitations": "The upload and its hash remain verified under HWT-0085. This record does not evaluate the application itself; it documents that the board could not be returned to normal boot without physical intervention."
}
```

Update (2026-11-05, after HWT-0086): the owner pressed the physical RESET
button / re-plugged USB. The board then re-enumerated as COM6 and the running
application answered the serial TIME command. See HWT-0087.

```json
{
  "record_type": "test",
  "id": "HWT-0087",
  "date": "2026-11-05",
  "firmware_version": "v0.20.0",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Runtime boot after physical reset",
  "hardware": "LILYGO T-Display-S3 Touch Glass",
  "wiring": "Native USB",
  "procedure": "Owner performed a physical RESET / USB re-plug. Then list USB serial devices and send TIME:1762340000 at 115200 with DTR asserted on the re-enumerated port.",
  "expected": "The application enumerates its native USB CDC and answers the serial time command.",
  "actual": "PASS. The device re-enumerated as COM6 with SER=441BF6D3AABC (the ROM USB-JTAG had reported SER=44:1B:F6:D3:AA:BC on COM5). The application replied '[TimeService] Time set: 1762340000 (source: USB)', proving the firmware loop is running. The boot banner was not captured because native USB CDC prints it before a host can open the port.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-05-v0200-runtime.txt"
  ],
  "evidence_kind": "Captured serial response and USB enumeration",
  "environment": "Windows / Python pyserial 3.x",
  "limitations": "Runtime boot only. BLE receiver alerts, page-two touch acceptance, heap/largest block, stack high-water marks, PSRAM, latency, power and twenty cycles remain NOT RUN."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0088",
  "date": "2026-11-05",
  "firmware_version": "v0.22.0",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "v0.22.0 firmware upload and hash verification",
  "hardware": "LILYGO T-Display-S3 Touch Glass",
  "wiring": "Native USB CDC on COM5 (ROM bootloader)",
  "procedure": "pio run -e lilygo-t-display-s3 -t upload --upload-port COM5",
  "expected": "esptool successfully connects, erases flash sectors, writes 1,594,896 bytes, and verifies flash hash with exit code 0.",
  "actual": "PASS. esptool v4.5.1 wrote 1,594,896 bytes at 460800 baud in 13.8 seconds. Hash of data verified. Returned SUCCESS in 45.67 seconds.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-05-v0220-upload.txt"
  ],
  "evidence_kind": "Captured esptool output",
  "environment": "Windows / PlatformIO espressif32@6.5.0",
  "limitations": "Flash write and hash verified. A physical RESET press is required to exit download mode and boot runtime application."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0089",
  "date": "2026-11-07",
  "firmware_version": "v0.22.1",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "v0.22.1 firmware upload and flash hash verification",
  "hardware": "LILYGO T-Display-S3 Touch Glass",
  "wiring": "Native USB CDC on COM5 (ROM bootloader)",
  "procedure": "pio run -e lilygo-t-display-s3 -t upload --upload-port COM5",
  "expected": "esptool successfully connects, erases flash sectors, writes 1,594,992 bytes, and verifies flash hash with exit code 0.",
  "actual": "PASS. esptool v4.5.1 wrote 1,594,992 bytes at 460800 baud in 13.7 seconds. Hash of data verified on all flash segments. Returned SUCCESS in 45.01 seconds.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-07-v0221-upload.txt"
  ],
  "evidence_kind": "Captured esptool output",
  "environment": "Windows / PlatformIO espressif32@6.5.0",
  "limitations": "Flash write and hash verified. A physical RESET press is required to exit download mode and boot runtime application."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0090",
  "date": "2026-11-07",
  "firmware_version": "v0.22.2",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "v0.22.2 firmware upload and flash hash verification",
  "hardware": "LILYGO T-Display-S3 Touch Glass",
  "wiring": "Native USB CDC on COM5 (ROM bootloader)",
  "procedure": "pio run -e lilygo-t-display-s3 -t upload --upload-port COM5",
  "expected": "esptool successfully connects, erases flash sectors, writes 1,595,072 bytes, and verifies flash hash with exit code 0.",
  "actual": "PASS. esptool v4.5.1 wrote 1,595,072 bytes at 460800 baud in 13.8 seconds. Hash of data verified on all flash segments. Returned SUCCESS in 45.62 seconds.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-07-v0222-upload.txt"
  ],
  "evidence_kind": "Captured esptool output",
  "environment": "Windows / PlatformIO espressif32@6.5.0",
  "limitations": "Flash write and hash verified. A physical RESET press is required to exit download mode and boot runtime application."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0091",
  "date": "2026-11-07",
  "firmware_version": "v0.22.3",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "v0.22.3 firmware upload, flash hash verification, and antenna diagnostic meter deployment",
  "hardware": "LILYGO T-Display-S3 Touch Glass",
  "wiring": "Native USB CDC on COM5 (ROM bootloader)",
  "procedure": "pio run -e lilygo-t-display-s3 -t upload --upload-port COM5",
  "expected": "esptool successfully connects, erases flash sectors, writes 1,595,104 bytes, and verifies flash hash with exit code 0.",
  "actual": "PASS. esptool v4.5.1 wrote 1,595,104 bytes at 460800 baud in 13.7 seconds. Hash of data verified on all flash segments. Returned SUCCESS in 45.38 seconds.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-07-v0223-upload.txt"
  ],
  "evidence_kind": "Captured esptool output",
  "environment": "Windows / PlatformIO espressif32@6.5.0",
  "limitations": "Flash write and hash verified. Board remains in download bootloader until user physically taps the RST button."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0092",
  "date": "2026-11-07",
  "firmware_version": "v0.22.4",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "v0.22.4 firmware upload, flash hash verification, and passive scan deployment",
  "hardware": "LILYGO T-Display-S3 Touch Glass",
  "wiring": "Native USB CDC on COM5 (ROM bootloader)",
  "procedure": "pio run -e lilygo-t-display-s3 -t upload --upload-port COM5",
  "expected": "esptool successfully connects, erases flash sectors, writes 1,595,296 bytes, and verifies flash hash with exit code 0.",
  "actual": "PASS. esptool v4.5.1 wrote 1,595,296 bytes at 460800 baud in 13.8 seconds. Hash of data verified on all flash segments. Returned SUCCESS in 45.02 seconds.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-07-v0224-upload.txt"
  ],
  "evidence_kind": "Captured esptool output",
  "environment": "Windows / PlatformIO espressif32@6.5.0",
  "limitations": "Flash write and hash verified. A physical tap of RST button is required to leave bootloader."
}
```

```json
{
  "record_type": "test",
  "id": "HWT-0093",
  "date": "2026-11-07",
  "firmware_version": "v0.22.4",
  "commit": "WORKING_TREE",
  "verification_type": "HARDWARE",
  "scope": "Physical hardware validation of external IPEX antenna Wi-Fi scanning and zero-flicker UI in v0.22.4",
  "hardware": "LILYGO T-Display-S3 Touch Glass (ESP32-S3R8) with external 2.4 GHz antenna via IPEX connector",
  "wiring": "External 2.4 GHz rubber duck antenna connected via IPEX-to-SMA pigtail cable; 0-ohm jumper resistor rotated to yellow external pad",
  "procedure": "Owner booted v0.22.4 runtime after RST, launched Wi-Fi Scanner, tested active probe and passive beacon scanning across 2.4 GHz channels 1-13.",
  "expected": "Wi-Fi scan successfully discovers surrounding APs with solid RSSI; screen displays cards cleanly with zero flickering or strobing.",
  "actual": "PASS. Owner confirmed physical operation: 'نعم الان يعمل بشكل ممتاز' (Yes, now it works excellently). APs discovered across channels, external antenna reception confirmed working, screen updates differentially with rock-solid stability and zero flicker.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-07-v0224-physical-pass.txt"
  ],
  "evidence_kind": "Owner physical hardware test confirmation and runtime observation",
  "environment": "Physical hardware / LILYGO T-Display-S3 Touch Glass",
  "limitations": "Physical validation of Wi-Fi scanning, passive beacon mode, and display rendering confirmed. Power consumption and long-term 20-cycle automated battery telemetry remain pending."
}
```
