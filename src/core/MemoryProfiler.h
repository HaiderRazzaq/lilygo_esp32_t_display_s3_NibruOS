#pragma once
#include <stdint.h>

struct MemorySnapshot {
  uint32_t freeInternalHeap = 0;
  uint32_t minFreeInternalHeap = 0;
  uint32_t largestFreeBlock = 0;
  uint32_t totalPsram = 0;
  uint32_t freePsram = 0;
  uint32_t sketchSize = 0;
  uint32_t flashSize = 0;
  uint32_t uptimeSec = 0;
  const char* resetReason = "Unknown";
};

class MemoryProfiler {
public:
  static MemorySnapshot capture();
  static const char* resetReasonString();
};

