#include "rti.h"

void start_rti();

void RTI::begin() {
  espC.begin((recv_cb_t)&RTI::receive, (report_cb_t)&RTI::report);
  irC.begin(IR_SEND_PIN);
  outln("... initialize RTI Protocols");

  out("DEVICE ID: ");
  outln(DEVICE_ID);

  out("NET PREFIX: ");
  outln(NET_PREFIX);

  out("\r\nNODE COUNT: ");
  outln(RTI_NODE_COUNT);

  // next.NID = NET_PREFIX;
  // byte dID = DEVICE_ID + 1;
  // if (dID <= RTI_NODE_COUNT) {
  //   next.DID = dID;
  // } else {
  //   next.DID = 0;
  // }
#ifdef END_DEVICE
  #if RTI_SCHEME == RTI_DEFAULT_SCHEME
  outln("DEFAULT SCHEME");
  for (int i = 0; i < RTI_NEIGHBOUR_COUNT; i++) {
    neighbour[i].node.NID = NET_PREFIX;
    neighbour[i].RSS = 0;
    neighbour[i].irRSS = 0;
    if ((i + 1) >= DEVICE_ID) {
      neighbour[i].node.DID = (i + 2);
    }
  }
  #endif /*RTI_SCHEME == RTI_DEFAULT_SCHEME*/
  #if RTI_SCHEME == RTI_SIDEWAY_SCHEME
  outln("SIDEWAY SCHEME");
  bool isEven = (DEVICE_ID % 2 == 0);
  for (int i = 0; i < RTI_NEIGHBOUR_COUNT; i++) {
    byte dID = (2 * i + 1);
    if (!isEven) {
      dID += 1;
    }
    neighbour[i].node.NID = NET_PREFIX;
    neighbour[i].node.DID = dID;
    neighbour[i].RSS = 0;
    neighbour[i].irRSS = 0;
  }

  #endif /*RTI_SCHEME == RTI_SIDEWAY_SCHEME*/
  outln("RECTANGULAR SCHEME");
  #if RTI_SCHEME == RTI_RECTANGULAR_SCHEME
  for (int i = 0; i < RTI_NEIGHBOUR_COUNT; i++) {
    neighbour[i].node.NID = NET_PREFIX;
    byte dID = i + (RECTANGULAR_SIDE * SIDE_NODE_COUNT + 2);
    neighbour[i].node.DID = dID;
    if (dID <= RTI_NODE_COUNT) {
      neighbour[i].node.DID = dID;
    } else {
      neighbour[i].node.DID = dID - RECTANGULAR_SIDE * SIDE_NODE_COUNT;
    }
  }
  #endif /*RTI_SIDEWAY_SCHEME == RTI_RECTANGULAR_SCHEME*/
  outln("CUSTOM SCHEME");
  #if RTI_SCHEME == RTI_CUSTOM_SCHEME
    #error In CUSTOM SCHEME, neighbours need to be manually defined
  #endif
  out("NEIGHBOUR COUNT: ");
  outln(RTI_NEIGHBOUR_COUNT);
#endif /*END_DEVICE*/
  outf("NEXT NEIGHBOUR: N%02x%02x", NEXT_NEIGHBOUR_NET_PREFIX,
       NEXT_NEIGHBOUR_DEVICE_ID);

#ifdef ROOT_NODE
  outln("-------- ROOT NODE --------");
  delay(START_DELAY);
  outln("...Sending BEACON");
  start_rti();
#endif /*ROOT_NODE*/
  // Set watchdog timer
  outln("...Initialize watchdog");
  out("RESET TIMEOUT: ");
  outln(RESET_TIMEOUT);
  esp_task_wdt_init(RESET_TIMEOUT, true);
  esp_task_wdt_add(NULL);
}

void RTI::msgToStr(message_t* msg, char* str) {
  char st[RTI_STR_SIZE];
  sniprintf(st, RTI_PREFIX_STR_SIZE, RTI_PREFIX_STR, msg->type, msg->msgID,
            msg->sNID, msg->sDID, msg->rNID, msg->rDID, msg->nNID, msg->nDID);
  if (msg->type == MESSAGE_TYPE_BEACON) {
    strcat(st, "BEACON");
    if (msg->len != 0) {
      outln("Error: Invalid Frame Format: contents on BEACON are not defined");
    }
  } else {
    size_t neighbourCount = (msg->len / 2 - 1);
    if (msg->content[0] != RTI_MSG_MASK_RSS) {
      outln("Error: Invalid Frame Format: not found RSS PREFIX");
    }
    for (int i = 1; i < (neighbourCount + 1); i++) {
      char temp[RTI_RSS_STR_SIZE];
      sniprintf(st, RTI_RSS_STR_SIZE, RTI_RSS_STR, i, msg->content[i]);
      re("DEBUG: check concat string: ");
      re(temp);
      strcat(st, temp);
      reln(st);
    }
    if (msg->content[(neighbourCount + 1)] != RTI_MSG_MASK_IR) {
      out("Error: Invalid Frame Format: not found IR PREFIX");
    }
    for (int i = (neighbourCount + 2); i < (2 * neighbourCount + 2); i++) {
      char temp[RTI_IR_STR_SIZE];
      sniprintf(st, RTI_IR_STR_SIZE, RTI_IR_STR, (i - (neighbourCount + 2)),
                msg->content[i]);
      strcat(st, temp);
    }
  }
  re("MESSAGE TO COPY:");
  reln(st);
  strcpy(str, st);
}

