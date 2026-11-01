#pragma once
#include <stdint.h>
#ifndef NIBRU_PROFILE
#define NIBRU_PROFILE 0
#endif

struct MemorySnapshot {
  bool available = false;
  uint32_t freeInternalHeap = 0;
  uint32_t minFreeInternalHeap = 0;
  uint32_t largestFreeBlock = 0;
  uint32_t totalPsram = 0;
  uint32_t freePsram = 0;
  uint32_t minFreePsram = 0;
  uint32_t largestPsramBlock = 0;
  uint32_t stackHighWaterBytes = 0;
  uint32_t sketchSize = 0;
  uint32_t flashSize = 0;
  uint32_t uptimeSec = 0;
  const char* resetReason = "Unknown";
};

class MemoryProfiler {
public:
  static MemorySnapshot capture();
  static const char* resetReasonString();
  static void checkpoint(const char* point);
  static void observeLoop(uint32_t elapsedUs);
  static uint32_t maxLoopUs();
};

class ProfileScope {
public:
#if NIBRU_PROFILE
  explicit ProfileScope(const char* point);
  ~ProfileScope();
private:
  const char* point_;
  uint32_t start_;
#else
  explicit ProfileScope(const char*) {}
#endif
};

