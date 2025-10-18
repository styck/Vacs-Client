//////////////////////////////////////////////////////////////////////
// DLList project.
// Copyright CorTek Software, Inc. 1997-1998. All rights reserved.
// Win32 API (windows 95, WindowsNT)
//

//=================================================
//
//Double linked list Functios
//
//=================================================

#include <windows.h>
#define  DLLIST_MAIN
#include "DLLIST.h"
#include "SPOOLER.h"

// Variables
//----------
HINSTANCE hInst = 0;
long lCounter = 0;
SYSTEM_INFO g_sinf = { 0 };

//========================================================
//FUNCTION: LockDLListRoot
//
//parameters:
//  LPDLROOTPTR     pRootOrig; the original Root pointer
//
//return:
//  LPDLROOTPTR     pRootLock; the new locked Root pointer
//
//purpose:
//  Locks the main Root and allocates the memory for the
// pLockLock
//========================================================
DLList32_API LPDLROOTPTR LockDLListRoot(LPDLROOTPTR pRootOrig)
{
    LPDLROOTPTR pRootLock = NULL;

    // Allocate memory
    //----------------
    pRootLock = (LPDLROOTPTR)GlobalAlloc(GPTR, sizeof(DLROOTPTR));
    if (pRootLock == NULL)
        return NULL;

    pRootLock->pRoot = pRootOrig->pRoot;
    pRootLock->bSyncEnabled = FALSE;

    // now try to enter the critical section
    //--------------------------------------
    if (pRootOrig->bSyncEnabled)
        EnterCriticalSection(&pRootOrig->csRoot);

    return pRootLock;
}

//========================================================
//FUNCTION: UnlockDLListRoot
//
//parameters:
//  LPDLROOTPTR     pRootOrig; the original Root pointer
//  LPDLROOTPTR     *pRootLock; the new locked Root pointer
//
//purpose:
//  Unlocks the main Root and frees the memory allocated
// for the pRootLock
//========================================================
DLList32_API BOOL UnlockDLListRoot(LPDLROOTPTR pRootOrig, LPDLROOTPTR pRootLock)
{
    // now Leave the critical section
    //-------------------------------
    if (pRootOrig->bSyncEnabled)
        LeaveCriticalSection(&pRootOrig->csRoot);

    GlobalFree(pRootLock);
    return TRUE;
}

//==================================
//FUNCTION:  InitDoubleLinkedList
//
//
//==================================
DLList32_API LPDLROOTPTR InitDoubleLinkedList(long   lEntryDataSize, long lEntriesNum,
    BOOL bSyncEnabled, BOOL bGrow, HANDLE hFile, LPSTR lpstrFName)
{
    LPDLROOT pdlRoot;
    LPDLROOTPTR pRootPtr;
    DWORD dwFileStartPos, dwFileSize;
    DWORD dwAddSize;
    HANDLE hFileMap;
    FILEMAP fmap;
    BOOL bCloseFileHandle;

    pdlRoot = NULL;

    // the caller application is responssible for
    // closing the File handle
    //-------------------------------------------
    bCloseFileHandle = FALSE;

    // make sure we can get a file handle
    //-----------------------------------
    if (hFile == NULL)
        if (lpstrFName != NULL)
        {
            // try to create the file
            //-----------------------
            hFile = CreateFile(lpstrFName, GENERIC_WRITE | GENERIC_READ,
                0, NULL, OPEN_ALWAYS,
                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                NULL);
            // indicates that the file handle is to be closed
            // when the ROOT is released
            //------------------------------------------------
            bCloseFileHandle = TRUE;
        }
        else
            return NULL;

    // Check the File again
    //---------------------
    if (hFile == NULL)
        return NULL;

    // Obtain the Current File Pointer
    // and other important Information
    //--------------------------------
    dwFileStartPos = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
    dwFileSize = GetFileSize(hFile, NULL);
    if (dwFileSize == DWORD_MAX)
    {
        ShowLastError();
        return NULL;
    }

    // Do we have to Initialize the 
    // File From Scratch
    //-----------------------------
    if (dwFileStartPos == dwFileSize)
    {
        // Calc the Index Page Size
        //-------------------------
        dwAddSize = sizeof(DLENTRY) * lEntriesNum;

        // Add the Data Buffer Size
        //-------------------------
        dwAddSize += (lEntryDataSize * lEntriesNum);

        // Add the Root Structure
        //-----------------------
        dwAddSize += sizeof(DLROOT);

        dwFileSize += dwAddSize;

        hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE | SEC_COMMIT | SEC_NOCACHE,
            0, dwFileSize, NULL);
        if (hFileMap == NULL)
        {
            ShowLastError();
            return NULL;
        }

        // Get Where in the File does this thing start
        //--------------------------------------------
        pdlRoot = (LPDLROOT)IQS_MapViewOfFile(hFileMap, dwFileStartPos, sizeof(DLROOT), &fmap);
        if (pdlRoot == NULL)
        {
            ShowLastError();
            CloseHandle(hFile);
            return NULL;
        }

        // Set some important stuff
        //-------------------------
        pdlRoot->dwID = IQS_DLLIST_ID;
        pdlRoot->dwVersion = IQS_DLLIST_VERSION;
        pdlRoot->dwSize = sizeof(DLROOT);

        // Set the Grow start position
        // and the grow size
        // NOTE: the fmapGrow will never
        // be actualy mapped. It will
        // be used as a referance ...
        //------------------------------
        pdlRoot->fmapGrow.lMapStart = dwFileStartPos + sizeof(DLROOT);
        pdlRoot->fmapGrow.lMapSize = dwAddSize - sizeof(DLROOT);

        // Calculate the Estimated Data Size
        //----------------------------------
        pdlRoot->lDataBuffSize = lEntryDataSize * lEntriesNum;

        pdlRoot->lItemsPerBuffer = lEntriesNum;
        pdlRoot->lItemMaxSize = lEntryDataSize;
        pdlRoot->bGrow = bGrow;

        pdlRoot->lEntryCount = 0;

        pdlRoot->lFirstItem = -1;
        pdlRoot->lLastItem = -1;
        pdlRoot->lFirstItemFree = -1;


        pdlRoot->lIDXnum = 1;
        pdlRoot->lIndexInUse = 0; // the default Linked List Index
        ZeroMemory(pdlRoot->szFileName, 512);
        strcpy_s(pdlRoot->szFileName, sizeof(pdlRoot->szFileName), lpstrFName);
    }
    else
    {
        hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE | SEC_COMMIT | SEC_NOCACHE,
            0, dwFileSize, NULL);
        if (hFileMap == NULL)
        {
            ShowLastError();
            return NULL;
        }

        // Get Where in the File does this thing start
        //--------------------------------------------
        pdlRoot = (LPDLROOT)IQS_MapViewOfFile(hFileMap, dwFileStartPos, sizeof(DLROOT), &fmap);
        if (pdlRoot == NULL)
        {
            ShowLastError();
            CloseHandle(hFileMap);
            return NULL;
        }

        // Verify the header
        //------------------
        if ((pdlRoot->dwID != IQS_DLLIST_ID) || (pdlRoot->dwVersion != IQS_DLLIST_VERSION))
        {
            IQS_UnmapViewOfFile(&fmap);
            CloseHandle(hFileMap);
            return NULL; // error
        }
    }

    // Remember the File mapping object
    //---------------------------------
    pdlRoot->hfilemap = hFileMap;
    pdlRoot->dwSizeMapped = dwFileSize;

    // make sure the first time
    // we need data it will be mapped
    //-------------------------------
    pdlRoot->fmapData.pBase = NULL;

    // store the file handle
    //----------------------
    pdlRoot->hfile = hFile;
    pdlRoot->bCloseFileHandle = bCloseFileHandle;

    // Store the File Mapping for the Root
    //------------------------------------
    pdlRoot->fmapRoot = fmap;

    // Allocate the RootPtr object
    //----------------------------
    // Allocate the RootPtr object
    //----------------------------
    pRootPtr = (LPDLROOTPTR)GlobalAlloc(GPTR, sizeof(DLROOTPTR));
    if (pRootPtr == NULL)
    {
        FreeDLListRoot(pdlRoot);
        return NULL;
    }

    pRootPtr->pRoot = pdlRoot;
    pRootPtr->bSyncEnabled = bSyncEnabled;
    // prepare the Root Object for
    // multithreading
    //----------------------------
    if (bSyncEnabled)
        InitializeCriticalSection(&pRootPtr->csRoot);

    // Init the Indexes
    //-----------------
    if (InitIDX(pRootPtr))
    {
        FreeDLListRoot(pdlRoot);
        return NULL;
    }
    return pRootPtr;
}

