//=================================================
// file trackingwindow.c
//
// Copyright 1997-2002, CorTek Software, Inc.
//=================================================


#include "SAMM.h"
#include "SAMMEXT.h"
#include "MACRO.h"
#include "ConsoleDefinition.h"
#include "DCXTCP_protocol.h" 

extern int                 g_aiAux[MAX_MATRIX_COUNT];	// See memory_map.c
extern int                 g_iAuxIdx ;

extern LPSTR   GetLabelText(LPCTRLZONEMAP , int );

char    gszTrackWindowClass_Name[] = "VACS_TrackingWindow";
HWND    g_hwndTrack = NULL;
HWND    g_hwndInitImg = NULL;
BOOL    g_bTrackingActive = TRUE;	// Default is ON


LRESULT CALLBACK  TrackingWndProc(HWND , UINT , WPARAM , LPARAM );
void    TW_DrawInformation(LPCTRLZONEMAP, int, int);
void    TW_Paint(void);
int			CheckForSub(LPCTRLZONEMAP );
int			CheckForAux(LPCTRLZONEMAP );

/////////////////////////////////////////
// FUNCTION: RegisterTrackingWindowClass
//
//
// Register the Full View Window Class
//
int       RegisterTrackingWindowClass(void)
{
	int         iReturn;
	WNDCLASS    wc;


	// Register Full View Class
	//--------------------------
	ZeroMemory(&wc, sizeof(WNDCLASS));      // Clear wndclass structure

	wc.style = CS_BYTEALIGNWINDOW ;//| CS_SAVEBITS;//CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)TrackingWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(LPSTR);// all of the data for this window will be stored here
	wc.hInstance = ghInstMain;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = gszTrackWindowClass_Name;

	iReturn = RegisterClass(&wc);

	if(iReturn == 0)
		return(IDS_ERR_REGISTER_CLASS);     // Error... Exit
	return 0;
}


////////////////////////////////////////////////////////////
// FUNCTION:        CreateTrackingWindow
//
// Creates the tracking Window that will show tool tips
//
//
int       CreateTrackingWindow(HWND hwndParent,LPARAM lParam)
{

	// Make sure things are clean .. 
	if(g_hwndTrack)
	{
		DestroyWindow(g_hwndTrack);
		//g_hwndTrack = NULL;
	}

	g_hwndTrack = CreateWindow(
		gszTrackWindowClass_Name,   // Window class name
		NULL,                   // Window's title
		WS_CHILD | WS_OVERLAPPED | WS_CLIPSIBLINGS , // WS_CLIPCHILDREN,
		100,
		100,
		150,         // Set it to the max Width
		45,         // Set it to the max Height
		hwndParent,             // Parent window's handle
		NULL,                   // Default to Class Menu
		ghInstMain,              // Instance of window
		NULL                    // Ptr To Data Structure For WM_CREATE
		);

	if(g_hwndTrack == NULL)
	{
		ErrorBox(ghwndMain, ghInstStrRes,IDS_ERR_MAX_WINDOW);
		return IDS_ERR_CREATE_WINDOW;
	}

	g_bTrackingActive = TRUE;
	//ShowWindow(g_hwndTrack, SW_SHOW);
	//UpdateWindow(g_hwndTrack);
	return 0;
};


//////////////////////////////////////
//
//
BOOL  IsTrackingActive(void)
{
	return g_bTrackingActive;
}

////////////////////////////////////////////////
//
//
void StopTrackingWindow(void)
{
	g_bTrackingActive = FALSE;
	DestroyWindow(g_hwndTrack);
	//g_hwndTrack = NULL;

}


////////////////////////////////////////////////
//
//
void  ActivateTrackingWindow(void)
{
	g_bTrackingActive = TRUE;
}


