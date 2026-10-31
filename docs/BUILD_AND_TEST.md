# NibruOS v0.1 — build, upload and test (Windows)

## Environment / tools

- Visual Studio Code + PlatformIO IDE extension.
- Windows 10/11; USB-C **data** cable; LILYGO **T-Display-S3 Touch Glass**, not AMOLED/Pro variants.
- Internet connection for initial download of `espressif32@6.5.0`, `TFT_eSPI@2.5.43`, and `fbiego/CST816S` from GitHub.
- Native Arduino ESP32 framework is selected by PlatformIO; **do not use the unrelated LilyGo-display-library / LovyanGFX** in this build.

The Git repository URL for CST816S is not pinned to a verified commit in v0.1; for repeatable releases, lock it to a verified commit after the first successful device build and record that SHA. PlatformIO will clone it once at the initial build.

## Build

**VS Code:** `File → Open Folder` → select the **NibruOS-v0.1** folder. Wait for PlatformIO initialization. Click **Build** (checkmark in PlatformIO toolbar).

**PowerShell terminal:**

```powershell
pio run -e lilygo-t-display-s3
```

If `pio` is not in `PATH`, use the PlatformIO extension's built-in Project Tasks → General → Build, or open a PlatformIO Core CLI terminal. A common Windows fallback is `& "$env:USERPROFILE\.platformio\penv\Scripts\pio.exe" run -e lilygo-t-display-s3`; verify that executable exists first. Latest observed versions/results are in [DEPENDENCIES](DEPENDENCIES.md) and [CURRENT_STATE](CURRENT_STATE.md).

## Upload (flashing replaces the existing program)

1. If the old Dr. Passwords build contains account data, preserve it first. Uploading a new firmware *replaces the sketch*, and changing partitions or erasing flash can delete NVS passwords. This release contains no migrations from the old vault.
2. Connect the Touch Glass with a USB-C **data** cable. Check **Windows Device Manager → Ports (COM & LPT)** for the COM port.
3. Choose PlatformIO **Upload**, or use (replace COM7 with the actual port):

```powershell
pio run -e lilygo-t-display-s3 -t upload --upload-port COM7
```

4. To read diagnostics:

```powershell
pio device monitor -p COM7 -b 115200
```

If upload fails: unplug/replug; close Serial Monitor; try another data cable; hold **BOOT**, press/release **RST**, release **BOOT**, and repeat Upload. Press RST when finished. Boot is GPIO0; do not hold it at power-on for normal operation.

**Battery-only boot:** The default environment enables native USB CDC so logs appear via USB-C. Some board/Arduino configurations wait on USB in battery-only mode. If needed, disable the `-DARDUINO_USB_CDC_ON_BOOT=1` flag (replace with `-UARDUINO_USB_CDC_ON_BOOT` in `platformio.ini`), rebuild, and retest. Disabling CDC can move diagnostics away from USB-C.

## Device acceptance checklist

**Hardware/display**
- [ ] Screen displays **NibruOS v0.1**, 3 tiles, properly oriented at 320 × 170.
- [ ] Upper-left back touch works in every app; glyphs not corrupted and backlight remains on.
- [ ] BOOT short cycles tile highlight; BOOT hold ≥650 ms opens selected app; GPIO14 returns to Home.

**Touch Test**
- [ ] Tap top-left/bottom-right *within body*; coordinate reading stays in x=0..319, y=0..169.
- [ ] Swipe in different directions; gesture label shows the physical landscape direction (UP/DOWN/LEFT/RIGHT). If gestures are opposite, report raw gesture IDs and touch positions before editing touch settings.
- [ ] Rapid taps do not cause repeated unexpected navigation; expected touch debounce is 145 ms.

**System Info**
- [ ] Chip reports ESP32-S3; Flash approx. 16 MB; PSRAM approx. 8 MB (if PSRAM configured and initialized correctly).
- [ ] Free heap shown and uptime increases at ~1-second intervals; no continuous resets.

**Wi-Fi Scanner**
- [ ] Nearby 2.4 GHz networks appear with nonzero RSSI values; empty scan is also handled.
- [ ] `RESCAN` touch triggers a fresh async scan after the prior one completes/fails (taps while scanning do not queue rescans); interface responds to GPIO14 while scanning.
- [ ] Scrolling via swipe up/down shows additional records when >3 networks; at most 12 stored.
- [ ] Exit Wi-Fi and return to System Info without needing to restart; Wi-Fi radio is switched off on exit.
- [ ] No infinite wait: 650ms warmup and 11-second scan deadline per attempt, at most two scan attempts (about 23.3 seconds plus driver/UI overhead if both time out). Earlier 16-second timeout is historical. Immediate initialization failure may stop sooner.

