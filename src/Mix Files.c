//=================================================
//Mix files
//
// CopyRight 1998 CorTek Software, Inc.
//=================================================

//#include <windows.h>
//#include <commctrl.h>
//#include <string.h>

#include "SAMM.H"
#include "SAMMEXT.H"
#include "MACRO.h"
#include <stdio.h>
#include <winioctl.h>

// externals ..
extern int	g_CueMasterSystem;
extern int  g_iMasterModuleIdx;
extern int	g_inputCueActiveCounter;

void	UpdateTBGroupButtons(); // external 
void HandleCueMasterMuteFilterEx(int iPhisChann, LPMIXERWNDDATA lpmwd, 
                                  LPCTRLZONEMAP lpctrlZM, BOOL bIsOn);
int     ReadGroupWndDataFromFile(HANDLE hFile, LPFILESECTIONHEADER pFSH);
int     WriteGroupWndDataToFile(HWND hwnd, HANDLE hFile);
void		HadleCueMasterSystem (void);
int			countInputCuesOn (void);
void		handleInputCuePriority (LPMIXERWNDDATA lpmwd, BOOL	input_cue_on);


static char gszTitleBuffer[255] = {0};
//======================================================
//function: OpenMixFile
//
//
//
//
//======================================================
int     OpenMixFile(void)
{
OPENFILENAME    ofn;

if (gfsMix.szFileDir[0] == 0)
	sprintf (gfsMix.szFileDir, "%smix\\", gszProgDir);

CopyMemory(&gfsTemp, &gfsMix, sizeof(FILESTRUCT));

ZeroMemory(&ofn, sizeof(OPENFILENAME));

ofn.lStructSize = sizeof( OPENFILENAME );
ofn.hwndOwner = ghwndMain; // An invalid hWnd causes non-modality
ofn.lpstrFilter = "VACS Mix Files\0*.mix\0All Files\0*.*\0\0";//(LPSTR)szFilter;  // See previous note concerning string
ofn.lpstrCustomFilter = NULL;
ofn.nFilterIndex = 1; // Always pick the first one
ofn.lpstrFile = (LPSTR)gfsTemp.szFileName;  // Stores the result in this variable
ofn.nMaxFile = 512;
ofn.lpstrFileTitle = NULL;//(LPSTR)fsTemp.szFileName;
ofn.nMaxFileTitle = 0;//512;
ofn.lpstrInitialDir = (LPSTR)gfsTemp.szFileDir;
ofn.lpstrTitle = "Open Mix File";  // Title for dialog
ofn.Flags = OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;// | OFN_EXPLORER;
ofn.lpstrDefExt = ".mix";//(LPSTR)szDefExt;


if(GetOpenFileName(&ofn))
    {
    CloseAllMDI();
		ShowSeqWindow(FALSE);
    wsprintf(gfsMix.szFileName, "%s", &gfsTemp.szFileName[ofn.nFileOffset]);
    gfsTemp.szFileName[ofn.nFileOffset] = 0;
    wsprintf(gfsMix.szFileDir, "%s", gfsTemp.szFileName);
    LoadMixFile(&gfsMix, TRUE);
    }


return 0;
}

