//=================================================
// Copyright 1998-2001 CorTek Software, Inc.
//=================================================
//
//
// $Author::                                      $
// $Archive::                                     $
// $Revision::                                    $
//

#include "SAMM.h"
#include "ConsoleDefinition.h"
#include "SAMMEXT.h"
#include "MACRO.h"
#include "..\\commport\\commport.h"

HWND          g_hwndSplashScreen = NULL;

void          ShowSplashScreen(BOOL );
BOOL APIENTRY dlgProcSplash(HWND ,UINT , UINT , LONG );

extern LPSTR	GetMixerTypeName(enum MIXER_TYPES iMixType ); // see mix_files.c


//////////////////////////////////////////////////////////
// the initialize procedure
// sets all the variables
// windows, buffers, ....
//////////////////////////////////////////////////////////

int   InitializeProc(void)
{
  int     iReturn;
  char    szString[256];
  LPSTR   lpstrFName = NULL;
  HWND    hwndTest = NULL;
	int			CTRL_key;	 

	int	i;
	char iChannelCount = 0;

	
  // ShowSplashScreen(TRUE);

	gDisplayEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(gDisplayEvent == NULL)
		return IDS_ERR_EVENT_HANDLE; 


  // Get the Directory where the proogram is ..
  //-------------------------------------------
  if(GetModuleFileName(ghInstMain, szString, 256)==0)
      return IDS_ERR_PROG_DIR;

  ZeroMemory(gszProgDir, 256);
  GetFullPathName(szString, 256, gszProgDir, &lpstrFName);
  if(lpstrFName)
  {
    // now we have only the path
    // because we put 0 where the actual file name begins
    //---------------------------------------------------
    *lpstrFName = 0;
  }


  // Load the Bitmap Resource DLL
  //-----------------------------
  wsprintf(szString,"%s%s",gszProgDir,"usengl.dll");
  ghInstStrRes = LoadLibrary(szString);
  if(ghInstStrRes == NULL )
  {
    return IDS_NO_RES_FILE;
  }

  // Load the Bitmap Resource DLL
  //-----------------------------
  wsprintf(szString,"%s%s",gszProgDir,"ConsoleDefinition.dll");
  ghInstConsoleDef = LoadLibrary(szString);
  if(ghInstConsoleDef == NULL )
  {
      return IDS_NO_RES2_FILE;
  }
	// Is Vacs already running
	if (CDef_isRunning ())
		return IDS_MULTIPLE_APPLICATIONS_RUNNING;

  // Read the Preferences
  //---------------------
  LoadPreferences();

  if(CreateApplicationDirectories() == FALSE)
  {
    return IDS_ERR_NO_SUPPORT_DIRECTORIES;    
  };


  g_hConsoleFont = CreateFont(15, 6, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET,
                              OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                              DEFAULT_PITCH | FF_MODERN, "Arial");

  // Load the main menu
  //-------------------
  ghMainMenu = LoadMenu(ghInstStrRes, MAKEINTRESOURCE(SAC_MAIN_MENU));
  if(ghMainMenu == NULL)
      return IDS_ERR_IN_RES_FILE;

  // Create the Main Window
  //-----------------------
  iReturn = CreateMainWindow();
  if(iReturn)
      return  iReturn;


  // Zero out the damn thing
  //------------------------
  ZeroMemory(&gfsMix, sizeof(FILESTRUCT));

  gDeviceSetup.hwndMain = ghwndMain;             // Main application Window
  gDeviceSetup.hinst    = ghInstMain;            // Application Instance
  gDeviceSetup.pfTXComm = DefinitionCallback;    // Callback function for the DLL to call directly into

  // Initialize the Definitiom DLL .... internal stuff

  if(CDef_Init()  == 0)   // This loads the DCX.BIN file if its exists
  {
    // transfer these variables to the ConsoleDefinition DLL
    if(CDef_SetGlobalData(&gDeviceSetup))
      return IDS_DEFINITION_FAILED;

  }
  else
  {
    // transfer these variables to the ConsoleDefinition DLL
 
    if(CDef_SetGlobalData(&gDeviceSetup))
      return IDS_DEFINITION_FAILED;

    // Show the Preferences for the Console definition
    // 
    CDef_Preferences(ghwndMain); 

  }

	//////////////////////////////////////////////////////////
  // start the client and get all the data from the GServer
  //
	// If Control key is held down then we ask user for
	// the IP address, etc. Bring up the preference dialog
	//
	// Else we transfer the current setup configuration data
	// in the gDeviceSetup structure.  For OFFLINE work
	// this needs to be operator controled.
	//////////////////////////////////////////////////////////

	CTRL_key = GetKeyState (VK_CONTROL);

  if( (CTRL_key < 0) || !CDEF_GetCSData(ghwndMain))
  {
		if(CDef_Preferences (ghwndMain) == APPLY)
		{
			if( CDEF_GetCSData(ghwndMain))
			{
				if (CDEF_ReadDCXBinFile ())
					return IDS_DCXBIN_FILE_FAILED;
				gInitialized = TRUE;
			}
			else
				return IDS_DEFINITION_FAILED2;
		}
		else
		{
				return IDS_DEFINITION_FAILED3;		// User CANCEL - Exiting
		}
  }
	else
	{
		// Load the DCX bin file
		if (CDEF_ReadDCXBinFile ())
			return IDS_DCXBIN_FILE_FAILED;
		gInitialized = TRUE;
	}


/*****************************************************************
DCX System Types:

DCX Cabaret       12 Inputs and 2 Sub/Martix (Total 14 module strips)
DCX Showtime    24 Inputs and 6 Sub/Matrix (Total 30 module strips)
DCX Event 40     40 Inputs and 8 Sub/Matrix (Total 48 module strips)
DCX Event 60     60 Inputs and 8 Sub/Matrix (Total 68 module strips)

So the formula to determine what DCX System Type Vacs has running would
be:

<15 strips = DCX Cabaret
>14 strips but <31 strips = DCX Showtime
>29 strips but <49 strips = DCX Event 40
>48 strips = DCX Event 60

******************************************************************/

	// Count the number of input channels so we can determine
	// what type of console it is.

	for(i=0;i<MAX_CHANNELS;i++)
	{
		if(gDeviceSetup.iaChannelTypes[i] == DCX_DEVMAP_MODULE_INPUT)
			iChannelCount++;
	}

	// Set what type of mixer we are connected to
	// or what was chosen in the offline option

	switch(iChannelCount)
	{
		case 12:
			giMixerType = DCX_CABARET;
			break;
		case 24:
			giMixerType = DCX_SHOWTIME;
			break;

		case 40:
			giMixerType = DCX_EVENT_40;
			break;

		case 60:
			giMixerType = DCX_EVENT_60;
			break;

		default:
			giMixerType = DCX_CUSTOM;
			break;

			break;

	}

	////////////////////////////////////////////////////////////
	// Get the MIXER type from the preferences, should only be
	// set if we selected OFFLINE, else it will be -1

	giMixerTypeOffline=CDef_MixerTypePreference();

	// We know what type of mixer it is, now set the window title.

	wsprintf(gszMainWndTitle, "CorTek -VACS-  %s", GetMixerTypeName(giMixerType));
	SetWindowText(ghwndMain, gszMainWndTitle);

  // Load Some default Strings
  //--------------------------
  //if(LoadString(ghInstStrRes, IDS_FILE_DEFEXT, gstrDefExt, sizeof(gstrDefExt)) == 0)
  //    wsprintf(gstrDefExt, "mix"); // needs to be done better

  // Initialize the Double Linked list
  // for the Mixer Windows
  // We can use this to browse all
  // of the opened Mixer_Windows
  //-----------------------------------
  //gpMixerWindows_DL_List =  InitDoubleLinkedList( sizeof(MW_DL_LIST), MAX_MIXER_WINDOW_OPENED,
  //                                                FALSE, NULL, NULL);
  //if(gpMixerWindows_DL_List == NULL)
  //    return IDS_ERR_ALLOCATE_MEMORY;



  //-----------------
  // Load the Pallete
  // and realize it
  //-----------------
  CreateResPalette((LPSTR)MAKEINTRESOURCE(IDB_IQS_PALETTE));
  if(ghPalette == NULL)
      return IDS_ERR_CREATING_PALETTE;

  //--------------------------------
  // Create the window which we are
  // going to use for the 256 colors
  // .. it will free its resources
  // when it is closing
  //--------------------------------
  iReturn = Init256Colors();
  if(iReturn)
      return iReturn;

  UpdatePalette(TRUE, ghdc256);


  //----------------------------------
  // Load Bitmap for the Client Window
  //
  // The Bitmap is deleted in the
  // ShutDownMDIClient function
  //----------------------------------
  ghbmpClientBk = LoadBitmap(ghInstConsoleDef, MAKEINTRESOURCE(IDB_CLIENT_BACKGROUND));
  if(ghbmpClientBk == NULL)
      return IDS_ERR_IN_RES2_FILE;

  //-----------------------------------
  // and now get the size of the bitmap
  //-----------------------------------
  GetObject(ghbmpClientBk, sizeof(BITMAP), &gbmpClientInfoBk);

  //-----------------------------------
  // Load the VU bitmap
  //-----------------------------------
  ghbmpVUONVert = LoadBitmap(ghInstConsoleDef, MAKEINTRESOURCE(IDB_VU_ON_VERTICAL));
  if(ghbmpVUONVert == NULL)
      return IDS_ERR_IN_RES3_FILE;
  ghbmpVUONHoriz = LoadBitmap(ghInstConsoleDef, MAKEINTRESOURCE(IDB_VU_ON_HORIZONTAL));
  if(ghbmpVUONHoriz == NULL)
      return IDS_ERR_IN_RES3_FILE;

  //-----------------------------------
  // and now get the size of the bitmap
  //-----------------------------------
  GetObject(ghbmpVUONVert, sizeof(BITMAP), &gbmpVUONVert);
  GetObject(ghbmpVUONHoriz, sizeof(BITMAP), &gbmpVUONHoriz);
  InitVULookupTables(FALSE);  // Log VU data - NOT linear

  //---------------------------------------
  // Load the Bitmap for the Number Display
  //---------------------------------------
  g_hbmpNumbers = LoadBitmap(ghInstConsoleDef, MAKEINTRESOURCE(IDB_NUMBERS_200_40));
  //g_hbmpNumbers = Load256Bitmap(ghInstConsoleDef, ghdc256, MAKEINTRESOURCE(IDB_NUMBERS_200_40));
  if(g_hbmpNumbers == NULL)
      return IDS_ERR_IN_RES4_FILE;

  //-----------------------------------
  // and now get the size of the bitmap
  //-----------------------------------
  GetObject(g_hbmpNumbers, sizeof(BITMAP), &g_bmpNumbersInfo);



  //------------------------------------
  // Register Some Of the Window Classes
  //------------------------------------
  iReturn = RegisterFullViewClass();
  if(iReturn)
      return iReturn;

  iReturn = RegisterZoomViewClass();
  if(iReturn)
      return iReturn;

  iReturn = RegisterMasterViewClass();
  if(iReturn)
      return iReturn;

  iReturn = RegLblGroupWnd();
  if(iReturn)
      return iReturn;

  iReturn = RegisterSeqWindowClass();
  if(iReturn)
      return iReturn;

  iReturn = RegisterGroupWindowClass();
  if(iReturn)
      return iReturn;

  iReturn = RegisterScrollWindowClass();
  if(iReturn)
      return iReturn;

  iReturn = RegisterTrackingWindowClass();
  if(iReturn)
      return iReturn;

  
	iReturn = RegisterStereoCueMetersViewClass();
  if(iReturn)
      return iReturn;


  //------------------------
  // Display the Main Window
  //------------------------
  ShowWindow(ghwndMain, SW_SHOWMAXIMIZED);
  UpdateWindow(ghwndMain);


	showStereoCueMetersView ();
  
	// Shouldn't we read in the dcx bin table here??????????
	// since we just got it from the client


  iReturn = Init_MixerData();
  if(iReturn)
    return iReturn;


  //----------------------------------------
	// prepare the External controls lookup
  //----------------------------------------
	InitExternalIface();
	SetDataInWindow(ghwndMain);

  //----------------------------------------
	// now try to open the External interface
	//
  //----------------------------------------
	if (OpenCommPort("\\\\.\\COM1"))
	{
//		unsigned char enable_time[6] = { 0xF0,0x15,0x25,0x31,0x01,0xF7};
//		unsigned char	spec_mode_1[9] = {0xF0,0x15,0x25,0x02,0x00,0x00,0x00,0x00,0xF7};
//		WriteCommBlock (spec_mode_1, sizeof (spec_mode_1));
//		WriteCommBlock (enable_time, sizeof (enable_time));
	}

  return 0;
};


