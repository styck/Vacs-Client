//=================================================
// Copyright 1998-2001 CorTek Software, Inc.
//=================================================
//
//
// $Author::                                      $
// $Archive::                                     $
// $Revision::                                    $
//
//=================================================
// Global Definitions
//=================================================

#include "SAMM.H"


// HERE WE PUT ALL OF THE GLOBAL VARIABLES
// SO THE PRECOMPILED HEADERS WILL WORK

#include "consoledefinition.h"


//--------------------
// Application General
//--------------------
char                gszProgDir[256];
HINSTANCE           ghInstConsoleDef = NULL;
HINSTANCE           ghInstStrRes = NULL;
HANDLE              ghMainMenu = NULL;
int                 giActiveMixers = 16;

HFONT               g_hConsoleFont = NULL;

UINT                g_CtrlTimer = 0;

//----------
// File Open
//----------
FILESTRUCT          gfsMix;
FILESTRUCT          gfsTemp;
char                gstrDefExt[128];
char                gstrFFilter[256];
MIXFILEHEADER       gmfhMix;
MIXFILEHEADER       gmfhTemp;


//------
// Mixer
//------
WORD                gwActiveMixer = 0;
int                 giMixerWndOpened = 0;
LPDLROOTPTR         gpMixerWindows_DL_List = NULL; // the root for the Mixer windows double linked list


//----------------------
// Global mixer settings
//----------------------
int                 giMax_MIXERS = 0;
int                 giMax_CHANNELS = 0; // per phisical mixer
int                 giMax_CONTROLS = 0; // per phisical channel
char                gsz_MIXERNAME[40];
DEVICE_SETUP_DATA   gDeviceSetup = {0};
int									gCueActiveCount=0;                                       

//------------
// Main Window
//------------
HINSTANCE           ghInstMain = 0;
HWND                ghwndMain = NULL;
HWND								ghwndTBPlay = NULL;
HWND								ghwndTBGeneral = NULL;
char                gszMainWndClass[] = "MAIN_WND_CLASS";
//char                gszMainWndTitle[] = "CorTek -VACS-                 "; was overwritting stuff after it
char                gszMainWndTitle[128]; // Build this dynamically during the init process
char                gszFullViewImageClass[] = "FULL_VIEW_IMAGE";
char                gszLblGroupClass[] = "LABEL_GROUP_CLASS";
char                gszSeqClass[] = "SEQUENCE MDI WINDOW";
char                gszGroupClass[] = "GROUP MDI WINDOW";
char                gsz256ColorsClass[] = "256_COLORS_CLASS";
char                gszScrollerWindowClass[] = "SCROLLER_WND_CLASS";
HANDLE              ghAccelMain; // Main Menu Accelerator Keys Handle
HDC                 ghdcMainWnd = NULL;
BOOL								gInitialized = FALSE;

//-----------------
// External controls..
//-----------------
EXTERNAL_IFACE_CONTROL		gExternalIface[3];

//-----------------
// Staus bar Window
//-----------------
HWND                ghwndStatus = NULL;

//----------------
// Sequence Window
//----------------
HWND                ghwndSeq = NULL;
HWND                ghwndSeqDlg = NULL;

HBITMAP             g_hbmpNumbers = NULL;
BITMAP              g_bmpNumbersInfo;

//-----------
// MDI Client
//-----------
HWND                ghwndMDIClient = NULL;
HBITMAP             ghbmpClientBk = NULL;
BITMAP              gbmpClientInfoBk;
WNDPROC             gorigMDIProc;


//-----------
// VU Display
//-----------
HBITMAP             ghbmpVUONHoriz = NULL;
HBITMAP             ghbmpVUONVert  = NULL;
BITMAP              gbmpVUONHoriz;
BITMAP              gbmpVUONVert;
int                 gVU_VertDispTable[4096];
int                 gVU_CompDispTable[4096];
int                 gVU_GateDispTable[4096];
HBITMAP             gbmpVUVertBuffer = NULL;
HBITMAP             gbmpVUHorzBuffer = NULL;

//-----------------------------------
// Global Zoom and Full Window Handle
//-----------------------------------
HWND                ghwndZoom = NULL;
HWND                ghwndFull = NULL;

//-------------
// Scroll stuff
//-------------
int                 g_iYSpeed = 0;
HWND                g_hwndScroller = NULL;

//-------
// Labels
//-------
LPSTR               gpLabels = NULL;
LPSTR               g_pAuxLabels = NULL;
LPSTR               g_pMatrixLabels = NULL;

//----------------------
// Drag and Drop Channel
//----------------------
DRAGDROPCHAN        gddcChanInfo = {0};

//-----------------------
// Full View Child Window
//-----------------------
char                gszFullViewClass[] = "FULL_VIEW_CLASS";
BOOL                gbFullImageWndClassReg = FALSE;

//-----------------------
// Zoom View Child Window
//-----------------------
char                gszZoomViewClass[] = "ZOOM_VIEW_CLASS";
int 								giMixerType;				// Mixer type, cabaret, event, showtime, etc.
int									giMixerTypeOffline=NULL;	// Same as above but only if OFFLINE, else will be -1

char								 gszStereoCueMetersClass[] = "STEREOCUEMETERSCLASS";

//-------------------------
// Master View Child Window
//-------------------------
char                gszMasterViewClass[] = "MASTER_VIEW_CLASS";
HWND                ghwndMaster = NULL;

