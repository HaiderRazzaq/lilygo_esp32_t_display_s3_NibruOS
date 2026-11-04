# Development log

Append-only engineering records, created in increasing ID order. Imported
events retain their supported event date or UNKNOWN; `date` is when this record
was written. No missing development chronology has been reconstructed. Git is
the authority for code diffs; these records preserve reasoning and test evidence.
Corrections must be appended with the original ID reference.

```json
{
  "record_type": "development",
  "id": "DEV-0001",
  "date": "2026-11-01T00:14:58+03:00",
  "event_date": "2026-10-26T17:36:05+03:00",
  "related": [
    "FEAT-0001",
    "FEAT-0002",
    "FEAT-0003",
    "FEAT-0004",
    "FEAT-0005",
    "FEAT-0006"
  ],
  "action": "BASELINE IMPORT",
  "files": [
    "src/main.cpp",
    "src/core/AppManager.cpp",
    "src/services/WiFiService.cpp",
    "docs/CHANGELOG.md",
    "docs/VERIFICATION_REPORT.md"
  ],
  "description": "First available commit contains complete firmware, three apps, Wi-Fi reliability patch, docs, tests and diagnostic project.",
  "reason": "Establish verifiable repository history without inventing intermediate changes.",
  "expected": "One combined modular firmware with protected baseline hardware.",
  "actual": "Source and Git inspection confirm contents. Legacy report states host PASS and target/hardware NOT RUN at package creation.",
  "verification": "STATIC import of Git/source/legacy reports; original test outputs unavailable.",
  "risks": "Commit title stable is not physical evidence. Original Wi-Fi fix sequence exists only in narrative documentation.",
  "commit": "d6cf0294f548b64884076e8732e2bf425ee8e773",
  "follow_up": "Validate present source on target and actual board; preserve legacy report."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0002",
  "date": "2026-11-01T00:14:58+03:00",
  "event_date": "2026-10-27T00:30:41+03:00",
  "related": [
    "FEAT-0005"
  ],
  "action": "BASELINE IMPORT",
  "files": [
    "docs/T_Display_S3.pdf"
  ],
  "description": "Second available commit adds the one-page T_Display_S3 schematic PDF only.",
  "reason": "Preserve exact scope of the latest code-history reference.",
  "expected": "Reference artifact added without code changes.",
  "actual": "git show --stat confirms a single PDF addition.",
  "verification": "STATIC Git diff and PDF text inspection; schematic-to-owner PCB revision match NOT VERIFIED.",
  "risks": "Schematic date 2024-04-29 is not project development or physical test date.",
  "commit": "ccbd0e625837196ce547f16788022be6465786cb",
  "follow_up": "Confirm board revision before relying on charger/RF details."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0003",
  "date": "2026-11-01T00:14:58+03:00",
  "event_date": "UNKNOWN",
  "related": [
    "FEAT-0004",
    "FEAT-0005",
    "FEAT-0007",
    "BUG-0001"
  ],
  "action": "EVIDENCE IMPORT",
  "files": [
    "docs/HARDWARE_TEST_LOG.md",
    "docs/WIFI_DIAGNOSTICS.md",
    "docs/HARDWARE_INVENTORY.md"
  ],
  "description": "Imported owner independent-sketch success, earlier Wi-Fi timeout and report of acquired MicroSD module. Patch is already in first Git commit.",
  "reason": "Separate owner observations, current code and planned hardware from assumptions.",
  "expected": "Keep successful baseline and reported failure traceable.",
  "actual": "HWT-0001 PASS scoped to reference sketch; HWT-0002 FAIL scoped to earlier firmware; MicroSD connection and patch outcome UNKNOWN.",
  "verification": "Historical owner reports; no new physical testing.",
  "risks": "No raw serial/binary/date for owner tests; proposed SD wiring is not installed evidence.",
  "commit": "UNKNOWN (historical event identity); reports present in d6cf0294f548b64884076e8732e2bf425ee8e773",
  "follow_up": "Run HWT-0003 through HWT-0005; identify SD module before HWT-0006."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0004",
  "date": "2026-11-01T00:14:58+03:00",
  "event_date": "2026-11-01T00:14:58+03:00",
  "related": [
    "FEAT-0008",
    "BUG-0001",
    "BUG-0002",
    "BUG-0003",
    "BUG-0004",
    "BUG-0005"
  ],
  "action": "ADD / DOCUMENTATION CORRECTION",
  "files": [
    "AGENTS.md",
    "README.md",
    "README_AR.md",
    "CHANGELOG.md",
    "docs/README.md",
    "docs/ENGINEERING_WORKFLOW.md",
    "docs/CURRENT_STATE.md",
    "docs/FEATURE_REGISTRY.md",
    "docs/KNOWN_ISSUES.md",
    "docs/TEST_MATRIX.md",
    "docs/HARDWARE_INVENTORY.md",
    "docs/HARDWARE_PINOUT.md",
    "docs/DEPENDENCIES.md",
    "docs/DEVELOPMENT_LOG.md",
    "docs/HARDWARE_TEST_LOG.md",
    "docs/ARCHITECTURE.md",
    "docs/PROJECT_CONTEXT.md",
    "docs/BUILD_AND_TEST.md",
    "docs/ADDING_AN_APP.md",
    "docs/AI_IDE_QUICKSTART.md",
    "docs/VERIFICATION_REPORT.md",
    "docs/CHANGELOG.md",
    "docs/decisions/README.md",
    "docs/decisions/0001-engineering-records.md",
    "docs/releases/README.md",
    "docs/evidence/README.md",
    "tools/check_docs.py",
    "tests/docs/test_check_docs.py",
    "tests/host/run_tests.sh",
    "tests/README.md",
    ".github/copilot-instructions.md",
    ".github/workflows/validate.yml",
    ".gitignore",
    "docs/superpowers/plans/2026-11-01-engineering-memory.md"
  ],
  "description": "Implemented engineering memory and checker, update workflow, typed evidence records and CI; corrected stale current timeout/build guidance while preserving old reports.",
  "reason": "Carry trustworthy context across AI/developer sessions without firmware refactoring.",
  "expected": "Current state, history, implementation status, hardware plans and test layers are distinct and cross-referenced.",
  "actual": "Records populated from two Git commits, source, local dependencies and owner evidence. Source/configuration retained; verification recorded in VER-0001 through VER-0004.",
  "verification": "See VER-0001, VER-0002, VER-0003, VER-0004. Checker tests were first run RED because checker did not exist.",
  "risks": "Checker cannot prove physical report honesty or enforce immutable Git history; CI hosted execution pending; no version bump.",
  "commit": "UNCOMMITTED working tree based on ccbd0e625837196ce547f16788022be6465786cb",
  "follow_up": "Maintain logs and registries for every meaningful change; complete physical acceptance and current host suite."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0005",
  "date": "2026-11-01T00:21:32+03:00",
  "event_date": "2026-11-01T00:21:32+03:00",
  "related": [
    "FEAT-0008",
    "BUG-0006"
  ],
  "action": "FIX / VALIDATE / HANDOFF",
  "files": [
    "tools/check_docs.py",
    "tests/docs/test_check_docs.py",
    "docs/ENGINEERING_WORKFLOW.md",
    "docs/VERIFICATION_REPORT.md",
    "docs/KNOWN_ISSUES.md",
    "docs/FEATURE_REGISTRY.md",
    "docs/CURRENT_STATE.md",
    "docs/TEST_MATRIX.md",
    "docs/evidence/2026-11-01-documentation.txt"
  ],
  "description": "Final review corrected validator historical-path semantics and physical-log ownership; completed evidence and handoff. Added complete change inventory below.",
  "reason": "Allow future file removal without rewriting historical logs, and keep physical outcomes in the dedicated log.",
  "expected": "Valid current docs and immutable history coexist; unsupported/misplaced records fail validation.",
  "actual": "Both review findings reproduced by failing regressions, then fixed; checker PASS and suite16/16. Earlier initial fixture/missing-record failures preserved (VER-0003). Integrated host rerun still blocked by c++ absence. No source/config changes, upload, erase, release/tag or commit.",
  "verification": [
    "VER-0001",
    "VER-0002",
    "VER-0003",
    "VER-0004",
    "VER-0005"
  ],
  "risks": "Physical acceptance, CST816S pinning, runtime identity and default build-file locks remain unresolved. Hosted CI not run.",
  "commit": "UNCOMMITTED working tree based on ccbd0e625837196ce547f16788022be6465786cb",
  "follow_up": "Use AGENTS and engineering workflow for future changes; run HWT-0003/0004/0005 on board; unblock current host suite; identify MicroSD module before implementation.",
  "changed_files": [
    ".github/copilot-instructions.md",
    ".gitignore",
    "AGENTS.md",
    "README.md",
    "README_AR.md",
    "docs/ADDING_AN_APP.md",
    "docs/AI_IDE_QUICKSTART.md",
    "docs/ARCHITECTURE.md",
    "docs/BUILD_AND_TEST.md",
    "docs/CHANGELOG.md",
    "docs/HARDWARE_PINOUT.md",
    "docs/PROJECT_CONTEXT.md",
    "docs/VERIFICATION_REPORT.md",
    "tests/README.md",
    "tests/host/run_tests.sh"
  ],
  "new_files": [
    ".github/workflows/validate.yml",
    "CHANGELOG.md",
    "docs/CURRENT_STATE.md",
    "docs/DEPENDENCIES.md",
    "docs/DEVELOPMENT_LOG.md",
    "docs/ENGINEERING_WORKFLOW.md",
    "docs/FEATURE_REGISTRY.md",
    "docs/HARDWARE_INVENTORY.md",
    "docs/HARDWARE_TEST_LOG.md",
    "docs/KNOWN_ISSUES.md",
    "docs/README.md",
    "docs/TEST_MATRIX.md",
    "docs/decisions/0001-engineering-records.md",
    "docs/decisions/README.md",
    "docs/evidence/2026-11-01-documentation.txt",
    "docs/evidence/2026-11-01-environment.txt",
    "docs/evidence/2026-11-01-target-build.txt",
    "docs/evidence/README.md",
    "docs/releases/README.md",
    "docs/superpowers/plans/2026-11-01-engineering-memory.md",
    "tests/docs/test_check_docs.py",
    "tools/check_docs.py"
  ],
  "build_image_sha256": "aeb0d3765b725788005c337009bf221547663eb2b5240f775e69906ae3ca1b26",
  "review": "One Important and one Minor validator finding addressed with RED-to-GREEN regressions; no deferred findings."
}
```


