//=================================================
// Copyright 1998-2002 CorTek Software, Inc.
//=================================================
//
//
// $Author:: Styck                                $
// $Archive:: /Vacs Client/src/ZoomView.c         $
// $Revision:: 20                                 $
//

//=================================================
// The Zoom View Window
//
//=================================================


#include "SAMM.h"
#include "SAMMEXT.h"
#include "MACRO.h"
#include <stdio.h>

//FDS 6/13/04 #include <zmouse.h>

static int giZoomWndCnt=0;

void ShowTBZoomWinCnt(LPSTR );	// see CreateMain.c
void	HandleRemoteSequenceControl(WORD wControl);

//====================================
// FUNCTION: RegisterZoomViewClass
//
//
// Register the Zoom View Window Class
//====================================
int       RegisterZoomViewClass(void)
{
	int         iReturn;
	WNDCLASS    wc;


	// Register Zoom View Class
	//--------------------------
	ZeroMemory(&wc,  sizeof(WNDCLASS));      // Clear wndclass structure

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)ZoomViewProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(LPSTR);// all of the data for this window will be stored here
	wc.hInstance = ghInstMain;
	wc.hIcon = LoadIcon(ghInstConsoleDef, MAKEINTRESOURCE(ZoomViewIcon));// this might leak memory
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = gszZoomViewClass;

	iReturn = RegisterClass(&wc);

	if(iReturn == 0)
		return(IDS_ERR_REGISTER_CLASS);     // Error... Exit

	return 0;
}

