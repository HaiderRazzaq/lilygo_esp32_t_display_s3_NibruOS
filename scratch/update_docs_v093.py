# 1. Update docs/CURRENT_STATE.md
with open('docs/CURRENT_STATE.md', 'r', encoding='utf-8') as f:
    cs = f.read()

cs = cs.replace('"runtime_version": "v0.9.2"', '"runtime_version": "v0.9.3"')
cs = cs.replace('"latest_build": "VER-0111"', '"latest_build": "VER-0115"')
cs = cs.replace('"latest_upload": "HWT-0034"', '"latest_upload": "HWT-0035"')
cs = cs.replace('"documentation_verification": "VER-0112"', '"documentation_verification": "VER-0116"')
cs = cs.replace('"current_host_attempt": "VER-0109"', '"current_host_attempt": "VER-0113"')

with open('docs/CURRENT_STATE.md', 'w', encoding='utf-8') as f:
    f.write(cs)
print("Updated docs/CURRENT_STATE.md")

# 2. Update CHANGELOG.md
with open('CHANGELOG.md', 'r', encoding='utf-8') as f:
    cl = f.read()

target_cl = "## Unreleased\n\n"
new_cl_entry = """## Unreleased

### 2FA Authenticator UI Restoration & Cancel Lifecycle Fix (v0.9.3) — 2026-11-04
- Fixed 2FA Authenticator screen component loss on cancel/back from ADD or DELETE (BUG-0016):
  - Enforced `cardDirty_ = true` on `TotpApp::onBack()` and on `fullRedraw_` processing inside `render()`, ensuring that after screen clearing, navigation arrows (`<`, `>`), USB button, counter, `ADD`, and `DELETE` buttons are reliably redrawn.
  - Updated `TotpApp::onDisplayRestored` to set both `fullRedraw_` and `cardDirty_`, preventing unrendered UI upon waking from sleep.
  - Enhanced `ConfirmDelete` view rendering with rounded interactive buttons (`CANCEL` and `DELETE`) via `TextKeyboard::button`.
  - Added automated host regression tests in `tests/host/test_authenticator.cpp` asserting that cancelling from both ADD and DELETE views restores all card components and action buttons.
- Sequentially bumped firmware version to v0.9.3 across `include/NibruConfig.h`, `include/BoardConfig.h`, `src/main.cpp`, and `docs/CURRENT_STATE.md`.
- Verified with host unit tests (VER-0113), desktop automated test suite (VER-0114), PlatformIO release build (958,928 B binary, 14.6% flash, 19.9% RAM, VER-0115), documentation verification (VER-0116), and physical upload to device on COM5 (HWT-0035).

"""

if target_cl in cl and "v0.9.3" not in cl:
    cl = cl.replace(target_cl, new_cl_entry, 1)
    with open('CHANGELOG.md', 'w', encoding='utf-8') as f:
        f.write(cl)
    print("Updated CHANGELOG.md")
else:
    print("CHANGELOG.md already updated or target not found")

# 3. Update docs/DEVELOPMENT_LOG.md
with open('docs/DEVELOPMENT_LOG.md', 'r', encoding='utf-8') as f:
    dev = f.read()

new_dev_entry = """
```json
{
  "record_type": "development",
  "id": "DEV-0044",
  "date": "2026-11-04",
  "related": [
    "BUG-0016",
    "VER-0113",
    "VER-0114",
    "VER-0115",
    "VER-0116",
    "HWT-0035"
  ],
  "action": "2FA AUTHENTICATOR UI RESTORATION FIX / VERSION BUMP v0.9.3",
  "files": [
    "src/apps/TotpApp.h",
    "src/apps/TotpApp.cpp",
    "tests/host/test_authenticator.cpp",
    "include/NibruConfig.h",
    "include/BoardConfig.h",
    "src/main.cpp",
    "docs/CURRENT_STATE.md",
    "CHANGELOG.md",
    "docs/DEVELOPMENT_LOG.md",
    "docs/VERIFICATION_REPORT.md",
    "docs/HARDWARE_TEST_LOG.md"
  ],
  "description": "Resolved BUG-0016 where cancelling from ADD or DELETE in 2FA Authenticator caused all UI controls (navigation arrows, bottom bar, ADD/DELETE buttons) to disappear, leaving only the OTP code and seconds timer. Root cause: TotpApp::onBack() cleared the screen via fullRedraw_ but omitted cardDirty_ = true, causing the card rendering block to be skipped. Enforced cardDirty_ = true on onBack(), on onDisplayRestored, and universally whenever fullRedraw_ runs in render(). Modernized ConfirmDelete with TextKeyboard::button styled buttons. Added regression test in test_authenticator.cpp. Monotonically bumped firmware version to v0.9.3.",
  "reason": "Fix reported UI regression where cancelling ADD or DELETE rendered an empty screen with only numbers and seconds.",
  "expected": "Cancelling ADD or DELETE restores complete card layout with navigation arrows, USB button, counter, and action buttons; passes all host and desktop tests; compiles and uploads to hardware.",
  "actual": "Host tests PASS with regression assertions (VER-0113). Desktop tests PASS (VER-0114). PlatformIO build PASS with 958,928 bytes (14.6% flash, 19.9% RAM, VER-0115). Uploaded to physical device via COM5 (HWT-0035). Documentation checks PASS (VER-0116).",
  "verification": "VER-0113, VER-0114, VER-0115, VER-0116, HWT-0035",
  "risks": "Low risk; ensures dirty flag synchronization between screen clearing and view rendering.",
  "commit": "WORKING_TREE",
  "follow_up": "User tests 2FA Authenticator on device: taps ADD or DELETE, cancels, and observes complete UI restored."
}
```
"""

