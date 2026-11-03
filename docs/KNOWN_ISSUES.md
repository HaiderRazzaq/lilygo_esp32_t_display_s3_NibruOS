# Known issues and verification blockers

Current status is mutable. Attempts remain in these records and in
[append-only history](DEVELOPMENT_LOG.md). FIXED — UNVERIFIED means a change
exists, not that the original physical failure has been resolved.
Status vocabulary and resolution rules: [workflow](ENGINEERING_WORKFLOW.md).

```json
{
  "record_type": "issue",
  "id": "BUG-0001",
  "title": "Wi-Fi scan timeout: physical resolution unverified",
  "status": "FIXED — UNVERIFIED",
  "description": "Earlier flashed NibruOS displayed Scan timed out; code mitigation exists but root cause and device resolution remain unconfirmed.",
  "symptoms": "No completed scan before original 16-second timeout.",
  "reproduction": "Earlier firmware: open Wi-Fi Scanner. Current firmware: run HWT-0004 and capture [WiFi] logs, retries, rescan and Back responsiveness.",
  "affected_versions": "Owner-reported earlier v0.1; exact image/commit UNKNOWN. v0.1.1-source-patch awaits device verification.",
  "suspected_cause": "STA readiness, async driver/event completion, power or board/RF configuration; none confirmed.",
  "root_cause": "UNCONFIRMED",
  "files": [
    "src/services/WiFiService.cpp",
    "src/apps/WiFiScannerApp.cpp",
    "docs/WIFI_DIAGNOSTICS.md",
    "tests/host/test_main.cpp"
  ],
  "attempts": [
    "Original immediate STA+scan and scanDelete-only shutdown: owner reported timeout; old source not separately present in Git.",
    "Historical patch adds warmup, completion-first polling, explicit scan stop, bounded params and one restart/retry. Host PASS reported in legacy verification; device outcome UNKNOWN.",
    "2026-11-01 target build PASS (VER-0001), current host rerun BLOCKED (VER-0002); no upload."
  ],
  "fix": "Implemented reliability mitigation, not established root-cause fix; see WIFI_DIAGNOSTICS.",
  "verification": [
    "HWT-0002",
    "HWT-0004",
    "VER-0001",
    "VER-0002"
  ],
  "risks": "No proof of Wi-Fi reception or physical responsiveness; do not blame antenna from a timeout alone."
}
```

```json
{
  "record_type": "issue",
  "id": "BUG-0002",
  "title": "CST816S source dependency is not commit-pinned",
  "status": "OPEN",
  "description": "platformio.ini points at a Git URL without a revision.",
  "symptoms": "New installations may resolve a different touch library.",
  "reproduction": "Inspect lib_deps; compare a clean resolution with DEPENDENCIES snapshot.",
  "affected_versions": "Current baseline",
  "suspected_cause": "Floating Git dependency configuration",
  "root_cause": "Confirmed configuration lacks revision suffix.",
  "files": [
    "platformio.ini",
    "docs/DEPENDENCIES.md"
  ],
  "attempts": [
    "Observed local 1.3.0 SHA 11fe1fbae1f226c328a9b08d808fb8f97d8a4c50; no library change attempted."
  ],
  "fix": "Not implemented; propose separately reviewed SHA pin after compatibility tests.",
  "verification": [
    "VER-0001"
  ],
  "risks": "Build reproducibility and driver compatibility differ across fresh setups."
}
```

```json
{
  "record_type": "issue",
  "id": "BUG-0003",
  "title": "Runtime/source-patch version identity differs",
  "status": "OPEN",
  "description": "Runtime v0.1 differs from the legacy v0.1.1-source-patch label; no central version or release tags.",
  "symptoms": "Logs and Home cannot identify the Wi-Fi patch by version alone.",
  "reproduction": "Compare main boot string, AppManager Home title and docs/CHANGELOG.md.",
  "affected_versions": "Current baseline",
  "suspected_cause": "Patch documentation updated independently of runtime labels.",
  "root_cause": "Source literals remain v0.1.",
  "files": [
    "src/main.cpp",
    "src/core/AppManager.cpp",
    "docs/CHANGELOG.md",
    "docs/CURRENT_STATE.md"
  ],
  "attempts": [
    "Documented both identities without a firmware change or automatic version bump."
  ],
  "fix": "Not implemented; decide centralized identity and release scope before tagging.",
  "verification": [
    "DEV-0004"
  ],
  "risks": "Use commit and image hash for physical tests; a version string alone is insufficient."
}
```

