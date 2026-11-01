#include "services/TimeService.h"
#include <Arduino.h>
#include <time.h>
#include <sys/time.h>

namespace {
// Approximate modern epoch threshold (Jan 1, 2024 = 1704067200)
constexpr uint32_t kValidEpochThreshold = 1704067200;
}

void TimeService::begin() {
  time_t now = clock_(nullptr);
  if (now > kValidEpochThreshold) {
    synced_ = true;
  }
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
  if (now > kValidEpochThreshold) {
    if (!synced_) {
      synced_ = true;
      syncRequested_ = false;
      Serial.printf("[TimeService] Time synchronized successfully! Epoch: %ld\n", (long)now);
    }
  } else {
    if (synced_) syncRequested_ = false;
    synced_ = false;
    // If not synced, and Wi-Fi became connected, request NTP sync automatically
    if (wifiConnected && !syncRequested_) {
      requestNtpSync();
    }
  }
}

uint32_t TimeService::getEpoch() const {
  time_t now = clock_(nullptr);
  if (now < kValidEpochThreshold) return 0;
  return static_cast<uint32_t>(now);
}

void TimeService::formatTime(char* outBuf, size_t maxLen) const {
  if (!outBuf || maxLen == 0) return;
  time_t now = clock_(nullptr);
  if (now < kValidEpochThreshold) {
    strncpy(outBuf, "--:--:--", maxLen - 1);
    outBuf[maxLen - 1] = '\0';
    return;
  }
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);
  strftime(outBuf, maxLen, "%H:%M:%S", &timeinfo);
}

void TimeService::formatDate(char* outBuf, size_t maxLen) const {
  if (!outBuf || maxLen == 0) return;
  time_t now = clock_(nullptr);
  if (now < kValidEpochThreshold) {
    strncpy(outBuf, "----/--/--", maxLen - 1);
    outBuf[maxLen - 1] = '\0';
    return;
  }
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);
  strftime(outBuf, maxLen, "%Y-%m-%d", &timeinfo);
}

