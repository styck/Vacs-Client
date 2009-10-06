//=================================================
// Copyright 1998, CorTek Software, Inc.
//=================================================

// Misc Resource Storage
//----------------------


//===================================================
// The Binary Global Resource Memory
// storage. It deals with all of the Binary Resources
// by putting them in globaly allocated memory
// and returns an index to them. The index and the
// global pointer (lpBinResTable) can be used to
// access the Resources.
//
//===================================================

//#include <windows.h>

#include "SAMM.h"
#include "SAMMEXT.h"
#include "MACRO.h"

//==============================================================
//FUNCTION: AddBinResource
//
//params:
//      int     iResourceID; -> the resource to load
//                              also this value is
//                              that we are going to check
//                              for duplicates. If there is a
//                              duplicate then we return the
//                              offset of this duplicate into
//                              the array, else we  load and
//                              add this Bin res to the Global
//                              array of Bin resources
//                              and return this value.
//
//      int     iResType; ->    the Resource Type
//
//return:
//      if less than 0 its an error else
//      returns the index at which this bitmap was
//      added in the Global table of bitmaps
//      for the Given Resource
//
//
//==============================================================
int     AddBinResource(HINSTANCE hinst, int iRes, int iResType)
{
	int             iCount;
	int             iOldAllocated;
	LPBINARYTABLE   lpGlob;
	HRSRC           hRes;


	for(iCount = 0; iCount < giBinResGlobAlloc;iCount++)
	{
		if(gpBinResTable[iCount].iResID == iRes)
			return iCount;
		else
			if(gpBinResTable[iCount].iResID == 0)
				break;
	}

	// Check if we need to allocate more memory
	// to put the next MIDI map in the Global Table
	//---------------------------------------------
	if(iCount == giBinResGlobAlloc)
	{
		iOldAllocated = giBinResGlobAlloc;
		giBinResGlobAlloc += 16;
		lpGlob = GlobalAlloc(GPTR, giBinResGlobAlloc*sizeof(BINARYTABLE));
		if(lpGlob == NULL)
			return -(IDS_ERR_ALLOCATE_MEMORY);

		CopyMemory((LPSTR)lpGlob, (LPSTR)gpBinResTable, iOldAllocated*sizeof(BINARYTABLE));
		GlobalFree(gpBinResTable);
		gpBinResTable = lpGlob;
	}

	// Load the ReadOut Resource
	//--------------------------
	hRes = FindResource(hinst, MAKEINTRESOURCE(iRes), MAKEINTRESOURCE(iResType));
	if(hRes == NULL)
		return -(IDS_ERR_LOCATING_RESOURCE);

	gpBinResTable[iCount].hgBinRes = LoadResource(hinst, hRes);
	gpBinResTable[iCount].lpBinRes = (LPSTR)LockResource(gpBinResTable[iCount].hgBinRes);
	// now  set the string pointer to the begining of the Strings
	//-----------------------------------------------------------
	gpBinResTable[iCount].iResID = iRes;

	return iCount;
}

//==============================================================
//FUNCTION: DeleteBinResGlobalIndx
//
// purpouse:
//          To delete a given index in the
//          Global Resource table
//
//==============================================================
void    DeleteBinResGlobalIndx(int iIndx)
{
	if(gpBinResTable == NULL)
		return;
	if(iIndx >= giBinResGlobAlloc)
		return; // wrong index

	if(gpBinResTable[iIndx].iResID)
	{
		UnlockResource(gpBinResTable[iIndx].hgBinRes);
		FreeResource(gpBinResTable[iIndx].hgBinRes);
		gpBinResTable[iIndx].iResID = 0;
	}
	return;
}

//==============================================================
//FUNCTION: DeleteBinResGlobalResID
//
// purpouse:
//          To delete a given ResID in the
//          Global Resource table
//
//==============================================================
void    DeleteBinResGlobalResID(int iResID)
{
	int     iCount;

	if(gpBinResTable == NULL)
		return;

	for(iCount=0; iCount < giBinResGlobAlloc; iCount++)
	{
		if(gpBinResTable[iCount].iResID == iResID)
		{
			UnlockResource(gpBinResTable[iCount].hgBinRes);
			FreeResource(gpBinResTable[iCount].hgBinRes);
			gpBinResTable[iCount].iResID = 0;
			return;
		}
	}
	return;
}

//===============================
// FUNCTION: InitBinResGlobal
//
//
//===============================
int     InitBinResGlobal(void)
{

	giBinResGlobAlloc = 16;
	gpBinResTable = (LPBINARYTABLE)GlobalAlloc(GPTR, giBinResGlobAlloc*sizeof(BINARYTABLE));
	if(gpBinResTable == NULL)
		return IDS_ERR_ALLOCATE_MEMORY;

	return 0;
}

//====================================
//FUNCTION: FreeBinResGlobal
//
//====================================
void    FreeBinResGlobal(void)
{
	int         iCount;

	if(gpBinResTable == NULL)
		return;

	for(iCount=0; iCount < giBinResGlobAlloc; iCount++)
		DeleteBinResGlobalIndx(iCount);

	GlobalFree((HGLOBAL)gpBinResTable);
	gpBinResTable = NULL;

	return;
}