```json
{
  "record_type": "issue",
  "id": "BUG-0004",
  "title": "MicroSD integration prerequisites not verified",
  "status": "BLOCKED",
  "description": "Feature is planned; acquired module has no confirmed model, connection, power characterization or source implementation.",
  "symptoms": "No storage capability exists. This is an integration blocker, not a demonstrated SD failure.",
  "reproduction": "Inspect source and hardware inventory; verify physical module and proposed wiring before implementation.",
  "affected_versions": "Current baseline; FEAT-0007 planned",
  "suspected_cause": "Missing hardware identification and verification",
  "root_cause": "No SD source/service or connection evidence.",
  "files": [
    "docs/HARDWARE_INVENTORY.md",
    "docs/HARDWARE_PINOUT.md"
  ],
  "attempts": [
    "No wiring, driver implementation or SD tests attempted."
  ],
  "fix": "Not implemented; identify module/voltage/current and approve actual pin allocation before adding bounded storage service.",
  "verification": [
    "HWT-0006"
  ],
  "risks": "Module VIN/level shifting may be unsuitable for proposed 3.3V; optional SD shield conflicts."
}
```

```json
{
  "record_type": "issue",
  "id": "BUG-0005",
  "title": "Local build cleanup encounters locked artifacts",
  "status": "OPEN",
  "description": "PlatformIO clean reports Windows file locks and nonempty directory even though command exits zero.",
  "symptoms": "WinError 32 for bootloader.bin/firmware.bin/partitions.bin and WinError 145 for build directory.",
  "reproduction": "Observed during pio run -e lilygo-t-display-s3 -t clean on this workspace; reproduction after lock release UNKNOWN.",
  "affected_versions": "Local Windows environment on 2026-11-01; not a demonstrated firmware fault",
  "suspected_cause": "Another process holds artifact files; process identity UNKNOWN.",
  "root_cause": "File-lock errors observed; locking process not established.",
  "files": [
    "docs/evidence/2026-11-01-environment.txt"
  ],
  "attempts": [
    "Clean did not fully remove default build artifacts; did not force-delete files or stop other processes.",
    "Full build in ignored .testbuild/pio-build succeeded (VER-0001)."
  ],
  "fix": "Workaround only: alternate PLATFORMIO_BUILD_DIR. No fix to locking process.",
  "verification": [
    "VER-0001",
    "VER-0004"
  ],
  "risks": "Do not treat zero clean exit code as proof of a clean tree; default-directory locks may recur."
}
```

```json
{
  "record_type": "issue",
  "id": "BUG-0006",
  "title": "Documentation validator record-lifecycle rules required correction",
  "status": "VERIFIED RESOLVED",
  "description": "Initial checker required historical DEV source paths to exist forever and allowed HWT records outside the physical log.",
  "symptoms": "A documented source rename failed validation unless old DEV history was rewritten; moving an HWT entry into the software report was accepted.",
  "reproduction": "Run test_file_rename_preserves_immutable_development_history and test_hardware_record_must_stay_in_dedicated_log on the initial checker.",
  "affected_versions": "Initial uncommitted FEAT-0008 implementation in this session only; no released firmware affected.",
  "suspected_cause": "Record validation treated historical/current path semantics alike and did not enforce the physical document location.",
  "root_cause": "Confirmed by independent review and two failing mutation regressions.",
  "files": [
    "tools/check_docs.py",
    "tests/docs/test_check_docs.py",
    "docs/ENGINEERING_WORKFLOW.md"
  ],
  "attempts": [
    "Initial checker: 14 tests passed but did not cover these lifecycle cases.",
    "New tests each reproduced the finding before fixes; whole 16-test suite then passed."
  ],
  "fix": "DEV files may be historical absent paths within the repository; current feature/issue files and evidence still require existence. HWT records must use HARDWARE type and reside in HARDWARE_TEST_LOG.",
  "verification": [
    "VER-0005"
  ],
  "risks": "Structural validation still cannot prove real device observations or enforce append-only history against Git."
}
```

