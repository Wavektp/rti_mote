#ifndef ESP_COMM_H
#define ESP_COMM_H
#include <esp_now.h>
#include <WiFi.h>

#define out(x)          Serial.print(x)
#define outln(x)        Serial.println(x)
#define SERIAL_BAUDRATE 115200

#define MESSAGE_TYPE_RTI    0x01
#define MESSAGE_TYPE_REPORT 0x02

#define BROADCAST_CODE   0xFF
#define PLACEHOLDER_CODE 0xAA
#define MAC_ADDR_SIZE    6
#define BROADCAST_MAC_ADDRESS                                       \
  {                                                                 \
    BROADCAST_CODE, BROADCAST_CODE, BROADCAST_CODE, BROADCAST_CODE, \
        BROADCAST_CODE, BROADCAST_CODE                              \
  }

#define MAX_PEER  5

#define NET_PREFIX 'A'
#define DEVICE_ID  1

/// @brief frame format for RTI node exchanges
typedef struct {
  /* data */
  byte type;
  byte msgID;
  byte sNID;
  byte sDID;
  byte rNID;
  byte rDID;
} rti_message_t;

///@brief manage communication to other nodes via esp_now
class esp_comm;
class esp_comm {
 private:
  esp_now_peer_info_t peerInfo[MAX_PEER];
  /// @brief Initialize WIFI and change MAC ADDRESS
  void customize_mac_address();
 public:
  /// @brief initialize communication module on esp
  void setup();
  void receive_cb();
  void send_cb();
};
#endif /*ESP_COMM_H*/