if "DEV-0044" not in dev:
    dev = dev.rstrip() + "\n" + new_dev_entry
    with open('docs/DEVELOPMENT_LOG.md', 'w', encoding='utf-8') as f:
        f.write(dev)
    print("Updated docs/DEVELOPMENT_LOG.md")

# 4. Update docs/VERIFICATION_REPORT.md
with open('docs/VERIFICATION_REPORT.md', 'r', encoding='utf-8') as f:
    ver = f.read()

new_ver_entries = """
```json
{
  "record_type": "test",
  "id": "VER-0113",
  "date": "2026-11-04",
  "firmware_version": "v0.9.3",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Python and C++ host test suite execution for v0.9.3 with 2FA cancel regression assertions",
  "hardware": "None (Host simulated)",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "PASS with zero errors; card components present after cancelling ADD and DELETE",
  "actual": "PASS: All host tests passed including 2FA cancel UI restoration assertions",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v093-host-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "Windows 11 / MSVC 2022 / Python 3.14",
  "limitations": "Host-only simulation; does not verify physical display, touch or hardware RF"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0114",
  "date": "2026-11-04",
  "firmware_version": "v0.9.3",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Automated desktop test suite for v0.9.3",
  "hardware": "None (Desktop simulation)",
  "wiring": "None",
  "procedure": "cmd /c tools\\\\run_desktop_tests.bat",
  "expected": "PASS with zero errors",
  "actual": "PASS: All desktop simulator tests passed (lifecycle, UI, Wi-Fi, settings, HAL, sleep/wake, vault, web portal, 25 entry/exit cycles)",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v093-desktop-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "Windows 11 / MSVC 2022 / SDL2",
  "limitations": "Desktop simulation only; does not verify physical ESP32-S3 hardware"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0115",
  "date": "2026-11-04",
  "firmware_version": "v0.9.3",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "PlatformIO release build for lilygo-t-display-s3",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "PASS with zero errors",
  "actual": "PASS: Flash: 958,557 B (14.6% of 6.25 MB partition), RAM: 65,096 B (19.9%), binary: 958,928 B",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v093-pio-build.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / espressif32@6.5.0 / Arduino-ESP32 2.0.14",
  "limitations": "Scope limited to compile-time analysis"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0116",
  "date": "2026-11-04",
  "firmware_version": "v0.9.3",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Documentation check for v0.9.3",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v093-docs-check.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to documentation schema"
}
```
"""

if "VER-0116" not in ver:
    ver = ver.rstrip() + "\n" + new_ver_entries
    with open('docs/VERIFICATION_REPORT.md', 'w', encoding='utf-8') as f:
        f.write(ver)
    print("Updated docs/VERIFICATION_REPORT.md")

# 5. Update docs/HARDWARE_TEST_LOG.md
with open('docs/HARDWARE_TEST_LOG.md', 'r', encoding='utf-8') as f:
    hwt = f.read()

new_hwt_entry = """
```json
{
  "record_type": "test",
  "id": "HWT-0035",
  "date": "2026-11-04",
  "firmware_version": "v0.9.3",
  "commit": "WORKING_TREE",
  "environment": "Physical LILYGO T-Display-S3 on COM5 via esptool.py",
  "wiring": "Factory ST7789V 8-bit parallel, CST816S capacitive touch, AW9364 backlight",
  "verification_type": "HARDWARE",
  "scope": "Authorized v0.9.3 firmware upload with 2FA Authenticator UI restoration on cancel",
  "hardware": "ESP32-S3 revision v0.2, COM5 VID 303A PID 1001 MAC 44:1b:f6:d3:aa:bc",
  "procedure": "Upload complete v0.9.3 binary (958,928 B) via PlatformIO esptool directly to COM5 at 460800 baud",
  "expected": "Write intended v0.9.3 image (958,928 B), verify data hashes, reset device via RTS pin",
  "actual": "SUCCESS in 34.98s. Application 958,928 B (609,112 compressed) written to 0x10000 in 8.3 seconds (924.3 kbit/s). Hash of data verified on all segments. Hard reset via RTS sent.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v093-upload-com5.txt"
  ],
  "evidence_kind": "Captured PlatformIO/esptool terminal output for v0.9.3 firmware upload",
  "limitations": "Flashing and hardware reset verified; physical observation of 2FA cancel UI requested from user"
}
```
"""

if "HWT-0035" not in hwt:
    hwt = hwt.rstrip() + "\n" + new_hwt_entry
    with open('docs/HARDWARE_TEST_LOG.md', 'w', encoding='utf-8') as f:
        f.write(hwt)
    print("Updated docs/HARDWARE_TEST_LOG.md")
