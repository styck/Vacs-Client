//////////////////////////////////////////////////////////////////////
// VACS project.
// Copyright CorTek Software, Inc. 1997-1998. All rights reserved.
//
// 256colors.c : implementation file
//
// Copyright CorTek Software, Inc. 1997-1998. All rights reserved.
//

//=================================================
// Functions for loading 256 color bitmaps from the
// resource file and also creating a pallete
//
//=================================================

//#include <windows.h>

#include "SAMM.H"
#include "SAMMEXT.H"


//=================================================================
//      Creates a palette from a given resource name
//
//  Parameters: LPSTR ResName -> the resource name
//
//  Input:      HINSTANCE   hInstMain   -> the instance from where
//                                         to load the resource this could
//                                         be passed as a parameter
//
//
//  Output:     HPALETTE    hPalette    -> is a handle to the currently
//                                         created palette, or NULL for error
//=================================================================
void    CreateResPalette(LPSTR ResName)
{
	HRSRC           TmpRsrc = NULL;
	HGLOBAL         TmpGlob = NULL;
	int             TmpColors;
	LPBITMAPINFO    TmpBmpInfo = NULL;
	WORD            n;
	const RGBQUAD far* rgb = NULL;


	if(ghPalette)
		DeleteObject(ghPalette);


	ghPalette = NULL;
	//--------------------------------------------------------
	TmpRsrc = FindResource(ghInstConsoleDef, ResName, RT_BITMAP);
	if(TmpRsrc == NULL)
		return ; // Could not find the resource

	TmpGlob = LoadResource(ghInstConsoleDef, TmpRsrc);
	if(TmpGlob == NULL)
		return ;// Could not load the resource

	//----------------------------------------------------
	TmpBmpInfo = (LPBITMAPINFO)LockResource(TmpGlob);
	if(TmpBmpInfo == NULL)
		return ;

	TmpColors = (int)NColors(TmpBmpInfo->bmiHeader.biBitCount);



	rgb = TmpBmpInfo->bmiColors;

	// if the ClrUsed field of the header is non-zero,
	// it means that we could have have a short color table.
	//-----------------------------------------------------

	if (TmpColors == 256)
	{
		LOGPALETTE far* logPal;
		HLOCAL  TempLocal = LocalAlloc(LMEM_FIXED, (sizeof(LOGPALETTE) + (TmpColors)*sizeof(PALETTEENTRY)));;
		logPal = (LOGPALETTE far*)LocalLock(TempLocal);

		if ( ! logPal )
			return;
		logPal->palVersion  = 0x300;      // Windows 3.0 version
		logPal->palNumEntries = TmpColors;
		for ( n = 0; n < TmpColors; n++)
		{
			logPal->palPalEntry[n].peRed   = rgb[n].rgbRed;
			logPal->palPalEntry[n].peGreen = rgb[n].rgbGreen;
			logPal->palPalEntry[n].peBlue  = rgb[n].rgbBlue;
			logPal->palPalEntry[n].peFlags = (BYTE)0;
		}
		ghPalette = CreatePalette(logPal);
		LocalUnlock(TempLocal);
		LocalFree(TempLocal);
	} 
	else
		ghPalette = NULL;

	FreeResource(TmpGlob);
	return;
}


/////////////////////////////////////////////////////////////////
//      Makes the hPalette active in the specified HDC
/////////////////////////////////////////////////////////////////
BOOL    UpdatePalette(BOOL Repaint, HDC hdc)
{
	if (ghPalette )
	{
		SelectPalette(hdc, ghPalette, FALSE);

		if (RealizePalette(hdc) != GDI_ERROR)
		{
			if (Repaint)
				UpdateColors(hdc);// 
			else
				UpdateColors(hdc);
			return TRUE;
		}
	}
	return FALSE;
};

