/////////////////////////////////////////////////////////////////
// file CDef_Preferences.c
//
//

#include <windows.h>
#include <stdio.h>


#include "DCXTCP_protocol.h"
#define _CONSOLE_DEFINITION_FILE_
#define CONTROLDATA      // This is very dumb but for some reason CONTROLDATA gets redefinition ERROR
#include  "..\..\GServer\DCXParser.h"
#undef CONTROLDATA
#include "ConsoleDefinition.h"
#include "CDef_External.h"
#include "ConsoleDefinitionRC.h"


char    gszDllPath[MAX_PATH];
BOOL		gPropertyDone = TRUE;

//////////////////////////////////////////////////////
// FUNCTION: dlgConfig
//
//
BOOL APIENTRY dlgConfig(HWND hdlg,UINT message, UINT wP, LONG lP)
{
static PROPSHEETPAGE * psConfig;
BOOL                    bError;

switch (message)
    {
	case WM_INITDIALOG:	
		// Save the PROPSHEETPAGE information.
		psConfig = (PROPSHEETPAGE *)lP;
		return (TRUE);

  case WM_COMMAND:
    switch(LOWORD(wP))
        {
        case IDC_BTN_TEST_CONNECTION:
          GetDlgItemText(hdlg, IDC_IP_ADDR, g_pref.szIP_Address, sizeof(g_pref.szIP_Address));
          g_pref.iPort = GetDlgItemInt(hdlg, IDC_PORT, &bError, FALSE);
          StartClient(hdlg);            
          break;
        }
    break;

	case WM_NOTIFY:
    	switch (((NMHDR *) lP)->code) 
    	    {

			    case PSN_SETACTIVE:
				    // Initialize the controls.
            SetDlgItemText(hdlg, IDC_IP_ADDR, g_pref.szIP_Address);
            SetDlgItemInt(hdlg, IDC_PORT, g_pref.iPort, FALSE);			        
            break;

			    case PSN_APPLY:

 	          SetWindowLong(hdlg,	DWL_MSGRESULT, TRUE);
						gPropertyDone = TRUE;
				    break;
					case PSN_QUERYCANCEL:
						gPropertyDone = TRUE;
						break;
			    case PSN_KILLACTIVE:
	          SetWindowLong(hdlg,	DWL_MSGRESULT, FALSE);
				    return 1;
				    break;

			    case PSN_RESET:
				    // Reset to the original values.
	          SetWindowLong(hdlg,	DWL_MSGRESULT, FALSE);
				    break;
            
            }
        break;
    }
return (FALSE);   
}

//////////////////////////////////////////////////////
// FUNCTION: dlgTable
//
//
BOOL APIENTRY dlgTable(HWND hdlg,UINT message, UINT wP, LONG lP)
{
static PROPSHEETPAGE * psTable;
char    szBuff[512];
int     i;

switch (message)
    {
	case WM_INITDIALOG:	
		// Save the PROPSHEETPAGE information.
		psTable = (PROPSHEETPAGE *)lP;
		return (TRUE);

  case WM_COMMAND:
    switch(LOWORD(wP))
        {
        case IDC_BTN_LOADTABLE:

          FreeDCXMapBuffers(&g_dcxMemMap);
          SendDlgItemMessage(hdlg, IDC_LIST_TABLE, LB_RESETCONTENT, 0, 0);
          GetDlgItemText(hdlg, IDC_FILENAME, szBuff, 256);
          if(ReadDCXTableFile(szBuff, &g_dcxMemMap))
            {
            SetDlgItemText(hdlg, IDC_STAT_1, "ERROR (!)while loading DCX file !");
            SetDlgItemText(hdlg, IDC_STAT_3, "ERROR (!)while loading DCX file !");
            }
          else
            {
            ZeroMemory(g_pref.szTableFileName, MAX_PATH);
            wsprintf(g_pref.szTableFileName, "%s", szBuff);
            SetDlgItemText(hdlg, IDC_STAT_1, "DCX file LOADED!");
            wsprintf(szBuff, "Total Controls Count=%d", g_dcxMemMap.dcxHdr.iCtrlCount);
            SetDlgItemText(hdlg, IDC_STAT_3, szBuff);
            for(i = 0; i < g_dcxMemMap.dcxHdr.iCtrlCount; i++)
              {
              wsprintf(szBuff,"#%d, %s, %d", i,
                                        (g_dcxMemMap.pDcxMap[i].pCtrlDesc)->szName, 
                                        (g_dcxMemMap.pDcxMap[i].pCtrlDesc)->iNumEntr);
              SendDlgItemMessage(hdlg, IDC_LIST_TABLE, LB_ADDSTRING, (WPARAM)0,(LPARAM)(LPSTR)szBuff);
              }
            }
        
          break;        
        }
    break;

	case WM_NOTIFY:
    	switch (((NMHDR *) lP)->code) 
    	    {

			    case PSN_SETACTIVE:
				    // Initialize the controls.
            SetDlgItemText(hdlg, IDC_FILENAME, g_pref.szTableFileName);
            for(i = 0; i < g_dcxMemMap.dcxHdr.iCtrlCount; i++)
              {
              wsprintf(szBuff,"#%d, %s, %d", i,
                                        (g_dcxMemMap.pDcxMap[i].pCtrlDesc)->szName, 
                                        (g_dcxMemMap.pDcxMap[i].pCtrlDesc)->iNumEntr);
              SendDlgItemMessage(hdlg, IDC_LIST_TABLE, LB_ADDSTRING, (WPARAM)0,(LPARAM)(LPSTR)szBuff);
              }
            break;

					case PSN_QUERYCANCEL:
						gPropertyDone = TRUE;
						break;
			    case PSN_APPLY:

 	          SetWindowLong(hdlg,	DWL_MSGRESULT, TRUE);
						gPropertyDone = TRUE;
				    break;

			    case PSN_KILLACTIVE:
	          SetWindowLong(hdlg,	DWL_MSGRESULT, FALSE);
				    return 1;
				    break;

			    case PSN_RESET:
				    // Reset to the original values.
	          SetWindowLong(hdlg,	DWL_MSGRESULT, FALSE);
				    break;
            
            }
        break;
    }
return (FALSE);   
}

