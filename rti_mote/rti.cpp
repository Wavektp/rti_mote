#include "rti.h"

volatile rti_info_t info;
esp_comm espC;
ir_comm irC;

void msgToStr(message_t* msg, char* str);
void create_rti_message(message_t* msg, byte type, bool isCompleted);
void receive(message_t* incoming);
void report(int rssi);
bool checkNeighbourP();

void RTI::begin() {
  // Set watchdog timer
  outln("\r\n...Initialize watchdog");
  out("RESET TIMEOUT: ");
  outln(RESET_TIMEOUT);
  esp_task_wdt_init(RESET_TIMEOUT, true);
  esp_task_wdt_add(NULL);

  espC.begin((recv_cb_t)&receive, (report_cb_t)&report);
  outln("\r\n... initialize RTI Protocols");

  out("DEVICE ID: ");
  outln(DEVICE_ID);

  out("NET PREFIX: ");
  outln(NET_PREFIX);

  out("\r\nNODE COUNT: ");
  outln(RTI_NODE_COUNT);

#if defined(END_DEVICE)
  irC.begin(IR_SEND_PIN);
  #if defined(RTI_DEFAULT_SCHEME)
  outln("DEFAULT SCHEME");
  for (int i = 0; i < RTI_NEIGHBOUR_COUNT; i++) {
    neighbour[i].node.NID = NET_PREFIX;
    neighbour[i].RSS = 0;
    neighbour[i].irRSS = 0;
    if ((i + 1) >= DEVICE_ID) {
      neighbour[i].node.DID = (i + 2);
    }
  }
  #endif /*RTI_DEFAULT_SCHEME*/
  #if defined(RTI_SIDEWAY_SCHEME)
  out("SIDEWAY SCHEME:");
  outf("DEVICE ID: %02x \n", DEVICE_ID);

  bool isEven = (DEVICE_ID % 2 == 0);
  // if DID is even
  if (isEven) {
    ver("-> DEVICE on EVEN side -> NEIGHBOUR on ODD side \n");
    SETFLAG(info.pos, ODD_SIDE_NEIGHBOUR_FLAG);
  } else {
    ver("-> DEVICE on ODD side -> NEIGHBOUR on EVEN side \n");
    SETFLAG(info.pos, EVEN_SIDE_NEIGHBOUR_FLAG);
  }
  for (int i = 0; i < RTI_NEIGHBOUR_COUNT; i++) {
    byte nDID = (2 * i + 1);  // NEIGHBOUR ID start at 1 on ODD side
    if CHECKFLAG (info.pos, EVEN_SIDE_NEIGHBOUR_FLAG)
      nDID++;  // NEIGHBOUR ID start at 2 on EVEN side
    info.neighbour[i].node.NID = NET_PREFIX;
    info.neighbour[i].node.DID = nDID;
    info.neighbour[i].RSS = 0;
    info.neighbour[i].irRSS = 0;
    verf("SET NEIGHBOUR INDEX:%02d, NID:%02d \n", i, nDID);
  }

  #endif /*RTI_SIDEWAY_SCHEME*/
  #if defined(RTI_RECTANGULAR_SCHEME)
  outln("RECTANGULAR SCHEME");
  for (int i = 0; i < RTI_NEIGHBOUR_COUNT; i++) {
    info.neighbour[i].node.NID = NET_PREFIX;
    byte dID = i + (RECTANGULAR_SIDE * SIDE_NODE_COUNT + 2);
    info.neighbour[i].node.DID = dID;
    if (dID <= RTI_NODE_COUNT) {
      info.neighbour[i].node.DID = dID;
    } else {
      info.neighbour[i].node.DID = dID - RECTANGULAR_SIDE * SIDE_NODE_COUNT;
    }
  }
  #endif /*RTI_RECTANGULAR_SCHEME*/
  #if defined(RTI_CUSTOM_SCHEME)
  outln("CUSTOM SCHEME");
    #error In CUSTOM SCHEME, neighbours need to be manually defined
  #endif
  out("NEIGHBOUR COUNT: ");
  outln(RTI_NEIGHBOUR_COUNT);
#endif /*END_DEVICE*/
  outf("NEXT NEIGHBOUR: N%02x%02x \n", NEXT_NEIGHBOUR_NET_PREFIX,
       NEXT_NEIGHBOUR_DEVICE_ID);

#if defined(ROOT_NODE)
  outln("-------- ROOT NODE --------");
  delay(START_DELAY);
  outln("...Sending BEACON");
  start_rti();
#endif /*ROOT_NODE*/
}