**Regression**
- [ ] Run 20 successive Home ↔ Wi-Fi ↔ Home cycles; verify no freezes/reset loops.
- [ ] Run with USB disconnected from a suitable safe power source; note that logging may vary with CDC mode.
- [ ] Observe Serial Monitor for exceptions, brownouts, or repeated boots.

## Tests without a real board

A mocked host C++ test is included to validate AppManager transitions, resource ownership, async scan state handling, timeouts, button events and the touch coordinate mapping:

```bash
bash tests/host/run_tests.sh
```

Requirements: `g++` or `clang++` and Bash (Linux / WSL / Git Bash + compiler). Mock success **does not establish that PlatformIO dependencies compile or that the electrical hardware works**.

## Known issues / troubleshooting

| Symptom | Initial checks |
|---|---|
| `CST816S.h` not found | Check access to github.com/fbiego/CST816S and downloaded PlatformIO libraries |
| White/black screen | Check `USER_SETUP_LOADED`, parallel 8-bit pin definitions, `GPIO15=HIGH`, `GPIO38=HIGH`, and right device model |
| Incorrect text size or orientation | Check display rotation **1** and native 170×320 setup |
| Touch not responding | Use the original working pin sequence `SDA18/SCL17/RST21/INT16`; verify Touch Glass chip is CST816-compatible |
| Wrong touch point | Verify exact `touchX = rawY`, `touchY = 169 - rawX` landscape mapping |
| Board connection lost during upload | Close Monitor, enter BOOT/RST download mode and repeat |
| Compiles with Arduino IDE but not PlatformIO | Compare Arduino ESP32 core and TFT_eSPI versions; avoid importing custom TFT_eSPI headers from the Arduino global libraries |
| Wi-Fi scan shows no APs | Check 2.4 GHz SSIDs exist nearby; not a 5 GHz scanner; try `RESCAN` |

## Build verification status for the delivered ZIP

This section preserves the **original ZIP creation environment's** results.
Fresh 2026-11-01 target-build evidence supersedes it for current build status;
see [VERIFICATION_REPORT](VERIFICATION_REPORT.md) and [TEST_MATRIX](TEST_MATRIX.md).

- Desktop **C++ host mock build and runtime assertions**: PASS.
- `src/main.cpp` compiled against desktop mock headers: PASS.
- PlatformIO firmware build against ESP32 SDK and external libraries: **NOT RUN** (PlatformIO dependencies/tools unavailable in the creation runtime).
- ESP32 flashing and physical display/touch verification: **NOT RUN** (no physical device connection).

## v0.1.1 Wi-Fi timeout patch and diagnostics
If scanning shows `Scan timed out`, use `docs/WIFI_DIAGNOSTICS.md`. Scanner now warms up STA asynchronously (650ms), polls completion before its deadline, stops hardware scans explicitly on abort and retries once. Logs are at **115200**. The isolated diagnostic program in `tools/wifi_scan_diagnostic/` is a **separate PlatformIO project**; uploading it temporarily overwrites NibruOS until the main project is reflashed. Do not use erase-flash on devices storing important data. This updated code was host-tested but **has not been physically validated**.

## Required engineering records and current checks

Before completing a change run:

```powershell
python tools/check_docs.py
python -m unittest discover -s tests/docs -v
```

Run host smoke tests for firmware behavior changes and target build when tools
are available. The host script also checks docs; a missing compiler is BLOCKED,
not firmware FAIL. Record real outcomes/types in [VERIFICATION_REPORT](VERIFICATION_REPORT.md),
update [TEST_MATRIX](TEST_MATRIX.md), and append [DEVELOPMENT_LOG](DEVELOPMENT_LOG.md).

Physical tests must record flashed version/commit and image hash, connected
modules, GPIO, power source, procedure, expected/actual observations and sanitized
serial evidence. Pending procedures HWT-0003 through HWT-0006 are in
[HARDWARE_TEST_LOG](HARDWARE_TEST_LOG.md); append actual results without rewriting
older failures. A target build does not establish physical PASS.

If Windows cleanup reports locked artifacts even with exit0, inspect the output.
For a fresh build without deleting locked files, set `$env:PLATFORMIO_BUILD_DIR`
to an unused ignored folder such as `.testbuild/pio-build-new` before the build;
remove that environment override afterward with `Remove-Item Env:PLATFORMIO_BUILD_DIR`.
Do not stop unknown processes or force-delete files. The actual observed case and
successful alternate-directory build are VER-0004 and VER-0001.
