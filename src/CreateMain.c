//=================================================
//
// Copyright 1998, CorTek Software, Inc.
//=================================================

//#include <windows.h>

#include "SAMM.h"
#include "SAMMEXT.h"
#include "MACRO.h"



HWND		ghwndTBSeqReadout = NULL;
int			giTBSeqReadout = 9512;

//=================================
// FUNCTION: CreateMainWindow
// creates the main window
//=================================
int       CreateMainWindow(void)
{
int         iReturn;
WNDCLASS    wc;


// Register Main Window Class
//---------------------------
ZeroMemory(&wc, sizeof(WNDCLASS));      // Clear wndclass structure

wc.style = CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNCLIENT;
wc.lpfnWndProc = (WNDPROC)WndMainProc;
wc.cbClsExtra = 0;
wc.cbWndExtra = 0;
wc.hInstance = ghInstMain;
wc.hIcon = LoadIcon(ghInstMain, MAKEINTRESOURCE(SAMMMainIcon));// this might leak memory
wc.hCursor = LoadCursor(NULL, IDC_ARROW);
wc.hbrBackground = GetStockObject(BLACK_BRUSH);
wc.lpszMenuName = NULL;
wc.lpszClassName = gszMainWndClass;

iReturn = RegisterClass(&wc);

if(iReturn == 0)
	 return(IDS_ERR_REGISTER_CLASS);     // Error... Exit

// Create Main Window
//-------------------
ghwndMain = CreateWindow(
					 gszMainWndClass,         // Window class name
					 gszMainWndTitle,         // Window's title
					 WS_OVERLAPPED   |       // Overlapped Style
					 WS_CAPTION      |       // Title and Min/Max
					 WS_SYSMENU      |       // Add system menu box
					 WS_MINIMIZEBOX  |       // Add minimize box
					 WS_MAXIMIZEBOX  |       // Add maximize box
					 WS_THICKFRAME   |       // thick sizeable frame
					 WS_CLIPCHILDREN ,
					 CW_USEDEFAULT,          // Use default x
					 CW_USEDEFAULT,          // Use default y
					 CW_USEDEFAULT,          // Use default Width
					 CW_USEDEFAULT,          // Use default Height
					 NULL,                   // Parent window's handle
					 NULL,                   // Default to Class Menu
					 ghInstMain,              // Instance of window
					 NULL);                  // Ptr To Data Structure For WM_CREATE

if(ghwndMain == NULL)
	 return(IDS_ERR_CREATE_WINDOW);      // Error... Exit


//AddToCaption(ghwndMain, sz_MIXERNAME);
SetMenu(ghwndMain, ghMainMenu);
return 0;
};

