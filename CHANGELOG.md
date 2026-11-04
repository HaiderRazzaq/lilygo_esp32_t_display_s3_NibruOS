# Changelog

## [v0.14.1] - 2026-11-04

### Changed

- Beacon Lab requests up to three transmissions per tick and displays the number of unique names accepted by the Wi-Fi driver.

## [v0.14.0] - 2026-11-04

### Added

- Beacon Lab has 48 fictional SSID names, adjustable five- to sixty-second sessions and independent open and WPA2-advertised network counts.

## [v0.13.0] - 2026-11-04

### Added

- Beacon Lab broadcasts synthetic Wi-Fi names in bounded ten-second sessions with selectable counts from 10 to 40. Desktop builds simulate the controls without radio transmission.

## [v0.12.1] - 2026-11-04

### Fixed

- Encode the BLE Device Information PnP ID in little-endian order. Windows keyboard acceptance remains pending.

## [v0.12.0] - 2026-11-04

### Fixed

- Use display-assisted BLE pairing. Show the one-time code on NibruOS and require an authenticated link before sending keyboard input.

## [v0.11.1] - 2026-11-04

### Fixed

- Allow Windows to discover BLE HID report metadata and subscribe before encryption completes. Send keystrokes only to the authenticated and subscribed link.

## [v0.11.0] - 2026-11-04

### Added

- Bluetooth card next to Wi-Fi on Home, with the same controls available from Settings.

### Fixed

- Wait for the host to subscribe to encrypted keyboard reports before showing keyboard readiness and allowing BLE typing. Request link encryption when a host connects.

## [v0.10.8] - 2026-11-04

### Changed

- Record BLE connection, disconnection reason, authentication result and advertising status over USB Serial.

## [v0.10.7] - 2026-11-04

### Fixed

- Enable Wi-Fi modem sleep before BLE keyboard startup and keep it enabled while Bluetooth is active. This avoids the ESP-IDF radio coexistence panic when Wi-Fi connects first.

## [v0.10.6] - 2026-11-04

### Fixed

- Check the BLE controller startup result before Arduino can spin indefinitely on an initialization error. Keep the keyboard disabled, release the radio lease, and report the failure in Settings and over Serial.

## [v0.10.5] - 2026-11-04

### Fixed
- **BLE HID Stack Stability & Advertising Optimization**: Aligned ESP32-S3 BLE HID keyboard initialization with the proven T-vK standard. Set `pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_MITM_BOND)`, set `pAdvertising->setScanResponse(false)` to prevent advertising packet overflow, and deferred `setBatteryLevel(100)` to execute strictly after service start and advertising launch, preventing pre-service GATTS notification faults.
- **Native USB Post-Flash Bootloader Recovery**: Resolved native USB-Serial/JTAG bootloader latch where esptool RTS resets left the ESP32-S3 in ROM download mode (`waiting for download`) instead of booting the application image. Added automated watchdog reset recovery to ensure immediate boot into runtime application mode (`COM6`).

## [v0.10.4] - 2026-11-04

### Fixed
- **BLE HID Null Pointer Crash on Initialization**: Fixed critical crash and board reboot triggered when tapping `[ENABLE BLE KEYBOARD]`. In `BleHidService`, calling `pHid->manufacturer("NibruOS")` directly bypassed `pHid->manufacturer()` characteristic creation in the ESP32 Arduino framework, causing a null pointer dereference (`StoreProhibited` panic). Changed to `pHid->manufacturer()->setValue("NibruOS")`.
- **BLE HID Output Report & Battery Level Order**: Added `pHid->outputReport(1)` to match keyboard LED output specifications, and moved `setBatteryLevel(100)` prior to `pHid->startServices()`.
- **BLE Security Stack Allocation**: Optimized `BLESecurity` configuration onto stack allocation without heap leakage.
- **Detailed BLE Serial Logging**: Added diagnostic serial output for each step of BLE HID startup and advertising.

## [v0.10.3] - 2026-11-04

