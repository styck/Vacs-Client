////////////////////////////////////////////////////////////////////////////////
// ConsoleDefinition implementation file  implement.c
//
//
//

#include <windows.h>

#define _CONSOLE_DEFINITION_FILE_

#include "DCXTCP_protocol.h"
#define CONTROLDATA      // This is very dumb but for some reason CONTROLDATA gets redefinition ERROR
#include  "..\..\GServer\DCXParser.h"
#undef CONTROLDATA
#include "ConsoleDefinition.h"
#include "CDef_External.h"
#include "CDef_Implement.h"
#include "ConsoleDefinitionRC.h"

extern BOOL		gPropertyDone;

////////////////////////////////
//
//
void FormatSockError(char *szBuffer, int iError)
{
  switch(iError)
  {
  case WSANOTINITIALISED:
    wsprintf(szBuffer, "WSANOTINITIALISED");
    break;
  case WSAENETDOWN:
    wsprintf(szBuffer, "WSAENETDOWN");
    break;
  case WSAEACCES:
    wsprintf(szBuffer, "WSAEACCES");
    break;
  case WSAEINTR:
    wsprintf(szBuffer, "WSAEINTR");
    break;
  case WSAEINPROGRESS:
    wsprintf(szBuffer, "WSAEINPROGRESS");
    break;
  case WSAEFAULT:
    wsprintf(szBuffer, "WSAEFAULT");
    break;
  case WSAENETRESET:
    wsprintf(szBuffer, "WSAENETRESET");
    break;
  case WSAENOBUFS:
    wsprintf(szBuffer, "WSAENOBUFS");
    break;
  case WSAENOTCONN:
    wsprintf(szBuffer, "WSAENOTCONN");
    break;
  case WSAENOTSOCK:
    wsprintf(szBuffer, "WSAENOTSOCK");
    break;
  case WSAEOPNOTSUPP:
    wsprintf(szBuffer, "WSAEOPNOTSUPP");
    break;
  case WSAESHUTDOWN:
    wsprintf(szBuffer, "WSAESHUTDOWN");
    break;
  case WSAEWOULDBLOCK:
    wsprintf(szBuffer, "WSAEWOULDBLOCK");
    break;
  case WSAEMSGSIZE:
    wsprintf(szBuffer, "WSAEMSGSIZE");
    break;
  case WSAEHOSTUNREACH:
    wsprintf(szBuffer, "WSAEHOSTUNREACH");
    break;
  case WSAEINVAL:
    wsprintf(szBuffer, "WSAEINVAL");
    break;
  case WSAECONNABORTED:
    wsprintf(szBuffer, "WSAECONNABORTED");
    break;
  case WSAECONNRESET:
    wsprintf(szBuffer, "WSAECONNRESET");
    break;
  case WSAETIMEDOUT:
    wsprintf(szBuffer, "WSAETIMEDOUT");
    break;

  default:
    wsprintf(szBuffer, "Unknown Socket Error!");
    break;
  }
  
};
HANDLE	g_running_mutex = NULL;
#define	VACS_IDENTIFIER_MUTEX "VACS_RUNNING_MUTEX"
//
int	CDef_isRunning (void)
{
	HANDLE	check_mutex;
	int			running = 0;

	check_mutex = CreateMutex (NULL, TRUE, VACS_IDENTIFIER_MUTEX);
	if (check_mutex == NULL)
		return 1;
	//
	if (GetLastError () == ERROR_ALREADY_EXISTS)
	{
		running = 1;
	}

	CloseHandle (check_mutex);

	return running;
}
//////////////////////////////////////////////////////////////////////
// FUNCTION: CDef_Init
//
// Initialize internal stuff
int     CDef_Init(void)
{

	g_running_mutex = CreateMutex (NULL, TRUE, VACS_IDENTIFIER_MUTEX);
	if (g_running_mutex == NULL)
		return 1;
	if (GetLastError () == ERROR_ALREADY_EXISTS)
	{
		CloseHandle (g_running_mutex);
		g_running_mutex = NULL;
	}

// Initialize the trhead that is going to handle the TCP protocol
g_threadIO = CreateThread(NULL, 0, ioThreadProc, NULL, THREAD_PRIORITY_NORMAL, &g_threadID);
if(g_threadIO == NULL)
  return 1; // Error

ResumeThread(g_threadIO);

// LoadPrefernces
if(LoadPreferences())
  {
  return 1;
  }
else
  {
/*
  // First off start the client and try to establish connection with the server ....
  //--------------------------------------------------------------------------------
  if(ReadDCXTableFile(g_pref.szTableFileName, &g_dcxMemMap))
    {
    return 1;
    }
*/
  }


return 0; // OK
};


