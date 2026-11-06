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

```json
{
  "record_type": "test",
  "id": "VER-0008",
  "date": "2026-11-01T02:15:00+03:00",
  "firmware_version": "v0.2 runtime",
  "commit": "UNCOMMITTED working tree based on ccbd0e6",
  "verification_type": "BUILD",
  "scope": "Main firmware target build with runtime v0.2 bump and flicker-free differential rendering",
  "hardware": "Target lilygo-t-display-s3 (ESP32-S3R8, 16 MB flash, 8 MB PSRAM)",
  "wiring": "Connected via USB CDC on COM3",
  "procedure": "pio run -e lilygo-t-display-s3 && pio run -e lilygo-t-display-s3 -t upload --upload-port COM3",
  "expected": "Compile and flash firmware binary with v0.2 versioning and differential rendering.",
  "actual": "Target build SUCCESS (7.91s, RAM 46688 bytes, Flash 757717 bytes); flash upload to COM3 at 460800 baud SUCCESS (19.42s).",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-target-build.txt"
  ],
  "evidence_kind": "PlatformIO build and flash terminal output",
  "environment": "Windows 11, PlatformIO Core 6.2.0, espressif32@6.5.0",
  "limitations": "Physical verification depends on user inspection of display stability."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0009",
  "date": "2026-11-01T02:40:00+03:00",
  "firmware_version": "v0.3 runtime",
  "commit": "UNCOMMITTED working tree based on ccbd0e6",
  "verification_type": "BUILD",
  "scope": "Main firmware target build with FEAT-0012: LEDC PWM brightness control, inactivity sleep with touch/button wake, and PIN code lock screen",
  "hardware": "Target lilygo-t-display-s3 (ESP32-S3R8, 16 MB flash, 8 MB PSRAM)",
  "wiring": "Connected via USB CDC on COM3",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Compile firmware binary with DisplayManager PWM/sleep, SecurityService PIN lock, SettingsApp controls, and v0.3 versioning.",
  "actual": "Target build SUCCESS (12.66s, RAM 46816 / 327680 bytes [14.3%], Flash 770661 / 6553600 bytes [11.8%]).",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-target-build.txt"
  ],
  "evidence_kind": "PlatformIO build terminal output",
  "environment": "Windows 11, PlatformIO Core 6.2.0, espressif32@6.5.0",
  "limitations": "Physical touch wakeup and PWM brightness dimming observation require on-device validation."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0010",
  "date": "2026-11-01T03:00:00+03:00",
  "firmware_version": "v0.3 runtime",
  "commit": "UNCOMMITTED working tree based on ccbd0e6",
  "verification_type": "BUILD",
  "scope": "Main firmware target build and flash with FEAT-0013: Essential Settings v0.1, AW9364 pulse dimming on GPIO38, SettingsService, and MemoryProfiler",
  "hardware": "Target lilygo-t-display-s3 (ESP32-S3R8, 16 MB flash, 8 MB PSRAM)",
  "wiring": "Connected via USB CDC on COM3",
  "procedure": "pio run -e lilygo-t-display-s3 && pio run -e lilygo-t-display-s3 -t upload --upload-port COM3",
  "expected": "Compile and flash firmware binary with AW9364 pulse dimming, SettingsService, MemoryProfiler, and Essential Settings v0.1.",
  "actual": "Target build SUCCESS (16.62s, RAM 46752 / 327680 bytes [14.3%], Flash 766705 / 6553600 bytes [11.7%]); flash upload to COM3 SUCCESS (17.58s); verified boot log over COM3.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-target-build.txt"
  ],
  "evidence_kind": "PlatformIO build and flash terminal output + serial boot log",
  "environment": "Windows 11, PlatformIO Core 6.2.0, espressif32@6.5.0",
  "limitations": "Physical observation of display dimming steps (25%, 50%, 75%, 100%) and touch responsiveness on the glass."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0011",
  "date": "2026-11-01T03:22:00+03:00",
  "firmware_version": "v0.3 runtime",
  "commit": "UNCOMMITTED working tree based on ccbd0e6",
  "verification_type": "BUILD",
  "scope": "Main firmware target build with FEAT-0014: Hardware 2FA Authenticator, CryptoService, TotpService, TimeService, UsbHidService, and TotpApp",
  "hardware": "Target lilygo-t-display-s3 (ESP32-S3R8, 16 MB flash, 8 MB PSRAM)",
  "wiring": "Connected via USB CDC on COM3",
  "procedure": "pio run -e lilygo-t-display-s3 -t upload --upload-port COM3",
  "expected": "Clean compilation, linking, and flashing of single-token 2FA Authenticator with compact typography and flicker-free differential renderer.",
  "actual": "Target build and upload SUCCESS (25.02s, RAM 61152 / 327680 bytes [18.7%], Flash 838065 / 6553600 bytes [12.8%]). Flashed 838432 bytes in 8.2s (effective 814.4 kbit/s). Verified hash and hard reset via RTS pin.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-target-build.txt"
  ],
  "evidence_kind": "PlatformIO build terminal output",
  "environment": "Windows 11, PlatformIO Core 6.2.0, espressif32@6.5.0",
  "limitations": "Physical observation of touch on screen and actual keystroke injection into host computer via USB."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0012",
  "date": "2026-11-01T03:51:00+03:00",
  "firmware_version": "v0.3 runtime",
  "commit": "UNCOMMITTED working tree based on ccbd0e6",
  "verification_type": "BUILD",
  "scope": "Main firmware target build and flash with FEAT-0014 UI refinement and FEAT-0013 Security/PIN configuration subviews",
  "hardware": "Target lilygo-t-display-s3 (ESP32-S3R8, 16 MB flash, 8 MB PSRAM)",
  "wiring": "Connected via USB CDC on COM3",
  "procedure": "pio run -e lilygo-t-display-s3 && pio run -e lilygo-t-display-s3 -t upload --upload-port COM3",
  "expected": "Compile, link, and upload firmware with 46px navigation chevrons, interactive PIN setup in Settings, and hardened lock screen.",
  "actual": "Build SUCCESS (12.88s, RAM 61168 / 327680 bytes [18.7%], Flash 841353 / 6553600 bytes [12.8%]); upload to COM3 SUCCESS (24.29s). Flashed 841712 bytes (545626 compressed) at 0x00010000 in 8.2s (819.3 kbit/s). Verified hash, hard reset via RTS.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-target-build.txt"
  ],
  "evidence_kind": "PlatformIO build and flash terminal output",
  "environment": "Windows 11, PlatformIO Core 6.2.0, espressif32@6.5.0",
  "limitations": "Physical observation of touch interactions on the glass."
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
  "expected": "Record real result at the indicated verification level",
  "evidence_kind": "Captured command output or explicitly labeled attempt summary",
  "environment": "Windows; Python 3.14; PlatformIO 6.2.0; local Zig 0.16.0 for HOST",
  "limitations": "No physical/runtime resource acceptance from build or mocks",
  "id": "VER-0013",
  "verification_type": "BUILD",
  "scope": "Pre-change identified target baseline",
  "procedure": "pio run -e lilygo-t-display-s3",
  "result": "PASS",
  "actual": "Linked Flash 841377 B; static RAM 61168 B; actual image 841744 B; SHA in baseline.",
  "evidence": [
    "docs/evidence/2026-11-01-settings-before-build.txt"
  ]
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
  "expected": "Record real result at the indicated verification level",
  "evidence_kind": "Captured command output or explicitly labeled attempt summary",
  "environment": "Windows; Python 3.14; PlatformIO 6.2.0; local Zig 0.16.0 for HOST",
  "limitations": "No physical/runtime resource acceptance from build or mocks",
  "id": "VER-0014",
  "verification_type": "HOST",
  "scope": "Initial host environment/stub/setup failures",
  "procedure": "bash availability check; Python host runner with Zig",
  "result": "FAIL",
  "actual": "WSL unavailable; stale mocks/duplicate variable and old test setup caused initial failures. Corrected and rerun in VER-0017.",
  "evidence": [
    "docs/evidence/2026-11-01-settings-failed-attempts.txt"
  ]
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
  "expected": "Record real result at the indicated verification level",
  "evidence_kind": "Captured command output or explicitly labeled attempt summary",
  "environment": "Windows; Python 3.14; PlatformIO 6.2.0; local Zig 0.16.0 for HOST",
  "limitations": "No physical/runtime resource acceptance from build or mocks",
  "id": "VER-0015",
  "verification_type": "DOCUMENTATION",
  "scope": "New tooling RED tests and encoding correction",
  "procedure": "unittest docs/resources before implementations/corrections",
  "result": "FAIL",
  "actual": "Missing checker module, resource required-doc tests and temporary Windows text encoding failures; fixed and rerun in VER-0018.",
  "evidence": [
    "docs/evidence/2026-11-01-settings-failed-attempts.txt"
  ]
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
  "expected": "Record real result at the indicated verification level",
  "evidence_kind": "Captured command output or explicitly labeled attempt summary",
  "environment": "Windows; Python 3.14; PlatformIO 6.2.0; local Zig 0.16.0 for HOST",
  "limitations": "No physical/runtime resource acceptance from build or mocks",
  "id": "VER-0016",
  "verification_type": "BUILD",
  "scope": "Resource post-image integration failure",
  "procedure": "pio run -e lilygo-t-display-s3 with new hook",
  "result": "FAIL",
  "actual": "Compilation succeeded; post-image check rejected OTA0 subtype 0x10 because parser assumed subtype0. Regression test and parser corrected before final builds.",
  "evidence": [
    "docs/evidence/2026-11-01-settings-failed-attempts.txt"
  ]
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
  "expected": "Record real result at the indicated verification level",
  "evidence_kind": "Captured command output or explicitly labeled attempt summary",
  "environment": "Windows; Python 3.14; PlatformIO 6.2.0; local Zig 0.16.0 for HOST",
  "limitations": "No physical/runtime resource acceptance from build or mocks",
  "id": "VER-0017",
  "verification_type": "HOST",
  "scope": "Full firmware mock suite and entry compile; default and profile+LEDC",
  "procedure": "CXX=.testbuild/toolchain/ziglang/zig.exe c++; python tools/run_host_tests.py; repeat with -DNIBRU_PROFILE=1 -DNIBRU_BACKLIGHT_LEDC=1",
  "result": "PASS",
  "actual": "Both modes PASS: persistent restore/validation/migration/failed loads+writes/scoped reset; physical navigation/cancel/reboot; held wake/contact; raw double-tap confirmation; timeout wrap; 20 lifecycle cycles.",
  "evidence": [
    "docs/evidence/2026-11-01-settings-host.txt",
    "docs/evidence/2026-11-01-settings-host-ledc-profile.txt"
  ]
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
  "expected": "Record real result at the indicated verification level",
  "evidence_kind": "Captured command output or explicitly labeled attempt summary",
  "environment": "Windows; Python 3.14; PlatformIO 6.2.0; local Zig 0.16.0 for HOST",
  "limitations": "No physical/runtime resource acceptance from build or mocks",
  "id": "VER-0018",
  "verification_type": "DOCUMENTATION",
  "scope": "Documentation and resource artifact unit tests",
  "procedure": "python tools/check_docs.py; python -m unittest discover -s tests/docs -v; python -m unittest discover -s tests/resources -v",
  "result": "PASS",
  "actual": "Final checker and 17 documentation mutation tests; six resource unit tests. Software-only; evidence files record exact outcomes.",
  "evidence": [
    "docs/evidence/2026-11-01-settings-doc-tests.txt",
    "docs/evidence/2026-11-01-settings-resource-tests.txt"
  ]
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
  "expected": "Record real result at the indicated verification level",
  "evidence_kind": "Captured command output or explicitly labeled attempt summary",
  "environment": "Windows; Python 3.14; PlatformIO 6.2.0; local Zig 0.16.0 for HOST",
  "limitations": "No physical/runtime resource acceptance from build or mocks",
  "id": "VER-0019",
  "verification_type": "BUILD",
  "scope": "Default, profile and LEDC real target builds with partition checks",
  "procedure": "pio run -e lilygo-t-display-s3 -e lilygo-t-display-s3-profile -e lilygo-t-display-s3-ledc",
  "result": "PASS",
  "actual": "All three SUCCESS. Default linked Flash842461/static RAM61200/image842832 B; profile image843408 B; LEDC image848592 B; all fit 6553600 B app0.",
  "evidence": [
    "docs/evidence/2026-11-01-settings-target-builds.txt",
    "docs/evidence/2026-11-01-settings-artifact-reports.txt"
  ]
}
```

```json
{
  "record_type": "test",
  "id": "VER-0020",
  "date": "2026-11-01",
  "firmware_version": "v0.3",
  "commit": "8239bb6 + uncommitted final contact regression fix",
  "verification_type": "BUILD",
  "scope": "Final three target builds after rejecting delayed reports from early second touch contact",
  "hardware": "Compile target LILYGO T-Display-S3 Touch Glass; no physical device test",
  "wiring": "Unchanged; no upload",
  "procedure": "pio run -e lilygo-t-display-s3 -e lilygo-t-display-s3-profile -e lilygo-t-display-s3-ledc",
  "expected": "All target images fit generated app0, no provisional 5% growth warning",
  "actual": "All three SUCCESS. Default linked Flash842481/static RAM61200/image842848 B; profile linked843061/image843440 B; LEDC linked848241/static RAM61272/image848608 B. Default growth1104 B (+0.1312%) vs measured before. VER-0019 remains the earlier successful checkpoint with its original capture restored; this record supersedes its current-image metrics.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-settings-target-builds-final.txt",
    "docs/evidence/2026-11-01-settings-artifact-final.txt"
  ],
  "evidence_kind": "Actual PlatformIO command output and generated partition artifact reports",
  "environment": "Windows, PlatformIO6.2.0, espressif32@6.5.0/core2.0.14",
  "limitations": "Runtime heap, stack, latency, brightness and power remain NOT MEASURED; HWT-0008 NOT RUN"
}
```

**Correction to VER-0020 (2026-11-01):** The profile image byte count in the actual field was transcribed as 843440; the generated artifact and captured build show **843424 bytes**, leaving **5710176 bytes** in app0. Linked program843061 B is unchanged. The resource report uses the actual artifact value; this corrects transcription only and adds no new physical evidence.

```json
{
  "record_type": "test",
  "id": "VER-0021",
  "date": "2026-11-01",
  "firmware_version": "v0.3",
  "commit": "8239bb6 + uncommitted Settings changes",
  "verification_type": "BUILD",
  "scope": "Fresh default build and artifact-capacity check immediately before authorized upload",
  "hardware": "Target LILYGO T-Display-S3 Touch Glass; build is software evidence",
  "wiring": "No source/pin/partition change",
  "procedure": "pio run -e lilygo-t-display-s3; python tools/check_resources.py",
  "expected": "Current default builds and matches previously reviewed image; fits app0",
  "actual": "Build SUCCESS7.118s; linked Flash842481 B, static RAM61200 B, image842848 B; app06553600 B, remainder5710752 B, no growth warning. SHA-256 f170c68ce03ee3e8967f648598942c5c162067d69e454060e7a384e08a04fb9b.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-settings-upload-preflight.txt",
    "docs/evidence/2026-11-01-settings-artifact-final.txt"
  ],
  "evidence_kind": "Fresh target build output and unchanged generated image hash/capacity",
  "environment": "Windows/PlatformIO6.2.0/espressif32@6.5.0/Arduino2.0.14",
  "limitations": "Compilation/static usage only; physical upload evidence is HWT-0009; runtime budgets remain unmeasured."
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
  "id": "VER-0022",
  "verification_type": "HOST",
  "scope": "RED regressions for user-reported touch repetition/flicker and review edge cases",
  "procedure": "python tools/run_host_tests.py against pre-fix variants before each correction",
  "actual": "Five failures reproduced: NONE/down prematurely taps; repeated Settings full clear; overlay restoration skipped by cached drawing; late delivered confirmation contact begun too early; coalesced/missed Up prevents next contact. Each received a focused fix before VER-0023.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-01-touch-red-tests.txt"
  ],
  "evidence_kind": "Captured assertion failure lines from actual native mock executions"
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
  "id": "VER-0023",
  "verification_type": "HOST",
  "scope": "Final full native suite including input/drawing/restoration/contact-time regressions",
  "procedure": "CXX=.testbuild/toolchain/ziglang/zig.exe c++; python tools/run_host_tests.py; repeat -DNIBRU_PROFILE=1 -DNIBRU_BACKLIGHT_LEDC=1",
  "actual": "Both modes PASS, actual entry unit compile included. Tests exercise repeated click/long reports, release-only gesture completion after observed Down, new-Down recovery, swipe mapping, changed-value incremental render, no-op drawing, lock-overlay restoration, raw driver-to-Settings deferred-contact confirmation, existing navigation/resources/NVS and20 lifecycle cycles.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-touch-host.txt",
    "docs/evidence/2026-11-01-touch-host-alt.txt"
  ],
  "evidence_kind": "Actual native mock test output; no physical measurements"
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
  "id": "VER-0024",
  "verification_type": "BUILD",
  "scope": "Three final target builds after touch and Settings drawing repair",
  "procedure": "pio run -e lilygo-t-display-s3 -e lilygo-t-display-s3-profile -e lilygo-t-display-s3-ledc; generated artifact capacity checks",
  "actual": "All3 SUCCESS. Default linked Flash843521 B, static RAM61240 B, firmware843888 B; profile image844448 B; LEDC image849664 B. All fit app0=6553600 B. Default +1040 B image and +40 B static RAM relative to prior HWT-0009 image.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-touch-builds.txt",
    "docs/evidence/2026-11-01-touch-artifacts.txt"
  ],
  "evidence_kind": "Captured PlatformIO output and generated artifact reports"
}
```
```json
{
  "record_type": "test",
  "id": "VER-0025",
  "date": "2026-11-01",
  "firmware_version": "v0.3",
  "commit": "8239bb6 + uncommitted Desktop Simulator",
  "verification_type": "HOST",
  "scope": "Automated Desktop Simulator test suite and headless smoke execution",
  "hardware": "Windows x64 Host Target; simulated 320x170 display and touch",
  "wiring": "Not applicable (Host Simulation)",
  "procedure": "cmake -B build -G Ninja; cmake --build build; .\\build\\nibru_tests.exe; .\\build\\nibru_simulator.exe --headless --test-frames=10",
  "expected": "All core lifecycle, UI transform, Wi-Fi fault injection, settings persistence, and 25-cycle stress tests PASS; simulator boots and shuts down cleanly",
  "actual": "Both targets built cleanly. nibru_tests passed all assertions across 5 test suites. nibru_simulator executed 10 headless frames and terminated cleanly.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-simulator-tests.txt"
  ],
  "evidence_kind": "Captured stdout from native build and test executable runs",
  "environment": "Windows 11 / MSVC 19.44 / CMake 3.31.6 / Ninja / SDL2 2.30.10",
  "limitations": "Host-only simulation; does not measure physical ESP32 SRAM, PSRAM, AW9364 pulse backlight, or physical radio reception."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0026",
  "date": "2026-11-01",
  "firmware_version": "v0.3",
  "commit": "8239bb6 + uncommitted Desktop Simulator",
  "verification_type": "BUILD",
  "scope": "ESP32-S3 PlatformIO regression build after desktop simulator integration",
  "hardware": "Target LILYGO T-Display-S3 Touch Glass (software compile test)",
  "wiring": "Standard baseline: SDA18, SCL17, RST21, INT16, LCD parallel pins",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Build succeeds; no desktop files or SDL2 dependencies leak into embedded build; image fits within app0 partition",
  "actual": "Build SUCCESS in 7.01s. RAM used: 61,240 bytes (18.7%). Flash used: 843,521 bytes (12.9% of 6,553,600 bytes). Zero compilation warnings.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-simulator-pio-build.txt"
  ],
  "evidence_kind": "Captured PlatformIO build report and artifact sizing",
  "environment": "Windows 11 / PlatformIO / espressif32@6.5.0 / Arduino 2.0.14",
  "limitations": "Compilation only; does not upload to physical hardware."
}
```


