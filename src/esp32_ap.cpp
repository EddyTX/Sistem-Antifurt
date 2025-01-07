#include "esp32_ap.h"
#include <WiFi.h>

// AP credentials
const char* ssid = "ESP32_Network";
const char* password = "123456789";

// AP Initialization Function
void createESP32AccessPoint() {
  // Configure and start the Access Point
  WiFi.softAP(ssid, password, 6, 0);
  Serial.println("Access Point started!");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("Password: ");
  Serial.println(password);

  // Print the ESP32's IP address
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.softAPIP());
}