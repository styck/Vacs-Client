//=================================================
// Copyright 1998, CorTek Software, Inc.
//=================================================


//#include <windows.h>    // includes basic windows functionality
//#include <commctrl.h>   // includes the common control header
#include <stdio.h>
//#include <string.h>


// Application Includes
//---------------------
#include "SAMM.H"
#include "SAMMEXT.H"
#include "MACRO.h"


//======================================
//FUNCTION: CreatePropertySheet
//
//
//purpose:
//  Creates the Preferences Dialog
//  box
//
//======================================
long CreatePropertySheet_PRF(HWND hwndOwner)
{
PROPSHEETPAGE psp[1];	// <==== MUST CHANGE THIS TO MATCH THE NUMBER OF PAGES BELOW
PROPSHEETHEADER psh;

int iIndex=0;

#ifdef NOTUSED	// Removed 1/7/2001 as per Gamble
psp[iIndex].dwSize = sizeof(PROPSHEETPAGE);
psp[iIndex].dwFlags = 0;
psp[iIndex].hInstance = ghInstStrRes;    
psp[iIndex].pszTemplate = MAKEINTRESOURCE(IDD_PRF_GENERAL);
psp[iIndex].pszIcon = NULL;
psp[iIndex].pfnDlgProc = dlgPRFGeneral;
psp[iIndex].pszTitle = NULL;
psp[iIndex].lParam = 0;
iIndex++;
#endif

/*
psp[1].dwSize = sizeof(PROPSHEETPAGE);
psp[1].dwFlags = 0;
psp[1].hInstance = ghInstStrRes;
psp[1].pszTemplate = MAKEINTRESOURCE(IDD_PRF_MIDI);
psp[1].pszIcon = NULL;
psp[1].pfnDlgProc = dlgPRFMidi;
psp[1].pszTitle = NULL;
psp[1].lParam = 0;

psp[1].dwSize = sizeof(PROPSHEETPAGE);
psp[1].dwFlags = 0;
psp[1].hInstance = ghInstStrRes;
psp[1].pszTemplate = MAKEINTRESOURCE(IDD_PRF_MTC);
psp[1].pszIcon = NULL;
psp[1].pfnDlgProc = dlgPRFMTC;
psp[1].pszTitle = NULL;
psp[1].lParam = 0;
*/

psp[iIndex].dwSize = sizeof(PROPSHEETPAGE);
psp[iIndex].dwFlags = 0;
psp[iIndex].hInstance = ghInstStrRes;
psp[iIndex].pszTemplate = MAKEINTRESOURCE(IDD_PRF_MIXER_CHANNELPORT);
psp[iIndex].pszIcon = NULL;
psp[iIndex].pfnDlgProc = dlgPRFMixerChanPort;
psp[iIndex].pszTitle = NULL;
psp[iIndex].lParam = 0;
iIndex++;

#ifdef NOTUSED	// Removed 1/7/2001 as per Gamble
psp[iIndex].dwSize = sizeof(PROPSHEETPAGE);
psp[iIndex].dwFlags = 0;
psp[iIndex].hInstance = ghInstStrRes;
psp[iIndex].pszTemplate = MAKEINTRESOURCE(IDD_PRF_VIEWS);
psp[iIndex].pszIcon = NULL;
psp[iIndex].pfnDlgProc = dlgPRFViews;
psp[iIndex].pszTitle = NULL;
psp[iIndex].lParam = 0;
iIndex++;
    
psp[iIndex].dwSize = sizeof(PROPSHEETPAGE);
psp[iIndex].dwFlags = 0;
psp[iIndex].hInstance = ghInstStrRes;
psp[iIndex].pszTemplate = MAKEINTRESOURCE(IDD_PRF_ACTIONS);
psp[iIndex].pszIcon = NULL;
psp[iIndex].pfnDlgProc = dlgPRFActions;
psp[iIndex].pszTitle = NULL;
psp[iIndex].lParam = 0;
iIndex++;
#endif

psh.dwSize = sizeof(PROPSHEETHEADER);
psh.dwFlags = PSH_PROPSHEETPAGE;
psh.hwndParent = hwndOwner;
psh.hInstance = ghInstMain;
psh.pszIcon = NULL;
psh.pszCaption = (LPSTR) "Preferences";
psh.nPages = sizeof(psp) / sizeof(PROPSHEETPAGE);
psh.ppsp = (LPCPROPSHEETPAGE) &psp;

return (PropertySheet(&psh));
}

