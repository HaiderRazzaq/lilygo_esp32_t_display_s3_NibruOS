#pragma once
class DisplayManager;
class ResourceManager;
class WiFiService;

struct AppContext {
  DisplayManager& display;
  ResourceManager& resources;
  WiFiService& wifi;
};
