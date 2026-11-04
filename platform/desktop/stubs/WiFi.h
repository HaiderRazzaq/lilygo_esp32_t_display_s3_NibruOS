#pragma once
#include "Arduino.h"
#include "platform/desktop/MockWiFiService.h"
#include <cstring>

static constexpr int WIFI_STA = 1, WIFI_OFF = 0, WIFI_AUTH_OPEN = 0;
static constexpr int WIFI_ALL_CHANNEL_SCAN = 0, WIFI_CONNECT_AP_BY_SIGNAL = 0;
using WiFiEvent_t = int;
using wifi_err_reason_t = int;

static constexpr int ARDUINO_EVENT_WIFI_STA_DISCONNECTED = 1;
static constexpr int ARDUINO_EVENT_WIFI_STA_CONNECTED = 2;
static constexpr int ARDUINO_EVENT_WIFI_STA_GOT_IP = 3;
static constexpr int WIFI_REASON_AUTH_FAIL = 202, WIFI_REASON_NO_AP_FOUND = 201;

struct WiFiEventInfo_t {
  struct { uint8_t reason; } wifi_sta_disconnected;
};

static constexpr int WIFI_SCAN_RUNNING = -1, WIFI_SCAN_FAILED = -2;

using wl_status_t = int;
static constexpr wl_status_t WL_NO_SHIELD = 255;
static constexpr wl_status_t WL_IDLE_STATUS = 0;
static constexpr wl_status_t WL_NO_SSID_AVAIL = 1;
static constexpr wl_status_t WL_SCAN_COMPLETED = 2;
static constexpr wl_status_t WL_CONNECTED = 3;
static constexpr wl_status_t WL_CONNECT_FAILED = 4;
static constexpr wl_status_t WL_CONNECTION_LOST = 5;
static constexpr wl_status_t WL_DISCONNECTED = 6;

struct IPAddress {
  uint8_t bytes[4]{192, 168, 1, 105};
  uint8_t operator[](int i) const { return (i >= 0 && i < 4) ? bytes[i] : 0; }
  String toString() const { return String("192.168.1.105"); }
};

struct MockWiFi {
  template<class T> void onEvent(T) {}
  const char* disconnectReasonName(wifi_err_reason_t) { return "Simulated reason"; }

  void persistent(bool) {}
  bool sleepEnabled = true;
  void setSleep(bool enabled) { sleepEnabled = enabled; }
  void setMinSecurity(int) {}
  void setScanMethod(int) {}
  void setSortMethod(int) {}
  void setAutoReconnect(bool) {}

  bool disconnect(bool = false, bool = false) {
    MockWiFiService::instance().onDisconnect();
    return true;
  }

  bool mode(int m) {
    return MockWiFiService::instance().onMode(m);
  }

  int scanNetworks(bool async, bool = false, bool = false, uint32_t = 300) {
    return MockWiFiService::instance().onScanNetworks(async, millis());
  }

  int scanComplete() {
    return MockWiFiService::instance().onScanComplete(millis());
  }

  void scanDelete() {
    MockWiFiService::instance().onScanDelete();
  }

  String SSID(int idx) {
    return String(MockWiFiService::instance().getSSID(idx));
  }

  int RSSI(int idx = 0) {
    return MockWiFiService::instance().getRSSI(idx);
  }

  int channel(int idx = 0) {
    return MockWiFiService::instance().getChannel(idx);
  }

  int encryptionType(int idx) {
    return MockWiFiService::instance().getEncryptionType(idx);
  }

  wl_status_t begin(const char* ssid, const char* pass = nullptr) {
    MockWiFiService::instance().onConnectBegin(ssid, pass, millis());
    return WL_IDLE_STATUS;
  }

  wl_status_t status() {
    return MockWiFiService::instance().getConnectStatus(millis());
  }

  IPAddress localIP() {
    if (status() != WL_CONNECTED) {
      IPAddress zero;
      zero.bytes[0] = zero.bytes[1] = zero.bytes[2] = zero.bytes[3] = 0;
      return zero;
    }
    return IPAddress{};
  }

  IPAddress gatewayIP() {
    IPAddress gw;
    gw.bytes[0] = 192; gw.bytes[1] = 168; gw.bytes[2] = 1; gw.bytes[3] = 1;
    return gw;
  }

  String macAddress() {
    return String("44:1B:F6:D3:AA:BC");
  }
};

inline MockWiFi WiFi;
