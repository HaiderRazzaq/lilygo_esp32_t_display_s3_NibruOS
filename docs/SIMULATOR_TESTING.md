# NibruOS — Desktop Simulator Testing Strategy

## 1. Overview

NibruOS includes a dedicated desktop automated test suite (`nibru_tests`) that executes without hardware, validating shared application logic, UI coordinate transformations, Wi-Fi failure modes, settings persistence, and lifecycle transitions.

The test suite runs in headless mode and is integrated into CMake via CTest.

---

## 2. Running Automated Tests

### Windows One-Click

```bat
tools\run_desktop_tests.bat
```

### Via CTest

```bat
ctest --test-dir build --output-on-failure
```

### Direct Executable Execution

```bat
.\build\nibru_tests.exe
```

---

## 3. Test Categories & Verification Scope

### Category A: Core Lifecycle & AppManager Tests
- **Application Registration**: Verifies registering 5 applications (WiFiApp, TotpApp, SettingsApp, SystemInfoApp, TouchTestApp).
- **Navigation Flow**:
  - Verifies Home initial state (`isHome() == true`, `selectedIndex() == 0`).
  - Verifies opening an application (`open(0)`).
  - Verifies direct switching between applications (`open(2)` exits App 0 and enters App 2).
  - Verifies Back button returns to Home while preserving the selected tile index.
  - Verifies out-of-bounds app index protection (`open(99)` returns false).
- **BOOT Button Navigation**:
  - Short press (`ButtonAction::SelectNext`) cycles tile selection sequentially.
  - Long press (`ButtonAction::Activate`) opens the focused application.

### Category B: UI & Coordinate Transformation Tests
- **Logical Resolution**: Asserts display width is 320 and height is 170.
- **Mouse to Touch Transformation**:
  - Verifies mapping window coordinates to logical [0..319, 0..169].
  - Verifies inverse raw coordinate calculation (`rawX = 169 - y`, `rawY = x`).
- **Screen Boundary Clamping**:
  - Tests negative coordinates (-10, 200) clamped to (0, 169).
  - Tests overflow coordinates (350, -5) clamped to (319, 0).
- **Gesture Decoding**:
  - Verifies Tap, DoubleTap, LongPress, SwipeLeft, SwipeRight, SwipeUp, and SwipeDown recognition.
  - Verifies contact ID increments on each physical interaction.

### Category C: Wi-Fi Service & Fault Injection Tests
- **Scenario 1: Normal Scan**
  - Acquires `ResourceManager` Wi-Fi lease.
  - Verifies 650ms `ScanState::Preparing` warmup state.
  - Verifies transition to `ScanState::Scanning` after warmup.
  - Verifies scan completion discovering 5 mock access points ("NibruOS_Lab", "Home_5G_Ext", etc.).
  - Verifies network attributes (SSID, RSSI, channel, security flag).
  - Verifies clean cleanup on `stopScan()` and resource release.
- **Scenario 2: Scan Timeout & Retry (BUG-0001 Simulation)**
  - Injects `WiFiSimMode::Timeout`.
  - Advances time past 11,000ms scan timeout (`kScanTimeoutMs`).
  - Verifies automatic scan retry attempt 2 (`attempt() == 2`).
  - Advances time past second timeout (total 2 attempts exceeded).
  - Verifies graceful transition to `ScanState::Failed`.
- **Scenario 3: Immediate Scan Failure**
  - Injects `WiFiSimMode::ScanFailed`.
  - Verifies failure detection and retry handling.

### Category D: Settings Persistence & Desktop NVS Tests
- **Default Preferences**: Asserts 100% brightness, 30s timeout, Theme 0 (Cyber Cyan).
- **Preference Modification**: Sets 50% brightness, 60s timeout, Theme 2 (Amber Gold).
- **Cross-Instance File Persistence**: Re-creates `SettingsService` from scratch; asserts loaded preferences match saved values from `simulator/runtime/nvs_store.dat`.
- **Validation**: Rejects invalid values (15% brightness, 105% brightness, 45s timeout, theme index 5).
- **Factory Reset**: Calls `resetToDefaults()`; asserts preferences revert to defaults while preserving unrelated namespaces.

### Category E: 25-Cycle Repeated Lifecycle Stress Tests
- Executes 25 consecutive application open/close cycles cycling through all 5 applications.
- Validates zero dangling pointers, zero memory leaks, and clean resource cleanup on every cycle.

---

## 4. Verification Output Example

```text
==================================================
  NibruOS Automated Desktop & Shared Logic Tests
==================================================

[DesktopStorage] Initialized with runtime directory 'simulator/runtime_test'
[Test] Running Core Lifecycle & AppManager Tests...
  -> Core Lifecycle Tests PASS
[Test] Running UI & Coordinate Transformation Tests...
  -> UI & Transformation Tests PASS
[Test] Running Wi-Fi Service & Fault Injection Tests...
[WiFi] Scan complete: 5 APs discovered in 1300 ms
[WiFi] attempt 1/2 failed: Scan timed out
[WiFi] retrying scan (attempt 2/2)...
[WiFi] attempt 2/2 failed: Scan timed out
  -> Wi-Fi Simulation Tests PASS
[Test] Running Settings Persistence & Desktop NVS Tests...
[Settings] Loaded: Bright=100% Timeout=30s Theme=Cyber Cyan
[Settings] Loaded: Bright=50% Timeout=60s Theme=Amber Gold
  -> Settings Persistence Tests PASS
[Test] Running 25 Repeated Lifecycle Entry/Exit Cycles...
  -> 25 Lifecycle Cycles PASS (Zero leaks, clean state)

==================================================
  ALL DESKTOP SIMULATOR TESTS PASSED SUCCESSFULLY!
==================================================
```
