#pragma once
#include <stdint.h>

enum class BleAdvertisingProfile : uint8_t {
  Generic, Android, Windows, Apple, SamsungBuds, SamsungWatch, All, Random,
  AppleAction, AppleNotYourDevice, Beacon, Count
};

const char* bleAdvertisingProfileName(BleAdvertisingProfile profile);
// Beacon names and generic lab identities do not accept inbound connections.
bool bleAdvertisingProfileConnectable(BleAdvertisingProfile profile);
uint8_t buildBleAdvertisingPayload(BleAdvertisingProfile profile, uint8_t identity,
                                   uint32_t randomSeed, uint8_t (&payload)[31]);
