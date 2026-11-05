# Verification matrix

Current snapshot: 2026-11-01 Essential Settings completion. BUILD and HOST are
software checks; all physical acceptance for this changed image remains pending.
The older table below is retained as a historical baseline, superseded by these
current rows (not new evidence of the old hardware claims).

| Current scope | Build | Host/tooling | Physical |
|---|---|---|---|
| Default firmware physical upload | VER-0024 | Image/capacity match | HWT-0013 PASS: COM5 repaired image write/hash/reset only |
| Settings persistence/migration/failure/reset | VER-0020 | VER-0017 | HWT-0008 NOT RUN |
| Scroll/buttons/focus/confirm/wake gating | VER-0020 | VER-0017, including held/raw double-tap cases | HWT-0008 NOT RUN |
| 20 Settings entry/exit cycles | VER-0020 | VER-0017 control flow/ownership only | HWT-0008, heap/PSRAM/latency NOT MEASURED |
| System/memory/Wi-Fi diagnostics | VER-0020 | Mock rendering only, not sensor validation | HWT-0008 NOT RUN; BUG-0001 remains |
| Default AW9364 / opt-in LEDC fallback | VER-0020 | VER-0017 duties/fallback/sleep assertions | HWT-0008 NOT RUN; LEDC not accepted on AW9364 |
| Profiling build switch / checkpoints | VER-0020 | VER-0017 profile-enabled compile/run | A–E pending, no free-heap/stack/power PASS |
| Partition capacity/growth warnings | VER-0020 | VER-0018 (six resource tests) | Static-only, N/A hardware |
| Documentation consistency | N/A | VER-0018 (17 mutation tests + checker) | N/A |

## Modular Architecture & HAL — current

| Scope | Software | Physical |
|---|---|---|
| EventTypes & IApp interface decoupling | VER-0027 HOST PASS, VER-0028 BUILD PASS | NOT APPLICABLE (Software architecture) |
| UI Layer & HomeScreen drawing extraction | VER-0027 HOST PASS, VER-0028 BUILD PASS | HWT-0012 pending |
| Centralized PinDefinitions & NibruConfig | VER-0027 HOST PASS, VER-0028 BUILD PASS | N/A (Compile-time layout) |
| WiFiService MAC encapsulation & boundary | VER-0027 HOST PASS, VER-0028 BUILD PASS | HWT-0012 pending |
| Multi-target build stability (default, profile, ledc) | VER-0028 BUILD PASS | Upload pending owner request |
| Hardware Decoupling (TouchManager to EventTypes) | VER-0052 HOST PASS, VER-0050 BUILD PASS | NOT APPLICABLE (Software architecture) |
| Compile-Time Pin Single Source of Truth (static_assert) | VER-0051 DOCS PASS, VER-0050 BUILD PASS | N/A (Compile-time layout) |
| MemoryProfiler Services Layer Forwarding Header | VER-0052 HOST PASS, VER-0050 BUILD PASS | N/A (Service architecture) |
| MSVC 64-bit Conversion Warnings Elimination | VER-0052 HOST PASS (0 warnings) | N/A (Host toolchain) |

## Touch and redraw repair — current

| Scope | Software | Physical |
|---|---|---|
| Duplicate click/hold suppression; swipes/release/new-Down recovery | VER-0023 HOST PASS, VER-0024 BUILD PASS | Original owner FAIL HWT-0010; repair HWT-0012 NOT RUN |
| Incremental/no-op paint; lock-overlay restore | Mock drawing counts and restore regression PASS VER-0023 | Actual flicker/latency HWT-0012 NOT RUN |
| Contact-start confirmation and raw wake | Driver-to-app regressions PASS VER-0023 | HWT-0012 NOT RUN |
| Resource costs | +1040 B image/+40 B static RAM vsHWT-0009 | Heap/PSRAM/stack/power NOT MEASURED |

## Desktop simulator — current

