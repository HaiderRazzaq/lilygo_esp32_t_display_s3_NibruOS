#include "services/WifiDefenseService.h"
#include <cstdio>
#include <cstring>

#if defined(ARDUINO_ARCH_ESP32) && !defined(UNIT_TEST)
#include <WiFi.h>
#include <esp_wifi.h>

static WifiDefenseService* s_activeDefenseService = nullptr;

static void IRAM_ATTR defensePromiscuousCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
  if (!s_activeDefenseService || s_activeDefenseService->isPaused()) return;
  const wifi_promiscuous_pkt_t* pkt = static_cast<const wifi_promiscuous_pkt_t*>(buf);
  const wifi_pkt_rx_ctrl_t& ctrl = pkt->rx_ctrl;
  s_activeDefenseService->processFrame(pkt->payload, ctrl.sig_len, ctrl.rssi, ctrl.channel, millis());
}
#endif

void WifiDefenseService::begin() {
  resetStats();
}

bool WifiDefenseService::start(uint8_t initialChannel, bool autoHop, uint32_t nowMs) {
  currentChannel_ = (initialChannel >= 1 && initialChannel <= 13) ? initialChannel : 1;
  autoHop_ = autoHop;
  lastHopMs_ = nowMs;
  lastRateCalcMs_ = nowMs;
  paused_ = false;

#if defined(ARDUINO_ARCH_ESP32) && !defined(UNIT_TEST)
  WiFi.persistent(false);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  esp_wifi_set_promiscuous(false);

  s_activeDefenseService = this;
  esp_wifi_set_promiscuous_rx_cb(&defensePromiscuousCallback);

  wifi_promiscuous_filter_t filter{};
  filter.filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA;
  esp_wifi_set_promiscuous_filter(&filter);

  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(currentChannel_, WIFI_SECOND_CHAN_NONE);
  Serial.printf("[WifiDefense] Sniffer started on channel %u (autoHop=%d)\n", currentChannel_, autoHop_ ? 1 : 0);
#endif

  running_ = true;
  return true;
}

void WifiDefenseService::stop() {
  if (!running_) return;

#if defined(ARDUINO_ARCH_ESP32) && !defined(UNIT_TEST)
  esp_wifi_set_promiscuous(false);
  esp_wifi_set_promiscuous_rx_cb(nullptr);
  s_activeDefenseService = nullptr;
  WiFi.mode(WIFI_OFF);
  Serial.println("[WifiDefense] Sniffer stopped");
#endif

  running_ = false;
  paused_ = false;
  attackAlert_ = false;
}

void WifiDefenseService::setChannel(uint8_t ch) {
  if (ch < 1 || ch > 13) return;
  currentChannel_ = ch;
#if defined(ARDUINO_ARCH_ESP32) && !defined(UNIT_TEST)
  if (running_) {
    esp_wifi_set_channel(currentChannel_, WIFI_SECOND_CHAN_NONE);
  }
#endif
}

void WifiDefenseService::setAutoHop(bool enabled) {
  autoHop_ = enabled;
}

void WifiDefenseService::cycleChannelMode() {
  if (autoHop_) {
    // Switch to Lock Ch 1
    autoHop_ = false;
    setChannel(1);
  } else if (currentChannel_ == 1) {
    setChannel(6);
  } else if (currentChannel_ == 6) {
    setChannel(11);
  } else {
    // Return to Auto-Hop
    autoHop_ = true;
  }
}

void WifiDefenseService::resetStats() {
  totalPackets_ = 0;
  deauthCount_ = 0;
  disassocCount_ = 0;
  eapolCount_ = 0;
  beaconCount_ = 0;
  deauthRate_ = 0;
  packetRate_ = 0;
  lastSecDeauthCount_ = 0;
  lastSecTotalCount_ = 0;
  attackAlert_ = false;
  lastAttackTimeMs_ = 0;
  lastIncident_ = IncidentRecord{};
}

