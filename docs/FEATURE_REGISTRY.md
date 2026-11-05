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
    "DEV-0021",
    "DEV-0064"
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
  "purpose": "Read-only async 2.4GHz AP discovery, 12 connection-list records, full-scan channel summaries, 3 visible rows, rescan and scrolling",
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

```json
{
  "record_type": "feature",
  "id": "FEAT-0020",
  "name": "Cyber Desk Clock Application",
  "purpose": "Provide a high-contrast cyber-styled desk clock with large digital time, date, NTP synchronization status badge, animated seconds indicator, and tap color-theme cycling",
  "module": "src/apps/ClockApp",
  "files": [
    "src/apps/ClockApp.h",
    "src/apps/ClockApp.cpp",
    "src/services/TimeService.h",
    "src/main.cpp",
    "platform/desktop/DesktopMain.cpp"
  ],
  "dependencies": [
    "FEAT-0001",
    "FEAT-0008",
    "FEAT-0010"
  ],
  "hardware": "Integrated ST7789V display, CST816S touch",
  "gpio": "Uses standard display and touch pins; no extra GPIO",
  "lifecycle": "Updates time representation on 1000ms tick; tap toggles cyber theme; back button or touch exits to home",
  "registration": "clockApp",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "VER-0127",
    "host": "VER-0125",
    "hardware": "HWT-0039"
  },
  "issues": [],
  "history": [
    "DEV-0046",
    "DEV-0047"
  ]
}
```

```json
{
  "record_type": "feature",
  "id": "FEAT-0021",
  "name": "Password Generator Tool",
  "purpose": "Provide secure on-device cryptographic password generation with configurable length and symbols, direct one-touch USB HID typing to PC, and one-touch saving to encrypted Password Vault",
  "module": "src/apps/GeneratorApp",
  "files": [
    "src/apps/GeneratorApp.h",
    "src/apps/GeneratorApp.cpp",
    "src/services/CryptoService.h",
    "src/services/UsbHidService.h",
    "src/services/VaultService.h",
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
  "hardware": "Integrated ST7789V display, CST816S touch, native USB HID controller",
  "gpio": "Uses standard display, touch, and USB pins; no extra GPIO",
  "lifecycle": "Generates passwords via CryptoService::fillRandom; securely zeroes memory buffers on exit",
  "registration": "generatorApp",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "VER-0127",
    "host": "VER-0125",
    "hardware": "HWT-0039"
  },
  "issues": [],
  "history": [
    "DEV-0046",
    "DEV-0047"
  ]
}
```

```json
{
  "record_type": "feature",
  "id": "FEAT-0022",
  "name": "Bluetooth LE HID Architecture & Mutex Service",
  "purpose": "Provide a modular Bluetooth LE HID service architecture strictly mediated through ResourceManager mutual exclusion, coexisting safely with Wi-Fi without degrading internal heap headroom",
  "module": "src/services/BleHidService",
  "files": [
    "src/apps/BluetoothApp.h",
    "src/apps/BluetoothApp.cpp",
    "src/services/BleHidService.h",
    "src/services/BleHidService.cpp",
    "src/core/ResourceManager.h",
    "src/core/AppContext.h",
    "src/main.cpp",
    "platform/desktop/DesktopMain.cpp",
    "src/apps/SettingsApp.h",
    "src/apps/SettingsApp.cpp",
    "src/apps/TotpApp.h",
    "src/apps/TotpApp.cpp",
    "src/apps/GeneratorApp.h",
    "src/apps/GeneratorApp.cpp",
    "src/apps/VaultApp.h",
    "src/apps/VaultApp.cpp"
  ],
  "dependencies": [
    "FEAT-0001",
    "FEAT-0004"
  ],
  "hardware": "ESP32-S3 BLE radio",
  "gpio": "No extra GPIO allocation",
  "lifecycle": "Explicit enable acquires ResourceOwner::BleKeyboard and enables Wi-Fi modem sleep for coexistence. Startup failure or explicit disable releases the lease. Wi-Fi restores its usual policy after the controller stops. Keyboard operation can continue across app navigation.",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "VER-0185, VER-0190",
    "host": "VER-0183, VER-0184, VER-0188, VER-0189",
    "hardware": "HWT-0064 upload PASS; HWT-0065 Windows and HWT-0066 phone keyboard acceptance failed"
  },
  "issues": ["BUG-0017", "BUG-0018"],
  "history": [
    "DEV-0046",
    "DEV-0047",
    "DEV-0048",
    "DEV-0049",
    "DEV-0050",
    "DEV-0051",
    "DEV-0052",
    "DEV-0053",
    "DEV-0054",
    "DEV-0055",
    "DEV-0056",
    "DEV-0057",
    "DEV-0058",
    "DEV-0059"
  ],
  "registration": "bluetoothApp"
}
```

