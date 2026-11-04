# Feature registry

Recorded 2026-11-01. Implementation states are independent of verification.
The three `registration` values match `launcher.registerApp(...)` in
[src/main.cpp](../src/main.cpp). No deprecated or removed features are evidenced
at this baseline; retain their identities if removal happens later.

See [workflow](ENGINEERING_WORKFLOW.md), [matrix](TEST_MATRIX.md),
[issues](KNOWN_ISSUES.md), and [history](DEVELOPMENT_LOG.md).

```json
{
  "record_type": "feature",
  "id": "FEAT-0001",
  "name": "Home launcher and App lifecycle",
  "purpose": "Route one active application, input and invalidated rendering",
  "module": "src/core",
  "files": [
    "src/main.cpp",
    "src/core/App.h",
    "src/core/AppContext.h",
    "src/core/AppManager.cpp",
    "src/core/ResourceManager.cpp",
    "src/core/EventTypes.h",
    "src/core/IApp.h",
    "src/ui/HomeScreen.cpp",
    "src/ui/HomeScreen.h",
    "src/ui/Theme.h"
  ],
  "dependencies": [
    "Arduino",
    "DisplayManager",
    "TouchManager",
    "ButtonManager"
  ],
  "hardware": "Integrated display/touch/buttons",
  "gpio": "Uses shared reserved input/display pins; no extra GPIO",
  "lifecycle": "12 fixed app slots, 3 tiles/page; manager calls exit before switching; upper-left Back and GPIO14 return Home",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "VER-0001",
    "host": "VER-0002",
    "hardware": "NOT VERIFIED"
  },
  "issues": [],
  "history": [
    "DEV-0001",
    "DEV-0021"
  ]
}
```

```json
{
  "record_type": "feature",
  "id": "FEAT-0002",
  "name": "System Info",
  "purpose": "Show ESP model, CPU, flash, free/total PSRAM, heap and uptime",
  "module": "src/apps/SystemInfoApp",
  "files": [
    "src/apps/SystemInfoApp.cpp",
    "src/apps/SystemInfoApp.h"
  ],
  "dependencies": [
    "Arduino ESP API",
    "DisplayManager"
  ],
  "hardware": "Integrated board/memory/display",
  "gpio": "No additional pins",
  "lifecycle": "Invalidate every 1000ms; no radio lease or dynamically owned buffer",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "VER-0001",
    "host": "VER-0002",
    "hardware": "NOT VERIFIED"
  },
  "issues": [],
  "history": [
    "DEV-0001"
  ]
}
```

```json
{
  "record_type": "feature",
  "id": "FEAT-0003",
  "name": "Touch Test",
  "purpose": "Show raw and mapped coordinates and landscape gesture labels",
  "module": "src/apps/TouchTestApp",
  "files": [
    "src/apps/TouchTestApp.cpp",
    "src/apps/TouchTestApp.h"
  ],
  "dependencies": [
    "FEAT-0005",
    "DisplayManager"
  ],
  "hardware": "Integrated CST816S and LCD",
  "gpio": "Shared touch 18/17/21/16; no extra pins",
  "lifecycle": "App-local last event; shell owns Back; shared TouchManager polls device",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "VER-0024",
    "host": "VER-0023",
    "hardware": "HWT-0010 owner touch/flicker FAIL before repair; HWT-0012 repaired acceptance NOT RUN"
  },
  "issues": [
    "BUG-0008"
  ],
  "history": [
    "DEV-0001",
    "DEV-0017"
  ]
}
```

```json
{
  "record_type": "feature",
  "id": "FEAT-0004",
  "name": "Wi-Fi Scanner and WiFiService",
  "purpose": "Read-only async 2.4GHz AP discovery, 12 records, 3 visible rows, rescan and scrolling",
  "module": "src/apps/WiFiScannerApp + src/services/WiFiService",
  "files": [
    "src/apps/WiFiScannerApp.cpp",
    "src/apps/WiFiScannerApp.h",
    "src/services/WiFiService.cpp",
    "src/services/WiFiService.h"
  ],
  "dependencies": [
    "Arduino WiFi",
    "esp_wifi",
    "ResourceManager"
  ],
  "hardware": "Integrated 2.4GHz radio; antenna path unverified",
  "gpio": "No external GPIO; supply/RF quality still matters",
  "lifecycle": "Acquire ResourceOwner::WiFiScanner on enter; 650ms warmup, 11s scan deadline, 300ms/channel, 2 attempts; onExit shuts down and releases; rescan ignored while busy",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "VER-0001",
    "host": "VER-0002",
    "hardware": "NOT VERIFIED"
  },
  "issues": [
    "BUG-0001"
  ],
  "history": [
    "DEV-0001"
  ]
}
```

