# NibruOS — Desktop Simulator Architecture

## 1. Architectural Philosophy

The NibruOS Desktop Simulator follows the core design principle:

**One Project → Shared Application Code → Two Execution Targets**

1. **Embedded Firmware Target**: LILYGO T-Display-S3 Touch Glass (ESP32-S3R8, ST7789 parallel LCD, CST816S touch, FreeRTOS/Arduino-ESP32, built with PlatformIO).
2. **Desktop Simulator Target**: Windows / POSIX Desktop (C++17, SDL2 display & input backend, CMake build system).

The simulator is **not** a mock, an HTML/CSS imitation, or a separate rewrite of the applications. It compiles and runs the **actual C++ source files** from `src/core/`, `src/apps/`, `src/services/`, and `src/hardware/`.

---

## 2. System Architecture Diagram

```mermaid
graph TD
    subgraph Shared NibruOS Application Layer
        AppManager["AppManager (Core Shell)"]
        AppContext["AppContext (Service References)"]
        ResourceManager["ResourceManager (Radio Lease)"]
        Apps["Applications (WiFiApp, TotpApp, SettingsApp, SystemInfoApp, TouchTestApp)"]
        Services["Services (SettingsService, TotpService, SecurityService, TimeService, UsbHidService)"]
        DisplayMgr["DisplayManager (UI Layout & Theme)"]
    end

    subgraph Hardware Target: ESP32-S3 (PlatformIO)
        EspMain["src/main.cpp"]
        BodmerTFT["Bodmer TFT_eSPI Driver"]
        CST816SDriver["CST816S Touch Driver"]
        ArduinoWiFi["Arduino-ESP32 WiFi Library"]
        EspNVS["ESP-IDF NVS / Preferences"]
    end

    subgraph Desktop Target: Windows Simulator (CMake)
        DesktopMain["platform/desktop/DesktopMain.cpp"]
        SDLDisplay["SDLDisplay (320x170 RGB565 Framebuffer + SDL2)"]
        DesktopInput["DesktopInput (Mouse -> TouchEvent, Keys -> ButtonAction)"]
        MockWiFi["MockWiFiService (Deterministic Scans & Fault Injection)"]
        DesktopStorage["DesktopStorage (Persistent NVS & Sandboxed SD)"]
        DesktopDiag["DesktopDiagnostics (FPS, Host RAM, HUD)"]
        PlatformStubs["platform/desktop/stubs (Arduino.h, TFT_eSPI.h, Preferences.h, WiFi.h)"]
    end

    Apps --> AppContext
    Apps --> DisplayMgr
    AppManager --> Apps
    AppManager --> DisplayMgr

    DisplayMgr --> BodmerTFT
    DisplayMgr --> SDLDisplay

    EspMain --> AppManager
    DesktopMain --> AppManager
    DesktopMain --> DesktopInput
    DesktopMain --> SDLDisplay
    DesktopMain --> DesktopDiag

    PlatformStubs --> SDLDisplay
    PlatformStubs --> DesktopStorage
    PlatformStubs --> MockWiFi
```

---

## 3. Code Separation and Isolation

| Directory | Target | Responsibility |
|---|---|---|
| `src/core/` | **Shared** | Application lifecycle, AppManager routing, ResourceManager leases, MemoryProfiler. |
| `src/apps/` | **Shared** | Complete UI layout, touch event handling, navigation state, subviews. |
| `src/services/` | **Shared** | Settings validation, TOTP code generation, crypto hashing, PIN lock security. |
| `src/hardware/` | **Shared** | DisplayManager, ButtonManager, TouchManager (with platform abstraction). |
| `include/` | **Shared** | Board geometry, pin reservations, runtime version constants. |
| `platform/desktop/` | **Desktop Only** | SDL2 display backend, mouse/keyboard translation, storage persistence, diagnostics. |
| `platform/desktop/stubs/` | **Desktop Only** | Desktop implementations of Arduino, TFT_eSPI, Preferences, WiFi headers. |
| `simulator/runtime/` | **Desktop Only** | Sandboxed runtime directory for NVS data and simulated MicroSD files. |

### Build Boundary Guarantees

