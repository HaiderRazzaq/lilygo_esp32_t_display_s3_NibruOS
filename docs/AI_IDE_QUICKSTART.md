# Open NibruOS in any AI coding IDE

**Supported workflow:** unzip the archive and use the **entire repository root** (the directory with `platformio.ini`) as the IDE workspace. Do not attach only `main.cpp` or paste only the sketch: the platform flags, shared managers, headers, app lifecycle and tests are essential.

## For Cursor, Windsurf, VS Code Copilot, Claude Code, Codex or similar

1. Extract archive. Open `NibruOS-v0.1/` itself as a project.
2. Tell the agent to **first** read `AGENTS.md`, `docs/PROJECT_CONTEXT.md`, `docs/HARDWARE_PINOUT.md`, `docs/ARCHITECTURE.md`, `docs/ADDING_AN_APP.md`, `docs/WIFI_DIAGNOSTICS.md`, and `platformio.ini`, and to inspect source code before editing.
3. Start with a **read-only architecture report and GPIO conflict audit**. Then ask for one new app or feature at a time, with an implementation plan and tests.
4. Ask it to run `pio run -e lilygo-t-display-s3` on YOUR computer after installing PlatformIO. Require reported output rather than claiming success.
5. When an app needs hardware, supply the sensor/module model and its exact intended 3.3V pins. Have the agent check wiring, pull-ups, voltage levels, current and reserved pins **before coding**.
6. Keep the `AGENTS.md` and both device/source docs inside version control for future IDE sessions. Commit after each tested milestone.

## Copy-paste first prompt (English)

> You are the firmware architect for **NibruOS**, a modular ESP32-S3 project targeting the **LILYGO T-Display-S3 Touch Glass** with a 320x170 ST7789V screen and CST816S touch. Before making changes, inspect the ENTIRE repository and read `AGENTS.md`, `docs/PROJECT_CONTEXT.md`, `docs/HARDWARE_PINOUT.md`, `docs/WIFI_DIAGNOSTICS.md`, `docs/ARCHITECTURE.md`, `docs/ADDING_AN_APP.md`, and `platformio.ini`. Use the actual source as the source of truth. Preserve all user-verified TFT/touch/button pin settings and the 320x170 coordinate mapping. Explain the architecture, dependencies, app lifecycle, resource ownership, known Wi-Fi timeout incident, hardware GPIO reservations, and build/test workflow. Then wait for my specific feature request. For any change, investigate the affected flow, avoid blocking the UI, keep one common hardware/service owner, update host tests and documentation, run PlatformIO Build when possible, and report any untested assumptions. Never claim hardware success without my physical-board test. Do not change the pinout or flash layout without a documented reason.

## Add a new app: suggested prompt

> Add a new modular application named **[APP NAME]** for **[PURPOSE]**. Follow all repository instructions and current conventions. Review the complete pin map, determine whether extra hardware/modules are required, explain risks and resource conflicts, then implement the smallest safe App-class addition with reusable services, correct `onEnter/onExit` ownership and non-blocking updates. Wire it into the launcher, add a host test, update docs, and provide build/board test instructions. Do not modify known-good TFT/touch definitions without evidence.

## Specific Wi-Fi troubleshooting prompt

> Diagnose the NibruOS Wi-Fi Scanner timeout. Read `docs/WIFI_DIAGNOSTICS.md`, `src/services/WiFiService.*`, `src/apps/WiFiScannerApp.*`, host tests, and platformio.ini. Here is my 115200 serial log: **[PASTE]**. Determine the failing stage and explain evidence. Compare Arduino-ESP32 core APIs to the pinned platform version. Make a minimal source change with a reproducible test, do not just increase timeout, and explain what still needs physical verification.
