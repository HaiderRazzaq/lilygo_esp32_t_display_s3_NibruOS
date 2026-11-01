# Changelog

User-facing changes follow Keep a Changelog categories and Semantic Versioning
when a release is actually made. Do not increment versions for every edit.
Release evidence and tagging policy: [releases](docs/releases/README.md).

## Unreleased

### Native Desktop Simulator & Cross-Platform Target — 2026-11-01

- Added interactive Windows/Desktop Simulator (`FEAT-0015`) using C++17, SDL2, and CMake. Reuses shared NibruOS core, applications (`WiFiApp`, `TotpApp`, `SettingsApp`, `SystemInfoApp`, `TouchTestApp`), and services with zero business logic duplication.
- Implemented 320x170 RGB565 framebuffer with 1x/2x/3x/4x integer nearest-neighbor scaling and pixel-accurate 5x7 GLCD typography.
- Implemented `DesktopInput` mapping mouse clicks and drags to `TouchEvent` taps and swipes, keyboard keys to `ButtonAction` (BOOT and User buttons), and developer shortcuts (`F1`..`F9`).
- Implemented deterministic Wi-Fi simulation supporting normal discovery, timeout (BUG-0001), scan errors, and connection handshakes.
- Implemented persistent NVS file storage in `simulator/runtime/nvs_store.dat` and sandboxed MicroSD storage in `simulator/runtime/sdcard/`.
- Added automated desktop test suite (`nibru_tests`) verifying lifecycle transitions, UI transforms, Wi-Fi fault injection, settings persistence across reloads, and 25-cycle stress tests.
- Preserved existing PlatformIO embedded target (`lilygo-t-display-s3`) with zero size regression or dependency leakage.

### Touch and Settings repaint repair — 2026-11-01

- Fixed repeated SINGLE_CLICK actions for one contact and premature NONE/down
  taps; preserved swipe completion and raw wake, with new-Down recovery.
- Fixed whole-screen clears on same-page control changes and no-op selections;
  update changed widgets/text and restore static page after the PIN overlay.
- Fixed delayed confirmation using actual press-start timing; keypad paints once
  per view. No framebuffer, new task, dependency or GPIO change.
- Owner's earlier physical FAIL remains recorded; repaired UI acceptance is pending.


### Authorized device upload — 2026-11-01

- Uploaded the latest default AW9364 image to the detected ESP32-S3 on COM5;
  write/hash verification and RTS reset command succeeded (HWT-0009).
  Runtime acceptance/profiling remains pending; no source or resource-cost change.

### Essential Settings completion — 2026-11-01

- Added scrollable Settings rows, physical focus/hold activation, two-page real
  system/memory diagnostics and confirmed resource-clean reboot/scoped reset.
- Fixed atomic versioned preference persistence, legacy migration through empty
  destinations, NVS failure reporting, unchanged-write suppression and safe
  defaults. Reset preserves credentials, security/vault and unrelated namespaces.
- Fixed rollover-safe inactivity timing and consumption of held wake contacts;
  touch confirmation requires a new contact beyond the double-tap window.
- Added opt-in serial profiling and LEDC build; AW9364 remains the default.
- Added generated-partition/image capacity checks, provisional growth warnings,
  resource tooling tests and executable host regression coverage.
- Changed resource documentation to withdraw unsupported approximate runtime
  baseline values. Build/static RAM figures never establish free heap headroom.
- No physical tests/upload/commit in this change; current acceptance remains
  IMPLEMENTED, verification incomplete. Historical entries below are retained;
  prior physical/PASS claims do not validate this image.


### Added
- Large tactile side navigation buttons in `TotpApp` (expanded from 32px to 46px width with double cyan border, solid filled triangle chevrons, and 54px wide hitboxes).
- Dedicated `Security` subview in `SettingsApp` featuring live status of PIN protection, PBKDF2-HMAC-SHA256 (2048 iterations) crypto details, and on-demand immediate device lock.
- Interactive `SetPin` subview with on-screen numeric keypad (0-9, DEL, SAVE), dynamic masked digit feedback, and 4-8 digit length validation.
- Hardened lock screen keypad with explicit `DEL` and `ENT` actions and dynamic 4-8 digit PIN verification.
- Hardware 2FA Authenticator & Crypto Vault (`FEAT-0014`) inspired by SecureGen.
- Hardware-accelerated `CryptoService` utilizing ESP32-S3 mbedTLS for PBKDF2-HMAC-SHA256, AES-256-GCM authenticated encryption/decryption, HMAC-SHA1/SHA256, and secure memory zeroization.
- Hardened `SecurityService` with zero plaintext secrets in flash, automatic migration of legacy PINs to salt+hash, constant-time validation, and reboot-persistent brute-force lockout.
- Non-blocking `TimeService` managing background NTP synchronization over Wi-Fi and real-time clock formatting.
- `TotpService` implementing RFC 6238 time-based one-time password generation and encrypted vault storage.
- Native `UsbHidService` enabling direct hardware keystroke injection of 2FA codes via ESP32-S3 USB-C OTG without host drivers or clipboard exposure.
- Touch-driven `TotpApp` with 320x170 card layout, color-coded countdown timers, multi-page touch navigation, and instant `[TYPE]` USB injection button.
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
