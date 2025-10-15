#ifndef DLTYPES_H_INCLUDED
#define DLTYPES_H_INCLUDED


#include <windows.h>

// FileMap pointer structure
//--------------------------
typedef struct FILEMAPtag {
    long        lMapStart;
    long        lMapStartOffset;
    long        lMapSize;
    LPSTR       pBase;
    LPSTR       pData;
    DWORD       dwReserved[3];
} FILEMAP;
typedef FILEMAP * LPFILEMAP;

// Double linked list entry
//-------------------
typedef struct DLENTRYtag {
    long        lPagePtr;
    long        lNext;
    long        lPrev;
    long        lParent;
    long        lChild;
    long        lDataPtr;
    long        lDataSize;
    long        lFlags;
} DLENTRY;
typedef DLENTRY * LPDLENTRY;

// The Index header
//-----------------
typedef struct IDXtag {
    long            lType;
    DWORD           dwVersion;
    long            lPageSize;
    long            lFirstPage;
    DWORD           dwReserved[4];
} IDX;
typedef IDX * LPIDX;

// Double Linked List root
//------------------------
typedef struct  DLROOTtag {
    DWORD           dwID;
    DWORD           dwSize;
    DWORD           dwVersion;
    SYSTEM_INFO     si;
    BOOL            bGrow;
    HANDLE          hfile;
    BOOL            bCloseFileHandle;
    HANDLE          hfilemap;
    DWORD           dwSizeMapped;
    long            lIndexInUse;
    IDX             pIDX[16];
    long            lIDXnum;
    long            lItemsPerBuffer;
    long            lItemMaxSize;
    long            lDataBuffSize;
    FILEMAP         fmapRoot;
    FILEMAP         fmapData;
    FILEMAP         fmapIdx;
    FILEMAP         fmapFree;
    FILEMAP         fmapGrow;
    long            lFirstItem;
    long            lLastItem;
    long            lFirstItemFree;
    long            lEntryCount;
    long            lDelCount;
    long            lBytesLost;
    char            szFileName[512];
    DWORD           dwReserved[32];
} DLROOT;
typedef DLROOT  *LPDLROOT;

// the Global Root Ptr
//----------------------------
typedef struct DLROOTPTRtag {
    LPDLROOT            pRoot;
    BOOL                bSyncEnabled;
    DWORD               dwReserved;
    CRITICAL_SECTION    csRoot;
} DLROOTPTR;
typedef DLROOTPTR * LPDLROOTPTR;

#endif // DLTYPES_H_INCLUDED
