///////////////////////////////////////////////////////////////
// file CDef_Implement.h
//
// the Implementation header


HINSTANCE       g_hinstCDef = NULL;
int		          g_iLastError = 0;


CDEF_PREFERENCES     g_pref = { 0 };
// Thread to handle the IO stuff
HANDLE          g_threadIO = NULL;
HWND            g_hwndIO = NULL;
DWORD           g_threadID = 0;
char            g_szIOWndClass[] = "ConsoleDefinition_IO_";

BOOL            g_bServerSetupData = FALSE;
BOOL            g_bServerTableData = FALSE;

// Server data Window
//
BOOL            g_bTimerLockOut = FALSE;

// Special TCP stuff       
SOCKET          g_socket = INVALID_SOCKET;
WSADATA         g_WSAData;
char            g_szNetAddr[MAX_INET_ADDR];
UINT            g_uiNetAddr;
short           g_portno;
DWORD           g_dwBReceived;
DWORD           g_dwBSent;
  
HDR_DCXTCP      g_hdrDCXTCP = { 0 };

DWORD           g_dwSentBytes = 0;

// the Application suplied variables
HWND                g_hwndApp = NULL;
HINSTANCE           g_hinstApp = NULL;
LP_XCOMM_MAIN_proto g_xcomm_main = NULL;

DCX_MEM_MAP     g_dcxMemMap = { 0 };