```json
{
  "record_type": "test",
  "id": "VER-0027",
  "date": "2026-11-01",
  "firmware_version": "v0.3",
  "commit": "8239bb6 + modular refactor",
  "verification_type": "HOST",
  "scope": "Automated desktop simulator suite, host Zig smoke tests, and doc mutation tests after modular refactoring",
  "hardware": "Desktop SDL2 simulation & Zig host mock",
  "wiring": "Simulated CST816S (SDA18/SCL17), BOOT/USER buttons, and ST7789 display",
  "procedure": "tools\\run_desktop_tests.bat; $env:CXX='.testbuild/toolchain/ziglang/zig.exe c++'; python tools/run_host_tests.py; python -m unittest discover -s tests/docs -v",
  "expected": "All core lifecycle, UI transforms, Wi-Fi simulation, settings persistence, modular HAL contracts, and 25 lifecycle cycles pass with zero errors",
  "actual": "All desktop automated tests PASS. Host Zig smoke tests PASS. 17 doc mutation tests PASS. 6 resource tests PASS.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-architecture-refactor-tests.txt"
  ],
  "evidence_kind": "Automated test execution logs and unit test reports",
  "environment": "Windows 11 / MSVC 2022 / Ninja / Zig 0.16.0 / Python 3.14",
  "limitations": "Host and simulator execution only; does not prove physical RF Wi-Fi reception or AW9364 pulse modulation."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0028",
  "date": "2026-11-01",
  "firmware_version": "v0.3",
  "commit": "8239bb6 + modular refactor",
  "verification_type": "BUILD",
  "scope": "ESP32-S3 PlatformIO multi-target builds (default, profile, ledc) after modular refactoring",
  "hardware": "Target LILYGO T-Display-S3 Touch Glass (software compile test)",
  "wiring": "Standard baseline: SDA18, SCL17, RST21, INT16, LCD parallel pins",
  "procedure": "pio run -e lilygo-t-display-s3 -e lilygo-t-display-s3-profile -e lilygo-t-display-s3-ledc; python tools/check_resources.py",
  "expected": "All 3 PlatformIO environments compile successfully; binary size fits within app0 partition; flash growth < 5%",
  "actual": "All 3 environments SUCCESS. Default env Flash: 843,565 B (+44 B, +0.005%). RAM: 61,240 B (unchanged). Binary: 843,936 B (+48 B). Resource check PASS.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-architecture-builds.txt"
  ],
  "evidence_kind": "PlatformIO build logs and check_resources.py artifact report",
  "environment": "Windows 11 / PlatformIO / espressif32@6.5.0 / Arduino 2.0.14",
  "limitations": "Compilation and ELF analysis only; does not upload to physical hardware."
}
```


```json
{
  "record_type": "test",
  "id": "VER-0029",
  "date": "2026-11-01",
  "firmware_version": "v0.3 authenticator phase one",
  "commit": "9513925 + uncommitted changes",
  "verification_type": "BUILD",
  "scope": "Pre-change pio run -e lilygo-t-display-s3 at clean9513925",
  "hardware": "Windows host; ESP32-S3 compile target only",
  "wiring": "No wiring change or physical session",
  "procedure": "Pre-change pio run -e lilygo-t-display-s3 at clean9513925",
  "expected": "Behavior matches the phase-one design and applicable test contract",
  "actual": "PASS: firmware843920 B; linked Flash843557 B; static RAM61240 B; app0 6553600 B; SHA256 806e275182a60f0c1b77c010fbe96dcc603c6e9101d49850c02467f62875f15d.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-authenticator-baseline-build.txt"
  ],
  "evidence_kind": "Captured local command output",
  "environment": "Windows / Python 3.12.6 / Zig0.15.2 / Arduino-ESP32 2.0.14 espressif32@6.5.0",
  "limitations": "Build only, no runtime observation."
}
```



```json
{
  "record_type": "test",
  "id": "VER-0030",
  "date": "2026-11-01",
  "firmware_version": "v0.3 authenticator phase one",
  "commit": "9513925 + uncommitted changes",
  "verification_type": "HOST",
  "scope": "Pre-change python tools/run_host_tests.py",
  "hardware": "Windows host; ESP32-S3 compile target only",
  "wiring": "No wiring change or physical session",
  "procedure": "Pre-change python tools/run_host_tests.py",
  "expected": "Behavior matches the phase-one design and applicable test contract",
  "actual": "Existing host/navigation/settings suite PASS before product changes.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-authenticator-baseline-host.txt"
  ],
  "evidence_kind": "Captured local command output",
  "environment": "Windows / Python 3.12.6 / Zig0.15.2 / Arduino-ESP32 2.0.14 espressif32@6.5.0",
  "limitations": "Original synthetic crypto backend; no device evidence."
}
```



```json
{
  "record_type": "test",
  "id": "VER-0031",
  "date": "2026-11-01",
  "firmware_version": "v0.3 authenticator phase one",
  "commit": "9513925 + uncommitted changes",
  "verification_type": "HOST",
  "scope": "Intentional RED regressions",
  "hardware": "Windows host; ESP32-S3 compile target only",
  "wiring": "No wiring change or physical session",
  "procedure": "Intentional RED regressions",
  "expected": "Behavior matches the phase-one design and applicable test contract",
  "actual": "Observed demo-account assertion and signed-shift panic; account-entry assertion; real OS write failure wrongly reports success; default PIN assertion; clock-loss cancellation assertion. Later corrected suites PASS.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-01-authenticator-attempts.txt"
  ],
  "evidence_kind": "Captured local command output",
  "environment": "Windows / Python 3.12.6 / Zig0.15.2 / Arduino-ESP32 2.0.14 espressif32@6.5.0",
  "limitations": "Intentional RED outcomes retained; not device failures."
}
```



```json
{
  "record_type": "test",
  "id": "VER-0032",
  "date": "2026-11-01",
  "firmware_version": "v0.3 authenticator phase one",
  "commit": "9513925 + uncommitted changes",
  "verification_type": "HOST",
  "scope": "Concurrent baseline/RED output collision",
  "hardware": "Windows host; ESP32-S3 compile target only",
  "wiring": "No wiring change or physical session",
  "procedure": "Concurrent baseline/RED output collision",
  "expected": "Behavior matches the phase-one design and applicable test contract",
  "actual": "Link could not replace nibru_smoke.exe (Permission denied). Overlap removed; serial rerun exposed actual RED behavior.",
  "result": "BLOCKED",
  "evidence": [
    "docs/evidence/2026-11-01-authenticator-attempts.txt"
  ],
  "evidence_kind": "Captured local command output",
  "environment": "Windows / Python 3.12.6 / Zig0.15.2 / Arduino-ESP32 2.0.14 espressif32@6.5.0",
  "limitations": "Tool orchestration collision, not a demonstrated firmware failure."
}
```



```json
{
  "record_type": "test",
  "id": "VER-0033",
  "date": "2026-11-01",
  "firmware_version": "v0.3 authenticator phase one",
  "commit": "9513925 + uncommitted changes",
  "verification_type": "HOST",
  "scope": "python tools/run_host_tests.py; repeat -DNIBRU_PROFILE=1 -DNIBRU_BACKLIGHT_LEDC=1",
  "hardware": "Windows host; ESP32-S3 compile target only",
  "wiring": "No wiring change or physical session",
  "procedure": "python tools/run_host_tests.py; repeat -DNIBRU_PROFILE=1 -DNIBRU_BACKLIGHT_LEDC=1",
  "expected": "Behavior matches the phase-one design and applicable test contract",
  "actual": "Both suites PASS: real CNG RFC6238 SHA1 vectors, authenticated reopen/scalar-U8 migration/key-tag corruption, bounds, failed writes, PIN guard, entry/retry/confirmed deletion, cleanup, time/USB state and cancellation.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-authenticator-host.txt",
    "docs/evidence/2026-11-01-authenticator-host-alternate.txt"
  ],
  "evidence_kind": "Captured local command output",
  "environment": "Windows / Python 3.12.6 / Zig0.15.2 / Arduino-ESP32 2.0.14 espressif32@6.5.0",
  "limitations": "CNG is not ESP32 mbedTLS/entropy/latency evidence. USB transport is a controlled double."
}
```



```json
{
  "record_type": "test",
  "id": "VER-0034",
  "date": "2026-11-01",
  "firmware_version": "v0.3 authenticator phase one",
  "commit": "9513925 + uncommitted changes",
  "verification_type": "HOST",
  "scope": "tools/run_desktop_tests.bat --smoke via Python/Zig/SDL2 fallback",
  "hardware": "Windows host; ESP32-S3 compile target only",
  "wiring": "No wiring change or physical session",
  "procedure": "tools/run_desktop_tests.bat --smoke via Python/Zig/SDL2 fallback",
  "expected": "Behavior matches the phase-one design and applicable test contract",
  "actual": "Shared entry/save/reopen/cancel and >=20 cycles PASS; actual disk reopen, OS write-failure rollback, corrupt-file preservation, existing desktop suite and isolated main-loop smoke PASS. Fixture framebuffer visually inspected.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-authenticator-desktop.txt"
  ],
  "evidence_kind": "Captured local command output",
  "environment": "Windows / Python 3.12.6 / Zig0.15.2 / Arduino-ESP32 2.0.14 espressif32@6.5.0",
  "limitations": "SDL2 2.30.10/CNG host only. Existing unused-constant and pragma warnings remain; no physical leak/UI/RF claim."
}
```



```json
{
  "record_type": "test",
  "id": "VER-0035",
  "date": "2026-11-01",
  "firmware_version": "v0.3 authenticator phase one",
  "commit": "9513925 + uncommitted changes",
  "verification_type": "BUILD",
  "scope": "pio run: default, hid, profile and ledc environments",
  "hardware": "Windows host; ESP32-S3 compile target only",
  "wiring": "No wiring change or physical session",
  "procedure": "pio run: default, hid, profile and ledc environments",
  "expected": "Behavior matches the phase-one design and applicable test contract",
  "actual": "All four builds SUCCESS. Default822912 B; HID858256 B; profile823520 B; LEDC828816 B. Native HID symbols present in opt-in ELF.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-authenticator-builds.txt",
    "docs/evidence/2026-11-01-authenticator-resources.json"
  ],
  "evidence_kind": "Captured local command output",
  "environment": "Windows / Python 3.12.6 / Zig0.15.2 / Arduino-ESP32 2.0.14 espressif32@6.5.0",
  "limitations": "Existing TFT parallel-DMA/TOUCH_CS warnings remain. No physical HID/LEDC acceptance."
}
```



```json
{
  "record_type": "test",
  "id": "VER-0036",
  "date": "2026-11-01",
  "firmware_version": "v0.3 authenticator phase one",
  "commit": "9513925 + uncommitted changes",
  "verification_type": "STATIC",
  "scope": "python tools/check_resources.py for all final images and generated partition tables",
  "hardware": "Windows host; ESP32-S3 compile target only",
  "wiring": "No wiring change or physical session",
  "procedure": "python tools/check_resources.py for all final images and generated partition tables",
  "expected": "Behavior matches the phase-one design and applicable test contract",
  "actual": "All images fit app0 6553600 B; no growth warnings; exact hashes and byte counts retained.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-authenticator-resources.json"
  ],
  "evidence_kind": "Captured local command output",
  "environment": "Windows / Python 3.12.6 / Zig0.15.2 / Arduino-ESP32 2.0.14 espressif32@6.5.0",
  "limitations": "Static RAM remainder cannot establish >=150 KiB free internal heap, PSRAM, stack, latency or power."
}
```



```json
{
  "record_type": "test",
  "id": "VER-0037",
  "date": "2026-11-01",
  "firmware_version": "v0.3 authenticator phase one",
  "commit": "9513925 + uncommitted changes",
  "verification_type": "BUILD",
  "scope": "Intermediate optional HID build",
  "hardware": "Windows host; ESP32-S3 compile target only",
  "wiring": "No wiring change or physical session",
  "procedure": "Intermediate optional HID build",
  "expected": "Behavior matches the phase-one design and applicable test contract",
  "actual": "FAILED: missing tud_mounted/tud_suspended declarations and C++11 aggregate construction. Added tusb.h and explicit constructor; all final builds PASS.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-01-authenticator-attempts.txt"
  ],
  "evidence_kind": "Captured local command output",
  "environment": "Windows / Python 3.12.6 / Zig0.15.2 / Arduino-ESP32 2.0.14 espressif32@6.5.0",
  "limitations": "Intermediate source only; no upload."
}
```



```json
{
  "record_type": "test",
  "id": "VER-0038",
  "date": "2026-11-01",
  "firmware_version": "v0.3 authenticator phase one",
  "commit": "9513925 + uncommitted changes",
  "verification_type": "HOST",
  "scope": "Intermediate test-harness compilation",
  "hardware": "Windows host; ESP32-S3 compile target only",
  "wiring": "No wiring change or physical session",
  "procedure": "Intermediate test-harness compilation",
  "expected": "Behavior matches the phase-one design and applicable test contract",
  "actual": "FAILED setup: missing Arduino include for millis and missing desktop Preferences include. Corrected; final host/desktop suites PASS.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-01-authenticator-attempts.txt"
  ],
  "evidence_kind": "Captured local command output",
  "environment": "Windows / Python 3.12.6 / Zig0.15.2 / Arduino-ESP32 2.0.14 espressif32@6.5.0",
  "limitations": "Setup errors are distinct from intentional behavioral RED."
}
```


```json
{
  "record_type": "test",
  "id": "VER-0039",
  "date": "2026-11-01",
  "firmware_version": "v0.3 phase one",
  "commit": "9513925 + uncommitted changes",
  "verification_type": "DOCUMENTATION",
  "scope": "python tools/check_docs.py and python -m unittest discover -s tests/docs -v",
  "hardware": "None",
  "wiring": "Not applicable",
  "procedure": "Run documentation consistency checker and full mutation suite after phase-one records",
  "expected": "Checker PASS and all17 tests pass",
  "actual": "Documentation checks PASS; 17 tests OK. Resource tooling unit suite separately passed6 tests; no physical evidence inferred.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-01-authenticator-doc-check.txt",
    "docs/evidence/2026-11-01-authenticator-doc-tests.txt",
    "docs/evidence/2026-11-01-authenticator-resource-tests.txt"
  ],
  "evidence_kind": "Captured command output",
  "environment": "Windows / Python3.12 / repository documentation tooling",
  "limitations": "Structural/source consistency only; not proof of physical outcomes or cryptographic certification."
}
```


```json
{
  "record_type": "test",
  "date": "2026-11-02",
  "firmware_version": "v0.3 authenticator phase one",
  "commit": "9513925 + uncommitted implementation",
  "environment": "Windows / PlatformIO espressif32@6.5.0 / Arduino-ESP32 2.0.14 / esptool4.5.1",
  "id": "VER-0040",
  "verification_type": "STATIC",
  "scope": "Upload preflight artifact identity and partition capacity",
  "hardware": "Host artifacts plus read-only device partition snapshot",
  "wiring": "Existing USB COM3",
  "procedure": "python tools/check_resources.py; SHA256 of firmware.bin; compare generated/device partitions.bin",
  "expected": "Identified phase-one image fits app0 and partition layout matches device",
  "actual": "PASS822912 B in app0 6553600 B, remaining5730688 B, no growth warning; image SHA256 00e74d8689773ea90efc9cfb707fa48aeb593c2cc10d2b8d5edfdb5b8cd0ea8b; device/generated table exact match.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-02-authenticator-preflight.txt",
    "docs/evidence/2026-11-02-authenticator-upload-com3.txt"
  ],
  "evidence_kind": "Captured artifact check and explicit byte/hash comparisons",
  "limitations": "No code or partition-layout change; no runtime memory/power or security certification claim."
}
```


```json
{
  "record_type": "test",
  "date": "2026-11-02",
  "firmware_version": "v0.3 easier account entry",
  "commit": "9513925 + uncommitted changes",
  "hardware": "Windows host; target artifacts only unless stated",
  "wiring": "No GPIO/wiring/framework/partition change",
  "environment": "Windows / Python3.12.6 / Zig0.15.2 / SDL2 2.30.10 / Node24.8.0 / espressif32@6.5.0 Arduino2.0.14",
  "id": "VER-0041",
  "verification_type": "HOST",
  "scope": "Retained behavioral RED and intermediate harness failures",
  "procedure": "New keyboard/import/helper/USB/expiry tests before fixes; later ownership coverage fixture setup",
  "expected": "New requested behavior and regression contracts",
  "actual": "FAIL before fixes: QWERTY mapping; missing import linkage; non-ASCII CSRF comparison/server close and missing relay function; busy-port retry and fields not cleared on close error; expired touch-before-tick candidate committed. Additional Wi-Fi test setup initially changed raw mock status without ticking WiFiService; corrected fixture through connect/tick. All final tests PASS separately.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-02-account-entry-attempts.txt"
  ],
  "evidence_kind": "Captured command output; browser/source-review summaries identified separately",
  "limitations": "Intentional RED, incomplete runner setup and fixture setup are distinct from final verified behavior; no physical attempt."
}
```


```json
{
  "record_type": "test",
  "date": "2026-11-02",
  "firmware_version": "v0.3 easier account entry",
  "commit": "9513925 + uncommitted changes",
  "hardware": "Windows host; target artifacts only unless stated",
  "wiring": "No GPIO/wiring/framework/partition change",
  "environment": "Windows / Python3.12.6 / Zig0.15.2 / SDL2 2.30.10 / Node24.8.0 / espressif32@6.5.0 Arduino2.0.14",
  "id": "VER-0042",
  "verification_type": "HOST",
  "scope": "Shared keyboard/import/service/ownership/lifecycle regressions",
  "procedure": "python tools/run_host_tests.py; repeat -DNIBRU_PROFILE=1 -DNIBRU_BACKLIGHT_LEDC=1 with Windows CNG",
  "expected": "All default/alternate assertions pass",
  "actual": "PASS both configurations: QWERTY/digits/Shift/symbols, actual app save/reopen, wrong key/tag/replay, bounds/overflow, expiry/rollover, no autosave, failed NVS retry, early and expired SAVE, Wi-Fi offline/conflict/lease/disconnect cleanup, sleep and >=20 import cycles.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-02-account-entry-host.txt",
    "docs/evidence/2026-11-02-account-entry-host-alternate.txt"
  ],
  "evidence_kind": "Captured command output; browser/source-review summaries identified separately",
  "limitations": "Injected I/O and mock NVS/Wi-Fi; not native USB/LAN/heap measurements."
}
```