### Fixed
- **Settings Bluetooth Touch Dispatch Routing**: Fixed critical bug where SettingsApp::onTouch called renderBluetooth(ctx) instead of handleBluetoothTouch(ctx, ev) under SettingsSubView::Bluetooth, preventing the [ENABLE BLE KEYBOARD] button from registering user touch input.
- **Settings Bluetooth Hardware Button Trigger**: Added SettingsSubView::Bluetooth handler in SettingsApp::onPrimaryAction, allowing users to toggle BLE keyboard activation or send test keystrokes using the physical BOOT button in addition to touchscreen taps.
- **Bluetooth Subview Clean Redraw & Generous Hit Target**: Enhanced touch coordinate tolerance (y: 98..150) for the CST816S touch controller and added background clearing to eliminate visual artifacts on state transitions.
- **Bluetooth LE Advertising Discovery**: Added setScanResponse(true) to BleHidService advertising configuration for optimal discovery across modern iOS, Android, and Windows hosts.

## [v0.10.2] - 2026-11-04

### Added
- **Bluetooth LE Wireless Keyboard Integration**: Implemented native on-device ESP32-S3 BLE HID keyboard stack (`BleHidService`) advertising as "NibruOS Keyboard" with bonded encrypted connection support, safe radio leasing via `ResourceManager`, and complete deinitialization on disable to preserve internal SRAM.
- **Settings Bluetooth Control View**: Added dedicated subview in `SettingsApp` with real-time connection state (`DISABLED`, `PAIRING...`, `CONNECTED!`), instant one-touch enable/disable, and a `[TEST KEYBOARD]` verification button typing keystrokes directly to the paired phone or computer.
- **Wireless OTP Transmission in 2FA Authenticator (`TotpApp`)**: Added interactive `[USB] / [BLE]` transport toggle in the Cyber Action Capsule. When in BLE mode and connected, holding touch or pressing the physical BOOT button transmits 6-digit OTP codes wirelessly over Bluetooth.
- **Wireless Password Typing in Password Generator (`GeneratorApp`)**: Added dedicated `[TYPE BLE]` button for instant wireless entry of high-entropy generated passwords to phone or PC.
- **Wireless Credential Typing in Password Vault (`VaultApp`)**: Added automatic wireless fallback/direct transmission of stored usernames and passwords over Bluetooth when BLE is connected.

## [v0.10.1] - 2026-11-04

### Added
- **Clock Screensaver Mode & Interactive Controls**: Cyber Desk Clock can now be engaged as an interactive screensaver upon system inactivity timeout. Added instant `[SAVER: ON/OFF]` toggle button directly inside `ClockApp`, and added screensaver toggle control inside `SettingsApp` -> `Display & Screen` view.
- **Wake on Input**: Any touch on the display or physical button press instantly exits screensaver mode and restores the active view.

### Fixed
- **Password Generator Vault Auto-Unlock & Save**: Fixed issue where tapping `[+ SAVE VAULT]` in `GeneratorApp` failed with "Unlock Vault first!". Now seamlessly detects locked session, prompts for master PIN pad, and upon successful authentication automatically loads the vault and commits the generated credential in one continuous flow.

## [v0.10.0] - 2026-11-04

### Added
- **Password Generator Tool (`GeneratorApp`)**: On-device cryptographic high-entropy password generator with configurable length (8-32), symbols toggle, one-touch USB HID typing, and direct "Save to Vault" integration.
- **Cyber Desk Clock App (`ClockApp`)**: Fullscreen cyber-themed digital clock displaying HH:MM:SS, date, animated seconds progress bar, NTP synchronization badge, and tap theme cycling.
- **Bluetooth LE HID Architecture Service (`BleHidService`)**: Modular BLE HID service mediated by `ResourceManager` mutual exclusion, safe default state to preserve internal SRAM.
- **Vault Categories & Quick Filter (`VaultApp`)**: Category tags (`ALL`, `WORK`, `PERS`, `FIN`) displayed on credentials cards with instant tap cycling and filtered `<` / `>` navigation.
- **Web Portal Encrypted Backup & Restore**: Added authenticated endpoints (`/api/vault/backup` and `/api/vault/restore`) and web portal UI tab for downloading and restoring PBKDF2/AES-GCM encrypted `.bin` vault backup files.


User-facing changes follow Keep a Changelog categories and Semantic Versioning
when a release is actually made. Do not increment versions for every edit.
Release evidence and tagging policy: [releases](docs/releases/README.md).

## Unreleased

