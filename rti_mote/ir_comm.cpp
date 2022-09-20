#include "ir_comm.h"
#include <IRremote.hpp>

void ir_comm::begin(unsigned int txPIN) {
  outln(F("START " __FILE__ " from " __DATE__
          "\r\nUsing library version " VERSION_IRREMOTE));
  pinMode(IR_RECEIVE_PIN, INPUT);
  IrSender.begin(txPIN);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  out(F("Ready to receive IR signals of protocols"));
  printActiveIRProtocols(&Serial);
}

void ir_comm::begin() {
  IrSender.begin();
}

void ir_comm::send() {
  uint32_t ir_test = IR_ADDRESS_32BIT;
  IrSender.sendNECRaw(ir_test, IR_REPETITION);
}

float ir_comm::rss() {
  return analogRead(IR_RECEIVE_PIN);
}