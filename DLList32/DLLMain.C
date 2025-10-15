//=====================================================
// CopyRight 1998 CorTek Software, Inc.
//=====================================================


#include <windows.h>

// function definition
//--------------------
int                         IQS_GetSystemInfo(SYSTEM_INFO *);


// Some of the defines gave to be here
// so they can be used in the other headers
//-----------------------------------------
extern HINSTANCE               hInst;
extern long                    lCounter;
extern SYSTEM_INFO             g_sinf;



//-----------------------------------------------------------------------
//
//  FUNCTION:    DllMain
//
//  INPUTS:      hDLL       - handle of DLL
//               dwReason   - indicates why DLL called
//               lpReserved - reserved
//
//  RETURNS:     TRUE (always, in this example.)
//
//               Note that the return value is used only when
//               dwReason = DLL_PROCESS_ATTACH.
//
//               Normally the function would return TRUE if DLL initial-
//               ization succeeded, or FALSE it it failed.
//
//  GLOBAL VARS: ghMod - handle of DLL (initialized when PROCESS_ATTACHes)
//
//------------------------------------------------------------------------
BOOL WINAPI DllMain (HANDLE hDLL, DWORD dwReason, LPVOID lpReserved)
{

switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        if(g_sinf.dwPageSize == 0)
            if(IQS_GetSystemInfo(&g_sinf))
                return FALSE;
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        break;
    }

return TRUE;
}

//=====================================================
//FUNCTION: ShowLastError
//
//
//
//
//=====================================================
void        ShowLastError(void)
{
LPVOID      lpMsgBuf;

FormatMessage( 
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
    NULL,
    GetLastError(),
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR) &lpMsgBuf,
    0,
    NULL 
);
// Display the string.
MessageBoxA( NULL, (LPCSTR)lpMsgBuf, "GetLastError", MB_OK | MB_ICONINFORMATION );
// Free the buffer.
LocalFree( lpMsgBuf );

return;
}
//=============================================================
//FUNCTION: IQS_GetSystemInfo
//
//
//return    0 = OK;
//          n = Error;
//
//=============================================================
int     IQS_GetSystemInfo(SYSTEM_INFO *psinf)
{

GetSystemInfo(psinf);
switch(psinf->dwAllocationGranularity)
    {
    //------------------------
    case    0x10000:
        psinf->dwAllocationGranularity = 16;
        break;
    //------------------------
    case    0x8000:
        psinf->dwAllocationGranularity = 15;
        break;
    //------------------------
    case    0x4000:
        psinf->dwAllocationGranularity = 14;
        break;
    //------------------------
    case    0x2000:
        psinf->dwAllocationGranularity = 13;
        break;
    //------------------------
    case    0x1000:
        psinf->dwAllocationGranularity = 12;
        break;
    //------------------------
    case    0x800:
        psinf->dwAllocationGranularity = 11;
        break;
    //------------------------
    case    0x400:
        psinf->dwAllocationGranularity = 10;
        break;
    //------------------------
    case    0x200:
        psinf->dwAllocationGranularity = 9;
        break;
    //------------------------
    case    0x20000:
        psinf->dwAllocationGranularity = 17;
        break;

    default:
        return 1; // ERROR
        break;
    }

return 0;
}
