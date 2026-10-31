# Engineering documentation

Start with [AGENTS](../AGENTS.md), [project context](PROJECT_CONTEXT.md), and
[current state](CURRENT_STATE.md). Inspect the real source before changing it.

| Document | Responsibility |
|---|---|
| [CURRENT_STATE](CURRENT_STATE.md) | Dated current snapshot, version ambiguity, baseline commit, next work |
| [PROJECT_CONTEXT](PROJECT_CONTEXT.md) | Purpose, code map, boot flow, constraints |
| [ARCHITECTURE](ARCHITECTURE.md) | Implemented components, ownership, lifecycle, memory and errors |
| [FEATURE_REGISTRY](FEATURE_REGISTRY.md) | Stable feature identity and separate implementation/verification states |
| [KNOWN_ISSUES](KNOWN_ISSUES.md) | Current issue status, evidence, attempts, unresolved risks |
| [HARDWARE_PINOUT](HARDWARE_PINOUT.md) | Authoritative reservations, electrical constraints, proposed wiring |
| [HARDWARE_INVENTORY](HARDWARE_INVENTORY.md) | Integrated, connected, owned, and proposed hardware |
| [DEPENDENCIES](DEPENDENCIES.md) | Configured and locally resolved tool/library versions |
| [TEST_MATRIX](TEST_MATRIX.md) | Capability coverage and pending verification |
| [HARDWARE_TEST_LOG](HARDWARE_TEST_LOG.md) | Append-only physical-device observations and pending procedures |
| [VERIFICATION_REPORT](VERIFICATION_REPORT.md) | Preserved old report, plus append-only typed software verification |
| [DEVELOPMENT_LOG](DEVELOPMENT_LOG.md) | Append-only meaningful engineering events and evidence imports |
| [CHANGELOG](../CHANGELOG.md) | Canonical user-facing release changes and Unreleased work |
| [Legacy changelog](CHANGELOG.md) | Preserved original package history; new entries go in root CHANGELOG |
| [ENGINEERING_WORKFLOW](ENGINEERING_WORKFLOW.md) | Update rules, ID conventions, templates, checks, Definition of Done |
| [decisions](decisions/README.md) | Major technical decisions; retrospective status made explicit |
| [releases](releases/README.md) | Release/version policy and release evidence checklist |
| [BUILD_AND_TEST](BUILD_AND_TEST.md) | Build/upload commands and reproducible physical acceptance steps |
| [WIFI_DIAGNOSTICS](WIFI_DIAGNOSTICS.md) | Detailed timeout incident and isolation procedures |
| [ADDING_AN_APP](ADDING_AN_APP.md) | App integration recipe |
| [AI_IDE_QUICKSTART](AI_IDE_QUICKSTART.md) | IDE handoff prompts |

Evidence files live under [evidence](evidence/README.md). Records complement Git;
they cannot reconstruct changes before the first available commit.
