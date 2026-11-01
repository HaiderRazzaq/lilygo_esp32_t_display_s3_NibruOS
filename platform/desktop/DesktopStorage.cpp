#include "platform/desktop/DesktopStorage.h"
#include <fstream>
#include <filesystem>
#include <cstdio>
#include <cstring>

namespace fs = std::filesystem;

DesktopStorage& DesktopStorage::instance() {
  static DesktopStorage s_instance;
  return s_instance;
}

void DesktopStorage::init(const std::string& runtimeDir) {
  if (initialized_) return;
  runtimeDir_ = runtimeDir;
  nvsFilePath_ = runtimeDir_ + "/nvs_store.dat";
  sdBasePath_ = runtimeDir_ + "/sdcard";
  ensureDirectories();
  loadNvsFromFile();
  initialized_ = true;
  std::printf("[DesktopStorage] Initialized with runtime directory '%s'\n", runtimeDir_.c_str());
}

void DesktopStorage::ensureDirectories() {
  try {
    fs::create_directories(runtimeDir_);
    fs::create_directories(sdBasePath_);
  } catch (const std::exception& e) {
    std::printf("[DesktopStorage] Directory creation failed: %s\n", e.what());
  }
}

void DesktopStorage::resetNvs() {
  nvsData_.clear();
  try {
    if (fs::exists(nvsFilePath_)) {
      fs::remove(nvsFilePath_);
    }
  } catch (...) {}
  std::printf("[DesktopStorage] NVS storage reset to empty\n");
}

void DesktopStorage::flush() {
  saveNvsToFile();
}

void DesktopStorage::loadNvsFromFile() {
  nvsData_.clear();
  std::ifstream is(nvsFilePath_, std::ios::binary);
  if (!is.is_open()) return;

  char magic[4]{};
  is.read(magic, 4);
  if (std::memcmp(magic, "NVS1", 4) != 0) {
    is.close();
    std::printf("[DesktopStorage] Invalid magic in NVS store\n");
    return;
  }

  uint32_t nsCount = 0;
  is.read(reinterpret_cast<char*>(&nsCount), sizeof(nsCount));
  for (uint32_t i = 0; i < nsCount; ++i) {
    uint32_t nsLen = 0;
    is.read(reinterpret_cast<char*>(&nsLen), sizeof(nsLen));
    std::string ns(nsLen, '\0');
    is.read(&ns[0], nsLen);

    uint32_t keyCount = 0;
    is.read(reinterpret_cast<char*>(&keyCount), sizeof(keyCount));
    for (uint32_t k = 0; k < keyCount; ++k) {
      uint32_t keyLen = 0;
      is.read(reinterpret_cast<char*>(&keyLen), sizeof(keyLen));
      std::string key(keyLen, '\0');
      is.read(&key[0], keyLen);

      uint32_t valLen = 0;
      is.read(reinterpret_cast<char*>(&valLen), sizeof(valLen));
      std::vector<uint8_t> val(valLen);
      if (valLen > 0) {
        is.read(reinterpret_cast<char*>(val.data()), valLen);
      }
      nvsData_[ns][key] = val;
    }
  }
  is.close();
  std::printf("[DesktopStorage] Loaded %zu NVS namespace(s) from '%s'\n",
              nvsData_.size(), nvsFilePath_.c_str());
}

void DesktopStorage::saveNvsToFile() {
  ensureDirectories();
  std::ofstream os(nvsFilePath_, std::ios::binary | std::ios::trunc);
  if (!os.is_open()) {
    std::printf("[DesktopStorage] Failed to open '%s' for saving\n", nvsFilePath_.c_str());
    return;
  }

  const char magic[4] = {'N', 'V', 'S', '1'};
  os.write(magic, 4);

  const uint32_t nsCount = static_cast<uint32_t>(nvsData_.size());
  os.write(reinterpret_cast<const char*>(&nsCount), sizeof(nsCount));

  for (const auto& [ns, keys] : nvsData_) {
    const uint32_t nsLen = static_cast<uint32_t>(ns.size());
    os.write(reinterpret_cast<const char*>(&nsLen), sizeof(nsLen));
    os.write(ns.data(), nsLen);

    const uint32_t keyCount = static_cast<uint32_t>(keys.size());
    os.write(reinterpret_cast<const char*>(&keyCount), sizeof(keyCount));

    for (const auto& [k, val] : keys) {
      const uint32_t keyLen = static_cast<uint32_t>(k.size());
      os.write(reinterpret_cast<const char*>(&keyLen), sizeof(keyLen));
      os.write(k.data(), keyLen);

      const uint32_t valLen = static_cast<uint32_t>(val.size());
      os.write(reinterpret_cast<const char*>(&valLen), sizeof(valLen));
      if (valLen > 0) {
        os.write(reinterpret_cast<const char*>(val.data()), valLen);
      }
    }
  }
  os.close();
}

