//====================================================
// Copyright 1998 CorTek Software, Inc.
//====================================================

//=================================================
//
//Spooler functions for the Double linked list
//
//=================================================

#define RETURN1IFNULL(ptr) if(ptr==NULL){ return 1;}
#define RETURN1IFNULL2(ptr1, ptr2)\
                           if((ptr1==NULL)||(ptr2==NULL)){ return 1;}

// return ret if any of the pointers == NULL
#define RETURN_N_IFNULL2(ptr1, ptr2, ret)\
                           if((ptr1==NULL)||(ptr2==NULL)){ return ret;}
#define RETURN_N_IFNULL(ptr1, ret)\
                           if(ptr1==NULL){ return ret;}


#define GLOBALALLOC(hg, lp, flags, size)\
                                (\
                                lp=NULL;\
                                if((hg=GlobalAlloc(flags, size)!=NULL)\
                                    lp = GlobalLock(hg);\
                                )
#define GLOBALFREE(hg)(GlobalUnlock(hg);GlobalFree(hg))

// Get an offset of a member of a structure
// the member has an address higher or
// equal to the struc variable address
//-----------------------------------------
#define GETOFFSET_MSTRUC(struc_v, struc_m ) ((long)((LPSTR)&struc_m - (LPSTR)&struc_v))
// this one does the same thnig but for pointers
#define GETOFFSET_MSTRUC_PTR(struc_v_ptr, struc_m_ptr)((long)((LPSTR)struc_m_ptr - (LPSTR)struc_v_ptr))


// Function Prototypes for the Double Linked List
//===============================================

// *** EXPORTED FUNCTIONS 'BEGIN' ***
__declspec(dllexport)LPDLROOTPTR             InitDoubleLinkedList(long, long, BOOL, BOOL, HANDLE, LPSTR);
__declspec(dllexport)void                    FreeDLListRootAll(LPDLROOTPTR *);
__declspec(dllexport)LPDLROOTPTR             LockDLListRoot(LPDLROOTPTR);
__declspec(dllexport)BOOL                    UnlockDLListRoot(LPDLROOTPTR ,LPDLROOTPTR);
__declspec(dllexport)long                    AddEntry(LPDLROOTPTR ,LPSTR ,long);
__declspec(dllexport)long                    InsertEntry(LPDLROOTPTR , long , LPSTR ,long ,long);
__declspec(dllexport)int                     DelEntry(LPDLROOTPTR, long);
__declspec(dllexport)int                     DelEntryPtr(LPDLROOTPTR , long);
__declspec(dllexport)LPDLROOTPTR             CompactDLList(LPDLROOTPTR);
__declspec(dllexport)long                    GetEntryLinkState(LPDLROOTPTR ,long);
__declspec(dllexport)long                    GetFirstEntry(LPDLROOTPTR);
__declspec(dllexport)long                    GetLastEntry(LPDLROOTPTR);
__declspec(dllexport)long                    GetNextEntry(LPDLROOTPTR, long);
__declspec(dllexport)long                    GetPrevEntry(LPDLROOTPTR, long);
__declspec(dllexport)LPVOID                  GetEntryData(LPDLROOTPTR, long);
// *** EXPORTED FUNCTIONS 'END' ***


// *** PRIVATE FUNCTIONS ***

int                         InitFromFile(LPDLROOT ,HANDLE ,LPSTR);
int                         InitIDX(LPDLROOTPTR);
int                         InitIDXDL_List(LPDLROOT , LPFILEMAP);
int                         AddIndexPage(LPDLROOTPTR);
void                        FreeDLListRoot(LPDLROOT);
void                        FreeIDX(LPDLROOT);
int                         IQS_GetSystemInfo(SYSTEM_INFO *);
LPSTR                       IsDataMapped(LPFILEMAP ,long ,long , 
                                         LPFILEMAP ,HANDLE);

// File Mapping Functions
//-----------------------
LPSTR                       IQS_MapViewOfFile(HANDLE ,DWORD ,DWORD ,LPFILEMAP);
int                         IQS_UnmapViewOfFile(LPFILEMAP);

int                         GrowDLFileMap(LPDLROOTPTR , long);


void                        ShowLastError(void);