```json
{
  "record_type": "development",
  "id": "DEV-0006",
  "date": "2026-11-01T00:36:00+03:00",
  "event_date": "2026-11-01T00:36:00+03:00",
  "related": [
    "FEAT-0004",
    "FEAT-0009"
  ],
  "action": "ADD / EXPAND",
  "files": [
    "src/apps/WiFiScannerApp.cpp",
    "src/apps/WiFiScannerApp.h",
    "src/services/WiFiService.cpp",
    "src/services/WiFiService.h",
    "include/BoardConfig.h",
    "tests/host/stubs/WiFi.h",
    "tests/host/test_main.cpp",
    "docs/FEATURE_REGISTRY.md",
    "docs/CURRENT_STATE.md",
    "docs/CHANGELOG.md",
    "docs/VERIFICATION_REPORT.md"
  ],
  "description": "Implemented AP selection, on-screen touch virtual keyboard with visible plaintext password, non-blocking Wi-Fi connection state machine with timeout, and connection status view.",
  "reason": "User requested the ability to select a scanned network and connect to it with password visible while typing.",
  "expected": "User can tap any scanned network; open networks connect directly, secured networks open full-screen virtual keyboard with visible password; non-blocking connection with status and disconnect.",
  "actual": "PlatformIO build passed (RAM 45976 / 327680 bytes, Flash 741117 / 6553600 bytes); firmware uploaded successfully to ESP32-S3 on COM3.",
  "verification": "VER-0006",
  "risks": "Touch precision on dense virtual keyboard requires user testing on CST816S glass.",
  "commit": "UNCOMMITTED working tree based on ccbd0e625837196ce547f16788022be6465786cb",
  "follow_up": "Validate physical touch accuracy, network join, and credential handling on hardware."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0007",
  "date": "2026-11-01T01:15:00+03:00",
  "event_date": "2026-11-01T01:15:00+03:00",
  "related": [
    "FEAT-0004",
    "FEAT-0009"
  ],
  "action": "ENHANCE / HARDEN",
  "files": [
    "src/apps/WiFiScannerApp.cpp",
    "src/apps/WiFiScannerApp.h",
    "src/services/WiFiService.cpp",
    "src/services/WiFiService.h",
    "include/BoardConfig.h",
    "tests/host/stubs/WiFi.h"
  ],
  "description": "Hardened Wi-Fi station association: enabled open auth threshold (WPA/WPA2/WPA3 Personal), all-channel active scan, auto-reconnect, and registered WiFi.onEvent to capture low-level disconnect reasons (e.g. 202 AUTH_FAIL, 14 4WAY_TIMEOUT, 201 NO_AP_FOUND) with live elapsed seconds, status, and on-screen diagnostics.",
  "reason": "User reported connection failed on physical device despite typing the correct password.",
  "expected": "Station associates with WPA/WPA2/WPA3 networks without being rejected by default WPA2-only threshold, and displays live status and precise disconnect codes if rejected by the AP.",
  "actual": "PlatformIO build passed (RAM 46048 / 327680 bytes, Flash 745937 / 6553600 bytes); firmware uploaded successfully to ESP32-S3 on COM3.",
  "verification": "VER-0006",
  "risks": "Networks requiring WPA-Enterprise (802.1X/EAP) or 5GHz-only APs remain unsupported by ESP32-S3 2.4GHz hardware.",
  "commit": "UNCOMMITTED working tree based on ccbd0e625837196ce547f16788022be6465786cb",
  "follow_up": "Observe physical screen and serial log during AP association attempt."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0008",
  "date": "2026-11-01T02:00:00+03:00",
  "event_date": "2026-11-01T02:00:00+03:00",
  "related": [
    "FEAT-0001",
    "FEAT-0004",
    "FEAT-0009",
    "FEAT-0011"
  ],
  "action": "ADD / REFACTOR",
  "files": [
    "src/apps/WiFiApp.cpp",
    "src/apps/WiFiApp.h",
    "src/apps/WiFiScannerApp.cpp",
    "src/apps/WiFiScannerApp.h",
    "src/core/AppManager.cpp",
    "src/services/WiFiService.cpp",
    "src/services/WiFiService.h",
    "src/main.cpp",
    "tests/host/stubs/Preferences.h",
    "tests/host/test_main.cpp",
    "docs/FEATURE_REGISTRY.md",
    "docs/CURRENT_STATE.md",
    "docs/CHANGELOG.md",
    "docs/VERIFICATION_REPORT.md"
  ],
  "description": "Implemented persistent NVS saved networks in Preferences, boot-time auto-connect with fallback, top-level WiFiApp container hosting Scan Networks and Saved Networks with hierarchical back navigation, and fixed home card text overflow with dynamic font sizing and centering.",
  "reason": "User reported text overflowing app cards, Wi-Fi credentials not persisting across reboots, and requested a dedicated Wi-Fi hub with saved networks management.",
  "expected": "No card text overflows; Wi-Fi credentials saved in NVS; automatic connection to known networks on boot; top-level Wi-Fi and Settings cards on home screen.",
  "actual": "PlatformIO build passed (RAM 46672 / 327680 bytes, Flash 757533 / 6553600 bytes). All doc checks pass.",
  "verification": "VER-0007",
  "risks": "Multiple saved networks with identical SSID in range will connect to strongest BSSID.",
  "commit": "UNCOMMITTED working tree based on ccbd0e625837196ce547f16788022be6465786cb",
  "follow_up": "Upload firmware to device on COM3 and verify on-device auto-connect and UI layout."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0009",
  "date": "2026-11-01T02:15:00+03:00",
  "event_date": "2026-11-01T02:15:00+03:00",
  "related": [
    "FEAT-0001",
    "FEAT-0002",
    "FEAT-0003",
    "FEAT-0010",
    "FEAT-0011"
  ],
  "action": "ENHANCE / VERSION BUMP",
  "files": [
    "include/BoardConfig.h",
    "src/main.cpp",
    "src/core/AppManager.cpp",
    "src/apps/SystemInfoApp.cpp",
    "src/apps/SystemInfoApp.h",
    "src/apps/TouchTestApp.cpp",
    "src/apps/TouchTestApp.h",
    "src/apps/SettingsApp.cpp",
    "src/apps/WiFiApp.cpp",
    "src/apps/WiFiApp.h",
    "docs/CURRENT_STATE.md",
    "docs/CHANGELOG.md",
    "docs/VERIFICATION_REPORT.md"
  ],
  "description": "Established versioning policy and bumped firmware runtime to v0.2. Centralized version constants in BoardConfig.h, eliminated periodic full-screen clearing flicker via targeted differential rendering in SystemInfoApp and TouchTestApp, and removed double-rendering on touch menu transitions.",
  "reason": "User required explicit version increments on modifications, and reported display flickering during System Info 1-second ticks and touch transitions.",
  "expected": "Runtime version displays NibruOS v0.2 across boot serial, home launcher, and System Info; periodic uptime ticks and touch transitions update without screen flicker.",
  "actual": "PlatformIO build passed (RAM 46688 / 327680 bytes, Flash 757717 / 6553600 bytes); firmware uploaded successfully to ESP32-S3 on COM3.",
  "verification": "VER-0008",
  "risks": "Differential redraw regions must match coordinate bounds of updated labels to avoid text clipping.",
  "commit": "UNCOMMITTED working tree based on ccbd0e625837196ce547f16788022be6465786cb",
  "follow_up": "Observe smooth non-flickering uptime counter and verify v0.2 label on physical board."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0010",
  "date": "2026-11-01T02:40:00+03:00",
  "event_date": "2026-11-01T02:40:00+03:00",
  "related": [
    "FEAT-0005",
    "FEAT-0010",
    "FEAT-0012"
  ],
  "action": "ENHANCE / FEATURE IMPLEMENTATION / VERSION BUMP",
  "files": [
    "include/BoardConfig.h",
    "src/main.cpp",
    "src/hardware/DisplayManager.h",
    "src/hardware/DisplayManager.cpp",
    "src/services/SecurityService.h",
    "src/services/SecurityService.cpp",
    "src/apps/SettingsApp.h",
    "src/apps/SettingsApp.cpp",
    "src/apps/SystemInfoApp.cpp",
    "src/core/AppContext.h",
    "src/core/App.h",
    "src/core/AppManager.h",
    "tests/host/stubs/Arduino.h",
    "tests/host/stubs/Preferences.h",
    "tests/host/stubs/TFT_eSPI.h",
    "docs/FEATURE_REGISTRY.md",
    "docs/CURRENT_STATE.md",
    "docs/DEVELOPMENT_LOG.md",
    "docs/VERIFICATION_REPORT.md",
    "CHANGELOG.md"
  ],
  "description": "Implemented FEAT-0012 adding display brightness PWM control via LEDC channel 0, inactivity auto-sleep mode with instant wake on touch/button, and persistent 4-digit PIN lock screen protection with on-screen touch numpad. Bumped runtime version to v0.3.",
  "reason": "User requested screen brightness control, sleep mode with touch/button wakeup, PIN code locking capability, and version bump with every update cycle.",
  "expected": "Screen brightness adjusts via hardware PWM (25..100%); display enters low-power sleep after timeout and wakes instantly; PIN code protects wake/boot when enabled; runtime version displays v0.3.",
  "actual": "PlatformIO build passed (RAM 46816 / 327680 bytes, Flash 770661 / 6553600 bytes). Documentation checks passed.",
  "verification": "VER-0009",
  "risks": "PWM frequency on backlight GPIO38 must avoid audible coil whine or display flicker (5 kHz selected); CST816S must remain powered during display sleep for touch wakeup.",
  "commit": "UNCOMMITTED working tree based on ccbd0e625837196ce547f16788022be6465786cb",
  "follow_up": "Flash firmware to board on COM3 and verify brightness presets, sleep timeout wakeup, and PIN lock verification on physical device."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0011",
  "date": "2026-11-01T03:00:00+03:00",
  "event_date": "2026-11-01T03:00:00+03:00",
  "related": [
    "FEAT-0005",
    "FEAT-0010",
    "FEAT-0012",
    "FEAT-0013"
  ],
  "action": "ENHANCE / HARDWARE FIX / ARCHITECTURE / POLICY",
  "files": [
    "src/hardware/DisplayManager.h",
    "src/hardware/DisplayManager.cpp",
    "src/services/SettingsService.h",
    "src/services/SettingsService.cpp",
    "src/core/MemoryProfiler.h",
    "src/core/MemoryProfiler.cpp",
    "src/core/AppContext.h",
    "src/apps/SettingsApp.h",
    "src/apps/SettingsApp.cpp",
    "src/main.cpp",
    "AGENTS.md",
    "docs/RESOURCE_BUDGET.md",
    "docs/MEMORY_PROFILING.md",
    "docs/performance/BASELINE.md",
    "docs/FEATURE_REGISTRY.md",
    "docs/CURRENT_STATE.md",
    "docs/DEVELOPMENT_LOG.md",
    "docs/VERIFICATION_REPORT.md",
    "CHANGELOG.md"
  ],
  "description": "Resolved backlight dimming failure on physical LilyGO T-Display-S3 by implementing AW9364 single-wire pulse-dimming protocol on GPIO38. Built centralized SettingsService for typed persistent configuration in NVS, created lightweight MemoryProfiler for real ESP-IDF heap and PSRAM runtime metrics, implemented Essential Settings v0.1 (Display, Theme Accent, Memory Monitor, Wi-Fi Diagnostics, Device Reboot and Settings Reset), and established permanent project-wide Memory Profiling & Resource Budget Policy.",
  "reason": "User reported that backlight brightness control did not work on the physical device (caused by AW9364 pulse-driver incompatibility with continuous 5 kHz LEDC PWM and TFT_eSPI pin overriding), and requested Essential Settings v0.1 along with a mandatory permanent engineering resource policy.",
  "expected": "Backlight brightness physically steps between 25%, 50%, 75%, and 100% on the board; centralized settings persist safely; real runtime metrics are profiled without fictional estimates; permanent engineering policy is codified.",
  "actual": "PlatformIO build SUCCESS (RAM 46752 / 327680 bytes [14.3%], Flash 766705 / 6553600 bytes [11.7%]). Uploaded to COM3 and verified boot log: restored 50% brightness and Cyber Cyan theme.",
  "verification": "VER-0010",
  "risks": "AW9364 requires holding LOW > 2.5ms to reset followed by precise microsecond pulses; delayMicroseconds is used to maintain strict timing windows.",
  "commit": "UNCOMMITTED working tree based on ccbd0e625837196ce547f16788022be6465786cb",
  "follow_up": "Physically observe multi-level screen dimming on the board and test the confirmation reboot modal."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0012",
  "date": "2026-11-01T03:20:00+03:00",
  "event_date": "2026-11-01T03:20:00+03:00",
  "related": [
    "FEAT-0001",
    "FEAT-0005",
    "FEAT-0014"
  ],
  "action": "ADD / ARCHITECTURE / SECURITY / EXPANSION",
  "files": [
    "src/services/CryptoService.h",
    "src/services/CryptoService.cpp",
    "src/services/SecurityService.h",
    "src/services/SecurityService.cpp",
    "src/services/TimeService.h",
    "src/services/TimeService.cpp",
    "src/services/TotpService.h",
    "src/services/TotpService.cpp",
    "src/services/UsbHidService.h",
    "src/services/UsbHidService.cpp",
    "src/apps/TotpApp.h",
    "src/apps/TotpApp.cpp",
    "src/core/AppContext.h",
    "src/main.cpp",
    "docs/FEATURE_REGISTRY.md",
    "docs/CURRENT_STATE.md",
    "docs/DEVELOPMENT_LOG.md",
    "docs/VERIFICATION_REPORT.md",
    "CHANGELOG.md"
  ],
  "description": "Implemented Hardware-Accelerated 2FA Authenticator & Crypto Vault (FEAT-0014) inspired by SecureGen. Built CryptoService using mbedTLS for PBKDF2-HMAC-SHA256, AES-256-GCM, HMAC-SHA1/SHA256, and secure memory zeroization. Hardened SecurityService by eliminating plaintext PIN storage, auto-migrating legacy PINs to salt+hash, enforcing reboot-persistent brute-force lockouts, and managing active in-memory session vault keys. Created TimeService with non-blocking NTP synchronization, TotpService for RFC 6238 code generation and encrypted vault persistence, UsbHidService for native USB-C keyboard keystroke injection, and TotpApp with a responsive 320x170 touch UI.",
  "reason": "User requested architectural plan and implementation of SecureGen capabilities (starting with Wi-Fi NTP time synchronization, maximum hardware security, and keystroke injection).",
  "expected": "Secure on-device 2FA generation with countdown timers; instant injection via USB-C; constant-time PIN validation; brute-force protection surviving reboots; zero plaintext secret exposure.",
  "actual": "PlatformIO build SUCCESS (RAM 61136 / 327680 bytes [18.7%], Flash 838093 / 6553600 bytes [12.8%]). Remaining internal SRAM is 266 KB, maintaining healthy headroom well above the 150 KB budget.",
  "verification": "VER-0011",
  "risks": "Native USB HID requires device to be plugged into a host USB port; typing pacing includes 15ms inter-character delay to avoid buffer overflow on target OS.",
  "commit": "UNCOMMITTED working tree based on ccbd0e625837196ce547f16788022be6465786cb",
  "follow_up": "Flash firmware to device on COM3 and physically test touch code generation and USB typing into text editor."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0013",
  "date": "2026-11-01T03:50:00+03:00",
  "event_date": "2026-11-01T03:50:00+03:00",
  "related": [
    "FEAT-0005",
    "FEAT-0013",
    "FEAT-0014"
  ],
  "action": "ENHANCE / UI / SECURITY / HARDENING",
  "files": [
    "src/apps/TotpApp.h",
    "src/apps/TotpApp.cpp",
    "src/services/SecurityService.h",
    "src/services/SecurityService.cpp",
    "src/apps/SettingsApp.h",
    "src/apps/SettingsApp.cpp",
    "docs/DEVELOPMENT_LOG.md",
    "docs/VERIFICATION_REPORT.md",
    "docs/HARDWARE_TEST_LOG.md",
    "docs/CURRENT_STATE.md",
    "CHANGELOG.md"
  ],
  "description": "Enlarged side navigation buttons in 2FA Authenticator from 32px to 46px with high-contrast double borders, solid filled triangle chevrons, and 54px wide hitboxes for effortless thumb navigation. Upgraded lock screen keypad with prominent DEL and ENT actions and dynamic 4-8 digit verification. Implemented comprehensive Security & PIN configuration subviews in SettingsApp, allowing users to toggle PIN protection, enter custom 4-8 digit PINs with on-screen masked keypad, derive PBKDF2-HMAC-SHA256 hashes with hardware TRNG salt, and lock the device on demand.",
  "reason": "User requested larger, clearer lateral navigation buttons for 2FA, and the ability to configure a highly secure custom PIN code to lock and protect the device.",
  "expected": "Thumb-friendly 46px side buttons with clear visual arrows; zero screen strobing; dedicated Security subview in Settings with interactive PIN setup and lock-now capabilities; PBKDF2 hash derivation with zero plaintext leakage.",
  "actual": "PlatformIO build SUCCESS (RAM 61168 / 327680 bytes [18.7%], Flash 841353 / 6553600 bytes [12.8%]). Flashed to physical ESP32-S3 on COM3 at 460800 baud (24.29s total, verified hash, hard reset via RTS pin).",
  "verification": "VER-0012",
  "risks": "Changing PIN derives PBKDF2 over 2048 iterations taking ~65ms, executed non-blockingly during the save touch event.",
  "commit": "UNCOMMITTED working tree based on ccbd0e625837196ce547f16788022be6465786cb",
  "follow_up": "User physically tests thumb side button clicks in 2FA Authenticator, enters Settings -> Security to set custom 4-8 digit PIN, and verifies lock and unlock behavior."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0014",
  "date": "2026-11-01",
  "related": [
    "FEAT-0013",
    "FEAT-0010",
    "FEAT-0008",
    "BUG-0007"
  ],
  "action": "COMPLETE / FIX / RESOURCE POLICY / EVIDENCE CORRECTION",
  "files": [
    "src/apps/SettingsApp.cpp",
    "src/services/SettingsService.cpp",
    "src/core/MemoryProfiler.cpp",
    "src/core/WakeInputGate.h",
    "src/hardware/TouchManager.cpp",
    "src/core/AppManager.cpp",
    "platformio.ini",
    "tools/check_resources.py",
    "tools/run_host_tests.py",
    "AGENTS.md",
    "docs/RESOURCE_BUDGET.md",
    "docs/MEMORY_PROFILING.md",
    "docs/performance/BASELINE.md",
    "docs/performance/SETTINGS_RESOURCE_IMPACT.md"
  ],
  "description": "Completed existing Settings with scroll/buttons/atomic preferences/migration/error handling/system diagnostics/scoped confirmed actions. Added contact-aware wake/confirmation, sparse opt-in profiling, artifact partition checks and permanent workflow. Preserved existing AW9364 default and Security/2FA; LEDC remains opt-in. Corrected unsupported approximate runtime baseline and stale current docs without deleting history.",
  "reason": "User supplied Essential Settings v0.1 and permanent project-wide resource policy execution request.",
  "expected": "Focused implementation, honest resource evidence and pending physical procedures; no unrelated advanced feature, upload or commit.",
  "actual": "Three target builds PASS; host default/profile+LEDC PASS; default image +1088 B and static RAM +32 B. Runtime resource measurements NOT MEASURED. Failed attempts retained; independent review issues covered by regressions.",
  "verification": "VER-0013 through VER-0019; HWT-0008 NOT RUN",
  "risks": "No hardware/latency/power/NVS power-loss proof. LEDC not accepted as default. Existing Security/2FA not audited. DEV-0012 linker RAM remainder did not prove runtime heap; earlier approximate baseline values are unsupported and withdrawn. DEV-0013 described synchronous PBKDF2 as non-blocking; this task does not reclassify that existing security operation.",
  "commit": "UNCOMMITTED based on 8239bb62d28ae5ac9c736033c7636c4e82ac8f30",
  "follow_up": "Run HWT-0008 with sanitized image-bound evidence; compare A–E and review thresholds before hardware acceptance."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0015",
  "date": "2026-11-01",
  "related": [
    "FEAT-0013",
    "BUG-0007"
  ],
  "action": "FINAL CONTACT REGRESSION / RESOURCE CORRECTION",
  "files": [
    "src/apps/SettingsApp.cpp",
    "tests/host/test_settings.cpp",
    "tests/host/test_main.cpp",
    "docs/performance/SETTINGS_RESOURCE_IMPACT.md"
  ],
  "description": "Added regression for a second contact begun inside the double-tap window but reporting SINGLE_CLICK later. Failed assertion reproduced unintended confirmation; retaining the rejected contact threshold fixes it. Tests also assert TouchManager emits distinct contact identities after release.",
  "reason": "Final source review extended raw-double-tap safety through delayed held-second-contact reports.",
  "expected": "A new confirming touch must begin after the rejection window; no action from early or held contacts.",
  "actual": "Full HOST default/profile+LEDC reruns PASS. Three final target builds PASS (VER-0020). Default image842848 B, growth1104 B (+0.1312%); static RAM61200 B (+32). These current-image metrics supersede DEV-0014 earlier checkpoint costs; historical attempts remain intact.",
  "verification": "VER-0017 final host captures; VER-0020 final target build; HWT-0008 NOT RUN",
  "risks": "All physical metrics/contact timing remain pending; software mocks do not prove controller behavior.",
  "commit": "UNCOMMITTED based on 8239bb6",
  "follow_up": "Run HWT-0008 and resource scenarios A–E with final image hashes."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0016",
  "date": "2026-11-01",
  "related": [
    "FEAT-0013",
    "BUG-0007"
  ],
  "action": "USER-AUTHORIZED DEVICE UPLOAD",
  "files": [
    "docs/HARDWARE_TEST_LOG.md",
    "docs/VERIFICATION_REPORT.md",
    "docs/CURRENT_STATE.md",
    "docs/TEST_MATRIX.md",
    "docs/performance/SETTINGS_RESOURCE_IMPACT.md"
  ],
  "description": "User explicitly requested uploading the latest firmware. Discovered actual ESP32 USB onCOM5 (COM3/COM4 are Bluetooth), reran default AW9364 build and capacity/hash checks, then uploaded without changing firmware source.",
  "reason": "Direct user request: ارفعه للجهاز",
  "expected": "Flash current default image and report actual upload result without claiming unobserved runtime success.",
  "actual": "Build/capacity PASS; COM5 upload SUCCESS19.734s,842848 B written, hash verified, RTS reset command. No full-flash erase or preferences reset. Updated current records; no commit/push.",
  "verification": "VER-0021; HWT-0009; python tools/check_docs.py",
  "risks": "Upload verification does not establish application boot, brightness/touch/Wi-Fi functionality or free heap/stack/power. No new runtime code or resource-cost change.",
  "commit": "UNCOMMITTED based on8239bb6",
  "follow_up": "User checks Settings/menu/touch/brightness/Back on device; follow HWT-0008 for complete A–E acceptance."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0017",
  "date": "2026-11-01",
  "related": [
    "BUG-0008",
    "FEAT-0013",
    "FEAT-0005",
    "FEAT-0003"
  ],
  "action": "REPAIR TOUCH / INCREMENTAL SETTINGS UI",
  "files": [
    "src/hardware/TouchManager.cpp",
    "src/hardware/TouchManager.h",
    "src/apps/SettingsApp.cpp",
    "src/apps/SettingsApp.h",
    "src/apps/TouchTestApp.h",
    "src/core/App.h",
    "src/core/AppManager.h",
    "src/main.cpp",
    "tests/host/test_main.cpp",
    "tests/host/test_settings.cpp",
    "docs/performance/TOUCH_FIX_RESOURCE_IMPACT.md"
  ],
  "description": "Diagnosed owner-reported repeat/flicker from repeated SINGLE_CLICK acceptance and full-screen clear per touch. Added one gesture per contact with NONE release fallback and phase recovery, raw wake visibility, small drawing cache/partial widget updates/no-op skip, padded diagnostic text and static keypad paint. Independent review exposed lock-overlay restoration and contact-start confirmation timing; both fixed with failing regression coverage.",
  "reason": "User reported poor Settings touch behavior on the just-uploaded physical device.",
  "expected": "One intended interaction per press, preserved swipes/wake/Back/confirmation safety and lower redraw work without a framebuffer, new task, hardware reinit or GPIO change.",
  "actual": "Native default/alternate suites PASS;3 target builds PASS. Default image843888 B (+1040 B/+0.1234% vs842848 B), static RAM61240 B (+40 B). Runtime memory/latency/power NOT MEASURED; repaired physical UI acceptance pending.",
  "verification": "VER-0022 RED; VER-0023 HOST PASS; VER-0024 BUILD PASS; HWT-0010 owner FAIL; HWT-0012 NOT RUN",
  "risks": "Actual CST816S timing/IRQ coalescing and panel response need owner recheck. No dependency/pin/partition/power changes; existing AW9364 retained. No commit/push.",
  "commit": "UNCOMMITTED based on8239bb6",
  "follow_up": "Upload default repaired image under existing authorization, record flash result separately, then owner performs HWT-0012 and resource scenarios A–E."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0018",
  "date": "2026-11-01",
  "related": [
    "BUG-0008",
    "FEAT-0013"
  ],
  "action": "UPLOAD REPAIRED DEFAULT IMAGE",
  "files": [
    "docs/HARDWARE_TEST_LOG.md",
    "docs/CURRENT_STATE.md",
    "docs/performance/TOUCH_FIX_RESOURCE_IMPACT.md"
  ],
  "description": "Uploaded the repaired AW9364 default image to the detected ESP32-S3 COM5 under the existing device-upload authorization; preserved source wiring/NVS/partitions and did not commit/push.",
  "reason": "Continue the user-authorized device workflow after correcting their reported touch/flicker regression.",
  "expected": "Actual upload confirmation with no invented repaired-UI PASS.",
  "actual": "HWT-0011 PASS flash843888 B/data hash/RTS reset; actual repaired display/contact behavior remains unobserved. Documentation checks pass after records update.",
  "verification": "VER-0023/VER-0024 software; HWT-0011 flash only; HWT-0012 UI NOT RUN",
  "risks": "Touch issue remains FIXED — UNVERIFIED until user physical feedback. Runtime resource metrics remain NOT MEASURED.",
  "commit": "UNCOMMITTED based on8239bb6",
  "follow_up": "Owner tests one held contact, repeated separate taps, Settings selection/scroll/Back, wake/PIN restoration and safe confirmations on the new image."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0019",
  "date": "2026-11-01",
  "related": [
    "BUG-0008",
    "FEAT-0013"
  ],
  "action": "REPEAT UPLOAD ON EXPLICIT REQUEST",
  "files": [
    "docs/HARDWARE_TEST_LOG.md",
    "docs/CURRENT_STATE.md"
  ],
  "description": "User requested upload again. Re-detected ESP32-S3 COM5 and uploaded current default repaired image without source/configuration or resource-cost change.",
  "reason": "Direct user request: ارفع",
  "expected": "Perform and report actual repeat upload.",
  "actual": "HWT-0013 PASS;843888 B, verified hash, RTS reset. Same image as HWT-0011; no new runtime metrics measured. Documentation-only assessment: no new Flash/RAM/PSRAM/task/GPIO/power cost.",
  "verification": "HWT-0013; python tools/check_docs.py",
  "risks": "No physical UI verification or secret/preferences reset performed; HWT-0012 remains pending.",
  "commit": "UNCOMMITTED based on8239bb6",
  "follow_up": "Owner checks repaired touch/display; no commit or push."
}
```
```json
{
  "record_type": "development",
  "id": "DEV-0020",
  "date": "2026-11-01",
  "related": [
    "FEAT-0015",
    "FEAT-0001",
    "FEAT-0003",
    "FEAT-0004",
    "FEAT-0010"
  ],
  "action": "IMPLEMENT DESKTOP SIMULATOR TARGET",
  "files": [
    "CMakeLists.txt",
    "platform/desktop/DesktopMain.cpp",
    "platform/desktop/SDLDisplay.cpp",
    "platform/desktop/SDLDisplay.h",
    "platform/desktop/DesktopInput.cpp",
    "platform/desktop/DesktopInput.h",
    "platform/desktop/MockWiFiService.cpp",
    "platform/desktop/MockWiFiService.h",
    "platform/desktop/DesktopStorage.cpp",
    "platform/desktop/DesktopStorage.h",
    "platform/desktop/DesktopDiagnostics.cpp",
    "platform/desktop/DesktopDiagnostics.h",
    "platform/desktop/stubs/Arduino.h",
    "platform/desktop/stubs/TFT_eSPI.h",
    "platform/desktop/stubs/Preferences.h",
    "platform/desktop/stubs/WiFi.h",
    "tests/desktop/test_simulator.cpp",
    "tools/run_simulator.bat",
    "tools/run_desktop_tests.bat",
    "docs/DESKTOP_SIMULATOR.md",
    "docs/SIMULATOR_ARCHITECTURE.md",
    "docs/SIMULATOR_TESTING.md",
    "AGENTS.md"
  ],
  "description": "Designed and implemented interactive Native Desktop Simulator for Windows using C++17, SDL2, and CMake. Reuses shared NibruOS core (AppManager, AppContext, ResourceManager), applications (WiFiApp, TotpApp, SettingsApp, SystemInfoApp, TouchTestApp), and services. Implemented 320x170 RGB565 framebuffer with 1x/2x/3x/4x nearest-neighbor scaling and pixel-exact 5x7 GLCD typography. Implemented DesktopInput mouse-to-touchscreen and keyboard-to-buttons mapping. Implemented persistent NVS file storage in simulator/runtime/nvs_store.dat and deterministic Wi-Fi simulation supporting normal scans, timeouts (BUG-0001), scan failures, and connection handshakes. Created automated desktop test suite covering lifecycle, UI transforms, Wi-Fi fault injection, settings persistence, and 25-cycle stress test.",
  "reason": "Enable interactive execution, visual verification, UI testing, and fault simulation on Windows without requiring physical ESP32-S3 hardware for every iteration.",
  "expected": "One maintainable codebase supporting two execution targets: physical ESP32-S3 and Windows desktop simulator. Zero duplication of application business logic. Clean verification.",
  "actual": "Both nibru_simulator.exe and nibru_tests.exe compile cleanly with MSVC 2022 and CMake/Ninja. All automated desktop tests PASS. pio run -e lilygo-t-display-s3 builds successfully without size regression (RAM 61240 B, Flash 843521 B).",
  "verification": "VER-0025; VER-0026; python tools/check_docs.py",
  "risks": "Desktop memory and timing do not replicate embedded SRAM or hardware AW9364 pulse modulation. Physical device acceptance remains mandatory before claiming hardware verification.",
  "commit": "UNCOMMITTED based on 8239bb6",
  "follow_up": "Use desktop simulator for rapid interactive app development; verify final changes on physical LILYGO board."
}
```


