//=================================================
//
// Copyright 1998, CorTek Software, Inc.
//=================================================

//#include <windows.h>

#include "SAMM.h"
#include "SAMMEXT.h"
#include "MACRO.h"



HWND		ghwndTBSeqReadout = NULL;
HWND		ghwndTBSeqReadout2 = NULL;
int			giTBSeqReadout = 9512;
int			giTBSeqReadout2 = 9513;

HWND		ghwndTBZoomWinCntReadout = NULL;


int			giTBZoomWinCntReadout = 9514;		

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

////////////////////////////////// 
#define TB_BUTTONS_COUNT	26	// was 28 with all the buttons

/////////////////////////////////////////////////////////////////////
//	MEMBER FUNCTION: CreateToolbars
//
//
//
int		CreateToolBars(HWND		hwndParent)
{
  int					iRet = 0;
  TBBUTTON		tbb[TB_BUTTONS_COUNT];
	TBBUTTONINFO tbinfo = {0};
  int					iCount = 0, iButtonCount = 0, iSeqButton,iSeqButton2, iNumZoomWindowsButton, i;
  RECT				r;                      
  HWND        hToolTips;

	TOOLINFO	lpToolInfo;


	// Setup the following Tool Bar buttons
	// Play			0	IDM_S_PLAY						REMOVED
	// Stop			1	IDM_S_RECALL_CURRENT	REMOVED
	// Pause		2	IDM_S_PAUSE						REMOVED
	// Begining	3	IDM_S_GOTO_FIRST		REMOVED
	// Reverse	4	IDM_S_BACK
	// Forward	5	IDM_S_NEXT
	// End			6	IDM_S_GOTO_LAST				REMOVED PER GAMBLE


// These are all the buttons but Gamble does not want them all
// says they are too dangerous

#ifdef NOTUSED
	for(iCount = 0; iCount < 7; iCount ++)
	{
	  tbb[iCount].iBitmap = iCount;
	  tbb[iCount].idCommand = iCount + IDM_S_PLAY; 
	  tbb[iCount].fsState = TBSTATE_ENABLED;
	  tbb[iCount].fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE; 
	  tbb[iCount].dwData = 0;
	  tbb[iCount].iString = -1; 
	}
#endif
		//////////////////////
		// Forward button
	  tbb[iCount].iBitmap = 4;
	  tbb[iCount].idCommand = IDM_S_BACK; 
	  tbb[iCount].fsState = TBSTATE_ENABLED;
	  tbb[iCount].fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE; 
	  tbb[iCount].dwData = 0;
	  tbb[iCount].iString = -1; 
		iCount++;

		//////////////////////
		// Back button

	  tbb[iCount].iBitmap = 5;
	  tbb[iCount].idCommand = IDM_S_NEXT; 
	  tbb[iCount].fsState = TBSTATE_ENABLED;
	  tbb[iCount].fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE; 
	  tbb[iCount].dwData = 0;
	  tbb[iCount].iString = -1; 
		iCount++;

	//////////////////
  // add a separator
  //
  tbb[iCount].iBitmap = -1;
  tbb[iCount].idCommand = 0;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_SEP | TBSTYLE_AUTOSIZE;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1; 
	iCount++;

	//////////////////
	// Active Sequence name
	iSeqButton = iCount;			// save for later
  tbb[iCount].iBitmap = -1; // account for the separator
  tbb[iCount].idCommand = giTBSeqReadout;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;// | TBSTYLE_DROPDOWN;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1;
  iCount++;

	//////////////////
	// Next Sequence name
	iSeqButton2 = iCount;			// save for later
  tbb[iCount].iBitmap = -1; // account for the separator
  tbb[iCount].idCommand = giTBSeqReadout2;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;// | TBSTYLE_DROPDOWN;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1;
  iCount++;
  
  
	//////////////////
  // add a separator
  //
  tbb[iCount].iBitmap = -1;
  tbb[iCount].idCommand = 0;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_SEP;// | TBSTYLE_AUTOSIZE;// | TBSTYLE_DROPDOWN;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1; 
  iCount++;

	//////////////////
	// Save mix file(fast save)
  tbb[iCount].iBitmap = 11; // account for the separator
  tbb[iCount].idCommand = IDM_F_SAVE_FILE;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1; 
  iCount++;

	//////////////////
  // add a separator
  //
  tbb[iCount].iBitmap = -1;
  tbb[iCount].idCommand = 0;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_SEP;// | TBSTYLE_AUTOSIZE;// | TBSTYLE_DROPDOWN;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1; 
  iCount++;
	
	//////////////////
	// Sequence Window tool button
  tbb[iCount].iBitmap = 20; // account for the separator
  tbb[iCount].idCommand = IDM_V_SEQUENCE;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1; 
  iCount++;

	//////////////////
	// Group Window tool button
  tbb[iCount].iBitmap = 7; // account for the separator
  tbb[iCount].idCommand = IDM_V_GROUP;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1; 
  iCount++;

	//////////////////
  // Zoom Window tool button
  tbb[iCount].iBitmap = 8; // account for the separator
  tbb[iCount].idCommand = IDM_V_FULLZOOM;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1; 
  iCount++;

	//////////////////
  // Full Window tool button
  tbb[iCount].iBitmap = 9; // account for the separator
  tbb[iCount].idCommand = IDM_V_FULLCONSOLE;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1; 
  iCount++;

  // add a separator
  //
  tbb[iCount].iBitmap = -1;
  tbb[iCount].idCommand = 0;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_SEP;// | TBSTYLE_AUTOSIZE;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1; 
  iCount++;

	//////////////////
  // Cancel the Cues
  //
  tbb[iCount].iBitmap = 21; 
  tbb[iCount].idCommand = IDM_V_CANCELCUES;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1; 
  iCount++;

	//////////////////
  // Cancel the Groups
  //
  tbb[iCount].iBitmap = 10; // account for the separator
  tbb[iCount].idCommand = IDM_V_CANCELGROUPS;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1; 
  iCount++;

	////////////////////////////////////////////////
  // Groups Selector starts after this separartor
  //
  tbb[iCount].iBitmap = -1;
  tbb[iCount].idCommand = 0;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_SEP;// | TBSTYLE_AUTOSIZE;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1; 
  iCount++;

	//////////////////
  // Cancel the Groups
  //
	for(i=0; i < 8; i++)
	{
		tbb[iCount].iBitmap = 12 + i; 
		tbb[iCount].idCommand = IDM_V_CANCELGROUPS + i + 1;
		tbb[iCount].fsState = TBSTATE_INDETERMINATE; // TBSTATE_ENABLED
		tbb[iCount].fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
		tbb[iCount].dwData = 0;
		tbb[iCount].iString = -1;  // i
	  iCount++;
	}

  // add a separator
  //
  tbb[iCount].iBitmap = -1;
  tbb[iCount].idCommand = 0;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_SEP;// | TBSTYLE_AUTOSIZE;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1; 
  iCount++;

	//////////////////
	// Number of Zoom windows open
	//////////////////
	iNumZoomWindowsButton = iCount;			// save for later
  tbb[iCount].iBitmap = -1; // account for the separator
  tbb[iCount].idCommand = giTBZoomWinCntReadout;
  tbb[iCount].fsState = TBSTATE_ENABLED;
  tbb[iCount].fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;// | TBSTYLE_DROPDOWN;
  tbb[iCount].dwData = 0;
  tbb[iCount].iString = -1;
  iCount++;

	// Set the number of buttons

	iButtonCount = iCount;

#ifdef ORIGINAL
  ghwndTBPlay = CreateToolbarEx(hwndParent, WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_ALTDRAG | TBSTYLE_LIST ,// | CCS_ADJUSTABLE, 
															  1100, 19, ghInstMain, IDB_PLAYTOOL,(LPCTBBUTTON) &tbb, iButtonCount,
															  16, 14, 16, 14, sizeof(TBBUTTON));
#else
  ghwndTBPlay = CreateToolbarEx(hwndParent, WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_LIST , 
															  IDB_PLAYTOOL, 19, ghInstMain, IDB_PLAYTOOL,(LPCTBBUTTON) &tbb, iButtonCount,
															  16, 14, 16, 14, sizeof(TBBUTTON));

#endif
	/////////////////////////////////////////////////////////
	// now Create the CURRENT SEQUENCE readout window ....
	/////////////////////////////////////////////////////////

	tbinfo.cbSize = sizeof(TBBUTTONINFO);
	tbinfo.dwMask = TBIF_SIZE | TBIF_STYLE;
	tbinfo.fsStyle = TBSTYLE_SEP;
	tbinfo.cx = 185;	// 175	SIZE OF THE SEQUENCE WINDOW

	SendMessage(ghwndTBPlay, TB_SETBUTTONINFO, (WPARAM)giTBSeqReadout, (LPARAM)&tbinfo);

  SetWindowPos(ghwndTBPlay, NULL, 100, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
  SendMessage(ghwndTBPlay, TB_AUTOSIZE, 0, 0);
	
	// Get the size of the sequence button passed in WPARAM

	SendMessage(ghwndTBPlay, TB_GETITEMRECT, (WPARAM)iSeqButton, (LPARAM)&r);
	r.top = 2;

	//////////////////////////////////////////////////
	// Create Sequence window on toolbar
	//////////////////////////////////////////////////

	ghwndTBSeqReadout = CreateWindow("STATIC", "", WS_VISIBLE | WS_CHILD | WS_BORDER,//CBS_DROPDOWNLIST | CBS_HASSTRINGS,
																	r.left, r.top, r.right - r.left, r.bottom - r.top,
																	ghwndTBPlay, NULL, ghInstMain, NULL);
	
	SetWindowText(ghwndTBSeqReadout, "Sequence ..........");


	/////////////////////////////////////////////////////////
	// now Create the NEXT SEQUENCE readout window ....
	/////////////////////////////////////////////////////////

	tbinfo.cbSize = sizeof(TBBUTTONINFO);
	tbinfo.dwMask = TBIF_SIZE | TBIF_STYLE;
	tbinfo.fsStyle = TBSTYLE_SEP;
	tbinfo.cx = 185;	// 175	SIZE OF THE SEQUENCE WINDOW

	SendMessage(ghwndTBPlay, TB_SETBUTTONINFO, (WPARAM)giTBSeqReadout2, (LPARAM)&tbinfo);
	
  SetWindowPos(ghwndTBPlay, NULL, 100, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
  SendMessage(ghwndTBPlay, TB_AUTOSIZE, 0, 0);

	// Get the size of the sequence button passed in WPARAM

	SendMessage(ghwndTBPlay, TB_GETITEMRECT, (WPARAM)iSeqButton2, (LPARAM)&r);
	r.top = 2;

	//////////////////////////////////////////////////
	// Create Sequence window on toolbar
	//////////////////////////////////////////////////

	ghwndTBSeqReadout2 = CreateWindow("STATIC", "", WS_VISIBLE | WS_CHILD | WS_BORDER,//CBS_DROPDOWNLIST | CBS_HASSTRINGS,
																	r.left, r.top, r.right - r.left, r.bottom - r.top,
																	ghwndTBPlay, NULL, ghInstMain, NULL);
	
	SetWindowText(ghwndTBSeqReadout2, "Next Sequence ......");


///////////////////////////////////////////
// Num Zoom Windows display
	
	tbinfo.cbSize = sizeof(TBBUTTONINFO);
	tbinfo.dwMask = TBIF_SIZE | TBIF_STYLE;
	tbinfo.fsStyle = TBSTYLE_SEP;
	tbinfo.cx = 155;	// 175	SIZE OF THE NUM ZOOM WINDOW

	SendMessage(ghwndTBPlay, TB_SETBUTTONINFO, (WPARAM)giTBZoomWinCntReadout, (LPARAM)&tbinfo);

  SetWindowPos(ghwndTBPlay, NULL, 100, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
  SendMessage(ghwndTBPlay, TB_AUTOSIZE, 0, 0);

	// Get the size of the number of zoom windows open button passed in WPARAM

	SendMessage(ghwndTBPlay, TB_GETITEMRECT, (WPARAM)iNumZoomWindowsButton, (LPARAM)&r);
	r.top = 2;

	//////////////////////////////////////////////////
	// Create Number of Zoom Windows on toolbar
	//////////////////////////////////////////////////

	ghwndTBZoomWinCntReadout = CreateWindow("STATIC", "", WS_VISIBLE | WS_CHILD | WS_BORDER,//CBS_DROPDOWNLIST | CBS_HASSTRINGS,
																	r.left, r.top, r.right - r.left, r.bottom - r.top,
																	ghwndTBPlay, NULL, ghInstMain, NULL);
	
	SetWindowText(ghwndTBZoomWinCntReadout, " Zoom Windows: 0");

//////////////////////////////////////////

  ShowWindow(ghwndTBPlay, SW_SHOW);

	/////////////////////////////////////////////////////////////////
	// Set up the Tool Tips for our Tool Bar
	/////////////////////////////////////////////////////////////////

  hToolTips = (HWND)SendMessage(ghwndTBPlay, TB_GETTOOLTIPS, 0, 0);
	if(hToolTips)
	{
		lpToolInfo.cbSize = sizeof(lpToolInfo);
		lpToolInfo.uFlags = TTF_IDISHWND | TTF_CENTERTIP;
		lpToolInfo.lpszText = (LPSTR) LPSTR_TEXTCALLBACK;
		lpToolInfo.hwnd = hwndParent;
		lpToolInfo.uId = (UINT) ghwndTBPlay;
		lpToolInfo.hinst = ghInstMain;
	}
	SendMessage(hToolTips,TTM_ADDTOOL,0,(LPARAM)(LPTOOLINFO)&lpToolInfo);

  SendMessage(hToolTips, TTM_ACTIVATE, (WPARAM)TRUE,0);

	// Set times for the tool tip to pop-up

	SendMessage(hToolTips, TTM_SETDELAYTIME, (WPARAM)TTDT_AUTOPOP, (LPARAM)MAKELONG(40000,0));
	SendMessage(hToolTips, TTM_SETDELAYTIME, (WPARAM)TTDT_INITIAL, (LPARAM)MAKELONG(200,0));
	SendMessage(hToolTips, TTM_SETDELAYTIME, (WPARAM)TTDT_RESHOW, (LPARAM)MAKELONG(200,0));

return iRet;
}

////////////////////////////////////////////////////////////////////////////////////////////
//
// Show the active sequence
//
void ShowTBSeqName(LPSTR pName)
{
	if(ghwndTBSeqReadout)
		SetWindowText(ghwndTBSeqReadout, pName);
}

//
// Show the NEXT  active sequence
//
void ShowTBNextSeqName(LPSTR pName)
{
	if(ghwndTBSeqReadout2)
		SetWindowText(ghwndTBSeqReadout2, pName);
}

// Show the number of zoom windows open

void ShowTBZoomWinCnt(LPSTR pName)
{
	if(ghwndTBZoomWinCntReadout)
		SetWindowText(ghwndTBZoomWinCntReadout, pName);
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
