//=================================================
// CopyRight 1998 CorTek Software, Inc.
//=================================================



// Structures
//-----------

// The Control Data(Complete)
//---------------------------
typedef struct
    {
    WORD        wMixer; // the mixer of the Control
    WORD        wChannel;  // the Channel of the control
    WORD        wCtrl;  // the control
    WORD        wVal;   // the value of the control
    }CONTROLDATA;
typedef CONTROLDATA  *   LPCONTROLDATA;


typedef struct
{
	float fbw;
  float ffc;
  float fgain;
  int   eqId;
  COLORREF color; 
}EQ_DISPLAY_DATA;
typedef EQ_DISPLAY_DATA * LPEQ_DISPLAY_DATA;

// Stereo Groups are defined in this, rather "complicated" structure :)
//
typedef struct
{
  WORD    wLeft;
  WORD    wRight;
  int     iModuleType;
}STEREOGROUP;
typedef STEREOGROUP * LPSTEREOGROUP;

// File Open Close Save ...
//-------------------------
typedef struct  FILESTRUCTtag
    {
    char        szFileName[512];  // including the Extension
    char        szFileDir[512];   // without the File name

    }FILESTRUCT;
typedef FILESTRUCT  *LPFILESTRUCT;


// Used to read the ReadOut Resources
//-----------------------------------
typedef struct RDOUTTABLEtag
    {
    SHORT           iMin;
    SHORT           iMax;
    SHORT           iDefault;
    SHORT           iTotVals;
    SHORT           iScaleInto;
    }RDOUTTABLE;
typedef RDOUTTABLE  * LPRDOUTTABLE;

// Used to manipulate the ReadOut Resources
//
//-----------------------------------------
typedef struct RDOUTGLOBREStag
    {
    int             iResID;
    HGLOBAL         hgRdOut;
    LPRDOUTTABLE    lpRdOut;
    LPSTR           lpStr;
    }RDOUTGLOBRES;
typedef RDOUTGLOBRES  * LPRDOUTGLOBRES;

// This is the Standard header for the Bniary Resources
// The Resource gets loaded into hgBinRes
// then a pointer to there is obtained in
// lpBinRes and through this pointer the
// resource data can be accessed.
//------------------------------------------------------
typedef struct BINARYTABLEtag
    {
    int             iResID;
    HGLOBAL         hgBinRes;
    LPSTR           lpBinRes;
    } BINARYTABLE;
typedef BINARYTABLE  * LPBINARYTABLE;

// Used for the MIDI to Screen Resources
// which are loaded as Binary resources
// and typecasted to this structure
//--------------------------------------
typedef struct MIDITOSCRTABLEtag
    {
    SHORT           iMin;
    SHORT           iMax;
    SHORT           iDefault;
    SHORT           iTotVals;
    SHORT           iScaleInto;
    SHORT           iCtrlPhisNum; // uesd in some cases
    WORD            warMIDIFormat[20];
    } MIDITOSCRTABLE;
typedef MIDITOSCRTABLE  * LPMIDITOSCRTABLE;

// Used for the Screen to MIDI Resources
// which are loaded as Binary resources
// and typecasted to this structure
//--------------------------------------
typedef struct SCRTOMIDITABLEtag
    {
    SHORT           iMin;
    SHORT           iMax;
    SHORT           iDefault;
    SHORT           iTotVals;
    SHORT           iScaleInto;
    } SCRTOMIDITABLE;
typedef SCRTOMIDITABLE  * LPSCRTOMIDITABLE;

// Used to read the Control Zone map Resources
//--------------------------------------------
typedef struct CTRLZONEMAPRESOURCEtag
    {
    SMALL_RECT      rZone;
    USHORT          uCtrlChanPos;
    USHORT          uCtrlType;
    USHORT          uDispType;
    char            chControlName[26];
    char            chControlNameAlt1[26];
    USHORT          uRdOutType; // type of readout it might be NULL so no readout
    USHORT          iCtrlBmp[3]; // three bitmaps max per control
    }CTRLZONEMAPRESOURCE;
typedef CTRLZONEMAPRESOURCE  * LPCTRLZONEMAPRESOURCE;