//----------------------------------------
// Init_MixerData
//
//
//
//----------------------------------------

int     Init_MixerData(void)
{
  int     iReturn;


  // Start the Vu data ... Flow
  //
  CDef_StopVuData();
  
  
  // Load and Create the ZoneMaps
  // and all of the other good stuff
  // that is needed to talk to the 
  // mixer *** BEGIN ***
  //--------------------------------

  //--------------------------------------
  // Get Global Mixer Phisical Description
  //--------------------------------------
  iReturn = GetGlobalMixerPhisDesc(ghInstConsoleDef);
  if(iReturn)
      return iReturn;


  //------------------------------------------------------
  // Use the data from the Server ... before InitMemoryMap
  // so we will allocate enough memory
  //------------------------------------------------------


  //----------------------------------------
  // Initialize the Memory map
  //----------------------------------------
  iReturn = InitMemoryMap();
  if(iReturn)
      return iReturn;

  //---------------------------------
  // Init the Global Resource Storage
  // for different types
  //---------------------------------
  iReturn = InitBinResGlobal();
  if(iReturn)
      return iReturn;

  iReturn = InitRdOutResGlobal();
  if(iReturn)
      return iReturn;

  iReturn = InitBMPResGlobal();
  if(iReturn)
      return iReturn;

  //-------------------------------
  // Time to apply the Prefferences
  //-------------------------------
  if(ApplyPreferences(TRUE) == FALSE)
      return IDS_ERR_APPLY_PREF;


  //--------------------------
  // Load the Zonemaps and the
  // respective bitmaps
  //--------------------------
  iReturn = LoadZoneMapIDs();
  if(iReturn)
      return iReturn;

  //-------------------------------
  // Now Load the actual Zone maps
  // this will also load all of the
  // bitmaps needed to assemble
  // the controls, mixers
  //-------------------------------
  iReturn = LoadZoneMaps();
  if(iReturn)
      return iReturn;


  //----------------------------------------------------------------------------
  // Prepare the map for the physical modules
  // Sub-Aux has the Matrix module mapped to it ....
  // The Master has the Cue Module mapped to it on the screen .. 
  // ... 
  // We need to this after the Zonemaps have been loaded 
  // since this function will affect a member variable of the Zonemap Structure
  //----------------------------------------------------------------------------

  iReturn = InitPhysicalModuleMap();
  if(iReturn)
    return iReturn;

  //--------------------------
  // initialize the Memory Map
  //--------------------------
  SetMemoryMapDefaults();

// FDS NOT NEEDED HERE  ShowSeqWindow(FALSE);  // Make sure we have all index tables initialized ...
  
  //--------------------------
  // Show the Master Window
  //--------------------------
  CreateMasterViewWindow("Zoom Master View", NULL); 



  //--------------------------
  // ZoneMaps and other stuff
  // *** END ***
  //-------------------------

  //--------------------------
  //Enable the menu Items 
  //--------------------------
  EnableMenuItem(ghMainMenu, IDM_V_FULLZOOM, MF_ENABLED);
  EnableMenuItem(ghMainMenu, IDM_V_FULLZOOM_AUX, MF_ENABLED);
  EnableMenuItem(ghMainMenu, IDM_V_FULLZOOM_MASTER, MF_ENABLED);
  EnableMenuItem(ghMainMenu, IDM_V_FULLCONSOLE, MF_ENABLED);


  return iReturn;
}


