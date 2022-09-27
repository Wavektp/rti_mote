#ifndef IR_COMM_H
#define IR_COMM_H

#include "setting.h"

#ifndef IR_TX_PIN
  #define IR_TX_PIN 23
#endif
#ifndef IR_RX_PIN
  #define IR_RX_PIN 33
#endif

#define IR_RECEIVE_PIN  15  // D15
#define IR_SEND_PIN     4   // D4
#define TONE_PIN        27  // D27 25 & 26 are DAC0 and 1
#define APPLICATION_PIN 16  // RX2 pin

#define IR_ADDRESS_16BIT ((NET_PREFIX << 8) | DEVICE_ID)
#define IR_ADDRESS_32BIT ((IR_ADDRESS_16BIT << 16) | IR_ADDRESS_16BIT)
#define IR_CODE_EXAMPLE  DEVICE_ID
#define IR_REPETITION    3

#define DECODE_NEC          // Includes Apple and Onkyo
#define DECODE_DISTANCE     // in case NEC is not received correctly


typedef unsigned char byte;

/**
 * @brief The algorithm to send an IR signal and estimate RSS.
 */
class ir_comm;

class ir_comm {
 private:
  int* p_write;

 public:
  /**
   * @brief initialise IR communication with a specified TX PIN
   *
   * @param txPin
   */
  void begin(unsigned int txPin);
  /// @brief initialise IR communication
  void begin();
  void send();
  float rss();
  void receive();
  void set_p_write(int* irRSS);
};
#endif /*IR_COMM_H*/