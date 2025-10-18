//=================================================
// Copyright 1998 - 2002, CorTek Softawre, Inc.
//=================================================
//
//
// $Author: Styck $
// $Archive: /Vacs Client/inc/SAMM.h $
// $Revision: 44 $
//

#include <windows.h>
#include <commctrl.h>
#include <stdlib.h>
#include <ctype.h>
#include <mmsystem.h>
#include <string.h>

//FDS 6/13/04 #define SCROLLBARS		// FOR TOUCHSCREEN OPERATION WE NEED SCROLL BARS
// #define AUTOSAVE		// 3 MINUTE POP UP BOX TO SAVE MIX


// FLAGS FOR DEBUG SESSION
//------------------------
//#define DONOT_CLIP_CURSOR // this will shutdown the clipping of the cursor when over control


// Some internal Messages
//
//#define WM_GET_SERVER_DATA      WM_USER + 100


// Some of the defines gave to be here
// so they can be used in the other headers
//-----------------------------------------
#define     NUMBER_MIDI_HDRS        16 // Yes .. Yes only 16 of them ;-)
#define     MAX_SMPTE_MESSAGES      8
#define     MIDI_IN_MAX_ADDED_HDRS  6

#define     MAX_CHANNELS            80 // maximum number of channels per mixer
#define     MAX_AUX_CHANNELS        16
// Midi buffer Size
//-----------------
#define     MIDI_BUFFER_SIZE        255

// Label size
//-----------
#define     HEIGHT_FULL_LABEL_WND   30 // The height of the Label Window in the Full view
#define     MAX_LABEL_SIZE          20 // the length in characters

// Group name
//-----------
#define     MAX_GROUPNAME           40

// General String definitions
//---------------------------
#define     MAX_STRING_SIZE         255

// RdOut Text Length
//------------------
#define     RDOUT_TEXT_LENGTH       16

// definitions for different label-group windows
//----------------------------------------------
#define     WND_GROUPLBL_FULL       1
#define     WND_GROUPLBL_ZOOM       2
#define     MAX_GROUPS_COUNT        128
enum
{
    NULL_GRP_TYPE = 0,
    FADERS_GROUPS,
    MUTES_GROUPS,
    GENERAL_GROUPS
} GROUP_TYPES;


enum FILTER_STATE
{
    NO_FILTER = 0,
    YES_FILTER
} FFILTER;

enum
{
    PROP_ALL = 0,
    PROP_NONE,
    PROP_CUSTOM
}PROPAGATE_OPTS;


// Max Mixer Window Opened
//------------------------
#define     MAX_MIXER_WINDOW_OPENED       100

// Mixer Window Modes
//-------------------
#define     MW_NOTHING_MODE               0
#define     MW_DRAGDROP_MODE              0xf001
#define     MW_CONTROL_ACTIVE             0xf002
#define     MW_SCROLL                     0xf003
#define     MW_SCROLL_RELATIVE            0xf004


// Direction flags
//----------------
#define     DIRECTIONS_NONE               0
#define     DIRECTIONS_BACK               1
#define     DIRECTIONS_FORWARD            2
#define     DIRECTIONS_ALL                3

// Drivers offsets                        
//----------------                        
#define     MIDI_IN_DEV                   0
#define     MIDI_OUT_DEV                  1
#define     MTC_IN_DEV                    2
#define     MTC_OUT_DEV                   3


#define     DCX_DEVMAP_MODULE_NA            0
#define     DCX_DEVMAP_MODULE_INPUT         1
#define     DCX_DEVMAP_MODULE_AUX           2
#define     DCX_DEVMAP_MODULE_MATRIX        3
#define     DCX_DEVMAP_MODULE_CUE           4
#define     DCX_DEVMAP_MODULE_MASTER        5


#define     EQ_ID_NULL                    0x00000000
#define     EQ_ID_LOCUT                   0x00000001
#define     EQ_ID_LOSHELF                 0x00000002
#define     EQ_ID_HIGHSHELF               0x00000003
#define     EQ_ID_BP                      0x00000004

