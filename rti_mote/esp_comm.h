#ifndef ESP_COMM_H
#define ESP_COMM_H
#include <WiFi.h>
#include <esp_now.h>

#define out(x)          Serial.print(x)
#define outf(x, y, z)   Serial.printf(x, y, z)
#define outln(x)        Serial.println(x)
#define SERIAL_BAUDRATE 115200

#define MESSAGE_TYPE_RTI    0x01
#define MESSAGE_TYPE_REPORT 0x02

#define BROADCAST_CODE    0xFF
#define PLACEHOLDER_CODE  0xAA
#define MAC_ADDR_SIZE     6
#define MAC_ADDR_STR_SIZE 18
#define BROADCAST_MAC_ADDRESS                                       \
  {                                                                 \
    BROADCAST_CODE, BROADCAST_CODE, BROADCAST_CODE, BROADCAST_CODE, \
        BROADCAST_CODE, BROADCAST_CODE                              \
  }
#define MAX_PEER 5

#define NET_PREFIX 0x10
#define DEVICE_ID  2

#define DEVICE_MAC_ADDRESS                                                  \
  {                                                                         \
    PLACEHOLDER_CODE, PLACEHOLDER_CODE, PLACEHOLDER_CODE, PLACEHOLDER_CODE, \
        NET_PREFIX, DEVICE_ID                                               \
  }

#define RTI_STR_SIZE 38
/// @brief frame format for RTI node exchanges
typedef struct {
  /* data */
  byte type;
  byte msgID;
  byte sNID;
  byte sDID;
  byte rNID;
  byte rDID;
  byte nNID;
  byte nDID;
} rti_message_t;

/**
 * @brief Communication based on ESP-NOW
 *
 * Make use of ESP-NOW Broadcast to communicate with
 * multiple devices
 */
class esp_comm;

class esp_comm {
 private:
  /// @brief Initialize WIFI and change MAC ADDRESS
  void customize_mac_address();
  /// @brief Initialize ESP-NOW
  void initESPNow();

 public:
  /// @brief initialize communication module on esp
  void setup();
  void send_rti();
  // void receive_cb(const uint8_t* macAddr, const uint8_t* data, int len);
  // void send_cb(const uint8_t* macAddr, esp_now_send_status_t st);
};
#endif /*ESP_COMM_H*/