### 2FA USB Long-Press & Hardware Button Trigger (v0.9.4) — 2026-11-04
- Redesigned 2FA Authenticator USB button UI into a sleek Cyber USB Action Capsule:
  - High-contrast Cyan border (`Colors::Cyan`) with rounded corners (`r=7`).
  - Dark-on-cyan left badge label (`USB`) and modern white prompt text (`HOLD OR BTN TO SEND`).
  - Dynamic state styling: active transmission (`TYPING TO PC...`), amber prompt on tap (`Hold touch or press BTN`), disconnected alert (`USB DISCONNECTED`), and missing time sync alert (`SYNC TIME TO SEND USB`).
- Added Touch Long-Press Gating for USB Code Typing:
  - Short taps no longer trigger accidental USB keystrokes; instead they display guidance (`Hold touch or press BTN`).
  - Sustained touch (LongPress) triggers `sendCurrentCode(ctx)` to type OTP digits to PC.
- Added Physical Hardware Button (BOOT / GPIO 0) USB Trigger:
  - Implemented `TotpApp::onPrimaryAction` (short press of BOOT) and `TotpApp::onActivate` (long press of BOOT) to directly trigger `sendCurrentCode(ctx)`.
- Sequentially bumped firmware version to v0.9.4 across `include/NibruConfig.h`, `include/BoardConfig.h`, `src/main.cpp`, and `docs/CURRENT_STATE.md`.
- Verified with host unit tests (VER-0117), desktop automated test suite (VER-0118), PlatformIO release build (959,440 B binary, 14.6% flash, 19.9% RAM, VER-0119), documentation verification (VER-0120), and physical upload to device on COM5 (HWT-0036).

### 2FA Authenticator UI Restoration & Cancel Lifecycle Fix (v0.9.3) — 2026-11-04
- Fixed 2FA Authenticator screen component loss on cancel/back from ADD or DELETE (BUG-0016):
  - Enforced `cardDirty_ = true` on `TotpApp::onBack()` and on `fullRedraw_` processing inside `render()`, ensuring that after screen clearing, navigation arrows (`<`, `>`), USB button, counter, `ADD`, and `DELETE` buttons are reliably redrawn.
  - Updated `TotpApp::onDisplayRestored` to set both `fullRedraw_` and `cardDirty_`, preventing unrendered UI upon waking from sleep.
  - Enhanced `ConfirmDelete` view rendering with rounded interactive buttons (`CANCEL` and `DELETE`) via `TextKeyboard::button`.
  - Added automated host regression tests in `tests/host/test_authenticator.cpp` asserting that cancelling from both ADD and DELETE views restores all card components and action buttons.
- Sequentially bumped firmware version to v0.9.3 across `include/NibruConfig.h`, `include/BoardConfig.h`, `src/main.cpp`, and `docs/CURRENT_STATE.md`.
- Verified with host unit tests (VER-0113), desktop automated test suite (VER-0114), PlatformIO release build (958,928 B binary, 14.6% flash, 19.9% RAM, VER-0115), documentation verification (VER-0116), and physical upload to device on COM5 (HWT-0035).

### Web Portal & Account Setup LinkedIn Integration (v0.9.2) — 2026-11-04
- Added official LinkedIn profiles and responsive badges to all web pages:
  - Embedded Web Portal (`src/services/WebPortalService.cpp` & `tools/account_setup/test_portal_preview.html`).
  - Standalone Account Setup application (`tools/account_setup/account_setup.html` & `tools/account_setup/account_setup.css`).
  - Added Nibru official LinkedIn link: `https://www.linkedin.com/company/nibruiq/`
  - Added Haider Razaq LinkedIn link: `https://www.linkedin.com/in/haiderrazaq/`
  - Styled with glowing neon hover effects and embedded vector SVG LinkedIn icons.
- Sequentially bumped firmware version to v0.9.2 across `include/NibruConfig.h`, `include/BoardConfig.h`, `src/main.cpp`, and `docs/CURRENT_STATE.md`.
- Verified with host unit tests (VER-0109), desktop automated test suite (VER-0110), PlatformIO release build (958,896 B binary, 14.6% flash, 19.9% RAM, VER-0111), documentation verification (VER-0112), and physical upload to device on COM5 (HWT-0034).