// Module General type definitions
//
#define     NULL_MODULE_TYPE        0xffffffff

#define     MAX_MATRIX_COUNT              8

/////////////////////////////////////////////////////
#include "DLLIST.h" // the Double linked list header
/////////////////////////////////////////////////////
#include "CTRL.h"
#include "SAMMSTRUC.h"// the Structure definitions
#include "SAMM_RC.h"  // the resource definitions
#include "lang.h"
//#include "SAMMRES00.h"// the additional resource defs from the DLL
#include "ConsoleDefinitionRC.h"
//=======================
// MDI defines
//=======================
#define WINDOWMENU                                     4
#define IDM_WINDOWCHILD                                2001

//======================
// General defines
//======================
#define     MSG_QUES_SIZE       42
#define     SAMMPLUS_ID         0x43434343 // just for fun if you wish
#define     SAMMPLUS_VERSION    0x00010001 // the Version numver

#define     LEFT      -1      // Scroll direction for zoom view
#define     RIGHT     1

#define			FIRST_INPUT		10
#define			FIRST_AUX			11

// File defines                     
//-------------
#define     MIXER_WINDOW_FILE_ID    0x10000001 // the ID for the Mixer Window Data Chunck
#define     GROUP_WINDOW_FILE_ID    0x10000002
#define     SEQUENCE_WINDOW_FILE_ID 0x10000010 // the ID for the Sequnce Window Data Chunck
#define     MASTER_WINDOW_FILE_ID   0x10000011 // is the master window visible... and perhaps some other data

#define     SEQUENCE_DATA_FILE_ID   0x20000001 // the ID for the Sequence Data Chunck
#define     INDEX_DATA_FILE_ID      0x30000001 // the ID for the Index Data Chunck

#define     GROUPS_FILE_ID          0x90000001 // the Groups get stored here ...
#define     LABELS_FILE_ID          0xa0000001 // the labels
#define     LABELS_AUX_FILE_ID      0xa0000002 // the labels
#define     LABELS_MATRIX_FILE_ID   0xa0000003 // the labels Matrix

#define     DEV_CONTROLS_IMAGE_FILE_ID  0xa00a0000 // the Memory image of the Device data

#define     MIDI_BUFF_HDR           256
#define     MIDI_IN_BUFF_SIZE       8192 // 32 * MIDI_BUFF_HDR
#define     MIDI_OUT_BUFF_SIZE      8192 // 32 * MIDI_BUFF_HDR

//======================
// Procedure Prototypes
//======================

// External Procedures
//--------------------

// MIDI.DLL
#ifdef __cplusplus
extern "C" {
#endif
void CALLBACK Dll_MidiInProc(HMIDIIN, UINT, DWORD, DWORD, DWORD);
void CALLBACK Dll_MidiOutProc(HMIDIOUT, UINT, DWORD, DWORD, DWORD);
void Dll_SetMainWindow(HWND);
HWND Dll_GetMainWindow(void);
#ifdef __cplusplus
}
#endif


// Module 256Colors
//-----------------
int Init256Colors(void);
void CreateResPalette(LPSTR);
BOOL UpdatePalette(BOOL, HDC);
HBITMAP Load256Bitmap(HINSTANCE, HDC, LPSTR);
int NColors(int);
LRESULT CALLBACK ColorsWndProc(HWND, UINT, WPARAM, LPARAM);



// Module 1
//---------
LRESULT CALLBACK WndMainProc(HWND, UINT, WPARAM, LPARAM);
int ConfirmationBox(HWND, HINSTANCE, int);
int InformationBox(HWND, HINSTANCE, int);
int ErrorBox(HWND, HINSTANCE, int);
void InformationStatus(HINSTANCE, int);
void GetMaxWindowSize(LPRECT, LPZONE_MAP, long, LPMIXERWNDDATA, BOOL);
void AddToCaption(HWND, LPSTR);
void ClipCtrlZone(HWND, LPMIXERWNDDATA);
void ClipZone(HWND, LPRECT);

