# Easier Account Entry Resource Impact Report

RESOURCE IMPACT REPORT

Change ID: DEV-0025 / FEAT-0014 / BUG-0011
Firmware Version: v0.3
Git Commit: 9513925 + uncommitted account-entry and prior phase-one work
Build Environment: Windows / espressif32@6.5.0 / Arduino-ESP32 2.0.14

## Flash and static SRAM

Identified before: VER-0035 default firmware822912 B, linked Flash822549 B,
static SRAM49248 B; HWT-0016 uploaded the same image. No accepted baseline update.

| Environment | firmware.bin B | Linked Flash B | Static SRAM B | app0 remaining B |
|---|---:|---:|---:|---:|
| lilygo-t-display-s3 | 831,408 | 831,041 | 50,056 | 5,722,192 |
| lilygo-t-display-s3-hid | 866,640 | 866,277 | 62,184 | 5,686,960 |
| lilygo-t-display-s3-profile | 831,984 | 831,617 | 50,064 | 5,721,616 |
| lilygo-t-display-s3-ledc | 837,216 | 836,853 | 50,128 | 5,716,384 |

Default binary +8496 B (+1.0324%), static SRAM +808 B. HID versus its previous
858256/61368 B image: +8384 B binary / +816 B static. Profile overhead versus
current default: +576 B binary / +8 B static; LEDC remains experimental.
Every image fits generated app0 6553600 B. Both the preserved accepted baseline
post-build check and identified before/after comparison stay below 5% review.
[Captured bytes/hashes](../evidence/2026-11-02-account-entry-resources.json).

## Internal RAM, PSRAM, stacks, timing, storage and power

Free Before / After / Minimum / Largest Internal Block / observed delta: NOT MEASURED
PSRAM Before / After / Minimum / Delta: NOT MEASURED
FreeRTOS stack HWM Before / After: NOT MEASURED
Startup / entry / exit / loop-update-render-input latency: NOT MEASURED
Power / current / thermal / battery impact: NOT MEASURED

The service retains a 256-byte frame, 32-byte key, 17-byte pairing code and
79-byte candidate plus small counters/pointers. The receive tick stack has
64 input bytes; decoding nests 12-byte IV, 96-byte ciphertext, 16-byte tag,
32-byte decoded key and 97-byte plaintext arrays plus mbedTLS overhead.
No new task or explicit PSRAM allocation. Linker SRAM is not runtime headroom.

Native Arduino WiFiUDP additionally allocates a 1460-byte temporary parse
buffer and a packet cbuf up to the received datagram length (at most 1460 in
this pinned implementation), plus socket/lwIP allocations. These are not
included in the service's 256-byte frame. The parser consumes at most64 bytes
per tick, rejects frame overflow and stops after eight bad frames; framework
allocation behavior, fragmentation and scheduling need physical measurements.
USB reads only during receive. AES-GCM/SHA256, existing synchronous vault NVS
commit and PIN work need actual input-latency/stack checks. Explicit import
keeps the display awake for up to120 s, affecting power; no new radio/AP/scan.

Storage schema unchanged: existing max696-byte encrypted vault and atomic51-byte
credential blob. No seed/pairing files or logs. Python/Node/browser/SDL/CNG
dependencies do not enter the firmware. No GPIO/bus/voltage/wiring/partition/
framework/backlight changes. Shared existing Wi-Fi connection is required.

Cleanup: close UDP and erase pairing/key/frame at first candidate; SAVE or
cancel/Back/exit/lock/sleep/expiry wipes candidate and releases importer lease.
Wi-Fi disconnection stops receive. Host tests cover ownership conflicts,
disconnect cleanup and >=20 cycles; this is not physical leak evidence.

Physical profiling A Startup, B Entry, C Exit, D >=20 cycles and E Combined
Wi-Fi/NTP/TOTP/USB workloads: NOT RUN HWT-0018. >=150 KiB free internal
byte-addressable heap and <=10% heap/largest-block regression acceptance are
NOT VERIFIED; stack/PSRAM/latency/power remain NOT MEASURED.

## Verification and status

VER-0042 HOST/CNG PASS; VER-0043 shared SDL simulator PASS.
VER-0044 helper/Python/Node/WebCrypto checks PASS; rendered local page inspected.
VER-0045 four PlatformIO builds PASS; VER-0046 capacity/resource checks PASS.
VER-0048 independent final source review found no remaining blocker.
VER-0041 retains RED and intermediate setup failures with subsequent corrections.

STATUS: WARNING — IMPLEMENTED, verification incomplete for native USB/UDP,
physical touch/UI/persistence and resource acceptance. No security certification,
RF reception, free-heap or power claim follows from these software checks.

## Authorized upload
HWT-0019 uploaded the identified default831408 B image on2026-11-02; artifact delta from the report above is zero. Matching table preflight VER-0049; no full-chip erase/NVS-range write. Runtime metrics remain NOT MEASURED.
