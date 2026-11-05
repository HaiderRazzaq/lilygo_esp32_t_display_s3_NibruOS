# BLE advertising modes implementation plan (v0.20.0)

**Goal:** Add selected Bruce BLE spam/advertising demonstrations to the existing
BLE Lab without changing the verified hardware baseline, radio ownership,
existing targets or session timing.

**Reference:** Bruce firmware
`src/modules/ble/ble_spam.cpp` at
`a59213f3ec6cd302fa32c2042b2700c9c24d991a`, AGPL-3.0. See
[BLE Lab](../../BLE_LAB.md) and the bundled license.

## Scope

- Apple Action: Continuity NearbyAction with Bruce's fifteen action codes and
  fresh trailing bytes per window.
- Apple Not Your Device: ProximityPair layout with prefix `0x01`.
- BLE Beacon spam: UUID `0x1812`, appearance `0x0180`, five preset names,
  non-connectable advertising.
- Second target-picker page behind header arrows. Page one, its coordinates,
  Back/BOOT behavior and existing tests remain unchanged.
- Random chooses among the seven connectable pairing/alert profiles; Beacon
  remains an explicit target.

## Non-goals

- No GATT Fast Pair service, key-based pairing, anti-spoofing or account keys.
- No TX-power change, faster advertising rhythm, MAC-rotation selector or
  persistent custom name lists.
- No new radio stack, task, queue, dynamic allocation, GPIO or partition change.
- No desktop registration while simulator work is paused by the owner.

## Verification

- Host: payload markers/lengths, identity bounds, page-two selection,
  connectable routing, cleanup and 20 cycles (VER-0230).
- ESP32 build and app0 capacity (VER-0231).
- Documentation consistency (VER-0232).
- Physical receiver behavior and resource scenarios A-E remain HWT-0084 NOT RUN.

## Constraints

- Every payload stays at or below 31 bytes.
- `ResourceOwner::BleAdvertising`, 400 ms windows, 100 ms nominal interval,
  two-second operation timeout and teardown retry are unchanged.
- Never claim that an advertisement-only payload will display a notification on
  a particular OS or vendor build.