LPSTR FindCharPosition(LPSTR, int);
void SetMemory(LPSTR, int, WORD);
void SetWordMemory(LPSTR, int, WORD);
void IQSCopyMemory(LPSTR, LPSTR, WORD);
void CopyWordMemory(LPSTR, LPSTR, WORD);
void OpenMemoryBuffer(LPSTR, LPSTR, WORD);
void CloseMemoryBuffer(LPSTR, LPSTR, WORD);
WORD GetWordValue(LPSTR);
DWORD GetDWordValue(LPSTR);
void SetWordValue(LPSTR, WORD);
void SetDWordValue(LPSTR, DWORD);

void WriteTextToDC(HDC, LPRECT, int, int, COLORREF, LPSTR);
void WriteTextLinesToDC(HDC, LPRECT, int, int, COLORREF, LPSTR);
void WriteTextLinesToDCVerticaly(HDC, LPRECT, int, int, COLORREF, LPSTR);

int  AllocLabelsBuff(void);
void FreeLabelsBuff(void);
void ClearLabelsMemory(void);

void IQS_MoveWindow(HWND, int, int);

void GenerateLL(void);

void ShowDebugInfo(BOOL, int, WORD);
BOOL CALLBACK DebugProc(HWND, UINT, WPARAM, LPARAM);


// Progress bar controls ..
//-------------------------
void  SetProgressRange(int iLow, int iHigh);
void  SetProgressStep(int iStep);
void  SetProgressPosition(int iPos);
void  ClearProgress(void);
void  StepProgress(int iStep);

// About
//------
BOOL CALLBACK   AboutProc(HWND, UINT, WPARAM, LPARAM);

//  data file and directory routines 
//----------------------------------
BOOL CreateApplicationDirectories(void);
BOOL OpenDataFile(LPSTR  lpstrFName);
BOOL CloseDataFile(void);
DWORD GetWriteOffset(void);
BOOL ReadDataFile(DWORD    dwReadOffset);
BOOL UpdateDataFile(DWORD    dwReadOffset, long itemPosition);
BOOL AddToDataFile(DWORD* pdw);


// Filters ... MUTE and perhaps some others
//
void StartControlDataFilter(int, LPMIXERWNDDATA, LPCTRLZONEMAP, BOOL, BOOL);
void SendDataToDevice(CONTROLDATA* ctrlData, BOOL bUseGroups, LPCTRLZONEMAP pctrlzm, int iDelta, LPMIXERWNDDATA lpmwd, BOOL bExternal);
void CheckForToggleSwitches(LPMIXERWNDDATA lpmwd, LPCTRLZONEMAP pctrlzm);
int CheckFilter(LPCTRLZONEMAP pctrlzm);
BOOL IsMuteFilter(LPCTRLZONEMAP pctrlzm);
int IsSpecialFilter(LPMIXERWNDDATA lpmwd, int iPhisChan, LPCTRLZONEMAP pctrlzm);
void PrepareCueMasterSystem(void);// this is not a filter function. However it sete the stage for
// the Master Cue Filter to work correctly!

// External Interface support
void InitExternalIface();
void UpdateExternalInterface(CONTROLDATA* pCtrlData);
void UpdateFromExternalInterface();


// Network updates ..
//
void  UpdateControlFromNetwork(WORD, WORD, int, BOOL);

// Sequence Window
//----------------
int RegisterSeqWindowClass(void);
int ShowSeqWindow(BOOL bShow);
BOOL CALLBACK SeqProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK SeqWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL FillSeqTree(HWND);
BOOL InitSeqList(HWND);
void						HandleRemoteSequenceControl(WORD wControl);

//HTREEITEM  AddItemToTree(HWND , LPSTR , int);
//BOOL InitTreeViewItems(HWND , LPSTR);
BOOL InitTreeViewImageLists(HWND);

