//===========================================================
// file ConsoleDefinition.h
//
// this is a general include file to be used by other modules
// to communicate with the ConsoleDefinition DLL
//
// Copyright 1998 CorTek Software, Inc.
//===========================================================
#ifndef CONSOLEDEFINITION_HEADER_
#define CONSOLEDEFINITION_HEADER_

typedef void (__stdcall XCOMM_MAIN_proto)(UINT ,UINT , void *, void *);
typedef XCOMM_MAIN_proto * LP_XCOMM_MAIN_proto; // define a pointer to this function type



// Device Setup data
//
typedef struct DEVICE_SETUP_DATAtag
{
  char      szDeviceID[25];       // DeviceID ... should be filled by the DLL
  HWND      hwndMain;             // Main application Window
  HINSTANCE hinst;                // Application Instance
  LP_XCOMM_MAIN_proto   pfTXComm; // Callback function for the DLL to call directly into
                                  // the Applications address space
  int       iaChannelTypes[80];   // channel types passed from the server 
                                  // EQU to the Server setup data         
}DEVICE_SETUP_DATA;
typedef DEVICE_SETUP_DATA *LPDEVICE_SETUP_DATA;



#define  CDEF_SHOW_TIME     0x00000001L

#define  CDEF_SHOW_MESSAGE  0x10000000L

#define  CDEF_SHOW_ERROR    0xf0000000L






// BEGIN of export/import stuff
// the  _CONSOLE_DEFINITION_FILE_ should be defined only in the
// ConsoleDefinition.dll modules all other should be using the
// IMPORT SECTION of this file
#ifdef  _CONSOLE_DEFINITION_FILE_


// The Control Data(Complete)
//---------------------------
#ifndef CONTROLDATA
typedef struct
    {
    WORD        wMixer; // the mixer of the Control
    WORD        wChannel;  // the Channel of the control
    WORD        wCtrl;  // the control
    WORD        wVal;   // the value of the control
    }CONTROLDATA;
typedef CONTROLDATA  *   LPCONTROLDATA;
#endif

typedef struct
  {
  DWORD     dwID;
  DWORD     dwVersion;
  DWORD     dwSize;
  char      szTableFileName[MAX_PATH];
  char      szIP_Address[MAX_PATH];
  int       iPort;
  int       iReserved[32];
  } CDEF_PREFERENCES;

#define CDEF_PREFERENCES_ID           mmioFOURCC('C','D','E','F')
#define CONSOLE_DEFINITION_VERSION    0x00010001L

#define WSA_READ                      (WM_USER + 1025) // Incomming TCP message



extern HWND      ghwndCS_Data;

// Internal functions
BOOL APIENTRY   dlgConfig(HWND ,UINT , UINT , LONG );
BOOL APIENTRY   dlgTable(HWND ,UINT , UINT , LONG );
BOOL APIENTRY   dlgAbout(HWND ,UINT , UINT , LONG );

// io thread ... callback functions and other crap goes here
DWORD WINAPI      ioThreadProc(LPVOID lpv);
int               ioCreateWindow(void);
LRESULT CALLBACK  ioWndProc(HWND , UINT , WPARAM , LPARAM  );

BOOL APIENTRY   dlgCS_Display(HWND ,UINT , UINT , LONG );

// TCP functions
int     ProcessTcpData(void);
int     StartClient(HWND );


void    SendData(LPSTR , int );

int     FreeDCXMapBuffers(LPDCX_MEM_MAP);

int     LoadPreferences();
int     SavePreferences();


// Exported functions
__declspec(dllexport)int      CDef_isRunning(void);
__declspec(dllexport)int      CDef_Init(void);
__declspec(dllexport)BOOL     CDEF_GetCSData(HWND);

__declspec(dllexport)int			CDEF_ReadDCXBinFile (void);

__declspec(dllexport)int      CDef_ShutDown(void);
__declspec(dllexport)int      CDef_SetGlobalData(LPDEVICE_SETUP_DATA);
__declspec(dllexport)int      CDef_Version(void);
__declspec(dllexport)int      CDef_Preferences(HWND hwnd);

__declspec(dllexport)int      CDef_SendData(LPCONTROLDATA);
__declspec(dllexport)int      CDef_RequestServerSetupData(void);
__declspec(dllexport)int      CDef_RequestServerTableData(void);
__declspec(dllexport)int      CDef_StartVuData(void);
__declspec(dllexport)int      CDef_StopVuData(void);
__declspec(dllexport)int		  CDef_ShowVuData(char *);

__declspec(dllexport)int      ReadRawTcpData(SOCKET , int );

__declspec(dllexport)int      CDef_FindControlByName(LPSTR);
__declspec(dllexport)char     *CDef_GetControlName(int);

__declspec(dllexport)int      CDef_GetCtrlMaxVal(int);
__declspec(dllexport)int      CDef_GetCtrlMinVal(int);
__declspec(dllexport)int      CDef_GetCtrlReadout(int , int , LPSTR );
__declspec(dllexport)int      CDef_GetCtrlDefaultVal(int);


__declspec(dllexport)int      CDef_GetLastError(LPSTR , int );
__declspec(dllexport)void     CDef_ResetBus(void);

#else
///////////////////   IMPORT SECTION  ///////////////////////////////

// Imported functions from Other modules ... APP
__declspec(dllimport)int      CDef_isRunning(void);

__declspec(dllimport)int      CDef_Init(void);
__declspec(dllimport)BOOL     CDEF_GetCSData(HWND);

__declspec(dllimport)int			CDEF_ReadDCXBinFile (void);

__declspec(dllimport)int      CDef_ShutDown(void);
__declspec(dllimport)int      CDef_SetGlobalData(LPDEVICE_SETUP_DATA);
__declspec(dllimport)int      CDef_Version(void);
__declspec(dllimport)int      CDef_Preferences(HWND hwnd);

__declspec(dllimport)int      CDef_SendData(LPCONTROLDATA);
__declspec(dllimport)int      CDef_RequestServerSetupData(void);
__declspec(dllimport)int      CDef_RequestServerTableData(void);
__declspec(dllimport)int      CDef_StartVuData(void);
__declspec(dllimport)int      CDef_StopVuData(void);
__declspec(dllimport)int		  CDef_ShowVuData(char *);

__declspec(dllimport)int      CDef_FindControlByName(LPSTR);
__declspec(dllimport)char     *CDef_GetControlName(int);

__declspec(dllimport)int      CDef_GetCtrlMaxVal(int);
__declspec(dllimport)int      CDef_GetCtrlMinVal(int);
__declspec(dllimport)int      CDef_GetCtrlReadout(int , int , LPSTR );
__declspec(dllimport)int      CDef_GetCtrlDefaultVal(int);

__declspec(dllimport)int      CDef_GetLastError(LPSTR , int );
__declspec(dllimport)void     CDef_ResetBus(void);

#endif // end of export/import stuff



#endif // CONSOLEDEFINITION_HEADER_