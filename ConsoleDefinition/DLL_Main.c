//===========================================================
// file DLL_Main.c
//
// These routines handle the informational dialog that is
// displayed when the GServer configuration is sent to
// the VACS
//
// Copyright 1998-2001 CorTek Software, Inc.
//===========================================================


#define MUTEXNAME "ConsoleDefinition32"

#include <windows.h>

#define _CONSOLE_DEFINITION_FILE_

#include "DCXTCP_protocol.h"
#define CONTROLDATA      // This is very dumb but for some reason CONTROLDATA gets redefinition ERROR
#include  "DCXParser.h"
#undef CONTROLDATA
#include "ConsoleDefinition.h"
#include "CDef_External.h"
#include "ConsoleDefinitionRC.h"

DWORD g_dwEvent60[DCX_DEVMAP_MAXSIZE] =
{
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,  // 20
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,  // 40
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,  // 60
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,  // 8 aux modules
DCX_DEVMAP_MODULE_MASTER,
DCX_DEVMAP_MODULE_CUE,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_NA,		
DCX_DEVMAP_MODULE_NA
};



DWORD g_dwEvent40[DCX_DEVMAP_MAXSIZE] =
{
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,  // 20
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,  // 40
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,  // 8 aux modules
DCX_DEVMAP_MODULE_MASTER,
DCX_DEVMAP_MODULE_CUE,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_NA,		
DCX_DEVMAP_MODULE_NA, 
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,		
DCX_DEVMAP_MODULE_NA, 
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,		
DCX_DEVMAP_MODULE_NA
};

DWORD g_dwShowTime[DCX_DEVMAP_MAXSIZE] =
{
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,  // 20
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,  // 24
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,  // 6 aux modules
DCX_DEVMAP_MODULE_MASTER,
DCX_DEVMAP_MODULE_CUE,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX, // 6
DCX_DEVMAP_MODULE_NA,		
DCX_DEVMAP_MODULE_NA,      // 2
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,    // 20
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA    // 40
};

DWORD g_dwCabaret[DCX_DEVMAP_MAXSIZE] =
{
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,  // 12
DCX_DEVMAP_MODULE_AUX,    // 13
DCX_DEVMAP_MODULE_AUX,    // 14
DCX_DEVMAP_MODULE_MASTER, // 15
DCX_DEVMAP_MODULE_CUE,    // 16
DCX_DEVMAP_MODULE_MATRIX, // 17
DCX_DEVMAP_MODULE_MATRIX, // 18
DCX_DEVMAP_MODULE_NA,		  // 19
DCX_DEVMAP_MODULE_NA,     // 20
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,    // 40
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,    // 60
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA    // 80
};

// Gloabl definitions .....
HWND      ghwndCS_Data = NULL;
UINT      g_uiTimerId = 0;
LPDEVICE_SETUP_DATA g_pDeviceSetupData = NULL;
static BOOL			g_running = FALSE;

///////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:    DllMain
//
//  INPUTS:      hDLL       - handle of DLL
//               dwReason   - indicates why DLL called
//               lpReserved - reserved
//
//  RETURNS:     TRUE (always, in this example.)
//
//               Note that the retuRn value is used only when
//               dwReason = DLL_PROCESS_ATTACH.
//
//               Normally the function would return TRUE if DLL initial-
//               ization succeeded, or FALSE it it failed.
//
//
///////////////////////////////////////////////////////////////////////////////
BOOL WINAPI DllMain (HANDLE hDLL, DWORD dwReason, LPVOID lpReserved)
{

switch (dwReason)
  {
  case DLL_PROCESS_ATTACH:
		if (g_hinstCDef  == NULL)
    g_hinstCDef = hDLL;
		else 
			g_running = TRUE;
    break;
  case DLL_THREAD_ATTACH:
    break;
  case DLL_THREAD_DETACH:
    break;
  case DLL_PROCESS_DETACH:
    break;
  }

return TRUE;
}

///////////////////////////////////////////////////////////////////
//  CreateDisplayWindow
//
//
BOOL    CDEF_GetCSData(HWND hwnd)
{
  g_bServerSetupData = FALSE;	/////////////////////////////////////
  g_bServerTableData = TRUE;	// <=== FORCE THAT WE ALREADY GOT IT
  g_bTimerLockOut = FALSE;		/////////////////////////////////////

  return (BOOL)(DialogBox(g_hinstCDef, MAKEINTRESOURCE(IDD_CLIENTSERVER_SETUP), hwnd, dlgCS_Display) >0)?TRUE:FALSE;
//  return (BOOL)DialogBox(g_hinstApp, MAKEINTRESOURCE(IDD_CLIENTSERVER_SETUP), hwnd, dlgCS_Display);
}