void msgToStr(message_t* msg, char* str) {
  char st[RTI_STR_SIZE];
  sniprintf(st, RTI_PREFIX_STR_SIZE, RTI_PREFIX_STR, msg->type, msg->msgID,
            msg->sNID, msg->sDID, msg->rNID, msg->rDID, msg->nNID, msg->nDID);
  // ver("MSG String:PREFIX: ");
  st[RTI_PREFIX_STR_SIZE] = 0;
  // ver(st);
  if (msg->type == MESSAGE_TYPE_BEACON) {
    strcat(st, "BEACON");
    // ver(st);
    if (msg->len != 0) {
      outln("Error: Invalid Frame Format: contents on BEACON are not defined");
    }
  } else {
    size_t neighbourCount = (msg->len / 2 - 1);
    if (msg->content[0] != RTI_MSG_MASK_RSS) {
      outln("Error: Invalid Frame Format: not found RSS PREFIX");
    } else {
      strcat(st, "REPORT RSSI. \n");
    }
    for (int i = 1; i < (neighbourCount + 1); i++) {
      char temp[RTI_RSS_STR_SIZE];
      sniprintf(temp, RTI_RSS_STR_SIZE, RTI_RSS_STR, i, msg->content[i]);
      strcat(st, temp);
      strcat(st, "\r\n");
    }
    if (msg->content[(neighbourCount + 1)] != RTI_MSG_MASK_IR) {
      out("Error: Invalid Frame Format: not found IR PREFIX");
    } else {
      strcat(st, "REPORT IR. \n");
    }
    for (int i = (neighbourCount + 2); i < (2 * neighbourCount + 2); i++) {
      char temp[RTI_IR_STR_SIZE];
      sniprintf(temp, RTI_IR_STR_SIZE, RTI_IR_STR, (i - (neighbourCount + 1)),
                msg->content[i]);
      strcat(st, temp);
      strcat(st, "\r\n");
    }
  }
  strcpy(str, st);
}

void create_rti_message(message_t* msg, byte type, bool isCompleted) {
  /*create message prefix*/
  re("\r\nCREATE MESSAGE: ");
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
  verf("Set NEXT:%02x%02x - ", msg->nNID, msg->nDID);
#if defined(ROOT_NODE)
  if (type == MESSAGE_TYPE_BEACON) {
    reln("BEACON");
    msg->len = 0;
    for (int i = 0; i < MAX_CONTENT_SIZE; i++) {
      msg->content[i] = 0;
    }
  }
#endif /*ROOT_NODE*/
#if defined(END_DEVICE)
  if (type == MESSAGE_TYPE_CONTENT) {
    reln("CONTENT: ");
    msg->len = 2 * RTI_NEIGHBOUR_COUNT + 2;
    msg->content[0] = RTI_MSG_MASK_RSS;
    for (int i = 1; i < (RTI_NEIGHBOUR_COUNT + 1); i++) {
      // verf("LOOK RSSI I%02x=%02d \n", (i - 1), info.neighbour[i - 1].RSS);
      msg->content[i] = info.neighbour[i - 1].RSS;
      // info.neighbour[i - 1].RSS = 0;  // reset value
      // verf("SET RSSI N%02x=%02d \n", i, msg->content[i]);
    }
    msg->content[RTI_NEIGHBOUR_COUNT + 1] = RTI_MSG_MASK_IR;
    for (int i = (RTI_NEIGHBOUR_COUNT + 2); i < (2 * RTI_NEIGHBOUR_COUNT + 2);
         i++) {
      msg->content[i] = info.neighbour[i - (RTI_NEIGHBOUR_COUNT + 2)].irRSS;
      // info.neighbour[i - (RTI_NEIGHBOUR_COUNT + 2)].irRSS = 0;  // reset
      // value verf("SET IR N%02x=%02d \n", (i - (RTI_NEIGHBOUR_COUNT + 1)),
      //      msg->content[i]);
    }
    msg->content[(2 * RTI_NEIGHBOUR_COUNT + 2)] = RTI_MSG_MASK_END;
    for (int i = 2 * RTI_NEIGHBOUR_COUNT + 3; i < MAX_CONTENT_SIZE; i++) {
      msg->content[i] = 0;
    }
  }
#endif /*END_DEVICE*/
}
#if defined(ROOT_NODE)
void RTI::start_rti() {
  message_t* m = espC.get_outgoing();
  create_rti_message(m, MESSAGE_TYPE_BEACON, true);
  espC.send();
}
#endif /*ROOT_NODE*/

void RTI::routine() {
#if defined(END_DEVICE)
  // reception of IR signal
  irC.receive();
#endif /*END_DEVICE*/
  if (info.sPending) {
    info.sPending = false;
    // send ESP-NOW and reset
    message_t* m = espC.get_outgoing();
#if defined(END_DEVICE)
    timestamp_t stamp = millis();
    while (millis() - stamp < SEND_DELAY) {
      irC.send();
    }
    // Create CONTENT message
    create_rti_message(m, MESSAGE_TYPE_CONTENT, true);
#endif /*END_DEVICE*/
#if defined(ROOT_NODE)
    // Create BEACON message
    create_rti_message(m, MESSAGE_TYPE_BEACON, true);
#endif /*ROOT_NODE*/
    espC.send();
  }
  if (espC.checkTimeout(RTI_TIMEOUT)) {
    // Next node does not tranmit assuming incorrect reception, repeat the
    // last message -> resend
    espC.send();
  }
}

