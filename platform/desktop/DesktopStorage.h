#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include <map>

class DesktopStorage {
public:
  static DesktopStorage& instance();

  void init(const std::string& runtimeDir = "simulator/runtime");
  void flush();
  void resetNvs();

  // NVS Key-Value emulation (backing Preferences)
  bool nvsOpen(const std::string& ns, bool readOnly);
  void nvsClose(const std::string& ns);
  bool nvsClear(const std::string& ns);
  bool nvsHasKey(const std::string& ns, const std::string& key);
  bool nvsRemove(const std::string& ns, const std::string& key);

  size_t nvsGetBytesLength(const std::string& ns, const std::string& key);
  size_t nvsGetBytes(const std::string& ns, const std::string& key, void* out, size_t maxLen);
  size_t nvsPutBytes(const std::string& ns, const std::string& key, const void* data, size_t len);

  // MicroSD Sandboxed File System Emulation
  bool isSdMounted() const { return sdMounted_; }
  void mountSd(bool mount = true) { sdMounted_ = mount; }
  void setSdIoFailure(bool fail) { sdIoFail_ = fail; }
  std::string getSdBasePath() const { return sdBasePath_; }
  bool isPathSafe(const std::string& relativePath, std::string* outFullPath = nullptr);

private:
  DesktopStorage() = default;

  std::string runtimeDir_ = "simulator/runtime";
  std::string nvsFilePath_ = "simulator/runtime/nvs_store.dat";
  std::string sdBasePath_ = "simulator/runtime/sdcard";

  bool initialized_ = false;
  bool sdMounted_ = true;
  bool sdIoFail_ = false;

  // namespace -> (key -> bytes)
  std::map<std::string, std::map<std::string, std::vector<uint8_t>>> nvsData_;
  std::map<std::string, bool> openNamespaces_;

  void loadNvsFromFile();
  void saveNvsToFile();
  void ensureDirectories();
};