//========================
// the ShutDown procedure
// close all the buffers,
// handles,...
//========================
void      ShutdownProc(void)
{
int     iCount = 0;
//HMENU   hMenu;

	WaitForSingleObject(gDisplayEvent, 1000);
	SetEvent(gDisplayEvent);
	// shutdown the ConsoleDefinition DLL
	CDef_ShutDown();
	WaitForSingleObject(gDisplayEvent, 1000);

	CloseHandle(gDisplayEvent);

	// Free the memory for the menu
	//-----------------------------
	DestroyMenu(ghMainMenu);

	CloseComm();
	// Close the Midi Drivers
	//-----------------------
	//CloseAllMIDIInDev();
	//CloseAllMIDIOutDev();

	// Close all of the buffers
	// after the drivers have been closed
	//===================================
	//UnPrepareMidiDataTransferBuffers(&gMidiDev);
	//UnPrepareMidiDataTransferBuffers(&gMTCDev);


	// Free the memory for the MemoryMap
	//----------------------------------
	FreeMemoryMap();


	// DeInitialize the Groups
	//
	DeInitGroups();

	// Dealocate the zone maps buffers
	//--------------------------------
	FreeZoneMapsMemory();

	// Free the Znone Map IDs
	//-----------------------
	FreeZoneMapIDs();


	// Free the Global Resource Storage
	// for different types
	//---------------------------------
	FreeBinResGlobal();
	FreeRdOutResGlobal();
	FreeBMPResGlobal();


	// Free the Double Linked List
	// for the Mixer_Windows
	//----------------------------
	//FreeDLListRootAll(gpMixerWindows_DL_List);


	// Free the Palette
	//-----------------
	if(ghPalette)
			DeleteObject(ghPalette);

//-------------------
// delete few bitmaps
//-------------------

  if(gbmpVUVertBuffer)
    DeleteObject(gbmpVUVertBuffer);
  if(gbmpVUHorzBuffer)
    DeleteObject(gbmpVUHorzBuffer);

  if(ghbmpVUONVert)
    DeleteObject(ghbmpVUONVert);
  
  if(ghbmpVUONHoriz)
    DeleteObject(ghbmpVUONHoriz);

  if(ghbmpClientBk)
      DeleteObject(ghbmpClientBk);

  if(g_hbmpNumbers)
      DeleteObject(g_hbmpNumbers);

  if(g_hbmpBuffer)
    DeleteObject(g_hbmpBuffer);

  if(g_hdcMemory)
    DeleteDC(g_hdcMemory);

  if(g_hdcBuffer)
    DeleteDC(g_hdcBuffer);

  if(g_hdcTempBuffer)
    DeleteDC(g_hdcTempBuffer);

  if(g_hConsoleFont)
    DeleteObject(g_hConsoleFont);

	//-------------------
	// Close the Dlls
	// with resources
	//-------------------

	FreeLibrary(ghInstConsoleDef);
	FreeLibrary(ghInstStrRes);

return;

}