//==============================================================
//FUNCTION: AddRdOutResGlobal
//
//params:
//      int     iResourceID; -> the resource to load
//                              also this value is
//                              that we are going to check
//                              for duplicates. If there is a
//                              duplicate then we return the
//                              offset of this duplicate into
//                              the array, else we  load and
//                              add this RdOut res to the Global
//                              array of RdOut resources
//                              and return this value.
//
//
//return:
//      if less than 0 its an error else
//      returns the index at which this bitmap was
//      added in the Global table of bitmaps
//      for the Given Resource
//
//
//==============================================================
int     PASCAL  AddRdOutResGlobal(HINSTANCE hinst, int iRes)
{
	int             iCount;
	int             iOldAllocated;
	LPRDOUTGLOBRES  lpGlob;
	HRSRC           hRes;
	LPSTR           lpStr;

	for(iCount=0; iCount < giRdOutGlobAllocated; iCount++)
	{
		if(gpRdOutTable[iCount].iResID == iRes)
			return iCount;
		else
			if(gpRdOutTable[iCount].iResID == 0)
				break;
	}

	// Check if we need to allocate more memory
	// to put the next MIDI map in the Global Table
	//---------------------------------------------
	if(iCount == giRdOutGlobAllocated)
	{
		iOldAllocated = giRdOutGlobAllocated;
		giRdOutGlobAllocated += 16;
		lpGlob = (LPRDOUTGLOBRES)GlobalAlloc(GPTR, giRdOutGlobAllocated*sizeof(RDOUTGLOBRES));
		if(lpGlob == NULL)
			return -(IDS_ERR_ALLOCATE_MEMORY);

		CopyMemory((LPSTR)lpGlob, (LPSTR)gpRdOutTable, iOldAllocated*sizeof(RDOUTGLOBRES));

		GlobalFree(gpRdOutTable);
		gpRdOutTable = lpGlob;
	}

	// Load the ReadOut Resource
	//--------------------------
	hRes = FindResource(hinst, MAKEINTRESOURCE(iRes), MAKEINTRESOURCE(RT_RDOUTTABLE) );
	if(hRes == NULL)
		return -(IDS_ERR_LOCATING_RESOURCE);

	gpRdOutTable[iCount].hgRdOut = LoadResource(hinst, hRes);
	gpRdOutTable[iCount].lpRdOut = (LPRDOUTTABLE)LockResource(gpRdOutTable[iCount].hgRdOut);
	// now  set the string pointer to the begining of the Strings
	//-----------------------------------------------------------
	lpStr = (LPSTR)gpRdOutTable[iCount].lpRdOut;
	gpRdOutTable[iCount].lpStr =  lpStr + sizeof(RDOUTTABLE);
	gpRdOutTable[iCount].iResID = iRes;

	return iCount;
}

//==============================================================
//FUNCTION: DeleteRdOutResGlobalIndx
//
// purpouse:
//          To delete a given index in the
//          Global Resource table
//
//==============================================================
void    PASCAL  DeleteRdOutResGlobalIndx(int iIndx)
{
	if(gpRdOutTable == NULL)
		return;
	if(iIndx >= giRdOutGlobAllocated)
		return; // wrong index
	if(gpRdOutTable[iIndx].iResID)
	{
		UnlockResource(gpRdOutTable[iIndx].hgRdOut);
		FreeResource(gpRdOutTable[iIndx].hgRdOut);
		gpRdOutTable[iIndx].iResID = 0;
	}
	return;
}

//==============================================================
//FUNCTION: DeleteRdOutResGlobalResID
//
// purpouse:
//          To delete a given ResID in the
//          Global Resource table
//
//==============================================================
void    PASCAL  DeleteRdOutResGlobalResID(int iResID)
{
	int     iCount;

	if(gpRdOutTable == NULL)
		return;

	for(iCount=0; iCount < giRdOutGlobAllocated; iCount++)
	{
		if(gpRdOutTable[iCount].iResID == iResID)
		{
			UnlockResource(gpRdOutTable[iCount].hgRdOut);
			FreeResource(gpRdOutTable[iCount].hgRdOut);
			gpRdOutTable[iCount].iResID = 0;
			return;
		}
	}
	return;
}

//===============================
// FUNCTION: InitRdOutResGlobal
//
//
//===============================
int     PASCAL InitRdOutResGlobal(void)
{

	giRdOutGlobAllocated = 32;
	gpRdOutTable = (LPRDOUTGLOBRES)GlobalAlloc(GPTR, giRdOutGlobAllocated*sizeof(RDOUTGLOBRES));
	if(gpRdOutTable == NULL)
		return IDS_ERR_ALLOCATE_MEMORY;

	return 0;
}