//======================================================
//function: SaveMixFile
//
//
//
//
//======================================================
int     SaveMixFile(void)
{
OPENFILENAME    ofn;

if (gfsMix.szFileDir[0] == 0)
	sprintf (gfsMix.szFileDir, "%smix\\", gszProgDir);

CopyMemory(&gfsTemp, &gfsMix, sizeof(FILESTRUCT));

ZeroMemory(&ofn, sizeof(OPENFILENAME));

ofn.lStructSize = sizeof( OPENFILENAME );
ofn.hwndOwner = ghwndMain; // An invalid hWnd causes non-modality
ofn.lpstrFilter = "VACS Mix Files\0*.mix\0All Files\0*.*\0\0";//(LPSTR)szFilter;  // See previous note concerning string
ofn.lpstrCustomFilter = NULL;
ofn.nFilterIndex = 1; // Always pick the first one
ofn.lpstrFile = (LPSTR)gfsTemp.szFileName;  // Stores the result in this variable
ofn.nMaxFile = 512;
ofn.lpstrFileTitle = NULL;//(LPSTR)fsTemp.szFileName;
ofn.nMaxFileTitle = 0;//512;
ofn.lpstrInitialDir = (LPSTR)gfsTemp.szFileDir;
ofn.lpstrTitle = "Save Mix File";  // Title for dialog
ofn.Flags = OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;// | OFN_EXPLORER;
ofn.lpstrDefExt = ".mix";//(LPSTR)szDefExt;


if(GetSaveFileName(&ofn))
    {
    wsprintf(gfsMix.szFileName, "%s", &gfsTemp.szFileName[ofn.nFileOffset]);
    gfsTemp.szFileName[ofn.nFileOffset] = 0;
    wsprintf(gfsMix.szFileDir, "%s", gfsTemp.szFileName);
    WriteMixFile(&gfsMix, TRUE);
    }


return 0;
}

//======================================================
//function: UpdateMixFile
//
//
//
//
//======================================================
int     UpdateMixFile(void)
{

if(WriteMixFile(&gfsMix, TRUE) == 0)
    InformationStatus(ghInstStrRes, IDS_MIXFILE_SAVED);
else
    InformationStatus(ghInstStrRes, IDS_MIXFILE_NOTSAVED);

return 0;
}

