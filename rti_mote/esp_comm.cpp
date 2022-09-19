#include "esp_comm.h"

#ifdef ROOT_NODE
byte msgID = 0;
#endif /*ROOT_NODE*/

const uint8_t brcst_addr[] = BROADCAST_MAC_ADDRESS;
uint8_t device_mac_addr[] = DEVICE_MAC_ADDRESS;

message_t incoming;
message_t outgoing;

void esp_comm::setup() {
  // Enable Serial
  Serial.begin(SERIAL_BAUDRATE);
  while (!Serial) {
  }
  // Enable WiFi and change MAC ADDRESS
  customize_mac_address();
  initESPNow();
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

void esp_comm::send(message_t* msg, size_t sz) {
  // send message
  if (((msg->type) & MESSAGE_INCOMPLETE_FLAG) == 0x00) {
    conf.isObserve = true;
    conf.msg_sz = sz;
    conf.expect.NID = msg->nNID;
    conf.expect.DID = msg->nDID;
  }
#ifdef ROOT_NODE
  if ((msg->type) == MESSAGE_TYPE_BEACON) {
    msg->msgID = msgID++;
  }
#endif /*ROOT_NODE*/
#ifdef END_DEVICE
  if ((msg->type) == MESSAGE_TYPE_CONTENT) {
    msg->msgID = conf.msg_id;
  }
#endif /*END_DEVICE*/

  esp_err_t res = esp_now_send(brcst_addr, (uint8_t*)msg, sz);
}

void esp_comm::resend() {
  esp_err_t res = esp_now_send(brcst_addr, (uint8_t*)&outgoing, conf.msg_sz);
}

void esp_comm::macAddrToStr(const uint8_t* macAddr, char* str, int len) {
  snprintf(str, len, "%02x:%02x:%02x:%02x:%02x:%02x", macAddr[0], macAddr[1],
           macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
}

bool esp_comm::checkTimeout(timestamp_t timeout) {
  if (conf.isObserve) {
    return ((millis() - stamp) > timeout);
  }
  return false;
}
message_t* esp_comm::get_incoming() {
  return &incoming;
}

message_t* esp_comm::get_outgoing() {
  return &outgoing;
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