//////////////////////////////////////////////////////////////////////
// FUNCTION: CDef_ShutDown
//
// Cleanup the mess
int     CDef_ShutDown(void)
{
if (g_running_mutex != NULL)
	CloseHandle (g_running_mutex);

if(g_hwndIO) 
  {
  SendMessage(g_hwndIO, WM_CLOSE, (WPARAM)0, (LPARAM)0);

  // there must a better way to determine when the thread has Exited
  // but for now this shoud do.....
  //----------------------------------------------------------------
  while(g_threadIO)
    Sleep(10);
  }

// free the other stuff
FreeDCXMapBuffers(&g_dcxMemMap);

// Save preferences ...
//---------------------
SavePreferences();


return  0; // OK
}


/////////////////////////////////////////////////////////////////////
// FUNCTION: CDef_SetGlobalData
//
// Set the main Window handle, main instance handle, 
// and default transfer function( which allows the DLL
// to talk back to the application)
int     CDef_SetGlobalData(LPDEVICE_SETUP_DATA pDevSetupData)
{
  g_pDeviceSetupData = pDevSetupData;
  g_hwndApp = pDevSetupData->hwndMain;
  g_hinstApp = pDevSetupData->hinst;
  g_xcomm_main = pDevSetupData->pfTXComm;

return 0; // OK
}



//////////////////////////////////////////////////////////////////////
// FUNCTION CDef_Version
//
// return the Version of this DLL
// NOTE: all versions of the same number should export the same 
// functions.
int      CDef_Version(void)
{

return CONSOLE_DEFINITION_VERSION;
}

/////////////////////////////////////////////////////////////////////
// FUNCTION: CDef_Preferences
//
//
//
int   CDef_Preferences(HWND hwnd)
{
int           iResult;
PROPSHEETPAGE psp[3];
PROPSHEETHEADER psh;


//if((g_hwndApp == NULL) || (g_hinstApp == NULL))
if((hwnd == NULL) || (g_hinstApp == NULL))
  return 1; // Error

ZeroMemory (&psh, sizeof (psh));
ZeroMemory (psp, sizeof (psp));
gPropertyDone = FALSE;

psp[0].dwSize = sizeof(PROPSHEETPAGE);
psp[0].dwFlags = 0;
psp[0].hInstance = g_hinstCDef;    
psp[0].pszTemplate = MAKEINTRESOURCE(IDD_CONFIGURATION);
psp[0].pszIcon = NULL;
psp[0].pfnDlgProc = dlgConfig;
psp[0].pszTitle = NULL;
psp[0].lParam = 0;

psp[1].dwSize = sizeof(PROPSHEETPAGE);
psp[1].dwFlags = 0;
psp[1].hInstance = g_hinstCDef;      
psp[1].pszTemplate = MAKEINTRESOURCE(IDD_TABLE);
psp[1].pszIcon = NULL;
psp[1].pfnDlgProc = dlgTable;
psp[1].pszTitle = NULL;
psp[1].lParam = 0;

psp[2].dwSize = sizeof(PROPSHEETPAGE);
psp[2].dwFlags = 0;
psp[2].hInstance = g_hinstCDef;      
psp[2].pszTemplate = MAKEINTRESOURCE(IDD_ABOUT_CONSOLEDEFINITION);
psp[2].pszIcon = NULL;
psp[2].pfnDlgProc = dlgAbout;
psp[2].pszTitle = NULL;
psp[2].lParam = 0;

psh.dwSize = sizeof(PROPSHEETHEADER);
psh.dwFlags = PSH_PROPSHEETPAGE;// |PSH_DEFAULT;
psh.hwndParent = hwnd;//g_hwndApp;
psh.hInstance = g_hinstApp;
psh.pszIcon = NULL;
psh.pszCaption = (LPSTR) "DCX Console Definition Setup";
psh.nPages = sizeof(psp) / sizeof(PROPSHEETPAGE);
psh.ppsp = (LPCPROPSHEETPAGE) &psp;

iResult = PropertySheet(&psh);

while (gPropertyDone == FALSE){
	Sleep (10);
}

return 0; // OK
}

