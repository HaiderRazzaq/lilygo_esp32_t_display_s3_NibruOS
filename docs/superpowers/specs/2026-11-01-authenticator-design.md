# Authenticator completion design

The owner approved phase one in chat: complete the existing authenticator, encrypted
save/reopen, valid-time gating, secret-free logging, truthful USB feedback, tests,
resource checks and engineering records. Keep Arduino-ESP32 2.0.14, current pins,
partitions, display/touch baseline and shared desktop application code.

## Approach

Extend existing services and TotpApp. Do not transplant SecureGen's main loop or
add its web/BLE stack. A separate firmware would replace NibruOS; a second vault
implementation would duplicate existing responsibilities. Manual account entry
uses a bounded Base32 keypad, and adds a name before a masked secret (SHA-1,
6 digits, 30 seconds initially). No real account or seed is included in source.

## Behavior and boundaries

- Start with an empty vault. Require an enabled, authenticated PIN session.
- Load a bounded, versioned AES-GCM NVS blob; authenticate its header. Preserve
  old valid records through migration; unknown/corrupt data fails closed and is
  never overwritten by an empty fallback. Commit account edits before showing
  success; keep previous account state on failed writes.
- Wipe plaintext accounts and entry buffers on exit, lock and display sleep.
- Block changing/disabling PIN while persisted accounts exist. This is the
  conservative interim choice: current keys depend directly on PIN/salt, and
  safe key rewrapping/backup is outside phase one. Deleting the final account
  explicitly removes only TOTP storage and restores credential-change ability.
- No uptime-derived tokens. Invalid time displays a waiting state and disables
  sending. Native USB readiness and write results gate success feedback; a
  successful HID report cannot prove the host focused the intended field.
- Keep Back responsive. No new tasks, radios, GPIO, PSRAM buffers, web cabinet,
  SD driver, or dependency upgrades. USB hardware mode remains unchanged.

## Verification and limits

Host tests cover authenticated reopen, failed/corrupt writes, PIN guards,
Base32 boundaries, invalid-time send suppression and USB feedback; desktop
tests exercise the shared UI entry/save/reopen/cancel flows and >=20 cycles.
Use real host crypto where available; label synthetic crypto mocks explicitly.
Build the embedded target and inspect firmware.bin against partitions.bin.
Physical scenarios A-E, internal heap >=150 KiB, stacks, latency, RF and power
remain NOT MEASURED until tested on an identified physical image. Do not flash,
commit or push as part of this implementation.
