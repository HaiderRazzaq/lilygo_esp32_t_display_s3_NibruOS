# Known issues and verification blockers

Current status is mutable. Attempts remain in these records and in
[append-only history](DEVELOPMENT_LOG.md). FIXED — UNVERIFIED means a change
exists, not that the original physical failure has been resolved.
Status vocabulary and resolution rules: [workflow](ENGINEERING_WORKFLOW.md).

```json
{
  "record_type": "issue",
  "id": "BUG-0001",
  "title": "Wi-Fi scan timeout: physical resolution unverified",
  "status": "FIXED — UNVERIFIED",
  "description": "Earlier flashed NibruOS displayed Scan timed out; code mitigation exists but root cause and device resolution remain unconfirmed.",
  "symptoms": "No completed scan before original 16-second timeout.",
  "reproduction": "Earlier firmware: open Wi-Fi Scanner. Current firmware: run HWT-0004 and capture [WiFi] logs, retries, rescan and Back responsiveness.",
  "affected_versions": "Owner-reported earlier v0.1; exact image/commit UNKNOWN. v0.1.1-source-patch awaits device verification.",
  "suspected_cause": "STA readiness, async driver/event completion, power or board/RF configuration; none confirmed.",
  "root_cause": "UNCONFIRMED",
  "files": [
    "src/services/WiFiService.cpp",
    "src/apps/WiFiScannerApp.cpp",
    "docs/WIFI_DIAGNOSTICS.md",
    "tests/host/test_main.cpp"
  ],
  "attempts": [
    "Original immediate STA+scan and scanDelete-only shutdown: owner reported timeout; old source not separately present in Git.",
    "Historical patch adds warmup, completion-first polling, explicit scan stop, bounded params and one restart/retry. Host PASS reported in legacy verification; device outcome UNKNOWN.",
    "2026-11-01 target build PASS (VER-0001), current host rerun BLOCKED (VER-0002); no upload."
  ],
  "fix": "Implemented reliability mitigation, not established root-cause fix; see WIFI_DIAGNOSTICS.",
  "verification": [
    "HWT-0002",
    "HWT-0004",
    "VER-0001",
    "VER-0002"
  ],
  "risks": "No proof of Wi-Fi reception or physical responsiveness; do not blame antenna from a timeout alone."
}
```

```json
{
  "record_type": "issue",
  "id": "BUG-0002",
  "title": "CST816S source dependency is not commit-pinned",
  "status": "OPEN",
  "description": "platformio.ini points at a Git URL without a revision.",
  "symptoms": "New installations may resolve a different touch library.",
  "reproduction": "Inspect lib_deps; compare a clean resolution with DEPENDENCIES snapshot.",
  "affected_versions": "Current baseline",
  "suspected_cause": "Floating Git dependency configuration",
  "root_cause": "Confirmed configuration lacks revision suffix.",
  "files": [
    "platformio.ini",
    "docs/DEPENDENCIES.md"
  ],
  "attempts": [
    "Observed local 1.3.0 SHA 11fe1fbae1f226c328a9b08d808fb8f97d8a4c50; no library change attempted."
  ],
  "fix": "Not implemented; propose separately reviewed SHA pin after compatibility tests.",
  "verification": [
    "VER-0001"
  ],
  "risks": "Build reproducibility and driver compatibility differ across fresh setups."
}
```

```json
{
  "record_type": "issue",
  "id": "BUG-0003",
  "title": "Runtime/source-patch version identity differs",
  "status": "OPEN",
  "description": "Runtime v0.1 differs from the legacy v0.1.1-source-patch label; no central version or release tags.",
  "symptoms": "Logs and Home cannot identify the Wi-Fi patch by version alone.",
  "reproduction": "Compare main boot string, AppManager Home title and docs/CHANGELOG.md.",
  "affected_versions": "Current baseline",
  "suspected_cause": "Patch documentation updated independently of runtime labels.",
  "root_cause": "Source literals remain v0.1.",
  "files": [
    "src/main.cpp",
    "src/core/AppManager.cpp",
    "docs/CHANGELOG.md",
    "docs/CURRENT_STATE.md"
  ],
  "attempts": [
    "Documented both identities without a firmware change or automatic version bump."
  ],
  "fix": "Not implemented; decide centralized identity and release scope before tagging.",
  "verification": [
    "DEV-0004"
  ],
  "risks": "Use commit and image hash for physical tests; a version string alone is insufficient."
}
```

