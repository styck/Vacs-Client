///////////////////////////////////////////////////////////////
// file CDef_Implement.h
//
// the Implementation header


#define WM_GET_SERVER_SETUP_DATA   WM_USER + 1
#define WM_GET_SERVER_TABLE_DATA   WM_USER + 2

extern  HINSTANCE       g_hinstCDef;
extern  int		          g_iLastError;

extern CDEF_PREFERENCES g_pref;
// Thread to handle the IO stuff
extern HANDLE           g_threadIO;
extern HWND             g_hwndIO;
extern DWORD            g_threadID;
extern char             g_szIOWndClass[];


extern BOOL            g_bServerSetupData;
extern BOOL            g_bServerTableData;

// Server data Window
//
extern BOOL            g_bTimerLockOut;

// Special TCP stuff
extern SOCKET           g_socket;
extern WSADATA          g_WSAData;
extern char             g_szNetAddr[];
extern UINT             g_uiNetAddr;
extern short            g_portno;
extern DWORD            g_dwBReceived;
extern DWORD            g_dwBSent;

extern HDR_DCXTCP       g_hdrDCXTCP;

extern DWORD            g_dwSentBytes;

// the Application suplied variables
extern  HWND                g_hwndApp;
extern  HINSTANCE           g_hinstApp;
extern  LP_XCOMM_MAIN_proto g_xcomm_main;

extern DCX_MEM_MAP          g_dcxMemMap;
extern LPDEVICE_SETUP_DATA  g_pDeviceSetupData;

// some of the dcx_parser module functions
int   ReadDCXTableFile(LPSTR  , LPDCX_MEM_MAP );
int   FreeDCXMapBuffers(LPDCX_MEM_MAP);