- **PlatformIO Embedded Build**:
  `platformio.ini` only scans `src/` and official libraries in `.pio/libdeps/`. The `platform/` and `simulator/` directories are completely ignored by PlatformIO.
- **CMake Desktop Build**:
  `CMakeLists.txt` compiles `platform/desktop/` alongside `src/`, with `-DUNIT_TEST=1` and `-DNIBRU_DESKTOP_SIMULATOR=1`.

---

## 4. Display Subsystem

### Resolution and Color Architecture
- **Logical Resolution**: Fixed at 320 × 170 pixels, identical to the physical ST7789 display in landscape Rotation 1.
- **Pixel Buffer**: Internal 16-bit RGB565 buffer (`uint16_t buffer[170][320]`).
- **SDL2 Streaming Texture**: Uploaded using `SDL_PIXELFORMAT_RGB565` directly to GPU memory, eliminating color format conversion overhead during drawing operations.
- **Scaling Engine**: GPU-accelerated scaling using nearest-neighbor interpolation (`SDL_HINT_RENDER_SCALE_QUALITY = "0"`), ensuring pixel grid fidelity.

### Typography Fidelity
- Standard 5x7 GLCD bitmap font from `glcdfont.h`.
- Character dimensions match the physical device:
  - `size = 1`: 6 × 8 pixels per glyph (5x7 glyph + 1px spacing).
  - `size = 2`: 12 × 16 pixels per glyph.
- Text metrics, label placements, header/footer spacing, and tile title truncation behavior are 100% identical between desktop and hardware.

---

## 5. Input Subsystem

### Mouse to Touchscreen Translation
The `DesktopInput` module converts desktop mouse events into NibruOS `TouchEvent` structures:
1. **Coordinate Mapping**: Window coordinates are divided by the active integer window scale (1×, 2×, 3×, 4×) and clamped to [0..319, 0..169].
2. **Gesture Recognition**:
   - Short click without drag (< 12px) = `TouchGesture::Tap`.
   - Rapid click within 300ms = `TouchGesture::DoubleTap`.
   - Click held >= 650ms (`kButtonLongPressMs`) = `TouchGesture::LongPress`.
   - Drag with displacement >= 12px = Directional swipes (`SwipeLeft`, `SwipeRight`, `SwipeUp`, `SwipeDown`).
3. **Inverted Raw Coordinates**:
   Physical CST816S raw coordinates are delivered as `rawX = 169 - y`, `rawY = x`, providing accurate diagnostic data for `TouchTestApp`.

### Keyboard to Button Translation
- `Escape` / `Backspace` / `U` → `ButtonAction::Back` (mapped to GPIO14 User Button).
- `Space` / `Tab` / `B` / `Right` → `ButtonAction::SelectNext` (mapped to BOOT short press).
- `Enter` / `O` → `ButtonAction::Activate` (mapped to BOOT long press).
- `Arrow Keys` → Injects directional swipe navigation gestures.

---

## 6. Service & Storage Emulation

### Wi-Fi Simulation (`MockWiFiService`)
Simulates the asynchronous state machine of the ESP32 Wi-Fi driver:
- **Scan Preparation**: 650ms STA warmup period before radio scan initiation.
- **Deterministic Discovery**: Discovers realistic APs ("NibruOS_Lab", "Home_5G_Ext", "CoffeeShop_Guest", "ESP32_Test_AP", "Office_IoT").
- **Fault Injection**:
  - `WiFiSimMode::Timeout`: Scan hangs past 11 seconds, triggering `WiFiService` retry (attempt 2) and failure transition.
  - `WiFiSimMode::ScanFailed`: Immediate hardware scan error.
  - `WiFiSimMode::StaError`: Failure to initialize STA mode.
- **Connection Handshake**: Simulates 1500ms association and DHCP handshake, assigning `192.168.1.105`.

### Persistent Storage (`DesktopStorage`)
- **NVS Emulation**: Serializes binary key-value data to `simulator/runtime/nvs_store.dat`. Backs the `Preferences` class so brightness, timeout, theme accent, and saved networks persist across simulator runs.
- **MicroSD Sandbox**: Sandboxed path `simulator/runtime/sdcard/`. Rejects directory traversal attempts (`..` paths).
