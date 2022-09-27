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
  IrSender.sendNEC(IR_ADDRESS_16BIT, DEVICE_ID, IR_REPETITION);
  repf("IR Signal Sent: %08x", DEVICE_ID);
}

float ir_comm::rss() {
  return analogRead(IR_RECEIVE_PIN);
}

void ir_comm::receive() {
  // if (IrReceiver.decode()) {
  //   re("Received command:");
  //   IrReceiver.printIRResultShort(&Serial);
  //   IrReceiver.printIRSendUsage(&Serial);
  //   // TODO keep value to evaluate RTI
  //   *p_write = 1;
  //   re(IrReceiver.decodedIRData.command);
  //   reln(":Write IR reception BOOLEAN value");
  //   if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
  //     Serial.println(
  //         F("Received noise or an unknown (or not yet enabled) protocol"));
  //     // We have an unknown protocol here, print more info
  //     IrReceiver.printIRResultRawFormatted(&Serial, true);
  //   }
  //   Serial.println();

  //   IrReceiver.resume();  // Enable receiving of the next value
  // }
}

void ir_comm::set_p_write(volatile int* irRSS) {
  *irRSS = 0;
  p_write = irRSS;
  receive();
}