### Web Portal Visual Identity & Haider Razaq Brand Alignment (v0.9.1) — 2026-11-04
- Redesigned the embedded Web Portal (`src/services/WebPortalService.cpp`) to match the official NibruOS visual identity in `tools/account_setup`:
  - Dark navy/cyan neon theme (`--brand-navy: #0f1954`, `--brand-aqua: #38ffe1`, `--bg: #080f2c`).
  - Embedded NibruOS vector SVG logo with radial and linear gradients (`#1FB6C9` -> `#38FFE1`).
  - Full RTL Arabic typography (`dir="rtl"`, `lang="ar"`).
  - Modern card layouts with glowing neon borders, prominent Web PIN authorization card, and real-time connection status pill.
  - Interactive tab switcher for Password Vault entry and 2FA Authenticator entry.
  - Developer contact information (`07838844400`) and signature (`Developed by Haider Razaq`) in the responsive footer.
  - Secure random password generation and password unmasking toggle directly in the browser interface.
  - Split embedded HTML/CSS/JS into chunked C++ raw string literals (`R"h0(...)h0" R"h1(...)h1" R"h2(...)h2"`) to comply with MSVC 16,380-byte string token limits across compilers.
- Sequentially bumped firmware version to v0.9.1 across `include/NibruConfig.h`, `include/BoardConfig.h`, `src/main.cpp`, and `docs/CURRENT_STATE.md`.
- Verified with host unit tests (VER-0105), desktop automated test suite (VER-0106), PlatformIO release build (956,976 B binary, 14.6% flash, 19.9% RAM, VER-0107), documentation verification (VER-0108), and physical upload to device on COM5 (HWT-0033).

### Local Web Portal & Device Control Center (v0.9.0) — 2026-11-04
- Implemented FEAT-0019: Local Web Portal and Device Control Center:
  - Added `WebPortalService` broadcasting an ephemeral SoftAP with random SSID (`NibruOS-XXXX`) and 8-character WPA2 passphrase.
  - Embedded responsive HTML5/CSS3 single-page web app in `PROGMEM` served at `http://192.168.4.1`, with tabs for adding Vault credentials (including built-in random password generator) and adding TOTP 2FA accounts.
  - Implemented authenticated REST API (`/api/status`, `/api/vault/add`, `/api/totp/add`) gated by a random 4-digit Web PIN shown on the device screen and requiring the active master key to be unlocked.
  - Added dedicated `WebPortal` subview in `SettingsApp` displaying SSID, IP, Password, prominent Web PIN, live 180s countdown timer, and a one-touch `[TYPE USB]` button to type the Wi-Fi password over USB HID to the connected host.
  - Added automatic shutdown after 180s timeout or on lock/sleep/exit to conserve battery and preserve security.
  - Sequentially bumped firmware version to v0.9.0 across `include/NibruConfig.h`, `include/BoardConfig.h`, `src/main.cpp`, and `docs/CURRENT_STATE.md`.
  - Verified with host tests (VER-0101), desktop simulator suite (VER-0102), PlatformIO release build (942,800 B, 14.4% flash, 19.9% RAM, VER-0103), documentation verification (VER-0104), and flash upload to physical device on COM5 (HWT-0032).

### Native USB HID Keyboard Composite Stack & Vault Wake Reload (v0.8.1) — 2026-11-04
- Fixed USB HID keyboard transmission and session lifecycle after wake (BUG-0015):
  - Enabled native TinyUSB composite mode (`-DARDUINO_USB_MODE=0`, `build_unflags = -DARDUINO_USB_MODE=1`) in the default `lilygo-t-display-s3` environment, restoring hardware USB HID keyboard scancode injection alongside CDC Serial on physical hardware.
  - Implemented `VaultApp::clearSession(AppContext& ctx)` resetting editor buffers, cancelling active USB transmissions, zeroing decrypted memory, and unlatching the session load gate upon device sleep or lock.
  - Updated `VaultApp::ensureSession(AppContext& ctx)` to automatically reload and decrypt credentials from NVS as soon as the device is unlocked with PIN upon waking, resolving the stuck 'Vault Locked' screen.
  - Added session clearing calls in `src/main.cpp` and `platform/desktop/DesktopMain.cpp` upon sleep and lock events.
  - Added automated desktop test in `tests/desktop/test_simulator.cpp` verifying session clearing on sleep and automatic reload upon waking and unlocking.
- Sequentially bumped firmware version to v0.8.1 across `include/NibruConfig.h`, `include/BoardConfig.h`, `src/main.cpp`, and `docs/CURRENT_STATE.md`.
- Verified with host unit tests (VER-0097), desktop automated test suite (VER-0098), PlatformIO release build (888,352 B binary, 13.5% flash, 19.7% RAM, VER-0099), documentation verification (VER-0100), and upload to physical device on COM5 (HWT-0031).