```json
{
  "record_type": "issue",
  "id": "BUG-0007",
  "title": "Essential Settings current-image hardware/resource acceptance pending",
  "status": "OPEN",
  "description": "Software completion passes build/mocks, but current brightness, contact timing, NVS physical recovery and resource budgets have no device evidence.",
  "symptoms": "Resource/device acceptance incomplete. Owner reported touch repetition/flicker (BUG-0008); other physical resource metrics remain unobserved.",
  "reproduction": "Follow HWT-0008 and MEMORY_PROFILING scenarios A–E on identified image.",
  "affected_versions": "v0.3 + current uncommitted Settings completion",
  "suspected_cause": "Unavailable physical evidence, not established code defect",
  "root_cause": "NOT ESTABLISHED; no physical test performed",
  "files": [
    "docs/SETTINGS.md",
    "docs/MEMORY_PROFILING.md",
    "docs/performance/SETTINGS_RESOURCE_IMPACT.md"
  ],
  "attempts": [
    "Initial existing source had Settings gaps/stale mocks; software fixes and failed attempts recorded in DEV-0014 and VER-0014 through VER-0019.",
    "No upload, serial monitor or physical profiling in this change.",
    "HWT-0009 upload followed by owner failure HWT-0010; focused repair tracked by BUG-0008, VER-0023/VER-0024 and HWT-0011; repaired UI still pending."
  ],
  "fix": "Implementation complete for software scope; pending physical validation.",
  "verification": [
    "VER-0017",
    "VER-0018",
    "VER-0019",
    "HWT-0008"
  ],
  "risks": "Do not infer >=150 KiB free heap, brightness compatibility, reception, touch responsiveness or power from static/mock PASS."
}
```

```json
{
  "record_type": "issue",
  "id": "BUG-0008",
  "title": "Repeated touch actions and full-screen Settings flicker",
  "status": "FIXED — UNVERIFIED",
  "description": "Owner physically reported repeated actions and flashes with every touch after HWT-0009.",
  "symptoms": "Multiple actions per second for one contact; screen clears on every settings touch, including unchanged selections.",
  "reproduction": "HWT-0010 owner report; native tests replay NONE/Down and repeated SINGLE_CLICK/Contact reports and count screen clears. Physical repaired recheck is HWT-0012.",
  "affected_versions": "HWT-0009 default image842848 B, runtimev0.3; repaired uncommitted source awaiting owner validation",
  "suspected_cause": "Source permits repeated classified gestures and premature NONE tap; Settings clears/repaints full screen unconditionally.",
  "root_cause": "Confirmed software paths by failing mock regressions; exact raw board event sequence was not captured.",
  "files": [
    "src/hardware/TouchManager.cpp",
    "src/apps/SettingsApp.cpp",
    "src/core/App.h",
    "src/core/AppManager.h",
    "src/main.cpp",
    "tests/host/test_main.cpp",
    "tests/host/test_settings.cpp"
  ],
  "attempts": [
    "Owner failure preserved as HWT-0010, never replaced with build PASS.",
    "RED assertions VER-0022; per-contact gesture latch and deferred NONE fallback, incremental/no-op rendering, surface restoration and contact-start confirmation added.",
    "Final native default/alternate tests PASS VER-0023; all3 target builds PASS VER-0024.",
    "Repaired default image uploaded COM5; HWT-0011 verifies write/hash/reset only. Await owner HWT-0012 recheck."
  ],
  "fix": "One classified action/contact, fallback only after release, new-Down recovery; redraw changed widgets, preserve static view and force restore after overlays; confirm using contact-start time.",
  "verification": [
    "HWT-0010",
    "VER-0022",
    "VER-0023",
    "VER-0024",
    "HWT-0012",
    "HWT-0011"
  ],
  "risks": "Physical controller event coalescing and actual panel flicker still need HWT-0012; no claim of verified physical resolution or memory headroom."
}
```


```json
{
  "record_type": "issue",
  "id": "BUG-0009",
  "title": "Authenticator demo/persistence/time/USB lifecycle incomplete",
  "status": "FIXED — UNVERIFIED",
  "description": "Prior app seeded public demo accounts, did not wire vault reopen, substituted uptime, logged tokens and reported unchecked USB success.",
  "symptoms": "Prior app seeded public demo accounts, did not wire vault reopen, substituted uptime, logged tokens and reported unchecked USB success.",
  "reproduction": "See AUTHENTICATOR.md and HWT-0015; host regressions in tests/host/test_authenticator.cpp",
  "affected_versions": "9513925 baseline / v0.3 phase one",
  "suspected_cause": "Inspected source and host reproductions",
  "root_cause": "Prior app seeded public demo accounts, did not wire vault reopen, substituted uptime, logged tokens and reported unchecked USB success.",
  "files": [
    "src/apps/TotpApp.cpp",
    "src/services/TotpService.cpp",
    "src/services/SecurityService.cpp",
    "src/services/UsbHidService.cpp",
    "docs/AUTHENTICATOR.md"
  ],
  "attempts": [
    "VER-0031 RED",
    "VER-0033 HOST PASS",
    "VER-0034 SIMULATOR PASS",
    "VER-0035 BUILD PASS"
  ],
  "fix": "Explicit provisioning, authenticated atomic persistence/migration, time gating, cleanup and bounded HID state; software checks pass, physical acceptance pending.",
  "verification": [
    "VER-0033",
    "VER-0034",
    "VER-0035",
    "HWT-0015"
  ],
  "risks": "Physical acceptance NOT RUN; no secure-boot/flash-encryption/security-certification claim."
}
```



