//=================================================
// Copyright 1998 CorTek Software, Inc.
//=================================================

#include "consoledefinition.h"

// Application General
//--------------------
extern char                gszProgDir[256];
extern HINSTANCE           ghInstConsoleDef;
extern HINSTANCE           ghInstStrRes;
extern HANDLE              ghMainMenu;
extern int                 giActiveMixers;

extern HFONT               g_hConsoleFont;

extern UINT                g_CtrlTimer; // the controls timer(UP_DOWN) control and such

// File Open
//----------
extern FILESTRUCT          gfsMix;
extern FILESTRUCT          gfsTemp;
extern char                gstrDefExt[128];
extern char                gstrFFilter[256];
extern MIXFILEHEADER       gmfhMix;
extern MIXFILEHEADER       gmfhTemp;

// Mixer
//------
extern WORD                gwActiveMixer;
extern int                 giMixerWndOpened;
extern LPDLROOTPTR         gpMixerWindows_DL_List; // the root for the Mixer windows double linked list

// Global mixer settings
//----------------------
extern int                 giMax_MIXERS;
extern int                 giMax_CHANNELS; // per phisical mixer
extern int                 giMax_CONTROLS; // per phisical channel
extern char                gsz_MIXERNAME[40];
extern DEVICE_SETUP_DATA   gDeviceSetup;
extern int								 gCueActiveCount;	

// Main Window
//------------
extern HINSTANCE           ghInstMain;
extern HWND                ghwndMain;
extern HWND								 ghwndTBPlay;
extern HWND								 ghwndTBGeneral;
//extern char                gszMainMenu[];
extern char                gszMainWndClass[];
extern char                gszMainWndTitle[];
extern char                gszFullViewImageClass[];
extern char                gszLblGroupClass[];
extern char                gszSeqClass[];
extern char                gszGroupClass[];
extern char                gsz256ColorsClass[];
extern char                gszScrollerWindowClass[];
extern HANDLE              ghAccelMain; // Main Menu Accelerator Keys Handle
extern HANDLE              ghMainMenu; // The Main Menu Handler
extern HDC                 ghdcMainWnd;
extern BOOL								 gInitialized;
// External controls..
//
extern EXTERNAL_IFACE_CONTROL		gExternalIface[];


extern	HANDLE									gDisplayEvent;
// Staus bar Window
//-----------------
extern HWND                ghwndStatus;

// Sequence Window
//----------------
extern HWND                 ghwndSeq;
extern HWND                 ghwndSeqDlg;

extern HBITMAP              g_hbmpNumbers;
extern BITMAP               g_bmpNumbersInfo;


extern SEQ_PROPAGATE        g_SeqPropagate;
// MDI Client
//-----------
extern HWND                ghwndMDIClient;
extern HBITMAP             ghbmpClientBk;
extern BITMAP              gbmpClientInfoBk;
extern WNDPROC             gorigMDIProc;

// VU Display
//-----------
extern HBITMAP             ghbmpVUONHoriz;
extern HBITMAP             ghbmpVUONVert;
extern BITMAP              gbmpVUONHoriz;
extern BITMAP              gbmpVUONVert;
extern int                 gVU_VertDispTable[];
extern int                 gVU_CompDispTable[];
extern int                 gVU_GateDispTable[];


extern HBITMAP             gbmpVUVertBuffer;
extern HBITMAP             gbmpVUHorzBuffer;

// Global Zoom and Full Window Handle
//-----------------------------------
extern HWND                ghwndZoom;
extern HWND                ghwndFull;

// Scroll stuff
//-------------
extern int                 g_iYSpeed;
extern HWND                g_hwndScroller;

// Labels
//-------
extern LPSTR               gpLabels;
extern LPSTR               g_pAuxLabels;
extern LPSTR               g_pMatrixLabels;

// Drag and Drop Channel
//----------------------
extern DRAGDROPCHAN        gddcChanInfo;

// Full View Child Window
//-----------------------
extern char                gszFullViewClass[];
extern BOOL                gbFullImageWndClassReg;

// Zoom View Child Window
//-----------------------
extern char                gszZoomViewClass[];
extern char								giMixerType;				// Mixer type, cabaret, event, showtime, etc.

extern char								 gszStereoCueMetersClass[];

