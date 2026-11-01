#include "core/MemoryProfiler.h"
#include <Arduino.h>

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
  s.available = true;
  const uint32_t internal = MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT;
  s.freeInternalHeap = heap_caps_get_free_size(internal);
  s.minFreeInternalHeap = heap_caps_get_minimum_free_size(internal);
  s.largestFreeBlock = heap_caps_get_largest_free_block(internal);
  s.totalPsram = ESP.getPsramSize();
  s.freePsram = ESP.getFreePsram();
  s.minFreePsram = heap_caps_get_minimum_free_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  s.largestPsramBlock = heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  // ESP-IDF reports this high-water mark in bytes (not upstream FreeRTOS words).
  s.stackHighWaterBytes = uxTaskGetStackHighWaterMark(nullptr);
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
  // Host diagnostics deliberately carry no purported device measurements.
  s.uptimeSec = millis() / 1000U;
  s.resetReason = resetReasonString();
  return s;
}

#endif

namespace {
#if NIBRU_PROFILE
uint32_t loopMaxUs = 0;
#endif
}

void MemoryProfiler::checkpoint(const char* point) {
#if NIBRU_PROFILE
  const auto s = capture();
  Serial.printf("[Profile] %s valid=%u int=%lu min=%lu block=%lu psram=%lu psmin=%lu psblock=%lu stack_bytes=%lu loop_max_us=%lu\n",
    point, s.available, (unsigned long)s.freeInternalHeap, (unsigned long)s.minFreeInternalHeap,
    (unsigned long)s.largestFreeBlock, (unsigned long)s.freePsram,
    (unsigned long)s.minFreePsram, (unsigned long)s.largestPsramBlock,
    (unsigned long)s.stackHighWaterBytes, (unsigned long)loopMaxUs);
#else
  (void)point;
#endif
}
void MemoryProfiler::observeLoop(uint32_t elapsedUs) {
#if NIBRU_PROFILE
  if (elapsedUs > loopMaxUs) loopMaxUs = elapsedUs;
#else
  (void)elapsedUs;
#endif
}
uint32_t MemoryProfiler::maxLoopUs() {
#if NIBRU_PROFILE
  return loopMaxUs;
#else
  return 0;
#endif
}
#if NIBRU_PROFILE
ProfileScope::ProfileScope(const char* point) : point_(point), start_(micros()) {}
ProfileScope::~ProfileScope() {
  const uint32_t duration = micros() - start_;
  Serial.printf("[Profile] %s duration_us=%lu\n", point_, (unsigned long)duration);
}
#endif

