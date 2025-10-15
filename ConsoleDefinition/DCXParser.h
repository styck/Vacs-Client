////////////////////////////////////////////////////////////////////////
// file DCXParser.h
//
// general stuff for the parsing routines for the DCX
//
// Copyright 1998 CorTek Software, Inc.

#ifndef DCXPARSER_H_
#define	DCXPARSER_H_


#define DCX_DEVMAP_ID_STR         "DCX_DEVMAP"
#define DCX_DEVMAP_MAXSIZE        80						// Maximum number of modules

#define DCX_DEVMAP_MODULE_NA			0
#define DCX_DEVMAP_MODULE_INPUT		1
#define DCX_DEVMAP_MODULE_AUX			2
#define DCX_DEVMAP_MODULE_MATRIX	3
#define DCX_DEVMAP_MODULE_CUE			4
#define DCX_DEVMAP_MODULE_MASTER	5

#pragma pack(1) // pack on a byte boundry

#define   DCX_FILE_VERSION (DWORD)0x00010000 // the version

typedef struct
  {
  char          szRdOut[16];       // the Readout of this Control
  char          achPotVal[6][4];   // the Pot Values ...
                                   // max of six pots per control
                                   // with 3 valid characters as a value
                                   // for each one of them
  } DCXCTRLENTRY;
typedef DCXCTRLENTRY * LPDCXCTRLENTRY;

typedef struct
  {
  int           iSize;					 // the size of the Entire buffer
  char          szName[25];			 // the name of the Control
  int           iCtrlID;				 // Assigned ID for this Control .. so we can find it fast
  int           iREG0;					 // the 0 value for this control ... this is an index into the table of
																 // possible values
  int           iNumPots;				 // the number of pots used by this Control
  char          achChip[6][3];	 // the Chip number for this control in string format!
  char          achPot[6][2];    // the Pots used by this Control
  int           iNumEntr;				 // the number of entries for the lookup of this control
  } DCX_CTRL_DESC;

typedef DCX_CTRL_DESC  * LPDCX_CTRL_DESC;

typedef struct
  {
  DWORD         dwID;
  DWORD         dwVersion;
  int           iSize; // size of the header alone
  int           iCtrlCount; // count of the Controls described in this file
  int           iMaxCtrlSteps;
  DWORD         dwReserved[10]; // yea ... nothing yet
  } DCX_CTRL_FILE_HDR;

typedef DCX_CTRL_FILE_HDR * LPDCX_CTRL_FILE_HDR;

typedef struct
  {
  int               iCtrlID; // the Control type
  LPDCX_CTRL_DESC   pCtrlDesc; // the pointer for where in memory is this control description
  }DCX_MAP_ENTRY;
typedef DCX_MAP_ENTRY * LPDCX_MAP_ENTRY;

typedef struct
  {
  DCX_CTRL_FILE_HDR     dcxHdr; // this is identical to the DCX control File header
  LPDCX_CTRL_DESC       pDcxCtrls; // Buffer with all of the controls in it ... directly read from the File
  LPDCX_MAP_ENTRY       pDcxMap; // map for all of the Controls
  }DCX_MEM_MAP;

typedef DCX_MEM_MAP * LPDCX_MEM_MAP;


//////////////////////////////////////////////////////
// WARNING
//
// If you change the structure below then you need to
// change the same structure in the client software.
//

typedef struct
{
	WORD		wAddr;							// the VUthread Reads from here(module address)
//	BYTE	iVUType;					// the VUthread Reads from here(Pre, Post, Comp, Gate)
	char    cLock;							// if Zero nobody monitors this VU, so we don't need to read the damn thnig
	WORD    wVUValue[8];		    // data. VUthread writes here
	WORD    wPeakClipValue;			// data
  WORD    wModuleIdx;
}VU_READ;


#pragma pack() // default packing


// The Control Data(Complete)
//---------------------------
// The Control Data(Complete)
//---------------------------
// CONTROLDATA structure is already defined in other headers
// We use the existing definition instead of redefining it
// This avoids the "redefinition" compilation error

#define     NEWLINE "\n\0"
#define     TABCHAR '\t'


#define     INCREMENT_WALK_POINTER  { \
                                    lpWalk = lpDelim + strlen(NEWLINE);\
                                    if(lpWalk >= lpEnd)\
                                      goto ON_EXIT;\
                                    }

#define     GET_NEXT_LINE {\
                          ZeroMemory(szLine, 512);\
                          if(lpWalk)\
                            lpDelim = strstr(lpWalk, NEWLINE);\
                          else\
                            lpDelim = NULL;\
                          if(lpDelim)\
                            {\
                            CopyMemory(szLine, lpWalk, lpDelim - lpWalk - 1);\
                            lpWalk = lpDelim + strlen(NEWLINE);\
                            if(lpWalk >= lpEnd)\
                              lpWalk = NULL;\
                            }\
                          else\
                            {\
                            if(lpWalk < lpEnd)\
                              if( (lpEnd - lpWalk) < 512)\
                                {\
                                CopyMemory(szLine, lpWalk, lpEnd - lpWalk);\
                                lpDelim = lpWalk;\
                                lpWalk = NULL;\
                                }\
                            }\
                          }

#endif
