#ifndef IR_COMM_H
#define IR_COMM_H

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

#define IR_ADDRESS_EXAMPLE 0x0102
#define IR_CODE_EXAMPLE    0x34
#define IR_REPETITION      0

/**
 * @brief The algorithm to send an IR signal and estimate RSS.
 */
class ir_comm;

class ir_comm {
 private:
 public:
  /**
   * @brief initialise IR communication with a specified TX PIN
   *
   * @param txPin
   */
  void setup(unsigned int txPin);
  void setup();
  void sendExample();
  float rss();
};
#endif /*IR_COMM_H*/