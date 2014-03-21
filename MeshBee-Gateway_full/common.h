/*
 * common.h
 * common defination of Gateway for SeeedStudio Mesh Bee(Zigbee) module
 *
 * Copyright (c) Seeed Studio. 2014.
 * Author     : Oliver Wang
 * Create Time: 2014/03
 * Change Log :
*/
#ifndef _COMMON_H
 
 /* 
   Distributed_MeshBee_dev is a public device which everybody can 
   access to its page through xively
 */
#define XI_FEED_ID 2123063079                                           // set Xively Feed ID (numerical, no quoutes)
#define XI_API_KEY "losJrpJ4PbbWrckZXruufhYiBvq32ZAskuBeRFhWwQZkTepv"   // set Xively API key (double-quoted string)

//sensor data verify byte
#define VERIFY_BYTE                     0x3e

#define TX_BUF_LEN      40
#define RX_BUF_LEN      40

#define RXFIFOLEN       128
#define THRESHOLD_READ 40

#define DATA_POINT_NUM  1

#define OK      1        
#define ERR     0
#define TRUE    1
#define FALSE   0

#define MIN(A,B)                ( ( ( A ) < ( B ) ) ? ( A ) : ( B ) )

#define Mask  0x00FF

typedef signed char             int8;
typedef short                   int16;
typedef long                    int32;
typedef long long               int64;
typedef unsigned char           uint8;
typedef unsigned short          uint16;
typedef unsigned long           uint32;
typedef unsigned long long      uint64;
    



#endif