#include "IRModule.h"

#define NF_INTERVAL 1000

IRModule irM;

uint16_t sAddress = 0x0102;
uint8_t sCommand = 0x34;
uint8_t sRepeats = 0;

void setup() {
  irM.setup();
  // Enable Serial
  Serial.begin(115200);
  while(!Serial){}
}

void loop() {
  delay(NF_INTERVAL);
}
