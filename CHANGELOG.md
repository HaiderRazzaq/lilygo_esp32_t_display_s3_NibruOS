# Changelog

User-facing changes follow Keep a Changelog categories and Semantic Versioning
when a release is actually made. Do not increment versions for every edit.
Release evidence and tagging policy: [releases](docs/releases/README.md).

## Unreleased

### Flicker-Free PIN Entry & Security Keypad Optimization (v0.4.3) — 2026-11-03
- Eliminated display flicker across PIN code entry and Security settings views (SettingsApp SetPin, SettingsApp Security, and SecurityService lock screen).
- Fixed uncontrolled continuous rendering loop in SettingsApp where SetPin and Security views were redrawing on every 12ms tick due to missing dirty guards.
- Decoupled static keypad buttons, header, and footer from dynamic PIN dots in both SettingsApp::renderSetPin and SecurityService::render.
- Replaced fillRect blank-then-draw patterns with DisplayManager::labelPadded() for status lines and localized dot bounding box erasure.
- Incrementally and monotonically bumped version to v0.4.3 across include/NibruConfig.h, include/BoardConfig.h, src/main.cpp, and docs/CURRENT_STATE.md.
- Verified with host unit tests (VER-0065), desktop simulator tests (VER-0066), PlatformIO build (833,280 B binary, 12.7% flash, 15.3% RAM, -1.01% growth vs baseline, VER-0067), and successful upload to COM5 (HWT-0023).

### Flicker-Free Display Architecture & Live Redraw Optimization (v0.4.2) — 2026-11-03
- Eliminated display flicker across all views containing live timers, countdowns, and dynamic metrics (TotpApp, SystemInfoApp, WiFiScannerApp, WiFiApp).
- Identified and eliminated the "Blank-then-Draw" anti-pattern (calling fillRect over screen areas immediately before drawing text), which caused human-visible black frame flickering.
- Introduced DisplayManager::labelPadded() with atomic background glyph rendering via setTextColor(fg, bg) and fixed-width space padding, rendering text and clearing trailing remnants in a single hardware pass without screen erasing.
- Decoupled static UI chrome (cards, buttons, headers, borders, footers) from dynamic counters in TotpApp (6-digit code and seconds counter) and SystemInfoApp (uptime, heap, link speed, IP, PSRAM).
- Sequentially bumped firmware version to v0.4.2 across include/NibruConfig.h, include/BoardConfig.h, src/main.cpp, docs/CURRENT_STATE.md, and engineering logs.
- Verified with host test suite (run_host_tests.py), desktop simulator (run_desktop_tests.bat), documentation verification (check_docs.py), and PlatformIO build (833,056 B binary, 12.7% flash, 15.3% RAM, -1.03% growth vs baseline).

### Pairing Code Auto-Transfer & Web Serial Stability (v0.4.1) — 2026-11-03
- Resolved USB CDC auto-reset when connecting from companion tool by asserting `{dataTerminalReady: true, requestToSend: false}` and providing a pre-connect flow in `account_setup.html` / `account_setup.mjs`.
- Added automatic pairing code broadcast over Serial and USB HID keyboard emulation upon entering 2FA USB import, tapping the pairing code on the screen, or pressing the User/BOOT button.
- Added live Web Serial background listener in the browser companion tool that automatically captures the 16-character one-time pairing code and auto-fills the input field.
- Implemented sequential monotonic version bump to `v0.4.1` across `include/NibruConfig.h`, `include/BoardConfig.h`, `src/main.cpp`, `docs/CURRENT_STATE.md`, and changelogs.
- Host test suite, desktop simulator suite, documentation verification, PlatformIO build (832,800 B, 12.7% flash, 15.3% RAM, -1.06% growth), and flashing on COM5 all PASS.

### Screen orientation control (Landscape Normal 0° & Inverted 180°) — 2026-11-03
- Added runtime screen rotation control allowing the user to invert the display 180° so the USB-C port can face left or right without awkward cable strain.
- Synchronized ST7789 TFT scan direction with CST816S capacitive touch controller coordinate transformation (x = 319 - rawY, y = rawX) and inverted swipe gestures (SWIPE_UP -> SwipeRight, SWIPE_DOWN -> SwipeLeft, etc.).
- Persisted screen orientation in NVS under key "rot", strictly preserving the existing 5-byte config schema.
- Added touch and BOOT button navigation controls in Settings -> Display & Screen view.
- Added unit tests in tests/host/test_settings.cpp and tests/desktop/test_simulator.cpp verifying rotation, touch mapping, and persistence.
- Verified PlatformIO ESP32-S3 build (832,464 B binary, 12.7% flash, 15.3% RAM, 0 warnings) and all host/desktop tests (VER-0053..VER-0055).