// This structure defines the individual Controls
// such as faders, buttons, ....
// with all of their atributes. Like
// the bitmaps that represent them, the
// Readout tables, the Remap tables(SCR to MIDI and
// MIDI to SCR), the Control Type, the Control number(used
// to find which value to pull/set in the global
// map of the mixer), the Control Zone, ....
//---------------------------------------------------------
typedef void (__stdcall CTRLFUNCPROTO)(HDC ,void  * , int, void *, int);
typedef struct CTRLZONEMAPtag
    {
    RECT            rZone;
    RECT            rLastDraw;
    int             iMinScrPos;
    int             iMaxScrPos;
    int             iNumScrPos; // total number of screen positions 
    int             iNumValues; // total number of different values for this control
    int             iCtrlChanPos; // the control position in Channel order
    int             iCtrlNumAbs; // an absolute index value for every control
    int             iCtrlNumAbsAlt1;// Aletrantive Control index value
    int             iModuleNumber; // this is the Physical module where this control is located.
    int             iCtrlType;
    int             iDispType;
    int             iCtrlBmp[3]; // three bitmaps max per control
    int             iFiltered;
    CTRLFUNCPROTO   *CtrlFunc;
    }CTRLZONEMAP;
typedef CTRLZONEMAP  *LPCTRLZONEMAP;

// Zone map ID resource
//---------------------
typedef struct ZONE_MAP_IDtag
    {
    WORD        w_ZM_ID;
    WORD        w_str_ID;
    }ZONE_MAP_ID;
typedef ZONE_MAP_ID     *LPZONE_MAP_ID;


// Binds a Set of ControlZoneMaps(LPCTRLZONEMAP)
// together into a module.
//----------------------------------------------
typedef struct ZONE_MAPtag
    {
    WORD                wID;       // the Zone ID
    LPCTRLZONEMAP       lpZoneMap; // Pointer to the Zone Buffer
    int                 iZonesCount;
    int                 iBmpIndx;  // The Bitmap index in the global resourse table
    char                chDefLabel[4];// the default label
    }ZONE_MAP;
typedef ZONE_MAP  *LPZONE_MAP;

// Used to read the Resource that describes
// the ZONE_MAP
//-----------------------------------------
typedef struct ZONE_MAP_DESCtag
    {
    WORD        wZMResID;
    WORD        wZMBmpID;
    char        sDefLabel[4];
    } ZONE_MAP_DESC;
typedef ZONE_MAP_DESC  *LPZONE_MAP_DESC;

// Phisical mixer description
//---------------------------
typedef struct MIXERDESCtag
    {
    SHORT           iID;
    SHORT           iMaxMixers;
    SHORT           iMaxChannels;
    SHORT           iMaxControls;
    char            szMixerName[40];
    }MIXERDESC;
typedef MIXERDESC  *LPMIXERDESC;


// Preferences Structure
//----------------------
typedef struct PREFERENCEStag
    {
    DWORD       dwID;
    DWORD       dwSize;
    DWORD       dwVersion;
    int         iMidiIn_DevId;
    int         iMidiOut_DevId;
    int         iMTCIn_DevId;
    int         iMTCOut_DevId;
    MIDIINCAPS  MidiInDev;
    MIDIOUTCAPS MidiOutDev;
    MIDIINCAPS  MidiMTCInDev;
    MIDIOUTCAPS MidiMTCOutDev;
    int         iFullViewSet;
    int         iZoomViewSet;
    int         iSaveStateOnExit;
    int         iAutoSaveMode;
    int         iTrackingWnd;
    int         iSequentualChans;
    BYTE        pbChanIn[16];// on which channel do we receive the data
    BYTE        pbPortIn[16];// on which port do we receive the data
    BYTE        pbChanOut[16];// on which channel do we send the data
    BYTE        pbPortOut[16];// on which port do we send the data
    int         iAutoExecute;
    int         iActOnFClick;
    int         iShowSeqViewOnGo;
    int         iEnableKbfNav;
    int         iSaveWindowPos;
    int         iSMPTEStopLatency;
    DWORD       dwReserved[50];
    } PREFERENCES;
