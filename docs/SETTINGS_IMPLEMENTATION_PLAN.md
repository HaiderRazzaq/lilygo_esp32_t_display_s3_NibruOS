# Essential Settings completion — 2026-11-01

The supplied Essential Settings specification is the execution authority. Extend
the existing service, app and profiler; retain existing Security/2FA functionality
without expanding those deferred features. No flash/upload/commit is part of this
change. Work in the supplied checkout; preserve framework, partitions and wiring.

1. Capture the unmodified target build and image size/hash. Inspect all affected
   hardware, lifecycle, NVS and host stubs. Runtime baseline remains NOT MEASURED.
2. Add regression tests for persistence, validation, migration, failed writes,
   scoped reset, physical navigation/confirmation, timeout/wake and lifecycle
   cleanup. Improve the existing mocks only as needed to execute these tests.
3. Complete SettingsService with a versioned, atomic preference record, legacy
   import, write-result reporting and unchanged-value write suppression. Notify
   DisplayManager only for affected values. Preserve all unrelated namespaces.
4. Complete the scrollable Settings menu and distinct short/long BOOT actions.
   Keep Back, existing Security subviews and touch diagnostics. Use bounded
   diagnostics, real chip/uptime metrics and confirmed resource-clean reboot.
5. Implement requested Arduino-ESP32 2.x LEDC brightness with visible HIGH fallback;
   retain GPIO15 and display/touch setup. Compare with existing AW9364 dimming and
   explicitly record physical brightness compatibility as pending.
6. Extend MemoryProfiler with build-time logging control, capability-specific
   memory, task stack and bounded timing maxima; instrument startup and app
   entry/exit without continuous logging. Add partition/baseline artifact checks.
7. Run host tests, tooling tests, documentation checks and target builds. Compare
   before/after resources and document actual results, failures and limitations.

Review focus: failed/partial NVS records and future schemas; repeated identical
settings; confirmation cancellation and wake gesture consumption; millis wrap;
radio ownership during diagnostics/reboot; 20 lifecycle cycles. Software tests
cannot prove PWM compatibility, runtime heap headroom, latency or power.

Resource design: no new task, framebuffer, radio manager, library or extra GPIO.
Use small fixed fields/snapshots, bounded stack formatting and sparse serial
output. Physical scenarios A–E and >=150 KiB internal heap remain pending until
captured from the current image. Existing approximate baseline values must not
be treated as measurements. Provisional regression triggers are review warnings.

## Execution outcome

Implemented in the supplied checkout without commits, dependency upgrades or
uploads. Default AW9364 retained; requested LEDC is a build-tested opt-in because
existing history reports PWM incompatibility. Added small contact state/identity
and a 500ms non-blocking confirmation window after review exposed repeated touch
reports. Existing Security/2FA retained; no new deferred features. Final software
evidence is VER-0017 through VER-0020; HWT-0008 remains NOT RUN. Full before/after
costs, initial failed attempts and unresolved runtime metrics are recorded in
[Resource Impact Report](performance/SETTINGS_RESOURCE_IMPACT.md).
