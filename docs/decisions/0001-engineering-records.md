# ADR-0001 — Markdown engineering records and evidence validation

- Date: 2026-11-01 (Asia/Baghdad)
- Status: ACCEPTED for implementation in this documentation change
- Related: FEAT-0008, DEV-0004
- Implementation: [workflow](../ENGINEERING_WORKFLOW.md),
  [checker](../../tools/check_docs.py), [regression tests](../../tests/docs/test_check_docs.py)

## Context

The owner requested durable engineering memory in the existing repository,
with no firmware refactoring or invented history. Existing narrative docs already
contain valuable hardware evidence and Wi-Fi failures, but current status,
historical evidence and planned features overlap. Only two Git commits exist;
runtime version and legacy patch labels differ.

## Decision

Keep human-readable Markdown, with small fenced JSON records for features,
issues, state, development and tests. Give each document one responsibility.
Keep original reports and changelog; add dated findings and a canonical root
changelog instead of rewriting historical outcomes. Git remains code history.
Use one Python standard-library validator plus mutation tests and CI; do not
generate PASS records automatically. Hardware observations carry provenance and
cannot be inferred from code or software test results.

## Alternatives and reasons

- Prose-only Markdown is simple but cannot reliably check IDs, evidence paths or
  source registration consistency. Keep prose for explanation, structured records
  for the small set of fields that need checking.
- A database, external issue tracker, or docs-site framework would add setup,
  dependencies and another source of truth. The owner explicitly requested low
  overhead, in-repository knowledge across IDEs and assistants.
- The selected format is readable in any editor and validates offline on Windows
  or Linux without installing libraries. This implementation choice was made by
  the implementing agent within the owner's detailed request.

## Consequences

Maintainers update current snapshots and append historical outcomes separately.
Stable IDs survive deprecation/removal. The checker detects missing records,
links and evidence references; it cannot prove report authenticity, measure
hardware behavior or enforce append-only history. Reviewers must inspect Git
diffs. Unpinned firmware dependencies remain a separate existing issue; no tool
or hardware settings are changed by this decision.
