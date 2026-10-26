#include "core/ResourceManager.h"
bool ResourceManager::acquireWiFi(ResourceOwner who) {
  if (who == ResourceOwner::None) return false;
  if (wifiOwner_ != ResourceOwner::None && wifiOwner_ != who) return false;
  wifiOwner_ = who;
  return true;
}
void ResourceManager::releaseWiFi(ResourceOwner who) {
  if (who == wifiOwner_) wifiOwner_ = ResourceOwner::None;
}
bool ResourceManager::ownsWiFi(ResourceOwner who) const {
  return who != ResourceOwner::None && who == wifiOwner_;
}
