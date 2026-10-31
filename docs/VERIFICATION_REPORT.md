# NibruOS v0.1 — verification report

> Historical package report below is preserved. Fresh 2026-11-01 observations
> are appended under their own dated section; current coverage is in
> [TEST_MATRIX](TEST_MATRIX.md). Earlier NOT RUN outcomes describe the creation
> environment and must not be read as this machine's latest build status.

**Scope:** LILYGO T-Display-S3 Touch Glass, ESP32-S3R8, native 170×320 ST7789 LCD,
CST816S touch, TFT_eSPI library, Arduino/PlatformIO project.

## Verified here

- Desktop C++17 mock build for all application, shell, resource, and hardware wrapper sources: **PASS**.
- Desktop C++17 mock build for the actual `src/main.cpp`: **PASS**.
- Executed host smoke assertions for app registration, Home navigation, touch
  coordinate and gesture rotation, button short/long/back events, Wi-Fi ownership,
  asynchronous scan state changes, scanned record copying, timeout, radio
  initialization failure, scan-start failure, scan-completion failure, cleanup,
  and returning Home: **PASS**.
- Checked the board's display macro definitions against Bodmer TFT_eSPI's
  `User_Setups/Setup206_LilyGo_T_Display_S3.h`: **MATCH**, including 8-bit bus
  D0..D7=39..48, WR=8, RD=9, CS=6, DC=7, RST=5, BL=38.
- Checked I2C GPIO18/17, display power GPIO15, and buttons GPIO0/14 against
  LilyGO documentation and the user's working sketch.
- CST816S API shape (`CST816S` constructor, `begin(FALLING)`, `available()`,
  `data.gestureID`, `data.x/y`) reviewed against the upstream library headers.

## Not verified here

- **PlatformIO target build NOT RUN:** PlatformIO SDK/ESP32 platform and external
  dependencies are not installed, and this runtime cannot reach remote registries.
- **Firmware upload NOT RUN:** no USB-attached LILYGO board in the environment.
- **Physical display/touch/gesture tests NOT RUN:** read the device acceptance
  checklist in `docs/BUILD_AND_TEST.md` and report any deviations.

Therefore the package contains **source code**, not a prebuilt `.bin` firmware.
The host tests use substitutes for hardware drivers and are not a guarantee of
ESP32 compatibility. A Windows PlatformIO build and on-device tests are the
next acceptance gates.

## Official references

- Board: https://docs.platformio.org/en/stable/boards/espressif32/lilygo-t-display-s3.html
- LilyGO technical page: https://wiki.lilygo.cc/products/t-display-series/t-display-s3/
- TFT_eSPI Setup206: https://github.com/Bodmer/TFT_eSPI/blob/master/User_Setups/Setup206_LilyGo_T_Display_S3.h
- CST816S API: https://github.com/fbiego/CST816S

## 2026-10-26 — v0.1.1 Wi-Fi reliability patch
- Re-ran local host C++ smoke suite (`bash tests/host/run_tests.sh`) after adding Wi-Fi state-machine changes: **PASS**.
- New mock tests cover Wi-Fi STA warmup, non-blocking progression, timeout -> stop/restart -> retry, retry success and final failure, async scan failure, delayed callback result after nominal timeout, cleanup during app exit, resource ownership and existing touch/button navigation.
- Recompiled `src/main.cpp` against mocked Arduino dependencies during the same test run: **PASS**.
- Packaged and ZIP-integrity-checked the AI-ready project, including source, docs, tests, full pinout image and standalone radio diagnostic.
- **Did NOT execute real PlatformIO firmware compilation** (`pio` unavailable here), did NOT flash or test on physical board; Wi-Fi timeout root cause and its real-board resolution remain **unverified**.


## 2026-11-01 — fresh verification records

The preceding report is preserved historical package evidence. The records below
are new observations, not revisions of its original outcomes. Current coverage:
[TEST_MATRIX](TEST_MATRIX.md).

