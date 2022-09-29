#include "ir_comm.h"
#include <IRremote.hpp>

volatile int* p_write;
volatile bool sIRRecord = false;

// void ARDUINO_ISR_ATTR irRecCB() {
//   if (sIRRecord) {
//     detachInterrupt(digitalPinToInterrupt(IR_RX_PIN));
//     uint16_t ir = analogRead(IR_RX_PIN);
//     uint8_t c = 0;
//     while ((ir == 0) && (c < 100)) {
//       ir = analogRead(IR_RX_PIN);
//       delayMicroseconds(200);
//       c++;
//     }
//     verf("CALLBACK IR Analog Read: %02i..", ir);
//     if (ir) {
//       *p_write = ir;
//       verf("Set IR = %04i \n", ir);
//     }
//     attachInterrupt(IR_RX_PIN, irRecCB, FALLING);
//   }
// }

void ir_comm::begin(unsigned int txPIN) {
  outln(F("START " __FILE__ " from " __DATE__
          "\r\nUsing library version " VERSION_IRREMOTE));
  pinMode(IR_RX_PIN, INPUT);
  // attachInterrupt(IR_RX_PIN, irRecCB, FALLING);
  IrSender.begin(txPIN);
  // IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  // out(F("Ready to receive IR signals of protocols \n"));
  // printActiveIRProtocols(&Serial);
}

void ir_comm::begin() {
  IrSender.begin();
}

void ir_comm::send() {
  repf("IR Signal Sent: %02x \n", IR_CODE_EXAMPLE);
  IrSender.sendNEC(IR_ADDRESS_16BIT, IR_CODE_EXAMPLE, IR_REPETITION);
}

void ir_comm::receive() {
  // if (IrReceiver.decode()) {
  // re("Received IR:");
  // IrReceiver.printIRResultShort(&Serial);
  // IrReceiver.printIRSendUsage(&Serial);
  // // TODO keep value to evaluate RTI
  // re(IrReceiver.decodedIRData.command);
  // *p_write = 1;
  // reln(":Write IR reception BOOLEAN value");
  // if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
  //   Serial.println(
  //       F("Received noise or an unknown (or not yet enabled) protocol"));
  //   // We have an unknown protocol here, print more info
  //   IrReceiver.printIRResultRawFormatted(&Serial, true);
  // }
  // Serial.println();

  // IrReceiver.resume();  // Enable receiving of the next value
  // }
  if (sIRRecord) {
    uint16_t ir = analogRead(IR_RX_PIN);
    // verf("IR Analog Read: %02i..", ir);
    uint8_t c = 0;
    while ((ir == NO_SIGNAL_LVL) && c < 100) {
      ir = analogRead(IR_RX_PIN);
      c++;
    }
    if (ir != NO_SIGNAL_LVL) {
      *p_write = ir;
      verf("Set IR = %04i \n", ir);
    }
  }
}

void ir_comm::set_p_write(volatile int* irRSS) {
  *irRSS = NO_SIGNAL_LVL;
  p_write = irRSS;
  sIRRecord = true;
  verln("Set Flag IR reception");
  receive();
}

void ir_comm::setFlag(bool b, volatile int* p) {
  sIRRecord = b;
  p_write = p;
}