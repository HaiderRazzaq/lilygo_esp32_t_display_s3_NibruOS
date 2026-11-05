# BLE Lab

BLE Lab is a physical-board Home app for classroom demonstrations of Bluetooth
accessory discovery and advertising alerts. Tap the profile row to open the
Targets picker. The first page offers Android, Windows, iPhone/iPad, Samsung
Buds, Samsung Watch, All types, BLE scanner and Random. Header arrows open the
second page for Apple Action, Apple Not Your Device and Beacon spam. Tap a card
to select it. BOOT moves selection and hold confirms; Back returns to
configuration. Fixed profiles provide Devices from 1 to 10 and Seconds from 5
to 60 (Beacon labels the row Names).
Defaults are Android Fast Pair, 5 identities and 30 seconds. START begins the session. STOP,
Back, lock, sleep, screensaver or duration expiry ends it.

In BLE scanner mode, names are LabBLE01 through LabBLE10, with stable random addresses within each
session. A single controller rotates identities in 400 ms advertising windows.
The nominal advertising interval is 100 ms. The raw legacy advertisement is 31
bytes, containing flags, a complete custom service UUID and a complete local
name. The UUID is 6e2746b1-d818-42a4-9811-720256842901.

BLE scanner and Beacon modes use non-connectable advertisements. Pairing and
Apple alert profiles use connectable advertisements matching the reference
default. Use a BLE scanner
to inspect their raw data and the receiver's discovery UI to check notifications. The
Advertised counter counts unique identities with successful GAP start events.
Receiver discovery is verified separately. Scanner caches may retain identities
after their advertising window has ended.

The service acquires an exclusive radio lease and uses existing Wi-Fi modem
sleep coexistence. It requires 150 KiB free internal heap after initialization.
Each asynchronous GAP operation has a two-second timeout. Rotation reuses the
controller. Failed teardown retains the radio lease and retries once per second.

Bruce's advertiser scheduling and controller reuse informed this implementation.
NibruOS uses its pinned Bluedroid host and shared checked startup. Reference
[Bruce source](https://github.com/BruceDevices/firmware/blob/a59213f3ec6cd302fa32c2042b2700c9c24d991a/src/modules/ble/ble_spam.cpp).

The pairing payload layouts and model data in BleAdvertisingPayload.cpp are
adapted from that source. Attribution is preserved in its header and the
[upstream AGPL-3.0 license](../LICENSES/Bruce-AGPL-3.0.txt) is included.
These profiles advertise accessory discovery data; completing accessory
pairing is outside this demonstration.

Android uses service UUID FE2C and registered model IDs from the reference.
Windows uses Microsoft's manufacturer-specific Swift Pair record with a
Classroom Buds display name. Apple uses the dynamic ProximityPair layout and
AirPods model IDs. Samsung uses the reference Buds or Watch records. The Buds
record intentionally retains Bruce's truncated final AD element as a
compatibility detail, requiring physical Samsung verification.

Apple Action emits the Continuity NearbyAction record (manufacturer 0x004C,
type 0x0F) with one of Bruce's fifteen action type codes and three fresh random
bytes per window. Apple Not Your Device reuses the ProximityPair layout with
the 0x01 prefix instead of the normal 0x07 prefix. Beacon spam emits the
reference UUID 0x1812, appearance 0x0180 and one of five preset names, truncated
to the 31-byte legacy limit, using non-connectable advertising. These payloads
are demonstration data only: they do not implement pairing, GATT services,
authentication or anti-spoofing.

All pairing types cycles five protocols, with the selected count applied to
each protocol. At ten per type, fifty identities are rotated. The session
duration still limits the entire run; very short sessions can end before a
complete rotation. Raw payloads are at most 31 bytes. Fixed profiles keep
per-session identity addresses. Operation timeouts and teardown behavior are retained.

Random chooses one of the seven connectable advertising profiles and a model
for each advertising window and refreshes the random address. Beacon remains an
explicit, non-connectable target. In this mode the count row shows automatic
type and model selection; Seconds controls the session duration. The First pass
counter applies to scheduled fixed-profile identities, not receiver notices.

The owner confirmed Windows Swift Pair notifications showing Classroom Buds
(HWT-0080). A host receiver also captured the ten Microsoft payloads. Android
FE2C advertisement reception was observed separately; Android, Apple, Samsung
and Random notification display remain unverified. The target picker and
Random tests belong to VER-0225 and physical acceptance HWT-0081.

Notifications depend on the receiver's software and settings. Windows needs
Swift Pair notification support enabled. Android discovery depends on Google
Play services, nearby-device discovery settings and model availability.
Apple and Samsung compatibility requires their corresponding receivers.
Source implementation and successful advertisement starts alone do not prove
that any particular system version will display a notification.
See [Microsoft](https://learn.microsoft.com/en-us/windows-hardware/design/component-guidelines/bluetooth-swift-pair)
and [Google](https://developers.google.com/nearby/fast-pair/specifications/service/provider).

Implementation is FEAT-0027. Host verification for the extended profiles is
VER-0230 and the target build is VER-0231. Windows v0.18.0 notifications were
confirmed in HWT-0080. New picker/Random acceptance and resource scenarios A–E
are pending under HWT-0081. Desktop work is deferred by the owner under
VER-0228. The v0.19.0 image was written and hash-verified and runtime COM6
returned (HWT-0082). The v0.20.0 image was uploaded on the second attempt and
esptool verified its write hash (HWT-0085). Post-upload software resets kept the
board in the ROM USB download mode, so a physical RESET press or USB re-plug is
needed before running the app (HWT-0086). Apple Action, Not Your Device and
Beacon receiver behavior is NOT RUN (HWT-0084).
