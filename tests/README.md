# Host-side C++ smoke tests

Run `bash tests/host/run_tests.sh` from the project directory (or directly from this project as used in the script).

These tests also separately compile the actual `src/main.cpp` entry unit, and compile the app/manager/service sources against tiny local stand-ins for Arduino, TFT_eSPI, CST816S and WiFi. They assert:

- Correct landscape touch mapping and gesture type.
- Physical button short/long/back behaviour.
- App transition Home→Touch Test→Home and Home→Wi-Fi→Home.
- Asynchronous scan start/completion/result transfer.
- Resource owner release and rejection of unrelated release.
- Nonblocking timeout, scan failure recovery, Wi-Fi mode setup failures, and ability to navigate back from scans.

**These are NOT hardware-integration or PlatformIO Arduino-target tests.** Build with PlatformIO and run the manual device acceptance list in `docs/BUILD_AND_TEST.md`.

## Documentation validation

Run `python tools/check_docs.py` for link/record/source consistency and
`python -m unittest discover -s tests/docs -v` for checker mutation tests.
Only Python 3.10+ standard library is needed. Host script runs the checker first;
set `PYTHON=python3` if that is your interpreter name. CI runs both the docs tests
and host suite. Current evidence/coverage and hardware procedures:
[TEST_MATRIX](../docs/TEST_MATRIX.md), [workflow](../docs/ENGINEERING_WORKFLOW.md).
Neither docs check nor CI creates test PASS records or changes firmware versions.

## Essential Settings regression workflow

POSIX: `bash tests/host/run_tests.sh` with Python and C++17 compiler available.
Windows portable runner: `python tools/run_host_tests.py`; CXX selects a compiler,
for example `$env:CXX = '.testbuild/toolchain/ziglang/zig.exe c++'` for the local
ignored Zig 0.16.0 installed during this change. Run the same suite with
`-DNIBRU_PROFILE=1 -DNIBRU_BACKLIGHT_LEDC=1` to compile/test alternate code paths.
The toolchain is not a firmware dependency and is not committed.

Run `python -m unittest discover -s tests/resources -v` for artifact tests and
`python -m unittest discover -s tests/docs -v` for documentation mutation tests.
Target builds exercise the PlatformIO post-image capacity checker; rerun
`python tools/check_resources.py` explicitly for an existing image. None of these
commands measures physical heap, timing, brightness, radio reception or power.

Host Settings tests include persistent reopen, validation, legacy import through
empty destination, failed NVS load/write, scoped reset preservation, future schema,
physical-button routes, raw early/held touch confirmation suppression, confirmed
reboot cleanup, unsigned timeout rollover, wake gate and 20 lifecycle cycles.

Touch/flicker repair tests additionally replay held/repeated SINGLE_CLICK frames,
Down/NONE followed by a swipe or release gesture, duplicate/orphan Up and recovery
from a missed Up. Mock graphics counters ensure ordinary same-page updates avoid
full-screen clears and unchanged selections draw nothing. Lock-overlay restore
and a real driver-to-Settings deferred contact-start confirmation path are covered.
These are behavioral mocks, not proof of physical CST816S or panel timing.
