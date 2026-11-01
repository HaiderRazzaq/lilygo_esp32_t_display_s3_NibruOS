# Changelog

User-facing changes follow Keep a Changelog categories and Semantic Versioning
when a release is actually made. Do not increment versions for every edit.
Release evidence and tagging policy: [releases](docs/releases/README.md).

## Unreleased

### Added
- Essential Settings v0.1 (`FEAT-0013`) with 6 modular subviews: Display & Screen, Theme Accent, Memory Monitor, Wi-Fi Diagnostics, Device Controls (Reboot & Reset), and Touch Test.
- Centralized `SettingsService` for typed, validated, and persistent user configuration in NVS namespace `nibru_settings`.
- Lightweight, zero-allocation `MemoryProfiler` utility capturing real ESP-IDF heap and PSRAM runtime metrics without fictional estimates.
- Permanent project-wide Memory Profiling & Resource Budget Policy in `AGENTS.md`, `docs/RESOURCE_BUDGET.md`, `docs/MEMORY_PROFILING.md`, and `docs/performance/BASELINE.md`.
- Display brightness control (`FEAT-0012`, `FEAT-0013`) with AW9364 hardware pulse dimming (25%, 50%, 75%, 100%) and NVS persistence.
- Inactivity sleep mode (`FEAT-0012`) with configurable timeouts (15s, 30s, 1m, 2m, Never), instant wake-up on touch or button press, and manual test button.
- 4-digit PIN security lock screen (`FEAT-0012`) with capacitive touch numpad, NVS storage, and lock enforcement on boot and sleep wake-up.
- Interactive PIN management in Settings allowing passcode changes and enabling/disabling security lock.
- Wi-Fi Hub App (`FEAT-0011`) hosting Scan Networks and Saved Networks in a unified menu with hierarchical Back navigation.
- Persistent saved networks via NVS `Preferences` (`nibru_wifi` namespace) supporting up to 5 remembered access points with MRU ordering.
- Boot-time background auto-connect with fallback across saved networks, maintaining connection across app switches.
- Saved Networks management view with instant Connect, Forget, and Disconnect controls.
- Automatic password pre-fill when selecting an already-saved secured network in the scanner.
- Settings App (`FEAT-0010`) organizing System Info and Touch Test diagnostic views in a dedicated submenu with hierarchical Back navigation.
- IP address, MAC address, and live Wi-Fi connection indicator in `SystemInfoApp`.
- Virtual `shortTitle()`, `subtitle()`, and `onBack()` hooks on `App` interface to support dynamic card badges and hierarchical in-app navigation.
- Balanced 2-card landscape home layout for clean home launcher presentation.
- Wi-Fi AP selection and on-screen virtual keyboard with visible plaintext password entry (`FEAT-0009`).
- Non-blocking connection state machine with timeout and connection status details view.
- Engineering memory: current-state snapshot, feature/issue registries,
  development and physical-test logs, test matrix, inventory, dependency
  baseline, ADR/release guidance, and lightweight documentation validation.
- Documentation regression checks and CI integration with the existing host suite.

### Changed
- Bumped firmware version to NibruOS v0.3 across boot banner, Home launcher, BoardConfig, and System Info.
- Switched `SystemInfoApp` and `TouchTestApp` to targeted differential redraws to eliminate periodic screen flicker.
- Root launcher now registers `wifiApp` and `settingsApp`, presenting two clean primary hubs on the Home screen.
- `WiFiScannerApp` now operates within `WiFiApp` without severing active background Wi-Fi connections on exit.
- `AppManager` home card rendering dynamically centers text and adapts font size (2 to 1) to prevent card text overflow.
- Agent/IDE development workflow now requires evidence-based change tracking
  and a Definition of Done. Updated current guidance to match Wi-Fi timing.

### Fixed
- Resolved hardware backlight brightness control failure on physical LilyGO T-Display-S3 by implementing the AW9364 single-wire pulse-dimming protocol on GPIO 38, replacing incompatible high-frequency LEDC PWM.
- Fixed initialization sequence ordering so `tft_.init()` does not overwrite backlight pin configuration.
- Eliminated text overflow on Home screen cards where long titles (e.g. "Wi-Fi Scanner") spilled into adjacent cards.
- Resolved integer underflow timing race condition where `nowMs < connectStartedAt_` triggered an immediate `kConnectTimeoutMs` failure (reporting 4,294,967 s timeout).
- Eliminated whole-screen flickering during virtual keyboard keystrokes and scan refreshing using differential redraws.
- Suppressed touch auto-repeat on continuous contact by tracking release state in `TouchManager`.
- Documentation ambiguity about build verification and runtime versus source
  patch version. Fresh target-build evidence is separate from historical reports.

No GPIO allocation, dependency pin, partition, or version string changed.
Removed, Deprecated, and Security have no entries this cycle; add those headings when a real change needs them.

## Historical source baseline

The original [package changelog](docs/CHANGELOG.md) is preserved, including its
2026-10-26 v0.1.1-source-patch label and undated v0.1 section. Both are already
present in the first available commit `d6cf0294f548b64884076e8732e2bf425ee8e773`.
Their development order and release dates cannot be reconstructed from Git.
No release tags exist at this baseline; runtime still displays v0.1.
