#include "esp_comm.h"

node_t cSender;
confirmable_t conf;
timestamp_t stamp;
recv_cb_t recv_cb;
report_cb_t rep_cb;

message_t incoming;
message_t outgoing;

/******************************************************************************
 *  Callback Functions on Reception/Transmission *
 ******************************************************************************/
void receive(const uint8_t* macAddr, const uint8_t* data, int len);
void send_cb(const uint8_t* macAddr, esp_now_send_status_t st);
void promiscuous_rx_cb(void* buf, wifi_promiscuous_pkt_type_t type);

bool checkNeighbourP();

#if defined(ROOT_NODE)
byte msgID = 0;
#endif /*ROOT_NODE*/

const uint8_t brcst_addr[] = BROADCAST_MAC_ADDRESS;
uint8_t device_mac_addr[] = DEVICE_MAC_ADDRESS;

void esp_comm::begin(recv_cb_t recv, report_cb_t rep) {
  // Enable Serial

  Serial.begin(SERIAL_BAUDRATE);
  while (!Serial) {
  }
  outln("... initialize ESP-NOW");
// Enable WiFi and change MAC ADDRESS
  customize_mac_address();
  initESPNow();
  
  recv_cb = recv;
  rep_cb = rep;
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
  esp_now_register_send_cb(&send_cb);
  esp_now_register_recv_cb(&receive);
  // add peer
  esp_now_peer_info_t peer_info = {};
  memcpy(&peer_info.peer_addr, brcst_addr, MAC_ADDR_SIZE);
  if (!esp_now_is_peer_exist(brcst_addr)) {
    if (esp_now_add_peer(&peer_info) != ESP_OK) {
      out("ERROR:ESP-NOW: Fail to add peer");
    }
  }
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&promiscuous_rx_cb);
}

void esp_comm::send() {
  // send message
  if (((outgoing.type) & MESSAGE_INCOMPLETE_FLAG) == 0x00) {
    conf.isObserve = true;
    conf.msg_sz = sizeof(outgoing);
  }
#if defined(ROOT_NODE)
  if ((outgoing.type) == MESSAGE_TYPE_BEACON) {
    outgoing.msgID = msgID++;
  }
#endif /*ROOT_NODE*/
#if defined(END_DEVICE)
  if ((msg->type) == MESSAGE_TYPE_CONTENT) {
    msg->msgID = conf.msg_id;
  }
#endif /*END_DEVICE*/
  stamp = millis();
  esp_err_t res =
      esp_now_send(brcst_addr, (uint8_t*)&outgoing, sizeof(outgoing));
  Serial.print("\r\nESP_NOW Sent Status: ");
  if (res == ESP_OK) {
    Serial.println("SUCCESS");
  } else if (res == ESP_ERR_ESPNOW_NOT_INIT) {
    // How did we get so far!!
    Serial.println("ESPNOW not Init.");
  } else if (res == ESP_ERR_ESPNOW_ARG) {
    Serial.println("Invalid Argument");
  } else if (res == ESP_ERR_ESPNOW_INTERNAL) {
    Serial.println("Internal Error");
  } else if (res == ESP_ERR_ESPNOW_NO_MEM) {
    Serial.println("ESP_ERR_ESPNOW_NO_MEM");
  } else if (res == ESP_ERR_ESPNOW_NOT_FOUND) {
    Serial.println("Peer not found.");
  } else {
    Serial.println("Not sure what happened");
  }
}

void esp_comm::resend() {
  send();
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

void receive(const uint8_t* macAddr, const uint8_t* data, int len) {
  // reset watchdog
  esp_task_wdt_reset();
  // copy data to incoming message
  re("Received MSG:");
  memcpy(&incoming, data, sizeof(incoming));
  cSender.NID = incoming.sNID;
  cSender.DID = incoming.sDID;
  if (conf.isObserve) {  // Check next neighbour reception
    re("..on check:");
    if (conf.msg_id == incoming.msgID) {  // Check ID
      re("CORRECT ID..");
      if ((incoming.sNID == NEXT_NEIGHBOUR_NET_PREFIX) &&
          (incoming.sDID == NEXT_NEIGHBOUR_DEVICE_ID)) {  // Check sender
        reln("NEXT NEIGHBOUR CONFIRMED >> CONFIRMED RECEPTION");
        conf.isObserve = false;
      }
    }
  }
  recv_cb(&incoming);
  // char rti_message_str[RTI_STR_SIZE];
  // rtiMessageToStr(rti_message_str, RTI_STR_SIZE);
  // char macAddrStr[MAC_ADDR_STR_SIZE];
  // macAddrToStr(macAddr, macAddrStr, MAC_ADDR_STR_SIZE);

  // outf("Received message from: %s\r\n  %s\n", macAddrStr, rti_message_str);
  // // check functionality
}

void send_cb(const uint8_t* macAddr, esp_now_send_status_t st) {
  // out("\r\nESP-NOW Sent: ");
  // outln(st == ESP_NOW_SEND_SUCCESS ? "SUCCESS\n" : "FAILED\n");
}

void promiscuous_rx_cb(void* buf, wifi_promiscuous_pkt_type_t type) {
  // All espnow traffic uses action frames which are a subtype of the mgmnt
  // frames so filter out everything else.
  if (type != WIFI_PKT_MGMT)
    return;

  const wifi_promiscuous_pkt_t* ppkt = (wifi_promiscuous_pkt_t*)buf;
  int rssi = ppkt->rx_ctrl.rssi;
  repf("WIFI CALLBACK: Retrieving RSSI: %02d, ", rssi);
  rep_cb(rssi);
}

node_t* esp_comm::getCurrentSender() {
  return &cSender;
}