//============================================
//InitIDX
//
//return:
//          0 = OK;
//          n = error;
//
// Init Indexes. if we are loading from a file
// the Indexes are intialized from there.
//
//============================================
int InitIDX(LPDLROOTPTR pdlRootPTR)
{
    LPDLROOT pdlRoot;
    LPIDX pIDX;
    long lIDXnum, lIDXcount;

    pdlRoot = pdlRootPTR->pRoot;
    lIDXnum = pdlRoot->lIDXnum;
    if (lIDXnum < 1)
        return 3; // error .. incorrect Number of Indexes

    // now set the Free pointers
    //--------------------------
    if ((pdlRoot->lIndexInUse >= MAX_INDEXES) && (pdlRoot->lIndexInUse < 0))
        pdlRoot->lIndexInUse = 0; // set it to the Default Index 

    if ((pdlRoot->lFirstItem == -1) && (pdlRoot->lFirstItemFree == -1))
    {
        pIDX = &pdlRoot->pIDX[0];
        // Set the Defaults
        //-----------------
        pIDX->lType = IDX_DLLIST_TYPE;
        pIDX->dwVersion = IQS_DLLIST_VERSION;
        pIDX->lPageSize = pdlRoot->lItemsPerBuffer * sizeof(DLENTRY);
        if (AddIndexPage(pdlRootPTR))
            return 4;
    }
    else
        // Read and initialize all of the Indexes
        //---------------------------------------
        for (lIDXcount = 0; lIDXcount < lIDXnum; lIDXcount++)
        {
            // Read the Index headers
            //-----------------------
            pIDX = &pdlRoot->pIDX[lIDXcount];
            switch (pIDX->lType)
            {
            case IDX_DLLIST_TYPE:
                // Do Something here if needed
                //----------------------------
                // like if(AddIndexPage(pdlRootPTR))
                //    return 4;
                break;
            default:
                break;
            }
        }

    //if(lpdlRoot->lFirstItemFree == -1)
    //    lpdlRoot->lFirstItemFree = lpdlRoot->lpIDX[lpdlRoot->lIndexInUse].pdleMem;

    return 0;
}

//==========================================================
//InitIDX_DLList
//
//parameters:
//      LPDLROOT    pdlr; the pointer to the Root
//      LPDLENTRY   pdle; pointer to the Entry buffer
//
//
//return:
//      0 = OK
//      n = error
//
//
//==========================================================
int InitIDXDL_List(LPDLROOT lpdlr, LPFILEMAP p_fmap)
{
    LPDLENTRY pdle;
    long lPrev;
    long lItemcount;
    long lItemsPerBuffer;
    long lMapStart, lMapCur;

    lItemsPerBuffer = lpdlr->lItemsPerBuffer;
    lMapStart = lMapCur = p_fmap->lMapStart;

    // go through the buffer
    //and set all of the Indexes
    //--------------------------
    lPrev = -1;
    pdle = (LPDLENTRY)p_fmap->pData;
    for (lItemcount = 0; lItemcount < lItemsPerBuffer; lItemcount++)
    {
        pdle->lPagePtr = lMapStart;
        pdle->lNext = lMapCur + sizeof(DLENTRY);
        pdle->lPrev = lPrev;
        pdle->lParent = 0;
        pdle->lChild = 0;
        pdle->lDataPtr = 0;
        pdle->lDataSize = 0;
        pdle->lFlags = DL_DELETED;
        lPrev = lMapCur;
        lMapCur += sizeof(DLENTRY);
        pdle++;
    }
    // go one back
    // so we point to the last one
    //----------------------------
    pdle--;
    pdle->lNext = -1;

    // now work on the Root Free items
    //--------------------------------
    if (lpdlr->lFirstItemFree != -1)
    {
        pdle->lNext = lpdlr->lFirstItemFree;
        // we might need to hook the current
        // lFirstItemFree->lPrev to the 
        // new Free items ... !!!!!!
        //----------------------------------
    }

    lpdlr->lFirstItemFree = lMapStart;
    return 0;
}

//===================================
//FUNCTION: FreeIDX
//
//
// free all the Indexes assosiated
// with a given Root
//
//===================================
void    FreeIDX(LPDLROOT lpdlr)
{
    /*
    long        lCount;
    LPIDX       lpidx;

    if(lpdlr->lIDXnum < 1)
        return;

    if(lpdlr->lpIDX == NULL)
        return;

    lpidx = lpdlr->lpIDX;
    for(lCount = 0; lCount < lpdlr->lIDXnum; lCount++)
        {
        if(lpidx != NULL)
            {
            if(lpidx->pdleMem)
                GlobalFree((HGLOBAL)lpidx->pdleMem);

            if(lpidx->pdleFile)
                GlobalFree((HGLOBAL)lpidx->pdleFile);

            }
        lpidx++; // move the pointer ahead
        }

    GlobalFree((HGLOBAL)lpdlr->lpIDX);
    lpdlr->lpIDX = NULL;
    */
    return;
}

//===================================
//FUNCTION: FreeDLListRoot
//
//params:
//      LPDLROOT  lpdlr
//
//purpose:
//  It will delete all of the Roots
//  allocated so far.
//
//====================================
void FreeDLListRoot(LPDLROOT lpdlr)
{
    HANDLE hfile;
    HANDLE hFileMap;
    BOOL bCloseFileHandle;

    if (lpdlr == NULL)
        return;

    hfile = lpdlr->hfile;
    hFileMap = lpdlr->hfilemap;
    bCloseFileHandle = lpdlr->bCloseFileHandle;

    // Release the Filemapping objects
    //--------------------------------
    if (lpdlr->fmapData.pBase)
        IQS_UnmapViewOfFile(&lpdlr->fmapData);

    //if(lpdlr->fmapGrow.pBase)
    //    IQS_UnmapViewOfFile(&lpdlr->fmapGrow);

    if (lpdlr->fmapIdx.pBase)
        IQS_UnmapViewOfFile(&lpdlr->fmapIdx);

    //if(lpdlr->fmapFree.pBase)
    //    IQS_UnmapViewOfFile(&lpdlr->fmapFree);

    if (lpdlr->fmapRoot.pBase)
        IQS_UnmapViewOfFile(&lpdlr->fmapRoot);

    // Close the File mapping Object
    //------------------------------
    CloseHandle(hFileMap);
    if (bCloseFileHandle)
        CloseHandle(hfile);

    return;
}