| Scope | Software | Physical |
|---|---|---|
| Desktop Simulator Target (FEAT-0015) | VER-0025 HOST PASS, VER-0026 BUILD PASS | NOT APPLICABLE (Host Target) |
| SDL2 320x170 display & typography | Verified in nibru_tests and simulator window | NOT APPLICABLE |
| DesktopInput mouse/keyboard mapping | Verified in nibru_tests (Tap/Swipes/Keys) | NOT APPLICABLE |
| Deterministic Wi-Fi & fault injection | Verified in nibru_tests (Normal/Timeout/Error) | NOT APPLICABLE |
| Desktop persistent NVS storage | Verified in nibru_tests across reloads | NOT APPLICABLE |

## Historical baseline table

| Capability / feature | Build verification | Unit / simulated verification | Hardware verification | Pending / blocked procedure |
|---|---|---|---|---|
| Firmware compilation | PASS, VER-0001 | Not equivalent | NOT RUN upload | Build warnings recorded; upload separately |
| Boot, flash/PSRAM reporting | PASS compiled, VER-0001 | main compile + mock values historically PASS | NOT RUN current image | HWT-0003, compare actual 16 MB / 8 MB reports |
| Display init / rotation / power | PASS compiled, VER-0001 | Drawing stubs do not validate bus/init | Owner reference sketch PASS HWT-0001; current NOT RUN | HWT-0003; inspect 320x170 orientation |
| CST816S init | PASS compiled, VER-0001 | Stub begin is a no-op; not init verification | Owner reference sketch PASS HWT-0001; current NOT RUN | HWT-0003, reset/IRQ and repeated taps |
| Touch coordinate mapping | PASS compiled, VER-0001 | Known-point assertion historically PASS; edge clamping lacks dedicated assertions | Reference mapping reported; current NOT RUN | HWT-0003, corners/bounds |
| Four landscape swipe directions | PASS compiled, VER-0001 | Rotation enum assertions historically PASS | NOT RUN current image | HWT-0003, verify visible directions |
| BOOT/user buttons | PASS compiled, VER-0001 | Short/long/Back assertions historically PASS | NOT RUN current image | HWT-0003, release/debounce/hold |
| Home/app navigation / 4th-app pagination | PASS compiled, VER-0001 | Navigation and dummy 4th app historically PASS | NOT RUN current image | HWT-0003; 12-slot capacity lacks exhaustive test |
| App resource cleanup / Home escape | PASS compiled, VER-0001 | Wi-Fi stop/off/lease release + header Back historically PASS | NOT RUN current image | HWT-0004/0005; actual latency/radio off pending |
| Wi-Fi scan/warmup/retry/deadline | PASS compiled, VER-0001 | Result copy, timeout/retry/failure/delayed-completion historically PASS | Earlier FAIL HWT-0002; patch NOT RUN | BUG-0001; HWT-0004 |
| Wi-Fi result cap / scrolling | PASS compiled, VER-0001 | Source cap12; no dedicated >12 or UI scroll assertions | NOT RUN | HWT-0004 with enough APs; UI handling limits |
| Memory stability / power modes | Static size only VER-0001 | NOT RUN soak/leak test; mocks insufficient | NOT RUN | HWT-0005, record heap/PSRAM trends and resets |
| Standalone Wi-Fi diagnostic | NOT RUN separate target build | NOT RUN | NOT RUN | Run only if needed per WIFI_DIAGNOSTICS; replaces firmware |
| MicroSD init/read/write | No implementation | No tests | BLOCKED HWT-0006 | FEAT-0007, BUG-0004; wiring proposed/unverified |
| Battery ADC / BLE / other peripherals | No app/service implementation | No tests | NOT RUN | No extra connected modules confirmed; inventory is authority |
| Documentation system FEAT-0008 | Not firmware | DOCUMENTATION tests/checker PASS, VER-0005 (16 tests) | Not applicable | Hosted CI execution pending; tool checks cannot verify truth of reports |

Use [BUILD_AND_TEST](BUILD_AND_TEST.md) for commands and physical acceptance;
[HARDWARE_TEST_LOG](HARDWARE_TEST_LOG.md) stores the pending reproducible
procedures. Append a new result record after a real test. Keep older failures.
Future apps/peripherals require a registry entry and matrix row; do not mark
their verification as passing merely because they share an existing driver.

