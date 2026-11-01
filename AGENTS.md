# NibruOS — Instructions for ANY AI Coding Agent

**READ THIS FILE FIRST**. Then read [project context](docs/PROJECT_CONTEXT.md), [current state](docs/CURRENT_STATE.md), [pinout](docs/HARDWARE_PINOUT.md), [architecture](docs/ARCHITECTURE.md), [Wi-Fi diagnostics](docs/WIFI_DIAGNOSTICS.md), and inspect `platformio.ini` and the relevant code BEFORE editing. Read affected feature/issue entries and [engineering workflow](docs/ENGINEERING_WORKFLOW.md). This is an ESP32-S3 firmware project, **not** a Linux OS. There is exactly one combined Arduino/PlatformIO firmware with multiple in-process apps.

## Device and non-negotiable tested baseline
- Target **LILYGO T-Display-S3 Touch Glass**, ESP32-S3R8, **16 MB flash**, **8 MB PSRAM**; ST7789V 170x320 8-bit parallel TFT and **CST816S** capacitive touch.
- **Observed working on user's device in an earlier independent sketch**: `SDA GPIO18`, `SCL GPIO17`, `touch reset GPIO21`, `touch interrupt GPIO16`; reset low 10ms / high, wait 50ms, `touch.begin(FALLING)`. `GPIO15 HIGH` powers the display/peripherals, `GPIO38 HIGH` enables backlight; `tft.init(); tft.setRotation(1)` => 320x170 landscape. Touch transform `x=rawY`, `y=169-rawX` (clamp to bounds). Buttons: BOOT GPIO0 and user GPIO14, active LOW `INPUT_PULLUP`.
- The user's **pinout image has a conflicting SDA/SCL label in its bottom-right caption**; LilyGO's official pin table and the user's WORKING CODE both confirm SDA18 / SCL17. DO NOT swap them to follow that caption.
- **Do not** reassign LCD pins: D0..D7=39,40,41,42,45,46,47,48; CS6, DC7, RST5, WR8, RD9, BL38, PWR15.
- Do not treat GPIO16/17/18/21 as freely available on the Touch variant; 17/18 are shared I2C and may be extended only after checking bus compatibility.
- Diagram/image copied to `docs/assets/T-Display-S3-user-pinout.png`. Complete pin reservations and exposed contacts in `docs/HARDWARE_PINOUT.md`.

## Architecture and editing rules
- Entry: `src/main.cpp`. App lifecycle: `src/core/App.h`, manager `src/core/AppManager.*`, shared context `src/core/AppContext.h`. All apps are in `src/apps/`.
- Hardware is accessed by drivers in `src/hardware/`; shared Wi-Fi owned through `ResourceManager` (`ResourceOwner::WiFiScanner`). Avoid direct Wi-Fi/display/touch initialization in individual apps.
- **Non-blocking** `onTick()` and touch events. Never run multi-second while-loops or synchronous network scans on the UI event loop. `onExit()` must release acquired resources; a Back button must remain responsive.
- Add a feature via the `App` interface; register it in `main.cpp`; keep app-local state; use shared managers; document every extra GPIO. Consult `docs/ADDING_AN_APP.md`.
- Do not refactor the verified TFT_eSPI settings or CST816S initialization merely because another library example differs. Reproduce on physical hardware before claiming compatibility.
- Wi-Fi scanner received a timeout fix (delayed STA initialization, bounded async scan, explicit driver stop, one automatic retry, serial logs). **This fix has host-test coverage but has NOT been proven on real board**; see `docs/WIFI_DIAGNOSTICS.md`.
- Record what you changed and what you tested. Do not claim successful board flashing, Wi-Fi reception or PlatformIO compilation unless actually run in that environment.
- Never commit passwords, API keys, TOTP seeds or production account data. Previous Dr. Passwords sketch is only a hardware-behavior reference, not a secure vault to embed unchanged. Avoid destructive flash erases without a data backup.

## Build and validation
- `python tools/check_docs.py` (required documentation consistency checks)
- `python -m unittest discover -s tests/docs -v` (when documentation tooling changes)
- `pio run -e lilygo-t-display-s3`
- `pio run -e lilygo-t-display-s3 -t upload --upload-port COM7` (replace COM port)
- `pio device monitor -p COM7 -b 115200`
- On a host with g++/bash: `bash tests/host/run_tests.sh` (mock unit/smoke tests; **not** hardware tests).
- PlatformIO pins from `platformio.ini`: `espressif32@6.5.0`, Arduino-ESP32 2.0.14, TFT_eSPI 2.5.43, CST816S via its GitHub repository. Do not silently upgrade the framework or rewrite the board manifest.