```json
{
  "record_type": "test",
  "date": "2026-11-02",
  "firmware_version": "v0.3 easier account entry",
  "commit": "9513925 + uncommitted changes",
  "hardware": "Windows host; target artifacts only unless stated",
  "wiring": "No GPIO/wiring/framework/partition change",
  "environment": "Windows / Python3.12.6 / Zig0.15.2 / SDL2 2.30.10 / Node24.8.0 / espressif32@6.5.0 Arduino2.0.14",
  "id": "VER-0043",
  "verification_type": "HOST",
  "scope": "SDL desktop shared application/UI and simulator smoke",
  "procedure": "tools/run_desktop_tests.bat --smoke",
  "expected": "Automated shared tests and clean simulator shutdown",
  "actual": "PASS SDL2 suite including shared manual QWERTY and injected paired receive/review/SAVE; simulator smoke shut down cleanly. Real Windows CNG and isolated disk-fault tests retained.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-02-account-entry-desktop.txt"
  ],
  "evidence_kind": "Captured command output; browser/source-review summaries identified separately",
  "limitations": "HOST/SIMULATOR only; no physical peripherals/real network delivery."
}
```


```json
{
  "record_type": "test",
  "date": "2026-11-02",
  "firmware_version": "v0.3 easier account entry",
  "commit": "9513925 + uncommitted changes",
  "hardware": "Windows host; target artifacts only unless stated",
  "wiring": "No GPIO/wiring/framework/partition change",
  "environment": "Windows / Python3.12.6 / Zig0.15.2 / SDL2 2.30.10 / Node24.8.0 / espressif32@6.5.0 Arduino2.0.14",
  "id": "VER-0044",
  "verification_type": "HOST",
  "scope": "Local helper guards, browser crypto interoperability and rendered UI",
  "procedure": "Python tests/tools unittest; Node built-in tests of parser/WebCrypto/UI; local in-app browser inspection",
  "expected": "Four Python and five Node tests pass; UI shows USB and Wi-Fi fields",
  "actual": "PASS 4 Python and 5 Node tests: route/Host/Origin/CSRF/body/IP/envelope bounds, URI validation, public WebCrypto/CNG envelope equality, busy USB retry and field clearing on failing close. Arabic local page rendered; choosing Wi-Fi displayed device IPv4 field. No actual seed/pairing entered.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-02-account-entry-companion.txt",
    "docs/evidence/2026-11-02-account-entry-webcrypto.txt",
    "docs/evidence/2026-11-02-account-entry-browser.txt"
  ],
  "evidence_kind": "Captured command output; browser/source-review summaries identified separately",
  "limitations": "HTTP server and public fixtures tested; browser port selection/native Web Serial and actual UDP device delivery NOT RUN."
}
```


```json
{
  "record_type": "test",
  "date": "2026-11-02",
  "firmware_version": "v0.3 easier account entry",
  "commit": "9513925 + uncommitted changes",
  "hardware": "Windows host; target artifacts only unless stated",
  "wiring": "No GPIO/wiring/framework/partition change",
  "environment": "Windows / Python3.12.6 / Zig0.15.2 / SDL2 2.30.10 / Node24.8.0 / espressif32@6.5.0 Arduino2.0.14",
  "id": "VER-0045",
  "verification_type": "BUILD",
  "scope": "Four pinned ESP32 targets",
  "procedure": "pio run -e lilygo-t-display-s3 -e lilygo-t-display-s3-hid -e lilygo-t-display-s3-profile -e lilygo-t-display-s3-ledc",
  "expected": "All builds succeed without pin/framework/partition change",
  "actual": "PASS all4; binary/staticSRAM B: default831408/50056, HID866640/62184, profile831984/50064, LEDC837216/50128. Existing TFT warnings retained.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-02-account-entry-builds.txt"
  ],
  "evidence_kind": "Captured command output; browser/source-review summaries identified separately",
  "limitations": "Builds are not physical runtime, resource or HID validation."
}
```


```json
{
  "record_type": "test",
  "date": "2026-11-02",
  "firmware_version": "v0.3 easier account entry",
  "commit": "9513925 + uncommitted changes",
  "hardware": "Windows host; target artifacts only unless stated",
  "wiring": "No GPIO/wiring/framework/partition change",
  "environment": "Windows / Python3.12.6 / Zig0.15.2 / SDL2 2.30.10 / Node24.8.0 / espressif32@6.5.0 Arduino2.0.14",
  "id": "VER-0046",
  "verification_type": "STATIC",
  "scope": "Resource artifacts, partition limits and tooling regression",
  "procedure": "check_resources.inspect_image per generated image/table; six tests/resources unit tests",
  "expected": "Each image fits actual app0; report measured image/static deltas",
  "actual": "PASS all4 fit6553600 B app0; default+8496 B (+1.0324%) vs phase one; static+808 B. No >5% review warning. Resource tooling6 tests OK; accepted baseline unchanged.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-02-account-entry-resources.json",
    "docs/evidence/2026-11-02-account-entry-resource-tests.txt"
  ],
  "evidence_kind": "Captured command output; browser/source-review summaries identified separately",
  "limitations": "All runtime free/min/largest heap, PSRAM, stacks, timing and power NOT MEASURED."
}
```


```json
{
  "record_type": "test",
  "date": "2026-11-02",
  "firmware_version": "v0.3 easier account entry",
  "commit": "9513925 + uncommitted changes",
  "hardware": "Windows host; target artifacts only unless stated",
  "wiring": "No GPIO/wiring/framework/partition change",
  "environment": "Windows / Python3.12.6 / Zig0.15.2 / SDL2 2.30.10 / Node24.8.0 / espressif32@6.5.0 Arduino2.0.14",
  "id": "VER-0048",
  "verification_type": "STATIC",
  "scope": "Independent final focused source review",
  "procedure": "Fresh review of account-entry changes and regression fixes",
  "expected": "No remaining correctness/security blocker",
  "actual": "Three P2 findings fixed: USB handle after successful open, clear fields before failing close, deadline checked before touch SAVE. Final review found no remaining blocker and independently reran two UI tests:2 PASS.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-02-account-entry-review.txt"
  ],
  "evidence_kind": "Captured command output; browser/source-review summaries identified separately",
  "limitations": "Source review is not a security certification; reviewer performed no target/device checks."
}
```


```json
{
  "record_type": "test",
  "id": "VER-0047",
  "date": "2026-11-02",
  "firmware_version": "v0.3 easier account entry",
  "commit": "9513925 + uncommitted changes",
  "verification_type": "DOCUMENTATION",
  "scope": "Engineering memory consistency and documentation tooling regressions",
  "hardware": "None",
  "wiring": "Not applicable",
  "procedure": "python tools/check_docs.py; python -m unittest discover -s tests/docs -v; git diff --check",
  "expected": "Documentation consistency PASS;17 tests OK; no whitespace errors",
  "actual": "PASS documentation checker;17 documentation tests OK; diff check exit0 with only existing LF/CRLF conversion warnings.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-02-account-entry-doc-check.txt",
    "docs/evidence/2026-11-02-account-entry-doc-tests.txt"
  ],
  "evidence_kind": "Captured command output",
  "environment": "Windows / Python3.12.6 / repository documentation tooling",
  "limitations": "Structural consistency only, not proof of hardware or security acceptance. Final snapshot rechecked after upload records."
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
  "id": "VER-0049",
  "verification_type": "STATIC",
  "scope": "Current upload preflight identity/partition compatibility",
  "hardware": "Host artifacts and read-only board partition table",
  "procedure": "Read device0x8000..0x8bff; byte-compare generated partitions.bin; hash current firmware.bin; python tools/check_resources.py",
  "expected": "Same partition table and identified image within app0",
  "actual": "PASS device/generated tables identical SHA256 bd0f7954aca2ef7d925ee21aaa1f3dc8822d1d6ce5cbbd26a135e5886bfff6ce. Current image831408 B SHA256 2b43fd32b2d8fbac27263b9bd9226c22c497d331187b384397593e59bb4661f2; capacity PASS.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-02-account-entry-preflight.txt",
    "docs/evidence/2026-11-02-account-entry-resource-check.txt"
  ],
  "evidence_kind": "Sanitized captured serial/tool output plus byte/hash comparison",
  "limitations": "No runtime or account-data read; physical heap/stack/PSRAM/timing/power unmeasured."
}
```


```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.3 architecture audit",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "Windows / PlatformIO espressif32@6.5.0 / Arduino-ESP32 2.0.14 / esptool 4.5.1",
  "wiring": "Integrated ST7789 display, CST816S touch (18/17/21/16), BOOT (0), USER (14)",
  "id": "VER-0050",
  "verification_type": "BUILD",
  "scope": "Target build compilation and post-image resource check after modular architecture audit and pin centralisation",
  "hardware": "Host toolchain and firmware image compilation",
  "procedure": "pio run -e lilygo-t-display-s3 && python tools/check_resources.py",
  "expected": "Clean build producing firmware.bin within app0 partition (6,553,600 B) and resource checks passing",
  "actual": "PASS: Flash used 831,065 B (12.7%), RAM used 50,056 B (15.3%), binary 831,424 B; partition capacity PASS",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-arch-audit-pio-build.txt"
  ],
  "evidence_kind": "PlatformIO build and resource check tool output",
  "limitations": "Software compile and image check only; physical hardware upload and runtime observation unperformed"
}
```


```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.3 architecture audit",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "Windows / Python 3.14.15 standard library unittest",
  "wiring": "None (host documentation and metadata validation)",
  "id": "VER-0051",
  "verification_type": "DOCUMENTATION",
  "scope": "Automated repository-wide documentation, cross-link, schema, and mutation checks",
  "hardware": "None (host file inspection)",
  "procedure": "python tools/check_docs.py && python -m unittest discover -s tests/docs -v",
  "expected": "All documentation checks and all 17 mutation tests pass without errors",
  "actual": "PASS: tools/check_docs.py reported Documentation checks: PASS; 17 unit tests in tests/docs passed",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-arch-audit-docs-check.txt"
  ],
  "evidence_kind": "Captured terminal output from check_docs and unittest runner",
  "limitations": "Structural and link consistency checks only; does not validate physical electrical properties"
}
```


```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.3 architecture audit",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "Windows / MSVC 19.44.35222 x64 / SDL2 2.30.10 / Node v24.12.0",
  "wiring": "None (desktop host simulation and mock C++ tests)",
  "id": "VER-0052",
  "verification_type": "HOST",
  "scope": "Desktop simulator tests, host mock tests, and helper unit tests",
  "hardware": "None (desktop CPU emulation)",
  "procedure": "python tools/run_host_tests.py && tools/run_desktop_tests.bat && node --test tests/tools/account_setup.test.mjs tests/tools/account_setup_ui.test.mjs",
  "expected": "All host tests, 25 lifecycle iterations, WebCrypto envelope matches, and simulator pass",
  "actual": "PASS: host smoke tests passed, all desktop simulator tests passed with 0 compiler warnings, 5 node tests passed",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-arch-audit-host-tests.txt"
  ],
  "evidence_kind": "Captured output from host test executables and test runners",
  "limitations": "Host execution does not measure physical ESP32-S3 RF reception, AW9364 backlight pulses, or hardware touch response"
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.3 screen orientation",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "Windows / PlatformIO 6.1.18 / espressif32@6.5.0 / Arduino-ESP32 2.0.14",
  "wiring": "None (compile-time ELF/binary image analysis and memory sizing)",
  "id": "VER-0053",
  "verification_type": "BUILD",
  "scope": "Embedded build and resource budget verification for screen orientation feature",
  "hardware": "ESP32-S3R8 / 16MB Flash / 8MB PSRAM (compile-time target)",
  "procedure": "pio run -e lilygo-t-display-s3 && python tools/check_resources.py",
  "expected": "Build succeeds; firmware image <= 6,553,600 bytes (app0); flash growth < 5%; internal SRAM usage bounded",
  "actual": "PASS: Flash: 832,093 B (12.7% of 6.25 MB partition), RAM: 50,064 B (15.3%), binary: 832,464 B (-1.10% vs baseline, 0 warnings)",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-screen-orientation-build.txt"
  ],
  "evidence_kind": "Captured terminal output from PlatformIO build and check_resources.py",
  "limitations": "Compilation and link-time size analysis only; physical flashing not executed"
}
```


```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.3 screen orientation",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "Windows / MSVC 19.44.35222 x64 / SDL2 2.30.10 / Node v24.12.0",
  "wiring": "None (desktop host simulation and mock C++ tests)",
  "id": "VER-0054",
  "verification_type": "HOST",
  "scope": "Desktop simulator tests, host mock tests, and helper unit tests for screen rotation and CST816S touch mapping",
  "hardware": "None (desktop CPU emulation)",
  "procedure": "python tools/run_host_tests.py && cmd /c tools/run_desktop_tests.bat && node tests/tools/account_setup.test.mjs",
  "expected": "All host tests pass; touch coordinate inversion in rotation 3 verified (x=119, y=59 from rawX=59, rawY=200); swipe gesture inversion verified (SWIPE_UP -> SwipeRight); settings persistence for key 'rot' verified; 25 lifecycle iterations pass",
  "actual": "PASS: Host smoke tests pass, desktop simulator passes with 0 warnings, node tests pass",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-screen-orientation-host-tests.txt"
  ],
  "evidence_kind": "Captured output from host test executables and test runners",
  "limitations": "Host execution does not measure physical ESP32-S3 RF reception, AW9364 backlight pulses, or hardware touch response"
}
```