//===================================
//FreeDLListRootAll
//
//
//
//===================================
DLList32_API void FreeDLListRootAll(LPDLROOTPTR* pRootPtr)
{
    LPDLROOTPTR pRootPtrObj;

    pRootPtrObj = (*pRootPtr);
    if (pRootPtrObj == NULL)
        return;

    if (pRootPtrObj->bSyncEnabled)
        EnterCriticalSection(&pRootPtrObj->csRoot);

    // make sure it's NULL
    // so we can avoid deadlocks
    // if someone else attempts
    // to get to it
    //---------------------------
    *pRootPtr = NULL;

    FreeDLListRoot(pRootPtrObj->pRoot);

    if (pRootPtrObj->bSyncEnabled)
        DeleteCriticalSection(&pRootPtrObj->csRoot);

    // Free the memory allocated for this
    // DLList object
    //--------------
    GlobalFree((HGLOBAL)pRootPtrObj);
    return;
}

//=====================================
//FUNCTION: AddEntry
//
//purpose: Add item to the linked list
// it always adds to the end and always
// to the root
//
//=====================================
DLList32_API long AddEntry(LPDLROOTPTR lpdlrPtr, LPSTR lpstrData, long lEntrySize)
{
    long lReturn;

    // Check if we this Root is Syncronized
    //-------------------------------------
    if (lpdlrPtr->bSyncEnabled)
        EnterCriticalSection(&lpdlrPtr->csRoot);

    lReturn = InsertEntry(lpdlrPtr, GetLastEntry(lpdlrPtr), lpstrData, lEntrySize, DL_RELATION_PEER);

    // Check if we this Root is Syncronized
    //-------------------------------------
    if (lpdlrPtr->bSyncEnabled)
        LeaveCriticalSection(&lpdlrPtr->csRoot);

    return lReturn;
}

