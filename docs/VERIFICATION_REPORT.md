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
