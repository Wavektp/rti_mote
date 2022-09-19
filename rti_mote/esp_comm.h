#ifndef ESP_COMM_H
#define ESP_COMM_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "esp_task_wdt.h"
#include "esp_wifi.h"
#include "setting.h"

#define out(x)        Serial.print(x)
#define outf(x, y, z) Serial.printf(x, y, z)
#define outln(x)      Serial.println(x)
#ifdef DEBUG_REPORT
  #define re(x)   Serial.print(x)
  #define reln(x) Serial.println(x)
#else
  #define re(x)   
  #define reln(x) 
#endif /*DEBUG_REPORT*/
#define SERIAL_BAUDRATE 115200

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

/******************************************************************************
 * Communication based on ESP-NOW
 *
 * Make use of ESP-NOW Broadcast to communicate with
 * multiple devices
 ******************************************************************************/
#define MESSAGE_TYPE_BEACON  0x00
#define MESSAGE_TYPE_CONTENT 0x01

#define MESSAGE_INCOMPLETE_FLAG 0x08
#define MAX_CONTENT_SIZE        220

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
  size_t len;
  byte content[MAX_CONTENT_SIZE];
} message_t;

typedef unsigned char byte;
typedef void (*recv_cb_t)(message_t* incoming);
typedef struct {
  byte NID;
  byte DID;
} node_t;

typedef struct {
  bool isObserve;
  node_t expect;
  byte msg_id;
  size_t msg_sz;
} confirmable_t;

typedef unsigned char timestamp_t;

/// @brief Class for managing ESP-NOW
class esp_comm;

class esp_comm {
 private:
  confirmable_t conf;
  timestamp_t stamp;
  recv_cb_t recv_cb;

  /// @brief Initialize WIFI and change MAC ADDRESS
  void customize_mac_address();
  /// @brief Initialize ESP-NOW
  void initESPNow();

 public:
  /// @brief initialize communication module on esp
  void begin(recv_cb_t recv_cb);
  /// @brief Broadcast msg via ESP-NOW
  /// @param pointer to message
  /// @param size of message
  void send(message_t* msg, size_t sz);
  void resend();
  /// @brief write MAC ADDRESS as string
  /// @param macAddr MAC ADDRESS
  /// @param str pointer to character array
  /// @param len length of string
  void macAddrToStr(const uint8_t* macAddr, char* str, int len);
  bool checkTimeout(timestamp_t timeout);
  message_t* get_incoming();
  message_t* get_outgoing();
  /******************************************************************************
   *  Callback Functions on Reception/Transmission *
   ******************************************************************************/
  void receive(const uint8_t* macAddr, const uint8_t* data, int len);
  void send_cb(const uint8_t* macAddr, esp_now_send_status_t st);
};

#endif /*ESP_COMM_H*/