void RTI::create_rti_message(message_t* msg, byte type, bool isCompleted) {
  /*create message prefix*/
  msg->type = type;
  msg->sNID = NET_PREFIX;
  msg->sDID = DEVICE_ID;
  msg->rNID = BROADCAST_CODE;
  msg->rDID = BROADCAST_CODE;
  if (isCompleted) {
    msg->nNID = NEXT_NEIGHBOUR_NET_PREFIX;
    msg->nDID = NEXT_NEIGHBOUR_DEVICE_ID;
  } else {
    msg->nNID = NET_PREFIX;
    msg->nDID = DEVICE_ID;
  }
#ifdef ROOT_NODE
  if (type == MESSAGE_TYPE_BEACON) {
    msg->len = 0;
    for (int i = 0; i < MAX_CONTENT_SIZE; i++) {
      msg->content[i] = 0;
    }
  }
#endif /*ROOT_NODE*/
#ifdef END_DEVICE
  if (type == MESSAGE_TYPE_CONTENT) {
    msg->len = 2 * RTI_NEIGHBOUR_COUNT + 2;
    msg->content[0] = RTI_MSG_MASK_RSS;
    for (int i = 1; i < (RTI_NEIGHBOUR_COUNT + 1); i++) {
      msg->content[i] = neighbour[i - 1].RSS;
      neighbour[i - 1].RSS = 0;  // reset value
    }
    msg->content[RTI_NEIGHBOUR_COUNT + 1] = RTI_MSG_MASK_IR;
    for (int i = (RTI_NEIGHBOUR_COUNT + 2); i < (2 * RTI_NEIGHBOUR_COUNT + 2);
         i++) {
      msg->content[i] = neighbour[i - (RTI_NEIGHBOUR_COUNT + 2)].irRSS;
      neighbour[i - (RTI_NEIGHBOUR_COUNT + 2)].irRSS = 0;  // reset value
    }
    msg->content[(2 * RTI_NEIGHBOUR_COUNT + 2)] = RTI_MSG_MASK_END;
    for (int i = 2 * RTI_NEIGHBOUR_COUNT + 3; i < MAX_CONTENT_SIZE; i++) {
      msg->content[i] = 0;
    }
  }
#endif /*END_DEVICE*/
}
#ifdef ROOT_NODE
void RTI::start_rti() {
  message_t* m = espC.get_outgoing();
  create_rti_message(m, MESSAGE_TYPE_BEACON, true);
  espC.send(m, sizeof(m));
}
#endif /*ROOT_NODE*/

void RTI::routine() {
  // reception of IR signal
  irC.receive();
  if (espC.checkTimeout(RTI_TIMEOUT)) {
    // Next node does not tranmit assuming incorrect reception, repeat the last
    // message -> resend
    espC.resend();
  }
}

void RTI::receive(message_t* incoming) {
  char outStr[RTI_STR_SIZE];
  msgToStr(incoming, outStr);
  // copy data to incoming message
  re("RTI CALLBACK: ");
  if (incoming->type == MESSAGE_TYPE_BEACON) {
    // Still no action
  }
  if (incoming->type == MESSAGE_TYPE_CONTENT) {
    // TODO set pointer record of IR, prepare to get IR RSS
  }
  if (incoming->nNID == NET_PREFIX &&
      incoming->nDID == DEVICE_ID) {  // if this node is the next sender
    delay(1);
    // send IR
    irC.send();
    // send ESP-NOW and reset
    message_t* m = espC.get_outgoing();
    create_rti_message(m, MESSAGE_TYPE_CONTENT, true);
    espC.send(m, sizeof(m));
  }
}

void RTI::report(int rssi) {
  re("RTI - REPORT CALLBACK - RSSI BEFORE SET: ");
  reln(neighbour[neighbourP].RSS);
  neighbour[neighbourP].RSS = rssi;
  outf("RSSI NEIGHBOUR: %02x%02x P:%02x RSSI:%02x",
       neighbour[neighbourP].node.NID, neighbour[neighbourP].node.DID,
       neighbourP, neighbour[neighbourP].RSS);
}