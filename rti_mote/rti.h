#ifndef RTI_H
#define RTI_H

#include "esp_comm.h"
#include "ir_comm.h"
/************************************************************************
 *  RTI SCHEME DEFINITIONS                                              *
 ************************************************************************/
#if RTI_SCHEME == 0
  #define RTI_DEFAULT_SCHEME
  #define RTI_NEIGHBOUR_COUNT (RTI_NODE_COUNT - 1)
#endif /*RTI_SCHEME == RTI_DEFAULT_SCHEME*/
#if RTI_SCHEME == 1
  #define RTI_SIDEWAY_SCHEME
  #if ((RTI_NODE_COUNT % 2) == 0)
    #define RTI_NEIGHBOUR_COUNT RTI_NODE_COUNT / 2
  #else
    #error NODE COUNT in SIDEWAY scheme must be even
  #endif /*(RTI_NODE_COUNT%2)!= 0*/
#endif   /*RTI_SCHEME == RTI_SIDEWAY_SCHEME*/
#if RTI_SCHEME == 2
  #define RTI_RECTANGULAR_SCHEME
  #if ((RTI_NODE_COUNT % 4) == 0)
    #define SIDE_NODE_COUNT (RTI_NODE_COUNT / 4)
    #if ((DEVICE_ID % SIDE_NODE_COUNT) == 1)
      #define RTI_NEIGHBOUR_COUNT (2 * SIDE_NODE_COUNT - 1)
    #else
      #define RTI_NEIGHBOUR_COUNT (3 * SIDE_NODE_COUNT - 1)
    #endif
    #define RECTANGULAR_SIDE ((DEVICE_ID - 1) / SIDE_NODE_COUNT + 1)
  #else
    #error NODE COUNT modulo 4 must be 0 in RECTANGULAR scheme
  #endif /*(RTI_NODE_COUNT % 4)== 0*/
#endif   /*RTI_SIDEWAY_SCHEME == RTI_RECTANGULAR_SCHEME*/
#if RTI_SCHEME == 3
  #define RTI_CUSTOM_SCHEME
  #define RTI_NODE_COUNT /*Must be manually assigned*/
#endif                   /*RTI_CUSTOM_SCHEME*/
#if defined(RTI_SIDEWAY_SCHEME)
  #define ODD_SIDE_FLAG 0b00000001
#endif
/************************************************************************
 *  ROOT NODE DEFINITIONS                                               *
 ************************************************************************/
#if defined(ROOT_NODE)
  #define START_DELAY         2000
  #define RTI_NEIGHBOUR_COUNT RTI_NODE_COUNT
#endif /*ROOT_NODE*/
/************************************************************************
 *  NEIGHBOUR DEFINITION                                                *
 ************************************************************************/
#define MAX_RTI_NEIGHBOUR 50
#if RTI_NEIGHBOUR_COUNT > MAX_RTI_NEIGHBOUR
  #error NEIGHBOUR COUNT exceed max number
#endif

typedef struct {
  node_t node;
  int irRSS = 0;
  int RSS = 0;
} neighbour_t;

/************************************************************************
 *  RTI MESSAGE DEFINITION                                              *
 ************************************************************************/
#define RTI_MSG_MASK_RSS 255  // 0b10101010
#define RTI_MSG_MASK_IR  255  // 0b01010101
#define RTI_MSG_MASK_END 255  // 0b11001100
#define RTI_MSG_DELAY    1

#define RTI_PREFIX_STR_SIZE 38
#define RTI_PREFIX_STR      "<T:%02x><ID:%02x><S:%02x%02x><R:%02x%02x><N:%02x%02x>\n"
#define RTI_RSS_STR         "<N%02x: RSS%02x>\n"
#define RTI_RSS_STR_SIZE    13
#define RTI_IR_STR          "<N%02x: IR%02x>\n"
#define RTI_IR_STR_SIZE     12
#define RTI_STR_SIZE        250
/************************************************************************
 *  RTI CLASS                                                           *
 ************************************************************************/
typedef unsigned char rti_position_scheme_t;

typedef struct {
  volatile bool isNeighbourExist = false;
  volatile uint8_t neighbourP = 0;
  int tempRSSI = 0;
  int tempIR = 0;
  rti_position_scheme_t pos = 0;
  neighbour_t neighbour[RTI_NEIGHBOUR_COUNT];
} rti_info_t;

class RTI {
 private:
  /**
   * @brief Read current sender and check whether neighbour exist at which index
   */

 public:
#if defined(ROOT_NODE)
  void start_rti();
#endif /*ROOT_NODE*/
  void begin();
  void routine();
};
#endif /*RTI_H*/