/////////////////////////////////////////////////////////////////////
//	MEMBER FUNCTION: CreateToolbars
//
//
//
int		CreateToolBars(HWND		hwndParent)
{
#define TB_BUTTONS_COUNT	28
  int					iRet = 0;
  //TBADDBITMAP tbab;
  TBBUTTON		tbb[TB_BUTTONS_COUNT];
	TBBUTTONINFO tbinfo = {0};
  int					iCount = 0 , i;
  RECT				r;                      
  //char        szBuff[MAX_STRING_SIZE];
  HWND        hToolTips;
	char				szStrings[64] = " 1\0 2\0 3\0 4\0 5\0 6\0 7\0 8\0\0";

  for(iCount = 0; iCount < 7; iCount ++)
	{
	  tbb[iCount].iBitmap = iCount;
	  tbb[iCount].idCommand = iCount + IDM_S_PLAY; 
	  tbb[iCount].fsState = TBSTATE_ENABLED;
	  tbb[iCount].fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE; 
	  tbb[iCount].dwData = 0;
	  tbb[iCount].iString = -1; 
	}
  // add a separator
  //
  tbb[iCount].iBitmap = -1;
  tbb[iCount].idCommand = 0;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_SEP;// | TBSTYLE_AUTOSIZE;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1; 

	// Sequence name
  iCount++;
  tbb[iCount].iBitmap = -1; // account for the separator
  tbb[iCount].idCommand = giTBSeqReadout;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;// | TBSTYLE_DROPDOWN;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1; 
  
  // add a separator
  //
  iCount++;
  tbb[iCount].iBitmap = -1;
  tbb[iCount].idCommand = 0;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_SEP;// | TBSTYLE_AUTOSIZE;// | TBSTYLE_DROPDOWN;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1; 

	// Save mix file(fast save)
  iCount++;
  tbb[iCount].iBitmap = 11; // account for the separator
  tbb[iCount].idCommand = IDM_F_SAVE_FILE;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1; 

  // add a separator
  //
  iCount++;
  tbb[iCount].iBitmap = -1;
  tbb[iCount].idCommand = 0;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_SEP;// | TBSTYLE_AUTOSIZE;// | TBSTYLE_DROPDOWN;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1; 
	
	// Sequence Window tool button
  iCount++;
  tbb[iCount].iBitmap = 20; // account for the separator
  tbb[iCount].idCommand = IDM_V_SEQUENCE;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1; 

	// Group Window tool button
  iCount++;
  tbb[iCount].iBitmap = 7; // account for the separator
  tbb[iCount].idCommand = IDM_V_GROUP;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1; 

  // Zoom Window tool button
  iCount++;
  tbb[iCount].iBitmap = 8; // account for the separator
  tbb[iCount].idCommand = IDM_V_FULLZOOM;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1; 

  // Full Window tool button
  iCount++;
  tbb[iCount].iBitmap = 9; // account for the separator
  tbb[iCount].idCommand = IDM_V_FULLCONSOLE;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1; 

  // add a separator
  //
  iCount++;
  tbb[iCount].iBitmap = -1;
  tbb[iCount].idCommand = 0;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_SEP;// | TBSTYLE_AUTOSIZE;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1; 

  // Cancel the Cues
  //
  iCount++;
  tbb[iCount].iBitmap = 21; 
  tbb[iCount].idCommand = IDM_V_CANCELCUES;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1; 

  // Cancel the Groups
  //
  iCount++;
  tbb[iCount].iBitmap = 10; // account for the separator
  tbb[iCount].idCommand = IDM_V_CANCELGROUPS;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1; 

  // Groups Selector starts after this separartor
  //
  iCount++;
  tbb[iCount].iBitmap = -1;
  tbb[iCount].idCommand = 0;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_SEP;// | TBSTYLE_AUTOSIZE;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1; 

  // Cancel the Groups
  //
	for(i=0; i < 8; i++)
	{
	  iCount++;
		tbb[iCount].iBitmap = 12 + i; 
		tbb[iCount].idCommand = IDM_V_CANCELGROUPS + i + 1;
		tbb[iCount].fsState = TBSTATE_INDETERMINATE; // TBSTATE_ENABLED
		tbb[iCount].fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
		tbb[iCount].dwData = 0;
		tbb[iCount].iString = -1;  // i
	}

  ghwndTBPlay = CreateToolbarEx(hwndParent, WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_ALTDRAG | TBSTYLE_LIST ,// | CCS_ADJUSTABLE, 
															  1100, 19, ghInstMain, IDB_PLAYTOOL,(LPCTBBUTTON) &tbb, TB_BUTTONS_COUNT,
															  16, 14, 16, 14, sizeof(TBBUTTON));

	// SendMessage(ghwndTBPlay, TB_ADDSTRING, (WPARAM)0, (LPARAM)szStrings);


	// now Create the readout window ....
	tbinfo.cbSize = sizeof(TBBUTTONINFO);
	tbinfo.dwMask = TBIF_SIZE | TBIF_STYLE;
	tbinfo.fsStyle = TBSTYLE_SEP;
	tbinfo.cx = 175;

	SendMessage(ghwndTBPlay, TB_SETBUTTONINFO, (WPARAM)giTBSeqReadout, (LPARAM)&tbinfo);

  SetWindowPos(ghwndTBPlay, NULL, 100, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
  SendMessage(ghwndTBPlay, TB_AUTOSIZE, 0, 0);


	SendMessage(ghwndTBPlay, TB_GETITEMRECT, (WPARAM)8, (LPARAM)&r);
	r.top = 2;

	ghwndTBSeqReadout = CreateWindow("STATIC", "", WS_VISIBLE | WS_CHILD | WS_BORDER,//CBS_DROPDOWNLIST | CBS_HASSTRINGS,
																	r.left, r.top, r.right - r.left, r.bottom - r.top,
																	ghwndTBPlay, NULL, ghInstMain, NULL);
	
	SetWindowText(ghwndTBSeqReadout, "Sequence ...");
	//SendMessage(ghwndTBSeqReadout, CB_RESETCONTENT, 0, (LPARAM)0);
	//SendMessage(ghwndTBSeqReadout, CB_ADDSTRING, 0, (LPARAM)"Sequence ...");
	
	//SendMessage(ghwndTBPlay, TB_SETEXTENDEDSTYLE, 0, (LPARAM)TBSTYLE_EX_DRAWDDARROWS);
/*
  // Add some strings to the Toolbar control so the Tool tip control will work
  //
  LoadString(ghInstStrRes, IDM_V_FULLCONSOLE, (LPSTR) &szBuff, MAX_STRING_SIZE);
  SendMessage(ghwndTBPlay, TB_ADDSTRING, 0, (LPARAM) (LPSTR) szBuff);

  LoadString(ghInstStrRes, IDM_V_FULLZOOM, (LPSTR) &szBuff, MAX_STRING_SIZE);
  SendMessage(ghwndTBPlay, TB_ADDSTRING, 0, (LPARAM) (LPSTR) szBuff);
*/                             



	//SendMessage(ghwndTBPlay, TB_SETEXTENDEDSTYLE, 0, (LPARAM)(DWORD)TBSTYLE_EX_DRAWDDARROWS);

  ShowWindow(ghwndTBPlay, SW_SHOW);

  GetWindowRect(ghwndTBPlay, &r);

  hToolTips = (HWND)SendMessage(ghwndTBPlay, TB_GETTOOLTIPS, 0, 0);
  SendMessage(hToolTips, TTM_ACTIVATE, (WPARAM)TRUE,0);


	i = SendMessage(hToolTips, TTM_GETDELAYTIME, (WPARAM)TTDT_AUTOPOP, (LPARAM)MAKELONG(0, 0));
	i = SendMessage(hToolTips, TTM_GETDELAYTIME, (WPARAM)TTDT_INITIAL, (LPARAM)MAKELONG(0, 0));
	i = SendMessage(hToolTips, TTM_GETDELAYTIME, (WPARAM)TTDT_RESHOW, (LPARAM)MAKELONG(0, 0));

return iRet;
}

////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
void ShowTBSeqName(LPSTR pName)
{
	if(ghwndTBSeqReadout)
		SetWindowText(ghwndTBSeqReadout, pName);
}

void EnableTBGroupButtons(int iLast)
{
	int	iEnable = 0;
	int	iCount;

	if(iLast < 1)
	{
		iEnable = 0;
	}
	else
	{
		iEnable = (iLast >= 8)?8:iLast;
	}

	// Enable some of the buttons
	for(iCount = 0; iCount < iEnable; iCount++)
	{
		SendMessage(ghwndTBPlay, TB_ENABLEBUTTON, (WPARAM)IDM_V_CANCELGROUPS + iCount + 1,(LPARAM) MAKELONG(TRUE, 0));
		SendMessage(ghwndTBPlay, TB_SETSTATE, (WPARAM)IDM_V_CANCELGROUPS + iCount + 1,(LPARAM) MAKELONG(TBSTATE_ENABLED, 0));
	}
	for(iCount; iCount < 8; iCount++)
	{
		SendMessage(ghwndTBPlay, TB_ENABLEBUTTON, (WPARAM)IDM_V_CANCELGROUPS + iCount + 1,(LPARAM) MAKELONG(FALSE, 0));
		SendMessage(ghwndTBPlay, TB_SETSTATE, (WPARAM)IDM_V_CANCELGROUPS + iCount + 1,(LPARAM) MAKELONG(TBSTATE_INDETERMINATE, 0));
	}


}

//=======================================
//FUNCTION: Create256Window
//
// creates a hidden window
// which DC we are going to use
// for the 256 color bitmaps
//=======================================
int       Create256Window(void)
{
int         iReturn;
WNDCLASS    wc;


// Register Main Window Class
//---------------------------
memset(&wc, 0x00, sizeof(WNDCLASS));      // Clear wndclass structure

wc.style = CS_CLASSDC;
wc.lpfnWndProc = (WNDPROC)DefWindowProc;
wc.cbClsExtra = 0;
wc.cbWndExtra = 0;
wc.hInstance = ghInstMain;
wc.hIcon = NULL;
wc.hCursor = NULL;
wc.hbrBackground = GetStockObject(BLACK_BRUSH);
wc.lpszMenuName = NULL;
wc.lpszClassName = "256COLOR";

iReturn = RegisterClass(&wc);

if(iReturn == 0)
	 return(IDS_ERR_REGISTER_CLASS);     // Error... Exit

// Create Main Window
//-------------------
ghwnd256 = CreateWindow(
					 "256COLOR",         // Window class name
					 NULL,         // Window's title
					 WS_POPUP,
					 CW_USEDEFAULT,          // Use default x
					 CW_USEDEFAULT,          // Use default y
					 CW_USEDEFAULT,          // Use default Width
					 CW_USEDEFAULT,          // Use default Height
					 NULL,                   // Parent window's handle
					 NULL,                   // Default to Class Menu
					 ghInstMain,              // Instance of window
					 NULL);                  // Ptr To Data Structure For WM_CREATE

if(ghwnd256 == NULL)
	 return(IDS_ERR_CREATE_WINDOW);      // Error... Exit

ghdc256 = GetDC(ghwnd256);
//UpdatePalette(FALSE, hdc256);
return 0;
}

//=============================================
//FUNCTION: Close256Window
//
// Closes the 256 Window and Releases the DC
//=============================================
void      Close256Window(void)
{
ReleaseDC(ghwnd256, ghdc256);
DestroyWindow(ghwnd256);

return;
}
