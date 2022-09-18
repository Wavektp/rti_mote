#include "esp_comm.h"

const uint8_t brcst_addr[] = BROADCAST_MAC_ADDRESS;
uint8_t device_mac_addr[] = DEVICE_MAC_ADDRESS;

int msg_count = 0;
message_t incoming_rti;
message_t outgoing_rti;

void esp_comm::setup() {
  // Enable Serial
  Serial.begin(SERIAL_BAUDRATE);
  while (!Serial) {
  }
  // Enable WiFi and change MAC ADDRESS
  customize_mac_address();
  initESPNow();
  // Set watchdog timer
  esp_task_wdt_init(RESET_TIMEOUT, true);
  esp_task_wdt_add(NULL);
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

void esp_comm::initESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  } else {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }
  // register callbacks
  esp_now_register_send_cb(send_cb);
  esp_now_register_recv_cb(receive_cb);
  // add peer
  esp_now_peer_info_t peer_info = {};
  memcpy(&peer_info.peer_addr, brcst_addr, MAC_ADDR_SIZE);
  if (!esp_now_is_peer_exist(brcst_addr)) {
    if (esp_now_add_peer(&peer_info) != ESP_OK) {
      out("Fail to add peer");
    }
  }
}

void esp_comm::send(uint8_t* msg, size_t sz) {
  // send message
  esp_err_t res = esp_now_send(brcst_addr, msg, sz);
}

void esp_comm::macAddrToStr(const uint8_t* macAddr, char* str, int len) {
  snprintf(str, len, "%02x:%02x:%02x:%02x:%02x:%02x", macAddr[0], macAddr[1],
           macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
}

void receive_cb(const uint8_t* macAddr, const uint8_t* data, int len) {
  // memcpy(&incoming_rti, data, sizeof(incoming_rti));
  // char rti_message_str[RTI_STR_SIZE];
  // rtiMessageToStr(rti_message_str, RTI_STR_SIZE);
  // char macAddrStr[MAC_ADDR_STR_SIZE];
  // macAddrToStr(macAddr, macAddrStr, MAC_ADDR_STR_SIZE);

  // outf("Received message from: %s\r\n  %s\n", macAddrStr, rti_message_str);
  // // check functionality
  esp_task_wdt_reset();
}

void send_cb(const uint8_t* macAddr, esp_now_send_status_t st) {
  out("\r\n ESP-NOW Sent: ");
  outln(st == ESP_NOW_SEND_SUCCESS ? "SUCCESS" : "FAILED");
}