typedef PREFERENCES  *LPPREFERENCES;



// SMPTE data format
//------------------
/*
typedef struct SMPTETIMEtag

    {
    union
        {
        DWORD       dwMTC;
        struct
        {
        BYTE        hour;
        BYTE        min;
        BYTE        sec;
        BYTE        frame;
        }
        }t;
    BYTE        sframe;
    BYTE        pad1;  // use this so the structure is paded on a
    BYTE        pad2;  // DWORD boundry
    BYTE        pad3;  //----------------------------------------
    } SMPTETIME;
typedef SMPTETIME  *LPSMPTETIME;
*/

// MIDI DATA TRANSFER
//-------------------
typedef struct MIDIDATATRANSFERtag
    {
    int         piaInHdr[NUMBER_MIDI_HDRS]; // the array will serve as a flag
    int         piaOutHdr[NUMBER_MIDI_HDRS]; //to show which headers need to be prepared allover
    int         iMidiInLongReceived;
    int         iMidiOutLongReceived;
    HGLOBAL     hmidihdrIn[NUMBER_MIDI_HDRS];
    HGLOBAL     hmidihdrOut[NUMBER_MIDI_HDRS];
    LPMIDIHDR   lpmidihdrIn[NUMBER_MIDI_HDRS];
    LPMIDIHDR   lpmidihdrOut[NUMBER_MIDI_HDRS];
    HGLOBAL     hHdrDataBuffIn[NUMBER_MIDI_HDRS];
    HGLOBAL     hHdrDataBuffOut[NUMBER_MIDI_HDRS];
    LPSTR       lpHdrDataBuffIn[NUMBER_MIDI_HDRS];
    LPSTR       lpHdrDataBuffOut[NUMBER_MIDI_HDRS];
    long        lError; // Set to value if an error occurs
    LPSTR       lpMidiInBuff;
    long        lInHeadPos;
    long        lInTailPos;
    long        lMidiInBuffSz;
    LPSTR       lpMidiOutBuff;
    long        lOutHeadPos;
    long        lOutTailPos;
    long        lMidiOutBuffSz;
    int         iNewTime; // Indicates if new time is available 0=no, 1=yes
    WORD        wSMPTEFormat;
    DWORD       dwSMPTETime;
    MMTIME      smpteTime;
    DWORD       dwLastFrameTick;
    int         iSMPTEMsgReceived;
    int         iSMPTEWorking;
    } MIDIDATATRANSFER;
typedef MIDIDATATRANSFER  *LPMIDIDATATRANSFER;

// MIDI devices
//-------------
typedef struct
    {
    int                 iInDevId;
    int                 iInOpen;
    int                 iInStarted;
    int                 iOutDevId;
    int                 iOutOpen;
    int                 iOutStarted;
    HMIDIOUT            hMidiOut; // handle for the Midi Out Device
    HMIDIIN             hMidiIn;  // handle for yhe Midi In Device
    MIDIDATATRANSFER    midiDT; // all the information we need to transfer
                                // the Midi data
    }MIDIDEV;
typedef MIDIDEV * LPMIDIDEV;

// General Mixer Window Component
//-------------------------------
typedef struct MW_GENERALCOMPONENTtag
    {
    HWND                    hwndComp;
    RECT                    rPos;
    HDC                     hdcMemory; // to store things
    HBITMAP                 hbmpMemory; // to hold an image in memory
    }MW_GENERALCOMPONENT;
typedef MW_GENERALCOMPONENT *LPMW_GENERALCOMPONENT;

// Sequence propagate options
//
typedef struct
{
  BOOL  bUseCurrent;
  int   iPropWhat; // All = 0; None = 1; Custom = 3
} SEQ_PROPAGATE;
typedef SEQ_PROPAGATE *LPSEQ_PROPAGATE;

// External interface control ....
//
//
//----------------------------------------
typedef struct
{
	int		ctrlId;
	int		range;
	int		chann;
	int		scale;
	float	fscale;

}EXTERNAL_IFACE_CONTROL;