## Authenticator phase-one coverage — 2026-11-01

| Capability | Software evidence | Physical |
|---|---|---|
| Entry/commit/reopen/cancel and >=20 cycles | VER-0033 HOST; VER-0034 SIMULATOR | HWT-0015 NOT RUN |
| RFC6238 SHA1 and Base32 bounds | VER-0033 real Windows CNG vectors | ESP32 NOT RUN |
| Legacy U8 migration, wrong key/tag, failed writes/PIN guard | VER-0033; OS disk faults VER-0034 | HWT-0015 NOT RUN |
| Time loss, USB completion/busy/cancel/disconnect | VER-0033 transport double; VER-0035 compile | Enumeration/delivery/layout NOT RUN |
| Four image capacities | VER-0035/VER-0036 PASS | Heap/stack/latency/power NOT MEASURED |

Default authenticator image uploaded 2026-11-02: HWT-0016 PASS for write/hash/reset
only; HWT-0015 runtime acceptance remains NOT RUN. Artifact preflight VER-0040 PASS.

## Easier account entry — 2026-11-02

| Capability | Software evidence | Physical evidence |
|---|---|---|
| QWERTY/digit/shift/symbol layout; shared Wi-Fi keyboard | VER-0042 HOST, VER-0043 SIMULATOR | Owner manual-entry difficulty HWT-0017; new layout HWT-0018 NOT RUN |
| Paired import, crypto bounds, timeout before SAVE, storage retry, sleep/20 cycles | VER-0042 HOST; VER-0043 SIMULATOR injected transport | Native USB/UDP HWT-0018 NOT RUN |
| Local helper guards, URI validation, WebCrypto/CNG interoperability, USB retry/clear | VER-0044 HOST/browser render | Real browser serial/LAN delivery HWT-0018 NOT RUN |
| Four ESP32 target builds/capacity | VER-0045 BUILD, VER-0046 STATIC | No runtime inference |
| Startup/entry/exit/20 cycles/combined workload resources | Host cycles only; resource report | HWT-0018 NOT RUN; heap/PSRAM/stack/latency/power NOT MEASURED |

RED and intermediate setup failures retained in VER-0041. Independent final
review VER-0048 found no remaining blocker; no device result follows from review.

Current easier-entry default upload HWT-0019 PASS for write/hash/reset only; matched partition/image preflight VER-0049. New native behavior HWT-0018 remains NOT RUN.

## Architecture Audit & Screen Orientation Control — 2026-11-03

| Capability | Software evidence | Physical evidence |
|---|---|---|
| Modular decoupling, static assertions, zero compiler warnings | VER-0050 BUILD, VER-0051 DOCS, VER-0052 HOST | Hardware baseline preserved |
| Screen orientation control: Landscape Normal 0° & Inverted 180° | VER-0053 BUILD, VER-0054 HOST/SIMULATOR, VER-0055 DOCS | Physical device rotation test pending |
| CST816S touch coordinate inversion & inverted swipe gesture handling | VER-0054 HOST (x=119, y=59 from rawX=59, rawY=200; SWIPE_UP -> SwipeRight) | Inverted cable physical touch test pending |
| Settings NVS persistence ('rot' key with 5-byte schema preservation) | VER-0054 HOST / Desktop NVS | Flash persistence verified |
| Resource budget & partition capacity | VER-0053 PASS (832,464 B binary, 12.7% flash, 15.3% RAM) | Heap/stack/latency/power pending measurement |

## BLE startup interruption — 2026-11-04

| Capability | Software evidence | Physical evidence |
|---|---|---|
| Controller/host failure, lease release, retry | VER-0145 RED, VER-0146 GREEN (fault injection) | HWT-0044 FAIL on prior image; HWT-0045 pending |
| Settings BLE control and busy radio | VER-0149 simulator PASS | HWT-0045 pending |
| Image size and runtime resources | VER-0150 BUILD, VER-0151 capacity PASS | Heap, PSRAM, stacks, latency, power and A–E NOT MEASURED |