```json
{
  "record_type": "development",
  "id": "DEV-0021",
  "date": "2026-11-01",
  "related": [
    "FEAT-0001",
    "FEAT-0005",
    "FEAT-0006",
    "FEAT-0015"
  ],
  "action": "ARCHITECTURE AUDIT AND MODULAR REFACTORING",
  "files": [
    "src/core/EventTypes.h",
    "src/core/IApp.h",
    "src/core/App.h",
    "src/core/AppManager.h",
    "src/core/AppManager.cpp",
    "src/ui/Theme.h",
    "src/ui/HomeScreen.h",
    "src/ui/HomeScreen.cpp",
    "src/hardware/DisplayManager.h",
    "src/hardware/ButtonManager.h",
    "src/services/WiFiService.h",
    "src/services/WiFiService.cpp",
    "src/apps/SystemInfoApp.cpp",
    "include/PinDefinitions.h",
    "include/NibruConfig.h",
    "include/BoardConfig.h",
    "platform/desktop/DesktopInput.h",
    "tests/desktop/test_simulator.cpp",
    "tests/host/test_main.cpp",
    "CMakeLists.txt",
    "tools/run_host_tests.py",
    "tests/host/run_tests.sh",
    "docs/ARCHITECTURE.md",
    "docs/decisions/0002-modular-architecture.md",
    "docs/decisions/README.md",
    "AGENTS.md"
  ],
  "description": "Conducted complete architecture audit of NibruOS. Implemented Feature-Based Modular Monolith with Layered Responsibilities and HAL. Extracted normalized input events into src/core/EventTypes.h, breaking inverted Core->Hardware dependency on ButtonManager.h. Formalized IApp interface standard in src/core/IApp.h. Extracted 76 lines of Home Screen UI card drawing and badge layout from AppManager into src/ui/HomeScreen.* and centralized color palettes and UI metrics into src/ui/Theme.h. Centralized board GPIO pin reservations in include/PinDefinitions.h (including explicit unverified markers for proposed SPI MicroSD pins) and global system parameters in include/NibruConfig.h while preserving include/BoardConfig.h backward compatibility. Encapsulated MAC address retrieval in WiFiService to eliminate direct <WiFi.h> hardware bypass in SystemInfoApp. Added permanent architecture and module organization rules to AGENTS.md. Added modular HAL tests to desktop simulator and host suites.",
  "reason": "Transform NibruOS into a clean, modular, maintainable, resource-efficient ESP32-S3 firmware platform with clear separation between applications, UI, services, core lifecycle, and hardware drivers.",
  "expected": "Clean downward dependency directions, zero circular dependencies, zero breaking changes to public interfaces, image fits in app0, all desktop simulator and PlatformIO builds pass.",
  "actual": "All automated desktop simulator tests, host Zig smoke tests, and 17 doc mutation tests PASS. PlatformIO builds for default (843,565 B Flash, +44 B), profile (844,121 B Flash), and ledc (849,333 B Flash) SUCCESS. check_resources PASS (+0.005% flash change, well within 5% limit). check_docs PASS.",
  "verification": "VER-0027; VER-0028; python tools/check_docs.py",
  "risks": "Physical CST816S touch responsiveness and AW9364 pulse dimming require hardware acceptance on real LILYGO board (HWT-0012 pending).",
  "commit": "UNCOMMITTED based on 8239bb6",
  "follow_up": "Validate binary on physical LILYGO T-Display-S3 Touch Glass hardware; complete HWT-0012."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0022",
  "date": "2026-11-01",
  "related": [
    "FEAT-0010",
    "FEAT-0013",
    "FEAT-0015",
    "FEAT-0007",
    "HWT-0014"
  ],
  "action": "HARDWARE FLASH / USER CLARIFICATION",
  "files": [
    "include/PinDefinitions.h",
    "docs/HARDWARE_PINOUT.md",
    "docs/HARDWARE_INVENTORY.md",
    "docs/CURRENT_STATE.md",
    "docs/HARDWARE_TEST_LOG.md",
    "docs/evidence/2026-11-01-modular-architecture-upload-com5.txt"
  ],
  "description": "Updated hardware documentation and pin definitions to record user explicit clarification that external SPI MicroSD is NOT connected (GPIOs 10-13 unallocated). Flashed modular architecture refactored NibruOS default firmware to physical LILYGO T-Display-S3 Touch Glass on COM5 per user request. Flashed bootloader, partitions, and application (843,936 bytes) in 7.4s at 460800 baud; hash verified; reset triggered via RTS.",
  "reason": "User confirmed MicroSD is not connected and requested firmware flashing to device.",
  "expected": "Flashing succeeds without error; device resets into newly refactored firmware; documentation accurately records disconnected MicroSD state and hardware upload outcome.",
  "actual": "PlatformIO upload to COM5 SUCCESS (32.056s total duration, 843,936 bytes flashed, hash verified, RTS hard reset executed).",
  "verification": "HWT-0014",
  "risks": "Physical runtime UI interaction and CST816S touch responsiveness pending user verification on device screen.",
  "commit": "UNCOMMITTED based on 8239bb6",
  "follow_up": "User tests interactive Home launcher navigation, Settings, and 2FA Authenticator on the physical display."
}
```


```json
{
  "record_type": "development",
  "id": "DEV-0023",
  "date": "2026-11-01",
  "related": [
    "FEAT-0012",
    "FEAT-0014",
    "FEAT-0015",
    "BUG-0009",
    "BUG-0010"
  ],
  "action": "CHANGED / FIXED / REMOVED / SECURITY",
  "files": [
    "src/apps/TotpApp.cpp",
    "src/services/TotpService.cpp",
    "src/services/SecurityService.cpp",
    "src/services/UsbHidService.cpp",
    "src/services/TimeService.cpp",
    "src/services/CryptoService.cpp",
    "src/main.cpp",
    "platform/desktop/DesktopMain.cpp",
    "platform/desktop/DesktopStorage.cpp",
    "tests/host/test_authenticator.cpp",
    "tests/desktop/test_simulator.cpp",
    "docs/AUTHENTICATOR.md",
    "docs/performance/AUTHENTICATOR_RESOURCE_IMPACT.md"
  ],
  "description": "Completed existing authenticator with masked entry/deletion, authenticated atomic persistence/credential writes, legacy compatibility, PIN guard, valid-time gating, cleanup and optional asynchronous HID. Removed demo seeds/token logs/fresh default PIN. Windows uses CNG/OS entropy and fail-closed atomic file storage.",
  "reason": "Owner authorized phase one after the comparison report.",
  "expected": "Persistent fixture accounts, valid time and truthful USB states, responsive Back, preserved board setup.",
  "actual": "Host/default-alternate and shared desktop suites PASS, >=20 cycles and actual disk failure/corruption tests PASS, four target builds/capacity checks PASS. Independent review caught NVS-type, HID-completion, deterministic-nonce, disk-write and time-loss gaps; fixes and regressions retained.",
  "verification": "VER-0029 through VER-0038; HWT-0015 NOT RUN",
  "risks": "Physical heap/PSRAM/stack/latency/power/USB/UI unverified. PIN rekey/backup deferred.",
  "commit": "UNCOMMITTED on 9513925; no commit/push/upload",
  "follow_up": "Run HWT-0015 with public fixtures and profiling A-E, >=20 physical cycles; safe rekey/backup is separate future work."
}
```