```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.3 screen orientation",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "Windows / Python 3.14.15 standard library unittest",
  "wiring": "None (host documentation and metadata validation)",
  "id": "VER-0055",
  "verification_type": "DOCUMENTATION",
  "scope": "Automated repository-wide documentation, cross-link, schema, and mutation checks",
  "hardware": "None (host file inspection)",
  "procedure": "python tools/check_docs.py && python -m unittest discover -s tests/docs -v",
  "expected": "All documentation checks and all 17 mutation tests pass without errors",
  "actual": "PASS: tools/check_docs.py reported Documentation checks: PASS; 17 unit tests in tests/docs passed",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-screen-orientation-docs-check.txt"
  ],
  "evidence_kind": "Captured terminal output from check_docs and unittest runner",
  "limitations": "Structural and link consistency checks only; does not validate physical electrical properties"
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.4",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "Windows / Python 3.14.15 / MSVC 19.44 / PlatformIO 6.1.18",
  "wiring": "None (version governance, automated documentation, and multi-target compilation check)",
  "id": "VER-0056",
  "verification_type": "DOCUMENTATION",
  "scope": "Verification of Mandatory Automatic Sequential Versioning Policy and v0.4 alignment across all sources of truth",
  "hardware": "None (multi-target build and doc verification)",
  "procedure": "python tools/check_docs.py && python -m unittest discover -s tests/docs -v && python tools/run_host_tests.py && cmd /c tools/run_desktop_tests.bat && pio run -e lilygo-t-display-s3 && python tools/check_resources.py",
  "expected": "All documentation checks pass; runtime version regex matches src/main.cpp; all host, desktop, and embedded builds pass with 0 errors",
  "actual": "PASS: check_docs reported Documentation checks: PASS; 17 unit tests passed; host and simulator tests passed; PlatformIO build succeeded (832,464 B binary, 0 warnings)",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-version-policy-check.txt"
  ],
  "evidence_kind": "Captured terminal output from check_docs, unittest, host tests, simulator, and pio build",
  "limitations": "Build, doc, and simulator checks only; physical runtime confirmation pending"
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.4.1",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "Windows / MSVC 19.44 / C++17 host runner",
  "wiring": "None (host software mock)",
  "id": "VER-0057",
  "verification_type": "HOST",
  "scope": "Host smoke test suite including pairing code touch/button broadcast and account import lifecycle",
  "hardware": "None (host execution)",
  "procedure": "python tools/run_host_tests.py",
  "expected": "PASS: authenticator lifecycle, bounded paired account import, and host navigation tests",
  "actual": "PASS: authenticator lifecycle, PASS: bounded paired account import, PASS: host navigation/resource/async scan/timeout/connect/crypto/settings smoke tests",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v041-host-tests.txt"
  ],
  "evidence_kind": "Captured terminal output from python tools/run_host_tests.py",
  "limitations": "Host test execution using MSVC; does not validate physical ESP32-S3 hardware pins"
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.4.1",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "Windows / CMake 3.31 / MSVC 19.44 / SDL2 desktop simulator",
  "wiring": "None (desktop SDL2 simulation)",
  "id": "VER-0058",
  "verification_type": "HOST",
  "scope": "Automated desktop test suite with 25 repeated lifecycle cycles and settings persistence",
  "hardware": "None (host simulated graphics and input)",
  "procedure": "cmd /c tools/run_desktop_tests.bat",
  "expected": "All desktop simulator tests pass with 0 warnings",
  "actual": "PASS: Core Lifecycle, UI & Transformation, Wi-Fi Simulation, Settings Persistence, HAL Contract, and 25 Repeated Lifecycle Entry/Exit Cycles all PASS",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v041-desktop-tests.txt"
  ],
  "evidence_kind": "Captured terminal output from tools/run_desktop_tests.bat",
  "limitations": "Desktop simulation only; does not validate real RF or physical TFT scan"
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.4.1",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "Node.js v24.12 / WebCrypto",
  "wiring": "None (companion web tool unit tests)",
  "id": "VER-0059",
  "verification_type": "HOST",
  "scope": "Account setup web tool verification (Base32 parsing, OTP auth URI, and AES-GCM WebCrypto)",
  "hardware": "None (Node.js runner)",
  "procedure": "node tests/tools/account_setup.test.mjs",
  "expected": "All 3 node test suites pass with 0 failures",
  "actual": "PASS: 3 tests pass (pasted key canonicalization, URI downgrade rejection, WebCrypto matching CNG vector)",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v041-node-tests.txt"
  ],
  "evidence_kind": "Captured terminal output from node tests/tools/account_setup.test.mjs",
  "limitations": "Node.js environment; Web Serial browser UI tested separately"
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.4.1",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "PlatformIO 6.1.18 / Arduino-ESP32 2.0.14 / xtensa-esp32s3-elf-gcc 8.4.0",
  "wiring": "LILYGO T-Display-S3 Touch Glass standard pinout",
  "id": "VER-0060",
  "verification_type": "BUILD",
  "scope": "PlatformIO embedded build and resource budget verification for lilygo-t-display-s3 target",
  "hardware": "ESP32-S3R8 (16MB Flash, 8MB PSRAM)",
  "procedure": "pio run -e lilygo-t-display-s3 && python tools/check_resources.py",
  "expected": "Build succeeds; Flash growth < 5%; free heap >= 150KB",
  "actual": "SUCCESS: RAM 50,064 B (15.3%), Flash 832,441 B (12.7%), Binary 832,800 B, growth -1.06% vs baseline, check_resources PASS",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v041-pio-build.txt"
  ],
  "evidence_kind": "PlatformIO build logs and check_resources JSON output",
  "limitations": "Compile-time validation and partition geometry check; physical flash verification logged in HWT-0021"
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.4.2",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "MSVC 2022 C++20 x64 / Windows 11",
  "wiring": "None (host unit test runner)",
  "id": "VER-0061",
  "verification_type": "HOST",
  "scope": "Host test suite for authenticator lifecycle, padded labels, and live timer updates",
  "hardware": "Host (MSVC 2022 C++20 x64)",
  "procedure": "python tools/run_host_tests.py",
  "expected": "All host tests pass without regressions across authenticator, settings, and import",
  "actual": "PASS: test_authenticator, test_account_import, test_settings all succeeded",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v042-host-tests.txt"
  ],
  "evidence_kind": "Host test runner output log",
  "limitations": "Host execution only; simulated display memory buffer"
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.4.2",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "MSVC 2022 C++20 x64 / SDL2 desktop harness",
  "wiring": "None (desktop simulation harness)",
  "id": "VER-0062",
  "verification_type": "HOST",
  "scope": "Desktop simulator comprehensive automated regression suite",
  "hardware": "Host (MSVC 2022 C++20 x64 / SDL2 desktop harness)",
  "procedure": "tools\\run_desktop_tests.bat",
  "expected": "Builds with 0 warnings, passes all 25 lifecycle tests, verifies flicker-free label rendering",
  "actual": "PASS: 0 warnings, 25/25 tests passed including repeated lifecycle and navigation",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v042-desktop-tests.txt"
  ],
  "evidence_kind": "Desktop simulator build and test run logs",
  "limitations": "Desktop host simulation; does not test physical ST7789 parallel bus timing"
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.4.2",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "PlatformIO 6.1.18 / Arduino-ESP32 2.0.14 / xtensa-esp32s3-elf-gcc 8.4.0",
  "wiring": "LILYGO T-Display-S3 Touch Glass standard pinout",
  "id": "VER-0063",
  "verification_type": "BUILD",
  "scope": "PlatformIO embedded build and resource budget verification for lilygo-t-display-s3 target",
  "hardware": "ESP32-S3R8 (16MB Flash, 8MB PSRAM)",
  "procedure": "pio run -e lilygo-t-display-s3 && python tools/check_resources.py",
  "expected": "Build succeeds; Flash growth < 5%; free heap >= 150KB",
  "actual": "SUCCESS: RAM 50,064 B (15.3%), Flash 832,693 B (12.7%), Binary 833,056 B, growth -1.03% vs baseline, check_resources PASS",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v042-pio-build.txt"
  ],
  "evidence_kind": "PlatformIO build logs and check_resources JSON output",
  "limitations": "Compile-time validation and partition geometry check; physical hardware flashing logged separately"
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.4.2",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "Python 3.14 / documentation check script",
  "wiring": "None",
  "id": "VER-0064",
  "verification_type": "DOCUMENTATION",
  "scope": "Documentation consistency and cross-reference validation for v0.4.2",
  "hardware": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "Documentation checks PASS with zero errors or warnings",
  "actual": "Documentation checks: PASS",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v042-docs-check.txt"
  ],
  "evidence_kind": "check_docs.py output log",
  "limitations": "Validates JSON structure, ID ordering, version synchronisation, and file paths only"
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.4.3",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "MSVC 2022 C++20 x64 / Windows 11",
  "wiring": "None (host unit test runner)",
  "id": "VER-0065",
  "verification_type": "HOST",
  "scope": "Host test suite for PIN state, security service, settings persistence, and lifecycle",
  "hardware": "Host (MSVC 2022 C++20 x64)",
  "procedure": "python tools/run_host_tests.py",
  "expected": "All host tests pass without regressions across authenticator, settings, and import",
  "actual": "PASS: test_authenticator, test_account_import, test_settings all succeeded",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v043-host-tests.txt"
  ],
  "evidence_kind": "Host test runner output log",
  "limitations": "Host execution only; simulated display memory buffer"
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.4.3",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "MSVC 2022 C++20 x64 / SDL2 desktop harness",
  "wiring": "None (desktop simulation harness)",
  "id": "VER-0066",
  "verification_type": "HOST",
  "scope": "Desktop simulator automated regression suite for flicker-free PIN and settings redraw",
  "hardware": "Host (MSVC 2022 C++20 x64 / SDL2 desktop harness)",
  "procedure": "tools\\\\run_desktop_tests.bat",
  "expected": "Builds with 0 warnings, passes all 25 lifecycle tests, verifies settings and security flows",
  "actual": "PASS: 0 warnings, 25/25 tests passed including settings persistence and security",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v043-desktop-tests.txt"
  ],
  "evidence_kind": "Desktop simulator build and test run logs",
  "limitations": "Desktop host simulation; does not test physical ST7789 parallel bus timing"
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.4.3",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "PlatformIO 6.1.18 / Arduino-ESP32 2.0.14 / xtensa-esp32s3-elf-gcc 8.4.0",
  "wiring": "LILYGO T-Display-S3 Touch Glass standard pinout",
  "id": "VER-0067",
  "verification_type": "BUILD",
  "scope": "PlatformIO embedded build and resource budget verification for lilygo-t-display-s3 target",
  "hardware": "ESP32-S3R8 (16MB Flash, 8MB PSRAM)",
  "procedure": "pio run -e lilygo-t-display-s3 && python tools/check_resources.py",
  "expected": "Build succeeds; Flash growth < 5%; free heap >= 150KB",
  "actual": "SUCCESS: RAM 50,088 B (15.3%), Flash 832,913 B (12.7%), Binary 833,280 B, growth -1.01% vs baseline, check_resources PASS",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v043-pio-build.txt"
  ],
  "evidence_kind": "PlatformIO build logs and check_resources JSON output",
  "limitations": "Compile-time validation and partition geometry check; physical hardware flashing logged separately"
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.4.3",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "Python 3.14 / documentation check script",
  "wiring": "None",
  "id": "VER-0068",
  "verification_type": "DOCUMENTATION",
  "scope": "Documentation consistency and cross-reference validation for v0.4.3",
  "hardware": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "Documentation checks PASS with zero errors or warnings",
  "actual": "Documentation checks: PASS",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v043-docs-check.txt"
  ],
  "evidence_kind": "check_docs.py output log",
  "limitations": "Validates JSON structure, ID ordering, version synchronisation, and file paths only"
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.5.0",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "MSVC 2022 C++20 x64 / Windows 11",
  "wiring": "None (host unit test runner)",
  "id": "VER-0069",
  "verification_type": "HOST",
  "scope": "Host test suite for Arabic localization, reshaper, language persistence in settings, and system lifecycle",
  "hardware": "Host (MSVC 2022 C++20 x64)",
  "procedure": "python tools/run_host_tests.py",
  "expected": "All host tests pass without regressions across authenticator, settings, and import",
  "actual": "PASS: test_authenticator, test_account_import, test_settings all succeeded",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v050-host-tests.txt"
  ],
  "evidence_kind": "Host test runner output log",
  "limitations": "Host execution only; simulated display memory buffer"
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.5.0",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "MSVC 2022 C++20 x64 / SDL2 desktop harness",
  "wiring": "None (desktop simulation harness)",
  "id": "VER-0070",
  "verification_type": "HOST",
  "scope": "Desktop simulator automated regression suite for bilingual localization, font rendering, and settings",
  "hardware": "Host (MSVC 2022 C++20 x64 / SDL2 desktop harness)",
  "procedure": "tools\\run_desktop_tests.bat",
  "expected": "Builds with 0 warnings, passes all 25 lifecycle tests, verifies settings persistence with language key",
  "actual": "PASS: 0 warnings, 25/25 tests passed including settings persistence and language switching",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v050-desktop-tests.txt"
  ],
  "evidence_kind": "Desktop simulator build and test run logs",
  "limitations": "Desktop host simulation; does not test physical ST7789 parallel bus timing"
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.5.0",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "PlatformIO 6.1.18 / Arduino-ESP32 2.0.14 / xtensa-esp32s3-elf-gcc 8.4.0",
  "wiring": "LILYGO T-Display-S3 Touch Glass standard pinout",
  "id": "VER-0071",
  "verification_type": "BUILD",
  "scope": "PlatformIO embedded build and resource budget verification for lilygo-t-display-s3 target",
  "hardware": "ESP32-S3R8 (16MB Flash, 8MB PSRAM)",
  "procedure": "pio run -e lilygo-t-display-s3 && python tools/check_resources.py",
  "expected": "Build succeeds; Flash growth < 5%; free heap >= 150KB",
  "actual": "SUCCESS: RAM 50,088 B (15.3%), Flash 843,501 B (12.9%), Binary 843,872 B, growth +0.25% vs baseline, check_resources PASS",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v050-pio-build.txt"
  ],
  "evidence_kind": "PlatformIO build logs and check_resources JSON output",
  "limitations": "Compile-time validation and partition geometry check; physical hardware flashing logged separately"
}
```

```json
{
  "record_type": "test",
  "date": "2026-11-03",
  "firmware_version": "v0.5.0",
  "commit": "8239bb6 + uncommitted changes",
  "environment": "Python 3.14 / documentation check script",
  "wiring": "None",
  "id": "VER-0072",
  "verification_type": "DOCUMENTATION",
  "scope": "Documentation consistency and cross-reference validation for v0.5.0",
  "hardware": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "Documentation checks PASS with zero errors or warnings",
  "actual": "Documentation checks: PASS",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v050-docs-check.txt"
  ],
  "evidence_kind": "check_docs.py output log",
  "limitations": "Validates JSON structure, ID ordering, version synchronisation, and file paths only"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0073",
  "date": "2026-11-03",
  "firmware_version": "v0.5.1",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Host unit tests for v0.5.1 Arabic localization",
  "hardware": "Host x64 architecture",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v051-host-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to specific test harness execution"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0074",
  "date": "2026-11-03",
  "firmware_version": "v0.5.1",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Desktop simulator tests for v0.5.1",
  "hardware": "Host x64 architecture",
  "wiring": "None",
  "procedure": "tools/run_desktop_tests.bat",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v051-desktop-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to specific test harness execution"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0075",
  "date": "2026-11-03",
  "firmware_version": "v0.5.1",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "PlatformIO build for v0.5.1",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v051-pio-build.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to specific test harness execution"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0076",
  "date": "2026-11-03",
  "firmware_version": "v0.5.1",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Documentation check for v0.5.1",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v051-docs-check.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to specific test harness execution"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0077",
  "date": "2026-11-03",
  "firmware_version": "v0.5.2",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Host unit tests for v0.5.2 lock screen redraw",
  "hardware": "Host x64 architecture",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v052-host-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to specific test harness execution"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0078",
  "date": "2026-11-03",
  "firmware_version": "v0.5.2",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Desktop simulator tests for v0.5.2",
  "hardware": "Host x64 architecture",
  "wiring": "None",
  "procedure": "tools/run_desktop_tests.bat",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v052-desktop-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to specific test harness execution"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0079",
  "date": "2026-11-03",
  "firmware_version": "v0.5.2",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "PlatformIO build for v0.5.2",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v052-pio-build.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to specific test harness execution"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0080",
  "date": "2026-11-03",
  "firmware_version": "v0.5.2",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Documentation check for v0.5.2",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v052-docs-check.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to specific test harness execution"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0081",
  "date": "2026-11-03",
  "firmware_version": "v0.6.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Host unit tests for v0.6.0 TimeService & USB sync",
  "hardware": "Host x64 architecture",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v060-host-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to specific test harness execution"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0082",
  "date": "2026-11-03",
  "firmware_version": "v0.6.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Desktop simulator tests for v0.6.0",
  "hardware": "Host x64 architecture",
  "wiring": "None",
  "procedure": "tools/run_desktop_tests.bat",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v060-desktop-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to specific test harness execution"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0083",
  "date": "2026-11-03",
  "firmware_version": "v0.6.0",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "PlatformIO build for v0.6.0",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v060-pio-build.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to specific test harness execution"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0084",
  "date": "2026-11-03",
  "firmware_version": "v0.6.0",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Documentation check for v0.6.0",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v060-docs-check.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to specific test harness execution"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0085",
  "date": "2026-11-03",
  "firmware_version": "v0.6.1",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Host unit tests for v0.6.1 display wake timing",
  "hardware": "Host x64 architecture",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v061-host-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to specific test harness execution"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0086",
  "date": "2026-11-03",
  "firmware_version": "v0.6.1",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Desktop simulator tests for v0.6.1",
  "hardware": "Host x64 architecture",
  "wiring": "None",
  "procedure": "tools/run_desktop_tests.bat",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v061-desktop-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to specific test harness execution"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0087",
  "date": "2026-11-03",
  "firmware_version": "v0.6.1",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "PlatformIO build for v0.6.1",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v061-build.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to specific test harness execution"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0088",
  "date": "2026-11-03",
  "firmware_version": "v0.6.1",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Documentation check for v0.6.1",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v061-docs-check.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to specific test harness execution"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0089",
  "date": "2026-11-03",
  "firmware_version": "v0.7.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Host unit tests for v0.7.0 Encrypted Vault Storage",
  "hardware": "Host x64 architecture",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v070-host-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to specific test harness execution"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0090",
  "date": "2026-11-03",
  "firmware_version": "v0.7.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Desktop simulator tests for v0.7.0",
  "hardware": "Host x64 architecture",
  "wiring": "None",
  "procedure": "tools/run_desktop_tests.bat",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v070-desktop-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to specific test harness execution"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0091",
  "date": "2026-11-03",
  "firmware_version": "v0.7.0",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "PlatformIO build for v0.7.0",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v070-pio-build.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to specific test harness execution"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0092",
  "date": "2026-11-03",
  "firmware_version": "v0.7.0",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Documentation check for v0.7.0",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-03-v070-docs-check.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to specific test harness execution"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0093",
  "date": "2026-11-04",
  "firmware_version": "v0.8.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Host unit tests for v0.8.0 VaultApp UI",
  "hardware": "Host x64 architecture",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v080-host-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to specific test harness execution"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0094",
  "date": "2026-11-04",
  "firmware_version": "v0.8.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Desktop simulator tests for v0.8.0",
  "hardware": "Host x64 architecture",
  "wiring": "None",
  "procedure": "tools/run_desktop_tests.bat",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v080-desktop-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to specific test harness execution"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0095",
  "date": "2026-11-04",
  "firmware_version": "v0.8.0",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "PlatformIO build for v0.8.0",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v080-pio-build.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to specific test harness execution"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0096",
  "date": "2026-11-04",
  "firmware_version": "v0.8.0",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Documentation check for v0.8.0",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v080-docs-check.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to specific test harness execution"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0097",
  "date": "2026-11-04",
  "firmware_version": "v0.8.1",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Host unit tests for v0.8.1 USB HID & Vault reload",
  "hardware": "Host x64 architecture",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v081-host-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to specific test harness execution"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0098",
  "date": "2026-11-04",
  "firmware_version": "v0.8.1",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Desktop simulator tests for v0.8.1",
  "hardware": "Host x64 architecture",
  "wiring": "None",
  "procedure": "tools/run_desktop_tests.bat",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v081-desktop-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to specific test harness execution"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0099",
  "date": "2026-11-04",
  "firmware_version": "v0.8.1",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "PlatformIO build for v0.8.1",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v081-pio-build.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to specific test harness execution"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0100",
  "date": "2026-11-04",
  "firmware_version": "v0.8.1",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Documentation check for v0.8.1",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v081-docs-check.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to specific test harness execution"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0101",
  "date": "2026-11-04",
  "firmware_version": "v0.9.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Host tests for Web Portal Service (SoftAP credential generation, PIN rejection, device lock gating, REST API for Vault & TOTP)",
  "hardware": "None (Host Mock)",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "PASS with zero errors",
  "actual": "PASS: Web Portal Service Host Tests verified successfully with zero errors",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v090-host-tests.txt"
  ],
  "evidence_kind": "Host test execution log",
  "environment": "MSVC 2022 x64 / Python 3.14",
  "limitations": "Desktop execution does not test real RF Wi-Fi radiation"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0102",
  "date": "2026-11-04",
  "firmware_version": "v0.9.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Automated desktop simulator tests including WebPortalService simulation and settings integration",
  "hardware": "None (Desktop Simulator)",
  "wiring": "None",
  "procedure": "tools/run_desktop_tests.bat",
  "expected": "ALL DESKTOP SIMULATOR TESTS PASSED SUCCESSFULLY",
  "actual": "PASS: All tests including testWebPortalSimulation passed with zero errors",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v090-desktop-tests.txt"
  ],
  "evidence_kind": "Desktop simulator log",
  "environment": "MSVC 2022 / SDL2",
  "limitations": "Simulated environment"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0103",
  "date": "2026-11-04",
  "firmware_version": "v0.9.0",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "PlatformIO release build for lilygo-t-display-s3",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "PASS with zero errors",
  "actual": "PASS: Flash: 942,441 B (14.4% of 6.25 MB partition), RAM: 65,096 B (19.9%), binary: 942,800 B",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v090-pio-build.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / espressif32@6.5.0 / Arduino-ESP32 2.0.14",
  "limitations": "Scope limited to compile-time analysis"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0104",
  "date": "2026-11-04",
  "firmware_version": "v0.9.0",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Documentation check for v0.9.0",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v090-docs-check.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to documentation schema"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0105",
  "date": "2026-11-04",
  "firmware_version": "v0.9.1",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Python and C++ host test suite execution for v0.9.1",
  "hardware": "None (Host simulated)",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "PASS with zero errors",
  "actual": "PASS: All host tests passed (lifecycle, import, vault, web portal, navigation, crypto, settings)",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v091-host-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "Windows 11 / MSVC 2022 / Python 3.14",
  "limitations": "Host-only simulation; does not verify physical display, touch or hardware RF"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0106",
  "date": "2026-11-04",
  "firmware_version": "v0.9.1",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Automated desktop test suite for v0.9.1",
  "hardware": "None (Desktop simulation)",
  "wiring": "None",
  "procedure": "cmd /c tools\\run_desktop_tests.bat",
  "expected": "PASS with zero errors",
  "actual": "PASS: All desktop simulator tests passed (lifecycle, UI, Wi-Fi, settings, HAL, sleep/wake, vault, web portal, 25 entry/exit cycles)",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v091-desktop-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "Windows 11 / MSVC 2022 / SDL2",
  "limitations": "Desktop simulation only; does not verify physical ESP32-S3 hardware"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0107",
  "date": "2026-11-04",
  "firmware_version": "v0.9.1",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "PlatformIO release build for lilygo-t-display-s3",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "PASS with zero errors",
  "actual": "PASS: Flash: 956,617 B (14.6% of 6.25 MB partition), RAM: 65,096 B (19.9%), binary: 956,976 B",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v091-pio-build.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / espressif32@6.5.0 / Arduino-ESP32 2.0.14",
  "limitations": "Scope limited to compile-time analysis"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0108",
  "date": "2026-11-04",
  "firmware_version": "v0.9.1",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Documentation check for v0.9.1",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v091-docs-check.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to documentation schema"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0109",
  "date": "2026-11-04",
  "firmware_version": "v0.9.2",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Python and C++ host test suite execution for v0.9.2",
  "hardware": "None (Host simulated)",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "PASS with zero errors",
  "actual": "PASS: All host tests passed (lifecycle, import, vault, web portal, navigation, crypto, settings)",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v092-host-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "Windows 11 / MSVC 2022 / Python 3.14",
  "limitations": "Host-only simulation; does not verify physical display, touch or hardware RF"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0110",
  "date": "2026-11-04",
  "firmware_version": "v0.9.2",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Automated desktop test suite for v0.9.2",
  "hardware": "None (Desktop simulation)",
  "wiring": "None",
  "procedure": "cmd /c tools\\run_desktop_tests.bat",
  "expected": "PASS with zero errors",
  "actual": "PASS: All desktop simulator tests passed (lifecycle, UI, Wi-Fi, settings, HAL, sleep/wake, vault, web portal, 25 entry/exit cycles)",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v092-desktop-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "Windows 11 / MSVC 2022 / SDL2",
  "limitations": "Desktop simulation only; does not verify physical ESP32-S3 hardware"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0111",
  "date": "2026-11-04",
  "firmware_version": "v0.9.2",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "PlatformIO release build for lilygo-t-display-s3",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "PASS with zero errors",
  "actual": "PASS: Flash: 958,537 B (14.6% of 6.25 MB partition), RAM: 65,096 B (19.9%), binary: 958,896 B",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v092-pio-build.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / espressif32@6.5.0 / Arduino-ESP32 2.0.14",
  "limitations": "Scope limited to compile-time analysis"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0112",
  "date": "2026-11-04",
  "firmware_version": "v0.9.2",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Documentation check for v0.9.2",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v092-docs-check.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to documentation schema"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0113",
  "date": "2026-11-04",
  "firmware_version": "v0.9.3",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Python and C++ host test suite execution for v0.9.3 with 2FA cancel regression assertions",
  "hardware": "None (Host simulated)",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "PASS with zero errors; card components present after cancelling ADD and DELETE",
  "actual": "PASS: All host tests passed including 2FA cancel UI restoration assertions",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v093-host-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "Windows 11 / MSVC 2022 / Python 3.14",
  "limitations": "Host-only simulation; does not verify physical display, touch or hardware RF"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0114",
  "date": "2026-11-04",
  "firmware_version": "v0.9.3",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Automated desktop test suite for v0.9.3",
  "hardware": "None (Desktop simulation)",
  "wiring": "None",
  "procedure": "cmd /c tools\\run_desktop_tests.bat",
  "expected": "PASS with zero errors",
  "actual": "PASS: All desktop simulator tests passed (lifecycle, UI, Wi-Fi, settings, HAL, sleep/wake, vault, web portal, 25 entry/exit cycles)",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v093-desktop-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "Windows 11 / MSVC 2022 / SDL2",
  "limitations": "Desktop simulation only; does not verify physical ESP32-S3 hardware"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0115",
  "date": "2026-11-04",
  "firmware_version": "v0.9.3",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "PlatformIO release build for lilygo-t-display-s3",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "PASS with zero errors",
  "actual": "PASS: Flash: 958,557 B (14.6% of 6.25 MB partition), RAM: 65,096 B (19.9%), binary: 958,928 B",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v093-pio-build.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / espressif32@6.5.0 / Arduino-ESP32 2.0.14",
  "limitations": "Scope limited to compile-time analysis"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0116",
  "date": "2026-11-04",
  "firmware_version": "v0.9.3",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Documentation check for v0.9.3",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v093-docs-check.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to documentation schema"
}
```