/////////////////////////////////////////////////////////////////
// FUNCTION: CDef_SendData
//
//note: this is the interface function that is called by
// SAC to send the data out ...
int   CDef_SendData(LPCONTROLDATA lpCD)
{
LP_HDR_DCXTCP    phdrTCP;
char             chBuff[255];

phdrTCP = (LP_HDR_DCXTCP)chBuff;
phdrTCP->iID      = DCX_TCP_ID;
phdrTCP->wMessage = DCX_SET_CONTROL_VALUE;
phdrTCP->wSize    = sizeof(CONTROLDATA);
phdrTCP->mmt.u.ms = 0;

CopyMemory(chBuff + sizeof(HDR_DCXTCP), lpCD, sizeof(CONTROLDATA));

//SendData((LPSTR)lpCD, sizeof(CONTROLDATA));
SendData(chBuff, sizeof(HDR_DCXTCP)+sizeof(CONTROLDATA));

return 0; // OK
};

void	CDef_ResetBus(void)
{
	HDR_DCXTCP			 hdrTCP;

	hdrTCP.iID      = DCX_TCP_ID;
	hdrTCP.wMessage = DCX_RESET_BUS;
	hdrTCP.wSize    = 0;
	hdrTCP.mmt.u.ms = 0;

	//SendData((LPSTR)lpCD, sizeof(CONTROLDATA));
	SendData((LPSTR)&hdrTCP, sizeof(HDR_DCXTCP));
};
//////////////////////////////////////////////////////////////////
//
//
//
int		CDef_RequestServerSetupData(void)
{
	HDR_DCXTCP       hdrTCP;

	hdrTCP.iID      = DCX_TCP_ID;
	hdrTCP.wMessage = DCX_TCP_SEND_SETUP;
	hdrTCP.wSize    = 0;
	hdrTCP.mmt.u.ms = 0;

	SendData(( LPSTR) &hdrTCP,sizeof(HDR_DCXTCP));
  return 0;
}

//////////////////////////////////////////////////////////////////
//
//
//
int		CDef_RequestServerTableData(void)
{
	HDR_DCXTCP       hdrTCP;

	hdrTCP.iID      = DCX_TCP_ID;
	hdrTCP.wMessage = DCX_TCP_SEND_TABLE;
	hdrTCP.wSize    = 0;
	hdrTCP.mmt.u.ms = 0;

	SendData(( LPSTR) &hdrTCP,sizeof(HDR_DCXTCP));
  return 0;
}

//////////////////////////////////////////////////////////////////
//FUNCTION: CDef_StartVuData
//
//
int		CDef_StartVuData(void)
{
	HDR_DCXTCP       hdrTCP;

	hdrTCP.iID      = DCX_TCP_ID;
	hdrTCP.wMessage = DCX_START_VU_DATA;
	hdrTCP.wSize    = 0;
	hdrTCP.mmt.u.ms = 0;

	SendData(( LPSTR) &hdrTCP,sizeof(HDR_DCXTCP));
  return 0;
}


//////////////////////////////////////////////////////////////////
//FUNCTION: CDef_ShowVuData
//
// Tells the server what modules to send VU data from
// buffer must point to 80 bytes of valid data
//
int		CDef_ShowVuData(char *buffer)
{
  LP_HDR_DCXTCP    phdrTCP;
  char             chBuff[255];

  phdrTCP = (LP_HDR_DCXTCP)chBuff;

	phdrTCP->iID      = DCX_TCP_ID;
	phdrTCP->wMessage = DCX_SHOW_VU_DATA;
	phdrTCP->wSize    = 80;
	phdrTCP->mmt.u.ms = 0;

  CopyMemory(chBuff + sizeof(HDR_DCXTCP), buffer, 80);

   SendData(( LPSTR) chBuff, sizeof(HDR_DCXTCP)+80);
  return 0;
}

//////////////////////////////////////////////////////////////////
//FUNCTION: CDef_StopVuData
//
//
int		CDef_StopVuData(void)
{
	HDR_DCXTCP       hdrTCP;

	hdrTCP.iID      = DCX_TCP_ID;
	hdrTCP.wMessage = DCX_STOP_VU_DATA;
	hdrTCP.wSize    = 0;
	hdrTCP.mmt.u.ms = 0;

	SendData(( LPSTR) &hdrTCP,sizeof(HDR_DCXTCP));
  return 0;
}



//////////////////////////////////////////////////////////////////
//FUNCTION: SendData
//
// internal use only ....
//
void    SendData(LPSTR lpBuff, int iSize)
{
  int   iSent;
  int   iError;
  char  szBuffer[64];

  
  if( g_socket != INVALID_SOCKET )
  {
    if( iSize > 0)
    {
SEND_DATA:
      iSent = send(g_socket, lpBuff, iSize, MSG_DONTROUTE);
      

      if(iSent == SOCKET_ERROR || iSent != iSize)
      {
        iError = WSAGetLastError();
        FormatSockError(szBuffer, iError);
        if(iError == WSAEWOULDBLOCK)
        {
          Sleep(10);
          goto SEND_DATA;
        }
      }
      else
        g_dwSentBytes += iSent;
    }
  }

return;
};