//////////////////////////////////
// FUNCTION: CreateZoomViewWindow
//
//purpouse:
//      Create the window and
// load all of the bitmaps and
// zone tables ...
//
//
HWND       CreateZoomViewWindow(HWND hWnd, LPSTR szTitle, LPMIXERWNDDATA  pMWD, int iType)
{
	RECT                rect;
	LPMIXERWNDDATA      lpmwd;
	DWORD								style;
	char szTempBuff[20];

	if (gInitialized == FALSE)
		return NULL;

	if(pMWD == NULL)	// We are creating a NEW ZOOM VIEW WINDOW
	{
		lpmwd = MixerWindowDataAlloc(gwActiveMixer,
			gpZoneMaps_Zoom,
			MAX_CHANNELS,
			iType);

		if(lpmwd == NULL)
		{
			ErrorBox(ghwndMain, ghInstStrRes, IDS_ERR_ALLOCATE_MEMORY);
			return NULL;
		}
		// store the Window title
		//
		lstrcpy(lpmwd->szTitle, szTitle);
		// make some room for the label Window
		// after all we want this window to appear
		// in it's full size
		//
		lpmwd->rWndPos.bottom -= HEIGHT_FULL_LABEL_WND;
	}
	else	// We are READING a mix file
	{
		lpmwd = pMWD;
		lpmwd->rWndPos.bottom -= HEIGHT_FULL_LABEL_WND;
#ifdef SCROLLBARS
		// FDS		lpmwd->rWndPos.right += GetSystemMetrics(SM_CYVTHUMB);	// allow space for scroll bar
#endif
	}

	lpmwd->lpZoneMapZoom = gpZoneMaps_Full;

	// this makes it possible if the window
	// is streched all the way to fit the entire image
	//
	lpmwd->rMaxWndPos.bottom += HEIGHT_FULL_LABEL_WND;
	lpmwd->rWndPos.bottom += HEIGHT_FULL_LABEL_WND;


	// Set the style for the Zoom Window View
	// WS_MAXIMIZEBOX - allows the window to be maximized, curently disabled
	// WS_VSCROLL - add vertical scroll bar to ZOOM window

	style = MDIS_ALLCHILDSTYLES | WS_CHILD | WS_SYSMENU | WS_CAPTION | WS_VISIBLE
		| WS_MINIMIZEBOX  | WS_THICKFRAME // | WS_MAXIMIZEBOX
		| WS_CLIPSIBLINGS | WS_CLIPCHILDREN;


#ifdef SCROLLBARS
	style = style | WS_VSCROLL;
	// ADJUST for Vertical Scroll Bars
	//	lpmwd->rWndPos.right = lpmwd->rWndPos.right + GetSystemMetrics(SM_CYVTHUMB) ;
#endif

	if(pMWD == NULL)
		hWnd = CreateMDIWindow (
		gszZoomViewClass,
		szTitle,
		style,
		1,//CW_USEDEFAULT,
		1,//CW_USEDEFAULT,
		lpmwd->rWndPos.right,
		lpmwd->rWndPos.bottom,
		ghwndMDIClient,
		ghInstMain,
		(LPARAM)lpmwd
		);
	else		// use the window positions stored in mix file
		hWnd = CreateMDIWindow (
		gszZoomViewClass,
		szTitle,
		style,
		lpmwd->rWndPos.left,
		lpmwd->rWndPos.top,
		lpmwd->rWndPos.right,
		lpmwd->rWndPos.bottom,
		ghwndMDIClient,
		ghInstMain,
		(LPARAM)lpmwd
		);


	// Check if we create the window

	if(hWnd == NULL)
	{
		MixerWindowDataFree(lpmwd);
		ErrorBox(ghwndMain, ghInstStrRes,IDS_ERR_CREATE_WINDOW);
		return NULL;//IDS_ERR_CREATE_WINDOW;
	}
	else
	{
		giZoomWndCnt++;		// increment our zoom window count
		wsprintf(szTempBuff,"Zoom Views: %d",giZoomWndCnt);
		ShowTBZoomWinCnt(szTempBuff);
	}

	//if(pMWD == NULL)
	//    {
	// adjust the Image Window Size to fit
	// perfectly inside of the Client Window
	//--------------------------------------
	lpmwd->rVisible.left = 0;
	lpmwd->rVisible.top = HEIGHT_FULL_LABEL_WND;
	lpmwd->rVisible.bottom -= HEIGHT_FULL_LABEL_WND;

	ImageWindowSize(hWnd, &lpmwd->rVisible, lpmwd);
	//    }


	// Ok we have a window opened
	//
	SetWindowLong(hWnd, 0, (LPARAM)lpmwd);


	// Create the Label-Group display Window
	//
	rect.left = rect.top = 0;
	rect.right = lpmwd->rMaxSize.right;
	rect.bottom = HEIGHT_FULL_LABEL_WND;
	lpmwd->wWndType = WND_GROUPLBL_ZOOM;//WND_GROUPLBL_FULL;

	if(CreateLblGroupWnd(&rect, hWnd, lpmwd) == NULL)
	{
		ErrorBox(ghwndMain, ghInstStrRes,IDS_ERR_CREATE_WINDOW);
		PostMessage(ghwndMDIClient, WM_MDIDESTROY, (WPARAM)hWnd, 0L);
		return NULL;//IDS_ERR_CREATE_WINDOW;
	}


	// now create the Image window
	// and continue
	// and if something goes wrong
	// destroy the Window
	//
	if(CreateFullViewImageWindow(hWnd, (LPARAM)lpmwd))
		PostMessage(ghwndMDIClient, WM_MDIDESTROY, (WPARAM)hWnd, 0L);

	SetFocus(hWnd);
	//SendMessage (ghwndMDIClient, WM_MDIACTIVATE, (WPARAM)hWnd, 0L);

#ifdef SCROLLBARS
	// Setup the Scroll bars
	SetScrollRange(hWnd, SB_VERT, 0, 3950, FALSE);							// 3950, 3378
	SetScrollPos(hWnd, SB_VERT, 1975, TRUE); 
	//		SetScrollPos(hWnd, SB_VERT, lpmwd->iYOffset, TRUE); 

#endif

	return hWnd;
};



#ifdef NOTUSED