int HandleSeqTVNExpand(LPNM_TREEVIEW);
int HandleSeqTVNBeginDrag(LPNM_TREEVIEW);
void HandleSeqTVNStopDrag(void);
void HandleSeqTVN_OnMouseMove(HWND, LONG, LONG);
void HandleTVGetDispInfo(TV_DISPINFO*);
void HandleTVSetItemIfno(TV_DISPINFO*);
void HandleTVEndLabelEdit(TV_DISPINFO*);


void StartDragTimer(int);
void StopDragTimer(void);
void HandleDragTimer(void);


int GetSeqUpdateProps(SEQENTRY* pSeqentry);


void PrepareMTCReadout(void);
void FreeMTCReadout(void);
void DisplayMTCReadout(void);

// Mixer related functions .. wide
//
int Init_MixerData(void); // INIT
int GetDeviceChannelCount(void);


// Misc Dlg Functions
//-------------------
long GetDlgWindowMaxX(HWND, int);
long GetDlgWindowMaxY(HWND, int);
long GetDlgWindowMinX(HWND, int);
long GetDlgWindowMinY(HWND, int);


// Mix Files
//----------
int  OpenMixFile(void);
int  SaveMixFile(void);
int  UpdateMixFile(void);
int  LoadMixFile(LPFILESTRUCT, BOOL);
int  WriteMixFile(LPFILESTRUCT, BOOL);
// added by Francis 
int WriteFkeyFile(LPFILESTRUCT, int);
int LoadFkeyFile(LPFILESTRUCT, int);
//
int  WriteMixerWndDataToFile(HWND, HANDLE);
int  WriteSequenceWndDataToFile(HWND, HANDLE);
int  ReadMixerWndDataFromFile(HANDLE, LPFILESECTIONHEADER);
int  ReadSequenceWndDataFromFile(HANDLE, LPFILESECTIONHEADER, BOOL);


// Module 2
//---------
int InitializeProc(void);
void ShutdownProc(void);
int GetGlobalMixerPhisDesc(HINSTANCE);


// Module 3
//---------
int CreateMainWindow(void);
int Create256Window(void);
void Close256Window(void);

// Status bar Window
//------------------
int	CreateMainStatusWindow(HWND);


// Toolbar window
//---------------
int CreateToolBars(HWND);
void ShowTBSeqName(LPSTR pName);			// Show Active Sequence Name
void ShowTBNextSeqName(LPSTR pName);	// Show Next Sequence Name



// Module 4
//---------
int             CreateMDIClient(HWND);
void            SizeClientWindow(HWND, UINT, WPARAM, LPARAM);
void            ClientWindowPaint(void);
void            ShutDownMDIClient(void);
LRESULT CALLBACK  MDIClientProc(HWND, UINT, WPARAM, LPARAM);
void            CloseAllMDI(void);
void            RefreshAllMDIWindows(void);
void            RefreshAllLblWindows(void);
LPMIXERWNDDATA  GetValidMixerWindowData(void);


// Module 5
//---------
int RegisterFullViewClass(void);
HWND CreateFullViewWindow(LPSTR, LPMIXERWNDDATA);
LRESULT CALLBACK  FullViewProc(HWND, UINT, WPARAM, LPARAM);
int CreateFullViewImageWindow(HWND, LPARAM);
LRESULT CALLBACK  FullViewImgProc(HWND, UINT, WPARAM, LPARAM);
int RegisterScrollWindowClass(void);
LRESULT CALLBACK  ScrollerWndProc(HWND, UINT, WPARAM, LPARAM);
int ShowScrollWindow(HWND, LPMIXERWNDDATA);
void DrawImgWindow(LPPAINTSTRUCT, LPMIXERWNDDATA);
void DrawControls(HDC, int, int, LPMIXERWNDDATA);
void ScrollImgWindow(HWND, LPMIXERWNDDATA, int);
void ScrollImgWindowRelative(HWND, LPMIXERWNDDATA);
void ScrollSideWays(HWND hwnd, LPMIXERWNDDATA lpmwd, int iDir);
void HandleScrollImgWindowRelative(HWND, LPMIXERWNDDATA);
LPMIXERWNDDATA  MixerWindowDataAlloc(WORD, LPZONE_MAP, long, int);
void MixerWindowDataFree(LPMIXERWNDDATA);
BOOL ImageWindowSize(HWND, LPRECT, LPMIXERWNDDATA);
int AddMWEntryToDLList(HWND, LPMIXERWNDDATA);
void DeleteMWEntryFromDLList(LPMIXERWNDDATA);


