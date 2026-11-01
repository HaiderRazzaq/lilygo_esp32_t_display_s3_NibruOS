# Resource Budget & Engineering Limits

Permanent project-wide engineering resource policy and budget specifications for **NibruOS** on the **LILYGO T-Display-S3 Touch Glass** (ESP32-S3R8).

---

## 1. Permanent Resource Budgets & Constraints

| Resource | Hard Constraint / Capacity | Baseline (v0.3) | Target Operational Headroom | Review Warning Trigger |
| :--- | :--- | :--- | :--- | :--- |
| **Flash Partition** | `app0` = 6,553,600 B (6.25 MB) | 766,705 B (11.7%) | >= 3,000,000 B (>= 45%) | Growth > 5% (+38 KB) |
| **Internal SRAM** | 512 KB total (~320 KB heap) | 46,752 B static (14.3%) | Free Heap >= 150 KB | Free heap drop > 10% |
| **Largest Free Block** | Contiguous internal block | ~180 KB observed | Largest block >= 80 KB | Block drop > 10% |
| **External PSRAM** | 8 MB (8,388,608 B) | ~8.0 MB free | Free PSRAM >= 7.0 MB | Unexpected leak > 64 KB |
| **Main Loop Latency** | Watchdog / UI responsiveness | 12 ms target tick | Average update < 25 ms | Spike > 50 ms sustained |
| **Wi-Fi Scanner** | 12 records max; async bounded | Single radio owner | Max 11s scan deadline | Attempt timeout / leak |
| **GPIO Allocation** | 0 conflicting reassignments | LCD/Touch reserved | GPIO10..13 unverified | Any unreviewed pin clash |

---

## 2. Hard Failure Rules
1. **Partition Overflow:** Firmware binary exceeding 6,553,600 bytes is an immediate build failure.
2. **Internal Memory Exhaustion:** Internal free heap falling below 64 KB triggers critical review.
3. **Repeated Lifecycle Leak:** Sustained decrease in free heap across 20 app entry/exit cycles.
4. **Pin Assignment Clash:** Overwriting LCD parallel bus or CST816S touch pins (18/17/21/16/15/38).
5. **Fabricated PASS:** Reporting hardware pass or runtime metrics without physical observation.

---

## 3. Resource Ownership Matrix

| Resource | Primary Owner | Secondary / Shared Access | Cleanup Rule on `onExit()` |
| :--- | :--- | :--- | :--- |
| **ST7789 Display** | `DisplayManager` | Shared via `AppContext` | Reset local redraw flags |
| **CST816S Touch** | `TouchManager` | Event polling in `loop()` | Discard stale touch gestures |
| **Backlight PWM** | `DisplayManager` / `AW9364` | `SettingsService` | Restore configured brightness |
| **2.4 GHz Radio** | `ResourceManager` | `WiFiScannerApp` (`WiFiApp`) | Stop scan, release `WiFiScanner` lock |
| **Preferences NVS** | `SettingsService`, `WiFiService` | Namespaced (`nibru_settings`) | Close handle (`prefs.end()`) |

---

## 4. Standard Resource Impact Report Format

```text
RESOURCE IMPACT REPORT

Change ID:
Feature:
Firmware Version:
Git Commit:
Build Environment:

FLASH:
Before:
After:
Delta:
Application Partition Capacity:
Remaining Headroom:

INTERNAL RAM:
Free Before:
Free After:
Minimum Free:
Largest Free Block:
Observed Delta:

PSRAM:
Free Before:
Free After:
Minimum Observed Free:
Observed Delta:

RUNTIME:
Application Startup:
Maximum Observed Loop/Update Time:
UI Responsiveness:
Resource Cleanup:
Repeated Entry/Exit Test:

OTHER:
Stack Impact:
Peripheral Resource Impact:
Storage Impact:
GPIO Impact:
Power Impact:

VERIFICATION:
Static Analysis:
Unit Tests:
PlatformIO Build:
Physical Hardware Test:

RISK ASSESSMENT:
Regression Findings:
Known Limitations:
Outstanding Verification:

STATUS:
PASS / WARNING / FAIL / NOT MEASURED

EVIDENCE:
Logs, measurements, or relevant test references.
```