//===============================
//FUNCTION:FullViewProc
//
//===============================
LRESULT CALLBACK  TrackingWndProc(HWND hWnd, UINT wMessage, 
								  WPARAM wParam, LPARAM lParam)
{
	POINT   pnt;

	switch (wMessage)
	{
		//////////////////////////////////////////////////////////////
	case WM_ERASEBKGND: // to reduce flashing on the screen
		//TW_DrawTrackingWindow();
		return DefWindowProc(hWnd, wMessage, wParam, lParam);
		break;
	case WM_PAINT:
		// fds    TW_Paint();
		break;
		//////////////////////////////////////////////////////////////
	case WM_MOUSEMOVE:
		pnt.x = LOWORD(lParam);
		pnt.y = HIWORD(lParam);
		ClientToScreen(hWnd, &pnt);
		ScreenToClient(g_hwndInitImg, &pnt);
		SetWindowPos(g_hwndTrack, HWND_TOP, pnt.x + 20, pnt.y + 50, 0, 0,
			SWP_SHOWWINDOW /*| SWP_NOZORDER */| SWP_NOSIZE);
		break;
		//////////////////////////////////////////////////////////////
	case WM_COMMAND:
		break;
		//////////////////////////////////////////////////////////////
	case WM_MOVE:
		break;
		//////////////////////////////////////////////////////////////
	case WM_GETMINMAXINFO:
		break;

		//////////////////////////////////////////////////////////////
	case WM_DESTROY:
		g_hwndInitImg = NULL;
		g_hwndTrack = NULL;
	default:
		return DefWindowProc(hWnd, wMessage, wParam, lParam);

	}

	return DefWindowProc(hWnd, wMessage, wParam, lParam);
}


//////////////////////////////////////////////////////////
// Update tracking window position and information
//
//
void  UpdateTrackingWindow(LPMIXERWNDDATA      lpmwd)
{
	RECT    rect;
	RECT    rClient;
	RECT    rTrack;
	POINT   pnt;
	HWND    hwnd;
	int     x, y;
	int							iRackMaxChannel[]={18,32,58,78};							// cabaret, showtime, event 40, event 60

	LPCTRLZONEMAP    lpczm;
	int			iSub, iAux;
	int			iChan=0;

	// Check if user turned it off

	if(g_bTrackingActive && (lpmwd != NULL))
	{

		iSub = iAux = -1;

		// Now get the channel pointer

		iChan = LOBYTE(lpmwd->lpwRemapToScr[lpmwd->iCurChan + lpmwd->iStartScrChan]);

		// Validate channel number, crashed for ->lpZoneMap[iChan].lpZoneMap because 
		// of iChan being out of range

		//		if((giMixerType) && (iChan > iRackMaxChannel[giMixerType-1]))	// giMixerType is base 1
		//		{
		//			iChan = iRackMaxChannel[giMixerType-1];
		//		}

		// Get our mouse coordinates

		pnt = lpmwd->pntMouseCur;

		pnt.x -= lpmwd->iXadj;
		pnt.y += lpmwd->iYOffset;

		// See if we have a zonemap at this point

		lpczm = ScanCtrlZonesPnt(lpmwd->lpZoneMap[iChan].lpZoneMap, pnt);

		// We are over a control/zonemap, if its a SUB or AUX handle differently

		if(lpczm)
		{

			iSub = CheckForSub(lpczm);	// Get Sub channel if over sub
			iAux = CheckForAux(lpczm);	// Get Aux channel if over aux

		}
#ifdef _DEBUG	// Set this if you want tool tips for every control
		//iSub = 1;
		//iAux = 1;
#endif

		// We are only interested in SUBS and AUXS

		if((iSub != -1) || (iAux != -1))
		{
			// Close the Window if there is nothing 
			// to display
			//
			if(lpmwd == NULL)      
			{
				DestroyWindow(g_hwndTrack);
				//g_hwndTrack = NULL;
				return;
			}

			hwnd = lpmwd->hwndImg;

			// Create the Window if it was closed
			//
			if(g_hwndTrack == NULL)
			{
				CreateTrackingWindow(GetParent(lpmwd->hwndImg), 0);
				g_hwndInitImg = lpmwd->hwndImg;
				//CreateTrackingWindow(lpmwd->hwndImg, 0);
			}
			else
			{
				// somewhow we switched Mixer image windows without 
				// destroing this tracking window ....
				// so we need to force this ...
				if(g_hwndInitImg != lpmwd->hwndImg)
				{
					DestroyWindow(g_hwndTrack);
					//g_hwndTrack = NULL;
					CreateTrackingWindow(GetParent(lpmwd->hwndImg), 0);
					g_hwndInitImg = lpmwd->hwndImg;
				}

			}

			GetClientRect(GetParent(g_hwndTrack), &rClient);
			GetClientRect(g_hwndTrack, &rTrack);

			pnt = lpmwd->pntMouseCur;

			// Get the outline of the Current module into rect .... Recomended by Tom
			GetMWScrChanRect(lpmwd, lpmwd->iCurChan, &rect);

			if( (rect.right + rTrack.right) < rClient.right)
				x = rect.right;     
			else
				x = rect.left - rTrack.right;


			if((pnt.y + rTrack.bottom + 5) < rClient.bottom)
				y = pnt.y + 20;
			else
				y = pnt.y - 20;

			// Display the information in out window

			TW_DrawInformation(lpczm,x,y);

		}	// End if iSub and iAux
		else
		{
			if(g_hwndTrack != NULL)
				ShowWindow(g_hwndTrack, SW_HIDE);
		}

	}	// End if tracking window active

}

