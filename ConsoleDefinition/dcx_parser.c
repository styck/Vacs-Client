////////////////////////////////////////////////////////////////
// file DCXparser.c
//
// Copyright 1998 CorTek Software, Inc.
//
//    This file provides a parsing routines for the DCX lookup
// tables(files)
////////////////////////////////////////////////////////////////

#include <windows.h>
#include <string.h>
#include <stdio.h>


#define _CONSOLE_DEFINITION_FILE_

#include  "dcxtcp_protocol.h"
#include  "DCXParser.h"



extern DCX_MEM_MAP     g_dcxMemMap;

////////////////////////////////////////////////////////////////////////
// FUNCTION: ReadDCXTableFile
//
//
int   ReadDCXTableFile(LPSTR  lpFName, LPDCX_MEM_MAP pMemMap)
{
int                 iRet;
LPDCX_CTRL_DESC     pdcxctrl; // buffer were the controls data will be stored
LPDCX_CTRL_DESC     pwalk; // buffer were the controls data will be stored
LPDCX_MAP_ENTRY     pdcxmap; // the lookup table for the controls position in memory
LPSTR               pend;
DCX_CTRL_FILE_HDR   dcxFileHeader = { 0 };
HANDLE              hf;
DWORD               dwRead;
int                 iCtrlCount;
DWORD               dwSize;

iRet = 1; // set it to error
pMemMap->pDcxCtrls = NULL; // ...
pMemMap->pDcxMap = NULL;
pdcxmap = NULL;
pdcxctrl = NULL;

hf = CreateFile(lpFName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

if( hf == INVALID_HANDLE_VALUE )
  goto ON_EXIT;


dwSize = GetFileSize(hf, NULL);
if(( dwSize == 0 ) || ( dwSize == 0xFFFFFFFF))
  goto ON_EXIT;

SetFilePointer(hf, 0, NULL, FILE_BEGIN);

ReadFile(hf, &dcxFileHeader, sizeof(DCX_CTRL_FILE_HDR), &dwRead, NULL);
if(dwRead != sizeof(DCX_CTRL_FILE_HDR))
  goto ON_EXIT;

// verify the File Header
//-----------------------
if( (dcxFileHeader.dwID != DCX_TCP_ID) || (dcxFileHeader.iSize != sizeof(DCX_CTRL_FILE_HDR)) )
  goto ON_EXIT;


// allocate the buffer to hold the File data
//------------------------------------------
dwSize -= sizeof(DCX_CTRL_FILE_HDR);
pdcxctrl = GlobalAlloc(GPTR, dwSize); // allocate a buffer to hold the rest of the file data
if(pdcxctrl == NULL)
  goto ON_EXIT;

// now read the Entire file in memory
//-----------------------------------
ReadFile(hf, pdcxctrl, dwSize, &dwRead, NULL);
if(dwRead != dwSize)
  goto ON_EXIT;

pwalk = pdcxctrl;
pend = (LPSTR)pdcxctrl + dwSize;

// allocate the index of the controls
//-----------------------------------
pdcxmap = GlobalAlloc(GPTR, sizeof(DCX_MAP_ENTRY) * dcxFileHeader.iCtrlCount);
if(pdcxmap == NULL) 
  goto ON_EXIT;

// loop to read all of the controls and load them into memory ...
//---------------------------------------------------------------
iCtrlCount = 0;
while( (LPSTR)pwalk < pend)
  {
  if(iCtrlCount > dcxFileHeader.iCtrlCount)
    // ERROR ERROR .... the file must be bad
    goto ON_EXIT;
  pdcxmap[iCtrlCount].iCtrlID   = pwalk->iCtrlID;
  pdcxmap[iCtrlCount].pCtrlDesc = pwalk; // store the pointer for the memory location of this control descriptor
  iCtrlCount++;
  (LPSTR)pwalk += pwalk->iSize;
  }

if( iCtrlCount != dcxFileHeader.iCtrlCount)
  {
  // the controls read is not the same as it's specified in the File Header
  // handle this ERROR
  //-----------------------------------------------------------------------

  }
else
  {
  // store the header information and the buffer pointers
  //-----------------------------------------------------
  pMemMap->dcxHdr     = dcxFileHeader;
  pMemMap->pDcxCtrls  = pdcxctrl;
  pMemMap->pDcxMap    = pdcxmap;
  iRet = 0;
  }

// ready to leave this function ....
//----------------------------------
ON_EXIT:
// Free the buffers if an error occured
// and they were already allocated
//-------------------------------------
if(iRet)
  {
  if(pdcxctrl)  
    GlobalFree( (HANDLE) pdcxctrl);
  if(pdcxmap)
    GlobalFree( (HANDLE) pdcxmap);
  }

if(hf != INVALID_HANDLE_VALUE)
  CloseHandle(hf);



return iRet;
};


/////////////////////////////////
// FUNCTIONS: FreeDCXMapBuffers
//
//
int     FreeDCXMapBuffers(LPDCX_MEM_MAP pMemMap)
{
if(pMemMap->pDcxCtrls)
  {
  GlobalFree((HANDLE)pMemMap->pDcxCtrls);
  pMemMap->pDcxCtrls = NULL;
  }
if(pMemMap->pDcxMap)
  {
  GlobalFree((HANDLE)pMemMap->pDcxMap);
  pMemMap->pDcxMap = NULL;
  }

ZeroMemory(&pMemMap->dcxHdr, sizeof(DCX_CTRL_FILE_HDR));

return 0;
}

