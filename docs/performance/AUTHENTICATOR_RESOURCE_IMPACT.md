# Authenticator Resource Impact Report

RESOURCE IMPACT REPORT

Change ID: DEV-0023 / FEAT-0014 / BUG-0009
Firmware Version: v0.3
Git Commit: 9513925 + uncommitted phase one
Build Environment: Windows / espressif32@6.5.0 / Arduino-ESP32 2.0.14

## Flash and static SRAM

Identified pre-change VER-0029: firmware843920 B, linked Flash843557 B,
static SRAM61240 B. Actual app0 capacity6553600 B.

| Environment | firmware.bin B | Linked Flash B | Static SRAM B | app0 remaining B |
|---|---:|---:|---:|---:|
| lilygo-t-display-s3 | 822,912 | 822,549 | 49,248 | 5,730,688 |
| lilygo-t-display-s3-hid | 858,256 | 857,897 | 61,368 | 5,695,344 |
| lilygo-t-display-s3-profile | 823,520 | 823,153 | 49,248 | 5,730,080 |
| lilygo-t-display-s3-ledc | 828,816 | 828,449 | 49,320 | 5,724,784 |

Default delta: -21008 B binary (-2.489%), -11992 B static SRAM. Default no longer
links/initializes competing TinyUSB HID; optional HID is +14336 B binary and
+128 B static SRAM versus identified pre-change default. Profile overhead versus
current default: +608 B binary, no static SRAM change. LEDC remains experimental.
All images checked against generated partitions.bin; no >5% warning. Accepted
baseline unchanged. [Hashes/bytes](../evidence/2026-11-01-authenticator-resources.json).

## Internal RAM / PSRAM / runtime

Free Before / After: NOT MEASURED
Minimum / largest free internal block / observed delta: NOT MEASURED
PSRAM free before/after/minimum/delta: NOT MEASURED; no new explicit PSRAM allocation
Stack HWM before/after: NOT MEASURED
Application Startup / maximum loop-update-render-input latency: NOT MEASURED
Power/current/thermal impact: NOT MEASURED

Account plaintext max664 B; editor24+53 B; transport queue65 B. Stack buffers
include max696-byte blob and up to664-byte account scratch; nested stack overhead
requires actual measurement. No new task, radio, GPIO or embedded dependency.
Synchronous bounded NVS commits and existing PBKDF2 require physical latency checks.
Default linker remainder does not prove >=150 KiB free internal byte-addressable
heap or fragmentation acceptance.

Storage: max696-byte TOTP blob and 51-byte credentials, plus existing lockout
counter. Legacy encrypted entries remain until explicit last-account deletion.
NVS wear/capacity not physically measured. Desktop allocations/bounded parser
and OS APIs do not enter the embedded target. Native USB19/20 already reserved.
CryptoService briefly owns SAR entropy with Wi-Fi off and releases it; future
ADC/BLE additions must coordinate this. No display/touch/backlight change.

Cleanup: exit/lock/sleep zero accounts/editor, cancel digits and retain only a
deferred release if endpoint busy. Shared Wi-Fi/NTP remain owned by their service.
Repeated lifecycle >=20 host/simulator cycles PASS, not physical leak evidence.
Physical A Startup, B Entry, C Exit, D >=20 cycles, E Combined workloads: NOT RUN.

## Verification and status

Static artifact capacity: PASS VER-0036
Host / real Windows CNG vectors: PASS VER-0033
SDL2 shared app and actual disk faults: PASS VER-0034
Four PlatformIO target builds: PASS VER-0035
Physical Hardware Test: NOT RUN HWT-0015

STATUS: WARNING — IMPLEMENTED, verification incomplete for physical UI/security/
resources. Heap/stack/PSRAM/latency/power remain NOT MEASURED. No watchdog,
allocation, RF, entropy or physical delivery claim follows from a build.
Limits: short PINs/offline guessing, no safe rekey/backup, unsigned firmware,
host focus/layout and peripheral timing. RED/blocked/failed attempts are retained
in VER-0031/VER-0032/VER-0037/VER-0038.

## Authorized upload — 2026-11-02
HWT-0016 verifies the same default822912 B image on COM3. No source/image-content,
GPIO or partition-layout change; resource delta for this upload is zero at artifact
level. Device NVS range was not targeted. Runtime metrics remain NOT MEASURED.