//=============================================
//function: DoPreferences
//
//
// purpose:
//  Create the Property sheet and set/unset
// few flags in case ....
//
//=============================================
void        DoPreferences(void)
{

gprfEdit = gprfPref;
if(CreatePropertySheet_PRF(ghwndMain)>=0)
    if(ApplyPreferences(FALSE))
        SavePreferences();
    else
        InformationStatus(ghInstStrRes, IDS_PREF_NOTSAVED);
        

return;
}

//=======================================
//=======================================
//
//
//
// Property Sheet Dialog box routines
//
//
//
//=======================================
//=======================================
BOOL APIENTRY dlgPRFGeneral(HWND hDlg,UINT message,
	                        UINT wParam, LONG lParam)
{
static PROPSHEETPAGE * psGeneral;

switch (message)
    {
	case WM_INITDIALOG:	
		// Save the PROPSHEETPAGE information.
		psGeneral = (PROPSHEETPAGE *)lParam;
		return (TRUE);

	case WM_NOTIFY:
    	switch (((NMHDR *) lParam)->code) 
    	    {

			case PSN_SETACTIVE:
				// Initialize the controls.
			    
            	break;

			case PSN_APPLY:

 	           	SetWindowLong(hDlg,	DWL_MSGRESULT, TRUE);
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
/*
//==================================================
BOOL APIENTRY dlgPRFMidi(HWND hDlg,UINT message,
	                     UINT wParam, LONG lParam)
{
static PROPSHEETPAGE * psMIDI;
HWND        hwnd;

switch (message)
    {
	case WM_INITDIALOG:	
		// Save the PROPSHEETPAGE information.
		psMIDI = (PROPSHEETPAGE *)lParam;

		// Initialize the controls.
        hwnd = GetDlgItem(hDlg, IDC_PRF_MIDI_IN);
        ListMidiInDev(hwnd);
        SendMessage(hwnd, CB_SETCURSEL, gprfEdit.iMidiIn_DevId, (LPARAM)0);

        hwnd = GetDlgItem(hDlg, IDC_PRF_MIDI_OUT);
        ListMidiOutDev(hwnd);
        SendMessage(hwnd, CB_SETCURSEL, gprfEdit.iMidiOut_DevId, (LPARAM)0);

		return (TRUE);
        break;

    case PSPCB_CREATE:

        return TRUE;
        break;

	case WM_NOTIFY:
    	switch (((NMHDR *) lParam)->code) 
    	{

			case PSN_SETACTIVE:

				break;

			case PSN_APPLY:
                hwnd = GetDlgItem(hDlg, IDC_PRF_MIDI_IN);
                gprfEdit.iMidiIn_DevId = SendMessage(hwnd, CB_GETCURSEL, 0, 0);
                
                hwnd = GetDlgItem(hDlg, IDC_PRF_MIDI_OUT);
                gprfEdit.iMidiOut_DevId = SendMessage(hwnd, CB_GETCURSEL, 0, 0);

 	           	SetWindowLong(hDlg,	DWL_MSGRESULT, TRUE);
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


//==================================================
BOOL APIENTRY dlgPRFMTC(HWND hDlg,UINT message,
	                     UINT wParam, LONG lParam)
{
static PROPSHEETPAGE * psMTC;
HWND        hwnd;

switch (message)
    {
	case WM_INITDIALOG:	
		// Save the PROPSHEETPAGE information.
		psMTC = (PROPSHEETPAGE *)lParam;

		// Initialize the controls.
        hwnd = GetDlgItem(hDlg, IDC_PRF_MTC_IN);
        ListMidiInDev(hwnd);
        SendMessage(hwnd, CB_SETCURSEL, gprfEdit.iMTCIn_DevId, (LPARAM)0);

        hwnd = GetDlgItem(hDlg, IDC_PRF_MTC_OUT);
        ListMidiOutDev(hwnd);
        SendMessage(hwnd, CB_SETCURSEL, gprfEdit.iMTCOut_DevId, (LPARAM)0);

		return (TRUE);
        break;

    case PSPCB_CREATE:

        return TRUE;
        break;

	case WM_NOTIFY:
    	switch (((NMHDR *) lParam)->code) 
    	{

			case PSN_SETACTIVE:

				break;

			case PSN_APPLY:
                hwnd = GetDlgItem(hDlg, IDC_PRF_MTC_IN);
                gprfEdit.iMTCIn_DevId = SendMessage(hwnd, CB_GETCURSEL, 0, 0);
                
                hwnd = GetDlgItem(hDlg, IDC_PRF_MTC_OUT);
                gprfEdit.iMTCOut_DevId = SendMessage(hwnd, CB_GETCURSEL, 0, 0);

 	           	SetWindowLong(hDlg,	DWL_MSGRESULT, TRUE);
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
*/

//==================================================
BOOL APIENTRY dlgPRFActions(HWND hDlg,UINT message,
	                        UINT wParam, LONG lParam)
{
static PROPSHEETPAGE * psActions;

switch (message)
    {
	case WM_INITDIALOG:	
		// Save the PROPSHEETPAGE information.
		psActions = (PROPSHEETPAGE *)lParam;
		return (TRUE);

	case WM_NOTIFY:
    	switch (((NMHDR FAR *) lParam)->code) 
    	{

			case PSN_SETACTIVE:
				// Initialize the controls.
				break;

			case PSN_APPLY:
 	           	SetWindowLong(hDlg,	DWL_MSGRESULT, TRUE);
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

//==================================================
BOOL APIENTRY dlgPRFMixerChanPort(HWND hDlg,UINT message,
	                              UINT wParam, LONG lParam)
{
static PROPSHEETPAGE * psM_CP;

switch (message)
    {
	case WM_INITDIALOG:	
		// Save the PROPSHEETPAGE information.
		psM_CP = (PROPSHEETPAGE *)lParam;
		return (TRUE);

	case WM_NOTIFY:
    	switch (((NMHDR FAR *) lParam)->code) 
    	{

			case PSN_SETACTIVE:
				// Initialize the controls.
				break;

			case PSN_APPLY:
 	           	SetWindowLong(hDlg,	DWL_MSGRESULT, TRUE);
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

//==================================================
BOOL APIENTRY dlgPRFViews(HWND hDlg,UINT message,
	                      UINT wParam, LONG lParam)
{
static PROPSHEETPAGE * psViews;

switch (message)
    {
	case WM_INITDIALOG:	
		// Save the PROPSHEETPAGE information.
		psViews = (PROPSHEETPAGE *)lParam;
		return (TRUE);

	case WM_NOTIFY:
    	switch (((NMHDR FAR *) lParam)->code) 
    	{

			case PSN_SETACTIVE:
				// Initialize the controls.
				break;

			case PSN_APPLY:
 	           	SetWindowLong(hDlg,	DWL_MSGRESULT, TRUE);
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

//=========================
// FUNCTION: SavePrefernces
//
//=========================
void      SavePreferences(void)
{
char        szBuff[255];
HANDLE      FHandle;
BOOL        bResult;
DWORD       dwBWrite;

wsprintf(szBuff,"%s%s", gszProgDir, "Vacs.prf");

FHandle = CreateFile(szBuff, GENERIC_WRITE, 
                    0, NULL, CREATE_ALWAYS, 
                    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                    NULL);
if(FHandle == INVALID_HANDLE_VALUE)
    {
    ErrorBox(ghwndMain, ghInstStrRes, IDS_ERR_CREATING_FILE);
    return;
    }

gprfPref.dwVersion = SAMMPLUS_VERSION;
gprfPref.dwSize = sizeof(PREFERENCES);
gprfPref.dwID = SAMMPLUS_ID;

bResult = WriteFile(FHandle, (LPVOID)&gprfPref, sizeof(PREFERENCES), &dwBWrite, NULL);
if(bResult == FALSE || dwBWrite == 0)
    {
    ErrorBox(ghwndMain, ghInstStrRes, IDS_ERR_CREATING_FILE);
    CloseHandle(FHandle);
    return;
    }

CloseHandle(FHandle);

InformationStatus(ghInstStrRes, IDS_PREF_SAVED);
return;
}

//=========================
// FUNCTION: LoadPrefernces
//
//=========================
void      LoadPreferences(void)
{
char        szBuff[255];
HANDLE      FHandle;
PREFERENCES prfBuff;
DWORD       dwBRead;
BOOL        bResult;

wsprintf(szBuff,"%s%s", gszProgDir, "Vacs.prf");

FHandle = CreateFile(szBuff, GENERIC_READ, 0,  
                    NULL, OPEN_EXISTING, 
                    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, 
                    NULL);
if(FHandle ==  INVALID_HANDLE_VALUE)
    {
    //ErrorBox(ghwndMain, ghInstStrRes, IDS_ERR_OPENING_FILE);
    return;
    }

bResult = ReadFile(FHandle, (LPSTR)&prfBuff, sizeof(PREFERENCES), &dwBRead, NULL);
if(bResult == FALSE || dwBRead == 0)
    {
    InformationStatus(ghInstStrRes, IDS_NEW_VER_PRF_FILE);
    goto ON_EXIT;
    }


if((prfBuff.dwID != SAMMPLUS_ID) || (prfBuff.dwSize != sizeof(PREFERENCES))||
    prfBuff.dwVersion > SAMMPLUS_VERSION)
    {
    ErrorBox(ghwndMain, ghInstStrRes, IDS_INCOMP_PREF_FILE);
    }
else
    {
    gprfPref = prfBuff;
    if(prfBuff.dwVersion < SAMMPLUS_VERSION)
        InformationStatus(ghInstStrRes, IDS_NEW_VER_PRF_FILE);
    }



ON_EXIT:
CloseHandle(FHandle);
return;
}

//====================================================
//function: ApplyPreferences
//
//params:
//      BOOL    bForce; force the Update of everything
//purpose:
//  Attempts to apply the Preferences
//
//====================================================
BOOL        ApplyPreferences(BOOL   bForce)
{

//-----------------
// START MIDI STUFF
//
/*
//      the MIDI IN device 
//------------------------
if(bForce || gprfEdit.iMidiIn_DevId != gprfPref.iMidiIn_DevId ||
   lstrcmp(gprfEdit.MidiInDev.szPname, gprfPref.MidiInDev.szPname)==0)
    {
    // Close the default(for the moment) device
    //-----------------------------------------
    if(CloseMIDIInDev(&gMidiDev))
        // try to open the new device ...
        //-------------------------------
        if(OpenMIDIInDev(gprfEdit.iMidiIn_DevId, &gMidiDev) == FALSE)
            // there was an error opening the MIDI IN
            // device, so try to reopen the default one
            //-----------------------------------------
            OpenMIDIInDev(gprfPref.iMidiIn_DevId, &gMidiDev);
    }

//      the MIDI OUT device
//-------------------------
if(bForce || gprfEdit.iMidiOut_DevId != gprfPref.iMidiOut_DevId ||
   lstrcmp(gprfEdit.MidiOutDev.szPname, gprfPref.MidiOutDev.szPname)==0)
    {
    // Close the default(for the moment) device
    //-----------------------------------------
    if(CloseMIDIOutDev(&gMidiDev))
        // try to open the new device ...
        //-------------------------------
        if(OpenMIDIOutDev(gprfEdit.iMidiOut_DevId, &gMidiDev) == FALSE)
            // there was an error opening the MIDI OUT
            // device, so try to reopen the default one
            //-----------------------------------------
            OpenMIDIOutDev(gprfPref.iMidiOut_DevId, &gMidiDev);
    }

//      the MTC/SMPTE IN device
//-----------------------------
if(bForce || gprfEdit.iMTCIn_DevId != gprfPref.iMTCIn_DevId ||
   lstrcmp(gprfEdit.MidiMTCInDev.szPname, gprfPref.MidiMTCInDev.szPname)==0)
    {
    // Close the default(for a moment) device
    //-----------------------------------------
    if(CloseMIDIInDev(&gMTCDev))
        // try to open the new device ...
        // only if the Midi and MTC
        // devices are different
        //-------------------------------
        if(gprfEdit.iMTCIn_DevId != gprfEdit.iMidiIn_DevId)
            if(OpenMIDIInDev(gprfEdit.iMTCIn_DevId, &gMTCDev) == FALSE)
                // there was an error opening the MTC
                // device, so try to reopen the default one
                //-----------------------------------------
                OpenMIDIInDev(gprfPref.iMTCIn_DevId, &gMTCDev);
    }

//      the MTC/SMPTE OUT device
//------------------------------
if(bForce || gprfEdit.iMTCOut_DevId != gprfPref.iMTCOut_DevId ||
   lstrcmp(gprfEdit.MidiMTCOutDev.szPname, gprfPref.MidiMTCOutDev.szPname)==0)
    {
    // Close the default(for the moment) device
    //-----------------------------------------
    if(CloseMIDIOutDev(&gMTCDev))
        // try to open the new device ...
        // only if the Midi and MTC
        // devices are different
        //-------------------------------
        if(gprfEdit.iMTCOut_DevId != gprfEdit.iMidiOut_DevId)
            if(OpenMIDIOutDev(gprfEdit.iMTCOut_DevId, &gMTCDev) == FALSE)
                // there was an error opening the MTC
                // device, so try to reopen the default one
                //-----------------------------------------
                OpenMIDIOutDev(gprfPref.iMTCOut_DevId, &gMTCDev);
    }
*/
// DONE WITH MIDI STUFF
//---------------------


gprfPref = gprfEdit;
return TRUE;
}


