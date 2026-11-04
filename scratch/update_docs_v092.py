# 1. Update docs/CURRENT_STATE.md
with open('docs/CURRENT_STATE.md', 'r', encoding='utf-8') as f:
    cs = f.read()

cs = cs.replace('"runtime_version": "v0.9.1"', '"runtime_version": "v0.9.2"')
cs = cs.replace('"latest_build": "VER-0107"', '"latest_build": "VER-0111"')
cs = cs.replace('"latest_upload": "HWT-0033"', '"latest_upload": "HWT-0034"')
cs = cs.replace('"documentation_verification": "VER-0108"', '"documentation_verification": "VER-0112"')
cs = cs.replace('"current_host_attempt": "VER-0105"', '"current_host_attempt": "VER-0109"')

with open('docs/CURRENT_STATE.md', 'w', encoding='utf-8') as f:
    f.write(cs)
print("Updated docs/CURRENT_STATE.md")

# 2. Update CHANGELOG.md
with open('CHANGELOG.md', 'r', encoding='utf-8') as f:
    cl = f.read()

target_cl = "## Unreleased\n\n"
new_cl_entry = """## Unreleased

### Web Portal & Account Setup LinkedIn Integration (v0.9.2) — 2026-11-04
- Added official LinkedIn profiles and responsive badges to all web pages:
  - Embedded Web Portal (`src/services/WebPortalService.cpp` & `tools/account_setup/test_portal_preview.html`).
  - Standalone Account Setup application (`tools/account_setup/account_setup.html` & `tools/account_setup/account_setup.css`).
  - Added Nibru official LinkedIn link: `https://www.linkedin.com/company/nibruiq/`
  - Added Haider Razaq LinkedIn link: `https://www.linkedin.com/in/haiderrazaq/`
  - Styled with glowing neon hover effects and embedded vector SVG LinkedIn icons.
- Sequentially bumped firmware version to v0.9.2 across `include/NibruConfig.h`, `include/BoardConfig.h`, `src/main.cpp`, and `docs/CURRENT_STATE.md`.
- Verified with host unit tests (VER-0109), desktop automated test suite (VER-0110), PlatformIO release build (958,896 B binary, 14.6% flash, 19.9% RAM, VER-0111), documentation verification (VER-0112), and physical upload to device on COM5 (HWT-0034).

"""

