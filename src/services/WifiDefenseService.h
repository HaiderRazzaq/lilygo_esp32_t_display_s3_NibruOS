#pragma once
#include <stdint.h>
#include <stddef.h>

struct IncidentRecord {
  uint8_t sourceMac[6];
  uint8_t targetMac[6];
  uint8_t bssid[6];
  uint16_t reasonCode;
  int8_t rssi;
  uint8_t channel;
  uint32_t timestampMs;
  bool isDeauth;
};

class WifiDefenseService {
public:
  void begin();
  bool start(uint8_t initialChannel = 1, bool autoHop = true, uint32_t nowMs = 0);
  void stop();
  void tick(uint32_t nowMs);

  bool isRunning() const { return running_; }
  bool isPaused() const { return paused_; }
  void setPaused(bool paused) { paused_ = paused; }

  uint8_t channel() const { return currentChannel_; }
  bool autoHop() const { return autoHop_; }
  void setChannel(uint8_t ch);
  void setAutoHop(bool enabled);
  void cycleChannelMode();

  uint32_t totalPackets() const { return totalPackets_; }
  uint32_t deauthCount() const { return deauthCount_; }
  uint32_t disassocCount() const { return disassocCount_; }
  uint32_t eapolCount() const { return eapolCount_; }
  uint32_t beaconCount() const { return beaconCount_; }
  uint16_t deauthRate() const { return deauthRate_; }
  uint16_t packetRate() const { return packetRate_; }
  bool isUnderAttack() const { return attackAlert_; }

  const IncidentRecord& lastIncident() const { return lastIncident_; }
  bool hasIncident() const { return lastIncident_.timestampMs != 0; }
  void resetStats();

  // Frame processing hook (called from promiscuous RX callback or host test injection)
  void processFrame(const uint8_t* payload, uint16_t length, int8_t rssi, uint8_t channel, uint32_t nowMs);

  static void formatMac(const uint8_t* mac, char* buffer, size_t maxLen);
  static const char* reasonString(uint16_t code);

private:
  bool running_ = false;
  bool paused_ = false;
  bool autoHop_ = true;
  uint8_t currentChannel_ = 1;
  uint32_t lastHopMs_ = 0;

  // Statistics counters
  volatile uint32_t totalPackets_ = 0;
  volatile uint32_t deauthCount_ = 0;
  volatile uint32_t disassocCount_ = 0;
  volatile uint32_t eapolCount_ = 0;
  volatile uint32_t beaconCount_ = 0;

  // Rates and attack detection
  uint32_t lastRateCalcMs_ = 0;
  uint32_t lastSecDeauthCount_ = 0;
  uint32_t lastSecTotalCount_ = 0;
  uint16_t deauthRate_ = 0;
  uint16_t packetRate_ = 0;
  bool attackAlert_ = false;
  uint32_t lastAttackTimeMs_ = 0;

  IncidentRecord lastIncident_{};
};
