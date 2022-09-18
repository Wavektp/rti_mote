#include "rti.h"

void start_rti();

void rti::begin() {
  next.NID = NET_PREFIX;
  byte dID = DEVICE_ID + 1;
  if (dID <= RTI_NODE_COUNT) {
    next.DID = dID;
  } else {
    next.DID = 0;
  }
#ifndef ROOT_NODE
  #if RTI_SCHEME == RTI_DEFAULT_SCHEME
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
  #if RTI_SCHEME == RTI_CUSTOM_SCHEME
    #error In CUSTOM SCHEME, neighbours need to be manually defined
  #endif
#else  /*ROOT_NODE*/
  delay(START_DELAY);
  start_rti();
#endif /*ROOT_NODE*/
}

void rti::msgToStr(message_t* msg, char* str) {
  char st[RTI_STR_SIZE];
  sniprintf(st, RTI_PREFIX_STR_SIZE, RTI_PREFIX_STR, msg->type, msg->msgID,
            msg->sNID, msg->sDID, msg->rNID, msg->rDID, msg->nNID, msg->nDID);
  if (msg->content[0] != RTI_MSG_MASK_RSS) {
    out("Error: Invalid Frame Format: not found RSS PREFIX");
  }
  size_t neighbourCount = (msg->len / 2 - 1);
  for (int i = 1; i < (neighbourCount + 1); i++) {
    char temp[RTI_RSS_STR_SIZE];
    sniprintf(st, RTI_RSS_STR_SIZE, RTI_RSS_STR, i, msg->content[i]);
    strcat(st, temp);
  }
  for (int i = (neighbourCount + 2); i < (2 * neighbourCount + 2); i++) {
    char temp[RTI_IR_STR_SIZE];
    sniprintf(st, RTI_IR_STR_SIZE, RTI_IR_STR, (i - (neighbourCount + 2)),
              msg->content[i]);
    strcat(st, temp);
  }
  strcpy(str, st);
}

void rti::create_rti_message(message_t* msg, byte type, bool isCompleted) {
  /*create message prefix*/
  msg->type = type;
  msg->sNID = NET_PREFIX;
  msg->sDID = DEVICE_ID;
  msg->rNID = BROADCAST_CODE;
  msg->rDID = BROADCAST_CODE;
  msg->nNID = next.NID;
  msg->nDID = next.DID;
#ifdef ROOT_NODE
  if (type == MESSAGE_TYPE_ฺBEACON) {
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
    }
    msg->content[RTI_NEIGHBOUR_COUNT + 1] = RTI_MSG_MASK_IR;
    for (int i = (RTI_NEIGHBOUR_COUNT + 2); i < (2 * RTI_NEIGHBOUR_COUNT + 2);
         i++) {
      msg->content[i] = neighbour[i - (RTI_NEIGHBOUR_COUNT + 2)].irRSS;
    }
  }
#endif /*END_DEVICE*/
}
#ifdef ROOT_NODE
void start_rti() {
  // TODO Algorithm to send and receive RTI FRAME
}
#endif /*ROOT_NODE*/
