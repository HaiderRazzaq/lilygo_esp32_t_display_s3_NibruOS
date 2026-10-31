# Hardware inventory

Recorded 2026-11-01 from source, preserved owner reports, and the current request.
UNKNOWN means evidence is missing. Electrical allocation belongs in
[HARDWARE_PINOUT](HARDWARE_PINOUT.md); test outcomes belong in
[HARDWARE_TEST_LOG](HARDWARE_TEST_LOG.md).

| Item | Installation / ownership | Firmware support | Verification / limits |
|---|---|---|---|
| T-Display-S3 Touch Glass, ESP32-S3R8, 16 MB flash / 8 MB PSRAM | Board integrated; owner target | Configured qio_opi, BoardConfig and PlatformIO | Target build PASS; actual memory values need HWT-0003 |
| ST7789V 170x320, parallel bus, power/backlight | Board integrated | DisplayManager + TFT_eSPI | Owner independent-sketch success; current image pending |
| CST816S capacitive panel | Board integrated | TouchManager; SDA18/SCL17/RST21/INT16 | Owner sketch success; rotated swipe enums pending |
| BOOT GPIO0 / user GPIO14 | Board integrated | ButtonManager active LOW/pull-ups | Host assertions historically; current physical tests pending |
| 2.4 GHz Wi-Fi / BLE radio | Board integrated | Wi-Fi discovery only; BLE unused | Earlier scan timeout; patch unverified physically |
| Native USB-C D-/D+ GPIO19/20 | Board integrated | USB CDC build flags | No current upload/CDC test performed |
| Battery ADC GPIO4 and power/charger circuitry | Board integrated | No battery-reading feature | Calibration, revision, safe battery/connector polarity unknown |
| Battery/cell | UNKNOWN whether installed | No battery gauge | Model, protection, power conditions unknown |
| External antenna + pigtail | Ownership reported in prior pinout doc | RF path not selected by this code | Connection, matching and resistor population NOT VERIFIED |
| External SPI MicroSD reader | Owner reports acquired; **not confirmed connected** | **Not implemented** (FEAT-0007) | Model, card, level shifters and current unknown; proposed 3.3V wiring only |
| Optional SD shield | Proposed board accessory, ownership UNKNOWN | Not implemented | Its CMD13/CLK11/D0=12 mapping is not external SPI mapping |
| Other sensors/NFC/SubGHz | No installation or ownership evidence | Not implemented | Do not infer from expansion headers |

No external peripheral is confirmed physically connected in this repository.
Acquisition is not installation, and a proposed pin map is not firmware support.

Before any external connection: identify module/revision, confirm 3.3V logic and
supply compatibility (some SD modules expect a different supply at VIN), common
ground, current/transient budget, decoupling, pull-ups, SPI speed, and contention
with LCD/touch/USB/UART/optional shield. The proposed SD supply is 3.3V only after
module suitability is established. Never feed 5V into an ESP32 GPIO. No current
budget or SD bus speed has been characterized.
