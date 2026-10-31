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
