#include "platform/desktop/DesktopStorage.h"
#include <fstream>
#include <filesystem>
#include <cstdio>
#include <cstring>
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

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
  std::error_code error;
  writable_ = !fs::exists(nvsFilePath_, error) && !error;
  std::printf("[DesktopStorage] NVS storage reset to empty\n");
}

bool DesktopStorage::reloadNvs() {
  openNamespaces_.clear();
  return loadNvsFromFile();
}

void DesktopStorage::flush() {
  saveNvsToFile();
}

bool DesktopStorage::loadNvsFromFile() {
  nvsData_.clear();
  writable_ = false;
  std::error_code error;
  const bool exists = fs::exists(nvsFilePath_, error);
  if (error) return false;
  if (!exists) { writable_ = true; return true; }
  const auto size = fs::file_size(nvsFilePath_, error);
  if (error || size < 8 || size > 1024 * 1024) return false;
  std::vector<uint8_t> data(static_cast<size_t>(size));
  std::ifstream input(nvsFilePath_, std::ios::binary);
  if (!input.read(reinterpret_cast<char*>(data.data()), data.size()) ||
      memcmp(data.data(), "NVS1", 4) != 0) return false;
  size_t offset = 4;
  auto number = [&](uint32_t& value) {
    if (data.size() - offset < 4) return false;
    memcpy(&value, data.data() + offset, 4); offset += 4; return true;
  };
  auto name = [&](std::string& value) {
    uint32_t length = 0;
    if (!number(length) || length == 0 || length > 64 || length > data.size() - offset) return false;
    value.assign(reinterpret_cast<const char*>(data.data() + offset), length);
    offset += length;
    return value.find('\0') == std::string::npos;
  };
  decltype(nvsData_) parsed;
  uint32_t namespaces = 0;
  if (!number(namespaces) || namespaces > 64) return false;
  for (uint32_t i = 0; i < namespaces; ++i) {
    std::string ns;
    uint32_t keys = 0;
    if (!name(ns) || parsed.count(ns) || !number(keys) || keys > 512) return false;
    auto& destination = parsed[ns];
    for (uint32_t j = 0; j < keys; ++j) {
      std::string key;
      uint32_t length = 0;
      if (!name(key) || destination.count(key) || !number(length) ||
          length > 65536 || length > data.size() - offset) return false;
      destination[key] = std::vector<uint8_t>(data.begin() + offset, data.begin() + offset + length);
      offset += length;
    }
  }
  if (offset != data.size()) return false;
  nvsData_ = std::move(parsed);
  writable_ = true;
  return true;
}

bool DesktopStorage::saveNvsToFile() {
  if (!writable_) return false;
  ensureDirectories();
  const fs::path temporary(nvsFilePath_ + ".tmp");
  std::ofstream os(temporary, std::ios::binary | std::ios::trunc);
  if (!os) return false;
  os.write("NVS1", 4);
  const uint32_t namespaces = static_cast<uint32_t>(nvsData_.size());
  os.write(reinterpret_cast<const char*>(&namespaces), 4);
  for (const auto& [ns, keys] : nvsData_) {
    const uint32_t nsSize = static_cast<uint32_t>(ns.size());
    os.write(reinterpret_cast<const char*>(&nsSize), 4); os.write(ns.data(), nsSize);
    const uint32_t count = static_cast<uint32_t>(keys.size());
    os.write(reinterpret_cast<const char*>(&count), 4);
    for (const auto& [key, value] : keys) {
      const uint32_t keySize = static_cast<uint32_t>(key.size()), valueSize = static_cast<uint32_t>(value.size());
      os.write(reinterpret_cast<const char*>(&keySize), 4); os.write(key.data(), keySize);
      os.write(reinterpret_cast<const char*>(&valueSize), 4);
      if (valueSize) os.write(reinterpret_cast<const char*>(value.data()), valueSize);
    }
  }
  os.flush();
  if (!os) return false;
  os.close();
  if (os.fail()) return false;
#if defined(_WIN32)
  return MoveFileExW(temporary.c_str(), fs::path(nvsFilePath_).c_str(),
                    MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
  std::error_code error;
  fs::rename(temporary, nvsFilePath_, error);
  return !error;
#endif
}

bool DesktopStorage::nvsOpen(const std::string& ns, bool readOnly) {
  if (!writable_) return false;
  openNamespaces_[ns] = readOnly;
  if (!readOnly) nvsData_[ns];
  return !readOnly || nvsData_.count(ns) != 0;
}
void DesktopStorage::nvsClose(const std::string& ns) { openNamespaces_.erase(ns); }

bool DesktopStorage::nvsClear(const std::string& ns) {
  auto it = openNamespaces_.find(ns);
  if (it == openNamespaces_.end() || it->second || !writable_) return false;
  auto previous = nvsData_[ns];
  nvsData_[ns].clear();
  if (saveNvsToFile()) return true;
  nvsData_[ns] = std::move(previous); return false;
}
bool DesktopStorage::nvsHasKey(const std::string& ns, const std::string& key) {
  const auto found = nvsData_.find(ns);
  return writable_ && found != nvsData_.end() && found->second.count(key);
}
bool DesktopStorage::nvsRemove(const std::string& ns, const std::string& key) {
  auto it = openNamespaces_.find(ns);
  if (it == openNamespaces_.end() || it->second || !nvsHasKey(ns, key)) return false;
  auto previous = nvsData_[ns][key];
  nvsData_[ns].erase(key);
  if (saveNvsToFile()) return true;
  nvsData_[ns][key] = std::move(previous); return false;
}
size_t DesktopStorage::nvsGetBytesLength(const std::string& ns, const std::string& key) {
  return nvsHasKey(ns, key) ? nvsData_[ns][key].size() : 0;
}
size_t DesktopStorage::nvsGetBytes(const std::string& ns, const std::string& key, void* out, size_t maxLen) {
  const size_t size = nvsGetBytesLength(ns, key);
  if (!out || size > maxLen || !size) return 0;
  memcpy(out, nvsData_[ns][key].data(), size); return size;
}
size_t DesktopStorage::nvsPutBytes(const std::string& ns, const std::string& key, const void* data, size_t len) {
  auto it = openNamespaces_.find(ns);
  if (it == openNamespaces_.end() || it->second || !writable_ || (!data && len)) return 0;
  const bool existed = nvsHasKey(ns, key);
  auto previous = nvsData_[ns][key];
  const auto* bytes = static_cast<const uint8_t*>(data);
  nvsData_[ns][key] = len ? std::vector<uint8_t>(bytes, bytes + len) : std::vector<uint8_t>{};
  if (saveNvsToFile()) return len;
  if (existed) nvsData_[ns][key] = std::move(previous); else nvsData_[ns].erase(key);
  return 0;
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
