//=================================================
// Copyright 1998-2002 CorTek Software, Inc.
//=================================================
//
//
// $Author:: Styck                                $
// $Archive:: /Vacs Client/src/Mix Files.c        $
// $Revision:: 24                                 $
//

#include "SAMM.H"
#include "SAMMEXT.H"
#include "MACRO.h"
#include <stdio.h>
#include <winioctl.h>


// externals ..
extern int	g_CueMasterSystem;
extern int  g_iMasterModuleIdx;
extern int	g_inputCueActiveCounter;
extern char	g_sequence_file_name[MAX_PATH];
extern LPDLROOTPTR g_pdlrSequence;
extern DWORD g_dwCurrentSeqSelection;
extern HWND		ghwndTBSeqSceneNumber;	// see CREATEMAIN.C

void	UpdateTBGroupButtons(); // external 
void	HandleCueMasterMuteFilterEx(int iPhisChann, LPMIXERWNDDATA lpmwd, 
                                  LPCTRLZONEMAP lpctrlZM, BOOL bIsOn);
int     ReadGroupWndDataFromFile(HANDLE hFile, LPFILESECTIONHEADER pFSH);
int     WriteGroupWndDataToFile(HWND hwnd, HANDLE hFile);
void		HadleCueMasterSystem (void);
int			countInputCuesOn (void);
void		handleInputCuePriority (LPMIXERWNDDATA lpmwd, BOOL	input_cue_on);
BOOL		OpenSequenceFiles (LPSTR  lpstrFName);
void		CloseSequenceFiles (void);
void		UpdateSeqSceneNumber(void);
LPSTR		GetMixerTypeName(enum MIXER_TYPES iMixType );


extern DWORD SeqSelectionIndex(void);
extern BOOL SeqGoToIndex(DWORD index);

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
int	iRet;

	// Registry variables 

	LONG lnResult;
	HKEY hKey = NULL;
	DWORD dwDisposition;
	DWORD   	  dwError;
	static LPCTSTR szRegKey = "Software\\CorTek\\VACS";
	static LPCTSTR szRegValue; 
	DWORD dwType;
	DWORD rc;
	DWORD dwBufferSize = sizeof( g_sequence_file_name );  
	char								szTempSeq[MAX_PATH];

	char                szProgDir[MAX_PATH];

  LPSTR   lpstrFName;

	if (gfsMix.szFileDir[0] == 0)
		sprintf (gfsMix.szFileDir, "%smix\\", gszProgDir);

	CopyMemory(&gfsTemp, &gfsMix, sizeof(FILESTRUCT));

	ZeroMemory(&ofn, sizeof(OPENFILENAME));

/////////////////////////////////////////////
// If we loaded a file before then default
// to that one

	if(!strlen(gfsTemp.szFileName))
	{

		lnResult = RegCreateKeyEx(
								HKEY_CURRENT_USER,
								szRegKey,
								0, 
								REG_NONE, 
								0, 
								KEY_ALL_ACCESS, 
								NULL, 
								&hKey,
								&dwDisposition );

		szRegValue = "MRUmix";
		rc = RegQueryValueEx( 
					hKey, 
					szRegValue, 
					NULL, 
					&dwType, 
					&szTempSeq, 
					&dwBufferSize ); 

		  // Set our file name to the last loaded mix file

		if(rc == ERROR_SUCCESS)
			wsprintf(gfsTemp.szFileName, "%s", szTempSeq);

		// We have the whole path now just get the filename

//		GetFullPathName(szTempSeq, 256, szProgDir, &lpstrFName);

	}

/////////////////////////////////////////////////////

	ofn.lStructSize = sizeof( OPENFILENAME );
	ofn.hwndOwner = ghwndMain; // An invalid hWnd causes non-modality
	ofn.lpstrFilter = "VACS Mix Files\0*.mix\0All Files\0*.*\0\0";//(LPSTR)szFilter;  // See previous note concerning string
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex = 1; // Always pick the first one
	ofn.lpstrFile = (LPSTR)gfsTemp.szFileName;  // Stores the result in this variable