//====================================================
//function: LoadMixFile
//
//
//
//
//====================================================
int     LoadMixFile(LPFILESTRUCT pfs, BOOL	saveName)
{
HANDLE              hf;
FILESECTIONHEADER   fsh;
char                szFile[1024];
BOOL                bResult;
BOOL                bRecallFromMemBuffer = FALSE;
DWORD               dwBRead;
//LPMIXERWNDDATA			lpmwd; // temp memory for the Mixer Window data
//int									CueActiveCountTemp;

wsprintf(szFile, "%s%s", pfs->szFileDir, pfs->szFileName);

hf = CreateFile(szFile, GENERIC_READ, 
                0, NULL, OPEN_EXISTING, 
                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                NULL);
if(hf == INVALID_HANDLE_VALUE)
    {
    return 1;
    }


bResult = ReadFile(hf, (LPSTR)&gmfhTemp, 
                   sizeof(MIXFILEHEADER), &dwBRead, NULL);
if(bResult == FALSE || dwBRead == 0)
    {
    InformationStatus(ghInstStrRes, IDS_ERR_OPENING_FILE);
    CloseHandle(hf);
    return 1;
    }


if((gmfhTemp.dwID != SAMMPLUS_ID) || (gmfhTemp.dwSize != sizeof(MIXFILEHEADER))||
    gmfhTemp.dwVersion > SAMMPLUS_VERSION)
    {
    ErrorBox(ghwndMain, ghInstStrRes, IDS_INCOMP_MIX_FILE);
    }
else
    {
    gmfhMix = gmfhTemp;
    if(gmfhTemp.dwVersion < SAMMPLUS_VERSION)
        ;//InformationStatus(ghInstStrRes, IDS_NEW_VER_PRF_FILE);
    }

// Go through the file and
// Read the Header for every Section
//----------------------------------
bResult = ReadFile(hf, (LPSTR)&fsh, 
                   sizeof(FILESECTIONHEADER), &dwBRead, NULL);
if(bResult == FALSE || dwBRead != sizeof(FILESECTIONHEADER))
    {
    InformationStatus(ghInstStrRes, IDS_ERR_READING_FILE);
    CloseHandle(hf);
    return 1;
    }

while(bResult != FALSE && dwBRead == sizeof(FILESECTIONHEADER))
  {
    switch(fsh.dwID)
    {
    case MASTER_WINDOW_FILE_ID:
      // Make sure the Master Window is visible ..
      if(ghwndMaster == NULL)
        CreateMasterViewWindow("Zoom Master View", NULL);
      break;
    case MIXER_WINDOW_FILE_ID:
        ReadMixerWndDataFromFile(hf, &fsh);
        break;
    case SEQUENCE_WINDOW_FILE_ID:
        ReadSequenceWndDataFromFile(hf, &fsh);
        break;
    case GROUP_WINDOW_FILE_ID:
        ReadGroupWndDataFromFile(hf, &fsh);
        break;
    case SEQUENCE_DATA_FILE_ID:
        //ReadSequenceDataFromFile(hf, &fsh);
        break;
    case LABELS_FILE_ID:
      ReadFile(hf, gpLabels, MAX_CHANNELS*MAX_LABEL_SIZE, &dwBRead, NULL);  
      break;
    case GROUPS_FILE_ID:
      ReadFile(hf, gpGroupStoreFaders, sizeof(GROUPSTORE)*MAX_GROUPS_COUNT, &dwBRead, NULL);  
			UpdateTBGroupButtons();
      break;
    case LABELS_AUX_FILE_ID:
      ReadFile(hf, g_pAuxLabels, MAX_AUX_CHANNELS*MAX_LABEL_SIZE, &dwBRead, NULL);  
      break;
    case LABELS_MATRIX_FILE_ID:
      ReadFile(hf, g_pMatrixLabels, MAX_AUX_CHANNELS*MAX_LABEL_SIZE, &dwBRead, NULL);  
      break;

    // Read the Device data image into memory ...
    //
    case DEV_CONTROLS_IMAGE_FILE_ID:
      if(giMemMapSize == fsh.lSize)
      {
        bRecallFromMemBuffer = TRUE;
        ReadFile(hf, gpwMemMapBuffer, giMemMapSize, &dwBRead, NULL);  
      }
      else
      {
        bRecallFromMemBuffer = FALSE;
        SetFilePointer(hf, fsh.lSize, 0, FILE_CURRENT);
      }
      break;

    default:
        // in case we don't know what
        // is this Section just skip over it
        //----------------------------------
        if(fsh.lSize > 0)
            SetFilePointer(hf, fsh.lSize, 0, FILE_CURRENT);
        else
            {
            InformationStatus(ghInstStrRes, IDS_ERR_READING_FILE);
            CloseHandle(hf);
            return 1;
            }
        break;
    }

    bResult = ReadFile(hf, (LPSTR)&fsh, sizeof(FILESECTIONHEADER), &dwBRead, NULL);
  }

CloseHandle(hf);


if(bRecallFromMemBuffer)
  RecallMemoryMapBuffer(FALSE);

//HadleCueMasterSystem ();
/*
// Start the Cue-system
if(g_CueMasterSystem == 0){

	lpmwd = MixerWindowDataAlloc(gwActiveMixer,
															 gpZoneMaps_Zoom,
															 MAX_CHANNELS, 1);
	// Scan for any Cue buttons.
	// We need this so the Cue_Master_System 
	// will not mess-up the Cues that might be active!
	PrepareCueMasterSystem ();
	if (gCueActiveCount > 0)
	{
		CueActiveCountTemp = gCueActiveCount;
		gCueActiveCount = 0;
		HandleCueMasterMuteFilterEx(g_iMasterModuleIdx, lpmwd, NULL, TRUE);
		gCueActiveCount = CueActiveCountTemp;
		g_CueMasterSystem  = 1;
	}
	else
	{
		HandleCueMasterMuteFilterEx(g_iMasterModuleIdx, lpmwd, NULL, FALSE);
		g_CueMasterSystem  = 1;
	}

	GlobalFree ((HANDLE) lpmwd);
}
*/

if(saveName)
{
	wsprintf(gszTitleBuffer, "%s %s", gszMainWndTitle, pfs->szFileName);
	SetWindowText(ghwndMain, gszTitleBuffer);
}

return 0;
}
//
//
//
void	HadleCueMasterSystem ()
{
	LPMIXERWNDDATA			lpmwd; // temp memory for the Mixer Window data
	int									CueActiveCountTemp;
	int									inpCueCount;
	// Start the Cue-system
	if(g_CueMasterSystem == 0){

		lpmwd = MixerWindowDataAlloc(gwActiveMixer,
																 gpZoneMaps_Zoom,
																 MAX_CHANNELS, 1);
		// Scan for any Cue buttons.
		// We need this so the Cue_Master_System 
		// will not mess-up the Cues that might be active!
		PrepareCueMasterSystem ();
		inpCueCount = countInputCuesOn ();

		if (gCueActiveCount > 0)
		{
			CueActiveCountTemp = gCueActiveCount;
			gCueActiveCount = 0;
			HandleCueMasterMuteFilterEx(g_iMasterModuleIdx, lpmwd, NULL, TRUE);
			gCueActiveCount = CueActiveCountTemp;
			g_CueMasterSystem  = 1;
		}
		else
		{
			HandleCueMasterMuteFilterEx(g_iMasterModuleIdx, lpmwd, NULL, FALSE);
			g_CueMasterSystem  = 1;
		}

		if(inpCueCount)
		{
			g_inputCueActiveCounter = inpCueCount - 1;
			handleInputCuePriority (lpmwd, TRUE);
		}
		else
		{
			g_inputCueActiveCounter = 0;
			handleInputCuePriority (lpmwd, FALSE);
		}

		GlobalFree ((HANDLE) lpmwd);
	}
}