### Password Vault Application UI & Confirmed USB HID Credential Transmission (v0.8.0) — 2026-11-04
- Implemented Phase 3 Step 2: Password Vault UI Application (`VaultApp`) and confirmed USB HID credential transmission:
  - Added new on-device application `VaultApp` registered in the Home Launcher with bilingual support ("Passwords" / "خزنة كلمات المرور").
  - Designed card-based credential view with masked password display (`••••••••`) and instantaneous `[SHOW]` / `[HIDE]` toggle.
  - Implemented confirmed two-step USB HID transmission for both username and password:
    - First touch arms the target credential button with prominent highlight and status banner.
    - Second separate touch with lifted finger confirms and injects the text via native USB HID keyboard emulation.
    - Touching any other area or navigating immediately disarms transmission, eliminating accidental keystroke injection.
  - Added on-device 4-step credential creation wizard (Title -> Username -> Password -> Notes) using `TextKeyboard`.
  - Added confirmed deletion view with fail-safe Cancel / Delete options.
  - Enforced fail-closed memory security: sensitive editor buffers and credential states are zeroed out via `CryptoService::secureZero` upon exit, cancel, or commit.
- Registered in `src/main.cpp`, `platform/desktop/DesktopMain.cpp`, and `tests/desktop/test_simulator.cpp`.
- Extended automated desktop test suite (`testVaultAppUI`) verifying arming, confirmation, finger lifting enforcement, show/hide toggle, and deletion.
- Sequentially bumped firmware version to v0.8.0 across `include/NibruConfig.h`, `include/BoardConfig.h`, `src/main.cpp`, and `docs/CURRENT_STATE.md`.
- Verified with host unit tests (VER-0093), desktop automated test suite (VER-0094), PlatformIO release build (852,864 B binary, 13.0% flash, 16.0% RAM, +1.32% growth, VER-0095), and successful upload to physical device on COM5 (HWT-0030, VER-0096).

### Encrypted Password Vault Storage Engine & Authenticated Backup (v0.7.0) — 2026-11-03
- Implemented Phase 3 Step 1: Encrypted Password Vault Storage & Backup Engine (`VaultService`):
  - Added authenticated AES-256-GCM encrypted persistence in NVS (`"vlt_rec"` namespace) for account credentials (title [24B], username [32B], password [48B], notes [24B]; up to 16 accounts).
  - Designed strict security governance: active encryption key (`SecurityService::getActiveVaultKey()`) is derived upon PIN entry and held exclusively in volatile RAM.
  - Immediate fail-closed memory sanitization: `VaultService::lock()` securely zeroes out all plaintext credentials in RAM using `CryptoService::secureZero` upon device lock or display sleep.
  - Implemented standalone authenticated backup export (`VaultService::exportBackup`): uses PBKDF2-HMAC-SHA256 (2048 iterations) with a fresh 16-byte cryptographic Salt, 12-byte random IV, and 16-byte AES-GCM Tag.
  - Implemented tamper-evident backup restore (`VaultService::importBackup`): validates the 16-byte AES-GCM authentication tag; any bit-flip or wrong passphrase immediately rejects the import with zero memory corruption.
- Cross-platform dual-target support:
  - ESP32-S3 hardware crypto via `mbedtls` for AES-256-GCM.
  - Host/Desktop simulation via Windows CNG `BCrypt` for AES-GCM and PBKDF2.
- Integrated into `AppContext`, `src/main.cpp`, and `platform/desktop/DesktopMain.cpp`.
- Added comprehensive test suites:
  - Host unit tests (`tests/host/test_vault.cpp`) verifying add/update/delete, memory zeroing on lock, wrong session key rejection, backup export, bit-flip tamper rejection, wrong passphrase rejection, and restore.
  - Desktop simulator regression suite (`tests/desktop/test_simulator.cpp`) verifying end-to-end NVS disk persistence and backup flow.
- Sequentially bumped firmware version to v0.7.0 across `include/NibruConfig.h`, `include/BoardConfig.h`, `src/main.cpp`, and `docs/CURRENT_STATE.md`.
- Verified with host unit tests (VER-0089), desktop automated test suite (VER-0090), PlatformIO release build (846,352 B binary, 12.9% flash, 15.9% RAM, +0.55% growth, VER-0091), and successful upload to physical device on COM5 (HWT-0029, VER-0092).

