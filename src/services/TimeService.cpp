#include "services/TimeService.h"
#include <Arduino.h>
#include <time.h>
#include <sys/time.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>

namespace {
// Approximate modern epoch threshold (Jan 1, 2024 = 1704067200)
constexpr uint32_t kValidEpochThreshold = 1704067200;
}

void TimeService::begin() {
  time_t now = clock_(nullptr);
  if (now > kValidEpochThreshold) {
    synced_ = true;
    source_ = TimeSource::Ntp;
    lastSyncEpoch_ = static_cast<uint32_t>(now);
  }
}

const char* TimeService::sourceName() const {
  switch (source_) {
    case TimeSource::Ntp: return "NTP";
    case TimeSource::UsbSerial: return "USB";
    case TimeSource::Manual: return "Manual";
    default: return "None";
  }
}

void TimeService::setTime(uint32_t epoch, TimeSource source) {
  if (epoch < kValidEpochThreshold) return;
  source_ = source;
  lastSyncEpoch_ = epoch;
  lastSyncLocalMs_ = millis();
  synced_ = true;
  syncRequested_ = false;

#if defined(ARDUINO) && !defined(DESKTOP_SIMULATOR)
  struct timeval tv;
  tv.tv_sec = static_cast<time_t>(epoch);
  tv.tv_usec = 0;
  settimeofday(&tv, nullptr);
#else
  simulatedEpoch_ = epoch;
  simulatedAtMs_ = millis();
#endif
  Serial.printf("[TimeService] Time set: %lu (source: %s)\n", (unsigned long)epoch, sourceName());
}

bool TimeService::processSerialCommand(const char* cmd) {
  if (!cmd) return false;
  while (*cmd == ' ' || *cmd == '\t') ++cmd;
  if (strncmp(cmd, "TIME:", 5) == 0 || strncmp(cmd, "time:", 5) == 0) {
    uint32_t epoch = static_cast<uint32_t>(strtoul(cmd + 5, nullptr, 10));
    if (epoch >= kValidEpochThreshold) {
      setTime(epoch, TimeSource::UsbSerial);
      return true;
    }
  } else if (strncmp(cmd, "T:", 2) == 0 || strncmp(cmd, "t:", 2) == 0) {
    uint32_t epoch = static_cast<uint32_t>(strtoul(cmd + 2, nullptr, 10));
    if (epoch >= kValidEpochThreshold) {
      setTime(epoch, TimeSource::UsbSerial);
      return true;
    }
  }
  return false;
}

bool TimeService::processSerialInput(char c) {
  if (c == '\r' || c == '\n') {
    if (serialIdx_ > 0) {
      serialBuf_[serialIdx_] = '\0';
      bool ok = processSerialCommand(serialBuf_);
      serialIdx_ = 0;
      return ok;
    }
    return false;
  }
  if (serialIdx_ + 1 < sizeof(serialBuf_)) {
    serialBuf_[serialIdx_++] = c;
  }
  return false;
}

void TimeService::requestNtpSync(const char* server, long gmtOffsetSec, int daylightOffsetSec) {
  if (server && server[0] != '\0') {
    strncpy(ntpServer_, server, sizeof(ntpServer_) - 1);
    ntpServer_[sizeof(ntpServer_) - 1] = '\0';
  }
  gmtOffsetSec_ = gmtOffsetSec;
  daylightOffsetSec_ = daylightOffsetSec;
  syncRequested_ = true;
  syncAttemptStartMs_ = millis();
  configTime(gmtOffsetSec_, daylightOffsetSec_, ntpServer_);
  Serial.printf("[TimeService] Initiated NTP sync with %s (offset: %ld s)\n", ntpServer_, gmtOffsetSec_);
}

void TimeService::tick(uint32_t nowMs, bool wifiConnected) {
  // Check clock every 1 second
  if (nowMs - lastCheckMs_ < 1000) return;
  lastCheckMs_ = nowMs;

  time_t now = clock_(nullptr);
#if !defined(ARDUINO) || defined(DESKTOP_SIMULATOR)
  if (clock_ == ::time && simulatedEpoch_ >= kValidEpochThreshold) {
    now = static_cast<time_t>(simulatedEpoch_ + (nowMs - simulatedAtMs_) / 1000);
  }
#endif

  if (now > kValidEpochThreshold) {
    if (!synced_) {
      synced_ = true;
      if (syncRequested_ || source_ == TimeSource::None) {
        source_ = TimeSource::Ntp;
      }
      syncRequested_ = false;
      lastSyncEpoch_ = static_cast<uint32_t>(now);
      Serial.printf("[TimeService] Time synchronized successfully! Epoch: %ld\n", (long)now);
    }
  } else {
    if (synced_) syncRequested_ = false;
    synced_ = false;
    source_ = TimeSource::None;
    // If not synced, and Wi-Fi became connected, request NTP sync automatically
    if (wifiConnected && !syncRequested_) {
      requestNtpSync();
    }
  }
}

uint32_t TimeService::getEpoch() const {
#if !defined(ARDUINO) || defined(DESKTOP_SIMULATOR)
  if (simulatedEpoch_ >= kValidEpochThreshold) {
    uint32_t elapsed = (millis() - simulatedAtMs_) / 1000;
    return simulatedEpoch_ + elapsed;
  }
#endif
  time_t now = clock_(nullptr);
  if (now < kValidEpochThreshold) return 0;
  return static_cast<uint32_t>(now);
}

void TimeService::formatTime(char* outBuf, size_t maxLen) const {
  if (!outBuf || maxLen == 0) return;
  uint32_t epoch = getEpoch();
  if (!epoch) {
    strncpy(outBuf, "--:--:--", maxLen - 1);
    outBuf[maxLen - 1] = '\0';
    return;
  }
  time_t localTime = static_cast<time_t>(epoch + (timezoneOffsetHours_ * 3600));
  struct tm timeinfo;
  gmtime_r(&localTime, &timeinfo);
  strftime(outBuf, maxLen, "%H:%M:%S", &timeinfo);
}

void TimeService::formatDate(char* outBuf, size_t maxLen) const {
  if (!outBuf || maxLen == 0) return;
  uint32_t epoch = getEpoch();
  if (!epoch) {
    strncpy(outBuf, "----/--/--", maxLen - 1);
    outBuf[maxLen - 1] = '\0';
    return;
  }
  time_t localTime = static_cast<time_t>(epoch + (timezoneOffsetHours_ * 3600));
  struct tm timeinfo;
  gmtime_r(&localTime, &timeinfo);
  strftime(outBuf, maxLen, "%Y-%m-%d", &timeinfo);
}