//=======================================
//FUNCTION: InsertEntry
//
//params:
//      LPDLROOT        lpdlr
//      long            lAfter ; the file position of
//                              the entry after which
//                              this one gets added
//      LPSTR           lpstrData
//      long            lEntrySize
//      long            lRelation
//
//return:
//      -1   if error
//      n >0 valid pointer(position in the file)
//
//
//purpose:
//  To insert an entry into
// a specific position in the linked list
// Note: it always does insert the
// Entry after the lAfter(Entry)
//=======================================
DLList32_API long InsertEntry(LPDLROOTPTR lpdlrPtr, long lAfter, LPSTR lpstrData, long lEntrySize, long lRelation)
{
    LPDLENTRY pdle, pdlAfter;
    LPDLROOT lpdlr;
    DWORD dwToGrow;
    FILEMAP fmap = { 0 };
    FILEMAP fmapAfter = { 0 };
    long ldlePos, lNext;
    LPSTR pData;
    HANDLE hfilemap;

    dwToGrow = 0;
    lpdlr = lpdlrPtr->pRoot;

    if ((lpdlr == NULL) || (lpstrData == NULL))
        return -1;

    // Check if we this Root is Syncronized
    //-------------------------------------
    if (lpdlrPtr->bSyncEnabled)
        EnterCriticalSection(&lpdlrPtr->csRoot);

    hfilemap = lpdlr->hfilemap;

    // make sure the Entry size
    // is not going to be larger than
    // the maximum Entry Size for this
    // Root ....
    //--------------------------------
    //if(lEntrySize > lpdlr->lDataBuffSize)
    //    return -1;


    // Check for free items in the
    // linked list pages
    //----------------------------
    if (lpdlr->lFirstItemFree == -1)
    {
        // Can we grow this file more
        //---------------------------
        if (lpdlr->bGrow == FALSE)
            //        if(lpdlr->lEntryCount >= lpdlr->lItemsPerBuffer)
            goto ERROR_EXIT;

        // now that we have the memory
        // lets reserve it for indexes
        //----------------------------
        if (AddIndexPage(lpdlrPtr))
            goto ERROR_EXIT;

        // get the local variables again
        //------------------------------
        lpdlr = lpdlrPtr->pRoot;
        hfilemap = lpdlr->hfilemap;
    }

    // Get the First free Item Pointer
    //--------------------------------
    pdle = (LPDLENTRY)IsDataMapped(&lpdlr->fmapIdx, lpdlr->lFirstItemFree, sizeof(DLENTRY),
        &lpdlr->fmapIdx, hfilemap);

    if (pdle == NULL)
        goto ERROR_EXIT;

    // is there  enough free  memory(file) available
    // and does this item has a memory assosiated with
    // it already that is big  enough
    //-------------------------------------------------
    if ((pdle->lDataSize < lEntrySize) && (lpdlr->fmapGrow.lMapSize < lEntrySize))
    {
        if (lpdlr->lDataBuffSize < lEntrySize)
            dwToGrow = lEntrySize;
        else
            dwToGrow = lpdlr->lDataBuffSize;

        // Grow the file
        //--------------
        if (GrowDLFileMap(lpdlrPtr, dwToGrow))
            goto ERROR_EXIT; // error

        // get the local variables again
        //------------------------------
        lpdlr = lpdlrPtr->pRoot;
        hfilemap = lpdlr->hfilemap;

        // Get the First free Item Pointer
        //--------------------------------
        pdle = (LPDLENTRY)IsDataMapped(&lpdlr->fmapIdx, lpdlr->lFirstItemFree, sizeof(DLENTRY),
            &lpdlr->fmapIdx, hfilemap);

        if (pdle == NULL)
            goto ERROR_EXIT;
    }

    lNext = pdle->lNext;

    // Get the File Position of this Free item
    //-----------------------------------------
    ldlePos = lpdlr->lFirstItemFree;


    // is there a valid position
    //--------------------------
    if (lAfter > 0)
    {
        pdlAfter = (LPDLENTRY)IQS_MapViewOfFile(hfilemap, lAfter, sizeof(DLENTRY), &fmapAfter);
        if (pdlAfter == NULL)
            goto ERROR_EXIT;

        switch (lRelation)
        {
        case DL_RELATION_CHILD:
            pdle->lParent = lAfter;
            pdle->lNext = pdlAfter->lChild;
            pdle->lPrev = -1;
            pdle->lChild = -1;

            // adjust the pointers of the
            // child, if any
            //---------------------------
            if (pdle->lNext > 0)
            {
                if (IQS_MapViewOfFile(hfilemap, pdle->lNext, sizeof(DLENTRY), &fmap) == NULL)
                    goto ERROR_EXIT;

                ((LPDLENTRY)fmap.pData)->lPrev = ldlePos;

                IQS_UnmapViewOfFile(&fmap);
            }

            // now adjust the pointers of the 
            // parent so this item will become
            // the first Child
            //--------------------------------
            pdlAfter->lChild = ldlePos;

            break;

        case DL_RELATION_PEER:
            pdle->lParent = pdlAfter->lParent;
            pdle->lNext = pdlAfter->lNext;
            pdle->lPrev = lAfter;
            pdle->lChild = -1;

            // set the link for the next item 
            // if any...
            //-------------------------------    
            if (pdle->lNext > 0)
            {
                if (IQS_MapViewOfFile(hfilemap, pdle->lNext, sizeof(DLENTRY), &fmap) == NULL)
                    goto ERROR_EXIT;

                ((LPDLENTRY)fmap.pData)->lPrev = ldlePos;

                IQS_UnmapViewOfFile(&fmap);
            }

            // now adjust the pointers of the 
            // previous peer
            //-------------------------------
            pdlAfter->lNext = ldlePos;

            break;

        default:
            // WE JUST NEED TO RETURN AN ERROR HERE
            //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            //-------------------------------------
            IQS_UnmapViewOfFile(&fmapAfter);
            // wrong relation ... so go ahead an
            // link back this item to the
            // free items chain
            //----------------------------------
//            lpdlr->lFirstItemFree = ldlePos;
            goto ERROR_EXIT;
            break;
        }
        // release this map of the file
        //-----------------------------
        IQS_UnmapViewOfFile(&fmapAfter);
    }
    else
    {
        // This must be the very first item in the list
        //---------------------------------------------
        if (lpdlr->lFirstItem == -1)
        {
            pdle->lParent = -1;
            pdle->lNext = -1;
            pdle->lPrev = -1;
            pdle->lChild = -1;
            // set the first Root Item
            lpdlr->lFirstItem = ldlePos;
        }
        else
        {
            // well we have an item that has to 
            // be added to the Start of the list
            //----------------------------------
            pdle->lParent = -1;
            pdle->lChild = -1;
            pdle->lNext = lpdlr->lFirstItem;
            pdle->lPrev = -1;
            // set the link for the next item 
            // if any...
            //-------------------------------    
            if (pdle->lNext > 0)
            {
                if (IQS_MapViewOfFile(hfilemap, pdle->lNext, sizeof(DLENTRY), &fmap) == NULL)
                    goto ERROR_EXIT;

                ((LPDLENTRY)fmap.pData)->lPrev = ldlePos;

                IQS_UnmapViewOfFile(&fmap);
            }
            // set the first Root Item
            lpdlr->lFirstItem = ldlePos;
        }
    }

    // Unlink the first free item from the
    // free items chain .. so it's ready to be 
    // used for insertion
    //----------------------------------------
    lpdlr->lFirstItemFree = lNext;


    // is there  enough free 
    // memory(file) available
    // and does this item has
    // a memory assosiated with
    // it already that is big 
    // enough
    //-------------------------
    if (pdle->lDataSize < lEntrySize)
    {
        if (pdle->lDataSize > 0)
        {
            // keep track of how many bytes
            // are lost here
            //-----------------------------
            lpdlr->lBytesLost += pdle->lDataSize;
        }
        // assaign the new memory attached
        // to this Index item
        //--------------------------------
        pdle->lDataPtr = lpdlr->fmapGrow.lMapStart;
        pData = IsDataMapped(&lpdlr->fmapData, pdle->lDataPtr, lEntrySize, &lpdlr->fmapData, hfilemap);
        if (pData == NULL)
            goto ERROR_EXIT;

        // Adjust how much space we have
        // available to grow the linked list
        //----------------------------------
        lpdlr->fmapGrow.lMapStart += lEntrySize;
        lpdlr->fmapGrow.lMapSize -= lEntrySize;
    }
    else
    {
        // Check if we have this data mapped
        // if not mapped it to the Global
        // fmapData
        //----------------------------------
        pData = IsDataMapped(&lpdlr->fmapData, pdle->lDataPtr, lEntrySize, &lpdlr->fmapData, hfilemap);
        if (pData == NULL)
        {
            goto ERROR_EXIT;
        }
        // keep track of how many bytes
        // are lost here
        //-----------------------------
        lpdlr->lBytesLost += pdle->lDataSize - lEntrySize;
    }


    pdle->lFlags = DL_USED;
    pdle->lDataSize = lEntrySize;

    // Store the Data into
    // the Global buffer
    //---------------------
    CopyMemory(pData, lpstrData, lEntrySize);


    // Store only the root items as last Items
    //----------------------------------------
    if ((lAfter == lpdlr->lLastItem) && (lRelation == DL_RELATION_PEER))
        lpdlr->lLastItem = ldlePos;


    lpdlr->lEntryCount++; // one more Entry
    // if there were Free items ... we have just used
    // one of them so go ahead and reduce them
    //-----------------------------------------------
    if (lpdlr->lDelCount > 0)
        lpdlr->lDelCount--;


    // Check if we this Root is Syncronized
    //-------------------------------------
    if (lpdlrPtr->bSyncEnabled)
        LeaveCriticalSection(&lpdlrPtr->csRoot);

    return ldlePos;


ERROR_EXIT:
    // Leave the critical section
    //---------------------------
    if (lpdlrPtr->bSyncEnabled)
        LeaveCriticalSection(&lpdlrPtr->csRoot);

    return -1;
}

//==============================
//FUNCION: DelEntry
//
//return:
//      0 if OK
//      n is an Error code
//
//purpose:
//      To delete an entry
// which means it will get
// added to the Deleted linked
// list. This function just
// finds the pointer to the
// item and then calls
// DelEntryPtr
//
//==============================
DLList32_API int DelEntry(LPDLROOTPTR lpdlr, long lEntryNum)
{
    /*
    long        lCount, lTotCount;
    LPDLENTRY   lpdle;

    if(lpdlr == NULL)
        return 1;

    lTotCount = lpdlr->lEntryCount;
    if(lTotCount < 1)
        return 3;


    if(lEntryNum == 0)
        {
        lpdle = lpdlr->lpdleFirst;
        goto DE_EXIT;
        }

    if(lEntryNum == lTotCount)
        {
        lpdle = lpdlr->lpdleLast;
        goto DE_EXIT;
        }

    // Check to see if the Entry is closer
    // to end or to the beginig. So will
    // search less
    //------------------------------------
    if((lTotCount/2)>lEntryNum)
        {
        lpdle = lpdlr->lpdleFirst;
        for(lCount=0; lCount < lEntryNum; lCount++)
            lpdle = lpdle->pNext;
        }
    else
        {
        lpdle = lpdlr->lpdleLast;
        for(lCount=lTotCount; lCount > lEntryNum; lCount--)
            lpdle = lpdle->pPrev;
        }

    DE_EXIT:
    return DelEntryPtr(lpdlr, lpdle);
    */
    return 0;
}