////////////////////////////////
// FUNCTION: OpenZoomViewWindow
//
//purpouse:
//      Create the window and
// load all of the bitmaps and
// zone tables ...
//
HWND       OpenZoomViewWindow(LPMIXERWNDDATA lpmwd, LPSTR szTitle)
{
	HWND                hWnd;
	RECT                rect;


	hWnd = CreateMDIWindow (
		gszZoomViewClass,
		szTitle,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		1,//CW_USEDEFAULT,
		lpmwd->rWndPos.right,
		lpmwd->rWndPos.bottom,
		ghwndMDIClient,
		ghInstMain,
		(LPARAM)lpmwd
		);
	if(hWnd == NULL)
	{
		MixerWindowDataFree(lpmwd);
		ErrorBox(ghwndMain, ghInstStrRes,IDS_ERR_CREATE_WINDOW);
		return NULL;
	}

	// adjust the Image Window Size to fit
	// perfectly inside of the Client Window
	//--------------------------------------
	lpmwd->rVisible.left = 0;
	lpmwd->rVisible.top = HEIGHT_FULL_LABEL_WND;
	lpmwd->rVisible.bottom -= HEIGHT_FULL_LABEL_WND;

	ImageWindowSize(hWnd, &lpmwd->rVisible, lpmwd);



	// Ok we have a window opened
	//---------------------------
	SetWindowLong(hWnd, 0, (LPARAM)lpmwd);


	// Create the Label-Group display Window
	//--------------------------------------
	rect.left = rect.top = 0;
	rect.right = lpmwd->rMaxSize.right;
	rect.bottom = HEIGHT_FULL_LABEL_WND;
	lpmwd->wWndType = WND_GROUPLBL_ZOOM;//WND_GROUPLBL_FULL;

	if(CreateLblGroupWnd(&rect, hWnd, lpmwd) == NULL)
	{
		ErrorBox(ghwndMain, ghInstStrRes,IDS_ERR_CREATE_WINDOW);
		PostMessage(ghwndMDIClient, WM_MDIDESTROY, (WPARAM)hWnd, 0L);
		return NULL;
	}

	//----------------------------
	// now create the Image window
	// and continue
	// and if something goes wrong
	// destroy the Window
	//----------------------------

	if(CreateFullViewImageWindow(hWnd, (LPARAM)lpmwd))
		PostMessage(ghwndMDIClient, WM_MDIDESTROY, (WPARAM)hWnd, 0L);

	SetFocus(hWnd);

	return hWnd;
};

#endif


/////////////////////////////////////////////
//FUNCTION:ZoomViewProc
//
//
//
// Handle messages for the Zoom View Windows
//

#define	PAGEUPDOWN_OFFSET	rect.bottom - rect.top - HEIGHT_FULL_LABEL_WND;	//  765	// Number of pixels to pageup/down
#define	LINEUPDOWN_OFFSET	25		// Number of pixels to lineup/down