//==================================================
//FUNCTION: GetGlobalMixerPhisDesc
//
//return:
//          0 = OK;
//          n = error;
//
// purpose:
//      Load the Phisical Description of the
//  Mixer
//
//==================================================
int       GetGlobalMixerPhisDesc(HINSTANCE hinst)
{
  HRSRC           hres;
  HGLOBAL         hglob;
  LPMIXERDESC     lpMixerDesc;

  hres = FindResource(hinst, MAKEINTRESOURCE(MIXER_PHIS_DESC), MAKEINTRESOURCE(RT_MIXER_PHIS_DESC));
  if(hres == NULL)
      return IDS_ERR_LOCATING_RESOURCE;

  hglob = LoadResource(hinst, hres);
  if(hglob==NULL)
      return  IDS_ERR_LOADING_RESOURCE;

  lpMixerDesc = (LPMIXERDESC)LockResource(hglob);

  giMax_MIXERS   = lpMixerDesc->iMaxMixers;
//  giMax_CHANNELS = lpMixerDesc->iMaxChannels;
//  giMax_CHANNELS = GetDeviceChannelCount();
  giMax_CHANNELS = MAX_CHANNELS;
  giMax_CONTROLS = lpMixerDesc->iMaxControls;
  lstrcpy(gsz_MIXERNAME, lpMixerDesc->szMixerName);

  UnlockResource(hglob);
  FreeResource(hglob);
  return 0;
}