//==============================
//FUNCION: DelEntryPtr
//
//return:
//      0 if OK
//      n is an Error code
//
//purpose:
//      To delete an entry
// which means it will get
// added to the Deleted linked
// list.
//NOTE: When deleting an Item
// that has children we need 
// to convert all of its
// children to Siblings
//==============================
DLList32_API int DelEntryPtr(LPDLROOTPTR lpdlrPtr, long lItemPos)
{
    LPDLROOT lpdlr;
    LPDLENTRY pdle, pdleParent, pdleChild, pdleNext, pdlePrev;
    long lCurrentPos, lNext;
    long lItemsCount;
    HANDLE hfilemap;
    FILEMAP fmapNext = { 0 };
    FILEMAP fmapPrev = { 0 };
    FILEMAP fmapParent = { 0 };
    FILEMAP fmapChild = { 0 };

    lpdlr = lpdlrPtr->pRoot;

    if ((lpdlr == NULL) || (lItemPos < 0))
        return 5;

    hfilemap = lpdlr->hfilemap;

    fmapNext.pData = NULL;
    fmapPrev.pData = NULL;

    // is this item already in the 
    // Index buffer
    //----------------------------
    pdle = (LPDLENTRY)IsDataMapped(&lpdlr->fmapIdx, lItemPos, sizeof(DLENTRY), &lpdlr->fmapIdx, hfilemap);
    if (pdle == NULL)
    {
        IQS_UnmapViewOfFile(&lpdlr->fmapIdx);
        return 7;
    }

    // Check if the Entry is not already Deleted
    //------------------------------------------
    if (pdle->lFlags != DL_USED)
        return 6;

    // Adjust the Links
    // for the Actual Entries
    //-----------------------
    if (pdle->lNext > 0)
    {
        pdleNext = (LPDLENTRY)IsDataMapped(&lpdlr->fmapIdx, pdle->lNext, sizeof(DLENTRY), &fmapNext, hfilemap);
        if (pdleNext == NULL)
        {
            return 7;
        }
        pdleNext->lPrev = pdle->lPrev;
        // now free the mapping if needed
        //-------------------------------
        if (fmapNext.pData)
            IQS_UnmapViewOfFile(&fmapNext);
    }
    else
    {
        // we need to adjust the global lLastItem
        // since there is no other entry after
        // this one we can assume it is the last
        // one in the list
        //---------------------------------------
        if (pdle->lParent == -1)
            lpdlr->lLastItem = pdle->lPrev;
    }

    if (pdle->lPrev > 0)
    {
        pdlePrev = (LPDLENTRY)IsDataMapped(&lpdlr->fmapIdx, pdle->lPrev, sizeof(DLENTRY), &fmapPrev, hfilemap);
        if (pdlePrev == NULL)
        {
            // now free the mapping if needed
            //-------------------------------
            if (fmapPrev.pData)
                IQS_UnmapViewOfFile(&fmapPrev);
            return 7;
        }
        pdlePrev->lNext = pdle->lNext;
        // now free the mapping if needed
        //-------------------------------
        if (fmapPrev.pData)
            IQS_UnmapViewOfFile(&fmapPrev);
    }
    else
    {
        // we need to adjust the global lpdleFirst
        // since there is no other entry before
        // this one we can assume it is the first
        // one in the list
        //----------------------------------------
        if (pdle->lParent == -1)
            lpdlr->lFirstItem = pdle->lNext;
    }

    if (pdle->lParent > 0)
    {
        pdleParent = (LPDLENTRY)IsDataMapped(&lpdlr->fmapIdx, pdle->lParent, sizeof(DLENTRY), &fmapParent, hfilemap);
        if (pdleParent == NULL)
        {
            // now free the mapping if needed
            //-------------------------------
            if (fmapParent.pData)
                IQS_UnmapViewOfFile(&fmapParent);
            return 7;
        }
        // this becomes the first Child
        //-----------------------------
        pdleParent->lChild = pdle->lNext;

        // Unmap the parent if needed
        //---------------------------
        if (fmapParent.pData)
            IQS_UnmapViewOfFile(&fmapParent);
    }

    // add this item to the Free Items List
    //-------------------------------------
    pdle->lNext = lpdlr->lFirstItemFree;
    lpdlr->lFirstItemFree = lItemPos;

    lItemsCount = 1;

    // from here on we can
    // go only to the Child of this one
    //---------------------------------
    lCurrentPos = pdle->lChild;
    pdle->lParent = -1;
    pdle->lChild = -1;
    pdle->lPrev = -1;

    // Check for any Child Items
    // How it works?!!
    // Go to the End of the Child
    // links and to the End of the
    // Peer links and the starts
    // going backwards freeing the items
    // basic rule -> find the last item
    // by going <next...child> and then
    // unroll by going <up...parent>
    //-----------------------------------
    if (lCurrentPos == -1)
        goto ON_EXIT;

    pdleChild = (LPDLENTRY)IsDataMapped(&lpdlr->fmapIdx, lCurrentPos, sizeof(DLENTRY),
        &fmapChild, hfilemap);
    if (pdleChild == NULL)
        goto ON_EXIT;

    if (pdleChild)
    {
    DO_ROLL:
        while (pdleChild->lNext > 0)
        {
            lCurrentPos = pdleChild->lNext;
            pdleChild = (LPDLENTRY)IsDataMapped(&fmapChild, lCurrentPos, sizeof(DLENTRY),
                &fmapChild, hfilemap);
        }
        while (pdleChild->lChild > 0)
        {
            lCurrentPos = pdleChild->lChild;
            pdleChild = (LPDLENTRY)IsDataMapped(&fmapChild, lCurrentPos, sizeof(DLENTRY),
                &fmapChild, hfilemap);
        }
        if (pdleChild->lNext > 0)
            goto DO_ROLL;

        // Prepare for Unroll
        // and unlink the items
        //---------------------
        if (pdleChild->lPrev > 0)
        {
            lNext = pdleChild->lPrev;
            pdleNext = (LPDLENTRY)IsDataMapped(NULL, lNext, sizeof(DLENTRY),
                &fmapNext, hfilemap);
            pdleNext->lNext = -1;
            IQS_UnmapViewOfFile(&fmapNext);
        }
        else
            if (pdleChild->lParent > 0)
            {
                lNext = pdleChild->lParent;
                pdleNext = (LPDLENTRY)IsDataMapped(NULL, lNext, sizeof(DLENTRY),
                    &fmapNext, hfilemap);
                pdleNext->lChild = -1;
                IQS_UnmapViewOfFile(&fmapNext);
            }
            else
                lNext = -1;
        // add this item to the Free Items List
        // it should always be the last item 
        // in the list.
        //-------------------------------------
    //    lpdlr->pFirstItemFree->pdlPrev = lpdleChild;
        pdleChild->lNext = lpdlr->lFirstItemFree;
        pdleChild->lPrev = -1;
        pdleChild->lChild = -1;
        pdleChild->lParent = -1;
        // Set the Flags
        //--------------
        pdleChild->lFlags = DL_DELETED | DL_OLD_DATA;
        lpdlr->lFirstItemFree = lCurrentPos;

        lCurrentPos = lNext;
        // keep track of how many items
        // we have freed
        //-----------------------------
        lItemsCount++;

        // Do the Unroll now
        // and then Roll again
        //--------------------
        if ((lNext > 0) && (lNext != lItemPos))
        {
            pdleChild = (LPDLENTRY)IsDataMapped(&fmapChild, lNext, sizeof(DLENTRY),
                &fmapChild, hfilemap);
            if (pdleChild)
                goto DO_ROLL;
        }
    }

ON_EXIT:
    // Cleanup the mess
    //-----------------
    if (fmapChild.pData)
        IQS_UnmapViewOfFile(&fmapChild);

    if (fmapParent.pData)
        IQS_UnmapViewOfFile(&fmapParent);

    if (fmapNext.pData)
        IQS_UnmapViewOfFile(&fmapNext);

    if (fmapPrev.pData)
        IQS_UnmapViewOfFile(&fmapPrev);

    // take care of the Counters
    //--------------------------
    lpdlr->lEntryCount -= lItemsCount;// One Entry Less
    lpdlr->lDelCount += lItemsCount; // One more Deleted Entry
    if (lpdlr->lEntryCount <= 0)
    {
        lpdlr->lEntryCount = 0;
    }
    return 0;
}