LRESULT CALLBACK  ZoomViewProc(HWND hWnd, UINT wMessage, 
							   WPARAM wParam, LPARAM lParam)
{
	MINMAXINFO FAR      *lpMMI;
	LPMIXERWNDDATA      lpmwd;
	RECT                rect;
	char								szTempBuff[20];
	int nVscrollInc;	// Dummy variable
	//int iPos;
	char            szBuff[128];

	static cyClient, cxClient, nVscrollMax, nVscrollPos;

	lpmwd = (LPMIXERWNDDATA)GetWindowLong(hWnd,0);

	switch (wMessage)
	{

#ifdef SCROLLBARS

		case WM_VSCROLL: 

			GetWindowRect(hWnd, &rect);
			//      iPos = GetScrollPos (hWnd, SB_VERT); 

			/* Determine how much to scroll vertically. 
			*/ 
			switch (LOWORD(wParam)) 
			{ 
			case SB_TOP: 
				nVscrollInc = -nVscrollPos; 
				break; 

			case SB_BOTTOM: 
				nVscrollInc = nVscrollMax - nVscrollPos; 
				break; 

			case SB_LINEUP: 
				nVscrollInc = -LINEUPDOWN_OFFSET;

				if (lpmwd && lpmwd->iCurMode == MW_NOTHING_MODE)
				{
					//						lpmwd->pntMouseLast = lpmwd->pntMouseCur;
					//							lpmwd->pntMouseCur.y -= LINEUPDOWN_OFFSET;	// Change the current mouse position
					//						if (lpmwd->pntMouseCur.y < 0)
					//							lpmwd->pntMouseCur.y = 0;

					ScrollImgWindow(lpmwd->hwndImg, lpmwd, nVscrollInc);		// Scroll to window to the new position
					//						lpmwd->pntMouseLast = lpmwd->pntMouseCur;	// restore original mouse coordinates
				}
				break; 

			case SB_LINEDOWN: 

				nVscrollInc = LINEUPDOWN_OFFSET;

				if (lpmwd && lpmwd->iCurMode == MW_NOTHING_MODE)
				{
					//						lpmwd->pntMouseLast = lpmwd->pntMouseCur;
					//						lpmwd->pntMouseCur.y += LINEUPDOWN_OFFSET;	// this is the number of pixels we move down
					//						if (lpmwd->pntMouseCur.y > 3950)
					//							lpmwd->pntMouseCur.y = 3950;		// set to the TOP

					ScrollImgWindow(lpmwd->hwndImg, lpmwd, nVscrollInc);		// Scroll to window to the new position
					//						lpmwd->pntMouseLast = lpmwd->pntMouseCur;	// restore original mouse coordinates
				}
				break; 

			case SB_PAGEUP:

				nVscrollInc = min(-1,-cyClient);	// Page up the window size

				if (lpmwd && lpmwd->iCurMode == MW_NOTHING_MODE)
				{
					//						lpmwd->pntMouseLast = lpmwd->pntMouseCur;
					//						lpmwd->pntMouseCur.y += nVscrollInc; //PAGEUPDOWN_OFFSET;	// Change the current mouse position

					ScrollImgWindow(lpmwd->hwndImg, lpmwd, nVscrollInc);		// Scroll to window to the new position
					//						lpmwd->pntMouseLast = lpmwd->pntMouseCur;	// restore original mouse coordinates
				}
				break; 

			case SB_PAGEDOWN: 

				nVscrollInc = max(1, cyClient);	// Page down the window size

				if (lpmwd && lpmwd->iCurMode == MW_NOTHING_MODE)
				{
					//						lpmwd->pntMouseLast = lpmwd->pntMouseCur;
					//						lpmwd->pntMouseCur.y += nVscrollInc;		// PAGEUPDOWN_OFFSET;	// this is the number of pixels we move down

					ScrollImgWindow(lpmwd->hwndImg, lpmwd, nVscrollInc);		// Scroll to window to the new position
					//						lpmwd->pntMouseLast = lpmwd->pntMouseCur;	// restore original mouse coordinates
				}
				break; 

			case SB_THUMBTRACK: 
				nVscrollInc = 7; 
				break; 

			default: 
				g_iYSpeed = 0;
				nVscrollInc = 0; 
			} 

			//				if ( nVscrollInc = max(-nVscrollPos,min(nVscrollInc, nVscrollMax - nVscrollInc)) ) 
			if ( nVscrollInc !=0 ) 
			{
				nVscrollPos += nVscrollInc;

				if(nVscrollPos > nVscrollMax)
					nVscrollPos = nVscrollMax;
				if(nVscrollPos < 0)
					nVscrollPos = 0;

				//					if (lpmwd && lpmwd->iCurMode == MW_NOTHING_MODE)
				//						SetScrollPos(hWnd,SB_VERT,lpmwd->iYOffset,TRUE);
				//					else
				//						SetScrollPos(hWnd,SB_VERT,nVscrollPos,TRUE);
			}

			break;
#endif

			//////////////////////////////////////////////////////////////
		case WM_ERASEBKGND: // to reduce flashing on the screen
			break;

			//////////////////////////////////////////////////////////////
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			default:
				return DefMDIChildProc(hWnd, wMessage, wParam, lParam);
			}
			break;

			//////////////////////////////////////////////////////////////
			// lpmwd-wKeyFlags is sent to the ScrollSideWays() routine and
			// it checks to see if it is set and scrolls only one module
			// at a time if it is.

		case WM_KEYUP:
			switch(wParam)
			{
			case VK_LEFT:
				ScrollSideWays(hWnd, lpmwd, LEFT);  // Scrolling left
				break;
			case VK_RIGHT:
				ScrollSideWays(hWnd, lpmwd, RIGHT);   // Scrollnig right
				break;
			case VK_UP:
				ScrollSideWays(hWnd, lpmwd, FIRST_INPUT);   // Scrollnig right
				break;
			case VK_DOWN:
				ScrollSideWays(hWnd, lpmwd, FIRST_AUX);   // Scrollnig right
				break;
			case VK_SHIFT:
				lpmwd->wKeyFlags &= ~VK_SHIFT;
				break;
			case VK_SPACE:
				if(lpmwd->wKeyFlags & VK_SHIFT)					// if shift key down then go backwards in sequence
					HandleRemoteSequenceControl(IDM_S_BACK);
				else
					HandleRemoteSequenceControl(IDM_S_NEXT);
				break;
			}
			break;                              
			//
		case WM_KEYDOWN:
			switch(wParam)
			{
			case VK_SHIFT:
				lpmwd->wKeyFlags |= VK_SHIFT;
				break;
			}
			break;
			//////////////////////////////////////////////////////////////
			//case WM_MDIACTIVATE:
			//    break;
			//////////////////////////////////////////////////////////////
		case WM_SIZE:

#ifdef SCROLLBARS
			cxClient = LOWORD(lParam);
			cyClient = HIWORD(lParam);
			cyClient = cyClient - HEIGHT_FULL_LABEL_WND; 

			nVscrollMax = 3950;

			//					nVscrollPos = min (nVscrollPos, nVscrollMax) ; 
			SetScrollRange(hWnd,SB_VERT,0,nVscrollMax, FALSE);
			//					SetScrollPos(hWnd, SB_VERT,nVscrollPos, TRUE);
#endif
			HandleWndSize(hWnd, lpmwd, LOWORD(lParam), HIWORD(lParam), wParam);
			break;
			//////////////////////////////////////////////////////////////
		case WM_GETMINMAXINFO:
			lpMMI = (MINMAXINFO FAR*)lParam;
			// get the information for this window
			//------------------------------------
			if(lpmwd)
			{
				lpMMI->ptMinTrackSize.x = lpmwd->rMaxWndPos.left;
				lpMMI->ptMinTrackSize.y = lpmwd->rMaxWndPos.top;
				lpMMI->ptMaxTrackSize.x = lpmwd->rMaxWndPos.right;
				lpMMI->ptMaxTrackSize.y = lpmwd->rMaxWndPos.bottom;
			}
			break;
			//////////////////////////////////////////////////////////////
		case WM_MOVE:
			if(lpmwd)
			{
				GetWindowRect(hWnd, &rect);
				ScreenToClient(ghwndMDIClient, (LPPOINT)&rect.left);
				lpmwd->rWndPos.left = rect.left;
				lpmwd->rWndPos.top  = rect.top;
			}

			break;
			//////////////////

#ifdef NOTWORKING

		case WM_MOUSEWHEEL:
			{
				int zDelta = (short)HIWORD(wParam);

				if (lpmwd && lpmwd->iCurMode == MW_NOTHING_MODE)
				{
					lpmwd->pntMouseLast = lpmwd->pntMouseCur;
					if (zDelta < 0 )
					{
						nVscrollInc = 50;
						//						lpmwd->pntMouseCur.y += nVscrollInc;
						//						if (lpmwd->pntMouseCur.y > 3950)	// 3950 or 3378
						//							lpmwd->pntMouseCur.y = 3950;		// set to the TOP
					}
					else
					{
						nVscrollInc = -50;
						//						lpmwd->pntMouseCur.y += nVscrollInc;
						//						if (lpmwd->pntMouseCur.y < 0)
						//							lpmwd->pntMouseCur.y = 0;
					}

					//					ScrollImgWindow(lpmwd->hwndImg, lpmwd,(int)(lpmwd->pntMouseCur.y - lpmwd->pntMouseLast.y));
					ScrollImgWindow(lpmwd->hwndImg, lpmwd,nVscrollInc);
					//					lpmwd->pntMouseLast = lpmwd->pntMouseCur;

#ifdef SCROLLBARSNOTUSEDYET
					if ( nVscrollInc !=0 ) 
					{
						nVscrollPos += nVscrollInc;

						if(nVscrollPos > nVscrollMax)
							nVscrollPos = nVscrollMax;

						if(nVscrollPos < 0)
							nVscrollPos = 0;

						sprintf(szBuff, "      nVscrollPos = %d ", nVscrollPos);
						SendMessage(ghwndStatus, SB_SETTEXT, MAKEWPARAM(0,SBT_POPOUT), (LPARAM)szBuff);

						//						SetScrollPos(hWnd,SB_VERT,nVscrollInc + lpmwd->iYOffset + lpmwd->rVisible.bottom,TRUE);
						//						SetScrollPos(hWnd,SB_VERT,nVscrollPos+lpmwd->rVisible.bottom,TRUE);
					}
#endif

				}

			}
			break;
#endif		// notworking


			//////////////////////////////////////////////////////////////
		case WM_DESTROY:
			if(ghwndZoom == hWnd)
			{
				ghwndZoom = NULL;
			}
			giZoomWndCnt--;		// decrement zoom window count and display it
			wsprintf(szTempBuff,"Zoom Views: %d",giZoomWndCnt);
			ShowTBZoomWinCnt(szTempBuff);				

			//      FALL THRU

		default:
			return DefMDIChildProc(hWnd, wMessage, wParam, lParam);

	}

	return 0;
}

