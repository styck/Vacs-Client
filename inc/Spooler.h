
//====================================================
// Copyright 1998 CorTek Software, Inc.
//====================================================

#ifndef SPOOLER_H_INCLUDED
#define SPOOLER_H_INCLUDED

#include "DLTYPES.H"
// ...existing code...


#define RETURN1IFNULL(ptr) if(ptr==NULL){ return 1;}
#define RETURN1IFNULL2(ptr1, ptr2) \
    if((ptr1==NULL)||(ptr2==NULL)){ return 1;}
#define RETURN_N_IFNULL2(ptr1, ptr2, ret) \
    if((ptr1==NULL)||(ptr2==NULL)){ return ret;}
#define RETURN_N_IFNULL(ptr1, ret) \
    if(ptr1==NULL){ return ret;}
#define GLOBALALLOC(hg, lp, flags, size) \
    (lp=NULL; if((hg=GlobalAlloc(flags, size))!=NULL) lp = GlobalLock(hg); )
#define GLOBALFREE(hg) (GlobalUnlock(hg);GlobalFree(hg))
#define GETOFFSET_MSTRUC(struc_v, struc_m ) ((long)((LPSTR)&struc_m - (LPSTR)&struc_v))
#define GETOFFSET_MSTRUC_PTR(struc_v_ptr, struc_m_ptr) ((long)((LPSTR)struc_m_ptr - (LPSTR)struc_v_ptr))

// Only keep private function prototypes here
int                         InitFromFile(LPDLROOT ,HANDLE ,LPSTR);
int                         InitIDX(LPDLROOTPTR);
int                         InitIDXDL_List(LPDLROOT , LPFILEMAP);
int                         AddIndexPage(LPDLROOTPTR);
void                        FreeDLListRoot(LPDLROOT);
void                        FreeIDX(LPDLROOT);
int                         IQS_GetSystemInfo(SYSTEM_INFO *);
LPSTR                       IsDataMapped(LPFILEMAP ,long ,long , LPFILEMAP ,HANDLE);
LPSTR                       IQS_MapViewOfFile(HANDLE ,DWORD ,DWORD ,LPFILEMAP);
int                         IQS_UnmapViewOfFile(LPFILEMAP);
int                         GrowDLFileMap(LPDLROOTPTR , long);
void                        ShowLastError(void);

#endif // SPOOLER_H_INCLUDED
// the member has an address higher or

// equal to the struc variable address

//-----------------------------------------