| Wi-Fi/BLE coexistence | VER-0153 RED, VER-0155 HOST, VER-0156 simulator, VER-0158 BUILD, VER-0159 capacity | HWT-0048 v0.10.6 panic, HWT-0051 v0.10.7 activation PASS, HWT-0049 extended acceptance |
| Windows BLE link and Generator typing | VER-0161 HOST, VER-0162 simulator, VER-0163 diagnostic build | HWT-0052 typing failed; HWT-0055 captures termination and authentication failure |
| Bluetooth Home card, Settings access and keyboard readiness | VER-0166/VER-0167 RED; VER-0168/VER-0169 GREEN; VER-0171 desktop launch; VER-0172 ESP32 build | HWT-0056 pending |
| Windows HID enumeration and authenticated report sending | VER-0175 RED; VER-0177/VER-0178 GREEN; VER-0179 BUILD | HWT-0058 reproduction; HWT-0059 pending |
| Display-assisted BLE authentication | VER-0182 RED; VER-0183/VER-0184 GREEN; VER-0185 BUILD | HWT-0065 Windows authentication without subscription; HWT-0066 phone rejected pairing before code entry |
| BLE PnP ID serialization (v0.12.1) | VER-0188 HOST, VER-0189 SIMULATOR, VER-0190 BUILD | NOT RUN. Check Windows PnP fields and HID subscription, then type test text. |

| Beacon Lab selector, radio exclusion, timeout and cleanup | VER-0192 HOST/SIMULATOR, VER-0193 BUILD | NOT RUN. Capture synthetic beacons and confirm stop and radio recovery. |

| Beacon Lab names, open/secured counts and duration | VER-0195 HOST/SIMULATOR, VER-0196 BUILD | NOT RUN. Verify unique names, WPA2 flags, and transmission stop on a second receiver. |

| Beacon Lab batch scheduling and accepted-name count | VER-0197 HOST/SIMULATOR, VER-0198 BUILD | NOT RUN. Compare on-device accepted count with a capture on the transmit channel. |

| Wi-Fi Channels aggregation, manual refresh, busy radio, Back and cancellation | VER-0205 RED, VER-0206 compile/link attempt FAIL, VER-0207 HOST/SIMULATOR, VER-0208 ESP32 BUILD | HWT-0075 NOT RUN; includes physical A–E and 20-cycle resource checks |
| Beacon Lab receiver visibility | Existing host/build evidence | HWT-0074 owner reports working; counts and runtime metrics were not captured |
| BLE Lab identities, GAP events, timeout and cleanup | VER-0211 RED, VER-0212 initial compile FAIL, VER-0213 HOST, VER-0214 ESP32 BUILD | HWT-0076 NOT RUN |
| BLE Lab desktop registration and simulation | VER-0216 SKIPPED by owner | Resume after explicit owner instruction |
| Bruce pairing payload layouts, profiles and mixed sessions | VER-0218 RED, VER-0219 HOST, VER-0220 BUILD | HWT-0078 notification and resource acceptance NOT RUN |
| Pairing profile desktop integration | VER-0222 SKIPPED by owner | Simulator remains paused |
| Matched Windows Swift Pair reception and notification | Existing payload/host build evidence | HWT-0080 receiver capture and owner-confirmed notification PASS |
| Target-family picker, contact suppression, Back/BOOT and Random | VER-0224 RED, VER-0225 HOST, VER-0226 BUILD | HWT-0081 NOT RUN |
| v0.19.0 simulator integration | VER-0228 SKIPPED by owner | Paused |
| Apple Action, Apple Not Your Device, Beacon payloads and page-two picker | VER-0230 HOST, VER-0231 BUILD, VER-0232 DOCS | HWT-0085 upload/hash PASS; HWT-0086 runtime boot BLOCKED until a physical reset; HWT-0084 receiver alerts, twenty cycles and resource scenarios A–E NOT RUN. |
| v0.20.0 simulator integration | VER-0233 SKIPPED by owner | BLE Lab remains absent from the paused simulator |