//====================================================
//function: SaveMixFile
//
//
//
//
//====================================================
int     WriteMixFile(LPFILESTRUCT pfs, BOOL  showName)
{
HANDLE          hf;
char            szFile[1024];
BOOL            bResult;
DWORD           dwBWrite;
HWND            hwnd;
int             *piWndID;
FILESECTIONHEADER   fsh;
USHORT					compression;


wsprintf(szFile, "%s%s", pfs->szFileDir, pfs->szFileName);


hf = CreateFile(szFile, GENERIC_WRITE, 
                0, NULL, OPEN_ALWAYS, 
                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                NULL);

if(hf == INVALID_HANDLE_VALUE)
    {
    return 1;
    }

// cool ... now set the compression on this file
compression = COMPRESSION_FORMAT_LZNT1;
DeviceIoControl (hf, FSCTL_SET_COMPRESSION, &compression, sizeof (compression), 0, 0, 
								 &dwBWrite, 0);

// Write the File header
//----------------------
gmfhMix.dwID   = SAMMPLUS_ID;
gmfhMix.dwVersion   = SAMMPLUS_VERSION;
gmfhMix.dwSize = sizeof(MIXFILEHEADER);
gmfhMix.dwDevID = 0; // IT SHOULD BE SET TO THE DEVICE ID for THE MIXER
gmfhMix.iNumDev = 0; // the number of devices
gmfhMix.iScrCX = 0; // the screen resolution CX
gmfhMix.iScrCY = 0; // the screen resolution CX
ZeroMemory(gmfhMix.iReserved, sizeof(gmfhMix.iReserved));

bResult = WriteFile(hf, (LPVOID)&gmfhMix, 
                    sizeof(MIXFILEHEADER), &dwBWrite, NULL);
if(bResult == FALSE || dwBWrite == 0)
    goto ON_ERROR_EXIT;


  // Save the Groups
  //----------------
  ZeroMemory(&fsh, sizeof(fsh));
  // Write the Labelss ID ...
  //
  fsh.dwID    = GROUPS_FILE_ID;
  fsh.lSize   = sizeof(GROUPSTORE)*MAX_GROUPS_COUNT;
  fsh.dwVersion = 0;

  WriteFile(hf, &fsh, sizeof(fsh), &dwBWrite, NULL);  
  WriteFile(hf, gpGroupStoreFaders, sizeof(GROUPSTORE)*MAX_GROUPS_COUNT, &dwBWrite, NULL);  
    

  // Save the Labels.
  //-----------------
  if(gpLabels)
  {
    ZeroMemory(&fsh, sizeof(fsh));
    // Write the Labelss ID ...
    //
    fsh.dwID    = LABELS_FILE_ID;
    fsh.lSize   = MAX_CHANNELS*MAX_LABEL_SIZE;
    fsh.dwVersion = 0;

    WriteFile(hf, &fsh, sizeof(fsh), &dwBWrite, NULL);  
    WriteFile(hf, gpLabels, MAX_CHANNELS*MAX_LABEL_SIZE, &dwBWrite, NULL);  
  }

  if(g_pAuxLabels)
  {
    ZeroMemory(&fsh, sizeof(fsh));
    // Write the Labelss ID ...
    //
    fsh.dwID    = LABELS_AUX_FILE_ID;
    fsh.lSize   = MAX_AUX_CHANNELS*MAX_LABEL_SIZE;
    fsh.dwVersion = 0;

    WriteFile(hf, &fsh, sizeof(fsh), &dwBWrite, NULL);  
    WriteFile(hf, g_pAuxLabels, MAX_AUX_CHANNELS*MAX_LABEL_SIZE, &dwBWrite, NULL);  
  }

  if(g_pMatrixLabels)
  {
    ZeroMemory(&fsh, sizeof(fsh));
    // Write the Labelss ID ...
    //
    fsh.dwID    = LABELS_MATRIX_FILE_ID;
    fsh.lSize   = MAX_AUX_CHANNELS*MAX_LABEL_SIZE;
    fsh.dwVersion = 0;

    WriteFile(hf, &fsh, sizeof(fsh), &dwBWrite, NULL);  
    WriteFile(hf, g_pMatrixLabels, MAX_AUX_CHANNELS*MAX_LABEL_SIZE, &dwBWrite, NULL);  
  }

  // Write this section .. so we know that tha master window was visible
  // before we start creating the other windows
  if(ghwndMaster)
  {
    fsh.dwID = MASTER_WINDOW_FILE_ID;
    fsh.lSize = 0;
    fsh.dwVersion = 0;
    WriteFile(hf, &fsh, sizeof(fsh), &dwBWrite, NULL);  
  }


// Save all of the Open Windows
//-----------------------------
hwnd = GetTopWindow(ghwndMDIClient);
if(GetWindow(hwnd, GW_HWNDLAST))
    hwnd = GetWindow(hwnd, GW_HWNDLAST);

  while(hwnd)
  {
    piWndID = (int *)GetWindowLong(hwnd, 0);
    if(piWndID == NULL)
        goto ON_ERROR_EXIT;

    switch((int)(*piWndID))
    {
    case MIXER_WINDOW_FILE_ID:
      if(WriteMixerWndDataToFile(hwnd, hf) != 0)
          goto ON_ERROR_EXIT; // Something went wrong saving the Window Data 
      break;
    case SEQUENCE_WINDOW_FILE_ID:
			if(IsWindowVisible(hwnd))
				if(WriteSequenceWndDataToFile(hwnd, hf) != 0)
					goto ON_ERROR_EXIT; // Something went wrong saving the Window Data 
      break;
		case GROUP_WINDOW_FILE_ID:
			if(IsWindowVisible(hwnd))
				if(WriteGroupWndDataToFile(hwnd, hf) != 0)
					goto ON_ERROR_EXIT; // Something went wrong saving the Window Data 
			break;
    }
    hwnd = GetNextWindow(hwnd, GW_HWNDPREV);
  }


  // Write the Device memory Image ..
  //
  fsh.dwID = DEV_CONTROLS_IMAGE_FILE_ID;
  fsh.lSize = giMemMapSize;
  fsh.dwVersion = 0x00000001;
  WriteFile(hf, &fsh, sizeof(fsh), &dwBWrite, NULL);  
  
  WriteFile(hf, gpwMemMapMixer, fsh.lSize, &dwBWrite, NULL);  


// Save the Memory State
//----------------------

// Set the End-Of-File
// Close the file
//--------------------
SetEndOfFile(hf);
CloseHandle(hf);
if(showName)
{
	wsprintf(gszTitleBuffer, "%s %s", gszMainWndTitle, pfs->szFileName);
	SetWindowText(ghwndMain, gszTitleBuffer);
}
return 0;

ON_ERROR_EXIT:
ErrorBox(ghwndMain, ghInstStrRes, IDS_ERR_CREATING_FILE);

SetEndOfFile(hf);
CloseHandle(hf);

return 1;
}

