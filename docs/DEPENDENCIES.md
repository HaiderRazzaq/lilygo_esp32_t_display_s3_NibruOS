# Dependency and build baseline

Observed 2026-11-01; configuration is [platformio.ini](../platformio.ini).
Framework/library pins are unchanged. Settings completion adds two opt-in build environments and a post-image resource checker; see the current report.

| Component | Configured | Locally resolved evidence |
|---|---|---|
| PlatformIO Core | Not pinned by repo | 6.2.0 on Windows; current session finds pio.exe through PATH |
| Platform | `espressif32@6.5.0` | Build selects Espressif 32 6.5.0, not the other globally installed pioarduino platform |
| Board / framework | `lilygo-t-display-s3`, `arduino` | ESP32-S3, 240 MHz; Arduino-ESP32 2.0.14 |
| Framework package | Selected by platform | `framework-arduinoespressif32@3.20014.231204` |
| Xtensa / RISC-V toolchains | Selected by platform | `8.4.0+2021r2-patch5` |
| esptool | Selected by platform | `tool-esptoolpy@1.40501.0`, esptool 4.5.1 |
| TFT_eSPI | `bodmer/TFT_eSPI@2.5.43` | 2.5.43; project `USER_SETUP_LOADED` build flags |
| CST816S | Git URL, **not commit-pinned** | library 1.3.0, SHA `11fe1fbae1f226c328a9b08d808fb8f97d8a4c50` in local checkout |
| Arduino FS / SPIFFS / Wire / WiFi | Bundled with core | Build dependency graph 2.0.0; FS/SPIFFS linkage is not app persistence |
| Python documentation tooling | Standard library, Python 3.10+ | Current run Python 3.14; standard library only |
| Host tests | Bash + desktop `c++`, C++17 | Current run: Zig 0.16.0 C++ on Windows, installed only in ignored .testbuild/toolchain; Bash/WSL has no distribution |

## Settings and limits

- `board_build.arduino.memory_type=qio_opi`, `BOARD_HAS_PSRAM`.
- `board_build.partitions=default_16MB.csv`: core's installed table has NVS
  at 0x9000/0x5000; OTA data 0xe000/0x2000; app0 0x10000/0x640000;
  app1 0x650000/0x640000; SPIFFS 0xc90000/0x360000;
  coredump 0xff0000/0x10000. No project OTA/filesystem service is implemented.
- `ARDUINO_USB_MODE=1`, `ARDUINO_USB_CDC_ON_BOOT=1`; no wait-for-Serial loop
  in main. Battery-only USB behavior still needs actual testing.
- TFT: ST7789, native 170x320, 8-bit parallel, inversion enabled, RGB order,
  INIT_SEQUENCE_3 and CGRAM offset. Exact pins are in
  [pinout](HARDWARE_PINOUT.md); fonts and all defines stay in platformio.ini.
- LDF `deep+`, monitor 115200 baud, upload speed 460800.

The unpinned CST816S URL can resolve differently on another computer (BUG-0002).
Record a verified SHA when deliberately pinning it; do not silently update the
core/library to get a build. The resolved SHA here is observational, not a new
configuration lock. Target build diagnostics and memory usage:
[VER-0001](VERIFICATION_REPORT.md), [captured output](evidence/2026-11-01-target-build.txt).

TFT_eSPI warns that parallel mode has no DMA and its own TOUCH_CS functions are
disabled. This project uses CST816S over I2C; do not add an arbitrary TOUCH_CS to
silence that warning. Physical compatibility is still not proven by compilation.

Host compiler setup is development-only; no firmware dependency was added.
The Python host runner accepts CXX; POSIX CI still uses c++. Current evidence: VER-0017.

## Authenticator test tools
No embedded dependency/framework upgrade. Optional HID uses existing Arduino
TinyUSB. This Windows session installed test-only Zig0.15.2 and official SDL2
2.30.10 headers/x64 import library under ignored .testbuild; neither is committed.
Host/simulator crypto uses Windows CNG/bcrypt; non-Windows synthetic doubles
remain explicit. Python desktop fallback installs/downloads nothing.
See [authenticator](AUTHENTICATOR.md).

Account setup adds no firmware dependency. The local helper uses Python 3
standard library, browser WebCrypto and optional Web Serial. Node v24.8.0
is test-only in this environment; tests/tools browser-module tests use built-in
node:test and public fixtures. No npm package/CDN, browser extension, camera or
external API is required.
