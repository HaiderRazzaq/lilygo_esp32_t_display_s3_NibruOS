# BLE startup resource impact

- Change BUG-0017 / FEAT-0022, v0.10.6, working tree.
- Environment `lilygo-t-display-s3`, espressif32 6.5.0, Arduino-ESP32 2.0.14.
- Image before 1,565,408 B (v0.10.5, VER-0143), after 1,565,824 B
  (VER-0150). Delta +416 B (+0.027%). App0 capacity 6,553,600 B, with
  4,987,776 B remaining (VER-0151). The accepted pre-BLE reference still
  triggers the cumulative growth warning (+86.02%).
- Static SRAM before and after 85,784 B. Internal heap free/minimum/largest,
  PSRAM, stack headroom and compliance with the 150 KiB free internal
  heap floor are NOT MEASURED.
- The controller uses the same SDK configuration and existing task path.
  Returned controller errors bypass the Arduino spin. Startup checks add
  two heap queries and sparse serial output on explicit activation only.
  The service releases the lease on failure. No new storage, pins, bus or
  peripheral allocations were introduced by the change.
- Startup time, touch responsiveness, enable/disable cycles, and power
  remain NOT MEASURED. The controller and Bluedroid calls still execute on
  the calling task. Physical profiling scenarios A–E and BLE/Wi-Fi
  coexistence remain pending HWT-0045.
- Host fault injection VER-0146 and simulator VER-0149 passed. HWT-0044
  captured the prior interruption without the reset reason. On the new image,
  collect the last startup stage and any SDK error before attributing the
  interruption to controller startup, allocation, a panic, or supply.

Status **PASS for capacity, NOT MEASURED for runtime acceptance**.

## v0.10.7 coexistence repair

- Same PlatformIO environment. Image before 1,565,824 B, after 1,566,096 B
  (+272 B, +0.017%). App0 remains 6,553,600 B, with 4,987,504 B free.
  Static SRAM moves from 85,784 B to 85,800 B. The historical baseline
  warning remains (+86.05%). See VER-0158 and VER-0159.
- HWT-0048 measured 232,972 B free internal heap and a 221,172 B
  largest block before v0.10.6 controller startup. The ESP-IDF radio
  coexistence code then panicked. This checkpoint does not establish
  minimum or concurrent headroom on v0.10.7.
- During BLE operation Wi-Fi uses modem sleep. On BLE stop the
  normal Wi-Fi policy is restored. Wi-Fi latency and power under
  concurrent BLE remain NOT MEASURED. No new tasks, pins, buses,
  or persistent storage were introduced.
- Full runtime headroom, PSRAM, stacks, touch/UI latency, 20-cycle loss,
  and power still require HWT-0049 and scenarios A–E.

## v0.10.8 Windows disconnect diagnostics

- BUG-0018 / FEAT-0022, working tree, default `lilygo-t-display-s3` build.
  Image 1,566,096 B before and 1,566,464 B after (+368 B, +0.024%).
  App0 6,553,600 B, with 4,987,136 B remaining. The cumulative
  pre-BLE baseline warning remains (+86.10%). Static SRAM is 85,800 B
  before and after (VER-0163 and VER-0164).
- Sparse status lines add serial work on existing GAP/GATT callback paths.
  Logging can perturb callback timing. No new tasks, GPIO, storage or
  radio allocations. GAP hook is unregistered before BLE shutdown.
- Runtime internal heap, PSRAM, stack headroom, latency, power and
  profiling scenarios A–E remain NOT MEASURED for this image.
- Host and simulator passed (VER-0161 and VER-0162). Physical reproduction
  using the diagnostic build is pending HWT-0053.

## v0.11.0 Home card and keyboard readiness

- FEAT-0022 / BUG-0018, working tree, default `lilygo-t-display-s3`.
  Image 1,566,464 B before and 1,567,296 B after (+832 B, +0.053%).
  App0 6,553,600 B, with 4,986,304 B free. The pre-BLE baseline warning
  remains (+86.20%). Static SRAM 85,800 B before, 85,832 B after.
  Build and capacity passed (VER-0172 and VER-0173).
- Home and Settings share the Bluetooth app implementation. View state is
  fixed-size. The app refreshes on BLE state changes and feedback expiry.
  Opening or leaving the view does not allocate a radio task or change the
  shared keyboard lifetime. Explicit disable releases the radio lease.
- Link encryption is requested on connect. The existing BLE server callback
  also handles encrypted input-report subscription. This adds bounded state
  and one diagnostic line per subscription event. No new GPIO, bus or
  persistent storage schema.
- Internal heap, PSRAM, stacks, UI latency, power, and A–E are NOT MEASURED.
  Windows pairing, reconnect, CCCD renewal and typing remain pending HWT-0056.

## v0.11.1 Windows HID enumeration

- BUG-0018 / FEAT-0022, working tree, default PlatformIO environment.
  Image 1,567,296 B before and 1,572,160 B after (+4,864 B, +0.31%).
  App0 6,553,600 B with 4,981,440 B free. Cumulative pre-BLE warning
  +86.77%. Static SRAM 85,832 B before and 85,904 B after.
- New state uses fixed peer and connection metadata. The shared server
  callback is static and security configuration is local to startup.
  Keystroke payloads are passed directly to the existing GATT transport.
  No new task, storage schema, pin or bus.
- HWT-0058 on v0.11.0 recorded pre-controller internal heap checkpoints
  192,832 and 179,604 B during toggle/re-pair activity. This shows a
  difference of 13,228 B; allocation retention and repeatability are
  unverified. Runtime heap floor, PSRAM, stacks, latency, power and A–E
  remain NOT MEASURED for v0.11.1. See HWT-0059.

## v0.12.0 display-assisted pairing

- BUG-0018 / FEAT-0022, default PlatformIO environment, working tree.
  Image 1,572,160 B before and 1,572,800 B after (+640 B, +0.041%).
  App0 6,553,600 B with 4,980,800 B free. Cumulative pre-BLE warning
  +86.85%. Static SRAM 85,904 B before and 85,928 B after.
- The passkey is held in fixed-size session state and rendered only
  in the Bluetooth view during pairing. No new task, GPIO, bus or
  persistent setting. Existing crypto and HID permissions still gate input.
- v0.11.1 pre-controller internal heap readings were 218,904, 205,724
  and 192,568 B during consecutive toggle attempts. This roughly 13 KiB
  decrease per observed toggle requires separate repeat-cycle investigation.
  v0.12.0 physical heap, PSRAM, stacks, latency, power and A–E are
  NOT MEASURED. See HWT-0061 through HWT-0063.

## v0.12.1 BLE PnP ID correction

The firmware is 1,572,864 B (+64 B versus v0.12.0) in a 6,553,600 B app0 partition. The existing warning against the accepted baseline persists. The patch sets the existing seven-byte GATT value during BLE startup. It introduces no retained buffer, peripheral lease, radio policy, task or periodic work. Internal heap, PSRAM, stack high-water marks, UI latency and power were not measured. Device scenarios A–E and the 150 KiB internal heap target remain unverified.
