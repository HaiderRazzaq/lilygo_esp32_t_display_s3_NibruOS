#pragma once
class DisplayManager;
class ResourceManager;
class WiFiService;
class SecurityService;
class SettingsService;
class TimeService;
class TotpService;
class UsbHidService;
class AccountImportService;
class VaultService;
class WebPortalService;
class BleHidService;
class WifiDefenseService;
class BleAdvertisingService;

struct AppContext {
  AppContext(DisplayManager& display, ResourceManager& resources, WiFiService& wifi,
             SecurityService& security, SettingsService& settings, TimeService& time,
             TotpService& totp, UsbHidService& usbHid, AccountImportService& accountImport,
             VaultService& vault, WebPortalService& webPortal, BleHidService& bleHid,
             WifiDefenseService& wifiDefense, BleAdvertisingService* bleAdvertising = nullptr)
      : display(display), resources(resources), wifi(wifi), security(security), settings(settings),
        time(time), totp(totp), usbHid(usbHid), accountImport(accountImport), vault(vault),
        webPortal(webPortal), bleHid(bleHid), wifiDefense(wifiDefense), bleAdvertising(bleAdvertising) {}

  DisplayManager& display;
  ResourceManager& resources;
  WiFiService& wifi;
  SecurityService& security;
  SettingsService& settings;
  TimeService& time;
  TotpService& totp;
  UsbHidService& usbHid;
  AccountImportService& accountImport;
  VaultService& vault;
  WebPortalService& webPortal;
  BleHidService& bleHid;
  WifiDefenseService& wifiDefense;
  BleAdvertisingService* bleAdvertising;
};
