# Architecture decision records

Create an ADR for major architectural, ownership, hardware bus/pin, persistence,
framework or dependency-policy decisions. Routine documentation fixes need no ADR.

| ID | Status | Date | Decision |
|---|---|---|---|
| ADR-0001 | ACCEPTED for implementation in this change | 2026-11-01 | [Markdown engineering records with lightweight validation](0001-engineering-records.md) |
| ADR-0002 | ACCEPTED for implementation in this architectural change | 2026-11-01 | [Feature-Based Modular Monolith with Layered Responsibilities and HAL](0002-modular-architecture.md) |
| ADR-0003 | ACCEPTED for implementation in this change | 2026-11-03 | [Architecture Audit, Layer Boundary Hardening and Modular Standards](0003-architecture-audit-modular-standards.md) |

Use the next unused `NNNN-short-title.md`. Include ID/title, date, status
(PROPOSED/ACCEPTED/SUPERSEDED/REJECTED), context, decision, alternatives, reasons,
consequences, and related source/features. Accepted means the documented decision
is in use; it does not imply the owner separately approved a design discussion.
When superseded retain the original decision and link its successor.

Existing Arduino/TFT_eSPI/single-owner design rationale is preserved in
[ARCHITECTURE](../ARCHITECTURE.md). No retroactive decision dates or discussions
are invented. Any future retrospective ADR must say it was reconstructed from
source/docs and identify missing historical evidence.
