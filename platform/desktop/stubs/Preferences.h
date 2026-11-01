#pragma once
#include "Arduino.h"
#include "platform/desktop/DesktopStorage.h"
#include <string>
#include <cstring>

class Preferences {
public:
  inline static bool failOpen = false;
  inline static bool failWrite = false;
  inline static unsigned writes = 0;

  static void resetMock() {
    failOpen = false;
    failWrite = false;
    writes = 0;
    DesktopStorage::instance().resetNvs();
  }

  bool begin(const char* name, bool readOnly = false) {
    if (failOpen) return false;
    ns_ = name ? name : "";
    ro_ = readOnly;
    open_ = DesktopStorage::instance().nvsOpen(ns_, readOnly);
    return open_;
  }

  void end() {
    if (open_) {
      DesktopStorage::instance().nvsClose(ns_);
      open_ = false;
    }
  }

  bool clear() {
    if (!open_ || ro_ || failWrite) return false;
    ++writes;
    return DesktopStorage::instance().nvsClear(ns_);
  }

  bool isKey(const char* key) {
    if (!open_ || !key) return false;
    return DesktopStorage::instance().nvsHasKey(ns_, key);
  }

  bool remove(const char* key) {
    if (!open_ || ro_ || failWrite || !key) return false;
    ++writes;
    return DesktopStorage::instance().nvsRemove(ns_, key);
  }

  size_t getBytesLength(const char* key) {
    if (!open_ || !key) return 0;
    return DesktopStorage::instance().nvsGetBytesLength(ns_, key);
  }

  size_t getBytes(const char* key, void* buf, size_t maxLen) {
    if (!open_ || !key || !buf) return 0;
    return DesktopStorage::instance().nvsGetBytes(ns_, key, buf, maxLen);
  }

  size_t putBytes(const char* key, const void* value, size_t len) {
    if (!open_ || ro_ || failWrite || !key) return 0;
    ++writes;
    return DesktopStorage::instance().nvsPutBytes(ns_, key, value, len);
  }

  template<class T>
  T get(const char* k, T d) {
    T v;
    return (getBytesLength(k) == sizeof(T) && getBytes(k, &v, sizeof(T)) == sizeof(T)) ? v : d;
  }

  uint8_t getUChar(const char* k, uint8_t d = 0) { return get(k, d); }
  uint16_t getUShort(const char* k, uint16_t d = 0) { return get(k, d); }
  uint32_t getUInt(const char* k, uint32_t d = 0) { return get(k, d); }
  bool getBool(const char* k, bool d = false) { return get(k, d); }

  size_t putUChar(const char* k, uint8_t v) { return putBytes(k, &v, sizeof(v)); }
  size_t putUShort(const char* k, uint16_t v) { return putBytes(k, &v, sizeof(v)); }
  size_t putUInt(const char* k, uint32_t v) { return putBytes(k, &v, sizeof(v)); }
  size_t putBool(const char* k, bool v) { return putBytes(k, &v, sizeof(v)); }

  String getString(const char* k, const String& d = "") {
    if (!isKey(k)) return d;
    const size_t len = getBytesLength(k);
    if (len == 0) return String("");
    std::string s(len, '\0');
    getBytes(k, &s[0], len);
    if (!s.empty() && s.back() == '\0') s.pop_back();
    return String(s.c_str());
  }

  size_t putString(const char* k, const char* v) {
    if (!v) return 0;
    return putBytes(k, v, std::strlen(v) + 1);
  }

  size_t putString(const char* k, const String& v) {
    return putString(k, v.c_str());
  }

private:
  std::string ns_;
  bool ro_ = false;
  bool open_ = false;
};
