#include "IRModule.h"
#include <IRremote.hpp>

void IRModule::setup(unsigned int txPIN) {
    pinMode(IR_RX_PIN, INPUT);
    IrSender.begin(txPIN);
}

void IRModule::setup() {
    IrSender.begin(IR_TX_PIN);
}

void IRModule::sendExample() {
    IrSender.sendNEC(IR_ADDRESS_EXAMPLE, IR_CODE_EXAMPLE, IR_REPETITION);
}

float IRModule::rss() {
    return analogRead(IR_RX_PIN);
}  