// fds for single file	ofn.lpstrFile = lpstrFName;		// This is filename with path => (LPSTR)gfsTemp.szFileName;  // Stores the result in this variable
	ofn.nMaxFile = 512;
	ofn.lpstrFileTitle = NULL;//(LPSTR)fsTemp.szFileName;
	ofn.nMaxFileTitle = 0;//512;
	ofn.lpstrInitialDir = (LPSTR)gfsTemp.szFileDir;
	ofn.lpstrTitle = "Open Mix File";  // Title for dialog
	ofn.Flags = OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;// | OFN_EXPLORER;
	ofn.lpstrDefExt = ".mix";//(LPSTR)szDefExt;


	if(GetOpenFileName(&ofn))
  {
		if (g_mixer_state_changed)
		{	
			if (ConfirmationBox(ghwndMDIClient, ghInstStrRes, IDS_CHANGES_MESSAGE) == IDYES)
				UpdateMixFile ();
		}

		g_mixer_state_changed = FALSE;
		g_monitor_mix_file_changes = FALSE;
		wsprintf(g_sequence_file_name, "%s", ofn.lpstrFile);

    CloseAllMDI();
		ShowSeqWindow(FALSE);

		// This is NOT using the loaded filename stored in lpstrFName (when we use it)

    wsprintf(gfsMix.szFileName, "%s", &gfsTemp.szFileName[ofn.nFileOffset]);

    gfsTemp.szFileName[ofn.nFileOffset] = 0;
    wsprintf(gfsMix.szFileDir, "%s", gfsTemp.szFileName);
		iRet = LoadMixFile(&gfsMix, TRUE);
    if(iRet != 0)	// MIX FILE did not match RACK CONFIGURATION or other problem
		{
				ShutdownProc();
				return FALSE;
		}

		g_monitor_mix_file_changes = TRUE;

  }