```json
{
  "record_type": "test",
  "id": "VER-0003",
  "date": "2026-11-01T00:18:29+03:00",
  "firmware_version": "Not applicable; repository tooling only",
  "commit": "UNCOMMITTED documentation changes based on ccbd0e6",
  "verification_type": "DOCUMENTATION",
  "scope": "Initial checker integration and mutation suite",
  "hardware": "None",
  "wiring": "Not applicable",
  "procedure": "Run python tools/check_docs.py and python -m unittest discover -s tests/docs -v after initial implementation.",
  "expected": "Repository checks and all 14 mutation tests pass.",
  "actual": "Checker reported 3 missing-record/evidence errors; suite ran 14 tests with 2 failures: missing verification records and a formatting-dependent mutation fixture.",
  "result": "FAIL",
  "evidence": ["docs/evidence/2026-11-01-documentation.txt"],
  "evidence_kind": "Observed command output summarized without the enormous assertion dump",
  "environment": "Windows, Python 3.12",
  "limitations": "Intermediate implementation check; follow-up results are appended separately. No firmware/hardware assertions."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0001",
  "date": "2026-11-01T00:14:58+03:00",
  "firmware_version": "v0.1 runtime / v0.1.1-source-patch documentation",
  "commit": "ccbd0e625837196ce547f16788022be6465786cb",
  "verification_type": "BUILD",
  "scope": "Main firmware target build, including fresh object rebuild",
  "hardware": "No device accessed; configured Touch Glass target only",
  "wiring": "Not applicable to compilation",
  "procedure": "Run installed PlatformIO Core 6.2.0: pio run -e lilygo-t-display-s3. After default clean lock warnings, set PLATFORMIO_BUILD_DIR to .testbuild/pio-build and rerun to compile all source/framework units in a new directory.",
  "expected": "Compile/link firmware with pinned platform/core and current libraries.",
  "actual": "Initial target build exit0 SUCCESS (11.82s). Fresh isolated build exit0 SUCCESS (19.84s); RAM45768/327680, flash733957/6553600. Warnings: no parallel DMA, TFT_eSPI TOUCH_CS absent, upstream core uartSetPins returns without value.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-target-build.txt",
    "docs/DEPENDENCIES.md"
  ],
  "evidence_kind": "Captured complete fresh-build output, local account paths sanitized; initial result transcribed from tool output.",
  "environment": "Windows PowerShell; installed PlatformIO executable outside PATH; native USB flags unchanged.",
  "limitations": "No upload/device test. Standalone diagnostic project not built. Linker RAM is not measured heap stability. Firmware/config source unchanged from baseline."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0002",
  "date": "2026-11-01T00:14:58+03:00",
  "firmware_version": "v0.1 runtime / v0.1.1-source-patch documentation",
  "commit": "ccbd0e625837196ce547f16788022be6465786cb",
  "verification_type": "HOST",
  "scope": "Current mocked firmware smoke suite",
  "hardware": "None; local framework stubs",
  "wiring": "No physical wiring",
  "procedure": "Invoke tests/host/run_tests.sh with installed Git Bash.",
  "expected": "Compile mocked sources/main and execute assertions.",
  "actual": "c++: command not found; exited127 before compilation or assertions.",
  "result": "BLOCKED",
  "evidence": [
    "docs/evidence/2026-11-01-environment.txt",
    "tests/host/run_tests.sh"
  ],
  "evidence_kind": "Captured environment error transcribed from tool result.",
  "environment": "Windows + Git Bash; no desktop C++ compiler found in Bash PATH.",
  "limitations": "Legacy host PASS in preserved report is historical, not a new rerun. No hardware conclusions."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0004",
  "date": "2026-11-01T00:14:58+03:00",
  "firmware_version": "v0.1 runtime",
  "commit": "ccbd0e625837196ce547f16788022be6465786cb",
  "verification_type": "STATIC",
  "scope": "Default PlatformIO build cleanup prerequisite",
  "hardware": "None",
  "wiring": "Not applicable",
  "procedure": "pio run -e lilygo-t-display-s3 -t clean",
  "expected": "Remove existing target build products.",
  "actual": "Returned0 but reported WinError32 file locks and WinError145 nonempty directory. Cleanup incomplete; used isolated build folder instead.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-01-environment.txt"
  ],
  "evidence_kind": "Observed command errors, output excerpts transcribed.",
  "environment": "Windows workspace; lock owner UNKNOWN.",
  "limitations": "Failure is local cleanup, not target compilation or runtime failure; workaround does not resolve locking process."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0005",
  "date": "2026-11-01T00:21:32+03:00",
  "firmware_version": "Not applicable; repository tooling only",
  "commit": "UNCOMMITTED documentation/tooling changes based on ccbd0e625837196ce547f16788022be6465786cb",
  "verification_type": "DOCUMENTATION",
  "scope": "Completed documentation system, validator and regression suite",
  "hardware": "None; temporary repository copies for mutation tests",
  "wiring": "Not applicable",
  "procedure": "Run python tools/check_docs.py and python -m unittest discover -s tests/docs -v. Review whole change against user requirements and compare protected source/history with HEAD.",
  "expected": "Valid baseline accepted; broken links/records/evidence rejected; historical source renames preserve immutable logs; physical records stay in their dedicated log.",
  "actual": "Documentation checker PASS; all 16 regression tests PASS. Both final-review findings reproduced RED then fixed GREEN. Original report/changelog content preserved; all 27 tracked firmware/config/diagnostic files unchanged.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-documentation.txt",
    "tests/docs/test_check_docs.py"
  ],
  "evidence_kind": "Observed local checker/test output and independent read-only review; failures and corrections retained in evidence/logs.",
  "environment": "Windows PowerShell, Python 3.12; no third-party checker dependencies.",
  "limitations": "CI configuration added but hosted runner not executed. Does not prove physical behavior, evidence authenticity or append-only history. Current firmware host suite remains BLOCKED (VER-0002)."
}
```


