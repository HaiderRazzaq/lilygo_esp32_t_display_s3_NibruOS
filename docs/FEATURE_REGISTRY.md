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
    "src/core/ResourceManager.cpp"
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
    "DEV-0001"
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
    "platformio.ini"
  ],
  "dependencies": [
    "TFT_eSPI 2.5.43",
    "CST816S",
    "Arduino"
  ],
  "hardware": "Integrated ST7789V/CST816S/buttons",
  "gpio": "All LCD/touch/button reservations in HARDWARE_PINOUT; power15/backlight38; active LOW buttons0/14",
  "lifecycle": "Single instances in main; reset delays only at boot; 145ms touch and 35ms button edge debounce; long press650ms",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "VER-0001",
    "host": "VER-0002",
    "hardware": "HWT-0001 (independent reference sketch only); current firmware NOT VERIFIED"
  },
  "issues": [
    "BUG-0002"
  ],
  "history": [
    "DEV-0001"
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
    "DEV-0001"
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
    ".github/workflows/validate.yml"
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
    "documentation": "VER-0005",
    "hardware": "NOT APPLICABLE (repository tooling)"
  },
  "issues": [
    "BUG-0006"
  ],
  "history": [
    "DEV-0004",
    "DEV-0005"
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
    "src/services/WiFiService.h"
  ],
  "dependencies": [
    "FEAT-0004",
    "ResourceManager"
  ],
  "hardware": "Integrated 2.4GHz radio and CST816S capacitive touch",
  "gpio": "Uses existing reserved pins; no additional GPIO",
  "lifecycle": "Selection opens plaintext visible touch keyboard; non-blocking connection with 15s timeout; disconnect on onExit or user action",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "VER-0006",
    "host": "VER-0006",
    "hardware": "NOT VERIFIED"
  },
  "issues": [],
  "history": [
    "DEV-0006"
  ]
}
```

```json
{
  "record_type": "feature",
  "id": "FEAT-0010",
  "name": "Settings App",
  "purpose": "Main settings menu organizing System Info and Touch Test diagnostics with hierarchical Back navigation",
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
  "lifecycle": "Submenu container view delegates to sub-apps with hierarchical onBack navigation before exiting to Home",
  "implementation": "IMPLEMENTED",
  "verification": {
    "build": "VER-0006",
    "host": "VER-0006",
    "hardware": "NOT VERIFIED"
  },
  "issues": [],
  "history": [
    "DEV-0007"
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