return TRUE;
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
	char						new_sequence_files [MAX_PATH];
	char						old_sequence_files [MAX_PATH];
	char						szFile [MAX_PATH];
	USHORT					compression;
	char						szBuff[MAX_PATH];
	DWORD						dwRet;

	if (gfsMix.szFileDir[0] == 0)
		sprintf (gfsMix.szFileDir, "%smix\\", gszProgDir);

	CopyMemory(&gfsTemp, &gfsMix, sizeof(FILESTRUCT));

	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof( OPENFILENAME );

	ofn.hwndOwner = ghwndMain;		 // An invalid hWnd causes non-modality

	ofn.lpstrFilter = "VACS Mix Files\0*.mix\0All Files\0*.*\0\0";//(LPSTR)szFilter;  // See previous note concerning string
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex = 1; // Always pick the first one
	ofn.lpstrFile = (LPSTR)gfsTemp.szFileName;  // Stores the result in this variable
	ofn.nMaxFile = 512;
	ofn.lpstrFileTitle = NULL;		//(LPSTR)gfsMix.szFileName;	// THIS IS THE DEFAULT FILE NAME
	ofn.nMaxFileTitle = 0;//512;
	ofn.lpstrInitialDir = (LPSTR)gfsTemp.szFileDir;
	ofn.lpstrTitle = "Save Mix File";  // Title for dialog
	ofn.Flags = OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;// | OFN_EXPLORER;
	ofn.lpstrDefExt = ".mix";//(LPSTR)szDefExt;


	if(GetSaveFileName(&ofn))
	{
			CloseSequenceFiles ();	
			wsprintf(gfsMix.szFileName, "%s", &gfsTemp.szFileName[ofn.nFileOffset]);
			gfsTemp.szFileName[ofn.nFileOffset] = 0;
			wsprintf(gfsMix.szFileDir, "%s", gfsTemp.szFileName);
			WriteMixFile(&gfsMix, TRUE);

			wsprintf(szFile, "%s%s", gfsMix.szFileDir, gfsMix.szFileName);


	// close the old files and the sequence window
	//SendMessage(ghwndMDIClient, WM_MDIDESTROY, (WPARAM)ghwndSeq, 0L);
	//DestroyWindow (ghwndSeq);
	//ghwndSeq = NULL;

	// assemble the new sequence file names and copy the old ones over
	wsprintf (new_sequence_files, "%s.ctek", szFile);
	wsprintf (old_sequence_files, "%s.ctek", g_sequence_file_name);
	CopyFile (old_sequence_files, new_sequence_files, FALSE);

	CloseDataFile();

	wsprintf (new_sequence_files, "%s.data", szFile);
	wsprintf (old_sequence_files, "%s.data", g_sequence_file_name);
	CopyFile (old_sequence_files, new_sequence_files, FALSE);

	// cool ... now set the compression on this file

//		wsprintf (szBuff, "%s.ctek", g_sequence_file_name);
//		wsprintf (szBuff, "%s.ctek", szFile);
//    g_pdlrSequence = InitDoubleLinkedList(sizeof(SEQENTRY), 32, TRUE, TRUE, NULL, szBuff);
//    wsprintf(szBuff, "%s.data",g_sequence_file_name);

  wsprintf(szBuff, "%s.data",szFile);

	wsprintf(szFile, "%s%s", gfsMix.szFileDir, gfsMix.szFileName);
	wsprintf(g_sequence_file_name, "%s", szFile);

  if(OpenDataFile(szBuff))
		OpenSequenceFiles (g_sequence_file_name);
  
	}
	else
	{
		dwRet = CommDlgExtendedError();	// Could not open Save dialog
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
char								szBuff[MAX_PATH];

	// Registry variables 

	LONG lnResult;
	HKEY hKey = NULL;
	DWORD dwDisposition;
	DWORD   	  dwError;
	static LPCTSTR szRegKey = "Software\\CorTek\\VACS";
	static LPCTSTR szRegValue; 
	
	DWORD dwType;
	DWORD rc;
	DWORD dwBufferSize = sizeof( g_sequence_file_name );  
	char								szTempSeq[MAX_PATH];

/////////////////

	BY_HANDLE_FILE_INFORMATION	bhfi;
	SYSTEMTIME	systime;
	FILETIME		ftLocal;

//LPMIXERWNDDATA			lpmwd; // temp memory for the Mixer Window data
//int									CueActiveCountTemp;

	wsprintf(szFile, "%s%s", pfs->szFileDir, pfs->szFileName);

	wsprintf(g_sequence_file_name, "%s", szFile);
	wsprintf(szBuff, "%smix\\%s", gszProgDir, "LA$T.mix");

	/////////////////////////////////////
	// OPEN an existing MIX FILE

	hf = CreateFile(szFile, GENERIC_READ, 
                0, NULL, OPEN_EXISTING, 
                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                NULL);

	if(hf == INVALID_HANDLE_VALUE)
  {
    return 1;
  }


	////////////////////////////////////////
	// READ the MIX FILE Header

	bResult = ReadFile(hf, (LPSTR)&gmfhTemp, 
                   sizeof(MIXFILEHEADER), &dwBRead, NULL);

	if (bResult == FALSE || dwBRead == 0)
  {
    InformationStatus(ghInstStrRes, IDS_ERR_OPENING_FILE);
    CloseHandle(hf);
    return 1;
  }


	/////////////////////////////////////////
	// CHECK MIX FILE Header to see if valid

	if((gmfhTemp.dwID != SAMMPLUS_ID) || 
		(gmfhTemp.dwSize != sizeof(MIXFILEHEADER)) ||
    (gmfhTemp.dwVersion > SAMMPLUS_VERSION) ||														
		(gmfhTemp.dwDevID == 0) ||	// // for BACKWARD COMPATIBILITY, if zero then let them load it
		(gmfhTemp.dwDevID != giMixerType))	//  make sure compatible with mixfile that we are loading
  {
    ErrorBox(ghwndMain, ghInstStrRes, IDS_INCOMP_MIX_FILE);
		return 2;	// signal EXIT - make define
  }
	else
  {
    gmfhMix = gmfhTemp;
    if(gmfhTemp.dwVersion < SAMMPLUS_VERSION)
        ;//InformationStatus(ghInstStrRes, IDS_NEW_VER_PRF_FILE);
  }

	///////////////////////////////////////////////////////////////////////////////
	// If we are loading the LAS$T.mix file then try to get the SEQUENCE FILE
	// of the last loaded MIX FILE

	if(lstrcmp(szBuff, g_sequence_file_name) == 0)
	{

		lnResult = RegCreateKeyEx(
								HKEY_CURRENT_USER,
								szRegKey,
								0, 
								REG_NONE, 
								0, 
								KEY_ALL_ACCESS, 
								NULL, 
								&hKey,
								&dwDisposition );

		szRegValue = "MRUmix";
		rc = RegQueryValueEx( 
					hKey, 
					szRegValue, 
					NULL, 
					&dwType, 
					&szTempSeq, 
					&dwBufferSize ); 

		  // Set our global sequence name to the last loaded mix file

		if(rc == ERROR_SUCCESS)
			wsprintf(g_sequence_file_name, "%s", szTempSeq);

	}

	/////////////////////////////////////
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
			{
        CreateMasterViewWindow("Zoom Master View", NULL);
			}
      break;
    case MIXER_WINDOW_FILE_ID:
        if(ReadMixerWndDataFromFile(hf, &fsh))	// Returns value if ERROR, more channels than setup for
				{
			    ErrorBox(ghwndMain, ghInstStrRes, IDS_INCOMP_MIX_FILE);
					return 2;	// signal EXIT - make define
				}
        break;
    case SEQUENCE_WINDOW_FILE_ID:
        ReadSequenceWndDataFromFile(hf, &fsh, TRUE);
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

	// If saveName then add the name of the mix file to our window title
	// and the time it was last written 

	if(saveName)
	{
		GetFileInformationByHandle(hf,&bhfi);	// Lets get the time it was last written
		bResult=FileTimeToLocalFileTime(&(bhfi.ftLastWriteTime),&ftLocal);

		if(bResult)
		{
			bResult=FileTimeToSystemTime(&ftLocal,&systime);
			if(bResult)
			{
 				wsprintf(gszTitleBuffer, "%s %s: %d//%d//%d %d:%d:%d", gszMainWndTitle, pfs->szFileName,
																											systime.wMonth, systime.wDay, systime.wYear,
																											systime.wHour, systime.wMinute, systime.wSecond);
				SetWindowText(ghwndMain, gszTitleBuffer);
			}
			else
			{
				dwError = GetLastError();
			}
		}
		else
		{
// debug only			dwERR=GetLastError();
 			wsprintf(gszTitleBuffer, "%s %s", gszMainWndTitle, pfs->szFileName);
			SetWindowText(ghwndMain, gszTitleBuffer);
		}
	}

	CloseHandle(hf);


	if(bRecallFromMemBuffer)
		RecallMemoryMapBuffer(FALSE,0);

	SetFocus(ghwndZoom);			// Gamble wants the Zoom window to be the one with focus after loading

return 0;
}