//////////////////////////////////////////////////////
// FUNCTION: dlgCS_Display
//
//
BOOL APIENTRY dlgCS_Display(HWND hdlg,UINT message, UINT wP, LONG lP)
{

switch (message)
  {
  case WM_TIMER:
    // do not do anything if we are in a lockout state
    //
    if(g_bTimerLockOut == TRUE)
      break;

    // see if the Client is started
    //
	  if((g_socket == INVALID_SOCKET) && (g_pref.iMixerType == -1))	 
    {
      g_bTimerLockOut = TRUE;
      if(StartClient(hdlg))
      {
        SetDlgItemText(hdlg, IDC_ADDMESSAGE, "Please check your Prefernces!");
				EndDialog(hdlg, FALSE);
        //CDef_Preferences(hdlg); // Show them the preferences
      }
    }
    else
    {
      // get the mixer modules setup
      //
      if(g_bServerSetupData == FALSE)
      {
				if(g_pref.iMixerType == -1)	// Not set means OFFLINE not set
				{
					g_bTimerLockOut = TRUE;
					SetDlgItemText(hdlg, IDC_ADDMESSAGE, "Request Server Setup Data!");
					PostMessage(g_hwndIO, WM_GET_SERVER_SETUP_DATA, 0, 0);
				}
				else	// force OFFLINE setup
				{
					switch(g_pref.iMixerType)
					{
						case DCX_CABARET:
						// Copy DCX_CABARET  setup 
						CopyMemory(g_pDeviceSetupData->iaChannelTypes, g_dwCabaret, sizeof(g_dwCabaret));
						g_bServerSetupData = TRUE;
						g_bTimerLockOut = FALSE; // used for the Timer in the Server Data Window
						SetDlgItemText(hdlg, IDC_ADDMESSAGE, "Working OFFLINE CABARET");
						break;

						case DCX_SHOWTIME:
						// Copy DCX_SHOWTIME setup 
						CopyMemory(g_pDeviceSetupData->iaChannelTypes, g_dwShowTime, sizeof(g_dwShowTime));
						g_bServerSetupData = TRUE;
						g_bTimerLockOut = FALSE; // used for the Timer in the Server Data Window
						SetDlgItemText(hdlg, IDC_ADDMESSAGE, "Working OFFLINE SHOWTIME");
						break;

						case DCX_EVENT_40:
						// Copy EVENT setup
						CopyMemory(g_pDeviceSetupData->iaChannelTypes, g_dwEvent40, sizeof(g_dwEvent40));
						g_bServerSetupData = TRUE;
						g_bTimerLockOut = FALSE; // used for the Timer in the Server Data Window
						SetDlgItemText(hdlg, IDC_ADDMESSAGE, "Working OFFLINE DCX EVENT 40");
						break;

						case DCX_EVENT_60:
						// Copy DCX_EVENT_60 
						CopyMemory(g_pDeviceSetupData->iaChannelTypes, g_dwEvent60, sizeof(g_dwEvent60));
						g_bServerSetupData = TRUE;
						g_bTimerLockOut = FALSE; // used for the Timer in the Server Data Window
						SetDlgItemText(hdlg, IDC_ADDMESSAGE, "Working OFFLINE DCX EVENT 60");
						break;

						default:
						g_bServerSetupData = TRUE;
						g_bTimerLockOut = FALSE; // used for the Timer in the Server Data Window
						SetDlgItemText(hdlg, IDC_ADDMESSAGE, "UNDEFINED SETUP");
						break;

					}
				}
      }
      else
      {  
        
        if(g_bServerTableData == FALSE)
        {
          g_bTimerLockOut = FALSE;	// <===== FORCE TO FALSE TO SKIP TABLE DATA =======
          SetDlgItemText(hdlg, IDC_ADDMESSAGE, "Request Server Bin Table Data!");
          PostMessage(g_hwndIO, WM_GET_SERVER_TABLE_DATA, 0, 0);
        }
        else
        {
          ghwndCS_Data = NULL;
          //DestroyWindow(hdlg);
          EndDialog(hdlg, TRUE);
        }                      
      }
    }
    break;
  case WM_CLOSE:
    ghwndCS_Data = NULL;

    //DestroyWindow(hdlg);
    EndDialog(hdlg, FALSE);
    break;

  case WM_DESTROY:
    KillTimer(hdlg, g_uiTimerId);
    break;

	case WM_INITDIALOG:	
		// Save the PROPSHEETPAGE information.
    ghwndCS_Data = hdlg;
    g_uiTimerId = SetTimer(hdlg, 144, 500, NULL);


		SetDlgItemText(hdlg, IDC_ADDMESSAGE, "Connect to Server ...");

		return (TRUE);
  }
return (FALSE);   
}