//===============================================================
// FUNCTION: AdjustGlobalDeviceDescription
//
//
//
int  GetDeviceChannelCount(void)
{
  // Actualy we might not need this at all
/*
  int       iRet = 0;
  int       iCount, iMCount;

  for(iMCount = 0; iMCount < 80; iMCount ++)
  {
    switch(gDeviceSetup.iaChannelTypes[iMCount])
    {
    case 1:
    case 2:
    case 5:
      iRet ++;
      break;
    }
  }
  //return iRet;  
*/
  return 80;
};


//===============================================================
// FUNCTION: ShowSplashScreen
//
//
//
void    ShowSplashScreen(BOOL bShow)
{

	if(bShow)
	{
		if(g_hwndSplashScreen == NULL)
		{
			g_hwndSplashScreen = CreateDialog(ghInstMain, MAKEINTRESOURCE(IDD_SPLASH), NULL, dlgProcSplash);
			ShowWindow(g_hwndSplashScreen, SW_SHOW);
		}

	}
	else
	{
		if(g_hwndSplashScreen)
			DestroyWindow(g_hwndSplashScreen);
		g_hwndSplashScreen = NULL;
	}

return;
}

//=======================================
//
// Property Sheet Dialog box routines
//
//=======================================
//=======================================
BOOL APIENTRY dlgProcSplash(HWND hDlg,UINT message, UINT wParam, LONG lParam)
{

	switch (message)
	{
		case WM_INITDIALOG:	
	//		return (TRUE);
			break;
	}

return (FALSE);   
}