## Standard workflow for each requested change
1. Inspect the affected app, lifecycle, service, drivers, configuration, documentation and test stubs.
2. Determine GPIO clashes, memory budget, network/peripheral ownership, and return-to-home behavior.
3. Make the smallest working modification; keep 320x170 screen constraints, button escape and responsiveness.
4. Add/update host tests; perform actual PlatformIO build when tools/dependencies are available.
5. Report unverified behaviors; provide hardware test steps and update project docs.

## Permanent engineering memory and Definition of Done

- Use [docs/README.md](docs/README.md) to find each document's responsibility.
  Inspect code before asserting that a feature exists. See the [registry](docs/FEATURE_REGISTRY.md)
  and [issues](docs/KNOWN_ISSUES.md); reuse stable FEAT/BUG IDs or allocate new ones.
- Before coding, establish a short implementation/verification plan and inspect
  GPIO, bus, voltage/current, memory and shared-service dependencies. Preserve
  verified behavior, architecture, Back escape, and resource cleanup.
- For every meaningful change, update feature/issue status, append a
  [development record](docs/DEVELOPMENT_LOG.md), update [current state](docs/CURRENT_STATE.md)
  and [Unreleased changelog](CHANGELOG.md) as appropriate. Track Added/Changed/
  Fixed/Removed/Deprecated/Security changes; retain removed feature identities,
  dates and reasons. Record hardware/dependency/behavior changes in their owning
  documents and use ADRs for major decisions.
- Record successful, failed, blocked, skipped and not-run tests separately,
  including type, evidence, environment and limitations. Hardware outcomes go
  in [HARDWARE_TEST_LOG](docs/HARDWARE_TEST_LOG.md); software evidence goes in
  [VERIFICATION_REPORT](docs/VERIFICATION_REPORT.md); maintain the [matrix](docs/TEST_MATRIX.md).
  Never equate build/mock PASS, a commit named stable, or an older reference
  sketch with physical validation of the current firmware. No fabricated PASS.
- Logs are append-only: never erase failed attempts, regressions, reverts,
  removals or historical test outcomes. Append clearly marked corrections.
  Git complements these records; do not rewrite history or create commits
  without authorization. Sanitize diagnostic evidence; do not store secrets.
- A task is done only when implementation, appropriate verification and required
  documentation updates are addressed and `python tools/check_docs.py` passes.
  If verification is impossible, state **IMPLEMENTED, verification incomplete**,
  record the blocker and provide reproducible next tests. Never claim verified
  resolution, compilation, flashing or reception without actual evidence.

## Mandatory Memory Profiling & Resource Budget Policy

**CRITICAL: Every code change with possible runtime, memory, storage, timing, peripheral, or power impact MUST undergo resource impact analysis. This is a permanent project-wide engineering policy for ALL future modifications.**

Every future AI agent must:
1. **Inspect resource budgets** in [RESOURCE_BUDGET.md](docs/RESOURCE_BUDGET.md) and current metrics in [performance/BASELINE.md](docs/performance/BASELINE.md) before making relevant changes.
2. **Measure or establish a baseline** when tools and device access are available.
3. **Evaluate impact** on Flash, Internal SRAM, PSRAM, FreeRTOS stacks, UI responsiveness, peripherals, and power.
4. **Follow the required profiling scenarios** in [MEMORY_PROFILING.md](docs/MEMORY_PROFILING.md):
   - Scenario A: Startup
   - Scenario B: Application Entry
   - Scenario C: Application Exit
   - Scenario D: Repeated Lifecycle Testing (20 cycles minimum)
   - Scenario E: Combined Workloads
5. **Enforce Resource Budget Controls**:
   - Firmware image MUST fit within the active application partition (`app0` = 6,553,600 bytes [6.25 MB]).
   - Maintain internal SRAM headroom (>= 150 KB free heap).
   - Flag provisional review triggers: Flash growth > 5%, free heap drop > 10%, largest allocatable block drop > 10%, or repeatable latency degradation.
6. **Produce a Resource Impact Report** for every resource-impacting change following the standard format in [RESOURCE_BUDGET.md](docs/RESOURCE_BUDGET.md).
7. **Clean up shared resources** on `onExit()`: cancel active jobs, release buffers, unregister callbacks, and release `ResourceManager` locks.
8. **Never claim hardware validation without physical evidence**: clearly distinguish compile-time results, host mocks, and physical measurements.

## Source of truth
Actual source code > explanatory documents if they diverge; user-verified hardware wiring > contradictory image labels. External validation sources: https://github.com/Xinyuan-LilyGO/T-Display-S3 , https://wiki.lilygo.cc/products/t-display-series/t-display-s3/ , https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html .
