#pragma once
#include "Arduino.h"
#include <map>
#include <vector>

// In-memory namespace persistence with injectable faults; not flash emulation.
class Preferences {
public:
  inline static std::map<std::string, std::map<std::string, std::vector<uint8_t>>> store;
  inline static bool failOpen = false, failWrite = false;
  inline static unsigned writes = 0;
  static void resetMock() { store.clear(); failOpen = failWrite = false; writes = 0; }
  bool begin(const char* ns, bool ro = false) {
    ns_ = ns; ro_ = ro;
    open_ = !failOpen && (!ro || store.count(ns_) != 0);
    if (open_ && !ro) store[ns_];
    return open_;
  }
  void end() { open_ = false; }
  bool clear() { if (!open_ || ro_ || failWrite) return false; store[ns_].clear(); ++writes; return true; }
  bool isKey(const char* key) { return open_ && store[ns_].count(key); }
  bool remove(const char* key) { return open_ && !ro_ && !failWrite && store[ns_].erase(key); }
  size_t getBytesLength(const char* key) { return isKey(key) ? store[ns_][key].size() : 0; }
  size_t getBytes(const char* key, void* out, size_t len) {
    if (!isKey(key) || len < getBytesLength(key)) return 0;
    const auto& b = store[ns_][key]; std::memcpy(out, b.data(), b.size()); return b.size();
  }
  size_t putBytes(const char* key, const void* value, size_t len) {
    if (!open_ || ro_ || failWrite) return 0;
    const auto* p = static_cast<const uint8_t*>(value);
    store[ns_][key] = std::vector<uint8_t>(p, p + len); ++writes; return len;
  }
  template<class T> T get(const char* k, T d) { T v; return getBytesLength(k) == sizeof(T) && getBytes(k, &v, sizeof(v)) ? v : d; }
  uint8_t getUChar(const char* k, uint8_t d = 0) { return get(k,d); }
  uint16_t getUShort(const char* k, uint16_t d = 0) { return get(k,d); }
  uint32_t getUInt(const char* k, uint32_t d = 0) { return get(k,d); }
  bool getBool(const char* k, bool d = false) { return get(k,d); }
  size_t putUChar(const char* k, uint8_t v) { return putBytes(k,&v,sizeof(v)); }
  size_t putUShort(const char* k, uint16_t v) { return putBytes(k,&v,sizeof(v)); }
  size_t putUInt(const char* k, uint32_t v) { return putBytes(k,&v,sizeof(v)); }
  size_t putBool(const char* k, bool v) { return putBytes(k,&v,sizeof(v)); }
  String getString(const char* k, const String& d = "") {
    return isKey(k) ? String(reinterpret_cast<const char*>(store[ns_][k].data())) : d;
  }
  size_t putString(const char* k, const char* v) { return v ? (putBytes(k,v,strlen(v)+1) ? strlen(v) : 0) : 0; }
  size_t putString(const char* k, const String& v) { return putString(k,v.c_str()); }
private:
  std::string ns_;
  bool ro_ = true, open_ = false;
};
