# Memory Profiling & Resource Verification Guide

Instructions and standardized profiling scenarios for **NibruOS** on the **LILYGO T-Display-S3 Touch Glass**.

---

## 1. Supported Profiling Methods

1. **`MemoryProfiler` API (`src/core/MemoryProfiler.h`):**
   - Lightweight, zero-allocation runtime snapshot mechanism.
   - Captures `heap_caps_get_free_size(MALLOC_CAP_INTERNAL)`, `heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL)`, and `heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL)`.
   - Reports total and free external PSRAM (`ESP.getPsramSize()`, `ESP.getFreePsram()`).
   - Retrieves actual binary sketch size and total SPI flash capacity.
   - Decodes hardware reset reason via `esp_reset_reason()`.
2. **PlatformIO Build Inspection:**
   - Command: `& "$env:USERPROFILE\.platformio\penv\Scripts\pio.exe" run -e lilygo-t-display-s3`
   - Reports linked Flash bytes vs 6,553,600-byte application partition capacity, and static SRAM consumption.
3. **Serial Runtime Diagnostics:**
   - Monitored over USB CDC (`COM3` at 115200 baud).
   - Logged at boot and on major state transitions (e.g. Wi-Fi connection, settings updates).

---

## 2. Standard Profiling Scenarios

### Scenario A — Startup
- Measure memory immediately after `setup()` completes.
- Record baseline free internal heap, largest free block, and free PSRAM.

### Scenario B — Application Entry
- Record memory before opening an app from the Home Launcher.
- Enter the app, trigger its main views, and record memory under typical workload.

### Scenario C — Application Exit
- Exit the application back to the Home Launcher via the Back button or header chevron.
- Ensure all app-owned resources and `ResourceManager` tokens are released.
- Measure memory and compare against pre-entry baseline.

### Scenario D — Repeated Lifecycle Testing
- Cycle entry and exit 20 consecutive times.
- Verify that internal free heap and largest allocatable block do not monotonically degrade.

### Scenario E — Combined Workloads
- Profile Wi-Fi scanning while display and touch UI remain active.
- Verify non-blocking operation (main loop tick <= 25 ms).

---

## 3. Verification Levels
1. **STATIC CHECK:** Code inspection and syntax analysis.
2. **UNIT TEST:** Isolated module tests (e.g. Python doc checks).
3. **HOST SIMULATION:** Mocked native tests (`tests/host`).
4. **BUILD VERIFIED:** Successful compilation for `lilygo-t-display-s3`.
5. **HARDWARE VERIFIED:** Verified on physical device supported by serial logs and observation.

