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