///////////////////////////////////////////////////////////
//
//
//
void    TW_Paint(void)
{
	HDC     hdc;
	RECT    r, rDraw;
	HBRUSH  hbr, hbrBack;


	GetClientRect(g_hwndTrack, &r);
	hdc = GetDC(g_hwndTrack);

	//  hbr = CreateSolidBrush(RGB(64, 48, 48));
	//  hbrBack = CreateSolidBrush(RGB(232, 232, 232));
	hbr = CreateSolidBrush(GetSysColor(COLOR_INFOTEXT));
	hbrBack = CreateSolidBrush(GetSysColor(COLOR_INFOBK));

	rDraw = r; 
	rDraw.top += 2;
	rDraw.left += 2;
	rDraw.bottom -= 3;
	rDraw.right -= 3;

	FillRect(hdc, &rDraw, hbrBack);

	rDraw = r;  rDraw.bottom = 2;
	FillRect(hdc, &rDraw, hbr);

	rDraw = r;  rDraw.right = 2;
	FillRect(hdc, &rDraw, hbr);

	rDraw = r;  rDraw.top = rDraw.bottom - 3;
	FillRect(hdc, &rDraw, hbr);

	rDraw = r;  rDraw.left = rDraw.right - 3;
	FillRect(hdc, &rDraw, hbr);

	DeleteObject(hbr);
	DeleteObject(hbrBack);
	ReleaseDC(g_hwndTrack, hdc);
}


///////////////////////////////////////////
// Give a zone map the routine will search
// for the Subs on the INPUT
// and return wich sub it is

int CheckForSub(LPCTRLZONEMAP lpczm)
{
	int iSub = -1;

	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"SUB 1",5) == 0)
		iSub=0;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"SUB 2",5) == 0)
		iSub=1;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"SUB 3",5) == 0)
		iSub=2;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"SUB 4",5) == 0)
		iSub=3;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"SUB 5",5) == 0)
		iSub=4;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"SUB 6",5) == 0)
		iSub=5;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"SUB 7",5) == 0)
		iSub=6;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"SUB 8",5) == 0)
		iSub=7;

	// Do not show tool tip for Subs that doen't exist in
	// the current hardware configuration, ie ShowTime

	if(iSub >= g_iAuxIdx)
		iSub = -1;

	return(iSub);
}


///////////////////////////////////////////
// Give a zone map the routine will search
// for the AUXs on the INPUT
// and return wich aux it is


