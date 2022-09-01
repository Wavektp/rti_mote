#include <IRremote.hpp>
// connect IR Sensor at GPIO 34
#define NF_INTERVAL 1000
#define IR_SEND_PIN 33

uint16_t sAddress = 0x0102;
uint8_t sCommand = 0x34;
uint8_t sRepeats = 0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  // Enable Serial
  Serial.begin(115200);
  while(!Serial){}
#if defined(IR_SEND_PIN)
  IrSender.begin(IR_SEND_PIN); // Start with IR_SEND_PIN as send pin and enable feedback LED at default feedback LED pin
  IrSender.enableIROut(38); // Call it with 38 kHz to initialize the values printed below
#else
  IrSender.begin(3, ENABLE_LED_FEEDBACK); // Specify send pin and enable feedback LED at default feedback LED pin
#endif
}

void loop() {
  IrSender.sendNEC(sAddress, sCommand, sRepeats);
  Serial.println("Already pass send command");
  delay(NF_INTERVAL);
}
