# Performance baseline — 2026-11-01

The measured pre-change software baseline is commit `8239bb6` on `main`, clean
at discovery. See [build evidence](../evidence/2026-11-01-settings-before-build.txt)
and [machine-readable baseline](build-baseline.json). Runtime version: v0.3.

- Environment: Windows, PlatformIO lilygo-t-display-s3, espressif32@6.5.0,
  Arduino-ESP32 2.0.14, Xtensa GCC 8.4.0, default release `-Os`.
- Dependencies: TFT_eSPI 2.5.43, CST816S Git dependency (still not revision-pinned).
- Partition source: framework default_16MB.csv. app0/app1 each 0x640000
  (6,553,600 B); SPIFFS 0x360000; NVS 0x5000; OTA metadata 0x2000;
  coredump 0x10000. Reserved slots do not implement OTA or filesystems.
- Build linked Flash: **841,377 B**; static data/BSS RAM **61,168 B**.
- Actual firmware.bin: **841,744 B**; app0 remainder **5,711,856 B**.
- SHA-256: `de13bcfea614fc3c8d5c5e2b6b88861d2d6e19bb0dfe8b6cacd0213041c552e3`.
- Build timestamp: see captured build output; no reproducible timestamp embedded.
- Configured peripherals: shared TFT/touch/buttons; Wi-Fi service and security,
  NTP/2FA/USB code exist. Actual attached external peripherals, supply, running
  user settings and wireless state: **NOT OBSERVED** this session.

## Physical baseline

All startup/free/min/largest internal heap, PSRAM free/min/largest, stack HWM,
app entry/exit, 20-cycle loss, loop/render/touch latency, current and watchdog
observations: **NOT MEASURED** for this image. No device was flashed or serial
session opened in this change. Use [profiling scenarios](../MEMORY_PROFILING.md).

## Correction to previous baseline claims

Earlier text called ~280 KB free heap, ~250 KB minimum, ~180 KB largest block
and ~8,000,000 B free PSRAM official measurements. No identified physical
measurement log supports these approximate values. They are withdrawn as
accepted baseline data. Historical records remain intact; the correction is
appended to the development/verification logs. Static RAM remainder is not free
heap and cannot establish the >=150 KiB runtime requirement.

After-change costs are in [Resource Impact Report](SETTINGS_RESOURCE_IMPACT.md).
Do not replace this pre-change reference merely to suppress regression warnings.

Current session comparison: [authenticator impact](AUTHENTICATOR_RESOURCE_IMPACT.md). Accepted baseline above is unchanged.

Easier account entry comparison: [account-entry impact](ACCOUNT_ENTRY_RESOURCE_IMPACT.md), VER-0046. Accepted baseline is preserved; actual runtime metrics remain NOT MEASURED.
