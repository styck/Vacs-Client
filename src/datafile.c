//////////////////////////////////////////////////////////////////////
// VACS_CLient project.
// Copyright CorTek Software, Inc. 1997-1998. All rights reserved.
//

#include "SAMM.H"
#include "SAMMEXT.h"
#include "macro.h"
#include <winioctl.h>




extern LPDLROOTPTR g_pdlrSequence;
extern int				 giMemControlCount;


typedef struct
{
  DWORD               dwID;
  DWORD               dwSize;     // header size
  DWORD               dwVersion;  // Header Version
  DEVICE_SETUP_DATA   DeviceSetup;// The device setup, so we can validate
                                  // the data coming in !!! Different device 
                                  // setups can not share the same Device
                                  // control information!
  DWORD               adwResrved[4];
}VACS_DATA_FILE_HEADER;

                                                         // VacS Data File
#define     VACS_DATA_FILE_HEADER_ID_def    makeDWORD_ID('V','S','D','F')
#define     VACS_DATA_FILE_HEADER_def       makeDWORD_ID('0','0','0','1')

HANDLE      g_hDataFile = INVALID_HANDLE_VALUE;
DWORD       g_dwDataFileWOffset = 0;


BOOL	UpdateWithPropagate(DWORD    dwReadOffset, long itemPosition);
BOOL	FillUpdateMaskBuffer(WORD *gpwMemMapUpdateBufferMask, WORD *gpwMemMapMixer);

/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: CreateApplicationDirectories
//
//
//
BOOL    CreateApplicationDirectories(void)
{
  BOOL      bRet = FALSE;
  char      szAppDirs[MAX_PATH];

  // DATA directory
  //
  wsprintf(szAppDirs,"%sdata",gszProgDir);

  if(CreateDirectory(szAppDirs, NULL) == TRUE)
  {
    bRet = TRUE;
  }
  else
  {
    if(GetLastError() == ERROR_ALREADY_EXISTS)
      bRet = TRUE;
  }

  // TABLE directory
  //
  wsprintf(szAppDirs,"%stable",gszProgDir);

  if(CreateDirectory(szAppDirs, NULL) == TRUE)
  {
    bRet = TRUE;
  }
  else
  {
    if(GetLastError() == ERROR_ALREADY_EXISTS)
      bRet = TRUE;
  }
  
  // MIX  directory
  //
  wsprintf(szAppDirs,"%mix",gszProgDir);

  if(CreateDirectory(szAppDirs, NULL) == TRUE)
  {
    bRet = TRUE;
  }
  else
  {
    if(GetLastError() == ERROR_ALREADY_EXISTS)
      bRet = TRUE;
  }

  return bRet;
}

/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: 
//
//
//
BOOL    CompareDeviceSetup(DEVICE_SETUP_DATA *pSrc, DEVICE_SETUP_DATA *pData)
{
  BOOL    bRet = FALSE;
  LPSTR   pEnd = (LPSTR)pData->iaChannelTypes + sizeof(pData->iaChannelTypes); 
  LPSTR   pDataWalker = (LPSTR)pData->iaChannelTypes;
  LPSTR   pSrcWalker = (LPSTR)pSrc->iaChannelTypes;


  while(pDataWalker++ < pEnd)
  {
    if(*pDataWalker != *pSrcWalker)
      goto ON_EXIT;
    pSrcWalker ++;
  }
  bRet = TRUE;

ON_EXIT:
  return bRet;
}

/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: CreateDataFile
//
//
//


BOOL    CreateDataFile(LPSTR  lpstrFName)
{
  BOOL                  bRet = FALSE;
  VACS_DATA_FILE_HEADER dfHeader = {0};
  DWORD                 dwRead;
  DWORD                 dwWrite;
  DWORD                 dwHiSize;
	USHORT								compression;


	// Lets make sure it is closed

	CloseDataFile();

  g_hDataFile = CreateFile(lpstrFName, GENERIC_WRITE | GENERIC_READ, 
                            0, NULL, OPEN_ALWAYS, 
                            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                            NULL);
  
		compression = COMPRESSION_FORMAT_LZNT1;
		DeviceIoControl (g_hDataFile, FSCTL_SET_COMPRESSION, &compression, sizeof (compression), 0, 0, 
										 &dwWrite, 0);

		dfHeader.dwID = VACS_DATA_FILE_HEADER_ID_def;
		dfHeader.dwSize = sizeof(VACS_DATA_FILE_HEADER);
		dfHeader.dwVersion = VACS_DATA_FILE_HEADER_def;
		CopyMemory((void*)&dfHeader.DeviceSetup, &gDeviceSetup, sizeof(DEVICE_SETUP_DATA));


		SetFilePointer(g_hDataFile, 0, 0, FILE_BEGIN);
		WriteFile(g_hDataFile, &dfHeader, sizeof(VACS_DATA_FILE_HEADER), &dwWrite, NULL);
		SetEndOfFile(g_hDataFile); // make the file size correct
		g_dwDataFileWOffset = GetFileSize(g_hDataFile, &dwHiSize);
		bRet = TRUE;

  return bRet;
}



