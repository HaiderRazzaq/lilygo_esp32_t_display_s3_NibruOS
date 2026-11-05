# BLE target selection and Random plan

The owner confirmed Windows Swift Pair notifications and requested clearer target-family selection plus Random.

- Record the owner confirmation and captured Android/Windows advertisements separately.
- Replace sequential mode cycling with a two-column target picker. Support Back, BOOT selection and activation. Preserve active-session editing locks.
- Add Random to the existing payload profiles. Random chooses an accessory profile/model per window and refreshes its random address. Keep the configured slots, duration cap, radio ownership and cleanup.
- Test picker routing, Back and physical-button flow, random payload bounds, address rotation and existing profile regressions. Build ESP32 and check image/resources/docs. Simulator work remains paused.
- Version v0.19.0. Upload after verification under the owner's existing instruction, then record runtime serial return separately from untested Random receiver behavior.