### Lock Screen Wake Freeze and Display Restoration Fix (v0.6.1) — 2026-11-03
- Resolved root causes of lock screen freeze and visual stalling upon waking from sleep mode:
  - Eliminated hardware command hazards in `DisplayManager`: removed ST7789 `DISPOFF` (0x28) from `sleep()` and `DISPON` (0x29) from `wake()`. This removes the 120ms internal stabilization delay that caused subsequent memory write (`RAMWR`) commands to be dropped by the parallel controller. The AW9364 LED backlight on GPIO 38 provides 100% black level when off, keeping ST7789 parallel bus synchronized and ready at all times.
  - Suppressed all UI rendering while `display.isSleeping()` is active in `main.cpp` and `DesktopMain.cpp`. This prevents `dirty_` and `fullRedraw_` flags from being prematurely consumed in the dark, preserving them for a clean full redraw on wake.
  - Enforced `security.invalidateFull()` upon display wake to guarantee immediate, full redraw of the lock screen interface (header, keypad, status, footer, circles).
  - Added `onDisplayRestored(ctx)` overrides to `WiFiApp` and `WiFiScannerApp`, ensuring that when the device is unlocked or returns from an overlay screen, all subviews cleanly set `fullRedraw_ = true` / `menuFullRedraw_ = true` without residual visual artifacts or frozen keyboard boxes.
- Sequentially bumped firmware version to v0.6.1 across `include/NibruConfig.h`, `include/BoardConfig.h`, `src/main.cpp`, and `docs/CURRENT_STATE.md`.
- Verified with host unit test suite, desktop automated test suite (including sleep/lock/wake flow test), PlatformIO build (845,952 B binary, 12.9% flash, 15.3% RAM, +0.50% growth vs baseline), and documentation consistency check.

### Time Management Engine, USB Serial Sync & TOTP Time Validity (v0.6.0) — 2026-11-03
- Introduced comprehensive Time Management & Validation subsystem (Phase 2):
  - Added programmatic time-setting API `TimeService::setTime(epoch, source)` updating hardware RTC via `settimeofday()` on ESP32-S3 and maintaining sub-second accuracy.
  - Implemented non-blocking USB Serial instant time synchronization protocol (`TIME:<epoch>` or `T:<epoch>`), allowing offline 1-second host computer time sync without requiring Wi-Fi.
  - Added time source and validity state tracking (`TimeSource::None`, `Ntp`, `UsbSerial`, `Manual`) with sync timestamp recording (`lastSyncEpoch()`).
  - Added configurable Timezone Offset (-12 to +14 hours, default UTC+3) in `SettingsService` and NVS (`"tz"` key), ensuring local display while preserving strict RFC 6238 UTC epoch for TOTP token calculation.
  - Enhanced `TotpApp` UI with clear time validity feedback: displays live countdown with sync source badge (`NTP` in green, `USB` in cyan) when valid, or prominent "Waiting for time (Sync)" guidance when unset, with tap-to-sync shortcut.
  - Enhanced `SystemInfoApp` to display live formatted local time and status banner in addition to uptime.
- Sequentially bumped firmware version to v0.6.0 across `include/NibruConfig.h`, `include/BoardConfig.h`, `src/main.cpp`, and `docs/CURRENT_STATE.md`.
- Verified with host unit test suite (VER-0081), desktop automated test suite (VER-0082), PlatformIO build (845,936 B binary, 12.9% flash, 15.3% RAM, +0.50% growth vs baseline, VER-0083), and successful upload to physical device on COM5 (HWT-0027).

### Wake Display Refresh & Menu Multi-Selection Fix (v0.5.2) — 2026-11-03
- Fixed lock screen visual freeze upon waking from sleep:
  - Addressed root cause in SecurityService::lock() where ullRedraw_ remained alse, preventing SecurityService::render() from clearing the previous app's screen content from ST7789 display controller RAM.
  - On wake/lock, explicitly set ullRedraw_ = true, dirty_ = true, drawnInputLen_ = 255, and drawnErrorState_ = false, ensuring the full PIN lock keypad is immediately cleared and redrawn on screen.
