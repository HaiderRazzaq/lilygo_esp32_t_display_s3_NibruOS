# NibruOS project context

NibruOS is one modular ESP32-S3 Arduino/PlatformIO firmware, not Linux or a loader
for other firmware binaries. Target: LILYGO T-Display-S3 Touch Glass, 16 MB flash,
8 MB PSRAM, ST7789 8-bit parallel 320x170 landscape, CST816S touch. Inspect
[pinout](HARDWARE_PINOUT.md) and AGENTS.md before editing. SDA18/SCL17/RST21/IRQ16,
power15/backlight38 and buttons0/14 remain reserved and unchanged.

Runtime version v0.3; Home currently registers Wi-Fi Hub, 2FA Authenticator and
Settings. Existing security/vault/time/USB functionality predates this task;
it is not evidence of hardware or security certification. Essential Settings
completion and resource policy are described in [Settings](SETTINGS.md),
[architecture](ARCHITECTURE.md), [current state](CURRENT_STATE.md),
[feature registry](FEATURE_REGISTRY.md) and [issues](KNOWN_ISSUES.md).

Code entry is src/main.cpp; lifecycle/router/context/resource ownership are in
src/core; shared drivers in src/hardware; services in src/services; apps in
src/apps. Keep apps non-blocking, use shared services and release app-owned jobs
on exit. Never duplicate display/touch/Wi-Fi initialization in apps.

Toolchain stays espressif32@6.5.0 / Arduino-ESP32 2.0.14, TFT_eSPI 2.5.43 and
CST816S Git library. platformio.ini preserves all verified TFT pins and default
16MB partition table. Compare actual firmware image with app0 6,553,600 B, not all
16 MB. Build/main host stubs are software checks only. See
[dependencies](DEPENDENCIES.md), [baseline](performance/BASELINE.md) and
[resource policy](RESOURCE_BUDGET.md).

Older device scanning showed Scan timed out; its mitigation remains unverified
on this changed firmware. Follow [Wi-Fi diagnostics](WIFI_DIAGNOSTICS.md), preserve
historical failures, and never infer reception from compilation. MicroSD and
antenna hardware are owned/planned, not evidence of a connected working module;
see [hardware inventory](HARDWARE_INVENTORY.md). No SD/BLE/OTA feature is added.

Start each change with code/context/budget inspection and a short verification
plan; update engineering records and run tools/check_docs.py. Follow
[documentation responsibilities](README.md) and [workflow](ENGINEERING_WORKFLOW.md).

## Authenticator completion — 2026-11-01
FEAT-0014 now has masked entry/deletion, authenticated persistence, explicit PIN
provisioning, time gating and cleanup. Default USB remains Serial/JTAG; HID has
an opt-in build. Physical acceptance pending HWT-0015. No web/password-manager/
BLE/SD/OTA feature added. See [authenticator](AUTHENTICATOR.md).

## Easier account entry — 2026-11-02
The authenticator now supports standard QWERTY/manual and paired USB/Wi-Fi paste through a localhost companion (not an embedded web cabinet). See [account setup](ACCOUNT_SETUP.md). Default image uploaded HWT-0019; native runtime and resource acceptance pending HWT-0018. No GPIO, framework, partition or display/touch/backlight change.