//---------------------------------
// Memory map of the Mixer Controls
//---------------------------------
WORD                *gpwMemMapMixer = NULL;
WORD                *gpwMemMapBuffer = NULL;
int                 giMemMapSize = 0;

//-----------------------------------
// Read Out(RDOUT) resource variables
//-----------------------------------
//HINSTANCE           ghInstRdOutMaps = NULL; // Instance of the RdOut Maps DLL
//HGLOBAL             ghgRdOutTable = NULL;
LPRDOUTGLOBRES      gpRdOutTable = NULL;
int                 giRdOutGlobAllocated = 0;

//----------------------------------------
// Binary Resource Alloctaion and handling
//----------------------------------------
//HGLOBAL             ghgBinResTable = NULL;
LPBINARYTABLE       gpBinResTable = NULL;
int                 giBinResGlobAlloc = 0;

//----------------
// Zone Map Tables
//----------------
HGLOBAL             ghZoneMapID_Full = NULL; // Holds the Full Zonemap descriptors
LPZONE_MAP_ID       gpZoneMapID_Full = NULL;

HGLOBAL             ghZoneMapID_Zoom = NULL; // Holds the Zoom Zonemap descriptors
LPZONE_MAP_ID       gpZoneMapID_Zoom = NULL;

int                 giZoneMapID_Count_Zoom = 0;
int                 giZoneMapID_Count_Full = 0;

//long                glZonesCount_Full = 0;
//long                glZonesCount_Zoom = 0;

HGLOBAL             ghZoneMapDesc_Full = NULL;
LPZONE_MAP_DESC     gpZoneMapDesc_Full = NULL;

HGLOBAL             ghZoneMapDesc_Zoom = NULL;
LPZONE_MAP_DESC     gpZoneMapDesc_Zoom = NULL;

LPZONE_MAP          gpZoneMaps_Full = NULL;

LPZONE_MAP          gpZoneMaps_Zoom = NULL;

//----------------
// Group Selection
//----------------
LPGROUP             gpGrpGroup = NULL;
LPGROUPSTORE        gpGroupStoreFaders = NULL;
LPGROUPSTORE        gpGroupStoreMutes = NULL;
LPGROUPSTORE        gpGroupStoreGeneral = NULL;

///////////////////////////////////////////////////////////////////////////
// Stereo groups are defined-stored here
// we should have separate buffers for the input, sub and master modules
// to gain speed and robustness.
///////////////////////////////////////////////////////////////////////////
STEREOGROUP         g_StereoGroups[120];

HANDLE									gDisplayEvent = NULL;

//--------
// Palette
//--------
HPALETTE            ghPalette = NULL;

//----------------------------------------------
// 256 Color Window .. it works just as a buffer
//----------------------------------------------
HWND                ghwnd256 = NULL;
HDC                 ghdc256 = NULL;

//------------------------------
// Global DC for Draws and stuff
//------------------------------
HDC                 ghdcMemory = NULL;
HDC                 g_hdcMemory = NULL;
HDC                 g_hdcBuffer = NULL;
HDC                 g_hdcTempBuffer = NULL;

HBITMAP             g_hbmpBuffer = NULL;

//-----------------------
// Global Bitmap Resource
//-----------------------
HGLOBAL             ghBMPTable = NULL;
LPBMPGLOBRES        gpBMPTable = NULL;
int                 giBmpGlobAllocated = 100;

//------------
// Preferences
//------------
PREFERENCES         gprfEdit={0}; // Null the Entire thing
PREFERENCES         gprfPref=
														{
														SAMMPLUS_ID,
														sizeof(PREFERENCES),
														SAMMPLUS_VERSION,
														-1,
														-1,
														-1,
														-1,
														{0},// MIDIINCAPS
														{0},// MIDIOUTCAPS
														{0},// MTCINCAPS
														{0},// MTCOUTCAPS
														0,
														0,
														0,
														0,
														0,
														1,
														{0},// set the entire array to 0
														{0},// set the entire array to 0
														{0},// set the entire array to 0
														{0},// set the entire array to 0
														0,
														0,
														0,
														0,
														0,
														100,
														{0}// set the entire array of Reserved to 0
														};


HBITMAP             ghbmpPrefBkgnd = NULL;
BOOL								g_mixer_state_changed = FALSE;
BOOL								g_monitor_mix_file_changes = FALSE;

//===========================
// MultiMedia Midi Variables
//===========================
MIDIDEV             gMidiDev = {0};
MIDIDEV             gMTCDev  = {0};
//HMIDIOUT            ghMidiOut = 0;
//HMIDIIN             ghMidiIn = 0;
//MIDIDATATRANSFER    gMidiDataTransfer = {0};
MMTIME              g_mtcNow = {0};
MMTIME              g_mtcLast = {0};
MMTIME              gsmpteTimeDest = {0};
WORD                gwSMPTELastFormat=0;
WORD                gwSMPTETracking = 0;
WORD                gwMidiInStarted = 0;
BYTE                gbarMIDIBuff[MIDI_BUFFER_SIZE];
WORD                gwarMIDIData[MIDI_BUFFER_SIZE];

//===========================
//SMPTE Window
//===========================
HWND                ghwndSMPTE = NULL;
DLGPROC             glpDlgProcSMPTE = NULL;

//=============
// Debug Window
//=============
HWND                ghwndDebug;
DLGPROC             gpDebugProc;
WORD                gwDebugBuff[8];
int                 giDebugIndx;

