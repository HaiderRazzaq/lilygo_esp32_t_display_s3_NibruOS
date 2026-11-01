#include "core/MemoryProfiler.h"

#ifdef ARDUINO
#include <Arduino.h>
#include <esp_heap_caps.h>
#include <esp_system.h>

const char* MemoryProfiler::resetReasonString() {
  switch (esp_reset_reason()) {
    case ESP_RST_POWERON:   return "Power-on";
    case ESP_RST_EXT:       return "External Pin";
    case ESP_RST_SW:        return "Software Restart";
    case ESP_RST_PANIC:     return "Software Crash/Panic";
    case ESP_RST_INT_WDT:   return "Interrupt WDT";
    case ESP_RST_TASK_WDT:  return "Task WDT";
    case ESP_RST_WDT:       return "Other WDT";
    case ESP_RST_DEEPSLEEP: return "Deep Sleep Wake";
    case ESP_RST_BROWNOUT:  return "Brownout Reset";
    case ESP_RST_SDIO:      return "SDIO Reset";
    default:                return "Normal / Unknown";
  }
}

MemorySnapshot MemoryProfiler::capture() {
  MemorySnapshot s{};
  s.freeInternalHeap = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
  s.minFreeInternalHeap = heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL);
  s.largestFreeBlock = heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL);
  s.totalPsram = ESP.getPsramSize();
  s.freePsram = ESP.getFreePsram();
  s.sketchSize = ESP.getSketchSize();
  s.flashSize = ESP.getFlashChipSize();
  s.uptimeSec = static_cast<uint32_t>(millis() / 1000UL);
  s.resetReason = resetReasonString();
  return s;
}

#else

const char* MemoryProfiler::resetReasonString() {
  return "Host Test";
}

MemorySnapshot MemoryProfiler::capture() {
  MemorySnapshot s{};
  s.freeInternalHeap = 280000;
  s.minFreeInternalHeap = 250000;
  s.largestFreeBlock = 180000;
  s.totalPsram = 8388608;
  s.freePsram = 8000000;
  s.sketchSize = 770000;
  s.flashSize = 16777216;
  s.uptimeSec = 100;
  s.resetReason = resetReasonString();
  return s;
}

#endif

