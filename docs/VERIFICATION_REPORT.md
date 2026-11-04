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