void receive(message_t* incoming) {
  esp_task_wdt_reset();
  info.sSetRSS = false;
  irC.setFlag(false, &info.tempIR);
  verln("Unflag IR Reception");
  // copy data to incoming message
  re("RTI CALLBACK: ");
  if (incoming->type == MESSAGE_TYPE_BEACON) {
    re("BEACON received: ");
  }
  if (incoming->type == MESSAGE_TYPE_CONTENT) {
#if defined(ROOT_NODE)
    re("CONTENT received: \n");
    char outStr[RTI_STR_SIZE];
    msgToStr(incoming, outStr);
    out(outStr);
#endif
#if defined(END_DEVICE)
    re("CONTENT received: ");
    if (checkNeighbourP()) {
      verf("Set IR Pointer to NEIGHBOUR %02x \n", info.neighbourP);
      irC.set_p_write(&info.neighbour[info.neighbourP].irRSS);
    } else {
      ver("Set IR Pointer to TEMP IR \n");
      irC.set_p_write(&info.tempIR);
    }
#endif
  }
  verf("NEXT N:%02x%02x, ", incoming->nNID, incoming->nDID);
  verf("THIS N:%02x%02x \n", NET_PREFIX, DEVICE_ID);
  if (incoming->nNID == NET_PREFIX &&
      incoming->nDID == DEVICE_ID) {  // if this node is the next sender
    reln("TOKEN RECEIVED..SET flag on pending message");
    info.sPending = true;
  }
}

void report(int rssi) {
  info.tempRSSI = rssi;
  verf("RTI - REPORT CALLBACK SET RSSI: %02d \n", info.tempRSSI);
#if defined(END_DEVICE)
  if (info.sSetRSS) {
    verf("Attempt RSSI N%02x=%02d: ", info.neighbourP, info.tempRSSI);
    verf("RSSI BEFORE SET: %02d \n", info.neighbour[info.neighbourP].RSS);
    info.neighbour[info.neighbourP].RSS = info.tempRSSI;
    repf("RSSI NEIGHBOUR: %02x%02x P:%02x RSSI:%02d \n",
         info.neighbour[info.neighbourP].node.NID,
         info.neighbour[info.neighbourP].node.DID, info.neighbourP,
         info.neighbour[info.neighbourP].RSS);
    info.sSetRSS = false;
  }
#endif /*END_DEVICE*/
}

#if defined(RTI_DEFAULT_SCHEME)
void RTI::checkNeighbourP() {
  // get currect sender from communication module
  node_t* cS = espC.getCurrentSender();
}
#endif /*RTI_DEFAULT_SCHEME*/
#if defined(RTI_SIDEWAY_SCHEME)
bool checkNeighbourP() {
  info.neighbourP = RTI_NEIGHBOUR_COUNT;
  info.sSetRSS = false;
  // get currect sender from communication module
  node_t* cS = espC.getCurrentSender();
  verf("\r\nCHECK NEIGHBOUR: SIDEWAY SCHEME: SID:%02x: ", cS->DID);
  if (cS->DID == 0) {
    verln("ROOT NODE detected \n");
    return false;
  }
  uint8_t nID;
  if CHECKFLAG (info.pos, ODD_SIDE_NEIGHBOUR_FLAG) {
    if (cS->DID % 2 == 0)
      return false;
    ver("Neighbour on ODD Side - ");
    nID = (cS->DID / 2);
  }
  if CHECKFLAG (info.pos, EVEN_SIDE_NEIGHBOUR_FLAG) {
    if (cS->DID % 2 == 1)
      return false;
    ver("Neighbour on EVEN Side - ");
    nID = (cS->DID / 2) - 1;
  }
  if (nID >= RTI_NEIGHBOUR_COUNT) {
    ver("Neigbour ID out of bound");
    return false;
  }
  verf("SET NID:%02d - Set flag for RSSI record \n", nID);
  info.neighbourP = (uint8_t)nID;
  info.sSetRSS = true;
  return true;
}
#endif /*RTI_SIDEWAY_POSITION*/
#if defined(RTI_RECTANGULAR_SCHEME)
void RTI::checkNeighbourP() {
  // get currect sender from communication module
  node_t* cS = espC.getCurrentSender();
}
#endif /*RTI_RECTANGULAR_SCHEME*/
#if defined(RTI_CUSTOM_SCHEME)
void RTI::checkNeighbourP() {
  // get currect sender from communication module
  node_t* cS = espC.getCurrentSender();
}
#endif /*RTI_CUSTOM_SCHEME*/