int RegisterTrackingWindowClass(void);
int CreateTrackingWindow(HWND, LPARAM);
void UpdateTrackingWindow(LPMIXERWNDDATA);
void StopTrackingWindow(void);
void ActivateTrackingWindow(void);
BOOL IsTrackingActive(void);


// Zoom View functions
//--------------------
int RegisterZoomViewClass(void);
HWND CreateZoomViewWindow(HWND, LPSTR, LPMIXERWNDDATA, int);
//HWND OpenZoomViewWindow(LPMIXERWNDDATA , LPSTR);
LRESULT CALLBACK  ZoomViewProc(HWND, UINT, WPARAM, LPARAM);


// Stere Cue Meters View functions
int RegisterStereoCueMetersViewClass(void);
LRESULT CALLBACK  StereoCueMetersViewProc(HWND hWnd, UINT wMessage,WPARAM wParam, LPARAM lParam);
int showStereoCueMetersView(void);



// Master View functions
//
#define   MASTER_MODULE_WIDTH   113
#define   MAX_BORDER_WIDTH      4

int RegisterMasterViewClass(void);
HWND CreateMasterViewWindow(LPSTR, LPMIXERWNDDATA);
void DestroyMasterViewWindow(HWND);



// Misc Resource functions
//------------------------
int AddBinResource(HINSTANCE, int, int);
void DeleteBinResGlobalIndx(int);
void DeleteBinResGlobalResID(int);
int InitBinResGlobal(void);
void FreeBinResGlobal(void);

int  __stdcall AddRdOutResGlobal(HINSTANCE, int);
void __stdcall DeleteRdOutResGlobalIndx(int);
void __stdcall DeleteRdOutResGlobalResID(int);
int  __stdcall InitRdOutResGlobal(void);
void __stdcall FreeRdOutResGlobal(void);

int AddBmpResGlobal(HINSTANCE, int, HDC);
void DeleteBmpResGlobalIndx(int);
void DeleteBmpResGlobalResID(int);
int InitBMPResGlobal(void);
void FreeBMPResGlobal(void);


// Module 8
// Load Zone Maps
//---------------
int LoadZoneMaps(void);
int InitZoneMap(LPCTRLZONEMAPRESOURCE, LPZONE_MAP);
void FreeZoneMapsMemory(void);
int LoadZoneMapIDs(void);
void FreeZoneMapIDs(void);


// Module 9
//---------
BOOL GetSourceFileName(HWND, char far*, char far*, BOOL);
BOOL GetDestinationFileName(HWND, char far*, char far*, BOOL);

// Preferences
//------------
BOOL APIENTRY   dlgPRFGeneral(HWND, UINT, UINT, LONG);
BOOL APIENTRY   dlgPRFMidi(HWND, UINT, UINT, LONG);
BOOL APIENTRY   dlgPRFMTC(HWND, UINT, UINT, LONG);
BOOL APIENTRY   dlgPRFActions(HWND, UINT, UINT, LONG);
BOOL APIENTRY   dlgPRFMixerChanPort(HWND, UINT, UINT, LONG);
BOOL APIENTRY   dlgPRFViews(HWND, UINT, UINT, LONG);

long CreatePropertySheet_PRF(HWND);
void SavePreferences(void);
void LoadPreferences(void);
void DoPreferences(void);
BOOL ApplyPreferences(BOOL);

// MIDI
//-----
#ifdef MIDI_SUPPORT
void CheckMidiStatus(void);
int  PrepareMidiDataTransferBuffers(LPMIDIDEV);
void UnPrepareMidiDataTransferBuffers(LPMIDIDEV);
void CheckMidiInData(void);
int  FlushMIDIBuff(LPMIDIDEV);
void TranslateSendOutMIDI(LPMIDIDEV, LPMIDITOSCRTABLE, int,int, int, int, BOOL);
int             printMIDI(LPSTR, int, LPWORD, LPWORD, int);

