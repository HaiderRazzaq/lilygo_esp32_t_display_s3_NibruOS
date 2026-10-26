#pragma once
#include <stdint.h>

// One owner for radio resource in v0.1. This prevents accidental conflicting use
// as more apps are added. Non-WiFi apps never acquire it.
enum class ResourceOwner : uint8_t { None, WiFiScanner };
class ResourceManager {
public:
  bool acquireWiFi(ResourceOwner requester);
  void releaseWiFi(ResourceOwner requester);
  bool ownsWiFi(ResourceOwner requester) const;
  ResourceOwner wifiOwner() const { return wifiOwner_; }
private:
  ResourceOwner wifiOwner_ = ResourceOwner::None;
};
