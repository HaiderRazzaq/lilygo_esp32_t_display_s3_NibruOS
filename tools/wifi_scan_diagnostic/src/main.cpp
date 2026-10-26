// Temporary diagnostic sketch for T-Display-S3 Touch Glass (Arduino IDE).
// WARNING: uploading it REPLACES the NibruOS firmware until you flash it again.
// The test deliberately uses a blocking scan to isolate radio+driver from
// NibruOS' asynchronous polling/app lifecycle. No Wi-Fi password is needed.
#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>

void setup() {
  Serial.begin(115200);
  delay(1500);  // give USB CDC a chance to appear
  pinMode(15, OUTPUT); digitalWrite(15, HIGH);
  Serial.println("\n[Diag] T-Display-S3 Wi-Fi scan, Arduino core");
  Serial.printf("[Diag] Chip: %s | Free heap: %u | PSRAM: %u\n",
                ESP.getChipModel(), ESP.getFreeHeap(), ESP.getPsramSize());
  WiFi.persistent(false);
  bool ok = WiFi.mode(WIFI_STA);
  Serial.printf("[Diag] mode(STA)=%d\n", ok);
  WiFi.setSleep(false);
  WiFi.disconnect(false, false);
  delay(1000);
}

void loop() {
  Serial.println("[Diag] Starting synchronous scan (2.4GHz only)");
  unsigned long before = millis();
  int n = WiFi.scanNetworks(false, false, false, 300);
  Serial.printf("[Diag] result=%d, duration=%lu ms, WiFi mode=%d\n",
                n, millis()-before, WiFi.getMode());
  if (n >= 0) {
    for (int i=0; i<n && i<15; ++i) {
      Serial.printf("[Diag] #%d SSID=%s RSSI=%d dBm channel=%d\n",
                    i+1, WiFi.SSID(i).c_str(), WiFi.RSSI(i), WiFi.channel(i));
    }
  } else {
    Serial.println("[Diag] synchronous scan FAILED. Check power, firmware, RF path.");
  }
  WiFi.scanDelete();
  delay(5000);
}