```json
{
  "record_type": "feature",
  "id": "FEAT-0005",
  "name": "Display, CST816S and physical input drivers",
  "purpose": "Initialize shared hardware and transform/debounce input",
  "module": "src/hardware",
  "files": [
    "src/hardware/DisplayManager.cpp",
    "src/hardware/TouchManager.cpp",
    "src/hardware/ButtonManager.cpp",
    "include/BoardConfig.h",
    "platformio.ini",
    "include/PinDefinitions.h",
    "include/NibruConfig.h"
  ],
  "dependencies": [
    "TFT_eSPI 2.5.43",
    "CST816S",
    "Arduino"
  ],
  "hardware": "Integrated ST7789V/CST816S/buttons",
  "gpio": "All LCD/touch/button reservations in HARDWARE_PINOUT; power15/backlight38; active LOW buttons0/14",
  "lifecycle": "Shared hardware init preserved; contact phase/gesture latch and coordinate transform; physical button debounce unchanged",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "VER-0024",
    "host": "VER-0023",
    "hardware": "HWT-0010 owner touch/flicker FAIL before repair; HWT-0012 repaired acceptance NOT RUN"
  },
  "issues": [
    "BUG-0002",
    "BUG-0008"
  ],
  "history": [
    "DEV-0001",
    "DEV-0017",
    "DEV-0021"
  ]
}
```

```json
{
  "record_type": "feature",
  "id": "FEAT-0006",
  "name": "Standalone radio isolation diagnostic",
  "purpose": "Temporarily isolate synchronous Wi-Fi scanning from all launcher and display code",
  "module": "tools/wifi_scan_diagnostic",
  "files": [
    "tools/wifi_scan_diagnostic/src/main.cpp",
    "tools/wifi_scan_diagnostic/wifi_scan_diagnostic.ino",
    "tools/wifi_scan_diagnostic/platformio.ini"
  ],
  "dependencies": [
    "Arduino WiFi",
    "espressif32@6.5.0"
  ],
  "hardware": "Same board/radio; replaces running firmware when uploaded",
  "gpio": "Sets GPIO15 HIGH; no touch or display initialization",
  "lifecycle": "Separate project, intentionally blocking scan/loop delays; must never be placed in UI event loop; restore NibruOS after diagnostic",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "NOT VERIFIED (separate project not built this session)",
    "host": "NOT VERIFIED",
    "hardware": "NOT VERIFIED"
  },
  "issues": [],
  "history": [
    "DEV-0001",
    "DEV-0021"
  ]
}
```

```json
{
  "record_type": "feature",
  "id": "FEAT-0007",
  "name": "External SPI MicroSD storage",
  "purpose": "Future removable storage; no SDCardService, initialization, read/write or app exists",
  "module": "Proposed only; no implementation directory",
  "files": [
    "docs/HARDWARE_INVENTORY.md",
    "docs/HARDWARE_PINOUT.md"
  ],
  "dependencies": [
    "Future SPI/SD driver and shared storage ownership design"
  ],
  "hardware": "Owner-acquired module; physical connection UNKNOWN",
  "gpio": "PROPOSED/UNVERIFIED CS10/MOSI11/SCK12/MISO13, 3.3V compatible module + GND; conflicts with optional shield must be checked",
  "lifecycle": "Future bounded I/O and onExit cleanup; unmount/error/removal behavior undecided",
  "implementation": "PLANNED",
  "verification": {
    "build": "NOT VERIFIED (no implementation)",
    "host": "NOT VERIFIED",
    "hardware": "HWT-0006"
  },
  "issues": [
    "BUG-0004"
  ],
  "history": [
    "DEV-0001"
  ]
}
```