```json
{
  "record_type": "test",
  "id": "VER-0117",
  "date": "2026-11-04",
  "firmware_version": "v0.9.4",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Python and C++ host test suite execution for v0.9.4 with long-press gating, short-tap guidance, and hardware button trigger assertions",
  "hardware": "None (Host simulated)",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "PASS with zero errors; long-press triggers code sending, short tap shows guidance without sending, BOOT button triggers code sending",
  "actual": "PASS: All host tests passed including 2FA USB long press and button trigger assertions",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v094-host-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "Windows 11 / MSVC 2022 / Python 3.14",
  "limitations": "Host-only simulation; does not verify physical display, touch or hardware RF"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0118",
  "date": "2026-11-04",
  "firmware_version": "v0.9.4",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Automated desktop test suite for v0.9.4",
  "hardware": "None (Desktop simulation)",
  "wiring": "None",
  "procedure": "cmd /c tools\\run_desktop_tests.bat",
  "expected": "PASS with zero errors",
  "actual": "PASS: All desktop simulator tests passed (lifecycle, UI, Wi-Fi, settings, HAL, sleep/wake, vault, web portal, 25 entry/exit cycles)",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v094-desktop-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "Windows 11 / MSVC 2022 / SDL2",
  "limitations": "Desktop simulation only; does not verify physical ESP32-S3 hardware"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0119",
  "date": "2026-11-04",
  "firmware_version": "v0.9.4",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "PlatformIO release build for lilygo-t-display-s3",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "PASS with zero errors",
  "actual": "PASS: Flash: 959,077 B (14.6% of 6.25 MB partition), RAM: 65,096 B (19.9%), binary: 959,440 B",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v094-pio-build.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / espressif32@6.5.0 / Arduino-ESP32 2.0.14",
  "limitations": "Scope limited to compile-time analysis"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0120",
  "date": "2026-11-04",
  "firmware_version": "v0.9.4",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Documentation check for v0.9.4",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v094-docs-check.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to documentation schema"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0121",
  "date": "2026-11-04",
  "firmware_version": "v0.10.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Desktop simulation tests for MVP features suite (Generator, Clock, BLE, Vault categories, Web Portal Backup & Restore)",
  "hardware": "Windows host simulation (MSVC 2022 / SDL2)",
  "wiring": "None (Host simulation)",
  "procedure": "tools\\run_desktop_tests.bat",
  "expected": "PASS with zero errors",
  "actual": "PASS: All desktop simulator tests passed (lifecycle, UI, Wi-Fi, settings, HAL, sleep/wake, vault, web portal backup & restore, generator, clock, ble, 25 entry/exit cycles)",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0100-desktop-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "Windows 11 / MSVC 2022 / SDL2",
  "limitations": "Desktop simulation only; does not verify physical ESP32-S3 hardware"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0122",
  "date": "2026-11-04",
  "firmware_version": "v0.10.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Host Python and MSVC test suite for v0.10.0",
  "hardware": "Windows host",
  "wiring": "None (Host unit tests)",
  "procedure": "python tools/run_host_tests.py",
  "expected": "PASS with zero errors",
  "actual": "PASS: All host tests passed (authenticator lifecycle, bounded paired account import, vault encrypted storage & backup, web portal endpoints, crypto, settings)",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0100-host-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "Windows 11 / Python 3.14 / MSVC 2022",
  "limitations": "Scope limited to host software tests"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0123",
  "date": "2026-11-04",
  "firmware_version": "v0.10.0",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "PlatformIO release build for lilygo-t-display-s3 (v0.10.0)",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "PASS with zero errors",
  "actual": "PASS: Flash: 975,293 B (14.9% of 6.25 MB partition), RAM: 65,176 B (19.9%), binary: 975,664 B",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0100-pio-build.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / espressif32@6.5.0 / Arduino-ESP32 2.0.14",
  "limitations": "Scope limited to compile-time analysis"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0124",
  "date": "2026-11-04",
  "firmware_version": "v0.10.0",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Documentation check for v0.10.0",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0100-docs-check.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to documentation schema"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0125",
  "date": "2026-11-04",
  "firmware_version": "v0.10.1",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Desktop simulation tests for Password Generator vault auto-save and Clock Screensaver",
  "hardware": "Windows host simulation (MSVC 2022 / SDL2)",
  "wiring": "None (Host simulation)",
  "procedure": "tools\\run_desktop_tests.bat",
  "expected": "PASS with zero errors",
  "actual": "PASS: All desktop simulator tests passed (screensaver activation, deactivation on touch/button, generator vault auto-unlock & auto-save, lifecycle, 25 entry/exit cycles)",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0101-desktop-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "Windows 11 / MSVC 2022 / SDL2",
  "limitations": "Desktop simulation only; does not verify physical ESP32-S3 hardware"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0126",
  "date": "2026-11-04",
  "firmware_version": "v0.10.1",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Host Python and MSVC test suite for v0.10.1",
  "hardware": "Windows host",
  "wiring": "None (Host unit tests)",
  "procedure": "python tools/run_host_tests.py",
  "expected": "PASS with zero errors",
  "actual": "PASS: All host tests passed (authenticator lifecycle, bounded paired account import, vault encrypted storage & backup, web portal endpoints, crypto, settings)",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0101-host-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "Windows 11 / Python 3.14 / MSVC 2022",
  "limitations": "Scope limited to host software tests"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0127",
  "date": "2026-11-04",
  "firmware_version": "v0.10.1",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "PlatformIO release build for lilygo-t-display-s3 (v0.10.1)",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "PASS with zero errors",
  "actual": "PASS: Flash: 977,297 B (14.9% of 6.25 MB partition), RAM: 65,200 B (19.9%), binary: 977,664 B",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0101-pio-build.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / espressif32@6.5.0 / Arduino-ESP32 2.0.14",
  "limitations": "Scope limited to compile-time analysis"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0128",
  "date": "2026-11-04",
  "firmware_version": "v0.10.1",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Documentation check for v0.10.1",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0101-docs-check.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to documentation schema"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0129",
  "date": "2026-11-04",
  "firmware_version": "v0.10.2",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Desktop simulation tests for Bluetooth LE HID stack, GeneratorApp BLE typing, and TotpApp BLE toggle",
  "hardware": "Windows host simulation (MSVC 2022 / SDL2)",
  "wiring": "None (Host simulation)",
  "procedure": "tools\\run_desktop_tests.bat",
  "expected": "PASS with zero errors",
  "actual": "PASS: All desktop simulator tests passed (BLE lifecycle, BLE typing in GeneratorApp, 25 entry/exit cycles)",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0102-desktop-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "Windows 11 / MSVC 2022 / SDL2",
  "limitations": "Desktop simulation only; does not verify physical ESP32-S3 radio"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0130",
  "date": "2026-11-04",
  "firmware_version": "v0.10.2",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Host Python and MSVC test suite for v0.10.2",
  "hardware": "Windows host",
  "wiring": "None (Host unit tests)",
  "procedure": "python tools/run_host_tests.py",
  "expected": "PASS with zero errors",
  "actual": "PASS: All host tests passed (authenticator lifecycle, bounded paired account import, vault encrypted storage & backup, web portal endpoints, crypto, settings)",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0102-host-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "Windows 11 / Python 3.14 / MSVC 2022",
  "limitations": "Scope limited to host software tests"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0131",
  "date": "2026-11-04",
  "firmware_version": "v0.10.2",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "PlatformIO release build for lilygo-t-display-s3 (v0.10.2) with native ESP32 BLE stack",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S / BLE radio",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "PASS with zero errors",
  "actual": "PASS: Flash: 1,454,077 B (22.2% of 6.25 MB partition), RAM: 84,952 B (25.9%), binary: 1,454,448 B",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0102-pio-build.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / espressif32@6.5.0 / Arduino-ESP32 2.0.14",
  "limitations": "Scope limited to compile-time analysis"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0132",
  "date": "2026-11-04",
  "firmware_version": "v0.10.2",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Documentation check for v0.10.2",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0102-docs-check.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to documentation schema"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0133",
  "date": "2026-11-04",
  "firmware_version": "v0.10.3",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Desktop simulation tests for SettingsApp Bluetooth touch routing, enable/disable, and BOOT primaryAction",
  "hardware": "Windows host simulation (MSVC 2022 / SDL2)",
  "wiring": "None (Host simulation)",
  "procedure": "tools\\run_desktop_tests.bat",
  "expected": "PASS with zero errors",
  "actual": "PASS: All desktop simulator tests passed (SettingsApp Bluetooth subview touch and primaryAction toggle)",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0103-desktop-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "Windows 11 / MSVC 2022 / SDL2",
  "limitations": "Desktop simulation only; does not verify physical ESP32-S3 radio"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0134",
  "date": "2026-11-04",
  "firmware_version": "v0.10.3",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Host Python and MSVC test suite for v0.10.3",
  "hardware": "Windows host",
  "wiring": "None (Host unit tests)",
  "procedure": "python tools/run_host_tests.py",
  "expected": "PASS with zero errors",
  "actual": "PASS: All host tests passed (authenticator lifecycle, bounded paired account import, vault encrypted storage & backup, web portal endpoints, crypto, settings)",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0103-host-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "Windows 11 / Python 3.14 / MSVC 2022",
  "limitations": "Scope limited to host software tests"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0135",
  "date": "2026-11-04",
  "firmware_version": "v0.10.3",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "PlatformIO release build for lilygo-t-display-s3 (v0.10.3)",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S / BLE radio",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "PASS with zero errors",
  "actual": "PASS: Flash: 1,564,065 B (23.9% of 6.25 MB partition), RAM: 85,784 B (26.2%), binary: 1,564,432 B",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0103-pio-build.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / espressif32@6.5.0 / Arduino-ESP32 2.0.14",
  "limitations": "Scope limited to compile-time analysis"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0136",
  "date": "2026-11-04",
  "firmware_version": "v0.10.3",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Documentation check for v0.10.3",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0103-docs-check.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to documentation schema"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0137",
  "date": "2026-11-04",
  "firmware_version": "v0.10.4",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Desktop simulator tests for v0.10.4",
  "hardware": "Host x64 architecture",
  "wiring": "None",
  "procedure": "tools/run_desktop_tests.bat",
  "expected": "PASS with zero errors",
  "actual": "PASS: All desktop simulator tests passed",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0104-desktop-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "Windows 11 / MSVC 2022 / SDL2",
  "limitations": "Scope limited to desktop simulation"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0138",
  "date": "2026-11-04",
  "firmware_version": "v0.10.4",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Host unit tests for v0.10.4",
  "hardware": "Host x64 architecture",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "PASS with zero errors",
  "actual": "PASS: All host tests passed",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0104-host-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "Windows 11 / Python 3.14 / MSVC 2022",
  "limitations": "Scope limited to host software tests"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0139",
  "date": "2026-11-04",
  "firmware_version": "v0.10.4",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "PlatformIO release build for lilygo-t-display-s3 (v0.10.4)",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S / BLE radio",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "PASS with zero errors",
  "actual": "PASS: Flash: 1,565,117 B (23.9% of 6.25 MB partition), RAM: 85,784 B (26.2%), binary: 1,565,488 B",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0104-pio-build.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / espressif32@6.5.0 / Arduino-ESP32 2.0.14",
  "limitations": "Scope limited to compile-time analysis"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0140",
  "date": "2026-11-04",
  "firmware_version": "v0.10.4",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Documentation check for v0.10.4",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0104-docs-check.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to documentation schema"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0141",
  "date": "2026-11-04",
  "firmware_version": "v0.10.5",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Desktop simulator tests for v0.10.5",
  "hardware": "Host x64 architecture",
  "wiring": "None",
  "procedure": "tools/run_desktop_tests.bat",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0105-desktop-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "Windows 11 / MSVC 2022 / SDL2",
  "limitations": "Scope limited to desktop simulation"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0142",
  "date": "2026-11-04",
  "firmware_version": "v0.10.5",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Host unit tests for v0.10.5",
  "hardware": "Host x64 architecture",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0105-host-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "Windows 11 / Python 3.14 / MSVC 2022",
  "limitations": "Scope limited to host software tests"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0143",
  "date": "2026-11-04",
  "firmware_version": "v0.10.5",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "PlatformIO release build for lilygo-t-display-s3 (v0.10.5)",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S / BLE radio",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0105-pio-build.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / espressif32@6.5.0 / Arduino-ESP32 2.0.14",
  "limitations": "Scope limited to compile-time analysis"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0144",
  "date": "2026-11-04",
  "firmware_version": "v0.10.5",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Documentation check for v0.10.5",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0105-docs-check.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to documentation schema"
}
```


```json
{
  "record_type": "test",
  "id": "VER-0145",
  "date": "2026-11-04",
  "firmware_version": "v0.10.5",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "BLE initialization failure regression before repair",
  "hardware": "None",
  "wiring": "None",
  "procedure": "Compile production BleHidService ESP32 branch with a temporary MSVC fault-injection harness.",
  "expected": "Rejected initialization leaves Disabled and releases lease.",
  "actual": "FAIL at !service.setEnabled(true, context) after injected controller-init failure.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-04-v0106-ble-mock.txt"
  ],
  "evidence_kind": "Captured output or identified transcript",
  "environment": "Windows / MSVC with BLE and ESP-IDF doubles",
  "limitations": "Simulates an early return from Arduino init. Does not model the real busy-loop timing."
}
```


```json
{
  "record_type": "test",
  "id": "VER-0146",
  "date": "2026-11-04",
  "firmware_version": "v0.10.6",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "BLE controller and host startup rejection and retry",
  "hardware": "None",
  "wiring": "None",
  "procedure": "Repeat the same temporary fault-injection harness against the changed service.",
  "expected": "Each failure keeps Disabled and releases the radio; successful retry acquires it and can be disabled.",
  "actual": "PASS for controller allocation failure, host startup failure, retry and disable.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0106-ble-mock.txt"
  ],
  "evidence_kind": "Captured output or identified transcript",
  "environment": "Windows / MSVC with BLE and ESP-IDF doubles",
  "limitations": "Software failure handling only."
}
```


```json
{
  "record_type": "test",
  "id": "VER-0147",
  "date": "2026-11-04",
  "firmware_version": "v0.10.6",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Host regression suite",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "Exit 0.",
  "actual": "All host smoke suites passed. Exit 0.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0106-host.txt"
  ],
  "evidence_kind": "Captured output or identified transcript",
  "environment": "Windows / MSVC",
  "limitations": "Host stubs."
}
```


```json
{
  "record_type": "test",
  "id": "VER-0148",
  "date": "2026-11-04",
  "firmware_version": "v0.10.6",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "First simulator invocation via Python fallback",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/run_desktop_tests.py",
  "expected": "Run SDL2 suite.",
  "actual": "Exited 1 because SDL2_ROOT was unavailable for the fallback.",
  "result": "BLOCKED",
  "evidence": [
    "docs/evidence/2026-11-04-v0106-desktop-attempt.txt"
  ],
  "evidence_kind": "Captured output or identified transcript",
  "environment": "Windows / Python",
  "limitations": "The standard batch runner uses CMake and was run separately."
}
```


