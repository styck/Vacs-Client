//////////////////////////////////////////////////////////////////////
// file DCXTCP_protocol.h
//
// definition of the TCP protocol messages, headers ...
// for Gamble - DCX ................
//
// Copyright 1998 CorTek Software, Inc.
//////////////////////////////////////////////////////////////////////


typedef struct
  {
  int     iID;      // Identify this as a valid DCX message
  WORD    wMessage; // what message
  WORD    wSize;    // sizeof the message
  MMTIME  mmt;      // time stamp
  int     ipad;
  } HDR_DCXTCP;
typedef HDR_DCXTCP  * LP_HDR_DCXTCP;

typedef struct 
{
  int   iAverage;           // the Average of the Data
  int   iPeak;              // the Peak of the Data
} SHORT_VU_METER_DATA;


#define   DCX_TCP_VERSION (DWORD)0x00010000 // the version
#define   DCX_TCP_ID      mmioFOURCC('G', 'D', 'C', 'X')
#define   NO_FLAGS_SET    0
#define   MAX_INET_ADDR   20
#define   MAX_NET_BUFFER  488 // Set after long tests with Francis !!!  

// Definition of the TCP Messages
#define   DCX_TCP_TIME          0x0001 // General Messages
#define   DCX_TCP_TIME_START    0x0002 // General Messages
#define   DCX_TCP_TIME_SET      0x0003 // General Messages

#define   DCX_GENERAL_STRING_MESSAGE  0x0010 // String messages ... chat like thing

#define   DCX_TCP_SEND_TABLE					0x0020	// request the dcx lookup table
#define   DCX_TCP_SEND_SETUP					0x0021	// Request the module setup to be send from the Server ..,
#define		DCX_TCP_SEND_CONTROL_STATES	0x0022	// Request all the current control settings	

#define   DCX_START_VU_DATA     0x0023 // Start sending VU data to a socket
#define   DCX_STOP_VU_DATA      0x0024 // Stop sending VU data to a socket
#define   DCX_SHOW_VU_DATA      0x0025 // 80 char bytes that will say what modules are on screen
                                       // so that the VU data can be sent for that module
#define		DCX_RESET_BUS					0x0026 // reset the DCX bus.


#define   DCX_VU_DATA           0x0100 // VU Data transmition  

#define   DCX_TCP_CONTROL_DATA  0x1000 // General Control Data
#define   DCX_SET_CONTROL_VALUE 0x1001 // a control data has to be set


#define   DCX_TCP_RECEIVE_TABLE 0x1020			// receive the dcx lookup table
#define   DCX_TCP_RECEIVE_TABLE_DONE 0x1021 // receive the dcx lookup table
#define   DCX_TCP_RECEIVE_SETUP 0x1022			// Tells to receive the setup

#define		DCX_TCP_RECEIVE_CONTROL_STATES	0x1023	// Tells client to receive all the current control settings	

#define   DCX_TCP_ERROR         0xF000 // the start offset of all ERROR messages

 