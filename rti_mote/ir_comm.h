#ifndef IR_COMM_H
#define IR_COMM_H

#ifndef IR_TX_PIN
#define IR_TX_PIN 23
#endif
#ifndef IR_RX_PIN
#define IR_RX_PIN 33
#endif

#if !defined(ESP_ARDUINO_VERSION_VAL)
#define ESP_ARDUINO_VERSION_VAL(major, minor, patch) 12345678
#endif
#if ESP_ARDUINO_VERSION <= ESP_ARDUINO_VERSION_VAL(2, 0, 2)
#define TONE_LEDC_CHANNEL \
  1  // Using channel 1 makes tone() independent of receiving timer -> No need
     // to stop receiving timer.

void tone(uint8_t aPinNumber, unsigned int aFrequency) {
  ledcAttachPin(aPinNumber, TONE_LEDC_CHANNEL);
  ledcWriteTone(TONE_LEDC_CHANNEL, aFrequency);
}

void tone(uint8_t aPinNumber,
          unsigned int aFrequency,
          unsigned long aDuration) {
  ledcAttachPin(aPinNumber, TONE_LEDC_CHANNEL);
  ledcWriteTone(TONE_LEDC_CHANNEL, aFrequency);
  delay(aDuration);
  ledcWriteTone(TONE_LEDC_CHANNEL, 0);
}

void noTone(uint8_t aPinNumber) {
  ledcWriteTone(TONE_LEDC_CHANNEL, 0);
}
#endif  // ESP_ARDUINO_VERSION  <= ESP_ARDUINO_VERSION_VAL(2, 0, 2)

#define IR_RECEIVE_PIN 15   // D15
#define IR_SEND_PIN 4       // D4
#define TONE_PIN 27         // D27 25 & 26 are DAC0 and 1
#define APPLICATION_PIN 16  // RX2 pin

#define IR_ADDRESS_EXAMPLE 0x0102
#define IR_CODE_EXAMPLE 0x34
#define IR_REPETITION 0

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