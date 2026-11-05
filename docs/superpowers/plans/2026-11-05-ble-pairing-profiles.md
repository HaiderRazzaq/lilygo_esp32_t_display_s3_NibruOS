# BLE pairing profile plan

The owner requested Bruce-style accessory discovery notifications and selected Android or Windows for the first physical test.

- Adapt the Google Fast Pair, Microsoft Swift Pair, Apple ProximityPair and Samsung Buds/Watch payload builders from Bruce, retaining attribution and its AGPL-3.0 license.
- Preserve the existing generic scanner mode and asynchronous controller/session lifecycle. Add profile selection and All pairing types. All mode assigns stable distinct identities across five protocol profiles.
- Use connectable advertisements for pairing profiles, matching the reference default, and non-connectable advertisements for the existing scanner mode.
- Test exact payload markers, lengths, maximum indices, dynamic Apple fields, profile cycling and mixed-profile operation. Run host and ESP32 validation only. Simulator work remains paused.
- Version v0.18.0. Document receiver notification requirements and the distinction between successful advertisement starts and displayed notifications.
