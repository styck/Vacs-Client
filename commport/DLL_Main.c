
#include <windows.h>

#define DLL_EXPORT_COMM

#include "commport.h"
/******************************************************************************\
*
*  FUNCTION:    DllMain
*
*  INPUTS:      hDLL       - handle of DLL
*               dwReason   - indicates why DLL called
*               lpReserved - reserved
*
*  RETURNS:     TRUE (always, in this example.)
*
*               Note that the retuRn value is used only when
*               dwReason = DLL_PROCESS_ATTACH.
*
*               Normally the function would return TRUE if DLL initial-
*               ization succeeded, or FALSE it it failed.
*
*  GLOBAL VARS: ghMod - handle of DLL (initialized when PROCESS_ATTACHes)
*
*  COMMENTS:    The function will display a dialog box informing user of
*               each notification message & the name of the attaching/
*               detaching process/thread. For more information see
*               "DllMain" in the Win32 API reference.
*
\******************************************************************************/
BOOL WINAPI DllMain(HANDLE hDLL, DWORD dwReason, LPVOID lpReserved)
{

    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
        if (m_cm.m_hDevice == NULL)
            SetupGlobal();
        break;
    }

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        CloseComm();
        break;
    }

    return TRUE;
}

