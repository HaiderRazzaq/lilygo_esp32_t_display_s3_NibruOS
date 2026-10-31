# Changelog

User-facing changes follow Keep a Changelog categories and Semantic Versioning
when a release is actually made. Do not increment versions for every edit.
Release evidence and tagging policy: [releases](docs/releases/README.md).

## Unreleased

### Added
- Engineering memory: current-state snapshot, feature/issue registries,
  development and physical-test logs, test matrix, inventory, dependency
  baseline, ADR/release guidance, and lightweight documentation validation.
- Documentation regression checks and CI integration with the existing host suite.

### Changed
- Agent/IDE development workflow now requires evidence-based change tracking
  and a Definition of Done. Updated current guidance to match Wi-Fi timing.

### Fixed
- Documentation ambiguity about build verification and runtime versus source
  patch version. Fresh target-build evidence is separate from historical reports.

No firmware behavior, GPIO allocation, dependency pin, partition, or version
string changed. Removed, Deprecated, and Security have no entries this cycle;
add those headings when a real change needs them.

## Historical source baseline

The original [package changelog](docs/CHANGELOG.md) is preserved, including its
2026-10-26 v0.1.1-source-patch label and undated v0.1 section. Both are already
present in the first available commit `d6cf0294f548b64884076e8732e2bf425ee8e773`.
Their development order and release dates cannot be reconstructed from Git.
No release tags exist at this baseline; runtime still displays v0.1.
