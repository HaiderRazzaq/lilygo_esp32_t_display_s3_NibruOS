# Engineering Memory Implementation Plan

> Execute inline in the existing repository, as explicitly requested. No commits,
> firmware edits, dependency upgrades, uploads, or new worktree are authorized by
> this documentation task. Apply the executing-plans and verification skills.

**Goal:** Preserve evidence-based current state, engineering history, and tests
across maintainers and AI sessions.

**Architecture:** Markdown documents with separate responsibilities and small
JSON records for validation. A Python standard-library checker validates records,
links, source registration, and evidence references; Git remains code history.

**Tech stack:** Markdown, Python 3.10+, existing Bash/C++ tests and PlatformIO.

**Spec:** The owner's engineering-memory request received 2026-11-01; its design
is captured in [ADR-0001](../../decisions/0001-engineering-records.md).

## Constraints and review focus

- Preserve existing reports verbatim, adding clearly dated context where needed.
- Preserve all firmware, board settings, GPIO definitions, and dependency pins.
- Separate implementation from build, simulated, and physical verification.
- Import only supported history; unknown dates/outcomes remain UNKNOWN.
- Proposed MicroSD GPIO10/11/12/13 wiring is never installed-hardware evidence.
- Relative links must work from nested documents and on Windows/Linux.
- Hardware PASS requires an observation and a supporting evidence reference.
- Append-only records are a maintainer rule; validation cannot prove honesty.

## Tasks

- [x] Inspect all tracked source, docs, tests, configurations, and both commits;
      distinguish local build artifacts from reproducible verification.
- [x] Write checker regression tests first; demonstrate failure before the checker
      exists, then implement it and check real corruption cases.
- [x] Add current-state, registries, hardware inventory, test matrix, append-only
      logs, dependency baseline, ADR/release conventions, and documentation index.
- [x] Integrate the workflow/Definition of Done in AGENTS, onboarding, app guide,
      build guide, tests, and CI; correct current prose without hiding old reports.
- [x] Run checker and its regression suite, existing host suite when possible,
      and real PlatformIO build. Save actual evidence, failures, and limitations.
- [x] Review changes against the request, verify firmware/configuration unchanged,
      and provide navigation, results, pending hardware procedures, and limitations.

No automatic version bump: runtime says v0.1; legacy docs call the Wi-Fi patch
v0.1.1-source-patch. No release tag or centralized firmware version exists.
