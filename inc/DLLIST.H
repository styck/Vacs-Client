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

// important ?!
#ifdef DLLIST_MAIN

#define IQS_DLLIST_ID           mmioFOURCC('d', 'l', 'l', 's')
#define IQS_DLLIST_VERSION      MAKELONG(1,0)
#define DEFAULT_IDX_PAGE_SIZE   8192 // Bytes

// the maximum number of Indexes
//------------------------------
#define MAX_INDEXES             16

// File IDs for the Indexes
//-------------------------
#define IDX_FILE_ID             0x00000001

// Type ID's for the Indexes
//--------------------------
#define IDX_DLLIST_TYPE         0x10000001


// These are BIT flags ... so don't mess up the values
//----------------------------------------------------
#define DL_USED             0x0000001
#define DL_DELETED          0x0000002
#define DL_OLD_DATA         0X0000004

#define DWORD_MAX           0xFFFFFFFF // this will be our NULL(terminator)
#define LONG_MAX            0x80000000 // maximum long value

// FileMap pointer structure
//--------------------------
typedef struct FILEMAPtag
    {
    long        lMapStart;// the requested position for file mapping
    long        lMapStartOffset; // File Start Offset from where the mapping actualy starts
    long        lMapSize; // size of the Mapped View
    LPSTR       pBase; // = MapViewOfFile(...)
    LPSTR       pData;
    DWORD       dwReserved[3];
    }FILEMAP; // 32 Bytes ...
typedef FILEMAP * LPFILEMAP;

// Double linked list
// for File access
//-------------------
typedef struct DLENTRYtag
    {
    long        lPagePtr; // the data size
    long        lNext; // File Position(the same as pointers in the File)
    long        lPrev; // File Position(the same as pointers in the File)
    long        lParent;  // File Position(the same as pointers in the File)
    long        lChild;  // File Position(the same as pointers in the File)
    long        lDataPtr;
    long        lDataSize;
    long        lFlags; // the current state of the Entry ...
    } DLENTRY;// -> 32 bytes in size for the Entry header
typedef DLENTRY * LPDLENTRY;

// The Index header
//-----------------
typedef struct IDXtag
    {
    long            lType; // the Type of the Index
    DWORD           dwVersion;
    long            lPageSize;
    long            lFirstPage;
    DWORD           dwReserved[4];
    }IDX;
typedef IDX * LPIDX;


// Double Linked List root
//------------------------
typedef struct  DLROOTtag
    {
    DWORD           dwID; // an id string(value)
    DWORD           dwSize;
    DWORD           dwVersion;

    SYSTEM_INFO     si;// the System Info

    BOOL            bGrow;
    HANDLE          hfile; // The file handle
    BOOL            bCloseFileHandle; // Close the File handle on exit
    HANDLE          hfilemap;// Handle to the File Map
    DWORD           dwSizeMapped;

    long            lIndexInUse; // the Currently Used Index
    IDX             pIDX[MAX_INDEXES]; // Buffer for the Index pointer headers
    long            lIDXnum; // Number of Indexes
    long            lItemsPerBuffer; // Used for paging
    long            lItemMaxSize;   // Used for Paging

    long            lDataBuffSize;

    FILEMAP         fmapRoot; // the Root map of the File
    FILEMAP         fmapData; // The Current DataBuffer
    FILEMAP         fmapIdx; // the currently active Index uses this
    FILEMAP         fmapFree;

    //long            lStartFilePos; // The Start offset within the file where the
                                   // Root data is stored 
    FILEMAP         fmapGrow; // Information about the growing of the file
                              // it should always be alligned on Memory Granularity

    // the actual file positions 
    // of these items
    //--------------------------
    long            lFirstItem;
    long            lLastItem;
    long            lFirstItemFree;

    long            lEntryCount;
    long            lDelCount;
    long            lBytesLost;
        
    char            szFileName[512]; // long file names
    DWORD           dwReserved[32]; // just to have it
    }DLROOT; // the size should be 1024
typedef DLROOT  *LPDLROOT;

// the Global Root Ptr
// used for accessing the DATA
//----------------------------
typedef struct DLROOTPTRtag
    {
    LPDLROOT            pRoot;
    BOOL                bSyncEnabled;
    DWORD               dwReserved;
    CRITICAL_SECTION    csRoot; // syncronization
//    CRITICAL_SECTION    csItem; // syncronization
    }DLROOTPTR;
typedef DLROOTPTR * LPDLROOTPTR;

#else

// define them like this so 
// we can use the names
// in the rest of the modules
// ... we will interface
// these the functions with these
// pointers
//-------------------------------
//typedef LPSTR   LPDLROOT;
typedef LPSTR   LPDLROOTPTR;
//typedef LPSTR   LPDLENTRY;

// Function Prototypes for the Double Linked List
//===============================================
__declspec(dllimport)LPDLROOTPTR             InitDoubleLinkedList(long, long, BOOL, BOOL, HANDLE, LPSTR);
__declspec(dllimport)void                    FreeDLListRootAll(LPDLROOTPTR *);
__declspec(dllimport)LPDLROOTPTR             LockDLListRoot(LPDLROOTPTR);
__declspec(dllimport)BOOL                    UnlockDLListRoot(LPDLROOTPTR ,LPDLROOTPTR);
__declspec(dllimport)long                    AddEntry(LPDLROOTPTR ,LPSTR ,long);
__declspec(dllimport)long                    InsertEntry(LPDLROOTPTR , long , LPSTR ,long ,long);
__declspec(dllimport)int                     DelEntry(LPDLROOTPTR, long);
__declspec(dllimport)int                     DelEntryPtr(LPDLROOTPTR , long);
__declspec(dllimport)LPDLROOTPTR             CompactDLList(LPDLROOTPTR);
__declspec(dllimport)long                    GetEntryLinkState(LPDLROOTPTR ,long);
__declspec(dllimport)long                    GetFirstEntry(LPDLROOTPTR);
__declspec(dllimport)long                    GetLastEntry(LPDLROOTPTR);
__declspec(dllimport)long                    GetNextEntry(LPDLROOTPTR, long);
__declspec(dllimport)long                    GetPrevEntry(LPDLROOTPTR, long);
__declspec(dllimport)LPVOID                  GetEntryData(LPDLROOTPTR, long);

#endif

