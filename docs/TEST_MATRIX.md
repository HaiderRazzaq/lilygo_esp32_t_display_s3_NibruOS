# Verification matrix

Current as of 2026-11-01. **BUILD** verifies compilation/linking only; **HOST**
uses stubs; **HARDWARE** requires device observations of the actual image.
Historical host PASS claims are preserved in [VERIFICATION_REPORT](VERIFICATION_REPORT.md).
This session's host attempt is BLOCKED (VER-0002), not a fresh PASS.

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
