#include "esp_comm.h"
#include "esp_wifi.h"

const uint8_t brcst_addr[] = BROADCAST_MAC_ADDRESS;
uint8_t device_mac_addr[] = {PLACEHOLDER_CODE, PLACEHOLDER_CODE,
                             PLACEHOLDER_CODE, PLACEHOLDER_CODE,
                             NET_PREFIX,       DEVICE_ID};

void esp_comm::setup() {
  // Enable Serial
  Serial.begin(SERIAL_BAUDRATE);
  while (!Serial) {}
  // Enable WiFi and change MAC ADDRESS
  customize_mac_address();
  
}

void esp_comm::customize_mac_address() {
  // Enable WIFI
  WiFi.mode(WIFI_STA);
  out("MAC ADDRESS: ");
  outln(WiFi.macAddress());
  // Customize MAC ADDRESS
  outln("Customize MAC ADDRESS...");
  esp_wifi_set_mac(WIFI_IF_STA, &device_mac_addr[0]);
  out("[NEW] MAC ADDRESS: ");
  outln(WiFi.macAddress());
}
