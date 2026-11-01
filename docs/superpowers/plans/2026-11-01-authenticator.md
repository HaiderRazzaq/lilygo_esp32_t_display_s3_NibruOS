# Authenticator Completion Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [x]`) syntax for tracking.

**Goal:** Make the existing 2FA app usable with explicitly entered accounts,
encrypted persistence and truthful time/USB states.

**Architecture:** Keep TotpService as account/storage owner, SecurityService as
PIN/session owner and TimeService/UsbHidService as shared capabilities. TotpApp
owns bounded editor state; entry/exit/lock/sleep clear secrets on both targets.

**Tech Stack:** Existing C++17 host simulation, Arduino/PlatformIO and mbedTLS.

**Spec:** [approved phase-one scope and design](../specs/2026-11-01-authenticator-design.md).

## Global Constraints

- Preserve espressif32@6.5.0 / Arduino-ESP32 2.0.14 and existing GPIO/configuration.
- app0 = 6,553,600 B; >=150 KiB free internal heap requires physical evidence.
- No secrets in source/logs, no flash/commit/push, no new web/BLE/SD features.
- Shared application code and desktop coverage; no simulator UI duplication.

## Review Focus

- Corrupt/future/partial NVS records must not silently become writable emptiness.
- Failed saves and credential changes must preserve recoverable prior state.
- Lock/sleep/Back must wipe account and editor plaintext.
- Time loss and unavailable/failed USB must never report successful sending.
- Base32 oversized/malformed input must not overflow buffers or bypass limits.

## Task 1: Baseline and encrypted account lifecycle

Files: src/services/TotpService.*, SecurityService.*, tests/host/test_authenticator.cpp,
tests/host/test_main.cpp, tools/run_host_tests.py, tests/host/run_tests.sh.
Interfaces: TotpService::begin/lock/loadVault/saveVault; transactional
addAccount(key,name,user,secret,digits,period), removeAccount(key,index),
canChangeCredentials(); SecurityService::setCredentialChangeGuard(bool(*)()).

- [x] Capture clean target/host baseline and exact resource artifacts.
- [x] RED: empty boot; save/reopen; bad key/tag/length; failed write rollback;
  oversized Base32; lock clears accounts; persisted vault blocks PIN changes.
- [x] Implement versioned authenticated bounded storage, migration and PIN guard.
- [x] GREEN: run complete host suite (real host crypto if available).

## Task 2: Valid-time and truthful USB behavior

Files: src/services/TimeService.*, UsbHidService.*, src/apps/TotpApp.*,
tests/host/test_authenticator.cpp and transport test doubles.
Interfaces: TimeService accepts a clock source (default ::time); USB availability
checks transport readiness and validates write counts; TotpApp retains distinct
waiting/error/sent feedback.

- [x] RED: invalid time cannot send; failed/disconnected writes do not report sent.
- [x] Implement time gating, remove code logging and explicit delay pacing.
- [x] GREEN: host tests and embedded compile.

## Task 3: Shared on-device account management and integration

Files: src/apps/TotpApp.*, src/ui/Base32Keyboard.h, src/main.cpp,
platform/desktop/DesktopMain.cpp, src/apps/SettingsApp.cpp,
tests/desktop/test_simulator.cpp.
Interfaces: bounded keypad emits A-Z/2-7, backspace/cancel/accept; app name then
masked secret editor persists before success; explicit deletion confirmation.

- [x] RED: shared entry/save/reopen, cancel and failed save; sleep/exit cleanup.
- [x] Implement entry and deletion flows and shared lifecycle integration.
- [x] GREEN: host/desktop suite and >=20 authentication lifecycle cycles.

## Task 4: Verification and permanent records

Files: docs/AUTHENTICATOR.md, FEATURE_REGISTRY.md, KNOWN_ISSUES.md,
CURRENT_STATE.md, DEVELOPMENT_LOG.md, VERIFICATION_REPORT.md, TEST_MATRIX.md,
HARDWARE_TEST_LOG.md, docs/performance/AUTHENTICATOR_RESOURCE_IMPACT.md,
CHANGELOG.md and docs/evidence/.

- [x] Target builds, resource check, documentation checks and appropriate suites.
- [x] Record RED/failed/blocked/skipped/not-run separately; review final diff.
- [x] Report implementation with physical verification incomplete; no upload.

## Execution ledger

- Ruling: implement in the existing clean checkout, without creating a worktree,
  committing or asking for another approval. Owner already authorized phase one;
  changes remain reviewable in the selected project.
- Ruling: no PIN rekey feature in phase one; reject credential changes while
  records exist rather than risk destroying access to a PIN-derived vault.
- Baseline target PASS: firmware.bin 843920 B, app0 6553600 B, SHA256
  806e275182a60f0c1b77c010fbe96dcc603c6e9101d49850c02467f62875f15d.
- Current checkout 9513925; prior documentation's simulator toolchain paths no
  longer resolve. Native test toolchain is being established in ignored .testbuild.

- Final evidence: VER-0029 through VER-0038; pending physical procedure HWT-0015.
- Independent review: all five initial source findings fixed and rechecked; final native connection delta and authenticator documentation have no blocking findings.
- Ruling: bounded desktop storage protection and OS entropy were necessary to make the same encrypted-account lifecycle honest on both targets.
- Physical profiling and USB/UI acceptance remain NOT RUN; no upload/commit/push.