//////////////////////////////////////////////////////
// FUNCTION: dlgAbout
//
//
BOOL APIENTRY dlgAbout(HWND hDlg,UINT message, UINT wParam, LONG lParam)
{
static PROPSHEETPAGE * psAbout;

switch (message)
    {
	case WM_INITDIALOG:	
		// Save the PROPSHEETPAGE information.
		psAbout = (PROPSHEETPAGE *)lParam;
		return (TRUE);

	case WM_NOTIFY:
    	switch (((NMHDR *) lParam)->code) 
    	    {

			    case PSN_SETACTIVE:
				    // Initialize the controls.
            	    break;

			    case PSN_APPLY:

       	    SetWindowLong(hDlg,	DWL_MSGRESULT, TRUE);
						gPropertyDone = TRUE;
				    break;
					case PSN_QUERYCANCEL:
						gPropertyDone = TRUE;
						break;

			    case PSN_KILLACTIVE:
       	    SetWindowLong(hDlg,	DWL_MSGRESULT, FALSE);
				    return 1;
				    break;

			    case PSN_RESET:
				    // Reset to the original values.
	           	    SetWindowLong(hDlg,	DWL_MSGRESULT, FALSE);
				    break;
            
            }
        break;
    }
return (FALSE);   
}


/////////////////////////////////////////////////////////////////
// FUNCTION: LoadPreferences
//
//
int   LoadPreferences()
{
char  szPath[MAX_PATH];
char  szBuff[MAX_PATH];
LPSTR lpstrFName;
FILE  *stream;
int   iRead;
int   iRet = 1;

ZeroMemory(szBuff, MAX_PATH);

// Get the Directory where the proogram is ..
//-------------------------------------------
if(GetModuleFileName(g_hinstCDef, szBuff, MAX_PATH)==0)
    return 1;

GetFullPathName(szBuff, MAX_PATH, szPath, &lpstrFName);
if(lpstrFName)
    {
    // now we have only the path
    // because we put 0 where the actual file name begins
    //---------------------------------------------------
    *lpstrFName = 0;
    }

wsprintf(gszDllPath, "%s", szPath);

wsprintf(szBuff, "%sconsoledefinition.prf", szPath);

stream = fopen(szBuff, "r");
if(stream == NULL)
  goto ON_EXIT;
  //return 1; // error

iRead = fread(&g_pref, sizeof(char), sizeof(CDEF_PREFERENCES), stream);
if(iRead == sizeof(CDEF_PREFERENCES))
  {
  if( (g_pref.dwID != CDEF_PREFERENCES_ID) ||
      (g_pref.dwSize != sizeof(CDEF_PREFERENCES)) )
    goto ON_EXIT;
  iRet = 0;
  }



ON_EXIT:
  if(stream)
    fclose(stream);

  if(iRet)
  {
    g_pref.iPort = 9191; 
    wsprintf(g_pref.szTableFileName, "%sdcx.bin", szPath);
  }


return iRet;
}



/////////////////////////////////////////////////////////////////
// FUNCTION: SavePreferences
//
//
int   SavePreferences()
{
char  szPath[MAX_PATH];
char  szBuff[MAX_PATH];
LPSTR lpstrFName;
FILE  *stream;
int   iWrite;
int   iRet = 1;

ZeroMemory(szBuff, MAX_PATH);

// Get the Directory where the proogram is ..
//-------------------------------------------
if(GetModuleFileName(g_hinstCDef, szBuff, MAX_PATH)==0)
    return 1;

GetFullPathName(szBuff, MAX_PATH, szPath, &lpstrFName);
if(lpstrFName)
    {
    // now we have only the path
    // because we put 0 where the actual file name begins
    //---------------------------------------------------
    *lpstrFName = 0;
    }
wsprintf(szBuff, "%sconsoledefinition.prf", szPath);

stream = fopen(szBuff, "w");
if(stream == NULL)
  return 1; // error


g_pref.dwID = CDEF_PREFERENCES_ID;
g_pref.dwSize = sizeof(CDEF_PREFERENCES);
g_pref.dwVersion = CONSOLE_DEFINITION_VERSION;

iWrite = fwrite(&g_pref, sizeof(char), sizeof(CDEF_PREFERENCES), stream);
if(iWrite == sizeof(CDEF_PREFERENCES))
  {
  iRet = 1;
  }

fclose(stream);

return iRet;
}