BOOL OpenMIDIInDev(int, LPMIDIDEV);
BOOL CloseMIDIInDev(LPMIDIDEV);
BOOL CloseAllMIDIInDev(void);
BOOL OpenMIDIOutDev(int, LPMIDIDEV);
BOOL CloseMIDIOutDev(LPMIDIDEV);
BOOL CloseAllMIDIOutDev(void);

int ListMidiInDev(HWND);
int ListMidiOutDev(HWND);
#endif

// MISC General
//-------------
void    VerifySMPTE(MMTIME*);


// ConsoleCommunication
//---------------------
void DefinitionCallback(UINT, UINT, VOID*, VOID*);
void DisplayVU_Data(VU_READ* pVuData, int iSzie);
void InitVULookupTables(BOOL);
void RequestVisibleVU(LPMIXERWNDDATA, int, int);




// Module 12
//----------
void CreateSMPTEWindow(void);
void DestroySMPTEWindow(void);
void UpdateSMPTEWnd(void);
BOOL DlgSMPTEProc(HWND, WORD, WORD, LONG);

// Module "Events Interface.c"
//----------------------------
void HandleMouseMove(HWND, POINTS, WPARAM, LPMIXERWNDDATA);
void HandleLBDown(HWND, POINTS, WPARAM, LPMIXERWNDDATA);
void HandleMBDown(HWND, POINTS, WPARAM, LPMIXERWNDDATA);
void HandleRBDown(HWND, POINTS, WPARAM, LPMIXERWNDDATA);
void HandleLBUp(HWND, POINTS, WPARAM, LPMIXERWNDDATA);
void HandleMBUp(HWND, POINTS, WPARAM, LPMIXERWNDDATA);
void HandleRBUp(HWND, POINTS, WPARAM, LPMIXERWNDDATA);
void HandleWndSize(HWND, LPMIXERWNDDATA, WORD, WORD, WPARAM);
void HandleCtrlTimer(HWND, LPMIXERWNDDATA); // timer for the controls
void ShowMousePos(HWND, LPMIXERWNDDATA);
void HandleFaderMoveVert(HWND, POINT, WPARAM, LPMIXERWNDDATA);
void HandleFaderMoveHorz(HWND, POINT, WPARAM, LPMIXERWNDDATA);
void HandleCtrlBtnClick(HWND hwnd, LPMIXERWNDDATA lpmwd);
void UpdateControlsByCtrlNum(HDC, HDC, LPMIXERWNDDATA, int, int, LPCTRLZONEMAP, int, int, BOOL);
void UpdateSameMixWndByCtrlNum(HWND, int, int, LPCTRLZONEMAP, int, HDC);

// Group Window
//
int ShowGroupWindow(BOOL bShow);
int RegisterGroupWindowClass(void);

int InitGroups(void);
void DeInitGroups(void);
BOOL ActivateGroup(int iGroup, int iType, int iForcedGroup);
void DeactivateGroup(void);
BOOL IsGrouped(int iChannel);
int AddGroup(LPGROUPSTORE pGrp, int iType);
int DeleteGroup(int iListItem, int iType, int iGroup);
BOOL GroupChannel(int iChannel, int iControl);
BOOL UnGroupChannel(int iChannel);
void UpdateGroupedControls(CONTROLDATA* pCtrlData, LPCTRLZONEMAP pctrlzm,
int iDelta, LPMIXERWNDDATA lpmwd, BOOL	skipSendingData);
void UpdateStereoControls(CONTROLDATA* pCtrlData, LPCTRLZONEMAP pctrlzm,
int iDelta, LPMIXERWNDDATA lpmwd);
void UpdateGroupedMutes(LPCTRLZONEMAP pctrlzm, LPMIXERWNDDATA lpmwd);
BOOL CalculatePhisChannelFromScreen(int* piRet, LPMIXERWNDDATA lpmwd);