```json
{
  "record_type": "feature",
  "id": "FEAT-0023",
  "name": "Encrypted Web Portal Backup & Restore System",
  "purpose": "Provide authenticated browser-based encrypted vault export and import via Web Portal endpoints (/api/vault/backup and /api/vault/restore) with passphrase-based PBKDF2/AES-GCM encryption",
  "module": "src/services/WebPortalService",
  "files": [
    "src/services/WebPortalService.h",
    "src/services/WebPortalService.cpp",
    "src/services/VaultService.h",
    "src/services/VaultService.cpp"
  ],
  "dependencies": [
    "FEAT-0017",
    "FEAT-0018",
    "FEAT-0019"
  ],
  "hardware": "Integrated 2.4GHz radio (SoftAP), native USB HID controller",
  "gpio": "No extra GPIO allocation",
  "lifecycle": "Requires active Web PIN and master PIN session; generates authenticated downloadable backup binary and parses uploaded backup blobs",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "VER-0127",
    "host": "VER-0125",
    "hardware": "HWT-0039"
  },
  "issues": [],
  "history": [
    "DEV-0046",
    "DEV-0047"
  ]
}
```

```json
{
  "record_type": "feature",
  "id": "FEAT-0024",
  "name": "Beacon Lab",
  "purpose": "Advertise up to 40 of 48 fictional SSIDs, with separate open and WPA2-advertised counts and five- to sixty-second sessions.",
  "module": "src/apps/BeaconLabApp",
  "files": [
    "src/apps/BeaconLabApp.cpp",
    "src/apps/BeaconLabApp.h",
    "src/services/WiFiService.cpp",
    "src/services/WiFiService.h"
  ],
  "dependencies": [
    "FEAT-0004"
  ],
  "hardware": "Integrated ESP32-S3 2.4 GHz Wi-Fi",
  "gpio": "No extra GPIO",
  "lifecycle": "Explicit Start acquires BeaconLab lease. Timeout, Stop, Back, lock and sleep release it. Connected Wi-Fi and existing radio owners block entry.",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "VER-0193, VER-0198",
    "host": "VER-0192, VER-0197",
    "hardware": "HWT-0074 owner-reported working; resource profiling pending"
  },
  "issues": [],
  "history": [
    "DEV-0061",
    "DEV-0062",
    "DEV-0063"
  ]
}
```


```json
{
  "record_type": "feature",
  "id": "FEAT-0025",
  "name": "WiFi Defense IDS",
  "purpose": "Promiscuous mode 802.11 intrusion detection monitoring deauthentication, disassociation and EAPOL handshakes with live attack alerts and channel hopping.",
  "module": "src/apps/WifiDefenseApp",
  "files": [
    "src/apps/WifiDefenseApp.cpp",
    "src/apps/WifiDefenseApp.h",
    "src/services/WifiDefenseService.cpp",
    "src/services/WifiDefenseService.h"
  ],
  "dependencies": [
    "FEAT-0004"
  ],
  "hardware": "Integrated ESP32-S3 2.4 GHz Wi-Fi",
  "gpio": "No extra GPIO",
  "lifecycle": "onEnter acquires WifiDefense lease and starts sniffer. onExit disables promiscuous mode and releases radio lease cleanly.",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "VER-0203",
    "host": "VER-0202",
    "hardware": "NOT RUN"
  },
  "issues": [],
  "history": [
    "DEV-0065"
  ]
}
```
```json
{
  "record_type": "feature",
  "id": "FEAT-0026",
  "name": "Wi-Fi channel analysis",
  "purpose": "Show access point counts and strongest RSSI per 2.4GHz channel using completed asynchronous scans.",
  "module": "src/apps/WiFiChannelsApp",
  "files": [
    "src/apps/WiFiChannelsApp.h",
    "src/apps/WiFiChannelsApp.cpp",
    "src/services/WiFiService.h",
    "src/services/WiFiService.cpp"
  ],
  "dependencies": [
    "FEAT-0004"
  ],
  "hardware": "Integrated ESP32-S3 Wi-Fi radio",
  "gpio": "No extra GPIO",
  "lifecycle": "Acquire WiFiChannels lease on entry; cached display and explicit Refresh; cancel pending scan and release lease on exit. Preserve an established station connection.",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "VER-0208",
    "host": "VER-0205 RED, VER-0207 GREEN",
    "hardware": "HWT-0075 NOT RUN"
  },
  "issues": [],
  "history": [
    "DEV-0066"
  ]
}
```

