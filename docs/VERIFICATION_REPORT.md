# NibruOS v0.1 — verification report

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