```json
{
  "record_type": "development",
  "id": "DEV-0024",
  "date": "2026-11-02",
  "related": [
    "FEAT-0014",
    "HWT-0016",
    "VER-0040"
  ],
  "action": "AUTHORIZED DEVICE UPLOAD / DOCUMENTATION",
  "files": [
    "docs/HARDWARE_TEST_LOG.md",
    "docs/VERIFICATION_REPORT.md",
    "docs/CURRENT_STATE.md",
    "docs/FEATURE_REGISTRY.md",
    "CHANGELOG.md"
  ],
  "description": "User explicitly requested flashing. Selected default lilygo-t-display-s3, detected COM3, verified exact partition-layout match, uploaded identified image and retained sanitized evidence.",
  "reason": "Explicit user instruction to flash the firmware",
  "expected": "Verified write and reset without full-chip erase or partition-layout change",
  "actual": "HWT-0016 PASS: 822912 B application, hashes verified, RTS reset,26.18s. No source or image-content change; NVS range not targeted by uploader.",
  "verification": "HWT-0016 upload only; VER-0040 artifact preflight; HWT-0015 runtime NOT RUN",
  "risks": "Interactive runtime and complete resource profiling remain unverified; default USB is Serial/JTAG, not opt-in HID.",
  "commit": "UNCOMMITTED on9513925; no commit/push",
  "follow_up": "Owner checks Home/touch/Settings, provisions own PIN and tests public TOTP fixture; perform HWT-0015 and profiling A-E."
}
```


```json
{
  "record_type": "development",
  "id": "DEV-0025",
  "date": "2026-11-02",
  "related": [
    "FEAT-0009",
    "FEAT-0014",
    "FEAT-0015",
    "BUG-0011"
  ],
  "action": "ADDED / CHANGED / FIXED / SECURITY",
  "files": [
    "src/ui/TextKeyboard.h",
    "src/ui/Base32Keyboard.h",
    "src/apps/TotpApp.cpp",
    "src/apps/WiFiScannerApp.cpp",
    "src/services/AccountImportService.cpp",
    "src/core/AppContext.h",
    "src/core/ResourceManager.h",
    "src/main.cpp",
    "platform/desktop/DesktopMain.cpp",
    "tools/account_setup.py",
    "tools/account_setup/account_setup.mjs",
    "tests/host/test_account_import.cpp",
    "tests/desktop/test_simulator.cpp",
    "tests/tools/test_account_setup.py",
    "docs/ACCOUNT_SETUP.md",
    "docs/performance/ACCOUNT_ENTRY_RESOURCE_IMPACT.md"
  ],
  "description": "Added standard number/QWERTY keyboard shared by 2FA/Wi-Fi, manual/USB/Wi-Fi ADD methods, bounded paired AES-GCM import and localhost-only paste companion; existing encrypted vault commits only on later physical SAVE.",
  "reason": "Owner requested easier long-secret entry and conventional keyboard; selected more than one paste method.",
  "expected": "Avoid manually typing long keys, preserve responsiveness, session cleanup, hardware baseline and user data.",
  "actual": "VER-0042..VER-0046 PASS host/simulator/helper/four builds/resource checks; final independent review VER-0048 no blocker. VER-0041 preserves RED and setup failures and their fixes. Default image+1.0324%, static+808 B.",
  "verification": "VER-0041 through VER-0046 and VER-0048; HWT-0018 pending native acceptance",
  "risks": "Physical USB/UDP/touch/persistence/heap/PSRAM/stack/latency/power not verified; no camera QR/rekey/backup; JS strings cannot promise full RAM erasure.",
  "commit": "UNCOMMITTED on9513925; no commit/push",
  "follow_up": "Upload only after software/documentation checks, then HWT-0018 with public fixtures and profiling A-E, >=20 physical cycles."
}
```


```json
{
  "record_type": "development",
  "id": "DEV-0026",
  "date": "2026-11-02",
  "related": [
    "FEAT-0014",
    "BUG-0011",
    "VER-0049",
    "HWT-0019"
  ],
  "action": "AUTHORIZED DEVICE UPLOAD / DOCUMENTATION",
  "files": [
    "docs/HARDWARE_TEST_LOG.md",
    "docs/VERIFICATION_REPORT.md",
    "docs/CURRENT_STATE.md",
    "docs/FEATURE_REGISTRY.md",
    "CHANGELOG.md"
  ],
  "description": "Uploaded completed easier-account-entry default firmware after software/documentation gates, using existing owner flashing authorization.",
  "reason": "Make requested QWERTY and multiple paste methods available on the owner device.",
  "expected": "Verified write and reset preserving partition layout and excluding NVS flash range.",
  "actual": "HWT-0019 PASS26.376s;831408 B app, hashes verified, reset sent. No full-chip erase, no source/image-content change after VER-0045.",
  "verification": "HWT-0019 upload only; VER-0049 preflight; HWT-0018 interactive/native/resource acceptance remains NOT RUN.",
  "risks": "Successful upload does not validate boot completion, real Web Serial/UDP transfer, touch/persistence or runtime resource headroom.",
  "commit": "UNCOMMITTED on9513925; no commit/push.",
  "follow_up": "Use launcher tools/open_account_setup.bat and public disposable fixture; complete HWT-0018 with resource scenarios A-E."
}
```


```json
{
  "record_type": "development",
  "id": "DEV-0027",
  "date": "2026-11-03",
  "related": [
    "FEAT-0001",
    "FEAT-0005",
    "FEAT-0006",
    "FEAT-0013",
    "FEAT-0015",
    "VER-0050",
    "VER-0051",
    "VER-0052"
  ],
  "action": "CHANGED / FIXED / REFACTORED",
  "files": [
    "include/BoardConfig.h",
    "include/PinDefinitions.h",
    "include/NibruConfig.h",
    "src/core/IApp.h",
    "src/hardware/TouchManager.h",
    "src/hardware/TouchManager.cpp",
    "src/services/CryptoService.cpp",
    "src/services/UsbHidService.cpp",
    "src/services/MemoryProfiler.h",
    "src/apps/SettingsApp.cpp",
    "tools/run_host_tests.py",
    "tools/run_desktop_tests.bat",
    "tests/desktop/test_simulator.cpp",
    "docs/ARCHITECTURE.md",
    "docs/CURRENT_STATE.md",
    "docs/decisions/0003-architecture-audit-modular-standards.md",
    "docs/decisions/README.md"
  ],
  "description": "Completed full architectural audit and modular standards refactoring. Decoupled Touch HAL from App interface, established compile-time static alignment for board pin configuration, added MemoryProfiler service header, resolved all MSVC 64-bit compiler warnings, and enhanced Windows host test tooling.",
  "reason": "Enforce Feature-Based Modular Monolith with Layered Responsibilities and HAL (ADR-0003), eliminate unnecessary dependencies, and guarantee cross-platform build cleanliness.",
  "expected": "Strict downward dependencies, single source of truth for pin definitions, zero compiler warnings on MSVC/GCC, 100% test pass rate, and zero flash/RAM regression.",
  "actual": "VER-0050, VER-0051, and VER-0052 PASS. Embedded binary size unchanged (831,424 B, 0.00% growth, 12.7% of app0). All 17 documentation checks and 17 mutation tests PASS.",
  "verification": "VER-0050 (PlatformIO target build), VER-0051 (Documentation checks), VER-0052 (Host unit tests, desktop simulator tests, node tests)",
  "risks": "Host tests and compilation checks do not validate real hardware CST816S touch response or AW9364 backlight pulses on physical board.",
  "commit": "UNCOMMITTED on main; no commit/push.",
  "follow_up": "Proceed with device upload and hardware test log verification when physical board is connected."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0028",
  "date": "2026-11-03",
  "related": [
    "FEAT-0010",
    "FEAT-0012",
    "FEAT-0013",
    "FEAT-0015",
    "VER-0053",
    "VER-0054",
    "VER-0055"
  ],
  "action": "ADDED / EXTENDED",
  "files": [
    "src/hardware/DisplayManager.h",
    "src/hardware/DisplayManager.cpp",
    "src/hardware/TouchManager.h",
    "src/hardware/TouchManager.cpp",
    "src/services/SettingsService.h",
    "src/services/SettingsService.cpp",
    "src/apps/SettingsApp.h",
    "src/apps/SettingsApp.cpp",
    "src/main.cpp",
    "platform/desktop/SDLDisplay.h",
    "tests/host/test_settings.cpp",
    "tests/desktop/test_simulator.cpp"
  ],
  "description": "Implemented runtime screen orientation control supporting Landscape Normal 0° (rotation 1) and Landscape Inverted 180° (rotation 3). Synchronized ST7789 TFT scan direction with CST816S capacitive touch coordinate transformation (x=319-rawY, y=rawX) and inverted swipe gestures, added dedicated non-volatile NVS persistence key 'rot' preserving the 5-byte config schema, exposed UI selection controls in Settings Display subview, and extended host unit tests and desktop simulator suites.",
  "reason": "Enable users to comfortably use the device when rotated 180° (e.g. USB-C port facing left vs. right) while maintaining the 320x170 landscape geometry across all OS views.",
  "expected": "Flawless rendering and touch mapping in both 0° and 180° orientations, persistent rotation state across reboots, zero regression on existing settings schema, and zero memory/resource budget violations.",
  "actual": "VER-0053 (PIO build 832,464 B binary, 12.7% flash, 15.3% RAM), VER-0054 (Host smoke tests, desktop simulator suite, and node tests PASS with 0 warnings), VER-0055 (Documentation checks PASS).",
  "verification": "VER-0053 (Embedded PlatformIO build), VER-0054 (Host unit & simulator tests), VER-0055 (Documentation consistency checks)",
  "risks": "Host tests and compilation checks verify driver math, NVS persistence, and layout. Physical verification on real board with inverted USB cable remains pending.",
  "commit": "UNCOMMITTED on main; no commit/push.",
  "follow_up": "Perform physical hardware test when user connects the physical board."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0029",
  "date": "2026-11-03",
  "related": [
    "FEAT-0008",
    "VER-0056"
  ],
  "action": "POLICY ADDED / VERSION BUMP",
  "files": [
    "AGENTS.md",
    "docs/ENGINEERING_WORKFLOW.md",
    "docs/releases/README.md",
    "include/NibruConfig.h",
    "include/BoardConfig.h",
    "src/main.cpp",
    "docs/CURRENT_STATE.md"
  ],
  "description": "Formulated and codified the Mandatory Automatic Sequential Versioning Policy across AGENTS.md, docs/ENGINEERING_WORKFLOW.md, and docs/releases/README.md. Sequentially bumped firmware version from v0.3 to v0.4 across all sources of truth (include/NibruConfig.h, include/BoardConfig.h, src/main.cpp, and docs/CURRENT_STATE.md). Verified with check_docs.py and multi-target suites.",
  "reason": "User explicit requirement: In ANY modification process, whether big or small, the version number must automatically be updated sequentially and monotonically.",
  "expected": "Zero version divergence across code and documentation, compliance with check_docs.py regex check on src/main.cpp, and automated build/test validation.",
  "actual": "VER-0056 PASS. Version bumped to v0.4. All documentation checks, host unit tests, simulator suite, and PlatformIO embedded build succeed.",
  "verification": "VER-0056 (Build, test, and documentation check suite)",
  "risks": "None; version strings updated in lockstep across all single sources of truth.",
  "commit": "UNCOMMITTED on main; no commit/push.",
  "follow_up": "Maintain sequential version bump on every subsequent task or edit."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0030",
  "date": "2026-11-03",
  "related": [
    "FEAT-0008",
    "VER-0057",
    "VER-0058",
    "VER-0059",
    "VER-0060",
    "HWT-0021"
  ],
  "action": "FEATURE / BUGFIX / VERSION BUMP",
  "files": [
    "include/NibruConfig.h",
    "include/BoardConfig.h",
    "src/main.cpp",
    "docs/CURRENT_STATE.md",
    "src/services/AccountImportService.h",
    "src/services/AccountImportService.cpp",
    "src/apps/TotpApp.h",
    "src/apps/TotpApp.cpp",
    "tools/account_setup/account_setup.html",
    "tools/account_setup/account_setup.css",
    "tools/account_setup/account_setup.mjs",
    "tests/host/test_account_import.cpp"
  ],
  "description": "Resolved USB CDC reset issue when pairing from browser companion tool by controlling DTR/RTS signals and providing a pre-connection workflow. Added instant automatic pairing code broadcast over Serial and USB HID keyboard emulation upon tapping screen or pressing button during 2FA USB import. Added live Web Serial background listener in companion tool to automatically capture and fill the pairing code. Sequentially bumped firmware version to v0.4.1.",
  "reason": "Fix device reboot when sending account setup from web companion tool, eliminate manual typing of 16-character one-time pairing code via touch/button trigger, and enforce sequential versioning policy.",
  "expected": "No device restart upon connection, immediate auto-fill of pairing code in browser tool upon screen tap/button press, seamless encrypted transfer, and clean build/test passes.",
  "actual": "VER-0057 (Host tests PASS), VER-0058 (Desktop tests PASS), VER-0059 (Node tests PASS), VER-0060 (PIO build PASS, 832,800 B, 12.7% flash, 15.3% RAM, -1.06% growth), HWT-0021 (Uploaded to COM5 successfully).",
  "verification": "VER-0057, VER-0058, VER-0059, VER-0060, HWT-0021",
  "risks": "Web Serial requires Chrome or Edge browser. Wi-Fi import path remains completely available and unaffected.",
  "commit": "UNCOMMITTED on main; no commit/push.",
  "follow_up": "Verify with user on physical device with companion web tool."
}
```