//=============================================
//function: WriteMixerWndDataToFile
//
// params:
//          HWND        hwnd; the Window handle
//          HANDLE      hFile; handle to the
//                             file
//
//=============================================
int     WriteMixerWndDataToFile(HWND hwnd, HANDLE hFile)
{
FILESECTIONHEADER   fsh;
LPMIXERWNDDATA      pmwd;
MIXERWINDOWFILE     mwdfile;
BOOL                bResult;
DWORD               dwBWrite;

ZeroMemory(&fsh, sizeof(FILESECTIONHEADER));
ZeroMemory(&mwdfile, sizeof(MIXERWINDOWFILE));

// Save the Section Header
//------------------------
fsh.lSize = sizeof(MIXERWINDOWFILE);
fsh.dwID = MIXER_WINDOW_FILE_ID;
fsh.dwVersion = SAMMPLUS_VERSION;
bResult = WriteFile(hFile, (LPVOID)&fsh, sizeof(FILESECTIONHEADER), &dwBWrite, NULL);
if(bResult == FALSE || dwBWrite != sizeof(FILESECTIONHEADER))
    return 1;// error


// Save the Window Data
//---------------------
pmwd = (LPMIXERWNDDATA)GetWindowLong(hwnd, 0);
if(pmwd != NULL)
    {
    lstrcpy(mwdfile.szTitle, pmwd->szTitle);
    mwdfile.iMixer = pmwd->iMixer;
    mwdfile.iZoneMap_ID = pmwd->lpZoneMap->wID;
    CopyMemory(mwdfile.lpwRemapToScr, pmwd->lpwRemapToScr, sizeof(WORD)*MAX_CHANNELS);
    CopyMemory(mwdfile.lpwRemapToPhis, pmwd->lpwRemapToPhis, sizeof(WORD)*MAX_CHANNELS);
    mwdfile.iStartScrChan = pmwd->iStartScrChan;
    mwdfile.iEndScrChan = pmwd->iEndScrChan;
    mwdfile.iXOffset = pmwd->iXOffset;
    mwdfile.iYOffset = pmwd->iYOffset;
    mwdfile.rWndPos = pmwd->rWndPos;
    mwdfile.rVisible = pmwd->rVisible;
    mwdfile.bLink = (hwnd == ghwndZoom);

    bResult = WriteFile(hFile, (LPVOID)&mwdfile, sizeof(MIXERWINDOWFILE), &dwBWrite, NULL);
    if(bResult == FALSE || dwBWrite != sizeof(MIXERWINDOWFILE))
        return 1;// error
    }
else
    return 1; // Error

return 0;
}