```json
{
  "record_type": "feature",
  "id": "FEAT-0027",
  "name": "BLE Lab advertising",
  "purpose": "Classroom demonstrations of generic discovery, accessory pairing advertisement profiles, Apple Continuity alerts and beacon name advertisements.",
  "module": "src/apps/BleLabApp + src/services/BleAdvertisingService",
  "files": [
    "src/apps/BleLabApp.h",
    "src/apps/BleLabApp.cpp",
    "src/services/BleAdvertisingService.h",
    "src/services/BleAdvertisingService.cpp",
    "src/services/BleStack.h",
    "src/services/BleAdvertisingPayload.h",
    "src/services/BleAdvertisingPayload.cpp",
    "src/core/AppContext.h"
  ],
  "dependencies": ["FEAT-0004", "FEAT-0022"],
  "hardware": "Integrated ESP32-S3 Bluetooth LE controller",
  "gpio": "No extra GPIO",
  "lifecycle": "Explicit Start acquires BleAdvertising radio lease. GAP events sequence advertising identities with per-profile connectable routing. Stop, timeout, Back, lock and sleep end the session. A failed teardown retains ownership and retries cleanup.",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "VER-0231",
    "host": "VER-0211/VER-0218/VER-0224 RED; VER-0213/VER-0219/VER-0225/VER-0230 GREEN",
    "hardware": "HWT-0080 Windows notification and received advertisements PASS; HWT-0081 Random/picker pending; HWT-0085 v0.20.0 upload PASS; HWT-0086 runtime boot BLOCKED pending physical reset; HWT-0084 Apple and beacon receiver behavior NOT RUN",
    "desktop": "VER-0216/VER-0222/VER-0228 SKIPPED by owner"
  },
  "issues": [],
  "history": ["DEV-0067", "DEV-0068", "DEV-0069", "DEV-0070"]
}
```

```json
{
  "record_type": "feature",
  "id": "FEAT-0028",
  "name": "Security Lab Hub",
  "purpose": "Consolidate wireless security and audit laboratory tools (BLE Lab, Beacon Lab, WiFi Defense, WiFi Channels) under a unified launcher and sub-menu.",
  "module": "src/apps/SecurityLabApp",
  "files": [
    "src/apps/SecurityLabApp.h",
    "src/apps/SecurityLabApp.cpp",
    "src/main.cpp",
    "platform/desktop/DesktopMain.cpp"
  ],
  "dependencies": ["FEAT-0001", "FEAT-0023", "FEAT-0025", "FEAT-0026", "FEAT-0027"],
  "hardware": "Integrated TFT display, CST816S touch, BOOT button",
  "gpio": "No extra GPIO",
  "lifecycle": "Launcher tile routes into a 4-card grid. Opening a tool delegates full App lifecycle (onEnter, onTick, onTouch, onPrimaryAction, onActivate, render). Nested onBack returns to hub; hub onBack returns to Home. Clean exit releases active tool and any radio lease.",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "VER-0234",
    "host": "VER-0233 GREEN",
    "hardware": "HWT-0087 NOT RUN",
    "desktop": "VER-0235 GREEN"
  },
  "issues": [],
  "history": ["DEV-0072"],
  "registration": "securityLabApp"
}
```
