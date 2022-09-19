#include "ir_comm.h"
#include <IRremote.hpp>

void ir_comm::begin(unsigned int txPIN) {
  pinMode(IR_RECEIVE_PIN, INPUT);
  IrSender.begin(txPIN);
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