# ADR-0003 — Architecture Audit, Layer Boundary Hardening and Modular Standards

- Date: 2026-11-03 (Asia/Baghdad)
- Status: ACCEPTED for implementation in this change
- Related: FEAT-0001, FEAT-0005, FEAT-0006, FEAT-0013, FEAT-0015, DEV-0027
- Implementation: [architecture](../ARCHITECTURE.md), [PinDefinitions](../../include/PinDefinitions.h), [BoardConfig](../../include/BoardConfig.h), [TouchManager](../../src/hardware/TouchManager.h), [IApp](../../src/core/IApp.h), [MemoryProfiler](../../src/services/MemoryProfiler.h)

## Context

Following the initial modular architecture adoption in [ADR-0002](0002-modular-architecture.md), a comprehensive architectural audit and engineering standards review was conducted across the firmware repository. The audit revealed five specific opportunities to strengthen modular boundaries and cross-platform maintainability:

1. **Hardware-to-Application Inversion in Touch HAL**: `src/hardware/TouchManager.h` included `core/App.h` solely to obtain `TouchEvent` and `TouchGesture`, causing a hardware driver to depend on the top-level application lifecycle class.
2. **Configuration Dual-Maintenance Risk**: `include/PinDefinitions.h` and `include/BoardConfig.h` both maintained definitions of GPIO numbers and timing constants, creating a potential drift risk between documentation regex tooling and hardware drivers.
3. **Diagnostics & Profiler Layer Placement**: Architectural documentation classified `MemoryProfiler` under the Services Layer, while its physical headers were located in `src/core/`.
4. **Host Compilation Warnings**: MSVC 64-bit builds on Windows emitted data conversion warnings (`size_t` to `uint32_t`, `int` to `int16_t`/`uint8_t`) in `CryptoService.cpp`, `TouchManager.cpp`, `UsbHidService.cpp`, and `SettingsApp.cpp`.
5. **Cross-Platform Test Execution**: `tools/run_host_tests.py` failed on Windows when POSIX `c++` was not in `PATH`, despite MSVC `cl.exe` being installed.

## Decision

Adopt the following architectural and engineering standardizations:

1. **Decouple Hardware from App Interface**: In `src/hardware/TouchManager.h`, replace `#include "core/App.h"` with `#include "core/EventTypes.h"`. Hardware drivers now depend only on normalized input types.
2. **Compile-Time Static Alignment of Hardware Configuration**: Centralize hardware pin definitions in `include/PinDefinitions.h` and timing constants in `include/NibruConfig.h`. Maintain `include/BoardConfig.h` with `static_assert` assertions verifying that all `Board::*` constants strictly equal `Pins::*` and `Config::*`, satisfying automated regex documentation checks while guaranteeing compile-time equivalence.
3. **Formalize MemoryProfiler Service Header**: Add `src/services/MemoryProfiler.h` forwarding to the implementation, cleanly classifying profiling under the Services layer while retaining `src/core/MemoryProfiler.h` for backward compatibility.
4. **Clean 64-bit Type Conversions**: Explicitly cast numeric conversions in `CryptoService.cpp`, `TouchManager.cpp`, `UsbHidService.cpp`, and `SettingsApp.cpp`, achieving zero warnings across MSVC and GCC/Clang.
5. **Auto-Detecting Toolchain in Test Runners**: Extend `tools/run_host_tests.py` and `tools/run_desktop_tests.bat` with automatic MSVC BuildTools/Community discovery, enabling full local automated testing on Windows without external prerequisites.

## Alternatives and Reasons

- **Physical Directory Restructuring of `src/core/MemoryProfiler.cpp` to `src/services/`**: Rejected. Moving the physical file would break existing documentation references verified by `tools/check_docs.py` and existing test runner globbing without adding technical value. A service header cleanly fulfills the architectural layering.
- **Removing `include/BoardConfig.h` Entirely**: Rejected. Legacy tests, host stubs, and `tools/check_docs.py` rely on `Board::*` namespace. Statically asserted equivalence provides 100% safety with zero breaking changes.

## Consequences

- Dependency directions flow strictly downward (Apps -> Core/Services/UI; UI -> Theme/Display; Services -> HAL/Crypto; Core -> EventTypes; HAL -> EventTypes/Pins).
- Zero binary size or RAM growth on physical ESP32-S3 firmware (Flash: 831,065 B [12.7% of app0], RAM: 50,056 B [15.3% of 320 KB]).
- All 17 documentation checks, 17 unit tests in `tests/docs`, 4 Python tests, 5 Node WebCrypto tests, and 25 lifecycle iterations in desktop simulator pass cleanly.
