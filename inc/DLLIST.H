#ifndef DLLIST_H_INCLUDED
#define DLLIST_H_INCLUDED

#include "DLTYPES.H"
//====================================================
// DLList Software Audio Console - Double linked List
// Header File
//
// Copyright 1998 CorTek Software, Inc.
//====================================================



// Relation definitions
//---------------------
#define DL_RELATION_CHILD       0x00000001
#define DL_RELATION_PEER        0x00000002

// Link state definitions
//-----------------------
#define DLENTRY_NEXT            0x00000001
#define DLENTRY_PREV            0x00000002
#define DLENTRY_CHILD           0x00000004
#define DLENTRY_PARENT          0x00000008

// Public constants/macros needed by both DLL and clients
#define IQS_DLLIST_ID           mmioFOURCC('d', 'l', 'l', 's')
#define IQS_DLLIST_VERSION      MAKELONG(1,0)
#define DEFAULT_IDX_PAGE_SIZE   8192 // Bytes
#define MAX_INDEXES             16
#define IDX_FILE_ID             0x00000001
#define IDX_DLLIST_TYPE         0x10000001
#define DL_USED                 0x0000001
#define DL_DELETED              0x0000002
#define DL_OLD_DATA             0X0000004
#define DWORD_MAX               0xFFFFFFFF // this will be our NULL(terminator)

// (All internal/private definitions have been removed from this public header.)
// DLL export/import macro
#ifdef _WIN32
#ifdef DLList32_EXPORTS
#define DLList32_API __declspec(dllexport)
#else
#define DLList32_API __declspec(dllimport)
#endif
#else
#define DLList32_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

DLList32_API LPDLROOTPTR InitDoubleLinkedList(long lEntryDataSize, long lEntriesNum, BOOL bSyncEnabled, BOOL bGrow, HANDLE hFile, LPSTR lpstrFName);
DLList32_API void        FreeDLListRootAll(LPDLROOTPTR *pRootPtr);
DLList32_API LPDLROOTPTR LockDLListRoot(LPDLROOTPTR pRootOrig);
DLList32_API BOOL        UnlockDLListRoot(LPDLROOTPTR pRootOrig, LPDLROOTPTR pRootLock);
DLList32_API long        AddEntry(LPDLROOTPTR lpdlrPtr, LPSTR lpstrData, long lDataSize);
DLList32_API long        InsertEntry(LPDLROOTPTR lpdlrPtr, long lAfter, LPSTR lpstrData, long lDataSize, long lFlags);
DLList32_API int         DelEntry(LPDLROOTPTR lpdlr, long lEntryNum);
DLList32_API int         DelEntryPtr(LPDLROOTPTR lpdlrPtr, long lItemPos);
DLList32_API LPDLROOTPTR CompactDLList(LPDLROOTPTR lpdlrPtr);
DLList32_API long        GetEntryLinkState(LPDLROOTPTR pRootPtrObj, long lCur);
DLList32_API long        GetFirstEntry(LPDLROOTPTR pRootPtrObj);
DLList32_API long        GetLastEntry(LPDLROOTPTR pRootPtrObj);
DLList32_API long        GetNextEntry(LPDLROOTPTR pRootPtrObj, long lCur);
DLList32_API long        GetPrevEntry(LPDLROOTPTR pRootPtrObj, long lCur);
DLList32_API LPVOID      GetEntryData(LPDLROOTPTR pRootPtrObj, long lEntryPos);

#ifdef __cplusplus
}
#endif
// ...existing code...

#endif // DLLIST_H_INCLUDED