```json
{
  "record_type": "test",
  "id": "VER-0149",
  "date": "2026-11-04",
  "firmware_version": "v0.10.6",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Desktop simulator suite including BLE enable after Wi-Fi lease contention",
  "hardware": "None",
  "wiring": "None",
  "procedure": "tools/run_desktop_tests.bat",
  "expected": "Exit 0.",
  "actual": "All simulator tests passed. Exit 0.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0106-desktop.txt"
  ],
  "evidence_kind": "Captured output or identified transcript",
  "environment": "Windows / MSVC / CMake / SDL2",
  "limitations": "Software simulation. CMake emitted its existing FetchContent developer warning."
}
```


```json
{
  "record_type": "test",
  "id": "VER-0150",
  "date": "2026-11-04",
  "firmware_version": "v0.10.6",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "Default firmware build",
  "hardware": "None",
  "wiring": "None",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Build the firmware image.",
  "actual": "Exit 0. Image 1,565,824 B, linked Flash 1,565,457 B, static SRAM 85,784 B.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0106-build.txt"
  ],
  "evidence_kind": "Captured output or identified transcript",
  "environment": "PlatformIO / espressif32 6.5.0 / Arduino-ESP32 2.0.14",
  "limitations": "TFT_eSPI parallel DMA and TOUCH_CS warnings. Compile-time evidence only."
}
```


```json
{
  "record_type": "test",
  "id": "VER-0151",
  "date": "2026-11-04",
  "firmware_version": "v0.10.6",
  "commit": "WORKING_TREE",
  "verification_type": "STATIC",
  "scope": "Application image and partition capacity",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_resources.py",
  "expected": "Image fits generated app0 partition.",
  "actual": "PASS. App0 6,553,600 B, 4,987,776 B remaining. Historical baseline warning persists (+86.02%).",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0106-resources.txt"
  ],
  "evidence_kind": "Captured output or identified transcript",
  "environment": "Windows / Python",
  "limitations": "Runtime metrics NOT MEASURED."
}
```


```json
{
  "record_type": "test",
  "id": "VER-0152",
  "date": "2026-11-04",
  "firmware_version": "v0.10.6",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Version and engineering record consistency",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "Exit 0.",
  "actual": "Documentation checker exited 0.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0106-docs.txt"
  ],
  "evidence_kind": "Captured output or identified transcript",
  "environment": "Windows / Python",
  "limitations": "Documentation consistency only."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0153",
  "date": "2026-11-04",
  "firmware_version": "v0.10.6",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "RED Wi-Fi-first BLE coexistence regression",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "Complete the stated verification at its own layer.",
  "actual": "Before the repair WiFi.sleepEnabled assertion failed after BLE enable.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-04-v0107-host-red.txt"
  ],
  "evidence_kind": "Captured command output",
  "environment": "Windows / MSVC, PlatformIO or Python as applicable",
  "limitations": "Software evidence. No physical acceptance follows from this record."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0154",
  "date": "2026-11-04",
  "firmware_version": "v0.10.7",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Initial host rebuild with missing SDK stub methods",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "Complete the stated verification at its own layer.",
  "actual": "Compilation failed because the test stub lacked WiFi.getMode and esp_wifi_get_ps.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-04-v0107-host-compile-attempt.txt"
  ],
  "evidence_kind": "Captured command output",
  "environment": "Windows / MSVC, PlatformIO or Python as applicable",
  "limitations": "Software evidence. No physical acceptance follows from this record."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0155",
  "date": "2026-11-04",
  "firmware_version": "v0.10.7",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Wi-Fi-first enable, driver rejection, reconnect, and disable",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "Complete the stated verification at its own layer.",
  "actual": "All host smoke checks passed, including clean rejection of BLE startup when modem sleep remains disabled.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0107-host.txt"
  ],
  "evidence_kind": "Captured command output",
  "environment": "Windows / MSVC, PlatformIO or Python as applicable",
  "limitations": "Software evidence. No physical acceptance follows from this record."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0156",
  "date": "2026-11-04",
  "firmware_version": "v0.10.7",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Settings integration and modem sleep lifetime",
  "hardware": "None",
  "wiring": "None",
  "procedure": "tools/run_desktop_tests.bat",
  "expected": "Complete the stated verification at its own layer.",
  "actual": "All desktop tests passed. BLE enable/disable drives the simulated sleep policy.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0107-desktop.txt"
  ],
  "evidence_kind": "Captured command output",
  "environment": "Windows / MSVC, PlatformIO or Python as applicable",
  "limitations": "Software evidence. No physical acceptance follows from this record."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0157",
  "date": "2026-11-04",
  "firmware_version": "v0.10.7",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Startup fault rollback and retry",
  "hardware": "None",
  "wiring": "None",
  "procedure": "Temporary MSVC ESP32 branch fault injection",
  "expected": "Complete the stated verification at its own layer.",
  "actual": "Controller/host failures released the radio and restored the Wi-Fi policy. Retry and disable succeeded.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0107-ble-mock.txt"
  ],
  "evidence_kind": "Captured command output",
  "environment": "Windows / MSVC, PlatformIO or Python as applicable",
  "limitations": "Software evidence. No physical acceptance follows from this record."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0158",
  "date": "2026-11-04",
  "firmware_version": "v0.10.7",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "ESP32 default environment build",
  "hardware": "ESP32-S3 target",
  "wiring": "None",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Complete the stated verification at its own layer.",
  "actual": "Image 1,566,096 B. Static SRAM 85,800 B. Exit 0.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0107-build.txt"
  ],
  "evidence_kind": "Captured command output",
  "environment": "Windows / MSVC, PlatformIO or Python as applicable",
  "limitations": "Software evidence. No physical acceptance follows from this record."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0159",
  "date": "2026-11-04",
  "firmware_version": "v0.10.7",
  "commit": "WORKING_TREE",
  "verification_type": "STATIC",
  "scope": "Image and partition capacity",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_resources.py",
  "expected": "Complete the stated verification at its own layer.",
  "actual": "4,987,504 B remain in app0. Cumulative pre-BLE warning persists.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0107-resources.txt"
  ],
  "evidence_kind": "Captured command output",
  "environment": "Windows / MSVC, PlatformIO or Python as applicable",
  "limitations": "Software evidence. No physical acceptance follows from this record."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0160",
  "date": "2026-11-04",
  "firmware_version": "v0.10.7",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Engineering records and version consistency",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "Complete the stated verification at its own layer.",
  "actual": "Documentation checks passed.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0107-docs.txt"
  ],
  "evidence_kind": "Captured command output",
  "environment": "Windows / MSVC, PlatformIO or Python as applicable",
  "limitations": "Software evidence. No physical acceptance follows from this record."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0161",
  "date": "2026-11-04",
  "firmware_version": "v0.10.8",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Host regression suite",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "Exit 0.",
  "actual": "All host smoke tests passed.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0108-host.txt"
  ],
  "evidence_kind": "Captured output",
  "environment": "Windows / PlatformIO, MSVC or Python",
  "limitations": "Software checks. The disconnect cause and HID delivery still require hardware evidence."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0162",
  "date": "2026-11-04",
  "firmware_version": "v0.10.8",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Desktop simulator suite",
  "hardware": "None",
  "wiring": "None",
  "procedure": "tools/run_desktop_tests.bat",
  "expected": "Exit 0.",
  "actual": "Desktop suite passed, including Settings and Generator BLE paths.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0108-desktop.txt"
  ],
  "evidence_kind": "Captured output",
  "environment": "Windows / PlatformIO, MSVC or Python",
  "limitations": "Software checks. The disconnect cause and HID delivery still require hardware evidence."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0163",
  "date": "2026-11-04",
  "firmware_version": "v0.10.8",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "ESP32 default environment",
  "hardware": "ESP32-S3 target",
  "wiring": "None",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Exit 0.",
  "actual": "Build passed. Image 1,566,464 B and static SRAM 85,800 B.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0108-build.txt"
  ],
  "evidence_kind": "Captured output",
  "environment": "Windows / PlatformIO, MSVC or Python",
  "limitations": "Software checks. The disconnect cause and HID delivery still require hardware evidence."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0164",
  "date": "2026-11-04",
  "firmware_version": "v0.10.8",
  "commit": "WORKING_TREE",
  "verification_type": "STATIC",
  "scope": "Image capacity",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_resources.py",
  "expected": "Exit 0.",
  "actual": "4,987,136 B remaining in app0. Pre-BLE baseline growth warning +86.10%.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0108-resources.txt"
  ],
  "evidence_kind": "Captured output",
  "environment": "Windows / PlatformIO, MSVC or Python",
  "limitations": "Software checks. The disconnect cause and HID delivery still require hardware evidence."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0165",
  "date": "2026-11-04",
  "firmware_version": "v0.10.8",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Engineering consistency",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "Exit 0.",
  "actual": "Documentation checks passed.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0108-docs.txt"
  ],
  "evidence_kind": "Captured output",
  "environment": "Windows / PlatformIO, MSVC or Python",
  "limitations": "Software checks. The disconnect cause and HID delivery still require hardware evidence."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0166",
  "date": "2026-11-04",
  "firmware_version": "v0.10.8",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Readiness test before fix",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "Pass the named verification.",
  "actual": "Failed because bare GATT connect was treated as ready.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-04-v0110-host-red.txt"
  ],
  "evidence_kind": "Captured terminal output",
  "environment": "Windows / PlatformIO, MSVC or Python",
  "limitations": "Software or artifact evidence only."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0167",
  "date": "2026-11-04",
  "firmware_version": "v0.10.8",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Home Bluetooth test before registration",
  "hardware": "None",
  "wiring": "None",
  "procedure": "tools/run_desktop_tests.bat",
  "expected": "Pass the named verification.",
  "actual": "Failed because opening the second Home card did not expose Bluetooth controls.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-04-v0110-desktop-red.txt"
  ],
  "evidence_kind": "Captured terminal output",
  "environment": "Windows / PlatformIO, MSVC or Python",
  "limitations": "Software or artifact evidence only."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0168",
  "date": "2026-11-04",
  "firmware_version": "v0.11.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Readiness and link loss regression",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "Pass the named verification.",
  "actual": "Passed. Raw link blocked typing; subscription enabled it; unsubscribe and disconnect blocked it.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0110-host.txt"
  ],
  "evidence_kind": "Captured terminal output",
  "environment": "Windows / PlatformIO, MSVC or Python",
  "limitations": "Software or artifact evidence only."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0169",
  "date": "2026-11-04",
  "firmware_version": "v0.11.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Settings and Bluetooth Home route regression",
  "hardware": "None",
  "wiring": "None",
  "procedure": "tools/run_desktop_tests.bat",
  "expected": "Pass the named verification.",
  "actual": "Passed. Touch opens Bluetooth beside Wi-Fi, Settings controls still work, and Back retains BLE for Generator.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0110-desktop.txt"
  ],
  "evidence_kind": "Captured terminal output",
  "environment": "Windows / PlatformIO, MSVC or Python",
  "limitations": "Software or artifact evidence only."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0170",
  "date": "2026-11-04",
  "firmware_version": "v0.11.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Desktop application compile attempt",
  "hardware": "None",
  "wiring": "None",
  "procedure": "cmake --build build --target nibru_simulator",
  "expected": "Pass the named verification.",
  "actual": "Pre-existing DesktopMain activeApp() calls failed because AppManager has no such method.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-04-v0110-desktop-app-attempt.txt"
  ],
  "evidence_kind": "Captured terminal output",
  "environment": "Windows / PlatformIO, MSVC or Python",
  "limitations": "Software or artifact evidence only."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0171",
  "date": "2026-11-04",
  "firmware_version": "v0.11.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Desktop application build and headless smoke",
  "hardware": "None",
  "wiring": "None",
  "procedure": "cmake --build build --target nibru_simulator and run five headless frames",
  "expected": "Pass the named verification.",
  "actual": "Passed after removing redundant display restore calls already covered by launcher.invalidate().",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0110-desktop-app.txt"
  ],
  "evidence_kind": "Captured terminal output",
  "environment": "Windows / PlatformIO, MSVC or Python",
  "limitations": "Software or artifact evidence only."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0172",
  "date": "2026-11-04",
  "firmware_version": "v0.11.0",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "ESP32 default firmware build",
  "hardware": "ESP32-S3",
  "wiring": "None",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Pass the named verification.",
  "actual": "Passed. Firmware image 1,567,296 B. Static SRAM 85,832 B.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0110-build.txt"
  ],
  "evidence_kind": "Captured terminal output",
  "environment": "Windows / PlatformIO, MSVC or Python",
  "limitations": "Software or artifact evidence only."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0173",
  "date": "2026-11-04",
  "firmware_version": "v0.11.0",
  "commit": "WORKING_TREE",
  "verification_type": "STATIC",
  "scope": "Image and partition capacity",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_resources.py",
  "expected": "Pass the named verification.",
  "actual": "App0 remaining 4,986,304 B. Historical baseline growth warning persists.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0110-resources.txt"
  ],
  "evidence_kind": "Captured terminal output",
  "environment": "Windows / PlatformIO, MSVC or Python",
  "limitations": "Software or artifact evidence only."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0174",
  "date": "2026-11-04",
  "firmware_version": "v0.11.0",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Engineering record consistency",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "Pass the named verification.",
  "actual": "Documentation checks passed.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0110-docs.txt"
  ],
  "evidence_kind": "Captured terminal output",
  "environment": "Windows / PlatformIO, MSVC or Python",
  "limitations": "Software or artifact evidence only."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0175",
  "date": "2026-11-04",
  "firmware_version": "v0.11.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Unauthenticated CCCD regression RED",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "Complete verification at the specified layer.",
  "actual": "The prior service marked keyboard ready after CCCD alone.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-04-v0111-host-red.txt"
  ],
  "evidence_kind": "Captured terminal output",
  "environment": "Windows / PlatformIO, MSVC or Python",
  "limitations": "Software or build checks. Windows HID behavior requires hardware verification."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0176",
  "date": "2026-11-04",
  "firmware_version": "v0.11.1",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "First firmware build",
  "hardware": "ESP32-S3",
  "wiring": "None",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Complete verification at the specified layer.",
  "actual": "Failed because the pinned BLEServer::getGattsIf is private. Use the public GATT callback interface instead.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-04-v0111-build-attempt.txt"
  ],
  "evidence_kind": "Captured terminal output",
  "environment": "Windows / PlatformIO, MSVC or Python",
  "limitations": "Software or build checks. Windows HID behavior requires hardware verification."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0177",
  "date": "2026-11-04",
  "firmware_version": "v0.11.1",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Authentication and subscription regression",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "Complete verification at the specified layer.",
  "actual": "Passed for both event orders, authentication loss, unsubscribe, disconnect and reconnect.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0111-host.txt"
  ],
  "evidence_kind": "Captured terminal output",
  "environment": "Windows / PlatformIO, MSVC or Python",
  "limitations": "Software or build checks. Windows HID behavior requires hardware verification."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0178",
  "date": "2026-11-04",
  "firmware_version": "v0.11.1",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Desktop simulator regressions",
  "hardware": "None",
  "wiring": "None",
  "procedure": "tools/run_desktop_tests.bat",
  "expected": "Complete verification at the specified layer.",
  "actual": "All tests passed, including pre-authentication subscription guard and BLE controls.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0111-desktop.txt"
  ],
  "evidence_kind": "Captured terminal output",
  "environment": "Windows / PlatformIO, MSVC or Python",
  "limitations": "Software or build checks. Windows HID behavior requires hardware verification."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0179",
  "date": "2026-11-04",
  "firmware_version": "v0.11.1",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "ESP32 release build",
  "hardware": "ESP32-S3",
  "wiring": "None",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Complete verification at the specified layer.",
  "actual": "Passed. Image 1,572,160 B, static SRAM 85,904 B.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0111-build.txt"
  ],
  "evidence_kind": "Captured terminal output",
  "environment": "Windows / PlatformIO, MSVC or Python",
  "limitations": "Software or build checks. Windows HID behavior requires hardware verification."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0180",
  "date": "2026-11-04",
  "firmware_version": "v0.11.1",
  "commit": "WORKING_TREE",
  "verification_type": "STATIC",
  "scope": "Image partition capacity",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_resources.py",
  "expected": "Complete verification at the specified layer.",
  "actual": "Passed, with 4,981,440 B left in app0. Pre-BLE reference warning persists.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0111-resources.txt"
  ],
  "evidence_kind": "Captured terminal output",
  "environment": "Windows / PlatformIO, MSVC or Python",
  "limitations": "Software or build checks. Windows HID behavior requires hardware verification."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0181",
  "date": "2026-11-04",
  "firmware_version": "v0.11.1",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Engineering records",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "Complete verification at the specified layer.",
  "actual": "Documentation checks passed.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0111-docs.txt"
  ],
  "evidence_kind": "Captured terminal output",
  "environment": "Windows / PlatformIO, MSVC or Python",
  "limitations": "Software or build checks. Windows HID behavior requires hardware verification."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0182",
  "date": "2026-11-04",
  "firmware_version": "v0.11.1",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Passkey lifecycle RED",
  "hardware": "None",
  "wiring": "None",
  "procedure": "MSVC /Zs compile of host regression",
  "expected": "Complete the scoped verification.",
  "actual": "Expected compile failure because passkey handling did not exist.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-04-v0120-host-red.txt"
  ],
  "evidence_kind": "Captured output",
  "environment": "Windows / MSVC, Python or PlatformIO",
  "limitations": "Software evidence. Physical passkey, pairing and HID delivery remain unverified."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0183",
  "date": "2026-11-04",
  "firmware_version": "v0.12.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Passkey lifecycle and BLE readiness",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "Complete the scoped verification.",
  "actual": "Passed for code visibility, invalid value, authentication and disconnect clearing.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0120-host.txt"
  ],
  "evidence_kind": "Captured output",
  "environment": "Windows / MSVC, Python or PlatformIO",
  "limitations": "Software evidence. Physical passkey, pairing and HID delivery remain unverified."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0184",
  "date": "2026-11-04",
  "firmware_version": "v0.12.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Desktop simulator",
  "hardware": "None",
  "wiring": "None",
  "procedure": "tools/run_desktop_tests.bat",
  "expected": "Complete the scoped verification.",
  "actual": "All desktop checks passed, including service passkey transitions.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0120-desktop.txt"
  ],
  "evidence_kind": "Captured output",
  "environment": "Windows / MSVC, Python or PlatformIO",
  "limitations": "Software evidence. Physical passkey, pairing and HID delivery remain unverified."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0185",
  "date": "2026-11-04",
  "firmware_version": "v0.12.0",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "Default ESP32 firmware",
  "hardware": "ESP32-S3",
  "wiring": "None",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Complete the scoped verification.",
  "actual": "Passed. Image 1,572,800 B and static SRAM 85,928 B.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0120-build.txt"
  ],
  "evidence_kind": "Captured output",
  "environment": "Windows / MSVC, Python or PlatformIO",
  "limitations": "Software evidence. Physical passkey, pairing and HID delivery remain unverified."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0186",
  "date": "2026-11-04",
  "firmware_version": "v0.12.0",
  "commit": "WORKING_TREE",
  "verification_type": "STATIC",
  "scope": "Image capacity",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_resources.py",
  "expected": "Complete the scoped verification.",
  "actual": "Passed. 4,980,800 B free in app0. Historical cumulative growth warning persists.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0120-resources.txt"
  ],
  "evidence_kind": "Captured output",
  "environment": "Windows / MSVC, Python or PlatformIO",
  "limitations": "Software evidence. Physical passkey, pairing and HID delivery remain unverified."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0187",
  "date": "2026-11-04",
  "firmware_version": "v0.12.0",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Engineering consistency",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "Complete the scoped verification.",
  "actual": "Documentation checks passed.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0120-docs.txt"
  ],
  "evidence_kind": "Captured output",
  "environment": "Windows / MSVC, Python or PlatformIO",
  "limitations": "Software evidence. Physical passkey, pairing and HID delivery remain unverified."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0188",
  "date": "2026-11-04",
  "firmware_version": "v0.12.1",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Host regressions",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "Complete the scoped check.",
  "actual": "Host suite passed.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0121-host.txt"
  ],
  "evidence_kind": "Captured output",
  "environment": "Windows",
  "limitations": "Host and simulator do not exercise ESP32 GATT. No physical acceptance was run."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0189",
  "date": "2026-11-04",
  "firmware_version": "v0.12.1",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Desktop regressions",
  "hardware": "None",
  "wiring": "None",
  "procedure": "tools/run_desktop_tests.bat",
  "expected": "Complete the scoped check.",
  "actual": "Simulator suite passed. The direct Python fallback attempt failed because SDL2_ROOT was unset.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0121-desktop-cmake.txt"
  ],
  "evidence_kind": "Captured output",
  "environment": "Windows",
  "limitations": "Host and simulator do not exercise ESP32 GATT. No physical acceptance was run."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0190",
  "date": "2026-11-04",
  "firmware_version": "v0.12.1",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "ESP32 build and image capacity",
  "hardware": "None",
  "wiring": "None",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Complete the scoped check.",
  "actual": "Build and capacity check passed. Firmware is 1,572,864 bytes. Runtime metrics were not measured.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0121-build.txt"
  ],
  "evidence_kind": "Captured output",
  "environment": "Windows",
  "limitations": "Host and simulator do not exercise ESP32 GATT. No physical acceptance was run."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0191",
  "date": "2026-11-04",
  "firmware_version": "v0.13.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Initial Beacon Lab host compilation",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "Complete the scoped verification.",
  "actual": "The initial host build selected ESP32-only functions. Corrected the platform guard before rerunning.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-04-v0130-host-attempt.txt"
  ],
  "evidence_kind": "Captured output",
  "environment": "Windows",
  "limitations": "Software evidence only. Real beacon transmission and performance require hardware measurements."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0192",
  "date": "2026-11-04",
  "firmware_version": "v0.13.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Host regression and desktop lifecycle",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py; build nibru_simulator and run nibru_tests",
  "expected": "Complete the scoped verification.",
  "actual": "Both suites passed. Desktop assertions exercise all seven counts, radio exclusion, timeout and exit. Desktop executable built.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0130-host.txt",
    "docs/evidence/2026-11-04-v0130-desktop-final.txt"
  ],
  "evidence_kind": "Captured output",
  "environment": "Windows",
  "limitations": "Software evidence only. Real beacon transmission and performance require hardware measurements."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0193",
  "date": "2026-11-04",
  "firmware_version": "v0.13.0",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "ESP32 final build and image capacity",
  "hardware": "None",
  "wiring": "None",
  "procedure": "pio run -e lilygo-t-display-s3; python tools/check_resources.py",
  "expected": "Complete the scoped verification.",
  "actual": "ESP32 build and generated app0 capacity checks passed.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0130-build-final.txt",
    "docs/evidence/2026-11-04-v0130-resources.txt"
  ],
  "evidence_kind": "Captured output",
  "environment": "Windows",
  "limitations": "Software evidence only. Real beacon transmission and performance require hardware measurements."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0194",
  "date": "2026-11-04",
  "firmware_version": "v0.13.0",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Version and engineering consistency",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "Complete the scoped verification.",
  "actual": "Documentation checker passed.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0130-docs.txt"
  ],
  "evidence_kind": "Captured output",
  "environment": "Windows",
  "limitations": "Software evidence only. Real beacon transmission and performance require hardware measurements."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0195",
  "date": "2026-11-04",
  "firmware_version": "v0.14.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Beacon Lab controls",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py and tools/run_desktop_tests.bat",
  "expected": "Complete scoped verification.",
  "actual": "Host and simulator passed. Simulator exercised count cap, invalid inputs and configured session timeout.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0140-host.txt",
    "docs/evidence/2026-11-04-v0140-desktop.txt"
  ],
  "evidence_kind": "Captured output",
  "environment": "Windows",
  "limitations": "Physical RF and resource scenarios A\u2013E not run."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0196",
  "date": "2026-11-04",
  "firmware_version": "v0.14.0",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "ESP32 build and partition fit",
  "hardware": "None",
  "wiring": "None",
  "procedure": "pio run -e lilygo-t-display-s3 and python tools/check_resources.py",
  "expected": "Complete scoped verification.",
  "actual": "Build and capacity check passed.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0140-build.txt",
    "docs/evidence/2026-11-04-v0140-resources.txt"
  ],
  "evidence_kind": "Captured output",
  "environment": "Windows",
  "limitations": "Physical RF and resource scenarios A\u2013E not run."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0197",
  "date": "2026-11-04",
  "firmware_version": "v0.14.1",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Beacon Lab rate and counter",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py and tools/run_desktop_tests.bat",
  "expected": "Complete the scoped verification.",
  "actual": "Host and simulator passed. Simulator verified batch size, pacing, count saturation and retention after timeout.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0141-host.txt",
    "docs/evidence/2026-11-04-v0141-desktop.txt"
  ],
  "evidence_kind": "Captured output",
  "environment": "Windows",
  "limitations": "Physical RF and resource scenarios A\u2013E not run."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0198",
  "date": "2026-11-04",
  "firmware_version": "v0.14.1",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "ESP32 build and partition fit",
  "hardware": "None",
  "wiring": "None",
  "procedure": "pio run -e lilygo-t-display-s3 and python tools/check_resources.py",
  "expected": "Complete the scoped verification.",
  "actual": "Build and capacity check passed.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0141-build.txt",
    "docs/evidence/2026-11-04-v0141-resources.txt"
  ],
  "evidence_kind": "Captured output",
  "environment": "Windows",
  "limitations": "Physical RF and resource scenarios A\u2013E not run."
}
```