```json
{
  "record_type": "feature",
  "id": "FEAT-0008",
  "name": "Engineering memory and validation",
  "purpose": "Preserve current state, evidence, history, issues, dependencies and maintenance workflow",
  "module": "docs + tools/check_docs.py",
  "files": [
    "AGENTS.md",
    "docs/ENGINEERING_WORKFLOW.md",
    "tools/check_docs.py",
    "tests/docs/test_check_docs.py",
    ".github/workflows/validate.yml",
    "docs/RESOURCE_BUDGET.md",
    "docs/MEMORY_PROFILING.md",
    "docs/performance/BASELINE.md",
    "tools/check_resources.py"
  ],
  "dependencies": [
    "Python 3.10+ standard library",
    "Git for code history"
  ],
  "hardware": "None; repository tooling only",
  "gpio": "No GPIO",
  "lifecycle": "Read/validate repository without modifying firmware; maintain logs by explicit human/agent records",
  "implementation": "IMPLEMENTED",
  "verification": {
    "documentation": "VER-0018",
    "hardware": "NOT APPLICABLE (repository tooling)"
  },
  "issues": [
    "BUG-0006"
  ],
  "history": [
    "DEV-0004",
    "DEV-0005",
    "DEV-0029"
  ]
}
```


```json
{
  "record_type": "feature",
  "id": "FEAT-0009",
  "name": "Wi-Fi AP Connection and On-Screen Keyboard",
  "purpose": "Select scanned AP, enter visible password via on-screen touch keyboard, and connect/disconnect non-blocking",
  "module": "src/apps/WiFiScannerApp + src/services/WiFiService",
  "files": [
    "src/apps/WiFiScannerApp.cpp",
    "src/apps/WiFiScannerApp.h",
    "src/services/WiFiService.cpp",
    "src/services/WiFiService.h",
    "src/ui/TextKeyboard.h"
  ],
  "dependencies": [
    "FEAT-0004",
    "ResourceManager"
  ],
  "hardware": "Integrated 2.4GHz radio and CST816S capacitive touch",
  "gpio": "Uses existing reserved pins; no additional GPIO",
  "lifecycle": "Selection opens plaintext visible touch keyboard; non-blocking connection with 15s timeout; disconnect on onExit or user action; shared QWERTY/digit/shift/symbol presentation reused with 2FA",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "VER-0045",
    "host": "VER-0042; VER-0043 shared keyboard",
    "hardware": "NOT VERIFIED"
  },
  "issues": [],
  "history": [
    "DEV-0006",
    "DEV-0025"
  ]
}
```

```json
{
  "record_type": "feature",
  "id": "FEAT-0010",
  "name": "Settings App",
  "purpose": "Essential Settings: scroll/focus/activate, validated persistent display preferences, diagnostics and confirmed scoped device actions",
  "module": "src/apps/SettingsApp",
  "files": [
    "src/apps/SettingsApp.cpp",
    "src/apps/SettingsApp.h"
  ],
  "dependencies": [
    "FEAT-0001",
    "FEAT-0002",
    "FEAT-0003"
  ],
  "hardware": "Display, CST816S capacitive touch",
  "gpio": "GPIO14 Back, BOOT 0",
  "lifecycle": "No owned radio/task/buffer; short BOOT focuses, hold activates; Back cancels then menu/Home; current SettingsService owns preference blob",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "VER-0024",
    "host": "VER-0023",
    "hardware": "HWT-0010 owner touch/flicker FAIL before repair; HWT-0012 repaired acceptance NOT RUN"
  },
  "issues": [
    "BUG-0007",
    "BUG-0008"
  ],
  "history": [
    "DEV-0007",
    "DEV-0014",
    "DEV-0017"
  ],
  "registration": "settingsApp"
}
```

```json
{
  "record_type": "feature",
  "id": "FEAT-0011",
  "name": "Wi-Fi Hub and Saved Networks",
  "purpose": "Top-level Wi-Fi hub containing scanner and saved networks management with persistent NVS credentials and auto-connect on boot",
  "module": "src/apps/WiFiApp",
  "files": [
    "src/apps/WiFiApp.cpp",
    "src/apps/WiFiApp.h",
    "src/services/WiFiService.cpp",
    "src/services/WiFiService.h"
  ],
  "dependencies": [
    "FEAT-0001",
    "FEAT-0004",
    "FEAT-0009"
  ],
  "hardware": "Integrated 2.4GHz radio and CST816S capacitive touch",
  "gpio": "Uses existing reserved pins; no additional GPIO",
  "lifecycle": "Hub container view delegates to Scan Networks and Saved Networks; auto-connects to known AP on boot; non-volatile NVS storage",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "VER-0007",
    "host": "VER-0007",
    "hardware": "NOT VERIFIED"
  },
  "issues": [],
  "history": [
    "DEV-0008"
  ],
  "registration": "wifiApp"
}
```

