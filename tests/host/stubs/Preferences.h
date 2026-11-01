#pragma once
#include <stddef.h>
#include <stdint.h>
#include <cstring>
#include "Arduino.h"

class Preferences {
public:
  bool begin(const char*, bool = false) { return true; }
  void end() {}
  bool clear() { return true; }
  uint8_t getUChar(const char*, uint8_t def = 0) { return def; }
  size_t putUChar(const char*, uint8_t) { return 1; }
  uint16_t getUShort(const char*, uint16_t def = 0) { return def; }
  size_t putUShort(const char*, uint16_t) { return 2; }
  bool getBool(const char*, bool def = false) { return def; }
  size_t putBool(const char*, bool) { return 1; }
  String getString(const char*, const String& def = "") { return def; }
  size_t putString(const char*, const char* val) { return val ? strlen(val) : 0; }
  size_t putString(const char*, const String& val) { return val.length(); }
};
