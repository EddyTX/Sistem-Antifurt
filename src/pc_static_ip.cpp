#include "pc_static_ip.h"
#include <WiFi.h>

void configureStaticIPForPC() {
  // Set up the static IP configuration for the PC
  IPAddress local_IP(192, 168, 4, 1);   // ESP32's own IP (Gateway)
  IPAddress gateway(192, 168, 4, 1);   // Gateway address (ESP32)
  IPAddress subnet(255, 255, 255, 0);  // Subnet mask

  if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
    Serial.println("Failed to configure static IP for the PC");
  } else {
    Serial.println("Static IP configured for PC. Ensure PC uses 192.168.4.x range.");
  }
}