//============================
//FUNCTION: CompactDLList
//
//NOTE: For now this will work only from
// memory
//
//============================
DLList32_API LPDLROOTPTR CompactDLList(LPDLROOTPTR lpdlrPtr)
{
    /*
    LPDLROOT    lpdlrNew;
    long        lDataSize, lEntrySize;
    BOOL        bGrow;
    LPDLENTRYMEM   lpdleSource;
    LPDLENTRYMEM   lpdleDest;
    char        szTempName[255];


    lDataSize = lpdlr->dwDataBuffSize;
    bGrow = lpdlr->bGrow;

    // use the temp name to compact to
    //--------------------------------
    lpdlrNew = InitDoubleLinkedList(lDataSize, 0, bGrow, lpdlr->bSpooled, szTempName);
    if(lpdlrNew == NULL)
        return NULL;

    lpdleSource = lpdlr->lpdleFirst;

    while(lpdleSource)
        {
        lEntrySize = lpdleSource->lEntrySize;   // this size will already include
                                                // the sizeof(DLENTRY)
        lpdleDest = AddEntry(lpdlrNew, lEntrySize);
        if(lpdleDest == NULL)
            {
            FreeDLListRoot(lpdlrNew);
            return NULL;
            }
        CopyMemory((LPSTR)lpdleDest->pData, (LPSTR)lpdleSource->pData, lEntrySize-sizeof(DLENTRY));

        lpdleSource = lpdleSource->pNext;
        }

    // Get rid of the old Root thingy
    //-------------------------------
    FreeDLListRoot(lpdlr);

    return lpdlrNew;
    */
    return NULL;
}


//===============================================
//===============================================
//===============================================
//===============================================
//
//  N A V I G A T I O N   P R O C E D U R E S
//  N A V I G A T I O N   P R O C E D U R E S
//  N A V I G A T I O N   P R O C E D U R E S
//  N A V I G A T I O N   P R O C E D U R E S
//  N A V I G A T I O N   P R O C E D U R E S
//
//===============================================
//===============================================
//===============================================
//===============================================

// in general these procedures return
// the pointer to DLENTRY
//-----------------------------------------
//-----------------------------------------

//==========================================
//
//
//
//
//
//==========================================
DLList32_API long GetEntryLinkState(LPDLROOTPTR pRootPtrObj, long lCur)
{
    LPDLROOT pdlr;
    long lLinkState;
    FILEMAP fmap;
    LPDLENTRY pdle;

    if (pRootPtrObj->bSyncEnabled)
        EnterCriticalSection(&pRootPtrObj->csRoot);

    lLinkState = 0;

    pdlr = pRootPtrObj->pRoot;
    if (pdlr == NULL)
        lLinkState = -1;
    else
    {
        fmap.pData = NULL;
        pdle = (LPDLENTRY)IsDataMapped(&pdlr->fmapIdx, lCur, sizeof(DLENTRY),
            &fmap, pdlr->hfilemap);
        if (pdle == NULL)
            lLinkState = -1;
        else
        {
            // now set the bit flags for the Entry Link state
            //-----------------------------------------------
            if (pdle->lNext > 0)
                lLinkState |= DLENTRY_NEXT;
            if (pdle->lPrev > 0)
                lLinkState |= DLENTRY_PREV;
            if (pdle->lChild > 0)
                lLinkState |= DLENTRY_CHILD;
            if (pdle->lParent > 0)
                lLinkState |= DLENTRY_PARENT;
        }

        if (fmap.pData != NULL)
            IQS_UnmapViewOfFile(&fmap);
    }

    if (pRootPtrObj->bSyncEnabled)
        LeaveCriticalSection(&pRootPtrObj->csRoot);

    return lLinkState;
}


//==========================================
//FUNCTION: GetFirstEntry
//
//return:
//          NULL    = error
//        LPDLENTRYMEM = pointer to the data
//                    part of the Entry
//
//==========================================
DLList32_API long GetFirstEntry(LPDLROOTPTR pRootPtrObj)
{
    LPDLROOT pdlr;
    long lReturn;

    if (pRootPtrObj->bSyncEnabled)
        EnterCriticalSection(&pRootPtrObj->csRoot);

    pdlr = pRootPtrObj->pRoot;

    if (pdlr == NULL)
        lReturn = -1;
    else
        lReturn = pdlr->lFirstItem;

    if (pRootPtrObj->bSyncEnabled)
        LeaveCriticalSection(&pRootPtrObj->csRoot);

    return lReturn;
}

//==========================================
//FUNCTION: GetLastEntry
//
//return:
//          NULL    = error
//        LPDLENTRYMEM = pointer to the data
//                    part of the Entry
//
//==========================================
DLList32_API long GetLastEntry(LPDLROOTPTR pRootPtrObj)
{
    LPDLROOT pdlr;
    long lReturn;

    if (pRootPtrObj->bSyncEnabled)
        EnterCriticalSection(&pRootPtrObj->csRoot);

    pdlr = pRootPtrObj->pRoot;
    if (pdlr == NULL)
        lReturn = -1;
    else
        lReturn = pdlr->lLastItem;

    if (pRootPtrObj->bSyncEnabled)
        LeaveCriticalSection(&pRootPtrObj->csRoot);

    return lReturn;
}


//==========================================
//FUNCTION: GetNextEntry
//
//
//return:
//          NULL    = error
//        LPDLENTRYMEM = pointer to the data
//                    part of the Entry
//
//==========================================
DLList32_API long GetNextEntry(LPDLROOTPTR pRootPtrObj, long lCur)
{
    LPDLROOT pdlr;
    long lReturn;
    LPDLENTRY pdle;
    FILEMAP fmap;

    if (pRootPtrObj->bSyncEnabled)
        EnterCriticalSection(&pRootPtrObj->csRoot);

    pdlr = pRootPtrObj->pRoot;
    if (pdlr == NULL)
        lReturn = -1;
    else
    {
        fmap.pData = NULL;
        pdle = (LPDLENTRY)IsDataMapped(&pdlr->fmapIdx, lCur, sizeof(DLENTRY),
            &fmap, pdlr->hfilemap);
        if (pdle == NULL)
            lReturn = -1;
        else
            lReturn = pdle->lNext;

        // if the data was not mapped into
        // the main fmapIdx. So lets map it there
        // Basicaly we are going to map the 
        // entire page of DLENTRY indexes
        //---------------------------------------
        if (fmap.pData != NULL)
        {
            // unmap the current file map
            //---------------------------
            IQS_UnmapViewOfFile(&pdlr->fmapIdx);

            // map the page of indexes
            //------------------------
            IQS_MapViewOfFile(pdlr->hfilemap, pdle->lPagePtr,
                pdlr->lItemsPerBuffer * sizeof(DLENTRY), &pdlr->fmapIdx);

            // unmap the local map
            //--------------------
            IQS_UnmapViewOfFile(&fmap);
        }
    }

    if (pRootPtrObj->bSyncEnabled)
        LeaveCriticalSection(&pRootPtrObj->csRoot);

    return lReturn;
}