```json
{
  "record_type": "test",
  "id": "VER-0199",
  "date": "2026-11-04",
  "firmware_version": "v0.14.2",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "UI UX and shell navigation tests",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py and tools/run_desktop_tests.bat",
  "expected": "PASS with zero errors across all lifecycle, touch, and UI test suites.",
  "actual": "Host and simulator test suites passed. Verified tactile back navigation, DoubleTap fast-launch, and shell hitboxes.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0142-host.txt",
    "docs/evidence/2026-11-04-v0142-desktop.txt"
  ],
  "evidence_kind": "Captured output",
  "environment": "Windows",
  "limitations": "Physical touch accuracy and thumb comfort on real capacitive glass not yet measured."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0200",
  "date": "2026-11-04",
  "firmware_version": "v0.14.2",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "ESP32 firmware build and resource partition check for v0.14.2",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3 and python tools/check_resources.py",
  "expected": "Successful compilation within app0 partition capacity (6,553,600 bytes).",
  "actual": "Build passed cleanly. Flash 1,578,289 bytes (24.1%), RAM 85,968 bytes (26.2%). Partition fit verified.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0142-build.txt",
    "docs/evidence/2026-11-04-v0142-resources.txt"
  ],
  "evidence_kind": "Captured output",
  "environment": "Windows",
  "limitations": "Physical board flash and runtime heap profiling not yet executed."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0201",
  "date": "2026-11-04",
  "firmware_version": "v0.14.2",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Documentation integrity and version synchronization check for v0.14.2",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "Documentation checks PASS with zero errors.",
  "actual": "All required docs, records, anchors, links, and version synchronizations verified cleanly.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0142-docs.txt"
  ],
  "evidence_kind": "Captured output",
  "environment": "Windows",
  "limitations": "Static cross-reference and schema validation only."
}
```


