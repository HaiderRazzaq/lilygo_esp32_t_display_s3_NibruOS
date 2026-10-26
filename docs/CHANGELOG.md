# Changes

## v0.1.1-source-patch (2026-10-26)
- Fixed Wi-Fi scanner lifecycle: asynchronous station warmup, completion polling precedence, explicit underlying scan cancellation, one radio restart/retry, more informative scan statuses.
- Kept user-verified TFT_eSPI and CST816S wiring and original display/touch init unchanged.
- Added `AGENTS.md`, `docs/PROJECT_CONTEXT.md`, `docs/HARDWARE_PINOUT.md`, `docs/AI_IDE_QUICKSTART.md`, and manufacturer/reference pinout image for AI IDEs.
- Added `docs/WIFI_DIAGNOSTICS.md` and a standalone synchronous radio test sketch.
- Extended mock-host tests to cover warmup, timeouts, retry success and failure, delayed completion, resources and scan cancellation.
- Not yet verified via actual PlatformIO build or physical-board flashing in assistant environment.

## v0.1 (original)
- Home Launcher, System Info, Touch Test and Wi-Fi Scanner (initial asynchronous implementation).