//=============================================
//function: ReadMixerWndDataToFile
//
// params:
//          HANDLE      hFile; handle to the
//                             file
//          FILESECTIONHEADER *pFSH;
//=============================================
int     ReadMixerWndDataFromFile(HANDLE hFile, LPFILESECTIONHEADER pFSH)
{
MIXERWINDOWFILE mwdfile;
BOOL            bResult;
DWORD           dwBRead;
LPMIXERWNDDATA  pmwd;
HWND            hwndLink;


// ??????????????????????????
// ??????????????????????????
//
// Save the type of the modules in the Windows in the file
//
// ??????????????????????????
// ??????????????????????????

ZeroMemory(&mwdfile, sizeof(MIXERWINDOWFILE));

bResult = ReadFile(hFile, (LPVOID)&mwdfile, sizeof(MIXERWINDOWFILE), &dwBRead, NULL);
if(bResult == FALSE || dwBRead != sizeof(MIXERWINDOWFILE))
    return 1;// error

// load the Correct Zone Map
//--------------------------
switch(mwdfile.iZoneMap_ID)
    {
    case IDZM_INPUT_FULL:
        // Set the pointers
        pmwd = MixerWindowDataAlloc((WORD)mwdfile.iMixer,
                                     gpZoneMaps_Full,
                                     MAX_CHANNELS, 1);
        break;
    case IDZM_INPUT_ZOOM:
        pmwd = MixerWindowDataAlloc((WORD)mwdfile.iMixer,
                                     gpZoneMaps_Zoom,
                                     MAX_CHANNELS, 1);
        break;
    }

if(pmwd == NULL)
    return 2;// error

lstrcpy(pmwd->szTitle, mwdfile.szTitle);
pmwd->iMixer = mwdfile.iMixer;

// set the remaping
CopyMemory(pmwd->lpwRemapToScr, mwdfile.lpwRemapToScr, sizeof(WORD)*MAX_CHANNELS);
CopyMemory(pmwd->lpwRemapToPhis, mwdfile.lpwRemapToPhis, sizeof(WORD)*MAX_CHANNELS);

pmwd->iStartScrChan = mwdfile.iStartScrChan ;
pmwd->iEndScrChan = mwdfile.iEndScrChan;
pmwd->iXOffset = mwdfile.iXOffset;
pmwd->iYOffset = mwdfile.iYOffset;
pmwd->rWndPos = mwdfile.rWndPos;
pmwd->rVisible = mwdfile.rVisible;
pmwd->bLink = mwdfile.bLink;


switch(pmwd->lpZoneMap->wID)
    {
    case IDZM_INPUT_FULL:
        // Create the Window
        CreateFullViewWindow(pmwd->szTitle, pmwd);
        break;
    case IDZM_INPUT_ZOOM:
        // Create the Window
        hwndLink = CreateZoomViewWindow(pmwd->szTitle, pmwd, 1);
        if(pmwd->bLink && hwndLink)
            ghwndZoom = hwndLink;
        break;
    }

return 0;// OK
}

