#include "core/AppManager.h"
#include "BoardConfig.h"
#include "core/AppContext.h"
#include "core/ResourceManager.h"
#include "hardware/DisplayManager.h"
#include "hardware/TouchManager.h"
#include "hardware/ButtonManager.h"
#include "services/WiFiService.h"
#include "apps/SystemInfoApp.h"
#include "apps/TouchTestApp.h"
#include "apps/WiFiScannerApp.h"
#include <Arduino.h>
#include <WiFi.h>
#include <cassert>
#include <cstdio>
uint32_t fakeMs=100;
int fakePins[64]{};
MockSerial Serial;
MockESP ESP;
MockWiFi WiFi;
bool fakeTouchPending=false;
TouchData fakeTouchData{};

int main() {
  for(auto& p:fakePins)p=HIGH;
  // Verify tested Touch Glass landscape conversion and gesture mapping.
  TouchManager touchDriver;
  touchDriver.begin();
  fakeTouchData.x=42;
  fakeTouchData.y=75;
  fakeTouchData.gestureID=SWIPE_DOWN;
  fakeTouchPending=true;
  fakeMs+=200;
  TouchEvent touchEvent{};
  assert(touchDriver.poll(touchEvent));
  assert(touchEvent.x==75 && touchEvent.y==127);
  assert(touchEvent.gesture==TouchGesture::SwipeRight); // native DOWN -> visible RIGHT
  assert(!touchDriver.poll(touchEvent));
  // Other directions are likewise rotated for landscape navigation.
  struct GestureCase { byte native; TouchGesture landscape; };
  const GestureCase cases[] = {
    {SWIPE_UP, TouchGesture::SwipeLeft},
    {SWIPE_LEFT, TouchGesture::SwipeDown},
    {SWIPE_RIGHT, TouchGesture::SwipeUp},
    {SINGLE_CLICK, TouchGesture::Tap},
  };
  for (const auto& c : cases) {
    fakeMs += 200;
    fakeTouchData.gestureID = c.native;
    fakeTouchPending = true;
    assert(touchDriver.poll(touchEvent));
    assert(touchEvent.gesture == c.landscape);
  }
  // Physical buttons: BOOT short changes selection, BOOT long activates;
  // GPIO14 short must provide escape.
  ButtonManager keyDriver;
  keyDriver.begin();
  fakeMs+=100;
  fakePins[0]=LOW;
  assert(keyDriver.poll(fakeMs)==ButtonAction::None);
  fakeMs+=90;
  fakePins[0]=HIGH;
  assert(keyDriver.poll(fakeMs)==ButtonAction::SelectNext);
  fakeMs+=90;
  fakePins[0]=LOW;
  keyDriver.poll(fakeMs);
  fakeMs+=700;
  assert(keyDriver.poll(fakeMs)==ButtonAction::Activate);
  fakeMs+=90;
  fakePins[0]=HIGH;
  assert(keyDriver.poll(fakeMs)==ButtonAction::None);
  fakeMs+=90;
  fakePins[14]=LOW;
  keyDriver.poll(fakeMs);
  fakeMs+=90;
  fakePins[14]=HIGH;
  assert(keyDriver.poll(fakeMs)==ButtonAction::Back);

  DisplayManager display;
  ResourceManager resources;
  WiFiService wifi;
  AppContext context{display, resources, wifi};
  AppManager mgr(context);
  SystemInfoApp info;
  TouchTestApp touch;
  WiFiScannerApp scanner;
  assert(mgr.registerApp(info));
  assert(mgr.registerApp(touch));
  assert(mgr.registerApp(scanner));
  mgr.begin();
  mgr.tick(fakeMs);
  assert(mgr.isHome() && mgr.appCount()==3);
  assert(resources.wifiOwner()==ResourceOwner::None);
  // Home selection and open.
  mgr.handleButton(ButtonAction::SelectNext);
  assert(mgr.selectedIndex()==1);
  mgr.handleButton(ButtonAction::Activate);
  assert(!mgr.isHome());
  mgr.handleTouch(TouchEvent{123,100,70,123,TouchGesture::SwipeLeft});
  mgr.tick(fakeMs);
  mgr.handleButton(ButtonAction::Back);
  assert(mgr.isHome());
  // Touch tap opens the Wi-Fi tile (third tile in landscape).
  mgr.handleTouch(TouchEvent{255,88,81,255,TouchGesture::Tap});
  assert(!mgr.isHome());
  assert(resources.ownsWiFi(ResourceOwner::WiFiScanner));
  assert(wifi.state()==ScanState::Preparing);
  assert(WiFi.modeValue==WIFI_STA);
  assert(WiFi.started==0);  // must not scan during STA startup
  mgr.tick(fakeMs+Board::kWiFiWarmupMs-1);
  assert(WiFi.started==0);
  mgr.tick(fakeMs+Board::kWiFiWarmupMs);
  assert(wifi.state()==ScanState::Scanning && WiFi.started==1);
  WiFi.scans=2;
  WiFi.complete=true;
  mgr.tick(fakeMs+Board::kWiFiWarmupMs+20);
  assert(wifi.state()==ScanState::Complete);
  assert(wifi.count()==2);
  const NetworkRecord* record=wifi.network(0);
  assert(record && record->rssi==-62 && record->channel==6 && record->secured);
  // Rescan through app onPrimaryAction; app exit stops an in-flight HW scan.
  mgr.handleButton(ButtonAction::SelectNext);
  assert(wifi.state()==ScanState::Preparing);
  mgr.tick(fakeMs+Board::kWiFiWarmupMs);
  assert(wifi.state()==ScanState::Scanning);
  const int stoppedBeforeExit=WiFi.stopped;
  mgr.handleButton(ButtonAction::Back);
  assert(mgr.isHome() && WiFi.modeValue==WIFI_OFF);
  assert(WiFi.stopped==stoppedBeforeExit+1);
  assert(resources.wifiOwner()==ResourceOwner::None);
  // Timeout of first attempt leads to automatic reset and second attempt.
  assert(wifi.startScan(fakeMs));
  wifi.tick(fakeMs+Board::kWiFiWarmupMs);
  assert(wifi.state()==ScanState::Scanning && wifi.attempt()==1);
  wifi.tick(fakeMs+Board::kWiFiWarmupMs+Board::kScanTimeoutMs+1);
  assert(wifi.state()==ScanState::Preparing && wifi.attempt()==2);
  const uint32_t retryAt=fakeMs+Board::kWiFiWarmupMs+Board::kScanTimeoutMs+1;
  wifi.tick(retryAt+Board::kWiFiWarmupMs);
  assert(wifi.state()==ScanState::Scanning && wifi.attempt()==2);
  wifi.tick(retryAt+Board::kWiFiWarmupMs+Board::kScanTimeoutMs+1);
  assert(wifi.state()==ScanState::Failed && WiFi.modeValue==WIFI_OFF);
  assert(std::strcmp(wifi.lastError(), "Scan timed out") == 0);
  wifi.shutdown();
  // The retry must also be able to recover, not just to fail twice.
  assert(wifi.startScan(fakeMs));
  WiFi.complete=false;
  wifi.tick(fakeMs+Board::kWiFiWarmupMs);
  wifi.tick(fakeMs+Board::kWiFiWarmupMs+Board::kScanTimeoutMs+1);
  assert(wifi.state()==ScanState::Preparing && wifi.attempt()==2);
  WiFi.complete=false;
  wifi.tick(fakeMs+Board::kScanTimeoutMs+Board::kWiFiWarmupMs*2+1);
  assert(wifi.state()==ScanState::Scanning);
  WiFi.complete=true;
  WiFi.scans=1;
  wifi.tick(fakeMs+Board::kScanTimeoutMs+Board::kWiFiWarmupMs*2+2);
  assert(wifi.state()==ScanState::Complete && wifi.count()==1);
  wifi.shutdown();
  // Completion should win even if poll was delayed past the local deadline.
  assert(wifi.startScan(fakeMs));
  WiFi.complete=false;
  wifi.tick(fakeMs+Board::kWiFiWarmupMs);
  WiFi.complete=true;
  wifi.tick(fakeMs+Board::kWiFiWarmupMs+Board::kScanTimeoutMs+10);
  assert(wifi.state()==ScanState::Complete);
  wifi.shutdown();
  // Failure paths: radio initialization, scan initiation and completion.
  WiFi.canMode=false;
  assert(!wifi.startScan(fakeMs));
  assert(wifi.state()==ScanState::Failed);
  WiFi.canMode=true;
  WiFi.failStart=true;
  assert(wifi.startScan(fakeMs));
  wifi.tick(fakeMs+Board::kWiFiWarmupMs);
  assert(wifi.state()==ScanState::Preparing && wifi.attempt()==2);
  wifi.tick(fakeMs+Board::kWiFiWarmupMs*2);
  assert(wifi.state()==ScanState::Failed && WiFi.modeValue==WIFI_OFF);
  WiFi.failStart=false;
  assert(wifi.startScan(fakeMs));
  WiFi.failComplete=true;
  wifi.tick(fakeMs+Board::kWiFiWarmupMs);
  wifi.tick(fakeMs+Board::kWiFiWarmupMs+1);
  assert(wifi.state()==ScanState::Preparing && wifi.attempt()==2);
  wifi.tick(fakeMs+Board::kWiFiWarmupMs*2+1);
  wifi.tick(fakeMs+Board::kWiFiWarmupMs*2+2);
  assert(wifi.state()==ScanState::Failed && WiFi.modeValue==WIFI_OFF);
  WiFi.failComplete=false;
  wifi.shutdown();
  // Protect from unauthorized resource release.
  assert(resources.acquireWiFi(ResourceOwner::WiFiScanner));
  resources.releaseWiFi(ResourceOwner::None);
  assert(resources.ownsWiFi(ResourceOwner::WiFiScanner));
  resources.releaseWiFi(ResourceOwner::WiFiScanner);
  // Upper-left header tap returns Home from any app.
  assert(mgr.open(0));
  mgr.handleTouch(TouchEvent{12,12,0,0,TouchGesture::Tap});
  assert(mgr.isHome());
  // Register a fourth app and verify pagination in the Home shell.
  struct DummyApp : App {
    const char* title() const override {return "Dummy";}
    void onEnter(AppContext&) override {invalidate();}
    void onExit(AppContext&) override {}
    void onTick(AppContext&, uint32_t) override {}
    void onTouch(AppContext&, const TouchEvent&) override {}
    void render(AppContext&) override {}
  } dummy;
  assert(mgr.registerApp(dummy));
  mgr.handleTouch(TouchEvent{200, 90, 0, 0, TouchGesture::SwipeLeft});
  assert(mgr.selectedIndex()==3);
  mgr.handleTouch(TouchEvent{20, 90, 0, 0, TouchGesture::Tap});
  assert(!mgr.isHome() && mgr.selectedIndex()==3);
  mgr.goHome();
  assert(mgr.isHome());
  puts("PASS: host navigation/resource/async scan/timeout smoke tests");
}