```json
{
  "record_type": "test",
  "id": "VER-0006",
  "date": "2026-11-01T00:36:00+03:00",
  "firmware_version": "v0.1 runtime",
  "commit": "UNCOMMITTED working tree based on ccbd0e6",
  "verification_type": "BUILD",
  "scope": "Main firmware target build with Wi-Fi AP connection and visible virtual keyboard",
  "hardware": "Target lilygo-t-display-s3 (ESP32-S3R8, 16 MB flash, 8 MB PSRAM)",
  "wiring": "Connected via USB CDC on COM3",
  "procedure": "pio run -e lilygo-t-display-s3 && pio run -e lilygo-t-display-s3 -t upload --upload-port COM3",
  "expected": "Compile and flash firmware binary with new AP selection, virtual keyboard, and connection state machine.",
  "actual": "Target build SUCCESS (18.90s, RAM 45976 bytes, Flash 741117 bytes); flash upload to COM3 at 460800 baud SUCCESS (18.57s).",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-target-build.txt"
  ],
  "evidence_kind": "PlatformIO build and esptool flash upload terminal output",
  "environment": "Windows 11, PlatformIO Core 6.2.0, espressif32@6.5.0",
  "limitations": "Physical touch keyboard responsiveness and on-air AP association depend on physical device interaction."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0007",
  "date": "2026-11-01T02:02:00+03:00",
  "firmware_version": "v0.1 runtime",
  "commit": "UNCOMMITTED working tree based on ccbd0e6",
  "verification_type": "BUILD",
  "scope": "Main firmware target build with WiFiApp container, persistent saved networks, and home card overflow prevention",
  "hardware": "Target lilygo-t-display-s3 (ESP32-S3R8, 16 MB flash, 8 MB PSRAM)",
  "wiring": "Connected via USB CDC on COM3",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Compile firmware binary with WiFiApp, Preferences NVS storage, and auto-connect.",
  "actual": "Target build SUCCESS (22.44s, RAM 46672 bytes, Flash 757533 bytes).",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-target-build.txt"
  ],
  "evidence_kind": "PlatformIO build terminal output",
  "environment": "Windows 11, PlatformIO Core 6.2.0, espressif32@6.5.0",
  "limitations": "Physical auto-connect on boot and touch responsiveness require on-board testing."
}
```