/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: OpenDataFile
//
//
//


BOOL    OpenDataFile(LPSTR  lpstrFName)
{
  BOOL                  bRet = FALSE;
  VACS_DATA_FILE_HEADER dfHeader = {0};
  DWORD                 dwRead;
  DWORD                 dwWrite;
  DWORD                 dwHiSize;
	USHORT								compression;


  g_hDataFile = CreateFile(lpstrFName, GENERIC_WRITE | GENERIC_READ, 
                            0, NULL, OPEN_ALWAYS, 
                            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                            NULL);

		// cool ... now set the compression on this file

		if (g_hDataFile != INVALID_HANDLE_VALUE){
			compression = COMPRESSION_FORMAT_LZNT1;
			DeviceIoControl (g_hDataFile, FSCTL_SET_COMPRESSION, &compression, sizeof (compression), 0, 0, 
											 &dwWrite, 0);
    }



  if(g_hDataFile != INVALID_HANDLE_VALUE)
  {
    ReadFile(g_hDataFile, &dfHeader, sizeof(VACS_DATA_FILE_HEADER), &dwRead, NULL);

    // validate the header ...
    //
    if((dfHeader.dwID == VACS_DATA_FILE_HEADER_ID_def) && 
       (dfHeader.dwSize == sizeof(VACS_DATA_FILE_HEADER)) &&
			 dwRead == sizeof(VACS_DATA_FILE_HEADER))
		{
			// validate the Device setup data ...
			// before setting global offset
			if(!CompareDeviceSetup(&gDeviceSetup , &dfHeader.DeviceSetup))
			{
				g_dwDataFileWOffset = GetFileSize(g_hDataFile, &dwHiSize);
			}
			else
			{
				// Overwrite file and file header
				//
//				CreateDataFile(lpstrFName);
    bRet = FALSE;
			}
		}
		else
		{
				// Overwrite file and file header
				//
//				CreateDataFile(lpstrFName);
    bRet = FALSE;

		}
  }
	else
	{
    bRet = FALSE;
	}

    // Everything is OK so lets get ready to work !!!
    //
    bRet = TRUE;

  return bRet;
}



/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: CloseDataFile
//
//
//
BOOL    CloseDataFile(void)
{
  BOOL    bRet = TRUE;

  if(g_hDataFile != INVALID_HANDLE_VALUE)
  {
    CloseHandle(g_hDataFile);
    g_hDataFile = INVALID_HANDLE_VALUE;
  }


  return bRet;
}


/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: 
//
//
//

DWORD   GetWriteOffset(void)
{
  return g_dwDataFileWOffset;
}

/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: 
//
//
//
BOOL  ReadDataFile(DWORD    dwReadOffset)
{
  BOOL    bRet = FALSE;
  DWORD   dwRead;

  if(g_hDataFile != INVALID_HANDLE_VALUE)
  {
    if(SetFilePointer(g_hDataFile, dwReadOffset, 0, FILE_BEGIN) != 0xFFFFFFFF)
    {
      ReadFile(g_hDataFile, gpwMemMapBuffer, giMemMapSize, &dwRead, NULL);  
      if(giMemMapSize == (int)dwRead)
      {
        //MoveMemory(gpwMemMapMixer, gpwMemMapBuffer, giMemMapSize);
        bRet = TRUE;
      }
    };
  }
  return bRet;
}

/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: 
//
//
//
BOOL  UpdateDataFile(DWORD    dwReadOffset, long itemPosition)
{
  BOOL  bRet = FALSE;
  DWORD dwWrite;

  if(g_hDataFile != INVALID_HANDLE_VALUE)
  {
    if(SetFilePointer(g_hDataFile, dwReadOffset, 0, FILE_BEGIN) != 0xFFFFFFFF)
    {
			if(g_SeqPropagate.iPropWhat == PROP_NONE)
			{
				WriteFile(g_hDataFile, gpwMemMapMixer, giMemMapSize, &dwWrite, NULL);  
				if(giMemMapSize == (int)dwWrite)
					bRet = TRUE;
			}
			else
			{
				bRet = UpdateWithPropagate(dwReadOffset, itemPosition);
			}
    };
  }
  return bRet;
}