bool DesktopStorage::nvsOpen(const std::string& ns, bool readOnly) {
  openNamespaces_[ns] = readOnly;
  if (!readOnly && nvsData_.find(ns) == nvsData_.end()) {
    nvsData_[ns] = {};
  }
  return true;
}

void DesktopStorage::nvsClose(const std::string& ns) {
  openNamespaces_.erase(ns);
}

bool DesktopStorage::nvsClear(const std::string& ns) {
  auto it = openNamespaces_.find(ns);
  if (it == openNamespaces_.end() || it->second) return false; // read-only or not open
  nvsData_[ns].clear();
  saveNvsToFile();
  return true;
}

bool DesktopStorage::nvsHasKey(const std::string& ns, const std::string& key) {
  auto nsIt = nvsData_.find(ns);
  if (nsIt == nvsData_.end()) return false;
  return nsIt->second.find(key) != nsIt->second.end();
}

bool DesktopStorage::nvsRemove(const std::string& ns, const std::string& key) {
  auto it = openNamespaces_.find(ns);
  if (it == openNamespaces_.end() || it->second) return false;
  auto nsIt = nvsData_.find(ns);
  if (nsIt == nvsData_.end()) return false;
  bool erased = nsIt->second.erase(key) > 0;
  if (erased) saveNvsToFile();
  return erased;
}

size_t DesktopStorage::nvsGetBytesLength(const std::string& ns, const std::string& key) {
  auto nsIt = nvsData_.find(ns);
  if (nsIt == nvsData_.end()) return 0;
  auto kIt = nsIt->second.find(key);
  if (kIt == nsIt->second.end()) return 0;
  return kIt->second.size();
}

size_t DesktopStorage::nvsGetBytes(const std::string& ns, const std::string& key, void* out, size_t maxLen) {
  if (!out || maxLen == 0) return 0;
  auto nsIt = nvsData_.find(ns);
  if (nsIt == nvsData_.end()) return 0;
  auto kIt = nsIt->second.find(key);
  if (kIt == nsIt->second.end()) return 0;

  const size_t copyLen = std::min(maxLen, kIt->second.size());
  std::memcpy(out, kIt->second.data(), copyLen);
  return copyLen;
}

size_t DesktopStorage::nvsPutBytes(const std::string& ns, const std::string& key, const void* data, size_t len) {
  auto it = openNamespaces_.find(ns);
  if (it == openNamespaces_.end() || it->second) return 0; // read-only
  if (!data && len > 0) return 0;

  const auto* bytes = static_cast<const uint8_t*>(data);
  nvsData_[ns][key] = std::vector<uint8_t>(bytes, bytes + len);
  saveNvsToFile();
  return len;
}

bool DesktopStorage::isPathSafe(const std::string& relativePath, std::string* outFullPath) {
  // Reject parent traversal
  if (relativePath.find("..") != std::string::npos) return false;

  std::string clean = relativePath;
  while (!clean.empty() && (clean.front() == '/' || clean.front() == '\\')) {
    clean.erase(0, 1);
  }

  try {
    fs::path base = fs::absolute(sdBasePath_);
    fs::path full = fs::absolute(base / clean);
    std::string baseStr = base.generic_string();
    std::string fullStr = full.generic_string();

    if (fullStr.rfind(baseStr, 0) != 0) return false;
    if (outFullPath) *outFullPath = fullStr;
    return true;
  } catch (...) {
    return false;
  }
}
