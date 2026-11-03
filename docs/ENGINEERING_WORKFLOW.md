# Engineering memory workflow

Use this with [AGENTS](../AGENTS.md). The document map is [here](README.md).
One change needs one meaningful development record, not a log of every edit.

## Before and during a change

1. Read context, current state, relevant feature/issue entries, architecture,
   pinout, dependencies, and affected code/tests. Use source over stale prose;
   preserve the owner's verified wiring over contradictory image captions.
2. Reuse an existing feature/issue ID or allocate the next unused four-digit ID.
   Inspect the whole repository before allocating; never reuse removed IDs.
3. State a short implementation and verification plan. Check GPIO conflicts,
   voltage, current, memory, peripheral ownership, Back navigation and cleanup.
4. Keep changes focused. Record major decisions as ADRs, meaningful failures and
   reverts in the development log, and diagnostics without secrets. Avoid
   changing pins, library versions, partitions, or framework by accident.
5. Apply the permanent [resource policy](RESOURCE_BUDGET.md) to every change with
   runtime/storage/timing/peripheral/power impact. Read the current
   [baseline](performance/BASELINE.md), capture what is available, and include a
   Resource Impact Report. Follow [scenarios A–E](MEMORY_PROFILING.md), with at
   least 20 lifecycle cycles. Missing device measurements stay NOT MEASURED.

## After a change / Definition of Done

1. Implement the requested scope and inspect the final diff.
2. Run appropriate checks: documentation checker, its tests when tooling changes,
   host tests when behavior changes, and target build when tools are available.
   Distinguish test failures from unavailable tools. Record every meaningful
   failed, successful, skipped, blocked, and not-run verification separately.
3. Update the feature registry and relevant issues; append a development entry.
   Update current state and user-facing Unreleased changes when applicable.
   Sequentially increment the firmware runtime version across configuration, code, and documentation
   per the Mandatory Automatic Sequential Versioning Policy.
   Update pinout/inventory, architecture, dependencies, or ADRs only if affected.
4. Append physical results only with real device evidence. Otherwise leave
   hardware NOT RUN/BLOCKED and provide reproducible procedures. A build, mock,
   review, or an older independent sketch never validates the current device image.
5. Run `python tools/check_docs.py`. Review that IDs, links, records, evidence,
   and documentation match the source. Record remaining limitations and follow-up.
6. Run `python tools/check_resources.py` after a target build; inspect the
   generated report and provisional growth warnings. Never accept runtime heap,
   stack, latency or power from a static artifact check. Document-only changes
   need a brief impact assessment, not an unnecessary firmware build.

A task is complete only after implementation, appropriate verification, and
required documentation are addressed. Where tests cannot run, report
**IMPLEMENTED, verification incomplete**; never turn that into a test PASS or
VERIFIED RESOLVED issue. Firmware runtime version must be sequentially incremented on every change per AGENTS.md; no Git commit without authorization.

## Record conventions

| ID | Authoritative definition |
|---|---|
| FEAT-0001 onward | FEATURE_REGISTRY, one identity even when deprecated/removed |
| BUG-0001 onward | KNOWN_ISSUES, current status with links to attempts |
| DEV-0001 onward | DEVELOPMENT_LOG, append-only engineering event |
| HWT-0001 onward | HARDWARE_TEST_LOG, physical test or explicitly pending procedure |
| VER-0001 onward | VERIFICATION_REPORT, software/build/static verification |
| ADR-0001 onward | decisions index and numbered ADR |

Records use Markdown fenced `json` objects (or arrays) with `record_type`.
This allows validation using only Python's standard library. Human explanation,
tables and links stay ordinary Markdown. Paths in structured `files`/`evidence`
arrays are repository-relative; use Markdown links for human navigation.
Development-record `files` are immutable historical references and may later
cease to exist after removal/rename; Git and the recorded commit preserve them.
Current feature/issue paths and all evidence-file references must still exist.
Unknown values are `UNKNOWN`, never invented dates or hashes. Use ISO 8601 with
`+03:00` for Baghdad observations, or preserve the original recorded timezone.

Feature states: PLANNED, IN DEVELOPMENT, IMPLEMENTED, DEPRECATED, REMOVED.
Verification is independent and references typed test records or NOT VERIFIED.
For removal/deprecation retain the entry, set date/reason, append DEV history,
and add the corresponding changelog category; in mutable feature/issue records,
move obsolete paths to `historical_files`, while current `files` must exist.
Do not rewrite older development records when a source file is removed.

Issue statuses: OPEN, INVESTIGATING, BLOCKED, FIXED — UNVERIFIED,
VERIFIED RESOLVED, WONTFIX. A verified-resolution entry must cite passing tests
of the original failure at the appropriate layer; hardware failures require
hardware evidence. Keep failed approaches and old test records after resolution.

Test types: BUILD, HOST, STATIC, DOCUMENTATION, HARDWARE. Outcomes: PASS, FAIL,
BLOCKED, NOT RUN, SKIPPED. NOT RUN = no attempt; BLOCKED = missing prerequisite;
SKIPPED = deliberate omission with reason; FAIL = attempted behavior failed.
Evidence has a provenance: captured command output, owner report, or historical
document. Import date and actual event date are different fields.

## Copyable record shapes

Use these field lists to create real records; do not copy example PASS outcomes.

```text
development: record_type, id, date, related, action, files, description, reason,
             expected, actual, verification, risks, commit, follow_up
feature: record_type, id, name, purpose, module, files, dependencies, hardware,
         gpio, lifecycle, implementation, verification, issues, history
issue: record_type, id, title, status, description, symptoms, reproduction,
       affected_versions, suspected_cause, root_cause, files, attempts, fix,
       verification, risks
test: record_type, id, date, firmware_version, commit, verification_type,
      scope, hardware, wiring, procedure, expected, actual, result,
      evidence, evidence_kind, environment, limitations
```

Append logs in record-creation order, using the next ID. An imported historical
event retains its actual date or UNKNOWN and records `recorded_at`. Never erase
or rewrite old entries to hide failures. Corrections are a new dated entry that
references the original ID and explains the factual correction. Snapshot docs
and issue statuses may change; history and prior observations stay intact.

## Checks and their limits

```powershell
python tools/check_docs.py
python -m unittest discover -s tests/docs -v
```

The checker validates required documents, relative Markdown links/anchors,
reference-style destinations, ID definitions/references, required record fields,
test types/results, evidence paths for hardware PASS, app registration and a
small source-constant snapshot. It never tests electricity, trusts neither a
compiler as hardware evidence nor a legacy sketch as current firmware proof,
and cannot establish that an attached report is truthful. It does not enforce
append-only history against Git; reviewers must inspect log diffs.

[CI](../.github/workflows/validate.yml) runs documentation checks and host tests
on pushes and pull requests. Target builds and manual hardware testing remain
separate gates; this workflow has not itself run on a hosted runner yet.
