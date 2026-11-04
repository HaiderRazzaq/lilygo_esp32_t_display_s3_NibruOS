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

struct AppContext {
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
};