///////////////////////////////////////////////////////////////////
//
//			Special Stereo Cue Meters Window
//
//

int       RegisterStereoCueMetersViewClass(void)
{
	int         iReturn;
	WNDCLASS    wc;

	// Register Zoom View Class
	//--------------------------
	ZeroMemory(&wc,  sizeof(WNDCLASS));      // Clear wndclass structure


	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)StereoCueMetersViewProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;// all of the data for this window will be stored here
	wc.hInstance = ghInstMain;
	wc.hIcon = NULL;// this might leak memory
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetStockObject(GRAY_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = gszStereoCueMetersClass;

	iReturn = RegisterClass(&wc);

	if(iReturn == 0)
		return(IDS_ERR_REGISTER_CLASS);     // Error... Exit

	return 0;
}

////////////////////////////////////////////////////////////
//
//
char			g_StereoCueMetersTitle[] = "Cue Meters";
HWND			g_stereoCueMetersWindow = NULL;
HBITMAP		g_bmpStereoCueMeters = NULL;
BITMAP		g_bmpInfoSCM;
HDC				g_hdcSCM = NULL;
HDC				g_hdcVUoffscreen = NULL;
HBITMAP		g_bmpVU2;
BITMAP		g_bmpInfoVU2;
HBITMAP		g_bmpCueVu;


