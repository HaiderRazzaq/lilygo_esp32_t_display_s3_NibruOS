# Wi-Fi Scanner: “Scan timed out” — investigation and troubleshooting

## What is KNOWN, and what is only suspected?

**Observed:** User flashed earlier NibruOS v0.1 firmware; Wi-Fi Scanner displayed `Scan timed out`.

**Verified by source inspection:** earlier `WiFiService.cpp` launched `WiFi.scanNetworks(true)` immediately after `WiFi.mode(WIFI_STA)`, called `scanComplete()` repeatedly and set a hard timeout after 16 seconds. On abandonment, it used `WiFi.scanDelete()` and then disabled Wi-Fi. In Arduino-ESP32 **2.0.14**, `scanDelete()` only releases Arduino's cached scan results; **it does not call `esp_wifi_scan_stop()`**. The Arduino 2.0.14 scan implementation signals completion through Wi-Fi driver events. See `https://raw.githubusercontent.com/espressif/arduino-esp32/2.0.14/libraries/WiFi/src/WiFiScan.cpp`.

**Not yet established:** why completion did not arrive for this physical unit. Possibilities include an unready/stuck station driver, runtime/event interaction, power instability, ESP framework/library mismatch, or hardware RF path issue. The timeout message **does not establish** that the antenna is defective; a weak antenna would normally still allow a properly completed scan with zero results. An empty list (0 APs) is different from a driver stuck scanning.

## Changes in this source patch

1. Added a **non-blocking 650ms station warm-up** (`ScanState::Preparing`) before launching async scan.
2. Disabled station sleep while scanning and prevented reconnect to remembered AP settings.
3. Explicit scan parameters: async, active, 300ms max per channel, all 2.4GHz channels.
4. **Poll `WiFi.scanComplete()` first** before testing the 11-second application timeout, avoiding erroneous timeout if the event arrived but the main loop was delayed.
5. When aborting a started scan, **call `esp_wifi_scan_stop()`** before `WiFi.scanDelete()` and `WiFi.mode(WIFI_OFF)`.
6. Restart radio automatically for **one extra attempt**; visible `1/2`, `2/2` progress; no blocking multi-second delay in the UI loop.
7. Log reason and attempt at **115200 baud**, with status distinction for start reject, driver-reported failure and external timeout.

These are plausible reliability improvements, **not confirmed resolution** until user tests them on their board.

## Test on the real board

**Before upload:** if an earlier firmware stores real data (especially passwords), back up through its legitimate export features before reflashing; avoid `erase_flash` unless explicitly intended.

1. Build and upload current project (see `docs/BUILD_AND_TEST.md`); open serial monitor **115200**. Reset the board after the serial monitor connects if logs are missed.
2. Tap **Wi-Fi Scanner**. Observe `Starting radio... 1/2`, then `Scanning 1/2`, then either list/empty result or retry.
3. Capture serial lines starting `[WiFi]`. A healthy scan should show e.g. `scanNetworks(async)=-1`, followed by `complete: ...`, possibly 0 networks. The exact timing may vary.
4. Tap **RESCAN** at least three times, including **after returning Home and reopening**. Verify GPIO14 returns Home immediately even during scan.
5. Turn on a nearby **2.4 GHz** AP/hotspot with a visible SSID. A 5GHz-only network will not appear. An antenna problem is more plausible if scans complete but have unexpectedly low RSSI or no APs.
6. If both attempts time out, note whether serial shows `scanNetworks(async)=-1`, `-2`, repeated restarts, or crash/boot logs. Collect the complete serial output. **Do not keep increasing the timeout as a substitute for diagnosis.**

## Isolation diagnostic

If the new firmware still fails, run the independent test under `tools/wifi_scan_diagnostic/wifi_scan_diagnostic.ino` with Arduino IDE or the **standalone PlatformIO project in that directory**. It uses **synchronous** `WiFi.scanNetworks(false, false, false, 300)` and prints visible networks. **Uploading the diagnostic replaces NibruOS**: re-upload the main PlatformIO project afterward. This test skips screen, touch and app lifecycle; it helps isolate whether the radio/driver can complete any scan. It still depends on the Arduino core and your exact board settings.

| Observation | Interpretation / next action |
|---|---|
| Async NibruOS fails; synchronous diagnostic succeeds | Focus on async event completion / firmware state handling, compare Arduino core versions and event logs. |
| Both fail (`-2` / no results) | Investigate board configuration, driver init, power, core/flash compatibility, and RF connection. Does not *prove* hardware failure. |
| Both complete with 0 networks | First check known nearby 2.4GHz AP, antenna/routing, RF interference and restrictions. |
| Scan works from USB but not battery | Investigate regulator, current spikes, battery and wiring. |
| Works once, fails after rescan/re-entry | Focus on scan cancellation/driver cleanup, resource ownership and stale callbacks. |

## Build details relevant to debugging

`platformio.ini` currently pins **PlatformIO espressif32 6.5.0** (normally Arduino-ESP32 **2.0.14**). Before changing versions, save the full build output listing core/SDK packages. API changes between Arduino 2.x/3.x mean a casual upgrade can break TFT/USB and Wi-Fi source compatibility. Do one variable at a time. `WiFi.scanNetworks()` is documented by Espressif: https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html .

## What to send an AI agent if unresolved

```
Device: LILYGO T-Display-S3 Touch Glass, ESP32-S3R8 N16R8.
NibruOS version: v0.1.1 source patch, platform espressif32@6.5.0.
Observed: Wi-Fi Scanner did [state and error].
Serial log: [attach complete 115200-baud log].
Build log: [attach PlatformIO output].
Standalone Wi-Fi diagnostic result: [number of APs/result code/time].
Power: [USB/battery], Antenna: [onboard/external routing].
Please read AGENTS.md, docs/HARDWARE_PINOUT.md, and services/WiFiService.* before proposing the smallest validated change.
```
