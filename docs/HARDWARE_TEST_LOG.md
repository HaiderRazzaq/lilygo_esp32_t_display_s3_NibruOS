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
