#ifndef ESP_COMM_H
#define ESP_COMM_H

#define out(x)          Serial.print(x)
#define outln(x)        Serial.println(x)
#define SERIAL_BAUDRATE 115200

#define MESSAGE_TYPE_RTI    0x01
#define MESSAGE_TYPE_REPORT 0x02

#define NET_PREFIX 'A'
#define DEVICE_ID  1

/// @brief frame format for RTI node exchanges
typedef struct rti_message {
  /* data */
  byte netPrefix;
  byte deviceID;
};

///@brief manage communication to other nodes via esp_now
class esp_comm;
class esp_comm {
 private:
 public:
  /// @brief initialize communication module on esp
  void setup();
};
#endif /*ESP_COMM_H*/