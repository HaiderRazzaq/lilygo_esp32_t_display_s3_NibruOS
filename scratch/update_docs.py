import os

# 1. Update docs/CURRENT_STATE.md
with open('docs/CURRENT_STATE.md', 'r', encoding='utf-8') as f:
    cs = f.read()

cs = cs.replace('"runtime_version": "v0.9.0"', '"runtime_version": "v0.9.1"')
cs = cs.replace('"latest_build": "VER-0103"', '"latest_build": "VER-0107"')
cs = cs.replace('"latest_upload": "HWT-0032"', '"latest_upload": "HWT-0033"')
cs = cs.replace('"documentation_verification": "VER-0104"', '"documentation_verification": "VER-0108"')
cs = cs.replace('"current_host_attempt": "VER-0101"', '"current_host_attempt": "VER-0105"')

with open('docs/CURRENT_STATE.md', 'w', encoding='utf-8') as f:
    f.write(cs)
print("Updated docs/CURRENT_STATE.md")

# 2. Update CHANGELOG.md
with open('CHANGELOG.md', 'r', encoding='utf-8') as f:
    cl = f.read()

target_cl = "## Unreleased\n\n"
new_cl_entry = """## Unreleased

### Web Portal Visual Identity & Haider Razaq Brand Alignment (v0.9.1) — 2026-11-04
- Redesigned the embedded Web Portal (`src/services/WebPortalService.cpp`) to match the official NibruOS visual identity in `tools/account_setup`:
  - Dark navy/cyan neon theme (`--brand-navy: #0f1954`, `--brand-aqua: #38ffe1`, `--bg: #080f2c`).
  - Embedded NibruOS vector SVG logo with radial and linear gradients (`#1FB6C9` -> `#38FFE1`).
  - Full RTL Arabic typography (`dir="rtl"`, `lang="ar"`).
  - Modern card layouts with glowing neon borders, prominent Web PIN authorization card, and real-time connection status pill.
  - Interactive tab switcher for Password Vault entry and 2FA Authenticator entry.
  - Developer contact information (`07838844400`) and signature (`Developed by Haider Razaq`) in the responsive footer.
  - Secure random password generation and password unmasking toggle directly in the browser interface.
  - Split embedded HTML/CSS/JS into chunked C++ raw string literals (`R"h0(...)h0" R"h1(...)h1" R"h2(...)h2"`) to comply with MSVC 16,380-byte string token limits across compilers.
- Sequentially bumped firmware version to v0.9.1 across `include/NibruConfig.h`, `include/BoardConfig.h`, `src/main.cpp`, and `docs/CURRENT_STATE.md`.
- Verified with host unit tests (VER-0105), desktop automated test suite (VER-0106), PlatformIO release build (956,976 B binary, 14.6% flash, 19.9% RAM, VER-0107), documentation verification (VER-0108), and physical upload to device on COM5 (HWT-0033).

"""