```json
{
  "record_type": "issue",
  "id": "BUG-0010",
  "title": "PIN rekey and backup not implemented for stored TOTP accounts",
  "status": "OPEN",
  "description": "Changing PIN/salt changes the vault key and would orphan ciphertext. Safe rewrap/export remains unimplemented.",
  "symptoms": "Changing PIN/salt changes the vault key and would orphan ciphertext. Safe rewrap/export remains unimplemented.",
  "reproduction": "See AUTHENTICATOR.md and HWT-0015; host regressions in tests/host/test_authenticator.cpp",
  "affected_versions": "9513925 baseline / v0.3 phase one",
  "suspected_cause": "Inspected source and host reproductions",
  "root_cause": "Changing PIN/salt changes the vault key and would orphan ciphertext. Safe rewrap/export remains unimplemented.",
  "files": [
    "src/apps/TotpApp.cpp",
    "src/services/TotpService.cpp",
    "src/services/SecurityService.cpp",
    "src/services/UsbHidService.cpp",
    "docs/AUTHENTICATOR.md"
  ],
  "attempts": [
    "VER-0031 RED",
    "VER-0033 HOST PASS",
    "VER-0034 SIMULATOR PASS",
    "VER-0035 BUILD PASS"
  ],
  "fix": "Block PIN change/disabling while any TOTP storage exists. Explicit last-account deletion clears only TOTP storage. Never delete sole recovery data to change PIN.",
  "verification": [
    "VER-0033",
    "VER-0034",
    "VER-0035",
    "HWT-0015"
  ],
  "risks": "Physical acceptance NOT RUN; no secure-boot/flash-encryption/security-certification claim."
}
```


```json
{
  "record_type": "issue",
  "id": "BUG-0011",
  "title": "Long setup-key entry and unfamiliar keyboard make account provisioning cumbersome",
  "status": "FIXED — UNVERIFIED",
  "description": "Owner reached ADD/Base32 on the uploaded firmware and requested easier entry plus conventional number/letter order.",
  "symptoms": "Long secrets require manual touch entry; prior alphabetic keypad order differs from mobile/laptop.",
  "reproduction": "Owner report HWT-0017; new behavior test_account_import.cpp and test_authenticator.cpp; physical acceptance HWT-0018.",
  "affected_versions": "v0.3 phase-one image822912 B uploaded HWT-0016",
  "suspected_cause": "Manual-only provisioning and compact alphabetic keypad",
  "root_cause": "No paired paste channel existed; previous keypad did not use QWERTY.",
  "files": [
    "src/apps/TotpApp.cpp",
    "src/apps/WiFiScannerApp.cpp",
    "src/ui/TextKeyboard.h",
    "src/services/AccountImportService.cpp",
    "tools/account_setup.py",
    "tools/account_setup/account_setup.mjs"
  ],
  "attempts": [
    "VER-0041 RED/intermediate setup failures retained",
    "VER-0042 HOST PASS",
    "VER-0043 SIMULATOR PASS",
    "VER-0044 helper/browser checks PASS",
    "VER-0048 final source review"
  ],
  "fix": "Shared number/QWERTY rows with Shift/symbols; USB and existing-network Wi-Fi paste through one-use authenticated session with later physical SAVE.",
  "verification": [
    "VER-0042",
    "VER-0043",
    "VER-0044",
    "VER-0045",
    "HWT-0018"
  ],
  "risks": "Native serial/UDP and touch/resource acceptance NOT RUN; no QR camera/export."
}
```

