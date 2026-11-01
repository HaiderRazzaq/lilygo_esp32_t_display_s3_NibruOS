#pragma once
#include <stdint.h>
#include <stddef.h>
#include <time.h>

/**
 * @brief Manages system time, NTP synchronization over Wi-Fi, and time formatting.
 */
class TimeService {
public:
  void begin();

  // Requests non-blocking NTP synchronization
  void requestNtpSync(const char* server = "pool.ntp.org", long gmtOffsetSec = 0, int daylightOffsetSec = 0);

  // Polls time state periodically in non-blocking fashion
  void tick(uint32_t nowMs, bool wifiConnected);

  // Whether system clock has been synchronized with real Unix epoch
  bool isSynced() const { return synced_; }

  // Current Unix Epoch seconds (0 if not set)
  uint32_t getEpoch() const;

  // Formats current time into HH:MM:SS string
  void formatTime(char* outBuf, size_t maxLen) const;

  // Formats current date into YYYY-MM-DD string
  void formatDate(char* outBuf, size_t maxLen) const;

private:
  bool synced_ = false;
  bool syncRequested_ = false;
  uint32_t lastCheckMs_ = 0;
  uint32_t syncAttemptStartMs_ = 0;
  long gmtOffsetSec_ = 0;
  int daylightOffsetSec_ = 0;
  char ntpServer_[32] = "pool.ntp.org";
};

