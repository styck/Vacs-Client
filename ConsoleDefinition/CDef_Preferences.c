
//===========================================================
// file CDef_Preferences.c
//
// These routines handle the communications and setup 
// between the VACS and the GSERVER.
//
// The preference dialog and setup dialog are handled here.
//
// Copyright 1998-2001 CorTek Software, Inc.
//===========================================================

#include <windows.h>
#include <stdio.h>

#include "DCXTCP_protocol.h"
#define _CONSOLE_DEFINITION_FILE_
#define CONTROLDATA      // This is very dumb but for some reason CONTROLDATA gets redefinition ERROR
#include  "DCXParser.h"
#undef CONTROLDATA
#include "ConsoleDefinition.h"
#include "CDef_External.h"
#include "ConsoleDefinitionRC.h"

char gszDllPath[MAX_PATH];
BOOL gPropertyDone;

//////////////////////////////////////////////////////
// FUNCTION: dlgConfig
//
//
BOOL APIENTRY dlgConfig(HWND hdlg, UINT message, UINT wP, LONG lP)
{
    static PROPSHEETPAGE* psConfig;
    BOOL bError;
    HWND hWndCombo;	// handle to combo box for selecting rack configuration
    int iCurSel;
    char szCurSelection[40];
    char szBuffer[80];

    switch (message)
    {
    case WM_INITDIALOG:

        ///////////////////////////////////////////////////
        // Setup RACK selection combo box for OFFLINE mode

        hWndCombo = GetDlgItem(hdlg, IDC_DCX_RACK_CONFIG);
        EnableWindow(hWndCombo, FALSE);	// Disable and Hide until OFFLINE selected
        ShowWindow(hWndCombo, SW_HIDE);

        /////////////////////////////////
        // Must be in this order to match
        // MIXER_TYPES in samm.h

        SendDlgItemMessage(hdlg, IDC_DCX_RACK_CONFIG, CB_ADDSTRING, 0, (LPARAM)"DCX Cabaret");
        SendDlgItemMessage(hdlg, IDC_DCX_RACK_CONFIG, CB_ADDSTRING, 0, (LPARAM)"DCX Showtime");
        SendDlgItemMessage(hdlg, IDC_DCX_RACK_CONFIG, CB_ADDSTRING, 0, (LPARAM)"DCX Event 40");
        SendDlgItemMessage(hdlg, IDC_DCX_RACK_CONFIG, CB_ADDSTRING, 0, (LPARAM)"DCX Event 60");

        // Select EVENT 40 as default (remember that defines are base 1)

        SendMessage(GetDlgItem(hdlg, IDC_DCX_RACK_CONFIG), CB_SETCURSEL, DCX_EVENT_40 - 1, (LPARAM)0L);	// Force a selection

        iCurSel = SendMessage(GetDlgItem(hdlg, IDC_DCX_RACK_CONFIG), CB_GETCURSEL, 0L, 0L);

        // ADD one because we made the mixer types start at 1
        // 0 will be not set

        g_pref.iMixerType = iCurSel + 1;	// Set mixer type based on MIXER_TYPES enum
        SendMessage(GetDlgItem(hdlg, IDC_DCX_RACK_CONFIG), CB_GETLBTEXT, (WPARAM)(int)iCurSel, (LPARAM)(LPCTSTR)(szCurSelection));
        wsprintf(szBuffer, "Working OFFLINE \n\n%s", szCurSelection);
        SetDlgItemText(hdlg, IDC_STAT_1, szBuffer);

        // Save the PROPSHEETPAGE information.
        psConfig = (PROPSHEETPAGE*)lP;
        return (TRUE);

    case WM_COMMAND:

        // Check if the RACK selection combo box has been changed and update message

        if ((HIWORD(wP) == CBN_SELCHANGE) && (LOWORD(wP) == IDC_DCX_RACK_CONFIG))
        {
            iCurSel = SendMessage(GetDlgItem(hdlg, IDC_DCX_RACK_CONFIG), CB_GETCURSEL, 0L, 0L);

            // ADD one because we made the mixer types start at 1
            // 0 will be not set

            g_pref.iMixerType = iCurSel + 1;	// Set mixer type based on MIXER_TYPES enum
            SendMessage(GetDlgItem(hdlg, IDC_DCX_RACK_CONFIG), CB_GETLBTEXT, (WPARAM)(int)iCurSel, (LPARAM)(LPCTSTR)(szCurSelection));
            wsprintf(szBuffer, "Working OFFLINE \n\n%s", szCurSelection);
            SetDlgItemText(hdlg, IDC_STAT_1, szBuffer);
            break;
        }

        switch (LOWORD(wP))
        {
        case IDC_BTN_TEST_CONNECTION:
            GetDlgItemText(hdlg, IDC_IP_ADDR, g_pref.szIP_Address, sizeof(g_pref.szIP_Address));
            g_pref.iPort = GetDlgItemInt(hdlg, IDC_PORT, &bError, FALSE);
            g_pref.iMixerType = -1;	// Re-Init this since it might have been changed, default NOT OFFLINE

            // Disable the ComboBox for RACK if we are trying to connect.

            hWndCombo = GetDlgItem(hdlg, IDC_DCX_RACK_CONFIG);
            EnableWindow(hWndCombo, FALSE);	// Disable and Hide until OFFLINE selected
            ShowWindow(hWndCombo, SW_HIDE);

            StartClient(hdlg);
            break;

        case IDC_BTN_WORK_OFFLINE:
            GetDlgItemText(hdlg, IDC_IP_ADDR, g_pref.szIP_Address, sizeof(g_pref.szIP_Address));
            g_pref.iPort = GetDlgItemInt(hdlg, IDC_PORT, &bError, FALSE);
            SetDlgItemText(hdlg, IDC_STAT_1, "Working OFFLINE \n\nSelect Rack Configuration");

            //////////////////////////////////////////
            // Enable selection of DCX System type

            hWndCombo = GetDlgItem(hdlg, IDC_DCX_RACK_CONFIG);
            ShowWindow(hWndCombo, SW_SHOW);
            EnableWindow(hWndCombo, TRUE);

            //////////////////// see WINDOWSX.H 						
            //						ComoBoxAddString(GetDlgItem(hdlg,IDC_DCX_RACK_CONFIG),"test");

            break;
        }
        break;

    case WM_NOTIFY:
        switch (((NMHDR*)lP)->code)
        {
        case PSN_SETACTIVE:
            // Initialize the controls.
            SetDlgItemText(hdlg, IDC_IP_ADDR, g_pref.szIP_Address);
            SetDlgItemInt(hdlg, IDC_PORT, g_pref.iPort, FALSE);
            break;

        case PSN_APPLY:

            SetWindowLong(hdlg, DWL_MSGRESULT, TRUE);
            gPropertyDone = APPLY;
            break;
        case PSN_QUERYCANCEL:
            gPropertyDone = CANCEL;
            break;
        case PSN_KILLACTIVE:
            SetWindowLong(hdlg, DWL_MSGRESULT, FALSE);
            return 1;
            break;

        case PSN_RESET:
            // Reset to the original values.
            SetWindowLong(hdlg, DWL_MSGRESULT, FALSE);
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
BOOL APIENTRY dlgTable(HWND hdlg, UINT message, UINT wP, LONG lP)
{
    static PROPSHEETPAGE* psTable;
    char szBuff[512];
    int i;

    switch (message)
    {
    case WM_INITDIALOG:
        // Save the PROPSHEETPAGE information.
        psTable = (PROPSHEETPAGE*)lP;
        return (TRUE);

    case WM_COMMAND:
        switch (LOWORD(wP))
        {
        case IDC_BTN_LOADTABLE:

            FreeDCXMapBuffers(&g_dcxMemMap);
            SendDlgItemMessage(hdlg, IDC_LIST_TABLE, LB_RESETCONTENT, 0, 0);
            GetDlgItemText(hdlg, IDC_FILENAME, szBuff, 256);
            if (ReadDCXTableFile(szBuff, &g_dcxMemMap))
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
                for (i = 0; i < g_dcxMemMap.dcxHdr.iCtrlCount; i++)
                {
                    wsprintf(szBuff, "#%d, %s, %d", i,
                        (g_dcxMemMap.pDcxMap[i].pCtrlDesc)->szName,
                        (g_dcxMemMap.pDcxMap[i].pCtrlDesc)->iNumEntr);
                    SendDlgItemMessage(hdlg, IDC_LIST_TABLE, LB_ADDSTRING, (WPARAM)0, (LPARAM)(LPSTR)szBuff);
                }
            }
            break;
        }
        break;

    case WM_NOTIFY:
        switch (((NMHDR*)lP)->code)
        {

        case PSN_SETACTIVE:
            // Initialize the controls.
            SetDlgItemText(hdlg, IDC_FILENAME, g_pref.szTableFileName);
            for (i = 0; i < g_dcxMemMap.dcxHdr.iCtrlCount; i++)
            {
                wsprintf(szBuff, "#%d, %s, %d", i,
                    (g_dcxMemMap.pDcxMap[i].pCtrlDesc)->szName,
                    (g_dcxMemMap.pDcxMap[i].pCtrlDesc)->iNumEntr);
                SendDlgItemMessage(hdlg, IDC_LIST_TABLE, LB_ADDSTRING, (WPARAM)0, (LPARAM)(LPSTR)szBuff);
            }
            break;

        case PSN_QUERYCANCEL:
            gPropertyDone = CANCEL;
            break;
        case PSN_APPLY:

            SetWindowLong(hdlg, DWL_MSGRESULT, TRUE);
            gPropertyDone = APPLY;
            break;

        case PSN_KILLACTIVE:
            SetWindowLong(hdlg, DWL_MSGRESULT, FALSE);
            return 1;
            break;

        case PSN_RESET:
            // Reset to the original values.
            SetWindowLong(hdlg, DWL_MSGRESULT, FALSE);
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
BOOL APIENTRY dlgAbout(HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
    static PROPSHEETPAGE* psAbout;

    switch (message)
    {
    case WM_INITDIALOG:
        // Save the PROPSHEETPAGE information.
        psAbout = (PROPSHEETPAGE*)lParam;
        return (TRUE);

    case WM_NOTIFY:
        switch (((NMHDR*)lParam)->code)
        {

        case PSN_SETACTIVE:
            // Initialize the controls.
            break;

        case PSN_APPLY:
            SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
            gPropertyDone = APPLY;
            break;

        case PSN_QUERYCANCEL:
            gPropertyDone = CANCEL;
            break;

        case PSN_KILLACTIVE:
            SetWindowLong(hDlg, DWL_MSGRESULT, FALSE);
            return 1;
            break;

        case PSN_RESET:
            // Reset to the original values.
            SetWindowLong(hDlg, DWL_MSGRESULT, FALSE);
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
int LoadPreferences()
{
    char szPath[MAX_PATH];
    char szBuff[MAX_PATH];
    LPSTR lpstrFName;
    FILE* stream;
    int iRead;
    int iRet = 1;

    ZeroMemory(szBuff, MAX_PATH);

    // Get the Directory where the proogram is ..
    //-------------------------------------------
    if (GetModuleFileName(g_hinstCDef, szBuff, MAX_PATH) == 0)
        return 1;

    GetFullPathName(szBuff, MAX_PATH, szPath, &lpstrFName);
    if (lpstrFName)
    {
        // now we have only the path
        // because we put 0 where the actual file name begins
        //---------------------------------------------------
        *lpstrFName = 0;
    }

    wsprintf(gszDllPath, "%s", szPath);

    wsprintf(szBuff, "%sconsoledefinition.prf", szPath);

    stream = fopen(szBuff, "r");
    if (stream == NULL)
        goto ON_EXIT;
    //return 1; // error

    iRead = fread(&g_pref, sizeof(char), sizeof(CDEF_PREFERENCES), stream);

    if (iRead == sizeof(CDEF_PREFERENCES))
    {
        if ((g_pref.dwID != CDEF_PREFERENCES_ID) ||
            (g_pref.dwSize != sizeof(CDEF_PREFERENCES)))
            goto ON_EXIT;
        iRet = 0;
    }

ON_EXIT:

    if (stream)
        fclose(stream);

    if (iRet)
    {
        g_pref.iPort = 9191;
        wsprintf(g_pref.szTableFileName, "%sdcx.bin", szPath);
    }

    g_pref.iMixerType = -1;	// Re-Init this since it was saved, default NOT OFFLINE

    return iRet;
}


/////////////////////////////////////////////////////////////////
// FUNCTION: SavePreferences
//
//
int SavePreferences()
{
    char  szPath[MAX_PATH];
    char  szBuff[MAX_PATH];
    LPSTR lpstrFName;
    FILE* stream;
    int   iWrite;
    int   iRet = 1;

    ZeroMemory(szBuff, MAX_PATH);

    // Get the Directory where the proogram is ..
    //-------------------------------------------
    if (GetModuleFileName(g_hinstCDef, szBuff, MAX_PATH) == 0)
        return 1;

    GetFullPathName(szBuff, MAX_PATH, szPath, &lpstrFName);

    if (lpstrFName)
    {
        // now we have only the path
        // because we put 0 where the actual file name begins
        //---------------------------------------------------
        *lpstrFName = 0;
    }

    wsprintf(szBuff, "%sconsoledefinition.prf", szPath);

    stream = fopen(szBuff, "w");
    if (stream == NULL)
        return 1; // error

    g_pref.dwID = CDEF_PREFERENCES_ID;
    g_pref.dwSize = sizeof(CDEF_PREFERENCES);
    g_pref.dwVersion = CONSOLE_DEFINITION_VERSION;

    iWrite = fwrite(&g_pref, sizeof(char), sizeof(CDEF_PREFERENCES), stream);

    if (iWrite == sizeof(CDEF_PREFERENCES))
    {
        iRet = 1;
    }

    fclose(stream);
    return iRet;
}