// Mixer Window data
// contains information about every aspect
// needed to redraw the Mixer controls
// to keep them updated and active
//----------------------------------------
typedef struct MIXERWNDDATAtag
    {
    int                 iWndID;// Mixer, sequence, etc...
    char                szTitle[128];
    LPZONE_MAP          lpZoneMap;// Actual Real zone map
    LPZONE_MAP          lpZoneMapZoom;// Special zone map ... used only in the Full View mode
    LPCTRLZONEMAP       lpCtrlZM; // last accessed control
    WORD                lpwRemapToScr[MAX_CHANNELS];// the array is the Phisical order the values are the screen Position
    WORD                lpwRemapToPhis[MAX_CHANNELS];// the array is the Screen order the values are the phisical Position
    long                lZMCount;
    WORD                wWndType;    // holds the type of the Label Group Window
    HWND                hwndLblGroup;// handle to the Label-Group Window
    HWND                hwndImg;
    RECT                rWndPos;
    RECT                rVisible;
    RECT                rMaxSize;
    RECT                rMaxWndPos;
    BOOL                bLink;
    int                 iMixer;
    int                 iCurChan;
    int                 iLastChan;
    int                 iStartScrChan;
    int                 iEndScrChan;
    int                 iXOffset;
    int                 iYOffset;
    int                 iXadj; // the x adjustment within the Window.
                               // If the Cursor is on Channel #10 -> iXadj = CHANNELWIDTH * 9
    int                 iYadj; // for now it is not being used
    POINT               pntMouseCur;  // the current mouse position
    POINT               pntMouseLast; // the last mouse position
    int                 iCurMode;
    int                 iCtrlMode;
//    LPDLENTRYMEM        lpdle;
    WORD                wKeyFlags;
    BOOL                bUseGroups;
    long                lDLEntryPos;
    char                acVisibleVU[80];
    }MIXERWNDDATA;
typedef MIXERWNDDATA  *LPMIXERWNDDATA;


// data for the Mixer window that gets stored
// in the Double linked list
//===========================================
typedef struct MW_DL_LISTtag
    {
    HWND                hwndMW;
    LPMIXERWNDDATA      lpMWD; // the pointer to the Mixer window data
    } MW_DL_LIST;
typedef MW_DL_LIST  *LPMW_DL_LIST;

// Global storage for the Bitmap Resources
//----------------------------------------
typedef struct BMPGLOBREStag
    {
    HBITMAP     hbmp;
    int         iWidth;
    int         iHeight;
    int         iResID;
    }BMPGLOBRES;
typedef BMPGLOBRES  *LPBMPGLOBRES;

// the Current Group description
//------------------------------
typedef struct GROUPtag
    {
    int     iActive; // is this active
    int     iMixer;
    int     iChannel;
    int     iControl; // added so we can limit the scope of the controls grouped
    }GROUPS;
typedef GROUPS  *LPGROUP;

// the Groups Storage description
//-------------------------------
typedef struct  GROUPSTOREtag
    {
    long        Count; // how many channels are in this group
    char        szGroupName[MAX_STRING_SIZE];
    GROUPS      Group[MAX_CHANNELS];
    }GROUPSTORE;
typedef GROUPSTORE  *LPGROUPSTORE;

// Drag and Drop Channel
//----------------------
typedef struct DRAGDROPCHANtag
    {
    int     iChan;
    int     iMixer;
    BOOL    bInUse;
    }DRAGDROPCHAN;
typedef  DRAGDROPCHAN  *LPDRAGDROPCHAN;

// All the Sections in the Mix files
// are using this Section Header to 
// identify their data and type
//-----------------------------------
typedef struct FILESECTIONHEADERtag
    {
    DWORD       dwID;
    LONG        lSize;
    DWORD       dwVersion;
		DWORD				dwCurrentSeqSelection;	// The current selected sequence
    char        achReserved[16];
    }FILESECTIONHEADER;
typedef FILESECTIONHEADER * LPFILESECTIONHEADER;


typedef struct MIXFILEHEADERtag
    {
    DWORD       dwID;
    DWORD       dwVersion;
    DWORD       dwSize;
    DWORD       dwDevID;
    int         iNumDev;
    int         iScrCX;
    int         iScrCY;
    int         iReserved[32]; // just Reserved
    } MIXFILEHEADER;