```json
{
  "record_type": "test",
  "id": "VER-0202",
  "date": "2026-11-04",
  "firmware_version": "v0.15.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Wi-Fi Defense IDS unit and host integration tests",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "PASS with zero errors across all lifecycle, frame injection, rate tracking, and defense suites.",
  "actual": "Host test suite passed. Verified deauth detection, EAPOL handshake parsing, attack rate alert, and channel mode cycling.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0150-host.txt"
  ],
  "evidence_kind": "Captured output",
  "environment": "Windows",
  "limitations": "Physical RF reception and hardware promiscuous capture not tested in host test."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0203",
  "date": "2026-11-04",
  "firmware_version": "v0.15.0",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "ESP32 firmware build and resource partition check for v0.15.0",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3 and python tools/check_resources.py",
  "expected": "Successful compilation within app0 partition capacity (6,553,600 bytes).",
  "actual": "Build passed cleanly. Flash 1,582,961 bytes (24.2%), RAM 86,088 bytes (26.3%). Partition fit verified.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0150-build.txt",
    "docs/evidence/2026-11-04-v0150-resources.txt"
  ],
  "evidence_kind": "Captured output",
  "environment": "Windows",
  "limitations": "Physical board flash and runtime heap profiling not yet executed."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0204",
  "date": "2026-11-04",
  "firmware_version": "v0.15.0",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Documentation integrity and version synchronization check for v0.15.0",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "Documentation checks PASS with zero errors.",
  "actual": "All required docs, records, anchors, links, and version synchronizations verified cleanly.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v0150-docs.txt"
  ],
  "evidence_kind": "Captured output",
  "environment": "Windows",
  "limitations": "Static cross-reference and schema validation only."
}
```
```json
{
  "record_type": "test",
  "id": "VER-0205",
  "date": "2026-11-05",
  "firmware_version": "v0.16.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Initial channel feature regression",
  "hardware": "None",
  "wiring": "None",
  "procedure": "tools/run_desktop_tests.bat",
  "expected": "Complete the scoped verification; the initial regression is expected to fail before implementation.",
  "actual": "Test failed as expected because WiFiChannelsApp.h did not exist before implementation.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-05-v0160-desktop-red.txt"
  ],
  "evidence_kind": "Captured command output",
  "environment": "Windows / MSVC / PlatformIO",
  "limitations": "Host and desktop outcomes are software evidence. Driver RF reception, UI timing and physical resource headroom are not verified."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0206",
  "date": "2026-11-05",
  "firmware_version": "v0.16.0",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "Initial cross-platform compilation attempts",
  "hardware": "None",
  "wiring": "None",
  "procedure": "Host and desktop compilation, then interactive desktop linking",
  "expected": "Complete the scoped verification; the initial regression is expected to fail before implementation.",
  "actual": "The first implementation used drawRect, absent from both host adapters. Switched to the existing drawRoundRect API. Interactive linking separately exposed missing existing WiFi Defense CMake sources, which were then added.",
  "result": "FAIL",
  "evidence": [
    "docs/evidence/2026-11-05-v0160-host-attempt.txt",
    "docs/evidence/2026-11-05-v0160-desktop-attempt.txt",
    "docs/evidence/2026-11-05-v0160-desktop-link-attempt.txt"
  ],
  "evidence_kind": "Captured command output",
  "environment": "Windows / MSVC / PlatformIO",
  "limitations": "Host and desktop outcomes are software evidence. Driver RF reception, UI timing and physical resource headroom are not verified."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0207",
  "date": "2026-11-05",
  "firmware_version": "v0.16.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Channel summaries, UI lifecycle and regressions",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py; build nibru_simulator and nibru_tests; run nibru_tests and a five-frame headless simulator startup",
  "expected": "Complete the scoped verification; the initial regression is expected to fail before implementation.",
  "actual": "Both host suites passed. Tests covered records beyond twelve and 255, negative RSSI peaks, invalid channels, empty results, refresh reset, radio contention, mode failure, channel touch/swipe and twenty shell cancellation cycles. Interactive simulator startup and clean exit passed.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-05-v0160-host-final.txt",
    "docs/evidence/2026-11-05-v0160-desktop-dense.txt",
    "docs/evidence/2026-11-05-v0160-simulator-startup.txt"
  ],
  "evidence_kind": "Captured command output",
  "environment": "Windows / MSVC / PlatformIO",
  "limitations": "Host and desktop outcomes are software evidence. Driver RF reception, UI timing and physical resource headroom are not verified."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0208",
  "date": "2026-11-05",
  "firmware_version": "v0.16.0",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "Final ESP32 release build",
  "hardware": "None",
  "wiring": "None",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Complete the scoped verification; the initial regression is expected to fail before implementation.",
  "actual": "Final source built successfully with the pinned platform. Existing TFT/core compiler warnings remain.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-05-v0160-build-dense.txt"
  ],
  "evidence_kind": "Captured command output",
  "environment": "Windows / MSVC / PlatformIO",
  "limitations": "Host and desktop outcomes are software evidence. Driver RF reception, UI timing and physical resource headroom are not verified."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0209",
  "date": "2026-11-05",
  "firmware_version": "v0.16.0",
  "commit": "WORKING_TREE",
  "verification_type": "STATIC",
  "scope": "Actual firmware and partition resource comparison",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_resources.py",
  "expected": "Complete the scoped verification; the initial regression is expected to fail before implementation.",
  "actual": "Actual firmware is 1,584,960 B in app0 of 6,553,600 B, leaving 4,968,640 B. The existing growth warning against the accepted baseline remains.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-05-v0160-resources.txt"
  ],
  "evidence_kind": "Captured command output",
  "environment": "Windows / MSVC / PlatformIO",
  "limitations": "Host and desktop outcomes are software evidence. Driver RF reception, UI timing and physical resource headroom are not verified."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0210",
  "date": "2026-11-05",
  "firmware_version": "v0.16.0",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Version synchronization and engineering records",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "Documentation checks pass.",
  "actual": "Documentation checks passed with synchronized v0.16.0 constants and feature, development, test and resource records.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-05-v0160-docs.txt"
  ],
  "evidence_kind": "Captured command output",
  "environment": "Windows / Python",
  "limitations": "Checks document consistency; physical evidence remains separately scoped."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0211",
  "date": "2026-11-05",
  "firmware_version": "v0.17.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Initial BLE Lab regression",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py before implementation",
  "expected": "The new feature test fails before implementation.",
  "actual": "Compilation failed because BleAdvertisingService.h did not exist.",
  "result": "FAIL",
  "evidence": ["docs/evidence/2026-11-05-v0170-host-red.txt"],
  "evidence_kind": "Captured output",
  "environment": "Windows / MSVC",
  "limitations": "Expected RED test."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0212",
  "date": "2026-11-05",
  "firmware_version": "v0.17.0",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "Initial target build",
  "hardware": "None",
  "wiring": "None",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Build with the pinned framework.",
  "actual": "C++11 rejected aggregate initialization with default member initializers. Explicit constructors corrected the event and optional context initialization.",
  "result": "FAIL",
  "evidence": ["docs/evidence/2026-11-05-v0170-build.txt"],
  "evidence_kind": "Captured output",
  "environment": "Windows / PlatformIO",
  "limitations": "Superseded by VER-0214."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0213",
  "date": "2026-11-05",
  "firmware_version": "v0.17.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "BLE Lab and existing host regressions",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "Host tests pass.",
  "actual": "Passed GAP sequencing, 31-byte payloads, LabBLE10 naming, stable rotation without reinit, exclusive ownership, operation timeout/rejection, startup and teardown failures, automatic cleanup retry and twenty app entry/exit cycles.",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-05-v0170-host-verified.txt"],
  "evidence_kind": "Captured output",
  "environment": "Windows / MSVC",
  "limitations": "Injected transport events do not measure RF or physical heap."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0214",
  "date": "2026-11-05",
  "firmware_version": "v0.17.0",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "Final ESP32 build",
  "hardware": "None",
  "wiring": "None",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Build the physical target with existing dependencies.",
  "actual": "Build passed. Existing TFT library warnings remain.",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-05-v0170-build-verified.txt"],
  "evidence_kind": "Captured output",
  "environment": "Windows / PlatformIO / Arduino-ESP32 2.0.14",
  "limitations": "Compilation does not establish receiver discovery or performance."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0215",
  "date": "2026-11-05",
  "firmware_version": "v0.17.0",
  "commit": "WORKING_TREE",
  "verification_type": "STATIC",
  "scope": "Actual image and partition capacity",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_resources.py",
  "expected": "The image fits the generated app0 partition.",
  "actual": "1,588,688 B image fits 6,553,600 B app0 with 4,964,912 B remaining. Existing baseline growth warning retained.",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-05-v0170-resources.txt"],
  "evidence_kind": "Captured output",
  "environment": "Windows / Python",
  "limitations": "Runtime metrics are NOT MEASURED."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0216",
  "date": "2026-11-05",
  "firmware_version": "v0.17.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "BLE Lab desktop integration and simulator verification",
  "hardware": "None",
  "wiring": "None",
  "procedure": "Deferred at owner instruction.",
  "expected": "Resume after the owner requests reactivation.",
  "actual": "Simulator code, registration and execution were not changed or run during the BLE Lab task.",
  "result": "SKIPPED",
  "evidence": ["docs/BLE_LAB.md"],
  "evidence_kind": "Owner instruction in this conversation",
  "environment": "Desktop work paused",
  "limitations": "The new BLE Lab app is registered only on the physical target."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0217",
  "date": "2026-11-05",
  "firmware_version": "v0.17.0",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Version and engineering consistency",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "Documentation checks pass.",
  "actual": "Documentation consistency check passed.",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-05-v0170-docs.txt"],
  "evidence_kind": "Captured output",
  "environment": "Windows / Python",
  "limitations": "Checks consistency only."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0218",
  "date": "2026-11-05",
  "firmware_version": "v0.18.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Pairing payload initial regression",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py before implementing the builders",
  "expected": "The new tests fail before implementation.",
  "actual": "Compilation failed because BleAdvertisingPayload.h did not exist.",
  "result": "FAIL",
  "evidence": ["docs/evidence/2026-11-05-v0180-host-red.txt"],
  "evidence_kind": "Captured output",
  "environment": "Windows / MSVC",
  "limitations": "Expected RED regression."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0219",
  "date": "2026-11-05",
  "firmware_version": "v0.18.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Pairing payloads, UI selection and lifecycle",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "Payload and lifecycle tests pass.",
  "actual": "Passed golden protocol markers, all ten model indices per profile, fifty mixed identities, dynamic Apple fields, variable payload lengths, connectability selection, Windows/All UI selection and existing timeout/cleanup/twenty-cycle regressions.",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-05-v0180-host-final.txt"],
  "evidence_kind": "Captured output",
  "environment": "Windows / MSVC",
  "limitations": "Golden bytes do not establish notification compatibility."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0220",
  "date": "2026-11-05",
  "firmware_version": "v0.18.0",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "ESP32 release build",
  "hardware": "None",
  "wiring": "None",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Build the pinned physical target.",
  "actual": "Build passed with existing library warnings.",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-05-v0180-build.txt"],
  "evidence_kind": "Captured output",
  "environment": "Windows / PlatformIO",
  "limitations": "Physical receiver and runtime resources remain unverified."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0221",
  "date": "2026-11-05",
  "firmware_version": "v0.18.0",
  "commit": "WORKING_TREE",
  "verification_type": "STATIC",
  "scope": "Actual image and generated partition fit",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_resources.py",
  "expected": "The image fits app0.",
  "actual": "1,589,984 B image fits app0 with 4,963,616 B remaining. Existing baseline growth warning retained.",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-05-v0180-resources.txt"],
  "evidence_kind": "Captured output",
  "environment": "Windows / Python",
  "limitations": "No runtime resource inference."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0222",
  "date": "2026-11-05",
  "firmware_version": "v0.18.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Pairing profile simulator integration",
  "hardware": "None",
  "wiring": "None",
  "procedure": "Deferred at the owner request.",
  "expected": "Resume only on explicit owner instruction.",
  "actual": "Simulator registration, code and execution remain paused.",
  "result": "SKIPPED",
  "evidence": ["docs/BLE_LAB.md"],
  "evidence_kind": "Owner instruction in this conversation",
  "environment": "Desktop paused",
  "limitations": "New profiles are validated through host tests and the physical target build."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0223",
  "date": "2026-11-05",
  "firmware_version": "v0.18.0",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Version and engineering consistency",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "Documentation checks pass.",
  "actual": "Documentation checks passed.",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-05-v0180-docs.txt"],
  "evidence_kind": "Captured output",
  "environment": "Windows / Python",
  "limitations": "Checks consistency only."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0224",
  "date": "2026-11-05",
  "firmware_version": "v0.19.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Target picker and Random initial regression",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py before implementation",
  "expected": "New tests fail before the feature exists.",
  "actual": "Compilation failed on the missing Random profile and target-picker API.",
  "result": "FAIL",
  "evidence": ["docs/evidence/2026-11-05-v0190-host-red.txt"],
  "evidence_kind": "Captured output",
  "environment": "Windows / MSVC",
  "limitations": "Expected RED test."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0225",
  "date": "2026-11-05",
  "firmware_version": "v0.19.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Picker, Random and existing regressions",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "Host tests pass.",
  "actual": "Passed Windows/All/Random selection, opening-contact suppression, Back, BOOT next/activate, random payload bounds and fresh addresses, plus existing operation failure and twenty-cycle tests.",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-05-v0190-host.txt"],
  "evidence_kind": "Captured output",
  "environment": "Windows / MSVC",
  "limitations": "Physical touch and Random receiver behavior remain pending."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0226",
  "date": "2026-11-05",
  "firmware_version": "v0.19.0",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "ESP32 target build",
  "hardware": "None",
  "wiring": "None",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Build with pinned dependencies.",
  "actual": "Build passed and the image contains the v0.19.0 boot marker.",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-05-v0190-build.txt"],
  "evidence_kind": "Captured output and image marker inspection",
  "environment": "Windows / PlatformIO",
  "limitations": "Compilation does not establish physical behavior."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0227",
  "date": "2026-11-05",
  "firmware_version": "v0.19.0",
  "commit": "WORKING_TREE",
  "verification_type": "STATIC",
  "scope": "Image and partition capacity",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_resources.py",
  "expected": "The actual image fits app0.",
  "actual": "1,590,992 B image fits with 4,962,608 B remaining. Existing accepted-baseline growth warning retained.",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-05-v0190-resources.txt"],
  "evidence_kind": "Captured output",
  "environment": "Windows / Python",
  "limitations": "Runtime acceptance is separate."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0228",
  "date": "2026-11-05",
  "firmware_version": "v0.19.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Target picker desktop integration",
  "hardware": "None",
  "wiring": "None",
  "procedure": "Deferred at the owner instruction.",
  "expected": "Resume on explicit owner request.",
  "actual": "Simulator code, registration and execution remain paused.",
  "result": "SKIPPED",
  "evidence": ["docs/BLE_LAB.md"],
  "evidence_kind": "Owner instruction in this conversation",
  "environment": "Desktop paused",
  "limitations": "Host tests and ESP32 build cover the current change."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0229",
  "date": "2026-11-05",
  "firmware_version": "v0.19.0",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Engineering and version consistency",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "Documentation checks pass.",
  "actual": "Documentation checks passed.",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-05-v0190-docs.txt"],
  "evidence_kind": "Captured output",
  "environment": "Windows / Python",
  "limitations": "Consistency only."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0230",
  "date": "2026-11-05",
  "firmware_version": "v0.20.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "BLE payload builders, target paging and lifecycle",
  "hardware": "None (host MSVC tests with fake transport)",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "Existing payload tests continue to pass; Apple Action, Not Your Device, Beacon, page-two selection and cleanup assertions pass.",
  "actual": "All host suites passed, including 'Bruce pairing, Apple action, Not Your Device and beacon payload formats' and 'BLE Lab GAP sequence, payload, timeouts, cleanup and 20 host cycles'.",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-05-v0200-host.txt"],
  "evidence_kind": "Captured MSVC host-test output",
  "environment": "Windows host; UNIT_TEST stubs",
  "limitations": "Fake GAP transport; does not prove physical RF behavior, heap or latency."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0231",
  "date": "2026-11-05",
  "firmware_version": "v0.20.0",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "ESP32-S3 target build and app0 capacity",
  "hardware": "None",
  "wiring": "None",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Target compiles, links and fits the generated app0 partition.",
  "actual": "SUCCESS. firmware.bin 1,592,096 B of 6,553,600 B app0, 4,961,504 B remaining, sha256 6dcaeb1827f6abd6bcef5c3489aeafe9a477cfb1c76ad40818f69ee2603a94fb. Static RAM 86,264 B (26.3%). The provisional >5% image-growth warning is retained.",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-05-v0200-build.txt"],
  "evidence_kind": "Captured PlatformIO output and resource report",
  "environment": "PlatformIO 6.2.0; espressif32@6.5.0; Arduino-ESP32 2.0.14",
  "limitations": "Build only; no upload, runtime heap, latency or power measurement."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0232",
  "date": "2026-11-05",
  "firmware_version": "v0.20.0",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Engineering and version consistency",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "Documentation checks pass with synchronized v0.20.0 records.",
  "actual": "Documentation checks passed.",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-05-v0200-docs.txt"],
  "evidence_kind": "Captured output",
  "environment": "Windows / Python",
  "limitations": "Consistency only."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0233",
  "date": "2026-11-05",
  "firmware_version": "v0.20.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Desktop simulator coverage for the new BLE profiles",
  "hardware": "None",
  "wiring": "None",
  "procedure": "tools/run_desktop_tests.bat",
  "expected": "Simulator mirrors the new BLE targets.",
  "actual": "Not executed; BLE Lab is not registered in the desktop simulator and simulator work remains paused by the owner.",
  "result": "SKIPPED",
  "evidence": ["docs/BLE_LAB.md"],
  "evidence_kind": "Owner decision recorded in project documentation",
  "environment": "Windows desktop target",
  "limitations": "Simulator coverage is absent by decision, not by test failure."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0234",
  "date": "2026-11-05",
  "firmware_version": "v0.21.0",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "ESP32-S3 compilation with SecurityLabApp and consolidated lab suite",
  "hardware": "LILYGO T-Display-S3 Touch Glass (target toolchain)",
  "wiring": "Default parallel ST7789 + CST816S pin definitions",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Build finishes with code 0 and fits in app0 partition.",
  "actual": "Flash 1,593,409 B (24.3%), RAM 86,312 B (26.3%), code 0.",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-05-v0210-build.txt"],
  "evidence_kind": "Captured compiler output",
  "environment": "Windows / PlatformIO espressif32@6.5.0",
  "limitations": "Does not prove physical display or radio execution."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0235",
  "date": "2026-11-05",
  "firmware_version": "v0.21.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Security Lab Hub navigation, card selection, touch opening, and lifecycle cleanup",
  "hardware": "None",
  "wiring": "None",
  "procedure": "tools/run_desktop_tests.bat",
  "expected": "All 26 desktop units pass including Security Lab Hub tests.",
  "actual": "ALL DESKTOP SIMULATOR TESTS PASSED SUCCESSFULLY.",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-05-v0210-desktop.txt"],
  "evidence_kind": "Automated unit test execution",
  "environment": "Windows / MSVC x64 Native",
  "limitations": "Host simulation only."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0236",
  "date": "2026-11-06",
  "firmware_version": "v0.22.0",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "ESP32-S3 compilation with Settings v0.1 consolidation, version bump to v0.22.0, and PWM compilation warning cleanup",
  "hardware": "LILYGO T-Display-S3 Touch Glass (target toolchain)",
  "wiring": "Default parallel ST7789 + CST816S pin definitions",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Build finishes with code 0 and fits in app0 partition.",
  "actual": "Flash 1,593,385 B (24.3%), RAM 86,312 B (26.3%), code 0.",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-06-v0220-build.txt"],
  "evidence_kind": "Captured compiler output",
  "environment": "Windows / PlatformIO espressif32@6.5.0",
  "limitations": "Does not prove physical display or radio execution."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0237",
  "date": "2026-11-06",
  "firmware_version": "v0.22.0",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "SettingsApp navigation, subviews (Display, Theme, Memory, WiFiDiag, Device), modal confirmation, and lifecycle exit",
  "hardware": "None",
  "wiring": "None",
  "procedure": "tools/run_desktop_tests.bat",
  "expected": "All desktop test cases pass including SettingsApp navigation tests.",
  "actual": "ALL DESKTOP SIMULATOR TESTS PASSED SUCCESSFULLY.",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-06-v0220-desktop.txt"],
  "evidence_kind": "Automated unit test execution",
  "environment": "Windows / MSVC x64 Native",
  "limitations": "Host simulation only."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0238",
  "date": "2026-11-06",
  "firmware_version": "v0.22.0",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Repository consistency and documentation validation",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "All documentation checks pass with 0 errors.",
  "actual": "Documentation checks: PASS",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-06-v0220-docs.txt"],
  "evidence_kind": "Automated documentation validation tool",
  "environment": "Windows / Python 3.12",
  "limitations": "Validates schema and references only."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0239",
  "date": "2026-11-06",
  "firmware_version": "v0.23.0",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "Compilation of CryptoService SHA-512, TotpService RFC 4226 HOTP, and multi-algo dynamic truncation",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Build succeeds without errors; mbedTLS SHA-512 and HOTP compile cleanly.",
  "actual": "Compilation succeeded with code 0. Flash: 1,595,641 B (24.3%), RAM: 87,128 B (26.6%).",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-06-v0230-pio-build-crypto.txt"],
  "evidence_kind": "PlatformIO build log",
  "environment": "PlatformIO 6.1.18 / espressif32@6.5.0 / Arduino-ESP32 2.0.14",
  "limitations": "Does not prove physical display or radio execution."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0240",
  "date": "2026-11-06",
  "firmware_version": "v0.23.0",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "Compilation of WebPortalService Authenticator Control Panel single-page application and REST API endpoints",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Full firmware compiles and links within partition capacity with zero errors.",
  "actual": "PlatformIO ESP32-S3 build succeeded at 1,608,208 B Flash (24.5% of 6.25 MB partition, 4,945,392 B free capacity) and 87,128 B RAM (26.6%).",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-06-v0230-pio-build-webportal.txt"],
  "evidence_kind": "PlatformIO build log",
  "environment": "PlatformIO 6.1.18 / espressif32@6.5.0 / Arduino-ESP32 2.0.14",
  "limitations": "Desktop simulator paused per explicit user directive; hardware RF validation pending."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0241",
  "date": "2026-11-06",
  "firmware_version": "v0.23.1",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "Compilation of UsbHidService transport readiness fix and TotpApp primary USB send gestures",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Build succeeds without errors; TinyUSB native transport compiles cleanly.",
  "actual": "Compilation succeeded with code 0. Flash: 1,607,361 B (24.5%), RAM: 87,128 B (26.6%).",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-06-v0231-pio-build.txt"],
  "evidence_kind": "PlatformIO build log",
  "environment": "PlatformIO 6.1.18 / espressif32@6.5.0 / Arduino-ESP32 2.0.14",
  "limitations": "Does not prove physical display or radio execution."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0242",
  "date": "2026-11-06",
  "firmware_version": "v0.23.2",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "Compilation of WebPortalService BLE coexistence suspension, automatic restoration, and SoftAP verification",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Build succeeds without errors; WebPortalService and SettingsApp compile cleanly with full resource tracking.",
  "actual": "Compilation succeeded with code 0. Flash: 1,607,753 B (24.5%), RAM: 87,144 B (26.6%). Binary image 1,608,112 B with verified SHA-256.",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-06-v0232-pio-build.txt"],
  "evidence_kind": "PlatformIO build log and resource artifact check",
  "environment": "PlatformIO 6.1.18 / espressif32@6.5.0 / Arduino-ESP32 2.0.14",
  "limitations": "Build verification only; physical flash upload tracked in HWT-0090."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0243",
  "date": "2026-11-06",
  "firmware_version": "v0.23.3",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "Compilation of CryptoService bootloader SAR entropy cleanup and runtime RNG safety",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Build succeeds without errors; CryptoService compiles with standard esp_fill_random().",
  "actual": "Compilation succeeded with code 0. Flash: 1,606,877 B (24.5%), RAM: 87,144 B (26.6%). Binary image 1,607,248 B with verified SHA-256.",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-06-v0233-pio-build.txt"],
  "evidence_kind": "PlatformIO build log and resource artifact check",
  "environment": "PlatformIO 6.1.18 / espressif32@6.5.0 / Arduino-ESP32 2.0.14",
  "limitations": "Build verification only; physical flash upload tracked in HWT-0091."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0244",
  "date": "2026-11-06",
  "firmware_version": "v0.23.4",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "Compilation of Web Portal display keep-alive and sleep lock prevention in v0.23.4",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Build succeeds without errors; WebPortalService and SettingsApp compile with display keep-alive notifications.",
  "actual": "Compilation succeeded with code 0. Flash: 1,606,893 B (24.5%), RAM: 87,144 B (26.6%). Binary image 1,607,264 B with verified SHA-256.",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-06-v0234-pio-build.txt"],
  "evidence_kind": "PlatformIO build log and resource artifact check",
  "environment": "PlatformIO 6.1.18 / espressif32@6.5.0 / Arduino-ESP32 2.0.14",
  "limitations": "Build verification only; physical flash upload tracked in HWT-0092."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0245",
  "date": "2026-11-06",
  "firmware_version": "v0.23.5",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "Compilation of flicker-free differential rendering for Clock Screensaver and ClockApp in v0.23.5",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Build succeeds without errors; ClockApp compiles with differential rendering cache and reset logic.",
  "actual": "Compilation succeeded with code 0. Flash: 1,607,749 B (24.5%), RAM: 87,144 B (26.6%). Binary image 1,608,112 B with verified SHA-256.",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-06-v0235-pio-build.txt"],
  "evidence_kind": "PlatformIO build log and resource artifact check",
  "environment": "PlatformIO 6.1.18 / espressif32@6.5.0 / Arduino-ESP32 2.0.14",
  "limitations": "Build verification only; physical flash upload tracked in HWT-0093."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0246",
  "date": "2026-11-06",
  "firmware_version": "v0.23.6",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "Compilation of continuous background Web Portal broadcast during screensaver in v0.23.6",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Build succeeds without errors; WebPortalService and ClockApp compile with background SoftAP persistence and screensaver portal indicators.",
  "actual": "Compilation succeeded with code 0. Flash: 1,607,977 B (24.5%), RAM: 87,160 B (26.6%). Binary image 1,608,336 B with verified SHA-256.",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-06-v0236-pio-build.txt"],
  "evidence_kind": "PlatformIO build log and resource artifact check",
  "environment": "PlatformIO 6.1.18 / espressif32@6.5.0 / Arduino-ESP32 2.0.14",
  "limitations": "Build verification only; physical flash upload tracked in HWT-0094."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0247",
  "date": "2026-11-06",
  "firmware_version": "v0.23.7",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "Compilation of clarified System Info Boot Cause telemetry label in v0.23.7",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Build succeeds without errors; SettingsApp compiles with Boot Cause label.",
  "actual": "Compilation succeeded with code 0. Flash: 1,607,977 B (24.5%), RAM: 87,160 B (26.6%). Binary image 1,608,336 B with verified SHA-256.",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-06-v0237-pio-build.txt"],
  "evidence_kind": "PlatformIO build log and resource artifact check",
  "environment": "PlatformIO 6.1.18 / espressif32@6.5.0 / Arduino-ESP32 2.0.14",
  "limitations": "Build verification only; physical flash upload tracked in HWT-0095."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0248",
  "date": "2026-11-06",
  "firmware_version": "v0.23.8",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "Compilation of Gzip Web Portal streaming and crash elimination in v0.23.8",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Build succeeds without errors; WebPortalService compiles with Gzip streaming and 16KB loop stack.",
  "actual": "Compilation succeeded with code 0. Flash: 1,583,669 B (24.2%), RAM: 90,240 B (27.5%). Binary image 1,584,032 B with verified SHA-256.",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-06-v0238-pio-build.txt"],
  "evidence_kind": "PlatformIO build log and resource artifact check",
  "environment": "PlatformIO 6.1.18 / espressif32@6.5.0 / Arduino-ESP32 2.0.14",
  "limitations": "Build verification only; physical flash upload tracked in HWT-0096."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0249",
  "date": "2026-11-06",
  "firmware_version": "v0.23.9",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "Compilation of official Nibru visual identity and branding in Web Portal for v0.23.9",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "Build succeeds without errors; WebPortalService compiles with updated Gzip PROGMEM payload.",
  "actual": "Compilation succeeded with code 0. Flash: 1,587,941 B (24.2%), RAM: 90,240 B (27.5%). Binary image 1,588,304 B with verified SHA-256.",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-06-v0239-pio-build.txt"],
  "evidence_kind": "PlatformIO build log and resource artifact check",
  "environment": "PlatformIO 6.1.18 / espressif32@6.5.0 / Arduino-ESP32 2.0.14",
  "limitations": "Build verification only; physical flash upload tracked in HWT-0097."
}
```

```json
{
  "record_type": "test",
  "id": "VER-0250",
  "date": "2026-11-06",
  "firmware_version": "v0.23.9",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Documentation check for v0.23.9",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": ["docs/evidence/2026-11-06-v0239-docs-check.txt"],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / Python 3.12",
  "limitations": "Scope limited to documentation schema"
}
```