```json
{
  "record_type": "issue",
  "id": "BUG-0012",
  "title": "Display flicker during periodic timer and counter redraws",
  "status": "FIXED — UNVERIFIED",
  "description": "Views with periodic counters or timers (TotpApp countdown, SystemInfoApp heap/uptime, WiFiScanner scanning animation, WiFiApp status) exhibited rapid screen flickering every second.",
  "symptoms": "Full screen or card-level black flash every second as counters and timers update.",
  "reproduction": "Open 2FA view with an active account; observe the 30-second timer countdown and code. Observe screen flash on every second.",
  "affected_versions": "v0.1.0 through v0.4.1",
  "suspected_cause": "Blank-then-draw anti-pattern using fillRect over entire cards/sections before drawing updated text.",
  "root_cause": "fillRect() clears pixels to background color before text is rendered, causing the LCD controller to scan out a blank frame. Additionally, TotpApp rendered the entire card (buttons, borders, account name, footer) on every second tick instead of only updating dynamic digits.",
  "files": [
    "src/hardware/DisplayManager.h",
    "src/hardware/DisplayManager.cpp",
    "src/apps/TotpApp.h",
    "src/apps/TotpApp.cpp",
    "src/apps/SystemInfoApp.h",
    "src/apps/SystemInfoApp.cpp",
    "src/apps/WiFiScannerApp.cpp",
    "src/apps/WiFiApp.cpp"
  ],
  "attempts": [
    "VER-0061 HOST PASS",
    "VER-0062 SIMULATOR PASS",
    "VER-0063 BUILD PASS"
  ],
  "fix": "Introduced DisplayManager::labelPadded() which uses setTextColor(fg, bg) with space padding to atomically overwrite text and background in a single pass without fillRect. Decoupled static card/footer rendering from dynamic counter updates in TotpApp and SystemInfoApp.",
  "verification": [
    "VER-0061",
    "VER-0062",
    "VER-0063"
  ],
  "risks": "Padded labels must match or exceed previous string lengths to fully erase remnants."
}
```

```json
{
  "record_type": "issue",
  "id": "BUG-0013",
  "title": "Display flicker during PIN entry and security configuration views",
  "status": "FIXED — UNVERIFIED",
  "description": "PIN entry screens (Set Security PIN in Settings and Device Lock screen in SecurityService) exhibited intense screen flickering either continuously or on every digit press.",
  "symptoms": "Screen continuously flashing black 60-80 times per second in Settings SetPin view; full screen black blanking on each keypad press on device lock screen.",
  "reproduction": "Open Settings -> Security -> Set PIN, observe screen flickering continuously. On lock screen, tap digits 1-2-3-4 and observe full screen wipe on each tap.",
  "affected_versions": "v0.1.0 through v0.4.2",
  "suspected_cause": "Missing dirty guard in SettingsApp::render causing unconditional 12ms tick redrawing, and full screen clear in SecurityService::render on every digit press.",
  "root_cause": "SettingsSubView::SetPin and Security were omitted from the controls dirty-check guard, triggering fillRect and fillRoundRect on every loop iteration. SecurityService::render called screen.clear(Colors::Bg) and redrew all 12 keypad buttons on every digit pressed.",
  "files": [
    "src/apps/SettingsApp.h",
    "src/apps/SettingsApp.cpp",
    "src/services/SecurityService.h",
    "src/services/SecurityService.cpp"
  ],
  "attempts": [
    "VER-0065 HOST PASS",
    "VER-0066 SIMULATOR PASS",
    "VER-0067 BUILD PASS",
    "HWT-0023 COM5 PASS"
  ],
  "fix": "Added dirty guards for SetPin and Security in SettingsApp::render. Decoupled static keypad buttons, headers, and footers from dynamic PIN dot indicators. Replaced status line fillRect with labelPadded and restricted circle erasure to localized bounding box.",
  "verification": [
    "VER-0065",
    "VER-0066",
    "VER-0067",
    "HWT-0023"
  ],
  "risks": "Local circle clearing must accurately bound the pin dot coordinates."
}
```

