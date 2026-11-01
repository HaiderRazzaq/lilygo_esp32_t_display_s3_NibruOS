#pragma once
class DisplayManager;
class ResourceManager;
class WiFiService;
class SecurityService;
class SettingsService;

struct AppContext {
  DisplayManager& display;
  ResourceManager& resources;
  WiFiService& wifi;
  SecurityService& security;
  SettingsService& settings;
};