```json
{
  "record_type": "feature",
  "id": "FEAT-0012",
  "name": "Display Controls, Sleep Mode, Screen Orientation and PIN Security",
  "purpose": "Existing display controls, inactivity sleep and PIN security (Security retained, not expanded by Settings completion)",
  "module": "src/hardware/DisplayManager + src/services/SecurityService + src/apps/SettingsApp",
  "files": [
    "src/hardware/DisplayManager.cpp",
    "src/hardware/DisplayManager.h",
    "src/services/SecurityService.cpp",
    "src/services/SecurityService.h",
    "src/apps/SettingsApp.cpp",
    "src/apps/SettingsApp.h",
    "src/main.cpp"
  ],
  "dependencies": [
    "FEAT-0005",
    "FEAT-0010"
  ],
  "hardware": "Shared ST7789/CST816S, AW9364 backlight on GPIO38",
  "gpio": "GPIO38 pulse control by default; GPIO15 power, buttons0/14",
  "lifecycle": "Shared DisplayManager inactivity sleep/wake; existing SecurityService PIN gate remains; LEDC is opt-in only",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "VER-0035",
    "host": "VER-0033; VER-0034",
    "hardware": "HWT-0016 upload/hash/reset PASS only; HWT-0015 runtime NOT RUN"
  },
  "issues": [],
  "history": [
    "DEV-0010",
    "DEV-0023",
    "DEV-0024",
    "DEV-0028"
  ]
}
```

```json
{
  "record_type": "feature",
  "id": "FEAT-0013",
  "name": "Essential Settings v0.1 and Memory Profiler",
  "purpose": "Essential Settings: scroll/focus/activate, validated persistent display preferences, diagnostics and confirmed scoped device actions",
  "module": "src/services/SettingsService + src/core/MemoryProfiler + src/apps/SettingsApp",
  "files": [
    "src/services/SettingsService.cpp",
    "src/services/SettingsService.h",
    "src/core/MemoryProfiler.cpp",
    "src/core/MemoryProfiler.h",
    "src/apps/SettingsApp.cpp",
    "src/apps/SettingsApp.h",
    "src/hardware/DisplayManager.cpp",
    "src/hardware/DisplayManager.h"
  ],
  "dependencies": [
    "FEAT-0005",
    "FEAT-0010"
  ],
  "hardware": "AW9364 pulse-controlled backlight driver on GPIO38, ST7789 parallel LCD, CST816S touch",
  "gpio": "No extra GPIO; default AW9364 GPIO38 retained, opt-in LEDC channel0 build requires physical acceptance",
  "lifecycle": "One gesture/contact, partial/no-op repaint, full view/overlay restore; contact-start guarded confirmations; no owned radio/task/framebuffer",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "VER-0024",
    "host": "VER-0023",
    "hardware": "HWT-0010 owner touch/flicker FAIL before repair; HWT-0012 repaired acceptance NOT RUN"
  },
  "issues": [
    "BUG-0007",
    "BUG-0008"
  ],
  "history": [
    "DEV-0011",
    "DEV-0014",
    "DEV-0015",
    "DEV-0017",
    "DEV-0028"
  ]
}
```