- Fixed multiple border selection ghosting in SettingsApp menus:
  - Addressed issue where touching multiple options or dragging across menu rows left lingering accent border highlights.
  - Added drawnArmedMenu_ state tracking and explicitly cleared non-armed row inner borders with Colors::Panel on every rendering frame.
- Sequentially bumped firmware version to v0.5.2 across include/NibruConfig.h, include/BoardConfig.h, src/main.cpp, and docs/CURRENT_STATE.md.
- Verified with host test suite (VER-0077), desktop simulator test suite (VER-0078), PlatformIO build (844,512 B binary, 12.9% flash, 15.3% RAM, +0.33% growth vs baseline, VER-0079), and successful upload to physical device on COM5 (HWT-0026).

### Touch Stability & Confirmed Two-Step Navigation (v0.5.1) — 2026-11-03
- Implemented the deterministic Select-then-Confirm touch interaction paradigm across Home Launcher, SettingsApp menus, and WiFiApp cards:
  - First touch selects the target item, arming it for execution and displaying prominent visual feedback (bold accent highlight border and dynamic footer instruction).
  - Second separate touch on the exact same item confirms and executes the action (opens the app or subview).
  - Finger MUST be lifted between touches (`contactId` differs); holding or dragging continuously never triggers confirmation.
  - Touching a different item immediately shifts selection without launching, disarming the previous item and arming the new one.
  - Physical button interactions (BOOT / USER) and swipe gestures immediately reset the armed touch confirmation state.
- Audited CST816S touch coordinate transformations across Landscape Normal (0°) and Inverted (180°) orientations, ensuring zero edge coordinate offset.
- Added comprehensive unit test coverage in `tests/host/test_main.cpp`, `tests/host/test_settings.cpp`, and `tests/desktop/test_simulator.cpp`.
- Sequentially bumped firmware version to v0.5.1 across `include/NibruConfig.h`, `include/BoardConfig.h`, `src/main.cpp`, and `docs/CURRENT_STATE.md`.
- Verified with host unit tests (VER-0073), desktop simulator regression suite (VER-0074), PlatformIO build (844,416 B binary, 12.9% flash, 15.3% RAM, +0.31% growth vs baseline, VER-0075), and successful upload to physical device on COM5 (HWT-0025).

### System-Wide Arabic & English Localization Engine (v0.5.0) — 2026-11-03
- Added complete system-wide language localization support with dynamic runtime switching between English and Arabic (العربية) across all apps, system views, menus, cards, and status banners.
- Created standalone, high-legibility 14px Arabic bitmap font engine (`src/ui/ArabicFont.h`) covering 125 presentation forms-B glyphs rasterized from Segoe UI Bold / Tahoma for sharp, crisp reading on 260 PPI 320x170 display with separated dots and clear baselines.
- Created `ArabicReshaper` (`src/services/ArabicReshaper.h` / `src/services/ArabicReshaper.cpp`) with Unicode UTF-8 decoding, contextual Arabic cursive shaping (isolated, initial, medial, final), Lam-Alef ligature combining (`لا`, `لإ`, `لأ`, `لآ`), and visual RTL bidirectional reordering with LTR protection for numbers and Latin tokens.
- Created `LocalizationService` (`src/services/LocalizationService.h` / `src/services/LocalizationService.cpp`) providing unified `TextId` lookup and string tables for English and Arabic.
- Integrated automatic UTF-8 Arabic detection in `DisplayManager::label()` and `labelPadded()` with `drawArabic()`, rendering connected Arabic glyphs seamlessly on both physical ST7789 TFT and desktop SDL2 simulator without dynamic malloc.
- Added "Language" (اللغة) card in SettingsApp with bilingual touch selection and button navigation, persisting choice in NVS under `"lang"` key (0 = English, 1 = Arabic).
- Updated HomeScreen, WiFiApp, TotpApp, SystemInfoApp, TouchTestApp, and SettingsApp to utilize localized labels.
- Incrementally and monotonically bumped version to v0.5.0 across `include/NibruConfig.h`, `include/BoardConfig.h`, `src/main.cpp`, and `docs/CURRENT_STATE.md`.
- Verified with host unit tests (VER-0069), desktop simulator tests (VER-0070), PlatformIO build (843,872 B binary, 12.9% flash, 15.3% RAM, +0.25% growth vs baseline, VER-0071), and successful flash upload to COM5 (HWT-0024).

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

