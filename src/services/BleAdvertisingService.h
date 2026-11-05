#pragma once
#include <stdint.h>
#include <stddef.h>
#include "services/BleAdvertisingPayload.h"

struct AppContext;
enum class BleAdvertisingOperation : uint8_t { None, Address, Data, Start, Stop };
struct BleAdvertisingEvent {
  BleAdvertisingEvent(BleAdvertisingOperation operation = BleAdvertisingOperation::None, bool success = false)
      : operation(operation), success(success) {}
  BleAdvertisingOperation operation;
  bool success;
};

// Operations enqueue GAP requests. poll() reports their asynchronous completion.
class BleAdvertisingTransport {
public:
  virtual ~BleAdvertisingTransport() = default;
  virtual bool begin() = 0;
  virtual bool setAddress(const uint8_t* address) = 0;
  virtual bool setData(const uint8_t* bytes, size_t length) = 0;
  virtual bool start(bool connectable) = 0;
  virtual bool stop() = 0;
  virtual bool end() = 0;
  virtual uint32_t randomSeed() = 0;
  virtual BleAdvertisingEvent poll() = 0;
};

class BleAdvertisingService {
public:
  explicit BleAdvertisingService(BleAdvertisingTransport* transport = nullptr) : transport_(transport) {}
  bool start(uint8_t count, uint8_t seconds, uint32_t nowMs, AppContext& ctx,
             BleAdvertisingProfile profile = BleAdvertisingProfile::Generic);
  void tick(uint32_t nowMs, AppContext& ctx);
  void stop(uint32_t nowMs, AppContext& ctx);
  bool isActive() const { return phase_ != Phase::Idle; }
  uint8_t advertisedCount() const { return advertisedCount_; }
  uint8_t identityCount() const { return count_; }
  const char* lastError() const { return error_; }

private:
  enum class Phase : uint8_t { Idle, Address, Data, Start, Advertising, Stop, Cleanup };
  void configureDevice(uint32_t nowMs, AppContext& ctx);
  void fail(const char* error, uint32_t nowMs, AppContext& ctx);
  BleAdvertisingTransport* transport_ = nullptr;
  Phase phase_ = Phase::Idle;
  uint32_t startedMs_ = 0;
  uint32_t phaseMs_ = 0;
  uint32_t durationMs_ = 0;
  uint8_t count_ = 0;
  uint8_t index_ = 0;
  uint8_t advertisedCount_ = 0;
  uint8_t address_[6]{};
  uint8_t payload_[31]{};
  uint8_t payloadLength_ = 0;
  BleAdvertisingProfile profile_ = BleAdvertisingProfile::Generic;
  const char* error_ = "";
};