//==================================================
//function: WriteSequenceWndDataToFile
//
//
//
//
//==================================================
int     WriteSequenceWndDataToFile(HWND hwnd, HANDLE hFile)
{
FILESECTIONHEADER       fsh;
LPSEQUENCEWINDOWFILE    lpSeqWF;
BOOL                    bResult;
DWORD                   dwBWrite;

ZeroMemory(&fsh, sizeof(FILESECTIONHEADER));

// Save the Section Header
//------------------------
fsh.lSize = sizeof(SEQUENCEWINDOWFILE);
fsh.dwID = SEQUENCE_WINDOW_FILE_ID;
fsh.dwVersion = SAMMPLUS_VERSION;
bResult = WriteFile(hFile, (LPVOID)&fsh, sizeof(FILESECTIONHEADER), &dwBWrite, NULL);
if(bResult == FALSE || dwBWrite != sizeof(FILESECTIONHEADER))
    return 1;// error

lpSeqWF = (LPSEQUENCEWINDOWFILE)GetWindowLong(hwnd, 0);

GetWindowRect (hwnd, &lpSeqWF->rWndPos);
ScreenToClient (ghwndMDIClient, (LPPOINT)&lpSeqWF->rWndPos.left);
ScreenToClient (ghwndMDIClient, (LPPOINT)&lpSeqWF->rWndPos.right);


bResult = WriteFile(hFile, (LPVOID)lpSeqWF, sizeof(SEQUENCEWINDOWFILE), &dwBWrite, NULL);
if(bResult == FALSE || dwBWrite != sizeof(SEQUENCEWINDOWFILE))
    return 1;// error

return 0;
}

