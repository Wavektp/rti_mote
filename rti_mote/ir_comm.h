#ifndef IR_COMM_H
#define IR_COMM_H

#include "setting.h"
#include <Arduino.h>

#ifndef IR_TX_PIN
  #define IR_TX_PIN 23
#endif
#ifndef IR_RX_PIN
  #define IR_RX_PIN 33
#endif

#if !defined(ESP_ARDUINO_VERSION_VAL)
#define ESP_ARDUINO_VERSION_VAL(major, minor, patch) 12345678
#endif
#if ESP_ARDUINO_VERSION  <= ESP_ARDUINO_VERSION_VAL(2, 0, 2)
#define TONE_LEDC_CHANNEL        1  // Using channel 1 makes tone() independent of receiving timer -> No need to stop receiving timer.
void tone(uint8_t aPinNumber, unsigned int aFrequency){
    ledcAttachPin(aPinNumber, TONE_LEDC_CHANNEL);
    ledcWriteTone(TONE_LEDC_CHANNEL, aFrequency);
}
void tone(uint8_t aPinNumber, unsigned int aFrequency, unsigned long aDuration){
    ledcAttachPin(aPinNumber, TONE_LEDC_CHANNEL);
    ledcWriteTone(TONE_LEDC_CHANNEL, aFrequency);
    delay(aDuration);
    ledcWriteTone(TONE_LEDC_CHANNEL, 0);
}
void noTone(uint8_t aPinNumber){
    ledcWriteTone(TONE_LEDC_CHANNEL, 0);
}
#endif

#define IR_RECEIVE_PIN  15  // D15
#define IR_SEND_PIN     4   // D4
#define TONE_PIN        27  // D27 25 & 26 are DAC0 and 1
#define APPLICATION_PIN 16  // RX2 pin

#define IR_ADDRESS_16BIT ((NET_PREFIX << 8) | DEVICE_ID)
#define IR_ADDRESS_32BIT ((IR_ADDRESS_16BIT << 16) | IR_ADDRESS_16BIT)
#define IR_CODE_EXAMPLE  DEVICE_ID
#define IR_REPETITION    5

#define DECODE_NEC          // Includes Apple and Onkyo
// #define DECODE_DISTANCE     // in case NEC is not received correctly

/**
 * @brief The algorithm to send an IR signal and estimate RSS.
 */
class ir_comm;

class ir_comm {
 private:
  volatile int* p_write;

 public:
  volatile bool sIRRecord = false;
  /**
   * @brief initialise IR communication with a specified TX PIN
   *
   * @param txPin
   */
  void begin(unsigned int txPin);
  /// @brief initialise IR communication
  void begin();
  void send();
  uint16_t rss();
  void receive();
  void set_p_write(volatile int* irRSS);
};
#endif /*IR_COMM_H*/