```json
{
  "record_type": "development",
  "id": "DEV-0031",
  "date": "2026-11-03",
  "related": [
    "BUG-0012",
    "VER-0061",
    "VER-0062",
    "VER-0063",
    "VER-0064",
    "HWT-0022"
  ],
  "action": "BUGFIX / REFACTOR / VERSION BUMP",
  "files": [
    "include/NibruConfig.h",
    "include/BoardConfig.h",
    "src/main.cpp",
    "docs/CURRENT_STATE.md",
    "src/hardware/DisplayManager.h",
    "src/hardware/DisplayManager.cpp",
    "src/apps/TotpApp.h",
    "src/apps/TotpApp.cpp",
    "src/apps/SystemInfoApp.h",
    "src/apps/SystemInfoApp.cpp",
    "src/apps/WiFiScannerApp.cpp",
    "src/apps/WiFiApp.cpp"
  ],
  "description": "Radically resolved display flickering across all views with timers and counters (TotpApp, SystemInfoApp, WiFiScannerApp, WiFiApp). Replaced fillRect blank-then-draw anti-pattern with single-pass glyph rendering via DisplayManager::labelPadded(). Decoupled static UI elements from dynamic counters. Incrementally bumped version to v0.4.2.",
  "reason": "User reported that all views with timers or counters flicker on every second or multiple times per second.",
  "expected": "Zero screen flicker during timer countdowns, live metrics updates, and scanning animations.",
  "actual": "Host tests PASS; desktop simulator tests PASS (25 cycles, 0 warnings); PlatformIO build PASS (833,056 B binary, 12.7% flash, 15.3% RAM, -1.03% growth vs baseline). check_resources PASS.",
  "verification": "VER-0061 (host), VER-0062 (desktop), VER-0063 (embedded build), VER-0064 (docs)",
  "risks": "Text fields must maintain fixed width formatting or use minChars padding to avoid glyph remnants.",
  "commit": "PENDING",
  "follow_up": "Firmware uploaded to COM5 successfully (HWT-0022); observe flicker-free display on physical board."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0032",
  "date": "2026-11-03",
  "related": [
    "BUG-0013",
    "VER-0065",
    "VER-0066",
    "VER-0067",
    "VER-0068",
    "HWT-0023"
  ],
  "action": "BUGFIX / REFACTOR / VERSION BUMP",
  "files": [
    "include/NibruConfig.h",
    "include/BoardConfig.h",
    "src/main.cpp",
    "docs/CURRENT_STATE.md",
    "src/apps/SettingsApp.h",
    "src/apps/SettingsApp.cpp",
    "src/services/SecurityService.h",
    "src/services/SecurityService.cpp"
  ],
  "description": "Eliminated display flickering across PIN entry views (SettingsApp SetPin, SettingsApp Security, and SecurityService lock screen). Fixed idle loop redraw in SettingsApp, decoupled static keypad chrome from dynamic PIN dots, and sequentially bumped firmware version to v0.4.3.",
  "reason": "User reported that the PIN code entry view also flickers.",
  "expected": "Completely flicker-free PIN setup and device unlock screens, with instant in-place dot rendering.",
  "actual": "VER-0065 (host tests PASS), VER-0066 (desktop tests PASS, 25 cycles), VER-0067 (PlatformIO build PASS, 833,280 B binary, 12.7% flash, 15.3% RAM, -1.01% growth), HWT-0023 (Uploaded to COM5 successfully).",
  "verification": "VER-0065, VER-0066, VER-0067, VER-0068, HWT-0023",
  "risks": "Low risk; keypad layout and touch coordinate hitboxes are completely unchanged.",
  "commit": "PENDING",
  "follow_up": "Observe physical screen on device during PIN entry."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0033",
  "date": "2026-11-03",
  "related": [
    "FEAT-0016",
    "VER-0069",
    "VER-0070",
    "VER-0071",
    "VER-0072",
    "HWT-0024"
  ],
  "action": "ADD / LOCALIZATION / TYPOGRAPHY / VERSION BUMP",
  "files": [
    "include/NibruConfig.h",
    "include/BoardConfig.h",
    "src/main.cpp",
    "docs/CURRENT_STATE.md",
    "src/ui/ArabicFont.h",
    "src/services/ArabicReshaper.h",
    "src/services/ArabicReshaper.cpp",
    "src/services/LocalizationService.h",
    "src/services/LocalizationService.cpp",
    "src/hardware/DisplayManager.h",
    "src/hardware/DisplayManager.cpp",
    "src/services/SettingsService.h",
    "src/services/SettingsService.cpp",
    "src/apps/SettingsApp.h",
    "src/apps/SettingsApp.cpp",
    "src/ui/HomeScreen.cpp",
    "src/apps/WiFiApp.h",
    "src/apps/TotpApp.h",
    "src/apps/SystemInfoApp.h",
    "src/apps/TouchTestApp.h",
    "tests/host/test_settings.cpp",
    "tests/host/stubs/TFT_eSPI.h",
    "platform/desktop/stubs/TFT_eSPI.h",
    "CMakeLists.txt"
  ],
  "description": "Implemented FEAT-0016 providing comprehensive system-wide Arabic and English localization. Created custom 14px Arabic bitmap font with 125 presentation forms-B glyphs, contextual cursive reshaping engine, bidirectional RTL reordering with LTR token preservation, centralized LocalizationService with TextId lookup, automated UTF-8 Arabic routing in DisplayManager, and bilingual Language setting card in SettingsApp with NVS persistence. Sequentially bumped firmware version to v0.5.0.",
  "reason": "User requested adding a Language option in Settings with Arabic (العربية) and English choices to switch the entire operating system interface language, while ensuring sharp, clear Arabic typography.",
  "expected": "Instant runtime language switching across all screens and apps; high-legibility unpixelated Arabic rendering; complete NVS persistence; clean build and verification passes.",
  "actual": "VER-0069 (host tests PASS), VER-0070 (desktop simulator tests PASS, 25 cycles), VER-0071 (PlatformIO build PASS, 843,872 B binary, 12.9% flash, 15.3% RAM, +0.25% growth vs baseline), HWT-0024 (Uploaded to COM5 successfully).",
  "verification": "VER-0069, VER-0070, VER-0071, VER-0072, HWT-0024",
  "risks": "Low risk; Arabic reshaping runs on-the-fly without heap allocations; Latin and English rendering paths are strictly preserved.",
  "commit": "PENDING",
  "follow_up": "Verify with user on physical device running v0.5.0."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0034",
  "date": "2026-11-03",
  "related": [
    "FEAT-0005",
    "VER-0073",
    "VER-0074",
    "VER-0075",
    "VER-0076",
    "HWT-0025"
  ],
  "action": "ENHANCE / TOUCH STABILITY / TWO-STEP CONFIRMATION / VERSION BUMP",
  "files": [
    "include/NibruConfig.h",
    "include/BoardConfig.h",
    "src/main.cpp",
    "docs/CURRENT_STATE.md",
    "src/core/AppManager.h",
    "src/core/AppManager.cpp",
    "src/ui/HomeScreen.h",
    "src/ui/HomeScreen.cpp",
    "src/apps/SettingsApp.h",
    "src/apps/SettingsApp.cpp",
    "src/apps/WiFiApp.h",
    "src/apps/WiFiApp.cpp",
    "tests/host/test_main.cpp",
    "tests/host/test_settings.cpp",
    "tests/desktop/test_simulator.cpp"
  ],
  "description": "Implemented Select-then-Confirm touch interaction pattern across Home Launcher, SettingsApp menus, and WiFiApp cards. First touch highlights and arms target item; second distinct touch (separate contactId with finger lifted) confirms execution. Holding or dragging does not confirm. Touching another item transfers selection. Physical buttons and swipe gestures reset confirmation arming. Sequentially bumped firmware version to v0.5.1.",
  "reason": "Prevent accidental app launches and false triggers on small 170x320 touch screen; ensure explicit visual feedback and confirmation before launching apps or changing views.",
  "expected": "Touch 1 highlights card with bold border and shows confirmation hint in footer; Touch 2 on same card launches it; touch elsewhere transfers selection.",
  "actual": "VER-0073 (host tests PASS), VER-0074 (desktop simulator tests PASS, 25 cycles), VER-0075 (PlatformIO build PASS, 844,416 B binary, 12.9% flash, 15.3% RAM, +0.31% growth vs baseline), HWT-0025 (Uploaded to COM5 successfully).",
  "verification": "VER-0073, VER-0074, VER-0075, VER-0076, HWT-0025",
  "risks": "Low risk; physical buttons (BOOT / USER) continue to function identically as secondary navigation path.",
  "commit": "PENDING",
  "follow_up": "Physically test on board: touch card once to select, observe highlight and footer, touch again to open."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0035",
  "date": "2026-11-03",
  "related": [
    "FEAT-0005",
    "VER-0077",
    "VER-0078",
    "VER-0079",
    "VER-0080",
    "HWT-0026"
  ],
  "action": "BUGFIX / LOCK SCREEN REDRAW / SETTINGS MULTI-SELECTION BORDER CLEAR / VERSION BUMP",
  "files": [
    "include/NibruConfig.h",
    "include/BoardConfig.h",
    "src/main.cpp",
    "docs/CURRENT_STATE.md",
    "src/services/SecurityService.cpp",
    "src/apps/SettingsApp.h",
    "src/apps/SettingsApp.cpp"
  ],
  "description": "Fixed two physical UX issues reported on LILYGO T-Display-S3 Touch: 1) Screen frozen on wake from sleep showing previous app while touches entered PIN on invisible keypad. Root cause: SecurityService::lock() left fullRedraw_ = false, rendering PIN dots incrementally over previous app pixels in ST7789 display RAM. Fixed by explicitly setting fullRedraw_ = true, dirty_ = true, drawnInputLen_ = 255, drawnErrorState_ = false in lock(). 2) Multi-finger touch in Settings menu caused multiple inner borders to remain highlighted simultaneously. Root cause: SettingsApp inner accent border was drawn when armed but never cleared to Colors::Panel when un-armed. Fixed by adding drawnArmedMenu_ tracking and redrawing inactive row borders with Colors::Panel on every frame. Monotonically bumped firmware version to v0.5.2.",
  "reason": "Ensure screen cleanly transitions to PIN lock screen upon wake/sleep and prevent ghost accent borders in settings menus.",
  "expected": "On wake from sleep, lock screen clears ST7789 and draws PIN keypad immediately. Touching settings menu options cleanly moves highlight border without leaving ghost borders.",
  "actual": "VER-0077 (host tests PASS), VER-0078 (desktop simulator tests PASS, 25 cycles), VER-0079 (PlatformIO build PASS, 844,512 B binary, 12.9% flash, 15.3% RAM, +0.33% growth vs baseline), HWT-0026 (Uploaded to COM5 successfully).",
  "verification": "VER-0077, VER-0078, VER-0079, VER-0080, HWT-0026",
  "risks": "Low risk; lock screen redraw and menu border rendering strictly within standard drawing routines.",
  "commit": "PENDING",
  "follow_up": "User to test wake from sleep (screen shows PIN keypad immediately) and multi-touch in settings menu (only active option is bordered)."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0036",
  "date": "2026-11-03",
  "related": [
    "FEAT-0005",
    "VER-0081",
    "VER-0082",
    "VER-0083",
    "VER-0084",
    "HWT-0027"
  ],
  "action": "FEAT / TIME MANAGEMENT / USB SERIAL SYNC / TOTP VALIDITY / VERSION BUMP",
  "files": [
    "include/NibruConfig.h",
    "include/BoardConfig.h",
    "src/main.cpp",
    "docs/CURRENT_STATE.md",
    "src/services/TimeService.h",
    "src/services/TimeService.cpp",
    "src/services/SettingsService.h",
    "src/services/SettingsService.cpp",
    "src/apps/TotpApp.cpp",
    "src/apps/SystemInfoApp.cpp",
    "tests/host/test_authenticator.cpp",
    "tests/host/test_settings.cpp"
  ],
  "description": "Implemented Phase 2 Step 1: Time Management & TOTP Time Validity Engine. Added TimeService::setTime() with settimeofday hardware RTC update on ESP32-S3 and simulated time on host/desktop. Added non-blocking USB Serial sync protocol (TIME:<epoch>) for 1-second offline host computer sync. Added sync source tracking (Ntp, UsbSerial, Manual, None) and lastSyncEpoch recording. Added timezone offset support (-12 to +14, default UTC+3) in SettingsService and NVS, keeping RFC 6238 UTC intact for TOTP while localizing formatted display. Updated TotpApp UI with live sync badge (NTP/USB) and tap-to-sync shortcut. Updated SystemInfoApp with formatted local clock. Monotonically bumped firmware version to v0.6.0.",
  "reason": "Allow offline TOTP generation, verify time accuracy and freshness, and provide instant USB time sync without requiring active Wi-Fi.",
  "expected": "Device accepts TIME:<epoch> over USB Serial; TotpApp immediately shows countdown and source badge; SystemInfoApp displays local time.",
  "actual": "VER-0081 (host tests PASS), VER-0082 (desktop tests PASS, 25 cycles), VER-0083 (PlatformIO build PASS, 845,936 B binary, 12.9% flash, 15.3% RAM, +0.50% growth vs baseline), HWT-0027 (uploaded to COM5 successfully).",
  "verification": "VER-0081, VER-0082, VER-0083, VER-0084, HWT-0027",
  "risks": "Low risk; time setting preserves UTC for cryptographic TOTP RFC 6238 while applying timezone purely to presentation strings.",
  "commit": "PENDING",
  "follow_up": "User to verify on physical device: send TIME:<epoch> or test TOTP display with sync source indicator."
}
```