// Module 14
//----------
int RegLblGroupWnd(void);
HWND CreateLblGroupWnd(LPRECT, HWND, LPMIXERWNDDATA);
LRESULT CALLBACK LblGroupProc(HWND, UINT, WPARAM, LPARAM);
void DrawLbl(HDC, LPMIXERWNDDATA);

// Module 16
//----------
int InitRemapTable(LPMIXERWNDDATA);
int RemapScrChannel(LPMIXERWNDDATA, int, int);
int RemapPhisFromScr(LPMIXERWNDDATA);
int GetMWScrChanNum(LPMIXERWNDDATA);
int GetMWScrChanRect(LPMIXERWNDDATA, int, RECT*);
int ActivateMWMode(HWND, LPMIXERWNDDATA);
int StopMWMode(HWND, LPMIXERWNDDATA);
BOOL MakeModulesArraySameType(LPMIXERWNDDATA, int, int);

// Module 17
//----------
int InitMemoryMap(void);
void FreeMemoryMap(void);
int InitPhysicalModuleMap(void);
int SetMemoryMapDefaults(void);
int PhisDataToScrPos(LPCTRLZONEMAP, int, int);
int ScrPosToPhisData(LPCTRLZONEMAP, int, int);

void RecallMemoryMapBuffer(BOOL, DWORD);

// Module 18
//----------
extern int gVU_VertDispTable[4096];
extern int gVU_CompDispTable[4096];
extern int gVU_GateDispTable[4096];

void __stdcall  DrawVertFader(HDC, LPCTRLZONEMAP, int, LPMIXERWNDDATA, int);
void __stdcall  DrawHorizFader(HDC, LPCTRLZONEMAP, int, LPMIXERWNDDATA, int);
void __stdcall  DrawVUData(HDC, LPCTRLZONEMAP, VU_READ*, LPMIXERWNDDATA, int, int);
void __stdcall  PushBtn(HDC, LPCTRLZONEMAP, int, LPMIXERWNDDATA, int);
void __stdcall  OpenExplodeWindow(HDC, LPCTRLZONEMAP, int, LPMIXERWNDDATA, int);
void __stdcall  JumpToMZWindow(HDC, LPCTRLZONEMAP, int, LPMIXERWNDDATA, int);
void __stdcall  RdOutText(HDC, LPCTRLZONEMAP, int, LPMIXERWNDDATA, int);
void __stdcall  UpDownControl(HDC, LPCTRLZONEMAP, int, LPMIXERWNDDATA, int);
void __stdcall  LeftRightControl(HDC, LPCTRLZONEMAP, int, LPMIXERWNDDATA, int);
void __stdcall  DrawChannelLabel(HDC, LPCTRLZONEMAP, int, LPMIXERWNDDATA, int);
void __stdcall  DrawEqGraphHook(HDC, LPCTRLZONEMAP, int, LPMIXERWNDDATA, int);
void __stdcall  ChannelNameTextVertical(HDC, LPCTRLZONEMAP, int, LPMIXERWNDDATA, int);
void __stdcall  ChannelNumberTextVertical(HDC, LPCTRLZONEMAP, int, LPMIXERWNDDATA, int);

void EditModuleLabel(HWND, LPMIXERWNDDATA, BOOL);

// Module 19
//----------
LPCTRLZONEMAP ScanCtrlZonesPnt(LPCTRLZONEMAP, POINT);
LPCTRLZONEMAP ScanCtrlZonesType(LPCTRLZONEMAP, int);
LPCTRLZONEMAP ScanCtrlZonesNum(LPCTRLZONEMAP, int);
LPCTRLZONEMAP ScanCtrlZonesDisp(LPCTRLZONEMAP, int);
LPCTRLZONEMAP ScanCtrlZonesAbs(LPCTRLZONEMAP, int);

// Module 21
//----------
void            TransSendMidiData(LPCTRLZONEMAP, int, int);

// Module Sac00.asm
//-----------------
long            DivMulInt(int, int, int);