//==========================================
//FUNCTION: GetPrevEntry
//
//
//return:
//          -1    = error
//          n > 0 = position to the data
//                    part of the Entry
//
//==========================================
DLList32_API long GetPrevEntry(LPDLROOTPTR pRootPtrObj, long lCur)
{
    LPDLROOT pdlr;
    long lReturn;
    LPDLENTRY pdle;
    FILEMAP fmap;

    if (pRootPtrObj->bSyncEnabled)
        EnterCriticalSection(&pRootPtrObj->csRoot);

    pdlr = pRootPtrObj->pRoot;
    if (pdlr == NULL)
        lReturn = -1;
    else
    {
        fmap.pData = NULL;
        pdle = (LPDLENTRY)IsDataMapped(&pdlr->fmapIdx, lCur, sizeof(DLENTRY),
            &fmap, pdlr->hfilemap);
        if (pdle == NULL)
            lReturn = -1;
        else
            lReturn = pdle->lPrev;

        // if the data was not mapped into
        // the main fmapIdx. So lets map it there
        // Basicaly we are going to map the 
        // entire page of DLENTRY indexes
        //---------------------------------------
        if (fmap.pData != NULL)
        {
            // unmap the current file map
            //---------------------------
            IQS_UnmapViewOfFile(&pdlr->fmapIdx);

            // map the page of indexes
            //------------------------
            IQS_MapViewOfFile(pdlr->hfilemap, pdle->lPagePtr,
                pdlr->lItemsPerBuffer * sizeof(DLENTRY), &pdlr->fmapIdx);

            // unmap the local map
            //--------------------
            IQS_UnmapViewOfFile(&fmap);
        }
    }

    if (pRootPtrObj->bSyncEnabled)
        LeaveCriticalSection(&pRootPtrObj->csRoot);

    return lReturn;
}


//================================================
//FUNCTION: GetEntryData
//
//params:
//      LPDLENTRYMEM   lpdle; the Entry pointer
//                         or Offset
//      LPDLROOT    lpdlr; the List Root
//                         pointer
//
//
// return:
//          NULL = error;
//          ptr  = pointer to this Entrys data
//================================================
DLList32_API LPVOID GetEntryData(LPDLROOTPTR pRootPtrObj, long lEntryPos)
{
    LPDLROOT pdlr;
    LPVOID pData;
    LPDLENTRY pdle;
    FILEMAP fmap = { 0 };

    if (pRootPtrObj->bSyncEnabled)
        EnterCriticalSection(&pRootPtrObj->csRoot);

    pdlr = pRootPtrObj->pRoot;

    pData = NULL;

    // first look for this entry
    // in the Index page
    //---------------------------
    pdle = (LPDLENTRY)IsDataMapped(&pdlr->fmapIdx, lEntryPos, sizeof(DLENTRY),
        &pdlr->fmapIdx, pdlr->hfilemap);
    if (pdle == NULL)
        goto ON_EXIT;


    // now try to locate the actual Entry(item) data
    // in the file and map a big chunck of data
    // in pdlr->fmapData ... usualy the size of the 
    // data being mapped = pdlr->lDataBuffSize
    //----------------------------------------------
    pData = IsDataMapped(&pdlr->fmapData, pdle->lDataPtr, pdle->lDataSize,
        NULL, NULL);

    if (pData == NULL)
    {
        IQS_UnmapViewOfFile(&pdlr->fmapData);

        pData = IQS_MapViewOfFile(pdlr->hfilemap, pdle->lDataPtr, pdlr->lDataBuffSize, &pdlr->fmapData);
        if (pData == NULL)
            pData = IQS_MapViewOfFile(pdlr->hfilemap, pdle->lDataPtr, pdle->lDataSize, &pdlr->fmapData);
        // this will map the entire lDataBuffSize from the
        // start of this entry
        //------------------------------------------------
    //    pData = IsDataMapped(&pdlr->fmapData, pdle->lDataPtr, pdlr->lDataBuffSize,
    //                         &pdlr->fmapData, pdlr->hfilemap);
    }

    if (pRootPtrObj->bSyncEnabled)
        LeaveCriticalSection(&pRootPtrObj->csRoot);

ON_EXIT:
    return pData;
}

//===================================================
//FUNCTION: IQS_MapViewOfFile
//
//return:
//      Pointer = OK;
//      NULL  =  error;
//
// NOTE: No validation is performed on the Parameters
//===================================================
LPSTR IQS_MapViewOfFile(HANDLE hfilemap, DWORD dwFilePosition, DWORD dwSize, LPFILEMAP pfilemap)
{
    register DWORD       dwBytesToMap;
    register DWORD       dwCalcOffset;

    pfilemap->pData = NULL;
    pfilemap->lMapStart = dwFilePosition;

    // Calculate the offsets an the sizes
    // The offset must a multiple of the
    // memory allocation granularity ...
    // the size should be adjusted if
    // the CalcOffset is different than the 
    // dwFilePosition
    //-------------------------------------
    dwCalcOffset = (dwFilePosition >> g_sinf.dwAllocationGranularity) << g_sinf.dwAllocationGranularity;
    dwBytesToMap = dwSize + (dwFilePosition - dwCalcOffset);

    pfilemap->pBase = (LPSTR)MapViewOfFile(hfilemap, FILE_MAP_ALL_ACCESS, 0, dwCalcOffset, dwBytesToMap);
    if (pfilemap->pBase == NULL)
        goto ON_EXIT;

    pfilemap->lMapStartOffset = dwCalcOffset;
    // store only the requested size
    // cause the rest we can't realy USE
    //----------------------------------
    pfilemap->lMapSize = dwSize;
    pfilemap->pData = pfilemap->pBase + (dwFilePosition - dwCalcOffset);

ON_EXIT:
    return pfilemap->pData;
}

//===================================================
//FUNCTION: IQS_UnmapViewOfFile
//
//
//
//===================================================
int IQS_UnmapViewOfFile(LPFILEMAP pfilemap)
{
    LPSTR    p;
    p = pfilemap->pBase;
    pfilemap->pBase = NULL;
    pfilemap->pData = NULL;

    if (p)
    {
        UnmapViewOfFile(p);
    }
    return 0;
}