### Architecture audit, layer boundary hardening & engineering standards — 2026-11-03
- Decoupled `TouchManager.h` from application interface by depending only on `core/EventTypes.h`.
- Single source of truth for hardware GPIO pins in `include/PinDefinitions.h` and system timing in `include/NibruConfig.h`, backed by compile-time `static_assert` assertions in `include/BoardConfig.h`.
- Formalized `src/services/MemoryProfiler.h` under the Services layer, retaining `src/core/MemoryProfiler.h` for backward compatibility.
- Cleaned all 64-bit data conversion warnings on MSVC desktop simulator and host builds (0 warnings).
- Enhanced `tools/run_host_tests.py` and `tools/run_desktop_tests.bat` with automatic MSVC BuildTools/Community toolchain discovery.
- All PlatformIO, host, desktop simulator, documentation, and mutation tests PASS (VER-0050, VER-0051, VER-0052); zero flash or RAM growth.

### Easier account entry — 2026-11-02
- Added local USB and existing-network Wi-Fi paste of Base32 keys/otpauth links,
  with a one-use pairing code, encrypted transfer, bounded two-minute session
  and explicit SAVE on the device. Local launcher: tools/open_account_setup.bat.
- Changed 2FA and Wi-Fi keyboards to standard number/QWERTY rows with Shift,
  symbols, Delete and Space. Base32 remains masked and restricted to valid keys.
- Fixed expired candidate saving before the regular loop tick and companion USB
  retry/clear behavior. No firmware dependency, GPIO or partition changes.
- Software/simulator/four build checks PASS; new native USB/UDP and physical
  resource acceptance remain pending HWT-0018. Default binary831408 B (+1.0324%).
- Uploaded the new default to COM3: HWT-0019 write/hash/reset PASS; native runtime
  acceptance still pending HWT-0018. No full-chip erase or NVS-range write.

### Authorized authenticator upload — 2026-11-02

- Flashed default phase-one image822912 B on COM3; write hashes verified and RTS
  reset sent (HWT-0016). Partition layout matched; no full-chip erase. Interactive
  runtime and physical resource acceptance remain pending HWT-0015.

### Authenticator phase one — 2026-11-01

- Added manual masked account entry/deletion and optional TinyUSB HID build.
- Fixed authenticated save/reopen/migration, atomic credentials, Base32 overflow/
  bounds, valid-time gating, USB completion/cancel and exit/lock/sleep cleanup.
- Removed public demo accounts, token logs and fresh default PIN.
- Security: explicit user PIN, credential guard, OS entropy/real Windows crypto
  and checked fail-closed desktop storage. No safe rekey/backup yet.
- Preserved default hardware/USB setup, partitions and framework. Four builds,
  host and simulator checks pass; physical HWT-0015 and runtime resources pending.
  No upload/commit/push, web cabinet, password manager or BLE added.


### Architecture Audit & Modular Refactoring — 2026-11-01

- Adopted Feature-Based Modular Monolith with Layered Responsibilities and Hardware Abstraction (HAL) ([ADR-0002](docs/decisions/0002-modular-architecture.md)).
- Extracted platform-neutral input event types (TouchEvent, TouchGesture, ButtonAction) into src/core/EventTypes.h, breaking inverted dependency of AppManager.h on ButtonManager.h.
- Formalized application lifecycle standard via src/core/IApp.h.
- Extracted Home Screen UI rendering logic out of AppManager into src/ui/HomeScreen.* and centralized system palettes and UI dimensions in src/ui/Theme.h.
- Centralized board GPIO pin configurations into include/PinDefinitions.h (including explicit unverified notes on proposed SPI MicroSD pins) and global system parameters into include/NibruConfig.h, while maintaining exact backward compatibility in include/BoardConfig.h.
- Added getMacAddress service method to WiFiService and eliminated direct <WiFi.h> hardware bypass from SystemInfoApp.
- Added permanent Architecture & Module Organization Policy to AGENTS.md.
- Added modular architecture contract tests to desktop simulator automated test suite.

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