//======================================================
//
//======================================================
HBITMAP Load256Bitmap(HINSTANCE hinst, HDC TmpDC, LPSTR ResName)
{
	HRSRC           TmpRsrc = NULL;
	HBITMAP         TmpBmp = NULL;
	HGLOBAL         TmpGlob = NULL;
	int             TmpColors;
	LPBITMAPINFO    TmpBmpInfo = NULL;
	char FAR        *TmpBits = NULL;


	//--------------------------------------------------------
	if(TmpDC == NULL)
		return NULL;

	//--------------------------------------------------------
	TmpRsrc = FindResource(hinst, ResName, RT_BITMAP);
	if(TmpRsrc == NULL)
		return NULL; // Could not find the resource

	TmpGlob = LoadResource(hinst, TmpRsrc);
	if(TmpGlob == NULL)
		return NULL;// Could not load the resource

	//----------------------------------------------------
	TmpBmpInfo = (LPBITMAPINFO)LockResource(TmpGlob);
	if(TmpBmpInfo == NULL)
	{
		FreeResource(TmpGlob);
		return NULL;
	}

	TmpColors = (int)NColors(TmpBmpInfo->bmiHeader.biBitCount);
	TmpBits = (char FAR*)TmpBmpInfo + (TmpBmpInfo->bmiHeader.biSize + (TmpColors*sizeof(PALETTEENTRY)));
	//----------------------------------------------------


	TmpBmp = CreateDIBitmap(TmpDC, (BITMAPINFOHEADER far*)TmpBmpInfo, CBM_INIT, TmpBits,
		TmpBmpInfo, DIB_RGB_COLORS);

	FreeResource(TmpGlob);
	return TmpBmp;
}

//====================================================
// Return the Number of colors
//====================================================
int    NColors(int bitCount)
{
	if (bitCount == 1 || bitCount == 4 || bitCount == 8)
		return 1 << bitCount;
	return 0;
}

//===========================================
//function:  Init256Colors
//
//
//===========================================
int     Init256Colors(void)
{
	WNDCLASS    wc;
	int         iReturn;
	HWND        hwnd;

	// Register Zoom View Class
	//--------------------------
	ZeroMemory(&wc,  sizeof(WNDCLASS));      // Clear wndclass structure

	wc.style = CS_CLASSDC;    
	wc.lpfnWndProc = (WNDPROC)ColorsWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(LPSTR);// all of the data for this window will be stored here
	wc.hInstance = ghInstMain;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = gsz256ColorsClass;

	iReturn = RegisterClass(&wc);

	if(iReturn == 0)
		return(IDS_ERR_REGISTER_CLASS);     // Error... Exit

	// Create the window
	//------------------
	hwnd = CreateWindow(
		gsz256ColorsClass,         // Window class name
		NULL,                    // Window's title
		WS_OVERLAPPED,          // Overlapped Style
		CW_USEDEFAULT,          // Use default x
		CW_USEDEFAULT,          // Use default y
		CW_USEDEFAULT,          // Use default Width
		CW_USEDEFAULT,          // Use default Height
		NULL,                   // Parent window's handle
		NULL,                   // Default to Class Menu
		ghInstMain,              // Instance of window
		NULL);                  // Ptr To Data Structure For WM_CREATE
	if(hwnd == 0)
	{
		return IDS_ERR_CREATE_WINDOW;
	}

	return 0;
}

//============================================
//
//
//
//============================================
LRESULT CALLBACK ColorsWndProc(HWND hwnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
	switch (wMessage)
	{
		//////////////////////////////////////////////////////////////
	case WM_CREATE:
		ghwnd256 = hwnd;
		ghdc256 = GetDC(hwnd);
		UpdatePalette(TRUE, ghdc256);
		break;

		//////////////////////////////////////////////////////////////
	case WM_DESTROY:
	case WM_CLOSE:
		ReleaseDC(hwnd, ghdc256);
		break;

	default:
		return DefWindowProc(hwnd, wMessage, wParam, lParam);

	}
	return 0;
}
