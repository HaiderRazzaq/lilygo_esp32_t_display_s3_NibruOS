#pragma once
#include <Arduino.h>
static constexpr int WIFI_STA=1, WIFI_OFF=0, WIFI_AUTH_OPEN=0;
static constexpr int WIFI_SCAN_RUNNING=-1, WIFI_SCAN_FAILED=-2;
struct MockWiFi {
  int modeValue=0;
  int started=0;
  int stopped=0;
  bool persistentSetting=true;
  bool sleepEnabled=true;
  void persistent(bool value) { persistentSetting=value; }
  void setSleep(bool value) { sleepEnabled=value; }
  bool disconnect(bool, bool) { return true; }
  int deleted=0;
  int scans=0;
  bool complete=false;
  bool canMode=true;
  bool failStart=false;
  bool failComplete=false;
  bool mode(int value){
    if (value == WIFI_STA && !canMode) return false;
    modeValue=value;
    return true;
  }
  void scanDelete(){++deleted;}
  int scanNetworks(bool async, bool=false, bool=false, uint32_t=300){
    if (async) ++started;
    complete=false;
    return failStart ? WIFI_SCAN_FAILED : WIFI_SCAN_RUNNING;
  }
  int scanComplete(){
    if (failComplete) return WIFI_SCAN_FAILED;
    return complete ? scans : WIFI_SCAN_RUNNING;
  }
  String SSID(int){return String("ExampleNet");}
  int RSSI(int){return -62;}
  int channel(int){return 6;}
  int encryptionType(int){return 3;}
};
extern MockWiFi WiFi;
