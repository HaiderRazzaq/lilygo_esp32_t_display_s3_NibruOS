# NibruOS — Desktop Simulator User & Developer Guide

NibruOS features a cross-platform Desktop Simulator running on Windows (and POSIX/macOS) powered by **SDL2** and **CMake**.

The simulator executes the **exact same C++ application code, UI rendering logic, navigation flow, settings persistence, and lifecycle transitions** as the physical ESP32-S3 firmware, allowing full development, visual verification, interaction, and failure testing without needing physical hardware.

---

## 1. Quick Start

### One-Click Launch (Windows)

From the project root in Command Prompt or PowerShell:

```bat
tools\run_simulator.bat
```

This script detects the MSVC 2022 toolchain, configures CMake, builds the simulator, copies required runtime dependencies (`SDL2.dll`), and launches the interactive simulator window.

### One-Click Automated Tests

```bat
tools\run_desktop_tests.bat
```

Runs the automated test suite verifying lifecycle, navigation, coordinate transforms, Wi-Fi fault injection, settings persistence, and stress cycles.

---

## 2. Prerequisites & Environment Setup

### Required Tools on Windows

- **CMake** (v3.16 or newer)
- **C++ Compiler**:
  - Microsoft Visual Studio 2022 (Community, Professional, or BuildTools with "Desktop development with C++")
  - Or Clang / GCC with MinGW-w64
- **Ninja** or MSBuild build generator
- **Python 3.8+** (for documentation and resource checks)

### SDL2 Dependency Management

The CMake build automatically handles the SDL2 dependency:
1. CMake searches for an existing local SDL2 installation via `find_package(SDL2 CONFIG QUIET)`.
2. If absent, CMake uses `FetchContent` to download the official prebuilt **SDL2 VC Development Package** (v2.30.10) directly from GitHub releases and unpacks it into the build tree.
3. A post-build command copies `SDL2.dll` directly alongside `nibru_simulator.exe`.

No manual environment variable configuration or system-wide installer is required.

---

## 3. Manual Build Commands

### Configuration

From the repository root:

```bat
cmake -B build -G "Ninja"
```

Or with Visual Studio generator:

```bat
cmake -B build -G "Visual Studio 17 2022" -A x64
```

### Compilation

```bat
cmake --build build --config Release
```

### Running

```bat
.\build\nibru_simulator.exe
```

Optional command-line arguments:
- `--scale=N` : Set initial window scale (1, 2, 3, or 4). Default is `3` (960×510).
- `--headless` : Run without creating an SDL GUI window (useful for headless CI / unit test runners).
- `--test-frames=N` : Run for exactly N frames and then exit cleanly.

---

## 4. Display & Scaling

| Scale | Logical Resolution | Window Client Area | Pixel Grid |
|---|---|---|---|
| **1×** | 320 × 170 | 320 × 170 | 1:1 Native |
| **2×** | 320 × 170 | 640 × 340 | 2×2 pixels |
| **3× (Default)** | 320 × 170 | 960 × 510 | 3×3 pixels (Recommended for 1080p/1440p) |
| **4×** | 320 × 170 | 1280 × 680 | 4×4 pixels (Recommended for 4K) |

- **Scaling Quality**: Uses nearest-neighbor sampling (`SDL_HINT_RENDER_SCALE_QUALITY = "0"`) to replicate the crisp, pixel-perfect appearance of the physical ST7789 display.
- **Font Rendering**: Uses the verified 5x7 GLCD bitmap font from TFT_eSPI. Character widths (`size=1` = 6×8px, `size=2` = 12×16px) match the physical device pixel-for-pixel.
- **Dynamic Scale Cycling**: Press **`F1`** at any time to cycle window size (1× → 2× → 3× → 4× → 1×).

---

## 5. Controls & Interaction

### Mouse (Touchscreen Simulation)

| Action | Physical Equivalent | Result |
|---|---|---|
| **Left Click (short)** | Single Tap | Selects tile, activates buttons, enters apps |
| **Double Click** | Double Tap | Header back shortcut, quick select |
| **Click & Hold (>650ms)** | Long Press | Triggers long-press actions |
| **Click & Drag Left/Right** | Swipe Left / Right | Pages through Home tiles, scrolls horizontally |
| **Click & Drag Up/Down** | Swipe Up / Down | Scrolls vertical lists in Settings / Scanners |

Coordinates map to the 320×170 logical screen; inverted raw coordinates (`rawX = 169 - y`, `rawY = x`) are provided to diagnostic views like `TouchTestApp`.

### Keyboard (Buttons & Navigation)

| Key | Physical Button / Action | NibruOS Function |
|---|---|---|
| **Escape** / **Backspace** / **`U`** | GPIO14 User Button | Back / Exit App / Return to Home |
| **Space** / **Tab** / **`B`** / **Right** | BOOT Button (Short Press) | Select Next tile / Focus next item |
| **Enter** / **`O`** | BOOT Button (Long Press) | Activate / Open selected app / Confirm |
| **Arrow Up / Down / Left / Right** | Directional Swipes | Injects swipe gesture navigation |

---

## 6. Simulator Developer Shortcuts (F1–F9)

| Key | Function | Description |
|---|---|---|
| **`F1`** | **Cycle Window Scale** | Switches scale between 1×, 2×, 3×, and 4×. |
| **`F2`** | **Wi-Fi Mode: Normal** | Simulates successful scan discovering 5 realistic APs in 1200ms. |
| **`F3`** | **Wi-Fi Mode: Timeout** | Simulates radio scan hang. Triggers 11s timeout and automatic retry logic (BUG-0001). |
| **`F4`** | **Wi-Fi Mode: Scan Failed** | Simulates hardware scan start rejection (`WIFI_SCAN_FAILED`). |
| **`F5`** | **Reboot Simulated Device** | Simulates physical ESP32 restart, resetting memory and rebooting services. |
| **`F6`** | **Toggle Diagnostic HUD** | Toggles a live status overlay showing FPS, host RSS memory, and active fault injection mode. |
| **`F9`** | **Wi-Fi Mode: STA Error** | Simulates Wi-Fi station initialization failure (`WiFi.mode(WIFI_STA)` fails). |

---

## 7. Storage Simulation & Persistence

### NVS Preferences Backend

- Settings (Brightness, Screen Timeout, Theme Accent) and Wi-Fi credentials persist across simulator runs in:
  `simulator/runtime/nvs_store.dat`
- Values survive restarting or closing the simulator.
- Calling Settings "Reset to Factory Defaults" resets user preferences while safely write-protecting unrelated security namespaces.

### Sandboxed MicroSD Backend

- MicroSD storage is sandboxed strictly inside:
  `simulator/runtime/sdcard/`
- Path sanitization prevents directory traversal attacks (`..` escapes).

---

## 8. Limitations & Distinctions from Hardware

1. **Host Process Memory vs. ESP32 SRAM**:
   The desktop process memory (~20–40 MB working set) includes the Windows OS runtime, SDL2 libraries, and CRT. It is **not** an indicator of ESP32 internal SRAM headroom (which must be >= 150 KB).
2. **Backlight Hardware**:
   On physical hardware, backlight brightness is controlled by the AW9364 pulse protocol or optional LEDC PWM. The simulator emulates brightness by modulating SDL texture color brightness.
3. **Wi-Fi Radio**:
   Simulated Wi-Fi scans use deterministic test data. A successful scan in the simulator does **not** prove radio reception or antenna performance on the physical board.
4. **Physical Flash / Upload**:
   Running the desktop simulator does not alter board flash. To flash the physical board, use `pio run -e lilygo-t-display-s3 -t upload`.
