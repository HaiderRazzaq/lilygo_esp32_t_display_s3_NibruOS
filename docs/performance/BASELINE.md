# Performance Baseline — NibruOS v0.3

Official resource measurements for **NibruOS v0.3** on **LILYGO T-Display-S3 Touch Glass** (ESP32-S3R8).

---

## 1. Environment & Build Metadata

- **Firmware Version:** NibruOS v0.3
- **Git Commit Baseline:** `ccbd0e625837196ce547f16788022be6465786cb` + working tree
- **PlatformIO Environment:** `lilygo-t-display-s3`
- **Platform:** `espressif32@6.5.0`
- **Framework:** `arduino-esp32 2.0.14`
- **Compiler:** `xtensa-esp32s3-elf-g++ (GCC) 8.4.0`
- **Target Partition Scheme:** `default_16MB.csv` / custom 16 MB (`app0` = 6,553,600 bytes [6.25 MiB])
- **Upload Target:** `COM3` at 460800 baud

---

## 2. Resource Consumption Measurements

### Flash Memory
- **Linked Program Size:** 766,705 bytes (748.7 KiB)
- **Application Partition (`app0`):** 6,553,600 bytes (6,400.0 KiB)
- **Partition Utilization:** 11.7%
- **Remaining Flash Headroom:** 5,786,895 bytes (5.52 MiB, 88.3% headroom)

### Internal RAM (SRAM)
- **Static BSS / Data:** 46,752 bytes (14.3% of 327,680 bytes)
- **Free Internal Heap (Boot):** ~280 KB
- **Minimum Free Internal Heap:** ~250 KB
- **Largest Free Allocatable Block:** ~180 KB

### External PSRAM
- **Detected Total PSRAM:** 8,388,608 bytes (8 MB)
- **Free PSRAM (Boot):** ~8,000,000 bytes (~7.6 MB)
- **PSRAM Headroom:** > 95%

### Hardware Drivers & Peripherals
- **ST7789 8-bit Parallel TFT:** Initialized at 320x170 landscape (`setRotation(1)`).
- **AW9364 Backlight Driver:** GPIO38 single-wire pulse dimming (100%, 75%, 50%, 25%, 0%).
- **CST816S Capacitive Touch:** I2C SDA=18, SCL=17, RST=21, INT=16 (`touch.begin(FALLING)`).
- **Wi-Fi Radio:** Managed via `WiFiService` & `ResourceManager`.

---

## 3. Verification Reference
- **Build Verification:** [VER-0010](../VERIFICATION_REPORT.md)
- **Development Record:** [DEV-0011](../DEVELOPMENT_LOG.md)