```json
{
  "record_type": "development",
  "id": "DEV-0037",
  "date": "2026-11-03",
  "related": [
    "FEAT-0005",
    "BUG-0014",
    "VER-0085",
    "VER-0086",
    "VER-0087",
    "HWT-0028"
  ],
  "action": "BUGFIX / DISPLAY SLEEP-WAKE / LOCK SCREEN REFRESH / VERSION BUMP",
  "files": [
    "include/NibruConfig.h",
    "include/BoardConfig.h",
    "src/main.cpp",
    "docs/CURRENT_STATE.md",
    "src/hardware/DisplayManager.cpp",
    "src/apps/WiFiApp.h",
    "src/apps/WiFiScannerApp.h",
    "platform/desktop/DesktopMain.cpp",
    "tests/desktop/test_simulator.cpp"
  ],
  "description": "Resolved lock screen freeze upon waking from sleep mode: removed ST7789 DISPOFF (0x28) and DISPON (0x29) hardware commands from DisplayManager (avoiding 120ms stabilization drops on parallel bus), prevented UI rendering while display.isSleeping() is active so fullRedraw_ is preserved for the wake event, enforced security.invalidateFull() on display wake, and added onDisplayRestored overrides to WiFiApp and WiFiScannerApp.",
  "reason": "User reported that after entering sleep while typing a Wi-Fi password (or in an app), waking up showed the frozen old app screen with no response to button presses except for green PIN circles appearing over the old image.",
  "expected": "Waking from sleep immediately displays the clean, crisp, fully drawn lock screen with responsive keypad and zero visual artifacts from previous apps.",
  "actual": "Host tests PASS (VER-0085), desktop simulator test suite PASS including new sleep/lock/wake flow test (VER-0086), PlatformIO release build PASS (845,952 B binary, 12.9% flash, 15.3% RAM, VER-0087).",
  "verification": "VER-0085, VER-0086, VER-0087",
  "risks": "Hardware-specific AW9364 backlight timing must be verified on physical board.",
  "commit": "WORKING_TREE",
  "follow_up": "Flash to COM5 and perform physical sleep/wake test on hardware."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0038",
  "date": "2026-11-03",
  "related": [
    "FEAT-0017",
    "VER-0089",
    "VER-0090",
    "VER-0091",
    "VER-0092",
    "HWT-0029"
  ],
  "action": "FEATURE / ENCRYPTED PASSWORD VAULT STORAGE ENGINE / AUTHENTICATED BACKUP / VERSION BUMP v0.7.0",
  "files": [
    "include/NibruConfig.h",
    "include/BoardConfig.h",
    "src/services/VaultService.h",
    "src/services/VaultService.cpp",
    "src/core/AppContext.h",
    "src/main.cpp",
    "platform/desktop/DesktopMain.cpp",
    "tests/host/test_vault.cpp",
    "tests/desktop/test_simulator.cpp"
  ],
  "description": "Implemented VaultService providing AES-256-GCM authenticated encrypted persistence for account credentials (title, username, password, notes) in NVS with fail-closed tamper detection, secure memory wiping upon lock/sleep, and authenticated standalone PBKDF2/AES-GCM backup export and restore.",
  "reason": "Phase 3 Step 1 required establishing a secure, authenticated, tamper-evident storage engine for the password vault before developing the UI and USB HID transmission capabilities.",
  "expected": "Vault entries are stored encrypted in NVS with AES-256-GCM using session key, zeroed out from RAM immediately upon lock or sleep, and can be exported/imported as authenticated backups with tamper resistance.",
  "actual": "Host tests PASS with bit-flip and wrong passphrase rejection (VER-0089). Desktop simulator tests PASS (VER-0090). PlatformIO build PASS with 846,352 bytes flash (0.55% growth, VER-0091). Flashed to COM5 (HWT-0029, VER-0092).",
  "verification": "VER-0089, VER-0090, VER-0091, VER-0092, HWT-0029",
  "risks": "Session key depends on SecurityService unlock state; locking clears active keys from RAM.",
  "commit": "WORKING_TREE",
  "follow_up": "Proceed to Phase 3 Step 2: Password Vault UI (VaultApp) and confirmed USB HID typing integration."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0039",
  "date": "2026-11-04",
  "related": [
    "FEAT-0018",
    "VER-0093",
    "VER-0094",
    "VER-0095",
    "VER-0096",
    "HWT-0030"
  ],
  "action": "FEATURE / PASSWORD VAULT UI / CONFIRMED USB HID TYPING / VERSION BUMP v0.8.0",
  "files": [
    "include/NibruConfig.h",
    "include/BoardConfig.h",
    "src/apps/VaultApp.h",
    "src/apps/VaultApp.cpp",
    "src/services/LocalizationService.h",
    "src/services/LocalizationService.cpp",
    "src/main.cpp",
    "platform/desktop/DesktopMain.cpp",
    "tests/desktop/test_simulator.cpp"
  ],
  "description": "Implemented Phase 3 Step 2: Password Vault UI (VaultApp) and confirmed two-step USB HID credential transmission. Built card-based credential browsing with dynamic masked password display and Show/Hide toggle, on-device account addition via 4-step wizard using TextKeyboard, and confirmed deletion. Built two-touch confirmed USB transmission: first touch arms the button and highlights it visually; second separate touch with lifted finger confirms and types username or password to the connected computer.",
  "reason": "Provide daily encrypted password vault access on device with secure, accidental-keystroke-proof USB HID credential typing.",
  "expected": "VaultApp renders card views; arming requires two distinct taps to type credentials via USB; memory zeroed upon exit; passes host and desktop tests; flashes and boots on COM5.",
  "actual": "Host tests PASS (VER-0093). Desktop simulator regression suite PASS including confirmed USB typing and mask toggle tests (VER-0094). PlatformIO build PASS with 852,864 bytes flash (+1.32% growth, VER-0095). Uploaded and verified boot on COM5 (HWT-0030, VER-0096).",
  "verification": "VER-0093, VER-0094, VER-0095, VER-0096, HWT-0030",
  "risks": "Requires active PIN unlock before viewing or typing credentials; wiping RAM on lock/sleep preserves security.",
  "commit": "WORKING_TREE",
  "follow_up": "User hardware validation: test on-screen password unmasking and two-step USB HID typing into PC text editor."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0040",
  "date": "2026-11-04",
  "related": [
    "FEAT-0018",
    "BUG-0015",
    "VER-0097",
    "VER-0098",
    "VER-0099",
    "VER-0100",
    "HWT-0031"
  ],
  "action": "BUGFIX / NATIVE USB HID MODE / VAULT SESSION RELOAD / VERSION BUMP v0.8.1",
  "files": [
    "platformio.ini",
    "include/NibruConfig.h",
    "include/BoardConfig.h",
    "src/apps/VaultApp.h",
    "src/apps/VaultApp.cpp",
    "src/main.cpp",
    "platform/desktop/DesktopMain.cpp",
    "tests/desktop/test_simulator.cpp",
    "docs/CURRENT_STATE.md"
  ],
  "description": "Fixed BUG-0015: 1) Enabled native TinyUSB composite mode (-DARDUINO_USB_MODE=0, build_unflags = -DARDUINO_USB_MODE=1) in default environment lilygo-t-display-s3, restoring hardware USB HID keyboard emulation alongside CDC Serial so 'Type User' and 'Type Pass' keystrokes are successfully received by connected PC. 2) Implemented VaultApp::clearSession(ctx) to zero memory, cancel active USB tasks, and reset attemptedLoad_ upon lock/sleep. 3) Updated VaultApp::ensureSession(ctx) to automatically load and decrypt credentials as soon as device is unlocked with PIN upon wake. Monotonically bumped firmware version to v0.8.1.",
  "reason": "Resolve reported issues where USB typing was not sending keystrokes to PC and Vault remained locked after waking from sleep.",
  "expected": "Vault automatically reloads credentials when waking and unlocking with PIN; PC detects USB keyboard and receives keystrokes when confirmed with two touches; host, desktop, and PlatformIO builds PASS; uploaded to COM5.",
  "actual": "Host tests PASS (VER-0097). Desktop simulator tests PASS including Vault session clear and wake reload test (VER-0098). PlatformIO release build PASS with 888,352 bytes flash (VER-0099). Successfully uploaded and verified flash write/reset on COM5 (HWT-0031). Documentation check PASS (VER-0100).",
  "verification": "VER-0097, VER-0098, VER-0099, VER-0100, HWT-0031",
  "risks": "Low risk; TinyUSB composite mode maintains CDC serial monitor on COM5 while registering standard USB HID keyboard with host OS.",
  "commit": "WORKING_TREE",
  "follow_up": "User hardware validation: test wake from sleep with PIN (confirming vault unlocks cleanly) and two-touch USB credential typing into Notepad."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0041",
  "date": "2026-11-04",
  "related": [
    "FEAT-0019",
    "VER-0101",
    "VER-0102",
    "VER-0103",
    "VER-0104",
    "HWT-0032"
  ],
  "action": "FEATURE IMPLEMENTATION / WEB PORTAL & DEVICE CONTROL CENTER / VERSION BUMP v0.9.0",
  "files": [
    "src/services/WebPortalService.h",
    "src/services/WebPortalService.cpp",
    "src/apps/SettingsApp.h",
    "src/apps/SettingsApp.cpp",
    "src/services/LocalizationService.h",
    "src/services/LocalizationService.cpp",
    "src/core/ResourceManager.h",
    "src/core/AppContext.h",
    "include/NibruConfig.h",
    "include/BoardConfig.h",
    "src/main.cpp",
    "platform/desktop/DesktopMain.cpp",
    "tests/host/test_web_portal.cpp",
    "tests/desktop/test_simulator.cpp",
    "docs/CURRENT_STATE.md"
  ],
  "description": "Implemented FEAT-0019: Local Web Portal and Device Control Center for NibruOS. 1) Created WebPortalService managing an ephemeral SoftAP with random SSID (NibruOS-XXXX) and WPA2 passphrase, serving an embedded responsive single-page web app in PROGMEM with tabs for Vault credentials (including random password generator) and TOTP 2FA accounts. 2) Provided authenticated REST API (/api/status, /api/vault/add, /api/totp/add) gated by a random 4-digit Web PIN displayed on the LCD and requiring active device master key unlock. 3) Added Web Portal UI into SettingsApp with live countdown timer, prominent Web PIN, and single-touch [TYPE USB] button typing the Wi-Fi password over USB HID to connected host. 4) Added cross-platform host and desktop test suites. Monotonically bumped firmware version to v0.9.0.",
  "reason": "Empower users to easily and securely populate Vault passwords and 2FA accounts from a PC or smartphone browser without external software.",
  "expected": "SoftAP broadcast with random credentials, web portal serving responsive dark UI, authenticated credential encryption into active vault, USB typing of Wi-Fi pass on screen tap, all host, desktop, and PlatformIO builds passing, flash uploaded to device.",
  "actual": "Host tests PASS (VER-0101). Desktop simulator tests PASS (VER-0102). PlatformIO build PASS with 942,800 bytes (VER-0103). Uploaded to physical device via COM5 (HWT-0032). Documentation checks PASS (VER-0104).",
  "verification": "VER-0101, VER-0102, VER-0103, VER-0104, HWT-0032",
  "risks": "SoftAP shuts down after 180s timeout or on lock/sleep/exit to preserve power; credential addition requires both matching Web PIN and active PIN session.",
  "commit": "WORKING_TREE",
  "follow_up": "User tests Web Portal on physical device: connects to SoftAP, loads http://192.168.4.1, adds Vault and TOTP credentials, and tests [TYPE USB] button."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0042",
  "date": "2026-11-04",
  "related": [
    "FEAT-0019",
    "VER-0105",
    "VER-0106",
    "VER-0107",
    "VER-0108",
    "HWT-0033"
  ],
  "action": "WEB PORTAL BRAND ALIGNMENT / HAIDER RAZAQ VISUAL IDENTITY / VERSION BUMP v0.9.1",
  "files": [
    "src/services/WebPortalService.cpp",
    "tools/account_setup/test_portal_preview.html",
    "include/NibruConfig.h",
    "include/BoardConfig.h",
    "src/main.cpp",
    "docs/CURRENT_STATE.md",
    "CHANGELOG.md",
    "docs/DEVELOPMENT_LOG.md",
    "docs/VERIFICATION_REPORT.md",
    "docs/HARDWARE_TEST_LOG.md"
  ],
  "description": "Redesigned the embedded Web Portal (src/services/WebPortalService.cpp) to fully align with the official visual identity of NibruOS and account_setup. Implemented dark navy/cyan neon theme (#0f1954 / #38ffe1 / #080f2c), embedded vector NibruOS SVG logo, RTL Arabic layout, prominent PIN authorization card, interactive tabs for Password Vault and 2FA accounts, client-side password unmasking and random generator, and developer contact footer (07838844400 / Developed by Haider Razaq). Resolved MSVC C2026 string literal token limit by splitting kIndexHtml into adjacent raw string literals (R\"h0(...)h0\" R\"h1(...)h1\" R\"h2(...)h2\"). Monotonically bumped firmware version to v0.9.1.",
  "reason": "Align the device Web Portal with the official NibruOS visual identity, branding, Arabic localization, and developer contact per user request.",
  "expected": "Embedded single-page portal with custom branding, MSVC and ESP32 compiler compatibility, passes all host and desktop tests, builds within partition budget, uploads and resets successfully on physical hardware.",
  "actual": "Host tests PASS (VER-0105). Desktop tests PASS (VER-0106). PlatformIO build PASS with 956,976 bytes (14.6% flash, 19.9% RAM, VER-0107). Uploaded to physical device via COM5 (HWT-0033). Documentation checks PASS (VER-0108).",
  "verification": "VER-0105, VER-0106, VER-0107, VER-0108, HWT-0033",
  "risks": "String literal concatenation occurs at compile-time with zero heap overhead; flash usage grew by 14,176 B (1.5% of flash budget), remaining safely within app0 partition.",
  "commit": "WORKING_TREE",
  "follow_up": "User tests the redesigned Web Portal from phone/PC browser to verify visual identity, branding, and credential additions."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0043",
  "date": "2026-11-04",
  "related": [
    "FEAT-0019",
    "VER-0109",
    "VER-0110",
    "VER-0111",
    "VER-0112",
    "HWT-0034"
  ],
  "action": "WEB PORTAL & ACCOUNT SETUP LINKEDIN INTEGRATION / VERSION BUMP v0.9.2",
  "files": [
    "src/services/WebPortalService.cpp",
    "tools/account_setup/test_portal_preview.html",
    "tools/account_setup/account_setup.html",
    "tools/account_setup/account_setup.css",
    "include/NibruConfig.h",
    "include/BoardConfig.h",
    "src/main.cpp",
    "docs/CURRENT_STATE.md",
    "CHANGELOG.md",
    "docs/DEVELOPMENT_LOG.md",
    "docs/VERIFICATION_REPORT.md",
    "docs/HARDWARE_TEST_LOG.md"
  ],
  "description": "Added official LinkedIn profile links for Haider Razaq (https://www.linkedin.com/in/haiderrazaq/) and Nibru (https://www.linkedin.com/company/nibruiq/) to the embedded Web Portal (src/services/WebPortalService.cpp), test preview (tools/account_setup/test_portal_preview.html), and standalone web setup tool (tools/account_setup/account_setup.html, tools/account_setup/account_setup.css). Integrated vector SVG LinkedIn icons, responsive button pills, and interactive hover animations. Monotonically bumped firmware version to v0.9.2.",
  "reason": "Direct user request to add Haider Razaq and Nibru LinkedIn links to the web pages.",
  "expected": "Clickable, styled LinkedIn badges on all web pages, zero regression on existing endpoints, passes host and desktop tests, compiles and uploads cleanly to device.",
  "actual": "Host tests PASS (VER-0109). Desktop tests PASS (VER-0110). PlatformIO build PASS with 958,896 bytes (14.6% flash, 19.9% RAM, VER-0111). Uploaded to physical device via COM5 (HWT-0034). Documentation checks PASS (VER-0112).",
  "verification": "VER-0109, VER-0110, VER-0111, VER-0112, HWT-0034",
  "risks": "Minimal; flash usage grew by 1,920 B for vector icons and URLs, well within app0 partition capacity.",
  "commit": "WORKING_TREE",
  "follow_up": "User tests the updated Web Portal in browser and confirms LinkedIn links open correctly."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0044",
  "date": "2026-11-04",
  "related": [
    "BUG-0016",
    "VER-0113",
    "VER-0114",
    "VER-0115",
    "VER-0116",
    "HWT-0035"
  ],
  "action": "2FA AUTHENTICATOR UI RESTORATION FIX / VERSION BUMP v0.9.3",
  "files": [
    "src/apps/TotpApp.h",
    "src/apps/TotpApp.cpp",
    "tests/host/test_authenticator.cpp",
    "include/NibruConfig.h",
    "include/BoardConfig.h",
    "src/main.cpp",
    "docs/CURRENT_STATE.md",
    "CHANGELOG.md",
    "docs/DEVELOPMENT_LOG.md",
    "docs/VERIFICATION_REPORT.md",
    "docs/HARDWARE_TEST_LOG.md"
  ],
  "description": "Resolved BUG-0016 where cancelling from ADD or DELETE in 2FA Authenticator caused all UI controls (navigation arrows, bottom bar, ADD/DELETE buttons) to disappear, leaving only the OTP code and seconds timer. Root cause: TotpApp::onBack() cleared the screen via fullRedraw_ but omitted cardDirty_ = true, causing the card rendering block to be skipped. Enforced cardDirty_ = true on onBack(), on onDisplayRestored, and universally whenever fullRedraw_ runs in render(). Modernized ConfirmDelete with TextKeyboard::button styled buttons. Added regression test in test_authenticator.cpp. Monotonically bumped firmware version to v0.9.3.",
  "reason": "Fix reported UI regression where cancelling ADD or DELETE rendered an empty screen with only numbers and seconds.",
  "expected": "Cancelling ADD or DELETE restores complete card layout with navigation arrows, USB button, counter, and action buttons; passes all host and desktop tests; compiles and uploads to hardware.",
  "actual": "Host tests PASS with regression assertions (VER-0113). Desktop tests PASS (VER-0114). PlatformIO build PASS with 958,928 bytes (14.6% flash, 19.9% RAM, VER-0115). Uploaded to physical device via COM5 (HWT-0035). Documentation checks PASS (VER-0116).",
  "verification": "VER-0113, VER-0114, VER-0115, VER-0116, HWT-0035",
  "risks": "Low risk; ensures dirty flag synchronization between screen clearing and view rendering.",
  "commit": "WORKING_TREE",
  "follow_up": "User tests 2FA Authenticator on device: taps ADD or DELETE, cancels, and observes complete UI restored."
}
```


