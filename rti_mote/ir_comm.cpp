#include "ir_comm.h"
#include <IRremote.hpp>

void ir_comm::setup(unsigned int txPIN) {
    pinMode(IR_RX_PIN, INPUT);
    IrSender.begin(txPIN);
}

void ir_comm::setup() {
    IrSender.begin(IR_TX_PIN);
}

void ir_comm::sendExample() {
    IrSender.sendNEC(IR_ADDRESS_EXAMPLE, IR_CODE_EXAMPLE, IR_REPETITION);
}

float ir_comm::rss() {
    return analogRead(IR_RX_PIN);
}  