int CheckForAux(LPCTRLZONEMAP lpczm)
{
	int iAux = -1;

	// These are for the AUX's on the INPUT module

	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX1 ",5) == 0)
		iAux=0;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX2 ",5) == 0)
		iAux=1;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX3 ",5) == 0)
		iAux=2;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX4 ",5) == 0)
		iAux=3;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX5 ",5) == 0)
		iAux=4;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX6 ",5) == 0)
		iAux=5;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX7 ",5) == 0)
		iAux=6;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX8 ",5) == 0)
		iAux=7;

	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX9 ",5) == 0)
		iAux=8;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX10 ",6) == 0)
		iAux=9;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX11 ",6) == 0)
		iAux=10;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX12 ",6) == 0)
		iAux=11;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX13 ",6) == 0)
		iAux=12;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX14 ",6) == 0)
		iAux=13;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX15 ",6) == 0)
		iAux=14;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX16 ",6) == 0)
		iAux=15;

	//////////////////////////////////////////////////////////////////////
	// These are for the AUX's on the MATRIX module

	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX 1 ",6) == 0)
		iAux=0;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX 2 ",6) == 0)
		iAux=1;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX 3 ",6) == 0)
		iAux=2;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX 4 ",6) == 0)
		iAux=3;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX 5 ",6) == 0)
		iAux=4;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX 6 ",6) == 0)
		iAux=5;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX 7 ",6) == 0)
		iAux=6;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX 8 ",6) == 0)
		iAux=7;

	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX 9 ",6) == 0)
		iAux=8;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX 10 ",7) == 0)
		iAux=9;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX 11 ",7) == 0)
		iAux=10;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX 12 ",7) == 0)
		iAux=11;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX 13 ",7) == 0)
		iAux=12;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX 14 ",7) == 0)
		iAux=13;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX 15 ",7) == 0)
		iAux=14;
	if(_strnicmp(CDef_GetControlName(lpczm->iCtrlNumAbs),"AUX 16 ",7 ) == 0)
		iAux=15;

	return iAux;
}


////////////////////////////////////////////////////////////////
// void    TW_DrawInformation(LPCTRLZONEMAP    lpczm, int x, int y)
//
// Draws the SUBS and AUXS text in our tracking window
//


void    TW_DrawInformation(LPCTRLZONEMAP    lpczm, int x, int y)
{
	HDC       hdc;
	RECT      r;
	char      szBuff[256];
	int				iSub=-1, iAux=-1;
	LPSTR     pLabel;
	int				iChanSub;
	int				iNumLines = 1;
	SIZE			size;

	if(g_hwndTrack == NULL)
		return;

	// Firat repaint the window
	// and clear previous text

	TW_Paint();

	// We are over a control/zonemap, if its a SUB or AUX handle differently

	if(lpczm)
	{

		iSub = CheckForSub(lpczm);	// Get Sub channel if over sub
		iAux = CheckForAux(lpczm);	// Get Aux channel if over aux

		if(iSub != -1)	// handle subs
		{
			// Get this subs channel
			iChanSub = g_aiAux[iSub];	
			// Get this subs label
			pLabel = &gpLabels[iChanSub * MAX_LABEL_SIZE];    

			// If we get the label then display it, else do the default
			if(pLabel)
				wsprintf(szBuff, "%s ", pLabel);
		}
		else if(iAux != -1)	// handle auxes
		{

			pLabel = &g_pAuxLabels[iAux * MAX_LABEL_SIZE];    

			// If we get the label then display it, else do the default
			if(pLabel)
				wsprintf(szBuff, "%s ", pLabel);
		}
		else
		{
			// **************************               DEBUG ONLY 
			// Use this to determine the control name
			//				wsprintf(szBuff, "CH: %d\n%s", lpczm->iCtrlChanPos,  CDef_GetControlName(lpczm->iCtrlNumAbs));
			//		wsprintf(szBuff, "CH: %d\n%s", iChan+1, &gpLabels[iChan * MAX_LABEL_SIZE]);
			//		iNumLines = 2;
		}

	} // end if lpzm


	// ONLY update over SUBS and AUXS

	if((iSub != -1) || (iAux != -1))
	{
		GetClientRect(g_hwndTrack, &r);
		hdc = GetDC(g_hwndTrack);

		// Get the size of the text we are going to print
		GetTextExtentPoint32(hdc,szBuff,strlen(szBuff),&size);
		size.cy *= iNumLines;

		// Resize and dispaly our tracking window
		SetWindowPos(g_hwndTrack, HWND_TOP, x, y, size.cx+5, size.cy+5, SWP_SHOWWINDOW);

		// Display the text
		WriteTextLinesToDC(hdc, &r, 1, 1, GetSysColor(COLOR_MENUTEXT),  szBuff);

		ReleaseDC(g_hwndTrack, hdc);
	}
}