```json
{
  "record_type": "issue",
  "id": "BUG-0014",
  "title": "Lock screen visual freeze and stalled display upon waking from sleep",
  "status": "FIXED — UNVERIFIED",
  "description": "When the device enters sleep mode (e.g. while entering a Wi-Fi password or in an app view) and is subsequently woken by touch, the screen remained frozen showing the previous application screen instead of the lock screen keypad.",
  "symptoms": "Upon waking, the screen continues to show the previous application UI (e.g. Wi-Fi keyboard); touching buttons does not respond as the app, but random touches cause green PIN indicator circles to appear superimposed on the old screen.",
  "reproduction": "With PIN enabled, navigate to Wi-Fi -> Scan Networks -> select network -> enter password view. Allow device to timeout into sleep. Tap screen to wake. Observe screen content.",
  "affected_versions": "v0.5.0 through v0.6.0",
  "suspected_cause": "Premature consumption of fullRedraw_ while sleeping combined with ST7789 DISPOFF/DISPON stabilization timing drops.",
  "root_cause": "DisplayManager::sleep() issued ST7789 DISPOFF (0x28) and wake() issued DISPON (0x29). ST7789 requires up to 120ms after DISPON to stabilize before accepting memory writes; initial full redraws sent immediately on wake were dropped by the controller. Additionally, SecurityService::render() was called while display was sleeping, prematurely consuming fullRedraw_ and dirty_ in the dark.",
  "files": [
    "src/hardware/DisplayManager.cpp",
    "src/main.cpp",
    "platform/desktop/DesktopMain.cpp",
    "src/apps/WiFiApp.h",
    "src/apps/WiFiScannerApp.h"
  ],
  "attempts": [
    "VER-0085 HOST PASS",
    "VER-0086 SIMULATOR PASS",
    "VER-0087 BUILD PASS"
  ],
  "fix": "Removed 0x28 and 0x29 commands from DisplayManager (AW9364 backlight OFF provides 100% black level without parallel bus disruption), suppressed UI rendering while display.isSleeping() is true, enforced invalidateFull() upon wake, and added onDisplayRestored overrides to WiFiApp and WiFiScannerApp.",
  "verification": [
    "VER-0085",
    "VER-0086",
    "VER-0087"
  ],
  "risks": "Hardware-specific AW9364 backlight timing must be verified on physical board."
}
```

```json
{
  "record_type": "issue",
  "id": "BUG-0015",
  "title": "VaultApp session lockup after sleep and USB HID keyboard unavailable in default build",
  "status": "FIXED — UNVERIFIED",
  "description": "After sleep timeout and unlocking with PIN, VaultApp remained stuck displaying 'Vault Locked'. Additionally, USB HID keystroke injection ('Type User' and 'Type Pass') failed to send credentials to the connected PC.",
  "symptoms": "VaultApp shows 'Vault Locked' after unlocking; USB typing buttons display 'USB unavailable' or fail to type on the host computer.",
  "reproduction": "1. With PIN enabled, open VaultApp, let device sleep, wake and enter PIN. Observe VaultApp stays locked. 2. Plug device into PC, open Notepad, tap 'Type User' or 'Type Pass' with 2-touch confirmation. Observe nothing types.",
  "affected_versions": "v0.8.0",
  "suspected_cause": "attemptedLoad_ boolean latch in VaultApp prevented re-evaluating loadVault() after sleep/lock memory wipe; platformio.ini default environment had ARDUINO_USB_MODE=1 disabling TinyUSB keyboard.",
  "root_cause": "1. VaultApp::ensureSession only attempted to load the vault once using attemptedLoad_ without resetting upon lock/sleep; main.cpp called vault.lock() directly without notifying VaultApp via clearSession(). 2. Default PlatformIO environment lilygo-t-display-s3 had ARDUINO_USB_MODE=1 (Hardware Serial/JTAG mode) which bypassed TinyUSB HID stack entirely, making UsbHidService::isReady() always return false.",
  "files": [
    "platformio.ini",
    "src/apps/VaultApp.h",
    "src/apps/VaultApp.cpp",
    "src/main.cpp",
    "platform/desktop/DesktopMain.cpp",
    "tests/desktop/test_simulator.cpp"
  ],
  "attempts": [
    "VER-0097 HOST PASS",
    "VER-0098 SIMULATOR PASS",
    "VER-0099 BUILD PASS",
    "HWT-0031 COM5 PASS"
  ],
  "fix": "1. Enabled native TinyUSB composite mode (-DARDUINO_USB_MODE=0, build_unflags = -DARDUINO_USB_MODE=1) in default environment lilygo-t-display-s3. 2. Implemented VaultApp::clearSession(ctx) to reset attemptedLoad_, zero buffers, and cancel USB transfers on lock/sleep. 3. Updated ensureSession() to load whenever !ctx.vault.isLoaded() and active key is available. 4. Updated main.cpp and DesktopMain.cpp to call vaultApp.clearSession(context) on lock/sleep.",
  "verification": [
    "VER-0097",
    "VER-0098",
    "VER-0099",
    "HWT-0031"
  ],
  "risks": "Host OS must accept ESP32-S3 composite CDC + HID Keyboard device driver."
}
```
