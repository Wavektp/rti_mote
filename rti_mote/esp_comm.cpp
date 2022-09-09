#include "esp_comm.h"
#include <WiFi.h>
#include <esp_now.h>

uint8_t brAddr[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void esp_comm::setup() {
  // Enable Serial
  Serial.begin(SERIAL_BAUDRATE);
  while (!Serial) {}
  // Enable WiFi and Print MAC ADDRESS
  WiFi.mode(WIFI_STA);
  out("MAC ADDRESS: ");
  outln(WiFi.macAddress());

  
}
