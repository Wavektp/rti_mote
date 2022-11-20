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

const uint8_t brcst_addr[] = BROADCAST_MAC_ADDRESS;
const uint8_t device_mac_addr[] = DEVICE_MAC_ADDRESS;
const uint8_t parent_addr[] = PARENT_MAC_ADDRESS;
const uint8_t nexthop_addr[] = NEXTHOP_MAC_ADDRESS;

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
  esp_task_wdt_reset();
  // send message
  if (((outgoing.type) & MESSAGE_INCOMPLETE_FLAG) == 0x00) {
    conf.isObserve = true;
    conf.msgSZ = sizeof(outgoing);
  }
#if defined(END_DEVICE)
  if ((outgoing.type) == MESSAGE_TYPE_CONTENT) {
    outgoing.msgID = conf.msgID;
  }
#endif /*END_DEVICE*/
  stamp = millis();
  esp_err_t res =
      esp_now_send(brcst_addr, (uint8_t*)&outgoing, sizeof(outgoing));
  re("ESP_NOW Sent Status: ");
  if (res == ESP_OK) {
    reln("SUCCESS");
  } else if (res == ESP_ERR_ESPNOW_NOT_INIT) {
    reln("ESPNOW not Initialized");
  } else if (res == ESP_ERR_ESPNOW_ARG) {
    reln("Invalid Argument");
  } else if (res == ESP_ERR_ESPNOW_INTERNAL) {
    reln("Internal Error");
  } else if (res == ESP_ERR_ESPNOW_NO_MEM) {
    reln("ESP_ERR_ESPNOW_NO_MEM");
  } else if (res == ESP_ERR_ESPNOW_NOT_FOUND) {
    reln("Peer not found.");
  } else {
    reln("Unexpected Errors");
  }
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

node_t* esp_comm::getCurrentSender() {
  return &cSender;
}

byte* esp_comm::getMsgID() {
  return &conf.msgID;
}

void receive(const uint8_t* macAddr, const uint8_t* data, int len) {
  // copy data to incoming message
  re("\r\nReceived MSG:");
  memcpy(&incoming, data, sizeof(incoming));
  cSender.NID = incoming.sNID;
  cSender.DID = incoming.sDID;
  repf("FROM N:%02x%02x \n", cSender.NID, cSender.DID);
  if (conf.isObserve) {  // Check next neighbour reception
    if (cSender.DID > DEVICE_ID || (cSender.DID == 0) || (incoming.msgID != conf.msgID)) {
      reln("Network alive confirmed..");
      conf.isObserve = false;
    }
  }

  recv_cb(&incoming);
}

void send_cb(const uint8_t* macAddr, esp_now_send_status_t st) {
  // out("\r\nESP-NOW Sent: ");
  // outln(st == ESP_NOW_SEND_SUCCESS ? "SUCCESS\n" : "FAILED\n");
}

void promiscuous_rx_cb(void* buf, wifi_promiscuous_pkt_type_t type) {
  // All espnow traffic uses action frames which are a subtype of the mgmnt
  // frames so filter out everything else.
  if (type != WIFI_PKT_MGMT) return;
  // Get promiscuous packet 
  const wifi_promiscuous_pkt_t* ppkt = (wifi_promiscuous_pkt_t*)buf;
  // Check void messages
  int len = ppkt->rx_ctrl.sig_len;
  if (len < 0) return;
  // Check validity of MAC ADDRESS in the experiment
  if (ppkt->payload[PROMISCUOUS_MAC_PREFIX_CHECK_INDEX] != PLACEHOLDER_CODE) return;
  // Extract DEVICE ID
  byte id = ppkt->payload[PROMISCUOUS_MAC_DEVICE_ID_INDEX];
  // Extract RSSI
  int rssi = ppkt->rx_ctrl.rssi;
  repf("WIFI CALLBACK: Retrieving RSSI: %02d, ", rssi);
  // Report RSSI Callback
  rep_cb(id, rssi);   
}
// Example of Extraction of MAC ADDRESS from Promiscuous Packets
// Check MAC Address
// String sniff;
// String mac;
// for (int i = 8; i <= 15; i++) {
//   sniff += String(ppkt->payload[i], HEX);
// }
// for (int i = 4; i <= 15; i++) {
//   mac += sniff[i];
// }
// ver("\nWIFI SNIFFER: Sender MAC ADDRESS:" + mac);