///////////////////////////////////////////////////////////////////
//
// Function: showStereoCueMetersView()
//
// Purpose: create the Stereo Cue VU meter window, save handle in
//					global variable g_stereoCueMetersWindow so that we can
//					hide and show window when CUE's are active
//					see ShowStereoCueMetersWindow()
//
int showStereoCueMetersView (void)
{
	HWND	hwnd;
	long	style;
	HDC		hdc;


	g_bmpStereoCueMeters = LoadBitmap (ghInstConsoleDef, MAKEINTRESOURCE(IDB_STEREO_CUE_METERS));
	GetObject(g_bmpStereoCueMeters, sizeof(BITMAP), &g_bmpInfoSCM);

	g_bmpVU2 = LoadBitmap (ghInstConsoleDef, MAKEINTRESOURCE(IDB_VU_DISPLAY2));
	GetObject(g_bmpVU2, sizeof(BITMAP), &g_bmpInfoVU2);

	// load the resources we need to display this VU
	//

	hwnd = CreateWindow (
		gszStereoCueMetersClass,
		"L CUE R",
		WS_POPUP  | WS_CAPTION,
		100,//CW_USEDEFAULT,
		100,//CW_USEDEFAULT,
		g_bmpInfoSCM.bmWidth + 
		GetSystemMetrics(SM_CXBORDER)*3,
		g_bmpInfoSCM.bmHeight + 
		GetSystemMetrics(SM_CYCAPTION) +
		GetSystemMetrics(SM_CYBORDER)*3,
		NULL, //ghwndMain,
		(HMENU) NULL,
		ghInstMain,
		(LPARAM) NULL
		);

	if(hwnd == NULL)
	{
		ErrorBox(ghwndMain, ghInstStrRes,IDS_ERR_CREATE_WINDOW);
	}

	g_stereoCueMetersWindow = hwnd;	// Set Global handle to Cue VU Meter window

	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	style = GetWindowLong (hwnd, GWL_EXSTYLE) | WS_EX_TOPMOST;
	SetWindowLong (hwnd, GWL_EXSTYLE, style);

	hdc = GetDC (hwnd);
	g_hdcSCM = CreateCompatibleDC (hdc);
	g_hdcVUoffscreen = CreateCompatibleDC (hdc);
	g_bmpCueVu = CreateCompatibleBitmap (hdc, 25, 300);
	ReleaseDC (hwnd, hdc);

	SelectObject (g_hdcSCM, g_bmpStereoCueMeters);
	SelectObject (g_hdcVUoffscreen, g_bmpCueVu);

	SetActiveWindow (ghwndMain);

	return 0; // IDS_ERR_CREATE_WINDOW;
}


///////////////////////////////////////////////////////////////////
//
// Function: ShowStereoCueMetersWindow()
//
// Purpose: Show/Hide the Stereo Cue VU Meter window
//
// Inputs: TRUE - show Cue VU window, FALSE - hide Cue Vu Window
//