void WifiDefenseService::processFrame(const uint8_t* payload, uint16_t length, int8_t rssi, uint8_t channel, uint32_t nowMs) {
  if (paused_ || !payload || length < 24) return;

  totalPackets_++;
  const uint8_t fc0 = payload[0];
  const uint8_t frameType = (fc0 & 0x0C) >> 2;
  const uint8_t frameSubtype = (fc0 & 0xF0) >> 4;

  // 1. Management Frames (Type == 0)
  if (frameType == 0) {
    // Beacon frame (Subtype 8 = 0x08)
    if (frameSubtype == 8) {
      beaconCount_++;
      return;
    }

    // Deauthentication (Subtype 12 = 0x0C) or Disassociation (Subtype 10 = 0x0A)
    const bool isDeauth = (frameSubtype == 12);
    const bool isDisassoc = (frameSubtype == 10);

    if (isDeauth || isDisassoc) {
      if (isDeauth) deauthCount_++;
      else disassocCount_++;

      // Extract Addresses:
      // Addr1 (DA - Destination): payload + 4
      // Addr2 (SA - Source):      payload + 10
      // Addr3 (BSSID):            payload + 16
      memcpy(lastIncident_.targetMac, payload + 4, 6);
      memcpy(lastIncident_.sourceMac, payload + 10, 6);
      memcpy(lastIncident_.bssid, payload + 16, 6);

      // Reason Code is 2 bytes at payload + 24 (little-endian)
      if (length >= 26) {
        lastIncident_.reasonCode = static_cast<uint16_t>(payload[24] | (payload[25] << 8));
      } else {
        lastIncident_.reasonCode = 0;
      }

      lastIncident_.rssi = rssi;
      lastIncident_.channel = channel != 0 ? channel : currentChannel_;
      lastIncident_.timestampMs = nowMs;
      lastIncident_.isDeauth = isDeauth;
      lastAttackTimeMs_ = nowMs;
    }
  }
  // 2. Data Frames (Type == 2) -> EAPOL / 4-Way Handshake Sniffing
  else if (frameType == 2) {
    // Look for LLC/SNAP 0xAA 0xAA 0x03 followed by EtherType 0x88 0x8E
    if (length >= 32) {
      for (uint16_t i = 24; i + 8 <= length && i <= 36; ++i) {
        if (payload[i] == 0xAA && payload[i + 1] == 0xAA && payload[i + 2] == 0x03) {
          if (payload[i + 6] == 0x88 && payload[i + 7] == 0x8E) {
            eapolCount_++;
            break;
          }
        }
      }
    }
  }
}

void WifiDefenseService::tick(uint32_t nowMs) {
  if (!running_ || paused_) return;

  // 1. Channel Hopping (every 300 ms if autoHop is enabled)
  if (autoHop_) {
    if (nowMs - lastHopMs_ >= 300) {
      lastHopMs_ = nowMs;
      uint8_t nextCh = (currentChannel_ % 13) + 1;
      setChannel(nextCh);
    }
  }

  // 2. Attack Rate & Alert Calculation (once per second)
  if (nowMs - lastRateCalcMs_ >= 1000) {
    const uint32_t curDeauths = deauthCount_ + disassocCount_;
    const uint32_t curTotal = totalPackets_;

    deauthRate_ = static_cast<uint16_t>(curDeauths >= lastSecDeauthCount_ ? (curDeauths - lastSecDeauthCount_) : 0);
    packetRate_ = static_cast<uint16_t>(curTotal >= lastSecTotalCount_ ? (curTotal - lastSecTotalCount_) : 0);

    lastSecDeauthCount_ = curDeauths;
    lastSecTotalCount_ = curTotal;
    lastRateCalcMs_ = nowMs;

    // Trigger alert if rate >= 3 packets/sec or active attack within last 3 seconds
    if (deauthRate_ >= 3 || (lastAttackTimeMs_ > 0 && nowMs - lastAttackTimeMs_ < 3500)) {
      attackAlert_ = true;
    } else {
      attackAlert_ = false;
    }
  }
}

void WifiDefenseService::formatMac(const uint8_t* mac, char* buffer, size_t maxLen) {
  if (!mac || !buffer || maxLen < 18) return;
  snprintf(buffer, maxLen, "%02X:%02X:%02X:%02X:%02X:%02X",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

const char* WifiDefenseService::reasonString(uint16_t code) {
  switch (code) {
    case 1:  return "Unspecified";
    case 2:  return "Prev Auth Invalid";
    case 3:  return "STA Left BSS";
    case 4:  return "Inactivity";
    case 6:  return "Class 2 (Unauth)";
    case 7:  return "Class 3 (Unassoc)";
    case 8:  return "Disassoc Leaving";
    case 15: return "4-Way HS Timeout";
    default: return "Reason Unknown";
  }
}