//////////////////////////////////////////////////////////////////////
// FUNCTION: int      CDef_GetCtrlDefaultVal(int)
//
// return the default value for this control
int      CDef_GetCtrlDefaultVal(int iCtrlIdx)
{
int iRet;
LPDCX_CTRL_DESC     pdcxctrl; // the lookup table for the controls position in memory

iRet = 0;

if( (iCtrlIdx >= 0) && (iCtrlIdx < g_dcxMemMap.dcxHdr.iCtrlCount))
  {
  pdcxctrl = g_dcxMemMap.pDcxMap[iCtrlIdx].pCtrlDesc;
  iRet = pdcxctrl->iREG0;
  }

return iRet;
}

////////////////////////////////////////////////////////////////////////
// FUNCTION: CDef_GetCtrlMaxVal
//
// if iCtrlIdx is invalid the return value is 0
//
int   CDef_GetCtrlMaxVal(int iCtrlIdx)
{
int                 iRet;
LPDCX_CTRL_DESC     pdcxctrl; // the lookup table for the controls position in memory

iRet = 2;

if( (iCtrlIdx >= 0) && (iCtrlIdx < g_dcxMemMap.dcxHdr.iCtrlCount))
  {
  pdcxctrl = g_dcxMemMap.pDcxMap[iCtrlIdx].pCtrlDesc;
  iRet = pdcxctrl->iNumEntr;
  }

return iRet;
}

////////////////////////////////////////////////////////////////////////
// FUNCTION: CDef_GetCtrlMinVal
//
// the return value is 0
//
int   CDef_GetCtrlMinVal(int iCtrlIdx)
{

  return 0;
}

////////////////////////////////////////////////////////////////////////
// FUNCTION: CDef_GetCtrlReadout
//
// stores the radout for a value for a given control
//
int   CDef_GetCtrlReadout(int iCtrlIdx, int iVal, LPSTR szRdOut)
{
LPDCX_CTRL_DESC     pdcxctrl; // the lookup table for the controls position in memory
LPDCXCTRLENTRY      pdcxentr;
int                 iRet = 0;

if( (iCtrlIdx >= 0) && (iCtrlIdx < g_dcxMemMap.dcxHdr.iCtrlCount))
  {
  pdcxctrl = g_dcxMemMap.pDcxMap[iCtrlIdx].pCtrlDesc;
  (LPSTR)pdcxentr = (LPSTR)pdcxctrl + sizeof(DCX_CTRL_DESC);

  if( ( iVal >= 0 ) && ( iVal < pdcxctrl->iNumEntr) )
    {
    lstrcpy(szRdOut, pdcxentr[iVal].szRdOut);
    }
    
  }

return 0;
};

//////////////////////////
//
//
int CDEF_ReadDCXBinFile (void)
{
  return ReadDCXTableFile(g_pref.szTableFileName, &g_dcxMemMap);
}
////////////////////////////////////////////////////
// FUNCTION: CDef_FindControlByName
//
// Find a given control by its name and return
// the index into the lookup table of where
// it is ... this offset can be used to access
// the controls properties
//
int      CDef_FindControlByName(LPSTR chCtrlName)
{
int                 iRet;
int                 i;
LPDCX_MAP_ENTRY     pdcxmap; // the lookup table for the controls position in memory

iRet = -1; // error

pdcxmap = g_dcxMemMap.pDcxMap ;
if(pdcxmap != NULL)
  {
  for(i = 0; i < g_dcxMemMap.dcxHdr.iCtrlCount; i++)
    {
    if( stricmp(chCtrlName, pdcxmap->pCtrlDesc->szName) == 0)    
      {
      iRet = i;
      break;
      }
    pdcxmap ++;
    }    
  }

return iRet;
}

////////////////////////////////////////////////////////////////////////
// FUNCTION: CDef_GetControlName
//
//
//
static    char  cdef_error_ctrl_string[] = "Invalid CtrlIdx"; 
char * CDef_GetControlName(int iCtrlIdx)
{
  if( (iCtrlIdx >= 0) && (iCtrlIdx < g_dcxMemMap.dcxHdr.iCtrlCount))
    return g_dcxMemMap.pDcxMap[iCtrlIdx].pCtrlDesc->szName;
  else
    return cdef_error_ctrl_string;
};

////////////////////////////////////////////////////////////////////////
// FUNCTION: CDef_GetLastError
//
// NOTE: returns the last Error number and message string
int   CDef_GetLastError(LPSTR lpBuff, int iBuffSize)
{

// copy the Error string into lpBuff

return g_iLastError;
}
