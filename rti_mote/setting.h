/***********************************************************
 *                  DEVICE SETTING                         *
 ***********************************************************/
#define NET_PREFIX 0x10
#define DEVICE_ID  1
#if (DEVICE_ID == 0)
  #define ROOT_NODE  // ROOT NODE
#else
  #define END_DEVICE  // END DEVICE
#endif
#define RESET_TIMEOUT 1000
/***********************************************************
 *                  RTI SETTING                            *
 ***********************************************************/
#define RTI_NODE_COUNT 10
#if DEVICE_ID > RTI_NODE_COUNT
  #error DEVICE_ID must be in range of NODE COUNT
#endif /*DEVICE_ID > RTI_NODE_COUNT*/
/**
 * RTI_DEFAULT_SCHEME     0
 * RTI_SIDEWAY_SCHEME     1
 * RTI_RECTANGULAR_SCHEME 2
 * RTI_CUSTOM_SCHEME      3
 **/
#define RTI_SCHEME 1
#define DEBUG_REPORT
