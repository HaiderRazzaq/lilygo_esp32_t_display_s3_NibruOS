# Independent Wi-Fi scan diagnostic — PlatformIO or Arduino IDE

**Caution:** Uploading this replaces the current firmware until NibruOS is flashed again. Back up important data before flashing (particularly any data in a previous password manager). Do not erase flash unnecessarily.

### PlatformIO
1. Open this directory `tools/wifi_scan_diagnostic/` as a **separate** VS Code/PlatformIO workspace.
2. Run `pio run -e lilygo-t-display-s3`.
3. Run `pio run -e lilygo-t-display-s3 -t upload --upload-port COM7`, correcting the port.
4. Run `pio device monitor -p COM7 -b 115200`, correct port, press RESET if output is missing.
5. Read `[Diag] result=N, duration=Tms` and nearby 2.4 GHz AP names.
6. Open the main `NibruOS-v0.1/` directory and upload its firmware again.

### Arduino IDE
Open `wifi_scan_diagnostic.ino` as a sketch, configure ESP32-S3 board with 16MB flash and OPI PSRAM, enable USB CDC on boot if using native USB, upload and open 115200 serial monitor. If unsure of the exact Arduino board configuration, prefer PlatformIO as above.

This isolated synchronous scan does **not** use NibruOS touch/display/app code. It is a diagnostic, not a replacement for the non-blocking scan used in the project.
