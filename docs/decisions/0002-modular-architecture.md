# ADR-0002 — Feature-Based Modular Monolith with Layered Responsibilities and HAL

- Date: 2026-11-01 (Asia/Baghdad)
- Status: ACCEPTED for implementation in this architectural change
- Related: FEAT-0001, FEAT-0005, FEAT-0006, FEAT-0015, DEV-0007
- Implementation: [architecture](../ARCHITECTURE.md), [EventTypes](../../src/core/EventTypes.h), [IApp](../../src/core/IApp.h), [HomeScreen](../../src/ui/HomeScreen.h), [Theme](../../src/ui/Theme.h), [PinDefinitions](../../include/PinDefinitions.h), [NibruConfig](../../include/NibruConfig.h)

## Context

As NibruOS expanded from a basic prototype into a multi-app firmware platform supporting both physical ESP32-S3 hardware and an SDL2-based desktop simulator, several architectural couplings accumulated:
1. **Layer Inversion**: `src/core/AppManager.h` included `src/hardware/ButtonManager.h` merely to receive `ButtonAction`, making the core lifecycle coordinator depend on a concrete hardware driver.
2. **God-Class / Mixed Responsibilities in Core**: `src/core/AppManager.cpp` held 76 lines of ST7789 display drawing code in `drawHome()`, mixing coordinate math, card geometry, text formatting, and theme colors into the lifecycle manager.
3. **Hardware / Library Bypasses in Applications**: `src/apps/SystemInfoApp.cpp` directly included `<WiFi.h>` and called `WiFi.macAddress()`, bypassing the shared `WiFiService` boundary.
4. **Scattered Configuration**: `include/BoardConfig.h` bundled physical GPIO pin assignments, display dimensions, touch debounce thresholds, and Wi-Fi timeouts into a single flat namespace without modular pin separation, and proposed external MicroSD SPI pins were only documented in prose.

## Decision

Adopt a **Feature-Based Modular Monolith with Layered Responsibilities and Hardware Abstraction (HAL)**:
1. **Event Types Extraction**: Move `TouchGesture`, `TouchEvent`, and `ButtonAction` into `src/core/EventTypes.h`. Core no longer depends on hardware driver headers for input enums.
2. **Standard Application Contract**: Introduce `src/core/IApp.h` aliasing `App` to formalize the lifecycle interface (`onEnter`, `onExit`, `onTick`, `onTouch`, `onPrimaryAction`, `onActivate`, `onBack`, `render`).
3. **UI Layer Extraction**: Create `src/ui/Theme.h` for system color palettes and UI metrics, and `src/ui/HomeScreen.*` for Home screen card layout, badges, and paginated rendering. `AppManager::drawHome()` delegates directly to `HomeScreen::draw()`.
4. **Hardware Pin Isolation**: Centralize board GPIO pins in `include/PinDefinitions.h` (including LCD parallel pins, touch pins, buttons, and proposed unverified SPI MicroSD pins) and system constants in `include/NibruConfig.h`, while preserving `include/BoardConfig.h` for exact backward compatibility with existing tests and scripts.
5. **Service Boundary Enforcement**: Add `WiFiService::getMacAddress()` to encapsulate MAC address retrieval across real hardware and desktop simulator stubs, removing direct `<WiFi.h>` usage from `SystemInfoApp.cpp`.

## Alternatives and Reasons

- **Enterprise DI / Plugin Framework**: Rejected. Runtime dynamic loaders or complex dependency injection frameworks add memory overhead, heap fragmentation, and runtime failure modes unsuitable for ESP32-S3 firmware. Compile-time composition via `AppContext` is lightweight, deterministic, and zero-allocation.
- **LVGL or Heavy GUI Framework**: Rejected. Introducing a third-party GUI library would require large memory buffers, significant partition restructuring, and would replace owner-verified display drivers.
- **Complete Application Directory Restructuring (Subfolders)**: Evaluated with care. Deep directory refactoring risks breaking existing historical evidence paths and test assumptions (e.g. `tests/docs/test_check_docs.py` which explicitly mutates `"src/apps/SystemInfoApp.cpp"`). The modular layering was achieved through clear directory responsibilities (`core/`, `hardware/`, `services/`, `ui/`, `apps/`) without disruptive file churn.

## Consequences

- Dependency directions are strictly downward: Applications -> Core/UI/Services; Core -> EventTypes/Services; Hardware -> EventTypes. No circular dependencies exist.
- Binary size growth is negligible: +48 bytes on disk (+0.005% flash image growth), well below the 5% budget review threshold.
- Verified across all environments: PlatformIO target builds (`lilygo-t-display-s3`, `profile`, `ledc`), host Zig smoke tests, and automated MSVC desktop simulator tests all pass with zero regressions.
- Physical hardware validation on real board remains outstanding (HWT-0012 pending device observation).