```json
{
  "record_type": "feature",
  "id": "FEAT-0014",
  "name": "Hardware 2FA Authenticator & Crypto Vault",
  "purpose": "PIN-protected TOTP entry via shared QWERTY or paired USB/Wi-Fi paste, bounded authenticated persistence, valid time and optional asynchronous HID",
  "module": "src/services/CryptoService + src/services/TimeService + src/services/TotpService + src/services/UsbHidService + src/apps/TotpApp + src/services/AccountImportService + src/ui/TextKeyboard",
  "files": [
    "src/services/CryptoService.cpp",
    "src/services/CryptoService.h",
    "src/services/SecurityService.cpp",
    "src/services/SecurityService.h",
    "src/services/TimeService.cpp",
    "src/services/TimeService.h",
    "src/services/TotpService.cpp",
    "src/services/TotpService.h",
    "src/services/UsbHidService.cpp",
    "src/services/UsbHidService.h",
    "src/apps/TotpApp.cpp",
    "src/apps/TotpApp.h",
    "src/main.cpp",
    "src/ui/Base32Keyboard.h",
    "tests/host/test_authenticator.cpp",
    "tests/support/AuthenticatorFlow.h",
    "docs/AUTHENTICATOR.md",
    "src/services/AccountImportService.cpp",
    "src/services/AccountImportService.h",
    "src/ui/TextKeyboard.h",
    "tests/host/test_account_import.cpp",
    "tests/support/AccountImportFixture.h",
    "tools/account_setup.py",
    "tools/account_setup/account_setup.mjs",
    "docs/ACCOUNT_SETUP.md"
  ],
  "dependencies": [
    "FEAT-0001",
    "FEAT-0005",
    "FEAT-0010"
  ],
  "hardware": "Integrated ST7789V parallel LCD, CST816S touch, Native USB OTG",
  "gpio": "Shared display/touch GPIOs, Native USB D+/D-",
  "lifecycle": "Explicit PIN session; load on entry; transactional edits; wipe plaintext/editor and cancel HID on exit/lock/sleep; default USB Serial/JTAG, opt-in HID; explicit 120s receive, one candidate, later physical SAVE; close/wipe/release on cancel/expiry/lock/sleep/exit",
  "registration": "totpApp",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "VER-0045",
    "host": "VER-0042; VER-0043; VER-0044",
    "hardware": "HWT-0036 upload PASS; HWT-0037 owner physical 2FA UI and USB typing PASS"
  },
  "issues": [
    "BUG-0009",
    "BUG-0010",
    "BUG-0011"
  ],
  "history": [
    "DEV-0012",
    "DEV-0023",
    "DEV-0024",
    "DEV-0025",
    "DEV-0026",
    "DEV-0045"
  ]
}
```
```json
{
  "record_type": "feature",
  "id": "FEAT-0015",
  "name": "Desktop Simulator & Cross-Platform Host Target",
  "purpose": "Native Windows/desktop simulation environment reusing shared C++ application logic, navigation, UI layout and settings with SDL2 rendering and mock hardware services",
  "module": "platform/desktop",
  "files": [
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
    "CMakeLists.txt",
    "tests/desktop/test_simulator.cpp"
  ],
  "dependencies": [
    "FEAT-0001",
    "FEAT-0002",
    "FEAT-0003",
    "FEAT-0004",
    "FEAT-0008",
    "FEAT-0010",
    "FEAT-0014"
  ],
  "hardware": "Desktop host GPU/Display via SDL2; simulated ST7789 320x170 and CST816S touch",
  "gpio": "No GPIO allocation; simulated in software",
  "lifecycle": "Full application lifecycle, Home Launcher routing, mouse-to-touch events, keyboard-to-buttons, persistent NVS file storage, deterministic Wi-Fi scans and fault injection",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "VER-0045 embedded builds remain separate",
    "host": "VER-0043 shared QWERTY/manual/import review/SAVE; native transport injected",
    "hardware": "HWT-0015 NOT RUN; prior outcomes historical"
  },
  "issues": [],
  "history": [
    "DEV-0020",
    "DEV-0021",
    "DEV-0023",
    "DEV-0025"
  ]
}
```

```json
{
  "record_type": "feature",
  "id": "FEAT-0016",
  "name": "System Localization and Arabic Typography Engine",
  "purpose": "Provide complete bilingual English and Arabic localization across all apps, system views, cards, and status messages with contextual Arabic cursive shaping, bidirectional reordering, and crisp 14px bitmap typography",
  "module": "src/services/LocalizationService + src/services/ArabicReshaper + src/ui/ArabicFont",
  "files": [
    "src/services/LocalizationService.cpp",
    "src/services/LocalizationService.h",
    "src/services/ArabicReshaper.cpp",
    "src/services/ArabicReshaper.h",
    "src/ui/ArabicFont.h",
    "src/hardware/DisplayManager.cpp",
    "src/hardware/DisplayManager.h",
    "src/apps/SettingsApp.cpp",
    "src/apps/SettingsApp.h",
    "src/services/SettingsService.cpp",
    "src/services/SettingsService.h"
  ],
  "dependencies": [
    "FEAT-0001",
    "FEAT-0005",
    "FEAT-0008"
  ],
  "hardware": "Integrated ST7789V parallel LCD, CST816S touch; also simulated in SDLDisplay",
  "gpio": "No extra GPIO allocation; uses ST7789V parallel bus",
  "lifecycle": "Stateless on-the-fly reshaping and BiDi reordering; zero dynamic allocations; NVS language configuration loaded on boot and persisted on change",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "VER-0071",
    "host": "VER-0069",
    "hardware": "HWT-0024"
  },
  "issues": [],
  "history": [
    "DEV-0033"
  ]
}
```