//=============================================
//function: ReadSequenceWndDataFromFile
//
// params:
//          HANDLE      hFile; handle to the
//                             file
//          FILESECTIONHEADER *pFSH;
//=============================================
extern HWND	ghwndSeq;
int     ReadSequenceWndDataFromFile(HANDLE hFile, LPFILESECTIONHEADER pFSH)
{
SEQUENCEWINDOWFILE      seqWF;
BOOL                    bResult;
DWORD                   dwBRead;

ZeroMemory(&seqWF, sizeof(SEQUENCEWINDOWFILE));

bResult = ReadFile(hFile, (LPVOID)&seqWF, sizeof(SEQUENCEWINDOWFILE), &dwBRead, NULL);
if(bResult == FALSE || dwBRead != sizeof(SEQUENCEWINDOWFILE))
    return 1;// error

ShowSeqWindow(TRUE);
if(ghwndSeq && seqWF.rWndPos.right != 0)
	SetWindowPos (ghwndSeq, HWND_TOP, seqWF.rWndPos.left, seqWF.rWndPos.top,
																			seqWF.rWndPos.right - seqWF.rWndPos.left, 
																			seqWF.rWndPos.bottom - seqWF.rWndPos.top,
																			SWP_NOZORDER);

return 0;
}

//==================================================
//function: WriteSequenceWndDataToFile
//
//
//
//
//==================================================
int     WriteGroupWndDataToFile(HWND hwnd, HANDLE hFile)
{
FILESECTIONHEADER       fsh;
LPGROUPWINDOWFILE       pGroupWF;
BOOL                    bResult;
DWORD                   dwBWrite;

ZeroMemory(&fsh, sizeof(FILESECTIONHEADER));

// Save the Section Header
//------------------------
fsh.lSize = sizeof(GROUPWINDOWFILE);
fsh.dwID = GROUP_WINDOW_FILE_ID;
fsh.dwVersion = SAMMPLUS_VERSION;
bResult = WriteFile(hFile, (LPVOID)&fsh, sizeof(FILESECTIONHEADER), &dwBWrite, NULL);
if(bResult == FALSE || dwBWrite != sizeof(FILESECTIONHEADER))
    return 1;// error

pGroupWF = (LPGROUPWINDOWFILE)GetWindowLong(hwnd, 0);

GetWindowRect (hwnd, &pGroupWF->rWndPos);
ScreenToClient (ghwndMDIClient, (LPPOINT)&pGroupWF->rWndPos.left);
ScreenToClient (ghwndMDIClient, (LPPOINT)&pGroupWF->rWndPos.right);

bResult = WriteFile(hFile, (LPVOID)pGroupWF, sizeof(GROUPWINDOWFILE), &dwBWrite, NULL);
if(bResult == FALSE || dwBWrite != sizeof(GROUPWINDOWFILE))
    return 1;// error

return 0;
}

//=============================================
//function: ReadSequenceWndDataFromFile
//
// params:
//          HANDLE      hFile; handle to the
//                             file
//          FILESECTIONHEADER *pFSH;
//=============================================
extern HWND		ghwndGroup;
int     ReadGroupWndDataFromFile(HANDLE hFile, LPFILESECTIONHEADER pFSH)
{
GROUPWINDOWFILE					groupWF;
BOOL                    bResult;
DWORD                   dwBRead;

ZeroMemory(&groupWF, sizeof(SEQUENCEWINDOWFILE));
																									 
bResult = ReadFile(hFile, (LPVOID)&groupWF, sizeof(GROUPWINDOWFILE), &dwBRead, NULL);
if(bResult == FALSE || dwBRead != sizeof(GROUPWINDOWFILE))
    return 1;// error

ShowGroupWindow(TRUE);
if(ghwndGroup)
	SetWindowPos (ghwndGroup, HWND_TOP, groupWF.rWndPos.left, groupWF.rWndPos.top,
																			groupWF.rWndPos.right - groupWF.rWndPos.left, 
																			groupWF.rWndPos.bottom - groupWF.rWndPos.top,
																			SWP_NOZORDER);

return 0;
}