```json
{
  "record_type": "development",
  "id": "DEV-0045",
  "date": "2026-11-04",
  "related": [
    "FEAT-0014",
    "VER-0117",
    "VER-0118",
    "VER-0119",
    "VER-0120",
    "HWT-0036"
  ],
  "action": "2FA USB ACTION CAPSULE UI REDESIGN / LONG-PRESS GATING / PHYSICAL BUTTON TRIGGER / VERSION BUMP v0.9.4",
  "files": [
    "src/apps/TotpApp.h",
    "src/apps/TotpApp.cpp",
    "tests/support/AuthenticatorFlow.h",
    "tests/host/test_authenticator.cpp",
    "include/NibruConfig.h",
    "include/BoardConfig.h",
    "src/main.cpp",
    "docs/CURRENT_STATE.md",
    "CHANGELOG.md",
    "docs/DEVELOPMENT_LOG.md",
    "docs/VERIFICATION_REPORT.md",
    "docs/HARDWARE_TEST_LOG.md",
    "docs/FEATURE_REGISTRY.md"
  ],
  "description": "Redesigned the 2FA Authenticator USB typing button into a sleek Cyber USB Action Capsule (Cyan border, USB badge, instructional prompt, and dynamic state feedback). Added touch long-press gating so short taps guide the user without accidental keystroke injection, and wired the physical BOOT hardware button (GPIO 0) via onPrimaryAction and onActivate to directly trigger USB HID OTP typing. Monotonically bumped firmware version to v0.9.4.",
  "reason": "User requested a modern UI redesign for the USB typing button, long-press protection against accidental taps, and physical hardware button triggering.",
  "expected": "Modern visual layout, short tap guidance, long press triggering code transmission, hardware BOOT button typing code, host tests pass, desktop tests pass, release build fits app0, and device upload succeeds.",
  "actual": "Host tests PASS with long press, short tap guidance, and button trigger assertions (VER-0117). Desktop tests PASS (VER-0118). PlatformIO build PASS with 959,440 bytes (14.6% flash, 19.9% RAM, VER-0119). Uploaded to physical device via COM5 (HWT-0036). Documentation checks PASS (VER-0120).",
  "verification": "VER-0117, VER-0118, VER-0119, VER-0120, HWT-0036",
  "risks": "Minimal; touch long-press uses existing CST816S duration normalization without blocking FreeRTOS event loops.",
  "commit": "WORKING_TREE",
  "follow_up": "User tests 2FA on device: tests short tap, long press hold to send USB digits to PC Notepad, and pressing physical BOOT button to send digits."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0046",
  "date": "2026-11-04T14:30:00+03:00",
  "related": [
    "FEAT-0020",
    "FEAT-0021",
    "FEAT-0022",
    "FEAT-0023"
  ],
  "action": "ADD / ARCHITECTURE / CRYPTO / SECURITY / TOOLING",
  "files": [
    "include/NibruConfig.h",
    "include/BoardConfig.h",
    "src/main.cpp",
    "src/services/LocalizationService.h",
    "src/services/LocalizationService.cpp",
    "src/apps/ClockApp.h",
    "src/apps/ClockApp.cpp",
    "src/apps/GeneratorApp.h",
    "src/apps/GeneratorApp.cpp",
    "src/apps/VaultApp.h",
    "src/apps/VaultApp.cpp",
    "src/core/ResourceManager.h",
    "src/core/AppContext.h",
    "src/services/BleHidService.h",
    "src/services/BleHidService.cpp",
    "src/services/WebPortalService.h",
    "src/services/WebPortalService.cpp",
    "platform/desktop/DesktopMain.cpp",
    "CMakeLists.txt",
    "tests/host/test_main.cpp",
    "tests/host/test_authenticator.cpp",
    "tests/host/test_account_import.cpp",
    "tests/host/test_web_portal.cpp",
    "tests/desktop/test_simulator.cpp"
  ],
  "description": "Implemented suite of 5 MVP features requested by device owner without regressions: Password Generator Tool (GeneratorApp) with random bytes, configurable length/symbols, one-touch USB typing and direct vault save; Cyber Desk Clock App (ClockApp) with large digital time, date, NTP badge, seconds bar and theme cycle; Bluetooth LE HID service (BleHidService) mediated safely by ResourceManager mutex; Vault category filtering (VaultApp) with tap cycling and filtered navigation; Authenticated Web Portal encrypted backup and restore endpoints (/api/vault/backup and /api/vault/restore) with PBKDF2/AES-GCM encryption. Monotonically bumped firmware version to v0.10.0.",
  "reason": "User requested all 5 suggested enhancements in MVP form without touching or breaking any existing working functionality.",
  "expected": "All existing features remain functional, new apps register and render cleanly, desktop and host tests pass with 0 errors, release binary fits app0 partition with ample free internal RAM, and device upload succeeds.",
  "actual": "Host tests PASS with 0 errors (VER-0122). Desktop tests PASS with all modules (VER-0121). PlatformIO build SUCCESS: 975,293 B Flash (14.9%), 65,176 B RAM (19.9%), binary 975,664 B (VER-0123). Uploaded to physical device on COM5 via esptool (HWT-0038). Documentation check PASS (VER-0124).",
  "verification": "VER-0121, VER-0122, VER-0123, VER-0124, HWT-0038",
  "risks": "Minimal; BLE radio remains unadvertised by default to protect internal heap; vault categories do not alter persistent storage format; web backup uses authenticated PBKDF2/AES-GCM.",
  "commit": "WORKING_TREE",
  "follow_up": "User tests the new Password Generator app, Desk Clock, categories filter in Vault, and Web Portal Backup/Restore on the device."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0047",
  "date": "2026-11-04T15:05:00+03:00",
  "related": [
    "FEAT-0020",
    "FEAT-0021",
    "VER-0125",
    "VER-0126",
    "VER-0127",
    "VER-0128",
    "HWT-0039"
  ],
  "action": "PASSWORD GENERATOR VAULT AUTO-UNLOCK & SAVE / CLOCK SCREENSAVER MODE & CONTROLS / VERSION BUMP v0.10.1",
  "files": [
    "include/NibruConfig.h",
    "include/BoardConfig.h",
    "src/main.cpp",
    "src/apps/GeneratorApp.h",
    "src/apps/GeneratorApp.cpp",
    "src/apps/ClockApp.h",
    "src/apps/ClockApp.cpp",
    "src/hardware/DisplayManager.h",
    "src/hardware/DisplayManager.cpp",
    "src/services/SettingsService.h",
    "src/services/SettingsService.cpp",
    "src/apps/SettingsApp.h",
    "src/apps/SettingsApp.cpp",
    "platform/desktop/DesktopMain.cpp",
    "tests/desktop/test_simulator.cpp",
    "docs/CURRENT_STATE.md",
    "CHANGELOG.md",
    "docs/DEVELOPMENT_LOG.md",
    "docs/VERIFICATION_REPORT.md",
    "docs/HARDWARE_TEST_LOG.md",
    "docs/FEATURE_REGISTRY.md"
  ],
  "description": "Resolved two key owner requests: 1) Password Generator auto-unlock & save flow: when [+ SAVE VAULT] is pressed in GeneratorApp without an active key, it seamlessly displays the PIN entry pad, caches pending save state, and upon successful authentication automatically loads the vault and commits the password entry without displaying 'Unlock Vault first!'. If already unlocked, it directly ensures session loading and commits. 2) Cyber Desk Clock Screensaver architecture: integrated DisplayManager screensaver state triggered by inactivity timeout, added dedicated screensaver rendering in ClockApp with animated seconds progress bar, added one-touch [SAVER: ON/OFF] toggle button in ClockApp, and added screensaver toggle option in SettingsApp Display & Screen view. Touch or hardware button wakes the device instantly. Monotonically bumped firmware version to v0.10.1.",
  "reason": "Address device owner issues: prevent 'unlock vault first' blocker when saving generated passwords, and enable controllable clock screensaver.",
  "expected": "Seamless PIN unlock and auto-save in GeneratorApp, full clock screensaver on inactivity with instant wake on touch/button, toggle controls in ClockApp and SettingsApp, all tests PASS, binary fits app0, uploaded to physical device on COM5.",
  "actual": "Host tests PASS with 0 errors (VER-0126). Desktop tests PASS including screensaver and generator auto-save assertions (VER-0125). PlatformIO build SUCCESS: 977,297 B Flash (14.9%), 65,200 B RAM (19.9%), binary 977,664 B (VER-0127). Uploaded to physical device on COM5 via esptool (HWT-0039). Documentation check PASS (VER-0128).",
  "verification": "VER-0125, VER-0126, VER-0127, VER-0128, HWT-0039",
  "risks": "Low; screensaver uses existing non-blocking tick and sleep timer without extra thread allocations or persistent overhead.",
  "commit": "WORKING_TREE",
  "follow_up": "User tests Password Generator save flow with locked/unlocked vault, and observes Clock Screensaver activation on timeout with wake on touch."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0048",
  "date": "2026-11-04T16:15:00+03:00",
  "related": [
    "FEAT-0022",
    "VER-0129",
    "VER-0130",
    "VER-0131",
    "VER-0132",
    "HWT-0040"
  ],
  "action": "BLUETOOTH LE HID WIRELESS KEYBOARD INTEGRATION / 2FA & GENERATOR & VAULT WIRELESS TRANSMISSION / VERSION BUMP v0.10.2",
  "files": [
    "include/NibruConfig.h",
    "include/BoardConfig.h",
    "src/main.cpp",
    "src/services/BleHidService.h",
    "src/services/BleHidService.cpp",
    "src/services/LocalizationService.h",
    "src/services/LocalizationService.cpp",
    "src/apps/SettingsApp.h",
    "src/apps/SettingsApp.cpp",
    "src/apps/TotpApp.h",
    "src/apps/TotpApp.cpp",
    "src/apps/GeneratorApp.h",
    "src/apps/GeneratorApp.cpp",
    "src/apps/VaultApp.h",
    "src/apps/VaultApp.cpp",
    "tests/desktop/test_simulator.cpp",
    "docs/CURRENT_STATE.md",
    "CHANGELOG.md",
    "docs/DEVELOPMENT_LOG.md",
    "docs/VERIFICATION_REPORT.md",
    "docs/HARDWARE_TEST_LOG.md",
    "docs/FEATURE_REGISTRY.md"
  ],
  "description": "Implemented Option 1: Native Bluetooth LE HID Wireless Keyboard stack and interactive controls across NibruOS. 1) BleHidService incorporates native ESP32-S3 BLEHIDDevice advertising 'NibruOS Keyboard' with Just-Works bonding, dynamic 8-byte HID reports, and complete deinitialization on disable to free heap memory. 2) SettingsApp incorporates dedicated Bluetooth subview with real-time pairing status, [ENABLE BLE], [DISABLE BLE], and [TEST KEYBOARD] buttons. 3) TotpApp adds interactive USB/BLE transport toggle to Cyber Action Capsule, typing OTP codes wirelessly to phones/PCs via long-press touch or physical BOOT button. 4) GeneratorApp adds [TYPE BLE] action button typing generated credentials wirelessly. 5) VaultApp adds wireless Bluetooth typing for vault usernames and passwords. Monotonically bumped firmware version to v0.10.2.",
  "reason": "User requested Option 1 (Wireless Bluetooth LE keyboard typing) professionally without disrupting any existing features.",
  "expected": "Full wireless keyboard emulation over BLE, zero disruption to existing features, tests pass on host and desktop, PlatformIO build succeeds with safe memory margins.",
  "actual": "Desktop tests PASS (VER-0129). Host tests PASS (VER-0130). PlatformIO release build PASS: 1,454,077 B Flash (22.2% of 6.25 MB partition), 84,952 B RAM (25.9%), binary 1,454,448 B (VER-0131). Documentation check PASS (VER-0132). Hardware upload pending device connection (HWT-0040).",
  "verification": "VER-0129, VER-0130, VER-0131, VER-0132, HWT-0040",
  "risks": "Low; BLE stack deinitializes completely when disabled, preserving internal RAM for Wi-Fi and applications.",
  "commit": "WORKING_TREE",
  "follow_up": "User connects ESP32-S3 to PC, uploads image on COM5, pairs phone or laptop with 'NibruOS Keyboard', and tests wireless OTP and password entry."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0049",
  "date": "2026-11-04T17:05:00+03:00",
  "related": [
    "FEAT-0022",
    "VER-0133",
    "VER-0134",
    "VER-0135",
    "VER-0136",
    "HWT-0041"
  ],
  "action": "SETTINGS BLUETOOTH TOUCH ROUTING FIX / HARDWARE BOOT BUTTON TRIGGER / SCAN RESPONSE / VERSION BUMP v0.10.3",
  "files": [
    "include/NibruConfig.h",
    "include/BoardConfig.h",
    "src/main.cpp",
    "src/apps/SettingsApp.cpp",
    "src/services/BleHidService.cpp",
    "tests/desktop/test_simulator.cpp",
    "docs/CURRENT_STATE.md",
    "CHANGELOG.md",
    "docs/DEVELOPMENT_LOG.md",
    "docs/VERIFICATION_REPORT.md",
    "docs/HARDWARE_TEST_LOG.md"
  ],
  "description": "Fixed issue where pressing [ENABLE BLE KEYBOARD] did nothing: SettingsApp::onTouch routed SettingsSubView::Bluetooth to renderBluetooth(ctx) instead of handleBluetoothTouch(ctx, ev). Fixed touch routing, expanded touch y-bounds to 98..150 for CST816S tolerance, added hardware BOOT button primary action trigger for BLE toggle/test, added setScanResponse(true) to BleHidService, and added desktop simulator regression test.",
  "reason": "User reported that tapping ENABLE BLE KEYBOARD did not do anything.",
  "expected": "Tapping ENABLE BLE KEYBOARD immediately transitions state to Advertising and updates UI, hardware BOOT button also toggles/tests BLE.",
  "actual": "Desktop simulator verified SettingsApp touch routing and primary action PASS (VER-0133). Host tests PASS (VER-0134). PlatformIO build PASS: 1,564,432 B binary, 23.9% Flash, 26.2% RAM (VER-0135). Documentation checks PASS (VER-0136).",
  "verification": "VER-0133, VER-0134, VER-0135, VER-0136, HWT-0041",
  "risks": "Low; straightforward touch handler routing fix and hardware button addition.",
  "commit": "WORKING_TREE",
  "follow_up": "Flash v0.10.3 to ESP32-S3 on COM5, verify touch response on ENABLE button and test Bluetooth pairing."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0050",
  "date": "2026-11-04T20:35:00+03:00",
  "related": [
    "FEAT-0022",
    "VER-0137",
    "VER-0138",
    "VER-0139",
    "VER-0140",
    "HWT-0042"
  ],
  "action": "FIX BLE HID NULL POINTER CRASH / OUTPUT REPORT & BATTERY ORDER / VERSION BUMP v0.10.4",
  "files": [
    "include/NibruConfig.h",
    "include/BoardConfig.h",
    "src/main.cpp",
    "src/services/BleHidService.cpp",
    "docs/CURRENT_STATE.md",
    "CHANGELOG.md",
    "docs/DEVELOPMENT_LOG.md",
    "docs/VERIFICATION_REPORT.md",
    "docs/HARDWARE_TEST_LOG.md",
    "docs/FEATURE_REGISTRY.md"
  ],
  "description": "Fixed critical ESP32 restart caused by null pointer dereference on BLE initialization: pHid->manufacturer(\"NibruOS\") was called directly, but in ESP32 Arduino BLEHIDDevice m_manufacturerCharacteristic is created lazily in BLECharacteristic* manufacturer(). Fixed to pHid->manufacturer()->setValue(\"NibruOS\"). Added outputReport(1), setBatteryLevel before service start, stack-allocated BLESecurity, and diagnostic serial logging.",
  "reason": "User reported device restarts upon pressing ENABLE BLE KEYBOARD.",
  "expected": "BLE HID stack initializes without crashing or panicking, starts advertising, and awaits connection.",
  "actual": "Desktop tests PASS (VER-0137). Host tests PASS (VER-0138). PlatformIO release build PASS: 1,565,488 B binary, 23.9% Flash, 26.2% RAM (VER-0139). Documentation check PASS (VER-0140). Hardware upload pending device connection (HWT-0042).",
  "verification": "VER-0137, VER-0138, VER-0139, VER-0140, HWT-0042",
  "risks": "Low; eliminated null pointer dereference in BLEHIDDevice setup.",
  "commit": "WORKING_TREE",
  "follow_up": "Connect device to USB, upload v0.10.4 firmware, and verify ENABLE BLE KEYBOARD does not reboot."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0051",
  "date": "2026-11-04T20:58:00+03:00",
  "related": [
    "FEAT-0022",
    "VER-0141",
    "VER-0142",
    "VER-0143",
    "VER-0144",
    "HWT-0043"
  ],
  "action": "FIX BLE ADVERTISING & BATTERY GATTS ORDER / BOOTLOADER RECOVERY / VERSION BUMP v0.10.5",
  "files": [
    "include/NibruConfig.h",
    "include/BoardConfig.h",
    "src/main.cpp",
    "src/services/BleHidService.cpp",
    "docs/CURRENT_STATE.md",
    "CHANGELOG.md",
    "docs/DEVELOPMENT_LOG.md",
    "docs/VERIFICATION_REPORT.md",
    "docs/HARDWARE_TEST_LOG.md",
    "docs/FEATURE_REGISTRY.md"
  ],
  "description": "Standardized BLE HID keyboard initialization against proven T-vK implementation: setAuthenticationMode(ESP_LE_AUTH_REQ_SC_MITM_BOND), setScanResponse(false) to prevent adv buffer overflows, and deferred setBatteryLevel(100) until after startServices and advertising start to eliminate invalid unstarted GATTS notify calls. Resolved native USB bootloader latch via watchdog reset recovery bringing device into active runtime mode (COM6).",
  "reason": "User reported device rebooted or appeared unupdated because esptool left board in ROM bootloader mode and BLE battery notification preceded service startup.",
  "expected": "Board boots cleanly into runtime mode, BLE stack advertises without fault, and user can pair and type over Bluetooth.",
  "actual": "Host tests PASS, desktop tests PASS, PlatformIO build pending, watchdog reset verified to revive runtime COM6.",
  "verification": "VER-0141, VER-0142, VER-0143, VER-0144, HWT-0043",
  "risks": "Low; follows official and field-proven ESP32 BLE HID initialization standard.",
  "commit": "WORKING_TREE",
  "follow_up": "Flash v0.10.5 firmware to device, verify boot into COM6, and verify BLE keyboard pairing and typing."
}
```