//====================================================
//function: HadleCueMasterSystem
//
//
//
//
//====================================================
void	HadleCueMasterSystem ()
{
	LPMIXERWNDDATA			lpmwd; // temp memory for the Mixer Window data
	int									CueActiveCountTemp;
	int									inpCueCount;
	// Start the Cue-system
	if(g_CueMasterSystem == 0){

		lpmwd = MixerWindowDataAlloc(gwActiveMixer,
																 gpZoneMaps_Zoom,
																 MAX_CHANNELS, DCX_DEVMAP_MODULE_INPUT);
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
//function: WriteMixFile
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

	BY_HANDLE_FILE_INFORMATION	bhfi;
	FILETIME ftLocal;	// Local time
	SYSTEMTIME	systime;

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
	////////////////////////////////////////////
	// Write the File header
	//----------------------
	gmfhMix.dwID   = SAMMPLUS_ID;
	gmfhMix.dwVersion   = SAMMPLUS_VERSION;
	gmfhMix.dwSize = sizeof(MIXFILEHEADER);
	gmfhMix.dwDevID = giMixerType; // Now used to indicate MIXER TYPE
//	gmfhMix.dwDevID = 0; // IT SHOULD BE SET TO THE DEVICE ID for THE MIXER
	gmfhMix.iNumDev = 0; // the number of devices
	gmfhMix.iScrCX = 0; // the screen resolution CX
	gmfhMix.iScrCY = 0; // the screen resolution CX
	ZeroMemory(gmfhMix.iReserved, sizeof(gmfhMix.iReserved));

	bResult = WriteFile(hf, (LPVOID)&gmfhMix, 
											sizeof(MIXFILEHEADER), &dwBWrite, NULL);
	if(bResult == FALSE || dwBWrite == 0)
			goto ON_ERROR_EXIT;


	////////////////////////////////////////////
  // Save the Groups
	////////////////////////////////////////////
  ZeroMemory(&fsh, sizeof(fsh));
  // Write the Labelss ID ...
  //
  fsh.dwID    = GROUPS_FILE_ID;
  fsh.lSize   = sizeof(GROUPSTORE)*MAX_GROUPS_COUNT;
  fsh.dwVersion = 0;

  WriteFile(hf, &fsh, sizeof(fsh), &dwBWrite, NULL);  
  WriteFile(hf, gpGroupStoreFaders, sizeof(GROUPSTORE)*MAX_GROUPS_COUNT, &dwBWrite, NULL);  
    

  ////////////////////////////////////////////
	// Save the Labels.
  ////////////////////////////////////////////
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

  ////////////////////////////////////////////
	// Write this section .. so we know that tha master window was visible
  // before we start creating the other windows
  ////////////////////////////////////////////
  if(ghwndMaster)
  {
    fsh.dwID = MASTER_WINDOW_FILE_ID;
    fsh.lSize = 0;
    fsh.dwVersion = 0;
    WriteFile(hf, &fsh, sizeof(fsh), &dwBWrite, NULL);  
  }


	////////////////////////////////////////////
	// Save all of the Open Windows
  ////////////////////////////////////////////
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


	////////////////////////////////////////////
  // Write the Device memory Image ..
  //
	////////////////////////////////////////////
  fsh.dwID = DEV_CONTROLS_IMAGE_FILE_ID;
  fsh.lSize = giMemMapSize;
  fsh.dwVersion = 0x00000001;
  WriteFile(hf, &fsh, sizeof(fsh), &dwBWrite, NULL);  
  
  WriteFile(hf, gpwMemMapMixer, fsh.lSize, &dwBWrite, NULL);  


	////////////////////////////////////////////
	// Save the Memory State
	////////////////////////////////////////////

	// If saveName then add the name of the mix file to our window title
	// and the time it was last written 

	if(showName)
	{
		GetFileInformationByHandle(hf,&bhfi);	// Lets get the time it was last written
		bResult=FileTimeToLocalFileTime(&(bhfi.ftLastWriteTime),&ftLocal);

		if(bResult)
		{
			bResult=FileTimeToSystemTime(&ftLocal,&systime);
			if(bResult)
			{
 				wsprintf(gszTitleBuffer, "%s %s: %d//%d//%d %d:%d:%d", gszMainWndTitle, pfs->szFileName,
																											systime.wMonth, systime.wDay, systime.wYear,
																											systime.wHour, systime.wMinute, systime.wSecond);
				SetWindowText(ghwndMain, gszTitleBuffer);
			}
		}
		else
		{
// debug only			dwERR=GetLastError();
 			wsprintf(gszTitleBuffer, "%s %s", gszMainWndTitle, pfs->szFileName);
			SetWindowText(ghwndMain, gszTitleBuffer);
		}
	}

	// Set the End-Of-File
	// Close the file
	//--------------------
	SetEndOfFile(hf);
	CloseHandle(hf);

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
int							i, iPhisChannel;
int							iRackMaxChannel[]={18,32,58,78};							// cabaret, showtime, event 40, event 60

	///////////////////////////////////////////////////////////
	// READ the type of the modules in the Windows in the file

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
                                   MAX_CHANNELS, DCX_DEVMAP_MODULE_INPUT);
        break;
    case IDZM_INPUT_ZOOM:
      pmwd = MixerWindowDataAlloc((WORD)mwdfile.iMixer,
                                   gpZoneMaps_Zoom,
                                   MAX_CHANNELS, DCX_DEVMAP_MODULE_INPUT);
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


	///////////////////////////////////////////
	// VALIDATE the MIX FILE with what
	// RACK we are set for

	for(i=pmwd->iStartScrChan;i<pmwd->iEndScrChan+1;i++)
	{
		iPhisChannel = LOBYTE(pmwd->lpwRemapToScr[i]);
		if((giMixerType) && (iPhisChannel > iRackMaxChannel[giMixerType-1]))	// giMixerType is base 1
			return 1;		// error, we have more channels then we are supposed to
	}

	switch(pmwd->lpZoneMap->wID)
	{
    case IDZM_INPUT_FULL:
      // Create the Window
      CreateFullViewWindow(pmwd->szTitle, pmwd);
      break;
    case IDZM_INPUT_ZOOM:
      // Create the Window
      hwndLink = CreateZoomViewWindow(ghwndMDIClient,pmwd->szTitle, pmwd, DCX_DEVMAP_MODULE_INPUT);

//       if(pmwd->bLink && hwndLink)
          ghwndZoom = hwndLink;	// Gamble wants zoom window with focus, Always set this
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
	fsh.dwCurrentSeqSelection = SeqSelectionIndex();	// SAVE THE CURRENTLY SELECTED SEQUENCE


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
//          BOOL		bSetSeqSelection	
//                  FALSE for Fkey - do not change the sequence selection
//=============================================
extern HWND	ghwndSeq;
int     ReadSequenceWndDataFromFile(HANDLE hFile, LPFILESECTIONHEADER pFSH, BOOL bSetSeqSelection)
{
SEQUENCEWINDOWFILE      seqWF;
BOOL                    bResult;
DWORD                   dwBRead;

	ZeroMemory(&seqWF, sizeof(SEQUENCEWINDOWFILE));

	bResult = ReadFile(hFile, (LPVOID)&seqWF, sizeof(SEQUENCEWINDOWFILE), &dwBRead, NULL);
	if(bResult == FALSE || dwBRead != sizeof(SEQUENCEWINDOWFILE))
			return 1;// error

	ShowSeqWindow(FALSE);	// Create the sequence window but don't show it until we position it

	if(ghwndSeq && seqWF.rWndPos.right != 0)
	{
		SetWindowPos (ghwndSeq, HWND_TOP, seqWF.rWndPos.left, seqWF.rWndPos.top,
																			seqWF.rWndPos.right - seqWF.rWndPos.left, 
																			seqWF.rWndPos.bottom - seqWF.rWndPos.top,
																			SWP_NOZORDER);
	}

	// Now show the sequence window, avoid the flashing.
	ShowSeqWindow(TRUE);

	/////////////////////////////////////////////////
	// If we saved the current sequence selection
	// then restore our global variable so that
	// we can move the current selection to the saved
	// point after loading it in.

	if(bSetSeqSelection)
	{
		if(pFSH->dwCurrentSeqSelection)
			g_dwCurrentSeqSelection = pFSH->dwCurrentSeqSelection;
		else
			g_dwCurrentSeqSelection = 0;

				// Set the saved selection if there is one
				// zero is valid for the first entry

					SeqGoToIndex(g_dwCurrentSeqSelection);
					UpdateSeqSceneNumber();			
	}

return 0;
}

////////////////////////////////
// Get the currently selected
// sequence and display its
// index number
////////////////////////////////

void UpdateSeqSceneNumber(void)
{
	char szTemp[16];

	itoa(SeqSelectionIndex()+1,&szTemp,10);
	SetWindowText(ghwndTBSeqSceneNumber, szTemp);

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
//function: ReadGroupWndDataFromFile
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

	ShowGroupWindow(FALSE);	// Create the Group window but DON'T show until we position it.

	if(ghwndGroup)
	{
		SetWindowPos (ghwndGroup, HWND_TOP, groupWF.rWndPos.left, groupWF.rWndPos.top,
																				groupWF.rWndPos.right - groupWF.rWndPos.left, 
																				groupWF.rWndPos.bottom - groupWF.rWndPos.top,
																				SWP_NOZORDER);
	}
	// Now show the group window, avoid the flashing.
	ShowGroupWindow(TRUE);

return 0;
}


//====================================================
//function: WriteFkeyFile(LPFILESTRUCT pfs, int fKey)
//
// fKey is the function key pressed
//
//
//====================================================
int     WriteFkeyFile(LPFILESTRUCT pfs, int fKey)
{
HANDLE          hf;
char            szFile[1024];
BOOL            bResult;
DWORD           dwBWrite;
HWND            hwnd;
int             *piWndID;
FILESECTIONHEADER   fsh;

	wsprintf(szFile, "%s%s", pfs->szFileDir, pfs->szFileName);

	hf = CreateFile(szFile, GENERIC_WRITE, 
									0, NULL, OPEN_ALWAYS, 
									FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
									NULL);

	if(hf == INVALID_HANDLE_VALUE)
	{
    return 1;
	}

	// Write the File header
	//----------------------
	gmfhMix.dwID   = SAMMPLUS_ID;
	gmfhMix.dwVersion   = SAMMPLUS_VERSION;
	gmfhMix.dwSize = sizeof(MIXFILEHEADER);
	gmfhMix.dwDevID = giMixerType; // Now used to indicate MIXER TYPE
//	gmfhMix.dwDevID = 0; // IT SHOULD BE SET TO THE DEVICE ID for THE MIXER
	gmfhMix.iNumDev = 0; // the number of devices
	gmfhMix.iScrCX = 0; // the screen resolution CX
	gmfhMix.iScrCY = 0; // the screen resolution CX
	ZeroMemory(gmfhMix.iReserved, sizeof(gmfhMix.iReserved));

	bResult = WriteFile(hf, (LPVOID)&gmfhMix, 
											sizeof(MIXFILEHEADER), &dwBWrite, NULL);
	if(bResult == FALSE || dwBWrite == 0)
			goto ON_ERROR_EXIT;

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


	// Set the End-Of-File
	// Close the file
	//--------------------
	SetEndOfFile(hf);
	CloseHandle(hf);
	return 0;

	ON_ERROR_EXIT:
	ErrorBox(ghwndMain, ghInstStrRes, IDS_ERR_CREATING_FILE);

	SetEndOfFile(hf);
	CloseHandle(hf);

	return 1;
}



//====================================================
//function: LoadFkeyFile(LPFILESTRUCT pfs, int fKey)
//
// fKey is the function key pressed
//
//
//====================================================

int     LoadFkeyFile(LPFILESTRUCT pfs, int fKey)
{
HANDLE              hf;
FILESECTIONHEADER   fsh;
char                szFile[1024];
char								szBuff[MAX_PATH];
char								szTempSeq[MAX_PATH];
BOOL                bResult;
BOOL                bRecallFromMemBuffer = FALSE;
DWORD               dwBRead;

	// Registry variables 

	LONG lnResult;
	HKEY hKey = NULL;
	DWORD dwDisposition;
	static LPCTSTR szRegKey = "Software\\CorTek\\VACS";
	static LPCTSTR szRegValue; 
	DWORD dwType;
	DWORD rc;
	DWORD dwBufferSize = sizeof( g_sequence_file_name );  

	wsprintf(szFile, "%s%s", pfs->szFileDir, pfs->szFileName);

	wsprintf(szBuff, "%smix\\%s", gszProgDir, "LA$T.mix");	// Used to see if we should check registery


	hf = CreateFile(szFile, GENERIC_READ, 
									0, NULL, OPEN_EXISTING, 
									FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
									NULL);
	if(hf == INVALID_HANDLE_VALUE)
	{
			return 1;
	}

	// Only close windows if we have a valid file available

	CloseAllMDI();	// close all the open windows

	bResult = ReadFile(hf, (LPSTR)&gmfhTemp, 
										 sizeof(MIXFILEHEADER), &dwBRead, NULL);
	if(bResult == FALSE || dwBRead == 0)
	{
		InformationStatus(ghInstStrRes, IDS_ERR_OPENING_FILE);
		CloseHandle(hf);
		return 1;
	}


	/////////////////////////////////////////
	// CHECK MIX FILE Header to see if valid
	// This validaties ONLINE and OFFLINE
	// modes of operation
	/////////////////////////////////////////

	if((gmfhTemp.dwID != SAMMPLUS_ID) || 
		(gmfhTemp.dwSize != sizeof(MIXFILEHEADER)) ||
    (gmfhTemp.dwVersion > SAMMPLUS_VERSION) ||
		(gmfhTemp.dwDevID == 0) ||	// // for BACKWARD COMPATIBILITY, if zero then let them load it
		(gmfhTemp.dwDevID != giMixerType))	// != -1 then OFFLINE, make sure compatible with mixfile that we are loading
  {
    ErrorBox(ghwndMain, ghInstStrRes, IDS_INCOMP_MIX_FILE);
		return 2;	// signal EXIT - make define
  }
	else
  {
    gmfhMix = gmfhTemp;
    if(gmfhTemp.dwVersion < SAMMPLUS_VERSION)
        ;//InformationStatus(ghInstStrRes, IDS_NEW_VER_PRF_FILE);
  }



	////////////////////////////////////////////////////////////////////////////
	// If we are loading the LAS$T.mix file then try to get the sequence
	// of the last loaded mix file

	if(lstrcmp(szBuff, g_sequence_file_name) == 0)
	{

		lnResult = RegCreateKeyEx(
								HKEY_CURRENT_USER,
								szRegKey,
								0, 
								REG_NONE, 
								0, 
								KEY_ALL_ACCESS, 
								NULL, 
								&hKey,
								&dwDisposition );

		szRegValue = "MRUmix";
		rc = RegQueryValueEx( 
					hKey, 
					szRegValue, 
					NULL, 
					&dwType, 
					&szTempSeq, 
					&dwBufferSize ); 

		if(rc == ERROR_SUCCESS)	// Only set if it was a success
			wsprintf(g_sequence_file_name, "%s", szTempSeq);	// Set it to the last .mix file in registry.

	}


	/////////////////////////
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
				if(ReadMixerWndDataFromFile(hf, &fsh))	// Returns value if ERROR, more channels than setup for
				{
					ErrorBox(ghwndMain, ghInstStrRes, IDS_INCOMP_MIX_FILE);
					return 2;	// signal EXIT - make define
				}
					break;

			case SEQUENCE_WINDOW_FILE_ID:
					ReadSequenceWndDataFromFile(hf, &fsh,FALSE); // get the sequence window up but don't set its position
					break;

			case GROUP_WINDOW_FILE_ID:
					ReadGroupWndDataFromFile(hf, &fsh);
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


return 0;
}


///////////////////////////////////////////////////////////////
//function: LPSTR	GetMixerTypeName(enum MIXER_TYPES iMixType )
//
// Used above to get the mixer name 
// to be displayed in the window bar
//
// iMixType is set in init.c
//
///////////////////////////////////////////////////////////////
//

LPSTR	GetMixerTypeName(enum MIXER_TYPES iMixType )
{
	static char			szText[80] = "Not Set";

	switch(iMixType)
	{

		case DCX_CABARET:
			wsprintf(szText,"%s","Cabaret -");
			break;

		case DCX_SHOWTIME:
			wsprintf(szText,"%s","Showtime -");
			break;

		case DCX_EVENT_40:
			wsprintf(szText,"%s","Event 40 -");
			break;

		case DCX_EVENT_60:
			wsprintf(szText,"%s","Event 60 -");
			break;

		case DCX_CUSTOM:
			wsprintf(szText,"%s","Custom -");
			break;
	
		default:
			wsprintf(szText,"%s","Undefined");
			break;

	}
	
	return &szText[0];

}