```json
{
  "record_type": "feature",
  "id": "FEAT-0017",
  "name": "Encrypted Password Vault Storage and Backup Engine",
  "purpose": "Provide authenticated AES-256-GCM encrypted persistence for account credentials in NVS with secure zeroing on lock/sleep and standalone PBKDF2/AES-GCM backup export and restore",
  "module": "src/services/VaultService",
  "files": [
    "src/services/VaultService.cpp",
    "src/services/VaultService.h",
    "src/services/CryptoService.cpp",
    "src/services/CryptoService.h",
    "src/core/AppContext.h",
    "src/main.cpp",
    "platform/desktop/DesktopMain.cpp"
  ],
  "dependencies": [
    "FEAT-0008",
    "FEAT-0012"
  ],
  "hardware": "ESP32-S3 hardware crypto / mbedtls AES-GCM; simulated on desktop via Windows BCrypt",
  "gpio": "No extra GPIO allocation; internal flash storage via Preferences (NVS)",
  "lifecycle": "Unlocks on valid session key; wipes all plaintext in memory immediately on lock/sleep via CryptoService::secureZero; fails closed on authentication tag mismatch",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "VER-0089",
    "host": "VER-0090",
    "hardware": "HWT-0029"
  },
  "issues": [],
  "history": [
    "DEV-0038"
  ]
}
```

```json
{
  "record_type": "feature",
  "id": "FEAT-0018",
  "name": "Password Vault Application and Confirmed USB HID Credential Transmission",
  "purpose": "Provide card-based on-device browsing of encrypted account credentials, password masking/unmasking, and two-step confirmed USB HID keystroke transmission with strict zero-touch arming",
  "module": "src/apps/VaultApp",
  "files": [
    "src/apps/VaultApp.h",
    "src/apps/VaultApp.cpp",
    "src/services/VaultService.h",
    "src/services/UsbHidService.h",
    "src/services/LocalizationService.h",
    "src/services/LocalizationService.cpp",
    "src/main.cpp",
    "platform/desktop/DesktopMain.cpp"
  ],
  "dependencies": [
    "FEAT-0001",
    "FEAT-0008",
    "FEAT-0010",
    "FEAT-0017"
  ],
  "hardware": "Integrated ST7789V display, CST816S touch, native USB HID controller",
  "gpio": "No extra GPIO allocation; uses standard display, touch, and USB pins",
  "lifecycle": "Requires active PIN session; arming requires two distinct taps with finger lift to trigger USB typing; memory zeroed on exit",
  "registration": "vaultApp",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "VER-0095",
    "host": "VER-0094",
    "hardware": "HWT-0030"
  },
  "issues": [],
  "history": [
    "DEV-0039"
  ]
}
```

```json
{
  "record_type": "feature",
  "id": "FEAT-0019",
  "name": "NibruOS Local Web Portal & Control Center",
  "purpose": "Provide high-security browser-based device management via an ephemeral SoftAP, allowing users to import encrypted Vault credentials and 2FA accounts directly from PC or mobile without software installation, plus USB HID typing of the Wi-Fi password",
  "module": "src/services/WebPortalService + src/apps/SettingsApp",
  "files": [
    "src/services/WebPortalService.h",
    "src/services/WebPortalService.cpp",
    "src/apps/SettingsApp.h",
    "src/apps/SettingsApp.cpp",
    "src/core/ResourceManager.h",
    "src/core/AppContext.h",
    "src/main.cpp",
    "platform/desktop/DesktopMain.cpp"
  ],
  "dependencies": [
    "FEAT-0001",
    "FEAT-0008",
    "FEAT-0010",
    "FEAT-0017",
    "FEAT-0018"
  ],
  "hardware": "Integrated 2.4GHz radio (SoftAP), ST7789V display, native USB HID controller",
  "gpio": "No extra GPIO allocation; uses standard display, touch, and radio",
  "lifecycle": "Leases ResourceOwner::WebPortal via ResourceManager; stopped automatically after 180s timeout or on lock/sleep/exit; requires Web PIN shown on LCD and active master key to accept POST requests",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "VER-0103",
    "host": "VER-0101",
    "hardware": "HWT-0032"
  },
  "issues": [],
  "history": [
    "DEV-0041"
  ]
}
```