typedef MIXFILEHEADER * LPMIXFILEHEADER;


typedef struct MIXERWINDOWFILEtag
    {
//    DWORD       dwID; // MIXER_WINDOW_FILE_ID
//    DWORD       dwSize; // the size of the Structure
//    FILESECTIONHEADER   fsh;

    char        szTitle[128];
    BOOL        bMaximized;// Maximized or Minimized
    BOOL        bLink;
    int         iMixer;
    int         iZoneMap_ID; // is it a FULL or ZOOM window
    WORD        lpwRemapToScr[MAX_CHANNELS];// the array is the Phisical order the values are the screen Position
    WORD        lpwRemapToPhis[MAX_CHANNELS];// the array is the Screen order the values are the phisical Position
    int         iStartScrChan;
    int         iEndScrChan;
    int         iXOffset;
    int         iYOffset;
    RECT        rWndPos;
    RECT        rVisible;
    }MIXERWINDOWFILE;
typedef MIXERWINDOWFILE * LPMIXERWINDOWFILE;

typedef struct SEQUENCEWINDOWFILEtag
    {
    int                 iWndID;// Mixer, sequence, etc...
    RECT                rWndPos;
    }SEQUENCEWINDOWFILE;
typedef SEQUENCEWINDOWFILE * LPSEQUENCEWINDOWFILE;

// Group Window header and information ..
//
typedef struct GROUPWINDOWFILEtag
    {
    int                 iWndID;// Mixer, sequence, etc...
    RECT                rWndPos;

    }GROUPWINDOWFILE;
typedef GROUPWINDOWFILE * LPGROUPWINDOWFILE;

// Te state of the Mixer ... the actual
// physical layout of the Controls
// in memory
//-------------------------------------
typedef struct MIXERSSTATEtag
    {
    DWORD       dwSize; // the size of the Structure
    DWORD       dwNumDev;
    DWORD       dwDataSize;     
    DWORD       dwReserved;
    }MIXERSSTATE;
typedef MIXERSSTATE * LPMIXERSSTATE;

// the sequence entry description
//-------------------------------
typedef struct SEQENTRYtag
    {
    DWORD       dwSize;
    DWORD       dwType;
    HANDLE      h;
    MMTIME      mmtIn;
    MMTIME      mmOut;
    DWORD       dwOffset;
    DWORD       dwReserved[3];
    char        szName[260];
    }SEQENTRY;
typedef  SEQENTRY * LPSEQENTRY;

//
//
typedef struct 
	{
	int		input;
	int		aux;
	int		sub;
	int		matrix;
	int		master;
	}CUE_PRIORITY;

// ????? Make this common for both the VACS client and the GServer
// ?????????????????????????????????????????????????????????????????
#pragma pack(1) // pack on a byte boundry


typedef struct
{
	WORD		wAddr;							// the VUthread Reads from here(module address)
//	BYTE	iVUType;					// the VUthread Reads from here(Pre, Post, Comp, Gate)
	char    cLock;							// if Zero nobody monitors this VU, so we don't need to read the damn thnig
	WORD    wVUValue[8];		    // data. VUthread writes here
	WORD    wPeakClipValue;			// data
  WORD    wModuleIdx;
}VU_READ;


#ifdef OLD
typedef struct
{
	int		iAddr;							// the VUthread Reads from here(module address)
	int 	iLock;							// if Zero nobody monitors this VU, so we don't need to read the damn thnig
  int   iVUValue[8];
  int		iPeakClipValue;			// data
  int   iModuleIdx;         // module Index into the Device setup array
}VU_READ;
#endif

#pragma pack() // default packing .... we need to keep this byte boundry



////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////
//
// Filter Structure Definitions ...

//
// Output Filter ... this filter
// effects only the output of a given Control(s)
//----------------------------------------------
typedef struct OUTPUTFILTERtag
  {
  WORD    wID; // filter ID ... MUTE_FILTER, SOLO_FILTER, others


  } OUTPUTFILTER;

