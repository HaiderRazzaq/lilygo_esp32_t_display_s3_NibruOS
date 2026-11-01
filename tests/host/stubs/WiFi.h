#pragma once
#include <Arduino.h>
#include <cstring>

static constexpr int WIFI_STA = 1, WIFI_OFF = 0, WIFI_AUTH_OPEN = 0;
static constexpr int WIFI_ALL_CHANNEL_SCAN = 0, WIFI_CONNECT_AP_BY_SIGNAL = 0;
using WiFiEvent_t = int;
using wifi_err_reason_t = int;
static constexpr int ARDUINO_EVENT_WIFI_STA_DISCONNECTED = 1;
static constexpr int ARDUINO_EVENT_WIFI_STA_CONNECTED = 2;
static constexpr int ARDUINO_EVENT_WIFI_STA_GOT_IP = 3;
static constexpr int WIFI_REASON_AUTH_FAIL = 202, WIFI_REASON_NO_AP_FOUND = 201;
struct WiFiEventInfo_t { struct { uint8_t reason; } wifi_sta_disconnected; };
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
  uint8_t bytes[4]{192, 168, 1, 100};
  uint8_t operator[](int i) const { return (i >= 0 && i < 4) ? bytes[i] : 0; }
  String toString() const { return String("192.168.1.100"); }
};

struct MockWiFi {
  template<class T> void onEvent(T) {}
  const char* disconnectReasonName(wifi_err_reason_t) { return "Mock reason"; }
  int modeValue = 0;
  int started = 0;
  int stopped = 0;
  bool persistentSetting = true;
  bool sleepEnabled = true;
  bool autoReconnectSetting = true;
  void persistent(bool value) { persistentSetting = value; }
  void setSleep(bool value) { sleepEnabled = value; }
  void setMinSecurity(int) {}
  void setScanMethod(int) {}
  void setSortMethod(int) {}
  void setAutoReconnect(bool val) { autoReconnectSetting = val; }
  bool disconnect(bool, bool) {
    statusValue = WL_DISCONNECTED;
    return true;
  }
  int deleted = 0;
  int scans = 0;
  bool complete = false;
  bool canMode = true;
  bool failStart = false;
  bool failComplete = false;

  wl_status_t statusValue = WL_IDLE_STATUS;
  char lastConnectSsid[33]{};
  char lastConnectPass[64]{};

  wl_status_t begin(const char* ssid, const char* pass = nullptr) {
    if (ssid) strncpy(lastConnectSsid, ssid, sizeof(lastConnectSsid) - 1);
    if (pass) strncpy(lastConnectPass, pass, sizeof(lastConnectPass) - 1);
    else lastConnectPass[0] = '\0';
    statusValue = WL_IDLE_STATUS;
    return statusValue;
  }

  wl_status_t status() { return statusValue; }
  IPAddress localIP() {
    IPAddress ip;
    if (statusValue != WL_CONNECTED) for (auto& b : ip.bytes) b = 0;
    return ip;
  }
  IPAddress gatewayIP() { return IPAddress{}; }
  String macAddress() { return String("44:1B:F6:D3:AA:BC"); }

  bool mode(int value) {
    if (value == WIFI_STA && !canMode) return false;
    modeValue = value;
    return true;
  }
  void scanDelete() { ++deleted; }
  int scanNetworks(bool async, bool = false, bool = false, uint32_t = 300) {
    if (async) ++started;
    complete = false;
    return failStart ? WIFI_SCAN_FAILED : WIFI_SCAN_RUNNING;
  }
  int scanComplete() {
    if (failComplete) return WIFI_SCAN_FAILED;
    return complete ? scans : WIFI_SCAN_RUNNING;
  }
  String SSID(int) { return String("ExampleNet"); }
  int RSSI(int = 0) { return -62; }
  int channel(int = 0) { return 6; }
  int encryptionType(int) { return 3; }
};
extern MockWiFi WiFi;