int			g_leftPosSCM = -1;
int			g_topPosSCM = -1;
HDC			g_memoryCueDC = NULL;

void		ShowStereoCueMetersWindow (BOOL show)
{
	RECT		r;

	if (show)
	{
		// Calculate the initial position
		if(g_leftPosSCM == -1 || g_topPosSCM == -1)
		{
			GetClientRect (ghwndMDIClient, &r);
			ClientToScreen (ghwndMDIClient, (LPPOINT)&r);
			ClientToScreen (ghwndMDIClient, (LPPOINT)&r.right);

			g_leftPosSCM = r.right - (g_bmpInfoSCM.bmWidth + 
				GetSystemMetrics(SM_CXBORDER)*3);	

			g_topPosSCM = r.top + ((r.bottom - r.top)/2) - 
				((g_bmpInfoSCM.bmHeight + 
				GetSystemMetrics(SM_CYCAPTION) +
				GetSystemMetrics(SM_CYBORDER)*3)/2);

			SetWindowPos (g_stereoCueMetersWindow, NULL, g_leftPosSCM, g_topPosSCM, 0, 0, SWP_NOSIZE); 

		}

		ShowWindow (g_stereoCueMetersWindow, SW_SHOW);
		UpdateWindow(g_stereoCueMetersWindow); 
		InvalidateRect (g_stereoCueMetersWindow, NULL, FALSE); 
	}
	else
	{
		ShowWindow (g_stereoCueMetersWindow, SW_HIDE);
	}
}

///////////////////////////////////////////////////////////////////
//
// Function: StereoCueMetersViewProc()
//
// Purpose: Process messages for the Stereo CUE VU Window
//

LRESULT CALLBACK  StereoCueMetersViewProc(HWND hWnd, UINT wMessage, 
										  WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps; 
	HDC				 hdc; 

	switch (wMessage) { 

	case WM_DISPLAYCHANGE: // Only comes through on plug'n'play systems 
		{ 
			SIZE  szScreen; 
			DWORD dwBitsPerPixel = (DWORD)wParam; 

			szScreen.cx = LOWORD(lParam); 
			szScreen.cy = HIWORD(lParam); 

		} 
		break; 
		// 					 
	case WM_ERASEBKGND:
		break;

	case WM_PAINT: 
		hdc = BeginPaint (hWnd, &ps); 
		BitBlt (hdc, 0, 0, g_bmpInfoSCM.bmWidth, g_bmpInfoSCM.bmHeight, g_hdcSCM, 0, 0, SRCCOPY);
		EndPaint (hWnd, &ps); 
		break; 

	case WM_LBUTTONDOWN:				  
		PostMessage(hWnd, WM_MOVE, 0, lParam);
		break;

	case WM_ACTIVATE:
		// force the first MDI child window
		//
		if (LOWORD(wParam) != 0)
			SetActiveWindow (ghwndMain);
		//SetActiveWindow (GetTopWindow(ghwndMDIClient));


		break;

	case WM_DESTROY: 
		DeleteObject (g_bmpStereoCueMeters);
		DeleteObject (g_bmpCueVu);

		DeleteDC (g_hdcSCM);
		DeleteDC (g_hdcVUoffscreen);
		// Tell WinHelp we don't need it any more... 
		//WinHelp (hWnd, APPNAME".HLP", HELP_QUIT,(DWORD)0); 
		PostQuitMessage(0); 
		break; 

	default: 
		return (DefWindowProc(hWnd, wMessage, wParam, lParam)); 
	} 
	return (0); 
}

///////////////////////////////////////////////////////////////////
//
// Function: DisplayStereoCueVUData()
//
// Purpose: Display the VU data for the Stereo CUE VU window
//

