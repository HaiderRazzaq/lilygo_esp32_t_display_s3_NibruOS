# Releases and firmware identity

No release tags or release manifests exist in the discovered two-commit history.
Runtime says **v0.1**; the preserved package changelog calls its patch
**v0.1.1-source-patch**. These are source labels, not evidence of a verified or
tagged release. Do not retroactively manufacture release dates or binary hashes.

New work stays in [Unreleased](../../CHANGELOG.md) until a deliberate release.
Use Semantic Versioning for actual release scope: patch for compatible fixes,
minor for compatible new features, major for incompatible contracts. Before 1.0,
document interface/partition/data compatibility explicitly. Do not increase the
firmware version for every documentation or code edit.

For an authorized release create `vX.Y.Z.md` here with:

- Date/time/timezone, release owner, Git commit/tag, clean/dirty source state.
- Runtime version and reason for version selection (resolve BUG-0003 first).
- Changelog scope: Added/Changed/Fixed/Removed/Deprecated/Security as applicable.
- Exact resolved toolchain, core, libraries and CST816S SHA; pin/test dependency
  choices deliberately (BUG-0002), never silently upgrade.
- Target variant, pinout, modules, partition and USB/PSRAM settings.
- Build command, result, warnings, image hash/size and evidence link.
- Host/documentation results and physical test IDs, including failures/blocked
  gates. Explicitly label unverified distributions; no silent conversion to PASS.
- Known issues, compatibility/migration/backup instructions and rollback approach.

Release/tag/commit/upload actions require the repository workflow or user
authorization. This documentation task creates none of them. Generated `.bin`,
`.elf`, `.map`, `.pio` and temporary test artifacts stay out of source control.