// Master View Child Window
//-------------------------
extern char                gszMasterViewClass[];
extern HWND                ghwndMaster;

// Memory map of the Mixer Controls
//---------------------------------
extern WORD                *gpwMemMapMixer;
extern WORD                *gpwMemMapBuffer;
extern WORD								 *gpwMemMapUpdateBuffer;
extern WORD								 *gpwMemMapUpdateBufferMask;
extern int                  giMemMapSize;
// Read Out(RDOUT) resource variables
//-----------------------------------
extern LPRDOUTGLOBRES      gpRdOutTable;
extern int                 giRdOutGlobAllocated;

// Binary Resource Alloctaion and handling
//----------------------------------------
//extern HGLOBAL             ghgBinResTable;
extern LPBINARYTABLE       gpBinResTable;
extern int                 giBinResGlobAlloc;

// Zone Map Tables
//----------------
extern HGLOBAL             ghZoneMapID_Full; // Holds the Full Zonemap descriptors
extern LPZONE_MAP_ID       gpZoneMapID_Full;

extern HGLOBAL             ghZoneMapID_Zoom; // Holds the Zoom Zonemap descriptors
extern LPZONE_MAP_ID       gpZoneMapID_Zoom;

extern int                 giZoneMapID_Count_Zoom;
extern int                 giZoneMapID_Count_Full;

//extern long                glZonesCount_Full;
//extern long                glZonesCount_Zoom;

extern HGLOBAL             ghZoneMapDesc_Full;
extern LPZONE_MAP_DESC     gpZoneMapDesc_Full;

extern HGLOBAL             ghZoneMapDesc_Zoom;
extern LPZONE_MAP_DESC     gpZoneMapDesc_Zoom;

extern ZONE_MAP            *gpZoneMaps_Full;

extern ZONE_MAP            *gpZoneMaps_Zoom;

// Group Selection
//----------------
//extern HGLOBAL             ghGroup;
extern LPGROUP             gpGrpGroup;
//extern HGLOBAL             ghGroupStore;
extern LPGROUPSTORE        gpGroupStoreFaders;
extern LPGROUPSTORE        gpGroupStoreMutes;
extern LPGROUPSTORE        gpGroupStoreGeneral;

extern STEREOGROUP         g_StereoGroups[120];

// Palette
//--------
extern HPALETTE            ghPalette;

// 256 Color Window .. it works just as a buffer
//----------------------------------------------
extern HWND                ghwnd256;
extern HDC                 ghdc256;

// Global DC for Draws and stuff
//------------------------------
extern HDC                 ghdcMemory;
extern HDC                 g_hdcMemory;
extern HDC                 g_hdcBuffer;
extern HDC                 g_hdcTempBuffer;

extern HBITMAP             g_hbmpBuffer;

// Global Bitmap Resource
//-----------------------
extern HGLOBAL             ghBMPTable;
extern LPBMPGLOBRES        gpBMPTable;
extern int                 giBmpGlobAllocated;

// Preferneces
//------------
extern PREFERENCES         gprfEdit; // Null the Entire thing
extern PREFERENCES         gprfPref;
extern HBITMAP             ghbmpPrefBkgnd;
extern int                 giPrefCurDlg;
extern BOOL								 g_mixer_state_changed;
extern BOOL						     g_monitor_mix_file_changes;

//--------------------------
// MultiMedia Midi Variables
//--------------------------
extern MIDIDEV             gMidiDev;
extern MIDIDEV             gMTCDev;

extern MMTIME              g_mtcNow;
extern MMTIME              g_mtcLast;
extern MMTIME              gsmpteTimeDest;
extern WORD                gwSMPTELastFormat;
extern WORD                gwSMPTETracking;
extern WORD                gwMidiInStarted;
extern BYTE                gbarMIDIBuff[MIDI_BUFFER_SIZE];
extern WORD                gwarMIDIData[MIDI_BUFFER_SIZE];

//===========================
//SMPTE Window
//===========================
extern HWND                ghwndSMPTE;
extern DLGPROC             glpDlgProcSMPTE;

//=============
// Debug Window
//=============
extern HWND                ghwndDebug;
extern DLGPROC             gpDebugProc;
extern WORD                gwDebugBuff[8];
extern int                 giDebugIndx;