//====================================
//FUNCTION: FreeRdOutResGlobal
//
//====================================
void    PASCAL  FreeRdOutResGlobal(void)
{
	int         iCount;

	if(gpRdOutTable == NULL)
		return;

	for(iCount=0; iCount < giRdOutGlobAllocated; iCount++)
		DeleteRdOutResGlobalIndx(iCount);

	GlobalFree(gpRdOutTable);
	gpRdOutTable = NULL;

	return;
}

// Misc resource functions
//------------------------

//==============================================================
//FUNCTION: AddBmpResGlobal
//
//params:
//      int     iResourceID; -> the resource to load
//                              also this value is
//                              that we are going to check
//                              for duplicates. If there is a
//                              duplicate then we return the
//                              offset of this duplicate into
//                              the array, else we add load and
//                              add this bitmap to the Global
//                              array of bitmaps and return this
//                              value.
//
//
//return:
//      if less than 0 its an error else
//      returns the index at which this bitmap was
//      added in the Global table of bitmaps
//      for the Given Resource
//
//
//==============================================================
int       AddBmpResGlobal(HINSTANCE hinst, int iRes, HDC hdc)
{
	int             iCount;
	int             iOldAllocated;
	LPBMPGLOBRES    lpGlob;
	BITMAP          bmpInfo;
	HBITMAP         hBmp;

	for(iCount=0; iCount < giBmpGlobAllocated; iCount++)
	{
		if(gpBMPTable[iCount].iResID == iRes)
			return iCount;
		else
			if(gpBMPTable[iCount].hbmp == NULL)
				break;
	}

	// Check if we need to allocate more memory
	// to put the next Bitmap in the Global Table
	//-------------------------------------------
	if(iCount == giBmpGlobAllocated)
	{
		iOldAllocated = giBmpGlobAllocated;
		giBmpGlobAllocated += 32;
		lpGlob = GlobalAlloc(GPTR, giBmpGlobAllocated*sizeof(BMPGLOBRES));
		if(lpGlob == NULL)
			return -(IDS_ERR_ALLOCATE_MEMORY);

		CopyMemory((LPSTR)lpGlob,(LPSTR) gpBMPTable, iOldAllocated*sizeof(BMPGLOBRES));
		GlobalFree(gpBMPTable);
		gpBMPTable = lpGlob;
	}

	hBmp = Load256Bitmap(hinst, hdc, (LPSTR)MAKEINTRESOURCE(iRes));
	if(hBmp == NULL)
		return - (IDS_ERR_LOAD_BITMAP);

	GetObject(hBmp, sizeof(BITMAP), &bmpInfo);
	gpBMPTable[iCount].iWidth = bmpInfo.bmWidth;
	gpBMPTable[iCount].iHeight = bmpInfo.bmHeight;
	gpBMPTable[iCount].hbmp = hBmp;
	gpBMPTable[iCount].iResID = iRes;

	return iCount;
}

//==============================================================
//FUNCTION: DeleteBmpResGlobalIndx
//
// purpouse:
//          To delete a given index in the
//          Global Resource table
//
//==============================================================
void      DeleteBmpResGlobalIndx(int iIndx)
{
	if(gpBMPTable == NULL)
		return;
	if(iIndx >= giBmpGlobAllocated)
		return; // wrong index
	if(gpBMPTable[iIndx].hbmp)
	{
		DeleteObject(gpBMPTable[iIndx].hbmp);
		gpBMPTable[iIndx].hbmp = NULL;
		gpBMPTable[iIndx].iWidth = 0;
		gpBMPTable[iIndx].iHeight = 0;
		gpBMPTable[iIndx].iResID = 0;
	}
	return;
}

//==============================================================
//FUNCTION: DeleteBmpResGlobalResID
//
// purpouse:
//          To delete a given ResID in the
//          Global Resource table
//
//==============================================================
void    DeleteBmpResGlobalResID(int iResID)
{
	int     iCount;

	if(gpBMPTable == NULL)
		return;

	for(iCount=0; iCount < giBmpGlobAllocated; iCount++)
	{
		if(gpBMPTable[iCount].iResID == iResID)
		{
			DeleteObject(gpBMPTable[iCount].hbmp);
			gpBMPTable[iCount].hbmp = NULL;
			gpBMPTable[iCount].iWidth = 0;
			gpBMPTable[iCount].iHeight = 0;
			gpBMPTable[iCount].iResID = 0;
			return;
		}
	}
	return;
}

//===============================
// FUNCTION: InitBMPResGlobal
//
//
//===============================
int     InitBMPResGlobal(void)
{


	giBmpGlobAllocated = 32;
	gpBMPTable = (LPBMPGLOBRES)GlobalAlloc(GPTR, giBmpGlobAllocated*sizeof(BMPGLOBRES));
	if(gpBMPTable == NULL)
		return IDS_ERR_ALLOCATE_MEMORY;

	return 0;
}

//====================================
//FUNCTION: FreeBMPResGlobal
//
//====================================
void    FreeBMPResGlobal(void)
{
	int         iCount;

	if(gpBMPTable == NULL)
		return;

	for(iCount=0; iCount < giBmpGlobAllocated;iCount++)
		DeleteBmpResGlobalIndx(iCount);

	GlobalFree(gpBMPTable);

	return;
}
