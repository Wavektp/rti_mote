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
  repf("IR Signal Sent: %08x", ir_test);
}

float ir_comm::rss() {
  return analogRead(IR_RECEIVE_PIN);
}

void ir_comm::receive() {
  if (IrReceiver.decode()) {
    IrReceiver.printIRResultShort(&Serial);
    IrReceiver.printIRSendUsage(&Serial);
    // TODO keep value to evaluate RTI
    *p_write = 1;
    re("Received command:");
    re(IrReceiver.decodedIRData.command);
    reln(":Write IR reception BOOLEAN value");
    if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
      Serial.println(
          F("Received noise or an unknown (or not yet enabled) protocol"));
      // We have an unknown protocol here, print more info
      IrReceiver.printIRResultRawFormatted(&Serial, true);
    }
    Serial.println();

    IrReceiver.resume();  // Enable receiving of the next value
  }
}

void ir_comm::set_p_write(int* irRSS) {
  *irRSS = 0;
  p_write = irRSS;
  receive();
}