```json
{
  "record_type": "issue",
  "id": "BUG-0004",
  "title": "MicroSD integration prerequisites not verified",
  "status": "BLOCKED",
  "description": "Feature is planned; acquired module has no confirmed model, connection, power characterization or source implementation.",
  "symptoms": "No storage capability exists. This is an integration blocker, not a demonstrated SD failure.",
  "reproduction": "Inspect source and hardware inventory; verify physical module and proposed wiring before implementation.",
  "affected_versions": "Current baseline; FEAT-0007 planned",
  "suspected_cause": "Missing hardware identification and verification",
  "root_cause": "No SD source/service or connection evidence.",
  "files": [
    "docs/HARDWARE_INVENTORY.md",
    "docs/HARDWARE_PINOUT.md"
  ],
  "attempts": [
    "No wiring, driver implementation or SD tests attempted."
  ],
  "fix": "Not implemented; identify module/voltage/current and approve actual pin allocation before adding bounded storage service.",
  "verification": [
    "HWT-0006"
  ],
  "risks": "Module VIN/level shifting may be unsuitable for proposed 3.3V; optional SD shield conflicts."
}
```

```json
{
  "record_type": "issue",
  "id": "BUG-0005",
  "title": "Local build cleanup encounters locked artifacts",
  "status": "OPEN",
  "description": "PlatformIO clean reports Windows file locks and nonempty directory even though command exits zero.",
  "symptoms": "WinError 32 for bootloader.bin/firmware.bin/partitions.bin and WinError 145 for build directory.",
  "reproduction": "Observed during pio run -e lilygo-t-display-s3 -t clean on this workspace; reproduction after lock release UNKNOWN.",
  "affected_versions": "Local Windows environment on 2026-11-01; not a demonstrated firmware fault",
  "suspected_cause": "Another process holds artifact files; process identity UNKNOWN.",
  "root_cause": "File-lock errors observed; locking process not established.",
  "files": [
    "docs/evidence/2026-11-01-environment.txt"
  ],
  "attempts": [
    "Clean did not fully remove default build artifacts; did not force-delete files or stop other processes.",
    "Full build in ignored .testbuild/pio-build succeeded (VER-0001)."
  ],
  "fix": "Workaround only: alternate PLATFORMIO_BUILD_DIR. No fix to locking process.",
  "verification": [
    "VER-0001",
    "VER-0004"
  ],
  "risks": "Do not treat zero clean exit code as proof of a clean tree; default-directory locks may recur."
}
```

```json
{
  "record_type": "issue",
  "id": "BUG-0006",
  "title": "Documentation validator record-lifecycle rules required correction",
  "status": "VERIFIED RESOLVED",
  "description": "Initial checker required historical DEV source paths to exist forever and allowed HWT records outside the physical log.",
  "symptoms": "A documented source rename failed validation unless old DEV history was rewritten; moving an HWT entry into the software report was accepted.",
  "reproduction": "Run test_file_rename_preserves_immutable_development_history and test_hardware_record_must_stay_in_dedicated_log on the initial checker.",
  "affected_versions": "Initial uncommitted FEAT-0008 implementation in this session only; no released firmware affected.",
  "suspected_cause": "Record validation treated historical/current path semantics alike and did not enforce the physical document location.",
  "root_cause": "Confirmed by independent review and two failing mutation regressions.",
  "files": [
    "tools/check_docs.py",
    "tests/docs/test_check_docs.py",
    "docs/ENGINEERING_WORKFLOW.md"
  ],
  "attempts": [
    "Initial checker: 14 tests passed but did not cover these lifecycle cases.",
    "New tests each reproduced the finding before fixes; whole 16-test suite then passed."
  ],
  "fix": "DEV files may be historical absent paths within the repository; current feature/issue files and evidence still require existence. HWT records must use HARDWARE type and reside in HARDWARE_TEST_LOG.",
  "verification": [
    "VER-0005"
  ],
  "risks": "Structural validation still cannot prove real device observations or enforce append-only history against Git."
}
```