//==================================================================
//FUNCTION: IsDataMapped
//
//parameters:
//          LPFILEMAP   pfmapOrig; the already mapped
//                              data where this data
//                              might be
//          long        lPos; where in the file is the data
//          long        lSize; the size of the data
//          LPFILEMAP   pfmapNew; the new file map structure
//                                which will be initialized if
//                                if the data is not in the 
//                                pfmapOrig
//          HANDLE      hfmap; the handle to the File Mapping Object
//                             from where to map the data if needed 
//
//
//return:
//          NULL if it is not mapped;
//          *pointer* if it is;
//
//==================================================================
LPSTR IsDataMapped(LPFILEMAP pfmapOrig, long lPos, long lSize, LPFILEMAP pfmapNew, HANDLE hfmap)
{
    long lMapEnd;
    LPSTR pData;

    pData = NULL;

    // validate some of the parameters
    //--------------------------------
    if ((lPos < 0) || (lSize < 0))
        return NULL;

    if (pfmapOrig == NULL)
        goto MAP_TO_NEW;

    if (pfmapOrig->pBase == NULL)
        goto MAP_TO_NEW;

    // Is the Data already mapped
    //---------------------------
    lMapEnd = pfmapOrig->lMapStart + pfmapOrig->lMapSize;
    if ((pfmapOrig->lMapStart <= lPos) && ((lPos + lSize) <= lMapEnd))
    {
        // we have it mapped lets calculate the pointer position
        //------------------------------------------------------
        pData = pfmapOrig->pData + (lPos - pfmapOrig->lMapStart);
        return pData;
    }

MAP_TO_NEW:
    if (pfmapNew == NULL)
        return NULL;

    if (hfmap == NULL)
        return NULL;

    // Unmap the original File mapping
    // if the New points to the same memory
    //-------------------------------------
    if (pfmapOrig == pfmapNew)
        IQS_UnmapViewOfFile(pfmapOrig);

    if (IQS_MapViewOfFile(hfmap, lPos, lSize, pfmapNew) == NULL)
        return NULL;

    pData = pfmapNew->pData;

    return pData;
}

//===================================================
//FUNCTION: GrowDLFileMap
//
//return:
//      0 = OK;
//      n = error;
//
//===================================================
int GrowDLFileMap(LPDLROOTPTR pdlrPtr, long lSize)
{
    LPDLROOT pdlr;
    HANDLE hFileMap;
    HANDLE hFile;
    FILEMAP fmap;
    long lCurMapped;
    DWORD dwFileStartPos;

    pdlr = pdlrPtr->pRoot;
    hFile = pdlr->hfile;
    hFileMap = pdlr->hfilemap;
    lCurMapped = pdlr->dwSizeMapped;
    fmap = pdlr->fmapRoot;
    dwFileStartPos = fmap.lMapStart;

    // is it possible to grow the file to this size
    //---------------------------------------------
    if ((long)(LONG_MAX - lCurMapped) < lSize)
        return 1; // error

    // First Unmap all of the 
    // Views for this Root
    //-----------------------
    if (pdlr->fmapData.pBase)
        IQS_UnmapViewOfFile(&pdlr->fmapData);

    //if(pdlr->fmapGrow.pBase)
    //    IQS_UnmapViewOfFile(&pdlr->fmapGrow);

    if (pdlr->fmapIdx.pBase)
        IQS_UnmapViewOfFile(&pdlr->fmapIdx);

    //if(pdlr->fmapFree.pBase)
    //    IQS_UnmapViewOfFile(&pdlr->fmapFree);

    if (pdlr->fmapRoot.pBase)
        IQS_UnmapViewOfFile(&pdlr->fmapRoot);

    //--------------------
    CloseHandle(hFileMap);
    pdlrPtr->pRoot = NULL;

    hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE | SEC_COMMIT | SEC_NOCACHE,
        0, lCurMapped + lSize, NULL);
    if (hFileMap == NULL)
    {
        // Set this value back to ZERO because 
        // we are going to use it latter and
        // and the file didn't grow for some reason
        //-----------------------------------------
        lSize = 0;
        // try to remap the file with its old size
        //----------------------------------------
        hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE | SEC_COMMIT | SEC_NOCACHE,
            0, lCurMapped, NULL);
        if (hFileMap == NULL)
            // this is bad ... damn
            //---------------------
        {
            pdlrPtr->pRoot = NULL;
            return 3;
        }
    }
    else
        lCurMapped += lSize;

    // Map the Root again
    //-------------------
    if (IQS_MapViewOfFile(hFileMap, dwFileStartPos, sizeof(DLROOT), &fmap) == NULL)
        // Critical Error
        return 2;

    pdlr = (LPDLROOT)fmap.pData;
    pdlrPtr->pRoot = pdlr;
    pdlr->fmapRoot = fmap;
    pdlr->hfilemap = hFileMap; // this is likely changed so save it agian
    pdlr->dwSizeMapped = lCurMapped;

    // once we have restored the Root Mapping
    // the rest of the Maps should be still intact
    // so we can reuse them and the rest of the program will know nothing
    // about the total Remaping we just went through
    //-------------------------------------------------------------------

    //Map the Data Again
    //------------------
    fmap = pdlr->fmapData;
    if (IQS_MapViewOfFile(hFileMap, fmap.lMapStart, fmap.lMapSize, &pdlr->fmapData) == NULL)
        return 2;

    // Map the Index part again
    //-------------------------
    fmap = pdlr->fmapIdx;
    if (IQS_MapViewOfFile(hFileMap, fmap.lMapStart, fmap.lMapSize, &pdlr->fmapIdx) == NULL)
        return 2;

    // Map the Free Index part again
    //------------------------------
    //fmap = pdlr->fmapFree;
    //if(IQS_MapViewOfFile(hFileMap, fmap.lMapStart, fmap.lMapSize, &pdlr->fmapFree) == NULL)
    //    return 2;

    // Map the Grow part Again
    // and include the new
    // data buffer we just allocated
    //------------------------------
    pdlr->fmapGrow.lMapSize += lSize;
    //fmap = pdlr->fmapGrow;
    //fmap.lMapSize += lSize;
    //if(IQS_MapViewOfFile(hFileMap, fmap.lMapStart, fmap.lMapSize, &pdlr->fmapGrow) == NULL)
    //    return 2;

    // we should be done now
    //----------------------
    return 0;
}

//===============================
//FUNCTION: AddIndexPage
//
//return:
//      0 = OK
//      n = error
//===============================
int AddIndexPage(LPDLROOTPTR pdlrPtr)
{
    LPDLROOT lpdlr;
    LPIDX    pidx;
    FILEMAP  fmap = { 0 };

    // get the pointer to the Root map
    //--------------------------------
    lpdlr = pdlrPtr->pRoot;

    // Get the Pointer to the Current Index In Use
    //--------------------------------------------
    pidx = &lpdlr->pIDX[lpdlr->lIndexInUse];

    // is there  enough free 
    // memory(file) available
    //-----------------------
    if (lpdlr->fmapGrow.lMapSize < pidx->lPageSize)
    {
        // Grow the file
        //--------------
        if (GrowDLFileMap(pdlrPtr, pidx->lPageSize))
            return 1; // error

        // get them again because the addresses 
        // might be different
        //-------------------------------------
        lpdlr = pdlrPtr->pRoot;
        pidx = &lpdlr->pIDX[lpdlr->lIndexInUse];
    }


    if (IQS_MapViewOfFile(lpdlr->hfilemap, lpdlr->fmapGrow.lMapStart, pidx->lPageSize, &fmap) == NULL)
        return 2;

    // Adjust the Grow data
    //---------------------
    lpdlr->fmapGrow.lMapStart += pidx->lPageSize;
    lpdlr->fmapGrow.lMapSize -= pidx->lPageSize;

    if (InitIDXDL_List(lpdlr, &fmap))
    {
        // Unmap View of the file
        // cause we don't need it
        // any more 
        //-----------------------
        IQS_UnmapViewOfFile(&fmap);
        return 3;
    }

    // Unmap View of the file
    // cause we don't need it
    // any more 
    //-----------------------
    IQS_UnmapViewOfFile(&fmap);

    return 0;
}