```json
{
  "record_type": "development",
  "id": "DEV-0052",
  "date": "2026-11-04T21:26:00+03:00",
  "related": [
    "FEAT-0022",
    "BUG-0017",
    "HWT-0044",
    "HWT-0045"
  ],
  "action": "Handle BLE controller startup failures in v0.10.6",
  "files": [
    "src/services/BleHidService.cpp",
    "src/apps/SettingsApp.cpp",
    "tests/desktop/test_simulator.cpp",
    "include/BoardConfig.h",
    "include/NibruConfig.h",
    "src/main.cpp",
    "docs/performance/BLE_STARTUP_RESOURCE_IMPACT.md"
  ],
  "description": "Use the pinned controller configuration with a checked initialization result, verify controller/host startup, release the lease on failure, and distinguish startup failures from a busy radio in Settings.",
  "reason": "Owner reproduced loss of the device during BLE initialization, before server creation.",
  "expected": "A returned startup error leaves the device usable and BLE disabled; successful startup follows the existing HID sequence.",
  "actual": "Injected failure regression RED before and GREEN after. Host, simulator, target build, image capacity and docs checks passed. New physical image verification pending.",
  "verification": "VER-0145 through VER-0152, HWT-0044, HWT-0045",
  "risks": "Underlying hardware reset reason, BLE runtime resource headroom, and pairing remain unverified.",
  "commit": "WORKING_TREE",
  "follow_up": "Upload only when authorized and complete HWT-0045."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0053",
  "date": "2026-11-04",
  "related": [
    "BUG-0017",
    "FEAT-0022",
    "HWT-0048",
    "HWT-0049"
  ],
  "action": "Repair Wi-Fi/BLE coexistence panic in v0.10.7",
  "files": [
    "src/services/WiFiService.h",
    "src/services/WiFiService.cpp",
    "src/services/BleHidService.cpp",
    "tests/host/test_main.cpp",
    "tests/desktop/test_simulator.cpp",
    "docs/performance/BLE_STARTUP_RESOURCE_IMPACT.md"
  ],
  "description": "Read the crash-only partition and decode the backtrace locally against the v0.10.6 ELF. Use Wi-Fi modem sleep throughout BLE startup and operation. Check the driver mode before BLE init, preserve modem sleep across STA reconnect and restore the prior policy after the controller stops.",
  "reason": "HWT-0048 found coex_core_enable abort during esp_bt_controller_enable. The service set WIFI_PS_NONE, matching Espressif issue 9595.",
  "expected": "BLE activation succeeds alongside Wi-Fi without an abort. Cleanly reject activation if modem sleep cannot be established.",
  "actual": "Host regression failed before the repair and passed after it. Simulator, target build, and image checks passed.",
  "verification": "VER-0153 through VER-0160; HWT-0049 tracks full device acceptance.",
  "risks": "Concurrent Wi-Fi modem sleep may change latency or power; long-run resource metrics and pairing require device evidence.",
  "commit": "WORKING_TREE",
  "follow_up": "Verify BLE enable with Wi-Fi on the board and track pairing and 20-cycle profiling separately."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0054",
  "date": "2026-11-04",
  "related": [
    "BUG-0017",
    "FEAT-0022",
    "HWT-0050",
    "HWT-0051"
  ],
  "action": "Verify v0.10.7 app upload and BLE activation on device",
  "files": [
    "docs/HARDWARE_TEST_LOG.md",
    "docs/CURRENT_STATE.md",
    "docs/KNOWN_ISSUES.md",
    "docs/FEATURE_REGISTRY.md"
  ],
  "description": "Installed the verified app0 image and monitored the same Bluetooth activation path on the owner's board.",
  "reason": "Validate the specific activation reboot on physical hardware.",
  "expected": "BLE startup reaches advertising and the board stays active.",
  "actual": "HWT-0050 recorded app0 write and hash verification. HWT-0051 reached advertising and retained runtime CDC after the original activation action.",
  "verification": "HWT-0050, HWT-0051",
  "risks": "RF discovery, pairing, typing, Wi-Fi continuity and extended resource measurements remain in HWT-0049.",
  "commit": "WORKING_TREE",
  "follow_up": "Complete the extended acceptance in HWT-0049."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0055",
  "date": "2026-11-04",
  "related": [
    "BUG-0018",
    "FEAT-0022",
    "HWT-0052",
    "HWT-0053"
  ],
  "action": "Instrument Windows BLE link loss in v0.10.8",
  "files": [
    "src/services/BleHidService.cpp",
    "docs/performance/BLE_STARTUP_RESOURCE_IMPACT.md"
  ],
  "description": "Log GATT connect and disconnect, GAP authentication and advertising result. Preserve BLE lifecycle and security configuration. Unregister the GAP hook before deinitialization.",
  "reason": "Owner reported BLE link unavailable in Generator after Windows pairing succeeded.",
  "expected": "Capture the disconnect reason without logging a device address, passkey or generated password.",
  "actual": "Build and regression checks passed. Windows confirms pairing on v0.10.7. v0.10.8 hardware reproduction pending.",
  "verification": "VER-0161 through VER-0165",
  "risks": "Sparse diagnostic writes from BLE callbacks can perturb timing; hardware measurements pending.",
  "commit": "WORKING_TREE",
  "follow_up": "Upload with authorization and capture HWT-0053."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0056",
  "date": "2026-11-04",
  "related": [
    "BUG-0018",
    "FEAT-0022",
    "HWT-0055",
    "HWT-0056"
  ],
  "action": "Add Bluetooth Home card and separate link from HID readiness in v0.11.0",
  "files": [
    "src/apps/BluetoothApp.h",
    "src/apps/BluetoothApp.cpp",
    "src/apps/SettingsApp.cpp",
    "src/services/BleHidService.cpp",
    "src/apps/GeneratorApp.cpp",
    "src/main.cpp",
    "platform/desktop/DesktopMain.cpp",
    "CMakeLists.txt",
    "tests/host/test_main.cpp",
    "tests/desktop/test_simulator.cpp"
  ],
  "description": "Extract shared Bluetooth app controls, register them beside Wi-Fi, retain Settings entry, request encryption at GATT connect and gate keyboard readiness on the encrypted input-report CCCD subscription. Refresh Generator status on readiness changes.",
  "reason": "User requested first-level Bluetooth controls and reported a green indicator immediately before Windows terminated the link.",
  "expected": "Both entry points work. A transient GATT link stays in setup state and cannot send keys.",
  "actual": "RED before change and GREEN after for navigation and readiness. Firmware and desktop entry point built.",
  "verification": "VER-0166 through VER-0174; HWT-0056 remains physical acceptance.",
  "risks": "Windows link stability and CCCD renewal on bonded reconnect need physical evidence. Link readiness does not establish peer identity.",
  "commit": "WORKING_TREE",
  "follow_up": "Test new pairing, reconnect, and synthetic HID input on the board with Windows."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0057",
  "date": "2026-11-04",
  "related": [
    "FEAT-0022",
    "BUG-0018",
    "HWT-0058",
    "HWT-0059"
  ],
  "action": "Enable Windows HID enumeration in v0.11.1",
  "files": [
    "src/services/BleHidService.cpp",
    "src/services/BleHidService.h",
    "tests/host/test_main.cpp",
    "tests/desktop/test_simulator.cpp",
    "docs/performance/BLE_STARTUP_RESOURCE_IMPACT.md"
  ],
  "description": "Expose neutral report attributes, report references and CCCD during enumeration. Require successful authentication of the matching GATT peer and subscription on that connection before sending. Send only via that GATT connection and do not place keystrokes in the readable attribute. Encode advertising metadata into one bounded packet.",
  "reason": "On v0.11.0 Windows authenticated after re-pairing but showed battery without HID and never enabled the keyboard CCCD.",
  "expected": "Windows can discover and subscribe to HID, while unauthenticated links cannot send keys.",
  "actual": "Host regression RED and GREEN. Simulator, corrected ESP32 build and capacity check passed. Device result pending.",
  "verification": "VER-0175 through VER-0181; HWT-0058 reproduced the issue; HWT-0059 is the verification procedure.",
  "risks": "Windows HID enumeration and notification behavior are unverified on v0.11.1. Bonded CCCD persistence and heap across repeated toggles need testing.",
  "commit": "WORKING_TREE",
  "follow_up": "Run HWT-0059 on the authorized image."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0058",
  "date": "2026-11-04",
  "related": [
    "BUG-0018",
    "FEAT-0022",
    "HWT-0061",
    "HWT-0062",
    "HWT-0063"
  ],
  "action": "Display an ephemeral BLE pairing code in v0.12.0",
  "files": [
    "src/services/BleHidService.cpp",
    "src/services/BleHidService.h",
    "src/apps/BluetoothApp.cpp",
    "src/apps/BluetoothApp.h",
    "tests/host/test_main.cpp",
    "tests/desktop/test_simulator.cpp"
  ],
  "description": "Configure DisplayOnly authenticated pairing, show the controller-provided passkey in the existing Bluetooth screen, and clear the displayed value on authentication, disconnect and disable. Keep report sending gated on authentication and subscription.",
  "reason": "Phone stayed connected and subscribed but the previous configuration requested MITM with no input or display capability. It failed authentication. Windows also failed to complete the prior setup.",
  "expected": "A user can complete the peer prompt by entering the displayed code. Reports require successful authentication and subscription.",
  "actual": "Host, simulator and target builds passed. Physical passkey negotiation pending.",
  "verification": "VER-0182 through VER-0187, HWT-0063",
  "risks": "Old bonds may require re-pairing. Pinned BLE SDK retains allocations across controller restarts, and Windows and phone pairing still need physical validation.",
  "commit": "WORKING_TREE",
  "follow_up": "Run HWT-0063 and complete the device resource scenarios."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0059",
  "date": "2026-11-04",
  "related": [
    "BUG-0018",
    "FEAT-0022",
    "HWT-0064",
    "HWT-0065",
    "HWT-0066"
  ],
  "action": "Install and test v0.12.0 display-assisted pairing",
  "files": [
    "docs/HARDWARE_TEST_LOG.md",
    "docs/CURRENT_STATE.md",
    "docs/KNOWN_ISSUES.md",
    "docs/TEST_MATRIX.md"
  ],
  "description": "Authorized app-only upload and serial capture; compare the Windows pairing event and owner phone result.",
  "reason": "Determine whether display-assisted authentication enables keyboard typing.",
  "expected": "Passkey completion and HID subscription on the intended host.",
  "actual": "Windows authenticated once but did not subscribe. Phone reported incorrect PIN/passkey immediately, with no code input field. Test text delivery remains unverified.",
  "verification": "HWT-0064 upload PASS; HWT-0065 and HWT-0066 functional FAIL.",
  "risks": "Immediate phone rejection and Windows HID subscription remain separate observations. Peer identity in the serial log is not captured.",
  "commit": "WORKING_TREE",
  "follow_up": "Isolate hosts and inspect pairing/CCCD traces without inferring the cause from a generic phone message."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0060",
  "date": "2026-11-04",
  "related": [
    "BUG-0018",
    "FEAT-0022"
  ],
  "action": "Correct BLE Device Information PnP ID serialization in v0.12.1",
  "files": [
    "src/services/BleHidService.cpp"
  ],
  "description": "Write the existing PnP characteristic in little-endian order, preserving vendor, product and version values.",
  "reason": "The bundled Bluedroid helper emits high bytes first. The NimBLE HID implementation used by Bruce emits low bytes first.",
  "expected": "Hosts reading 0x2A50 receive 02 02 E5 11 A1 10 02.",
  "actual": "Corrected characteristic initialization. No physical peer read or pairing was performed.",
  "verification": "VER-0188, VER-0189, VER-0190",
  "risks": "Correct metadata may not resolve Windows HID subscription or the separate phone pairing rejection.",
  "commit": "WORKING_TREE",
  "follow_up": "Read 0x2A50 on the device, then test fresh pairing and actual keyboard typing with Wi-Fi active."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0061",
  "date": "2026-11-04",
  "related": [
    "FEAT-0024"
  ],
  "action": "Implement Beacon Lab in v0.13.0",
  "files": [
    "src/apps/BeaconLabApp.cpp",
    "src/services/WiFiService.cpp",
    "src/main.cpp",
    "platform/desktop/DesktopMain.cpp"
  ],
  "description": "Provide seven count positions and a ten-second synthetic beacon session on the current channel with an exclusive radio lease.",
  "reason": "Add a controlled test of multiple visible SSIDs inside the existing firmware.",
  "expected": "A receiver observes synthetic SSIDs and transmission stops by timeout or on leaving the app.",
  "actual": "Implemented the shared UI and ESP32 transmit path. Physical RF reception was not measured.",
  "verification": "VER-0191 through VER-0194",
  "risks": "Receiver scan cadence and ESP32 transmit acceptance require hardware testing. Wi-Fi airtime and heap impact have not been measured.",
  "commit": "WORKING_TREE",
  "follow_up": "Observe beacon frames from a second receiver in an isolated lab. Run lifecycle and combined resource scenarios."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0062",
  "date": "2026-11-04",
  "related": [
    "FEAT-0024"
  ],
  "action": "Extend Beacon Lab in v0.14.0",
  "files": [
    "src/apps/BeaconLabApp.cpp",
    "src/services/WiFiService.cpp",
    "tests/desktop/test_simulator.cpp"
  ],
  "description": "Replace Orchid template with 48 fictional names. Add open and secured steppers and a 5 to 60 second session setting.",
  "reason": "Allow different test mixes and durations without changing idle radio behavior.",
  "expected": "Distinct names, requested open/secured split, and bounded stop.",
  "actual": "Host, simulator and ESP32 build passed. Physical advertisement parsing has not been tested.",
  "verification": "VER-0195, VER-0196",
  "risks": "Scan cache can outlast transmission. Longer sessions increase active radio time.",
  "commit": "WORKING_TREE",
  "follow_up": "Capture open and WPA2-advertised beacons on a second receiver and verify timeout and radio recovery."
}
```

```json
{
  "record_type": "development",
  "id": "DEV-0063",
  "date": "2026-11-04",
  "related": [
    "FEAT-0024"
  ],
  "action": "Improve Beacon Lab observation in v0.14.1",
  "files": [
    "src/apps/BeaconLabApp.cpp",
    "src/services/WiFiService.cpp",
    "tests/desktop/test_simulator.cpp"
  ],
  "description": "Schedule at most three names per transmit tick and report the number of distinct names accepted by the Wi-Fi driver.",
  "reason": "On v0.14.0 the user saw six names on phone and PC, and Windows polling observed two distinct names during a separate session. The selected count and raw over-air frame count were not captured.",
  "expected": "A faster list pass and a driver acceptance count that distinguishes an incomplete submission from a partial scan result.",
  "actual": "Host, simulator and ESP32 build passed. Physical visibility remains unverified.",
  "verification": "VER-0197, VER-0198",
  "risks": "Higher active radio request rate and unknown receiver filtering. Driver acceptance does not verify RF delivery.",
  "commit": "WORKING_TREE",
  "follow_up": "Compare TX accepted, serial errors and distinct over-air SSIDs on the transmit channel. Measure resources before accepting the rate change."
}
```