/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: 
//
//
//
BOOL    AddToDataFile(DWORD   *pdw)
{
  BOOL  bRet = FALSE;
  DWORD dwWrite;

  if(g_hDataFile != INVALID_HANDLE_VALUE)
  {
    *pdw = SetFilePointer(g_hDataFile, 0, 0, FILE_END);
    if( *pdw != 0xFFFFFFFF)
    {
      WriteFile(g_hDataFile, gpwMemMapMixer, giMemMapSize, &dwWrite, NULL);  
      if(giMemMapSize == (int)dwWrite)
        bRet = TRUE;
    };
  }
  return bRet;
}

//////////////////////////
//
//
BOOL	UpdateWithPropagate(DWORD    dwReadOffset, long itemPosition)
{
	BOOL						bRet = TRUE;
	long            lItemPos = itemPosition; 
	long						lLinkState;
	LPSEQENTRY      p_seqentry;
	DWORD						dwRead, dwWrite;
	int							iIdx;
	WORD						*pMaskWalker = gpwMemMapUpdateBufferMask;
	WORD						*pDestWalker = gpwMemMapUpdateBuffer;
	DWORD						dwUpdates = 0;
	
	if(g_hDataFile == INVALID_HANDLE_VALUE)
		return FALSE;

	// Clean up the buffer
	FillMemory(gpwMemMapUpdateBufferMask, giMemMapSize, 0xFF);
	if(FillUpdateMaskBuffer(gpwMemMapUpdateBufferMask, gpwMemMapMixer) == FALSE) return FALSE;

  while(lItemPos > 0)
  {
		// find the entry offset into the linked list ...
		lLinkState = GetEntryLinkState(g_pdlrSequence, lItemPos);
		p_seqentry = (LPSEQENTRY)GetEntryData(g_pdlrSequence, lItemPos);
		if(p_seqentry == NULL)
			break;
		
    if(SetFilePointer(g_hDataFile, p_seqentry->dwOffset, 0, FILE_BEGIN) != 0xFFFFFFFF)
		{
      ReadFile(g_hDataFile, gpwMemMapUpdateBuffer, giMemMapSize, &dwRead, NULL);  
      if(giMemMapSize != (int)dwRead) return FALSE;

			pMaskWalker = gpwMemMapUpdateBufferMask; 
			pDestWalker = gpwMemMapUpdateBuffer;
			dwUpdates = 0;
			// now scan the data an update it ....
			for(iIdx = 0; iIdx < giMemControlCount; iIdx++)
			{
				if(*pMaskWalker != 0xFFFF)
				{
					*pDestWalker = *pMaskWalker;
					dwUpdates ++;
				}
				pMaskWalker++;
				pDestWalker++;
			}
			
			SetFilePointer(g_hDataFile, p_seqentry->dwOffset, 0, FILE_BEGIN);
			WriteFile(g_hDataFile, gpwMemMapUpdateBuffer, giMemMapSize, &dwWrite, NULL);  

		}
		else
			return FALSE;

		lItemPos = GetNextEntry(g_pdlrSequence, lItemPos);
	}

	return bRet;
}


BOOL	FillUpdateMaskBuffer(WORD *pwMemMapUpdateBufferMask, WORD *pwMemMapMixer)
{
	BOOL						bRet = FALSE;
	int							iIdx;
	WORD						*pMaskWalker = pwMemMapUpdateBufferMask;
	WORD						*pSrcWalker = pwMemMapMixer;
	WORD						*pLastBuffer = gpwMemMapBuffer;
	DWORD						dwUpdate = 0;


	if(g_SeqPropagate.iPropWhat == PROP_ALL)
	{
		for(iIdx = 0; iIdx < giMemControlCount; iIdx++)
		{
			// check if things are different and update the Mask buffer ...
			//
			if(*pSrcWalker != *pLastBuffer)
			{
				*pMaskWalker = *pSrcWalker;
				dwUpdate++;
			}
			pMaskWalker++;
			pSrcWalker++;
			pLastBuffer ++;
		}
		
		bRet = TRUE;
	}


	return bRet;
};