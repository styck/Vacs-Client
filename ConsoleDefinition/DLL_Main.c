
#define MUTEXNAME "ConsoleDefinition32"

#include <windows.h>

#define _CONSOLE_DEFINITION_FILE_

#include "DCXTCP_protocol.h"
#define CONTROLDATA      // This is very dumb but for some reason CONTROLDATA gets redefinition ERROR
#include  "..\..\GServer\DCXParser.h"
#undef CONTROLDATA
#include "ConsoleDefinition.h"
#include "CDef_External.h"
#include "ConsoleDefinitionRC.h"


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
  g_bServerSetupData = FALSE;
  g_bServerTableData = FALSE;
  g_bTimerLockOut = FALSE;

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
	  if(g_socket == INVALID_SOCKET)
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
        g_bTimerLockOut = TRUE;
        SetDlgItemText(hdlg, IDC_ADDMESSAGE, "Request Server Setup Data!");
        PostMessage(g_hwndIO, WM_GET_SERVER_SETUP_DATA, 0, 0);
      }
      else
      {  
        
        if(g_bServerTableData == FALSE)
        {
          g_bTimerLockOut = TRUE;
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