if target_cl in cl and "v0.9.2" not in cl:
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
  "id": "DEV-0043",
  "date": "2026-11-04",
  "related": [
    "FEAT-0019",
    "VER-0109",
    "VER-0110",
    "VER-0111",
    "VER-0112",
    "HWT-0034"
  ],
  "action": "WEB PORTAL & ACCOUNT SETUP LINKEDIN INTEGRATION / VERSION BUMP v0.9.2",
  "files": [
    "src/services/WebPortalService.cpp",
    "tools/account_setup/test_portal_preview.html",
    "tools/account_setup/account_setup.html",
    "tools/account_setup/account_setup.css",
    "include/NibruConfig.h",
    "include/BoardConfig.h",
    "src/main.cpp",
    "docs/CURRENT_STATE.md",
    "CHANGELOG.md",
    "docs/DEVELOPMENT_LOG.md",
    "docs/VERIFICATION_REPORT.md",
    "docs/HARDWARE_TEST_LOG.md"
  ],
  "description": "Added official LinkedIn profile links for Haider Razaq (https://www.linkedin.com/in/haiderrazaq/) and Nibru (https://www.linkedin.com/company/nibruiq/) to the embedded Web Portal (src/services/WebPortalService.cpp), test preview (tools/account_setup/test_portal_preview.html), and standalone web setup tool (tools/account_setup/account_setup.html, tools/account_setup/account_setup.css). Integrated vector SVG LinkedIn icons, responsive button pills, and interactive hover animations. Monotonically bumped firmware version to v0.9.2.",
  "reason": "Direct user request to add Haider Razaq and Nibru LinkedIn links to the web pages.",
  "expected": "Clickable, styled LinkedIn badges on all web pages, zero regression on existing endpoints, passes host and desktop tests, compiles and uploads cleanly to device.",
  "actual": "Host tests PASS (VER-0109). Desktop tests PASS (VER-0110). PlatformIO build PASS with 958,896 bytes (14.6% flash, 19.9% RAM, VER-0111). Uploaded to physical device via COM5 (HWT-0034). Documentation checks PASS (VER-0112).",
  "verification": "VER-0109, VER-0110, VER-0111, VER-0112, HWT-0034",
  "risks": "Minimal; flash usage grew by 1,920 B for vector icons and URLs, well within app0 partition capacity.",
  "commit": "WORKING_TREE",
  "follow_up": "User tests the updated Web Portal in browser and confirms LinkedIn links open correctly."
}
```
"""

if "DEV-0043" not in dev:
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
  "id": "VER-0109",
  "date": "2026-11-04",
  "firmware_version": "v0.9.2",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Python and C++ host test suite execution for v0.9.2",
  "hardware": "None (Host simulated)",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "PASS with zero errors",
  "actual": "PASS: All host tests passed (lifecycle, import, vault, web portal, navigation, crypto, settings)",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v092-host-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "Windows 11 / MSVC 2022 / Python 3.14",
  "limitations": "Host-only simulation; does not verify physical display, touch or hardware RF"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0110",
  "date": "2026-11-04",
  "firmware_version": "v0.9.2",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Automated desktop test suite for v0.9.2",
  "hardware": "None (Desktop simulation)",
  "wiring": "None",
  "procedure": "cmd /c tools\\\\run_desktop_tests.bat",
  "expected": "PASS with zero errors",
  "actual": "PASS: All desktop simulator tests passed (lifecycle, UI, Wi-Fi, settings, HAL, sleep/wake, vault, web portal, 25 entry/exit cycles)",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v092-desktop-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "Windows 11 / MSVC 2022 / SDL2",
  "limitations": "Desktop simulation only; does not verify physical ESP32-S3 hardware"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0111",
  "date": "2026-11-04",
  "firmware_version": "v0.9.2",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "PlatformIO release build for lilygo-t-display-s3",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "PASS with zero errors",
  "actual": "PASS: Flash: 958,537 B (14.6% of 6.25 MB partition), RAM: 65,096 B (19.9%), binary: 958,896 B",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v092-pio-build.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / espressif32@6.5.0 / Arduino-ESP32 2.0.14",
  "limitations": "Scope limited to compile-time analysis"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0112",
  "date": "2026-11-04",
  "firmware_version": "v0.9.2",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Documentation check for v0.9.2",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v092-docs-check.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to documentation schema"
}
```
"""

if "VER-0112" not in ver:
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
  "id": "HWT-0034",
  "date": "2026-11-04",
  "firmware_version": "v0.9.2",
  "commit": "WORKING_TREE",
  "environment": "Physical LILYGO T-Display-S3 on COM5 via esptool.py",
  "wiring": "Factory ST7789V 8-bit parallel, CST816S capacitive touch, AW9364 backlight",
  "verification_type": "HARDWARE",
  "scope": "Authorized v0.9.2 firmware upload with LinkedIn links integrated into Web Portal",
  "hardware": "ESP32-S3 revision v0.2, COM5 VID 303A PID 1001 MAC 44:1b:f6:d3:aa:bc",
  "procedure": "Upload complete v0.9.2 binary (958,896 B) via PlatformIO esptool directly to COM5 at 460800 baud",
  "expected": "Write intended v0.9.2 image (958,896 B), verify data hashes, reset device via RTS pin",
  "actual": "SUCCESS in 35.15s. Application 958,896 B (609,720 compressed) written to 0x10000 in 8.3 seconds (924.5 kbit/s). Hash of data verified on all segments. Hard reset via RTS sent.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v092-upload-com5.txt"
  ],
  "evidence_kind": "Captured PlatformIO/esptool terminal output for v0.9.2 firmware upload",
  "limitations": "Flashing and hardware reset verified; interactive validation of Web Portal in browser requested from user"
}
```
"""

if "HWT-0034" not in hwt:
    hwt = hwt.rstrip() + "\n" + new_hwt_entry
    with open('docs/HARDWARE_TEST_LOG.md', 'w', encoding='utf-8') as f:
        f.write(hwt)
    print("Updated docs/HARDWARE_TEST_LOG.md")