void	DisplayStereoCueVUData(VU_READ *pVuDataBuffer)
{
	HDC		hdc, hdcLocal;
	int		iVal, iTop, iHeight, iWidth;
	HBITMAP	oldBmp;


	if (!IsWindowVisible (g_stereoCueMetersWindow))
		return;

	iHeight = g_bmpInfoVU2.bmHeight;
	iWidth  = g_bmpInfoVU2.bmWidth;

	hdc = GetDC (g_stereoCueMetersWindow);
	hdcLocal = CreateCompatibleDC(hdc);
	oldBmp = SelectObject (hdcLocal, g_bmpVU2);

	iVal = pVuDataBuffer->wVUValue[0];
	// Calculate the Vertical Position
	//--------------------------------
	if(iVal > 4095)iVal = 4095;
	if(iVal < 0)iVal = 0;
	iVal = gVU_VertDispTable[iVal];
	iTop = iVal;

	//BitBlt(hdc, 27, 9, iWidth/2, iHeight, g_hdcSCM, 27, 9, SRCCOPY);
	//BitBlt(hdc, 27, 9 + iHeight - iVal, iWidth/2, iVal, hdcLocal, 0, iHeight - iVal, SRCCOPY);
	BitBlt(g_hdcVUoffscreen, 0, 0, iWidth/2, iHeight, g_hdcSCM, 27, 9, SRCCOPY);
	BitBlt(g_hdcVUoffscreen, 0, 0 + iHeight - iVal, iWidth/2, iVal, hdcLocal, 0, iHeight - iVal, SRCCOPY);

	iVal = pVuDataBuffer->wVUValue[1];
	// Calculate the Vertical Position
	//--------------------------------
	if(iVal > 4095)iVal = 4095;
	if(iVal < 0)iVal = 0;
	iVal = gVU_VertDispTable[iVal];
	iTop = iVal;

	//BitBlt(hdc, 27 + iWidth/2, 9, iWidth/2, iHeight, g_hdcSCM, 27, 9, SRCCOPY);
	//BitBlt(hdc, 27 + iWidth/2, 9 + iHeight - iVal, iWidth/2, iVal, hdcLocal, 0, iHeight - iVal, SRCCOPY);
	BitBlt(g_hdcVUoffscreen, iWidth/2, 0, iWidth/2, iHeight, g_hdcSCM, 27, 9, SRCCOPY);
	BitBlt(g_hdcVUoffscreen, iWidth/2, 0 + iHeight - iVal, iWidth/2, iVal, hdcLocal, 0, iHeight - iVal, SRCCOPY);

	// go to the screen
	BitBlt(hdc, 27, 9, iWidth, iHeight, g_hdcVUoffscreen, 0, 0, SRCCOPY);



	iVal = pVuDataBuffer->wVUValue[2];
	// Calculate the Vertical Position
	//--------------------------------
	if(iVal > 4095)iVal = 4095;
	if(iVal < 0)iVal = 0;
	iVal = gVU_VertDispTable[iVal];
	iTop = iVal;

	//BitBlt(hdc, 51, 9, iWidth/2, iHeight, g_hdcSCM, 51, 9, SRCCOPY);
	//BitBlt(hdc, 51, 9 + iHeight - iVal, iWidth/2, iVal, hdcLocal, 0, iHeight - iVal, SRCCOPY);
	BitBlt(g_hdcVUoffscreen, 0, 0, iWidth/2, iHeight, g_hdcSCM, 51, 9, SRCCOPY);
	BitBlt(g_hdcVUoffscreen, 0, 0 + iHeight - iVal, iWidth/2, iVal, hdcLocal, 0, iHeight - iVal, SRCCOPY);

	iVal = pVuDataBuffer->wVUValue[3];
	// Calculate the Vertical Position
	//--------------------------------
	if(iVal > 4095)iVal = 4095;
	if(iVal < 0)iVal = 0;
	iVal = gVU_VertDispTable[iVal];
	iTop = iVal;

	//BitBlt(hdc, 51 + iWidth/2, 9, iWidth/2, iHeight, g_hdcSCM, 51, 9, SRCCOPY);
	//BitBlt(hdc, 51 + iWidth/2, 9 + iHeight - iVal, iWidth/2, iVal, hdcLocal, 0, iHeight - iVal, SRCCOPY);
	BitBlt(g_hdcVUoffscreen, iWidth/2, 0, iWidth/2, iHeight, g_hdcSCM, 51, 9, SRCCOPY);
	BitBlt(g_hdcVUoffscreen, iWidth/2, 0 + iHeight - iVal, iWidth/2, iVal, hdcLocal, 0, iHeight - iVal, SRCCOPY);

	// go to the screen
	BitBlt(hdc, 51, 9, iWidth, iHeight, g_hdcVUoffscreen, 0, 0, SRCCOPY);


	SelectObject (hdcLocal, oldBmp);
	DeleteDC (hdcLocal);
	ReleaseDC (g_stereoCueMetersWindow, hdc);
}

