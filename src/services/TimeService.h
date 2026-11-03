#pragma once
#include <stdint.h>
#include <stddef.h>
#include <time.h>

enum class TimeSource : uint8_t {
  None = 0,
  Ntp = 1,
  UsbSerial = 2,
  Manual = 3
};

/**
 * @brief Manages system time, NTP synchronization over Wi-Fi, USB serial sync, and time formatting.
 */
class TimeService {
public:
  using ClockSource = time_t (*)(time_t*);
  explicit TimeService(ClockSource source = ::time) : clock_(source ? source : ::time) {}
  void begin();

  // Requests non-blocking NTP synchronization
  void requestNtpSync(const char* server = "pool.ntp.org", long gmtOffsetSec = 0, int daylightOffsetSec = 0);

  // Polls time state periodically in non-blocking fashion
  void tick(uint32_t nowMs, bool wifiConnected);

  // Directly sets the system clock (via settimeofday on target, or simulated clock)
  void setTime(uint32_t epoch, TimeSource source = TimeSource::Manual);

  // Non-blocking serial command processing (e.g. "TIME:1730648400")
  bool processSerialCommand(const char* cmd);
  bool processSerialInput(char c);

  // Whether system clock has been synchronized with real Unix epoch
  bool isSynced() const { return synced_; }

  // Current synchronization source
  TimeSource source() const { return source_; }
  const char* sourceName() const;

  // Epoch of last synchronization
  uint32_t lastSyncEpoch() const { return lastSyncEpoch_; }

  // Timezone offset in hours (-12 to +14) for local display
  void setTimezoneOffset(int8_t hours) { timezoneOffsetHours_ = hours; }
  int8_t timezoneOffset() const { return timezoneOffsetHours_; }

  // Current Unix Epoch seconds (0 if not set / below threshold)
  uint32_t getEpoch() const;

  // Formats current local time into HH:MM:SS string
  void formatTime(char* outBuf, size_t maxLen) const;

  // Formats current local date into YYYY-MM-DD string
  void formatDate(char* outBuf, size_t maxLen) const;

private:
  ClockSource clock_;
  bool synced_ = false;
  bool syncRequested_ = false;
  TimeSource source_ = TimeSource::None;
  uint32_t lastCheckMs_ = 0;
  uint32_t syncAttemptStartMs_ = 0;
  uint32_t lastSyncEpoch_ = 0;
  uint32_t lastSyncLocalMs_ = 0;
  int8_t timezoneOffsetHours_ = 3; // Default UTC+3 (Asia/Baghdad)
  long gmtOffsetSec_ = 0;
  int daylightOffsetSec_ = 0;
  char ntpServer_[32] = "pool.ntp.org";

  uint32_t simulatedEpoch_ = 0;
  uint32_t simulatedAtMs_ = 0;

  char serialBuf_[32]{};
  uint8_t serialIdx_ = 0;
};
