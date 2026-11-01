# Easier account entry implementation plan

Approved scope: owner requests QWERTY/digit order and more than one way to avoid
manual entry of long Base32 secrets. Extend existing FEAT-0014; preserve the
already-dirty phase-one workspace and uploaded data. No commits or flash erases.

## Design / constraints

- Shared QWERTY keyboard: number row 1234567890, QWERTYUIOP, ASDFGHJKL,
  Shift/ZXCVBNM/Delete, Space/Symbols/Cancel/Accept. Reuse it in 2FA and Wi-Fi.
  Base32 mode rejects invalid digits; account names support normal ASCII case.
- ADD offers manual, USB paste and Wi-Fi paste. Receive only during explicit
  unlocked import session, 120 s maximum, then require new physical confirmation
  before encrypted NVS commit. Cancel/exit/sleep/lock clear candidate/session.
- Local, first-party companion page accepts pasted Base32 or otpauth TOTP URI;
  rejects unsupported algorithms/digits/periods and does not persist secrets.
  Serve it on loopback only, without cloud/CDN dependencies.
- Both channels carry the same AES-GCM envelope. Display a one-time 16-character
  random Base32 pairing code (80 random bits); SHA256 derives an ephemeral key.
  Use fresh 96-bit nonce, 128-bit tag, fixed versioned AAD; one authenticated
  candidate/session prevents replay. No ECDH/TLS imitation or untrusted device JS.
- USB uses Web Serial. Wi-Fi uses bounded UDP on existing connection only;
  loopback helper relays ciphertext to a user-selected private IPv4 address.
  No AP, new radio initialization, new task or firmware dependency. Wi-Fi lease
  belongs to AuthenticatorImport and is released when receive stops.
- Preserve GPIO18/17/21/16 touch, parallel LCD, AW9364, framework and partitions.
  app0 6553600 B; physical >=150 KiB internal heap/stack/latency/power acceptance
  must remain NOT MEASURED without current device evidence.

## Tasks / verification

- [x] RED -> GREEN shared QWERTY mapping, shift/symbols/digits, existing Wi-Fi
  and authenticator flows; avoid full clears per keypress.
- [x] RED -> GREEN bounded import service: authenticated frame, bad key/tag,
  overflow, timeout/rollover, cancellation, one candidate, no automatic save.
- [x] Connect shared service through AppContext on both targets; lease and
  physical confirmation tests, lock/sleep/Back cleanup, >=20 import cycles.
- [x] Implement trusted companion with localhost/Host/Origin/CSRF bounds,
  pasted URI validation, WebCrypto GCM, Web Serial and ciphertext UDP relay.
  Test parsers, relay bounds, no-secret persistence/logging, browser crypto
  interoperability using public fixtures, and inspect rendered UI.
- [x] Build default/HID/profile/LEDC; check generated image/partition capacity;
  run host/desktop/docs/tool tests, independent source review and update registry,
  issues, current state, changelog, verification/development/hardware records and
  resource report. Preserve failed attempts. Upload only after all software
  checks pass, following owner's existing authorization to update this device.

Baseline: already captured default image822912 B, static SRAM49248 B,
SHA256 00e74d8689773ea90efc9cfb707fa48aeb593c2cc10d2b8d5edfdb5b8cd0ea8b.
No extra GPIO. Wi-Fi may add one UDP buffer/socket while explicitly importing;
measure image/static delta and leave actual heap/power unclaimed.

Outcome: software gates PASS; authorized upload HWT-0019 PASS. Native physical acceptance/resource measurements HWT-0018 remain NOT RUN / NOT MEASURED. No commit/push.
