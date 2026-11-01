# RESOURCE IMPACT REPORT — touch repetition and Settings flicker

Change ID: BUG-0008 / FEAT-0013 / DEV-0017
Feature: Single-action touch contacts and incremental Settings drawing
Firmware Version: v0.3 (unchanged)
Git Commit: 8239bb6 + uncommitted Essential Settings and focused repair
Build Environment: Windows; lilygo-t-display-s3; espressif32@6.5.0;
Arduino-ESP32 2.0.14, Xtensa GCC8.4.0 -Os, unchanged libraries/partitions/pins.

## Diagnosis and plan

Owner reported actual repeated touch actions several times per second and
flicker after HWT-0009. Source allowed repeated SINGLE_CLICK per contact after
180ms, and synthesized a tap on NONE/down before the final swipe/click. Settings
called screen.clear on control updates and even unchanged selections.
Mock failures reproduced the input and drawing paths; raw device traces were
not captured. Plan: one classified action per contact, release fallback for a
plain tap, preserve swipes/wake/Back, cache small drawing state, update only changed
controls, cover overlay restoration and contact-start confirmation safety,
build/check resources, upload default image and request physical recheck.

## FLASH

Before: image 842848 B, linked Flash842481 B (HWT-0009 image).
After: image 843888 B, linked Flash843521 B.
Delta: image +1040 B (+0.1234%).
Application Partition Capacity: 6553600 B from generated app0.
Remaining Headroom: 5709712 B. No 5% growth warning.

| Environment | Image B | Linked Flash B | Static RAM B | app0 remainder B |
|---|---:|---:|---:|---:|
| lilygo-t-display-s3 | 843888 | 843521 | 61240 | 5709712 |
| lilygo-t-display-s3-profile | 844448 | 844077 | 61240 | 5709152 |
| lilygo-t-display-s3-ledc | 849664 | 849293 | 61312 | 5703936 |

Default image SHA-256: `2d68c7dd233c5404626eee4d8a6aa4613b9ad50323e321e3a7b0a3de6dcacdd1`.

## INTERNAL RAM

Free Before: NOT MEASURED.
Free After: NOT MEASURED.
Minimum Free: NOT MEASURED.
Largest Free Block: NOT MEASURED.
Observed Delta: runtime NOT MEASURED; static data/BSS delta
+40 B vs61200 B before. Small fixed contact/drawing state is added;
unused linker RAM is not free internal heap and does not prove >=150KiB headroom.

## PSRAM

Free Before: NOT MEASURED.
Free After: NOT MEASURED.
Minimum Observed Free: NOT MEASURED.
Observed Delta: NOT MEASURED. No framebuffer, sprite buffer or PSRAM allocation.

## RUNTIME

Application Startup: NOT MEASURED physically.
Maximum Observed Loop/Update Time: NOT MEASURED physically.
UI Responsiveness: HOST verifies no full-screen clear on same-page control
changes; selecting the same focused value performs no draw calls. It does not
prove physical flicker elimination or latency. NONE/down is withheld until a
classified gesture or release; wake still sees raw contact immediately.
Resource Cleanup: unchanged app ownership; no new lease/task/callback/file.
Repeated Entry/Exit Test: existing20-cycle HOST assertions PASS; physical
scenarioD and A–E remain pending. No new device runtime baseline was invented.

## OTHER

Stack Impact: bounded49-byte metric text helper and small local formatting;
actual stack HWM NOT MEASURED. No task stack allocation or extra task.
Peripheral Resource Impact: same TFT/CST816S drivers; touch initialization,
IRQ/reset/pins and rotation transform untouched. New Down phase recovers a
coalesced/missed Up; repeated Down/Contact reports do not auto-repeat actions.
Storage Impact: no schema/NVS/SD change; no-op settings keep existing write suppression.
GPIO Impact: none; AW9364 GPIO38 default preserved, GPIO15 supply unchanged.
Power Impact: no new PWM/power state changes; actual current NOT MEASURED.

## VERIFICATION

Static Analysis: focused independent code review; fixed surface restoration,
raw-contact confirmation timing and missed-Up recovery with failing regressions.
No dedicated static-analyzer run.
Unit Tests: C++ firmware host mocks (default and profile+LEDC variants); graphics
counters measure drawing calls, not real panel refresh.
PlatformIO Build: all3 target environments PASS (VER-0024).
Physical Hardware Test: original owner observation is FAIL HWT-0010; repaired
runtime behavior still needs owner recheck. Upload status is recorded separately.

## RISK ASSESSMENT

Regression Findings: RED input/drawing/lock-restoration/contact-time/missed-Up
assertions are preserved in [evidence](../evidence/2026-11-01-touch-red-tests.txt).
Known Limitations: the CST816S dependency has one IRQ flag rather than an event
queue. Release-only frames without any observed contact are ignored to avoid
phantom/repeated actions; very fast taps/controller event timing require device
checks. One full-page paint is still required on view/overlay restoration;
no per-contact full-screen wipe remains for ordinary control updates. Existing
Security/2FA and other app drawing implementations were not redesigned.
Outstanding Verification: HWT-0012; slow/held/rapid separate taps, all swipes,
Settings keypad, scroll/Back, PIN lock/unlock, wake and safe confirmations, plus
A–E memory/latency/power scenarios and20 physical cycles.

STATUS: **WARNING — IMPLEMENTED, verification incomplete**.
Build/mock checks pass; actual repaired touch/display/resource behavior remains
unverified until physical feedback. Hardware wiring compatibility is not inferred
from the host tests.

## EVIDENCE

[Owner report](../evidence/2026-11-01-touch-owner-report.txt),
[before build](../evidence/2026-11-01-touch-before-build.txt),
[final builds](../evidence/2026-11-01-touch-builds.txt),
[artifacts](../evidence/2026-11-01-touch-artifacts.txt),
[host](../evidence/2026-11-01-touch-host.txt),
[host alternate](../evidence/2026-11-01-touch-host-alt.txt).

## Upload outcome

Repaired default image was uploaded on COM5 (HWT-0011 PASS):843888 B written,
data hash verified, RTS reset sent. This is flash-write verification only.
Owner must confirm repaired touch and display behavior via HWT-0012; no runtime
heap/latency/power or visual PASS has been recorded.