if target_cl in cl and "v0.9.1" not in cl:
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
  "id": "DEV-0042",
  "date": "2026-11-04",
  "related": [
    "FEAT-0019",
    "VER-0105",
    "VER-0106",
    "VER-0107",
    "VER-0108",
    "HWT-0033"
  ],
  "action": "WEB PORTAL BRAND ALIGNMENT / HAIDER RAZAQ VISUAL IDENTITY / VERSION BUMP v0.9.1",
  "files": [
    "src/services/WebPortalService.cpp",
    "tools/account_setup/test_portal_preview.html",
    "include/NibruConfig.h",
    "include/BoardConfig.h",
    "src/main.cpp",
    "docs/CURRENT_STATE.md",
    "CHANGELOG.md",
    "docs/DEVELOPMENT_LOG.md",
    "docs/VERIFICATION_REPORT.md",
    "docs/HARDWARE_TEST_LOG.md"
  ],
  "description": "Redesigned the embedded Web Portal (src/services/WebPortalService.cpp) to fully align with the official visual identity of NibruOS and account_setup. Implemented dark navy/cyan neon theme (#0f1954 / #38ffe1 / #080f2c), embedded vector NibruOS SVG logo, RTL Arabic layout, prominent PIN authorization card, interactive tabs for Password Vault and 2FA accounts, client-side password unmasking and random generator, and developer contact footer (07838844400 / Developed by Haider Razaq). Resolved MSVC C2026 string literal token limit by splitting kIndexHtml into adjacent raw string literals (R\\\"h0(...)h0\\\" R\\\"h1(...)h1\\\" R\\\"h2(...)h2\\\"). Monotonically bumped firmware version to v0.9.1.",
  "reason": "Align the device Web Portal with the official NibruOS visual identity, branding, Arabic localization, and developer contact per user request.",
  "expected": "Embedded single-page portal with custom branding, MSVC and ESP32 compiler compatibility, passes all host and desktop tests, builds within partition budget, uploads and resets successfully on physical hardware.",
  "actual": "Host tests PASS (VER-0105). Desktop tests PASS (VER-0106). PlatformIO build PASS with 956,976 bytes (14.6% flash, 19.9% RAM, VER-0107). Uploaded to physical device via COM5 (HWT-0033). Documentation checks PASS (VER-0108).",
  "verification": "VER-0105, VER-0106, VER-0107, VER-0108, HWT-0033",
  "risks": "String literal concatenation occurs at compile-time with zero heap overhead; flash usage grew by 14,176 B (1.5% of flash budget), remaining safely within app0 partition.",
  "commit": "WORKING_TREE",
  "follow_up": "User tests the redesigned Web Portal from phone/PC browser to verify visual identity, branding, and credential additions."
}
```
"""

if "DEV-0042" not in dev:
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
  "id": "VER-0105",
  "date": "2026-11-04",
  "firmware_version": "v0.9.1",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Python and C++ host test suite execution for v0.9.1",
  "hardware": "None (Host simulated)",
  "wiring": "None",
  "procedure": "python tools/run_host_tests.py",
  "expected": "PASS with zero errors",
  "actual": "PASS: All host tests passed (lifecycle, import, vault, web portal, navigation, crypto, settings)",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v091-host-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "Windows 11 / MSVC 2022 / Python 3.14",
  "limitations": "Host-only simulation; does not verify physical display, touch or hardware RF"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0106",
  "date": "2026-11-04",
  "firmware_version": "v0.9.1",
  "commit": "WORKING_TREE",
  "verification_type": "HOST",
  "scope": "Automated desktop test suite for v0.9.1",
  "hardware": "None (Desktop simulation)",
  "wiring": "None",
  "procedure": "cmd /c tools\\\\run_desktop_tests.bat",
  "expected": "PASS with zero errors",
  "actual": "PASS: All desktop simulator tests passed (lifecycle, UI, Wi-Fi, settings, HAL, sleep/wake, vault, web portal, 25 entry/exit cycles)",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v091-desktop-tests.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "Windows 11 / MSVC 2022 / SDL2",
  "limitations": "Desktop simulation only; does not verify physical ESP32-S3 hardware"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0107",
  "date": "2026-11-04",
  "firmware_version": "v0.9.1",
  "commit": "WORKING_TREE",
  "verification_type": "BUILD",
  "scope": "PlatformIO release build for lilygo-t-display-s3",
  "hardware": "ESP32-S3 (lilygo-t-display-s3)",
  "wiring": "Standard LILYGO T-Display-S3 ST7789 parallel / CST816S",
  "procedure": "pio run -e lilygo-t-display-s3",
  "expected": "PASS with zero errors",
  "actual": "PASS: Flash: 956,617 B (14.6% of 6.25 MB partition), RAM: 65,096 B (19.9%), binary: 956,976 B",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v091-pio-build.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / espressif32@6.5.0 / Arduino-ESP32 2.0.14",
  "limitations": "Scope limited to compile-time analysis"
}
```

```json
{
  "record_type": "test",
  "id": "VER-0108",
  "date": "2026-11-04",
  "firmware_version": "v0.9.1",
  "commit": "WORKING_TREE",
  "verification_type": "DOCUMENTATION",
  "scope": "Documentation check for v0.9.1",
  "hardware": "None",
  "wiring": "None",
  "procedure": "python tools/check_docs.py",
  "expected": "PASS with zero errors",
  "actual": "PASS with zero errors; exit code 0",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v091-docs-check.txt"
  ],
  "evidence_kind": "Execution log file",
  "environment": "PlatformIO 6.1.18 / MSVC 2022 / Python 3.14",
  "limitations": "Scope limited to documentation schema"
}
```
"""

if "VER-0108" not in ver:
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
  "id": "HWT-0033",
  "date": "2026-11-04",
  "firmware_version": "v0.9.1",
  "commit": "WORKING_TREE",
  "environment": "Physical LILYGO T-Display-S3 on COM5 via esptool.py",
  "wiring": "Factory ST7789V 8-bit parallel, CST816S capacitive touch, AW9364 backlight",
  "verification_type": "HARDWARE",
  "scope": "Authorized v0.9.1 firmware upload with redesigned branded Web Portal",
  "hardware": "ESP32-S3 revision v0.2, COM5 VID 303A PID 1001 MAC 44:1b:f6:d3:aa:bc",
  "procedure": "Upload complete v0.9.1 binary (956,976 B) via PlatformIO esptool directly to COM5 at 460800 baud",
  "expected": "Write intended v0.9.1 image (956,976 B), verify data hashes, reset device via RTS pin",
  "actual": "SUCCESS in 35.22s. Application 956,976 B (608,634 compressed) written to 0x10000 in 8.3 seconds (924.2 kbit/s). Hash of data verified on all segments. Hard reset via RTS sent.",
  "result": "PASS",
  "evidence": [
    "docs/evidence/2026-11-04-v091-upload-com5.txt"
  ],
  "evidence_kind": "Captured PlatformIO/esptool terminal output for v0.9.1 firmware upload",
  "limitations": "Flashing and hardware reset verified; interactive validation of redesigned Web Portal visual identity and browser interaction requested from user"
}
```
"""

if "HWT-0033" not in hwt:
    hwt = hwt.rstrip() + "\n" + new_hwt_entry
    with open('docs/HARDWARE_TEST_LOG.md', 'w', encoding='utf-8') as f:
        f.write(hwt)
    print("Updated docs/HARDWARE_TEST_LOG.md")
