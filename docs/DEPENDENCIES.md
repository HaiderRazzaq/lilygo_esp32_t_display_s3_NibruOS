# Dependency and build baseline

Observed 2026-11-01; configuration is [platformio.ini](../platformio.ini).
No dependency versions or flags were changed for this documentation work.

| Component | Configured | Locally resolved evidence |
|---|---|---|
| PlatformIO Core | Not pinned by repo | 6.2.0 on Windows; installed `pio.exe` is outside PATH |
| Platform | `espressif32@6.5.0` | Build selects Espressif 32 6.5.0, not the other globally installed pioarduino platform |
| Board / framework | `lilygo-t-display-s3`, `arduino` | ESP32-S3, 240 MHz; Arduino-ESP32 2.0.14 |
| Framework package | Selected by platform | `framework-arduinoespressif32@3.20014.231204` |
| Xtensa / RISC-V toolchains | Selected by platform | `8.4.0+2021r2-patch5` |
| esptool | Selected by platform | `tool-esptoolpy@1.40501.0`, esptool 4.5.1 |
| TFT_eSPI | `bodmer/TFT_eSPI@2.5.43` | 2.5.43; project `USER_SETUP_LOADED` build flags |
| CST816S | Git URL, **not commit-pinned** | library 1.3.0, SHA `11fe1fbae1f226c328a9b08d808fb8f97d8a4c50` in local checkout |
| Arduino FS / SPIFFS / Wire / WiFi | Bundled with core | Build dependency graph 2.0.0; FS/SPIFFS linkage is not app persistence |
| Python documentation tooling | Standard library, Python 3.10+ | Run on Python 3.12; no third-party runtime dependencies |
| Host tests | Bash + desktop `c++`, C++17 | Git Bash available; desktop compiler missing this session |

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
