//=================================================
// Copyright 1998-2002, CorTek Software, Inc.
//=================================================
//
//
// $Author: Styck $
// $Archive: /Vacs Client/src/Events interface.c $
// $Revision: 39 $
//

//=================================================
// Mouse releated Routines
// Button handling - updating all windows
//                   and sending over network
//=================================================

#include "SAMM.h"
#include "SAMMEXT.h"
#include "MACRO.h"

#include "consoledefinition.h"


extern int                 g_aiAux[MAX_MATRIX_COUNT];
extern int                 g_aiMatrix[MAX_MATRIX_COUNT];
extern int                 g_iCueModuleIdx;
extern int                 g_iAuxIdx;
extern int                 g_iMasterModuleIdx;

extern BOOL									g_bIsSoloCueMode; // see MAIN.C

extern BOOL UpdateFromNetwork(int , LPCTRLZONEMAP );	// see ControlDataFilters.cpp


void HandleMasterCueSwitch(LPMIXERWNDDATA lpmwd, WORD wVal);

void TurnOffAllVUforMixerWindow(LPMIXERWNDDATA lpmwd);
void  CheckForSpecialFilters(/*LPMIXERWNDDATA lpwmd, */LPCTRLZONEMAP pctrlzm);
void	syncInputPriority (LPCTRLZONEMAP pctrlzm, int	icount, LPMIXERWNDDATA lpmwd);
void	CancelAllCues (HWND);	// see ControlDataFilters.c
BOOL IsCancellingCues(void);	// see ControlDataFilters.c

BOOL  IsCtrlCueButton(int,int);
BOOL  IsCtrlPrePostFilter(int);

//================================================
//FUNCTION: HandleMouseMove
//
// Whenever the mouse is over a Window, this
// routine is called.  It checkes lpmwd->iCurMode
// to determine what to do. The following are
// valid modes:
//
//    MW_NOTHING_MODE               Nothing
//    MW_DRAGDROP_MODE              Dragging/Dropping
//    MW_CONTROL_ACTIVE             Active control - button, etc.
//    MW_SCROLL                     Scrolling 
//    MW_SCROLL_RELATIVE            Scrolling Relative within the zoom/full view windows

//================================================

void    HandleMouseMove(HWND hwnd,POINTS pnts, WPARAM wKeyFlags,
						LPMIXERWNDDATA lpmwd)
{
	int     iChan;
	POINT   pnt;
	int iCtrlMode;

	WaitForSingleObject(gDisplayEvent, 90);

	if(lpmwd == NULL)
		return;

	pnt.x = pnts.x;
	pnt.y = pnts.y;

	lpmwd->wKeyFlags = wKeyFlags;		// wKeyFlags passed above

	if(lpmwd->iCurMode == MW_SCROLL)
	{
		lpmwd->pntMouseLast = lpmwd->pntMouseCur;
		lpmwd->pntMouseCur = pnt;
		ScrollImgWindow(hwnd, lpmwd,(int)(lpmwd->pntMouseCur.y - lpmwd->pntMouseLast.y));
		SetEvent(gDisplayEvent);
		return;
	}

	////////////////////////////////////////
	// Right mouse button is down and user
	// is scrolling up/down in the zoom/full
	// view windows.

	if(lpmwd->iCurMode == MW_SCROLL_RELATIVE)
	{
		lpmwd->pntMouseLast = pnt;
		ScrollImgWindowRelative(hwnd, lpmwd);
		SetEvent(gDisplayEvent);
		return;
	}

	// If we are in some sort of mode
	// then call the function that
	// handles this mode
	//-------------------------------

	iCtrlMode = lpmwd->iCtrlMode;
	if(iCtrlMode != MW_NOTHING_MODE)
	{
		switch(iCtrlMode)
		{
		case    CTRL_TYPE_FADER_VERT:
			HandleFaderMoveVert(hwnd, pnt, wKeyFlags, lpmwd);
			break;
		case    CTRL_TYPE_FADER_HORZ:
			HandleFaderMoveHorz(hwnd, pnt, wKeyFlags, lpmwd);
			break;
		}

		////////////////////////////////
		// save the last mouse position
		// and then store the new one
		//-----------------------------
		lpmwd->pntMouseLast = lpmwd->pntMouseCur;
		lpmwd->pntMouseCur = pnt;
		SetEvent(gDisplayEvent);

		return;
	}

	///////////////////////////////
	// save the last mouse position
	// and then store the new one
	//-----------------------------
	lpmwd->pntMouseLast = lpmwd->pntMouseCur;
	lpmwd->pntMouseCur = pnt;
	iChan = GetMWScrChanNum(lpmwd);

	if(iChan != lpmwd->iCurChan)
	{
		//WaitForSingleObject(gDisplayEvent, 90);
		lpmwd->iLastChan = lpmwd->iCurChan;
		lpmwd->iCurChan = iChan;
		// Show the mouse position only if the channel has changed
		//--------------------------------------------------------
		ShowMousePos(hwnd, lpmwd);
		//SetEvent(gDisplayEvent);
	}

	UpdateTrackingWindow(lpmwd);
	SetEvent(gDisplayEvent);

	return;
}

//================================================
//FUNCTION: HandleLBDown
//
// When the left mouse button is pressed down this
// routine is called to scan our zonemapping to 
// determine if a control is being activated. If
// the control is being activated then the iCurMode
// is set to MW_CONTROL_ACTIVE to signal further
// processing else where depending on the type
// of control (fader, button, etc)
//================================================

void    HandleLBDown(HWND hwnd, POINTS pnts, WPARAM wKeyFlags,
					 LPMIXERWNDDATA lpmwd)
{
	int                 iChan;
	LPCTRLZONEMAP       lpczm;
	POINT               pnt;
	LPCTRLZONEMAP				lpczmChan;

	if(lpmwd == NULL)
		return;

	lpmwd->wKeyFlags = wKeyFlags;

	if(lpmwd->iCurMode != MW_NOTHING_MODE)
		return;


	pnt.x = pnts.x;
	pnt.y = pnts.y;

	SetCapture(hwnd);

	iChan = LOBYTE(lpmwd->lpwRemapToScr[lpmwd->iCurChan + 
		lpmwd->iStartScrChan]); //lpmwd->iCurChan;

	// adjust the point to be usable with the
	// zone map. So we can scan
	//---------------------------------------
	pnt.x -= lpmwd->iXadj;
	pnt.y += lpmwd->iYOffset;


	// Scan for Controls
	//------------------
	lpczmChan = lpmwd->lpZoneMap[iChan].lpZoneMap;

	if(lpczmChan != NULL)
	{

		lpczm = ScanCtrlZonesPnt(lpczmChan, pnt);

		// Check the CTRL flag
		//---------------------
		if(wKeyFlags & MK_CONTROL)
		{
			lpmwd->iCurMode = MW_DRAGDROP_MODE;
		}
		else
		{
			if(lpczm)
			{
				lpmwd->lpCtrlZM = lpczm;
				lpmwd->iCurMode = MW_CONTROL_ACTIVE;	// If its part of our zonemapping then set as ACTIVE
			}
			else
			{
				// If last button pressed was a CUE button then leaving this out will force
				// the call to the Cancel Cue routine. This will only happen in the Solo CUE mode
				// and might be a feature to remove the cue

				//					lpmwd->lpCtrlZM->iCtrlChanPos = -1;		// Clear previous channel position
				return;	// lpczm MUST be NULL, need to check elsewhere before using
			}
		}

	}

	return;
}

//================================================
//FUNCTION: HandleMBDown
//
// Handle the Middle Mouse button down.
// If this is the Intelli Mouse then it will
// be when the wheel is pressed down.  
//================================================

void    HandleMBDown(HWND hwnd, POINTS pnts, WPARAM wKeyFlags,
					 LPMIXERWNDDATA lpmwd)
{
	POINT               pnt;

	if(lpmwd == NULL)
		return;

	lpmwd->wKeyFlags = wKeyFlags;

	if(lpmwd->iCurMode != MW_NOTHING_MODE)
		return;

	pnt.x = pnts.x;
	pnt.y = pnts.y;

	SetCapture(hwnd);

	////////////////////////////////////////////
	// Activate the Scroll Mode for this Window
	//-----------------------------------------
	lpmwd->iCurMode = MW_SCROLL;
	lpmwd->pntMouseCur = pnt;
	ActivateMWMode(hwnd, lpmwd);

	return;
}

//================================================
//FUNCTION: HandleRBDown
//
// Handle when the Right mouse button is pressed
// This scrolls the image view
//================================================

void    HandleRBDown(HWND hwnd, POINTS pnts, WPARAM wKeyFlags,
					 LPMIXERWNDDATA lpmwd)
{
	POINT       pnt;

	if(lpmwd == NULL)
		return;

	lpmwd->wKeyFlags = wKeyFlags;

	if(lpmwd->iCurMode != MW_NOTHING_MODE)
		return;

	pnt.x = pnts.x;
	pnt.y = pnts.y;

	SetCapture(hwnd);

	/////////////////////////////////////////////////////
	// Activate the Relative Scroll Mode for this Window

	lpmwd->iCurMode = MW_SCROLL_RELATIVE;
	lpmwd->pntMouseCur = pnt;
	lpmwd->pntMouseLast = pnt;

	ActivateMWMode(hwnd, lpmwd);

	return;
}

//================================================
//FUNCTION: HandleLBUp
//
// Left mouse button UP
//================================================
void     HandleLBUp(HWND hwnd, POINTS pnts, WPARAM wKeyFlags,
					LPMIXERWNDDATA lpmwd)
{
	POINT       pnt;

	ReleaseCapture(); // Release the mouse capture in any case
	if(lpmwd == NULL)
		return;

	lpmwd->wKeyFlags = wKeyFlags;

	pnt.x = pnts.x;
	pnt.y = pnts.y;


	if(lpmwd->iCurMode != MW_NOTHING_MODE)
		StopMWMode(hwnd, lpmwd);

	return;
}

//================================================
//FUNCTION: HandleMBUp
//
// Middle mouse button UP
//================================================

void     HandleMBUp(HWND hwnd, POINTS pnts, WPARAM wKeyFlags,
					LPMIXERWNDDATA lpmwd)
{
	POINT       pnt;

	ReleaseCapture(); // Release the mouse capture in any case
	if(lpmwd == NULL)
		return;

	pnt.x = pnts.x;
	pnt.y = pnts.y;


	if(lpmwd->iCurMode != MW_NOTHING_MODE)
		StopMWMode(hwnd, lpmwd);

	return;
}

//================================================
//FUNCTION: HandleRBUp
//
// Right mouse button UP
//================================================

void     HandleRBUp(HWND hwnd, POINTS pnts, WPARAM wKeyFlags,
					LPMIXERWNDDATA lpmwd)
{
	POINT           pnt;

	ReleaseCapture(); // Release the mouse capture in any case
	if(lpmwd == NULL)
		return;

	lpmwd->wKeyFlags = wKeyFlags;

	pnt.x = pnts.x;
	pnt.y = pnts.y;

	if(lpmwd->iCurMode != MW_NOTHING_MODE)
		StopMWMode(hwnd, lpmwd);

	return;
}

//=========================================================
//FUNCTION: HandleWndSize
//
// Whenever ANY window is sized this routine is called
// Shared with FULL and ZOOM windows
//
// Need to handle sizing the Zoom/Full View windows so that
// they are always on a Channel boarder and need to keep
// track of what channels are visible
//=========================================================

void     HandleWndSize(HWND hwnd, LPMIXERWNDDATA lpmwd,
					   WORD wWidth, WORD wHeight, WPARAM wFlags)
{
	int     iCount;
	int     iCX;
	int     iPhisChannel;
	int     iBMPIndex;
	int     iW, iOldWidth; // width
	int     iStartScrChan;
	long    lZMCount;
	int     iAdjWidth, iAdjHeight;
	int     iWndMinWidth;
	int     iPrevStart, iPrevEnd;

	if(lpmwd == NULL)
		return;

	if((wFlags == SIZE_MINIMIZED) || (wFlags == SIZE_MAXHIDE))
		return;


	WaitForSingleObject(gDisplayEvent, 70);

	// Turn off VU data for now

	TurnOffAllVUforMixerWindow(lpmwd);

	iWndMinWidth = GetSystemMetrics(SM_CXMIN);

	// Save current visible channels

	iPrevStart = lpmwd->iStartScrChan;
	iPrevEnd = lpmwd->iEndScrChan;

	iStartScrChan = lpmwd->iStartScrChan;
	iOldWidth = lpmwd->rWndPos.right;


	////////////////////////////////////////////////////
	// Calculate the width of all the visible channels
	// Proceses Zoom and Full windows

CALC_WIDTH:
	lZMCount = lpmwd->lZMCount;
	iCX = 0;

	for(iCount=0; iCount < lZMCount; iCount++)
	{
		//////////////////////////////////
		// Get the actual phis channel
		// because they might be remaped
		//------------------------------

		iPhisChannel = LOBYTE(lpmwd->lpwRemapToScr[iCount]);
		iBMPIndex = lpmwd->lpZoneMap[iPhisChannel].iBmpIndx;
		iW = gpBMPTable[iBMPIndex].iWidth-1;

		if((iW+iCX)> wWidth)
			break;

		iCX += iW+1;
	}

	////////////////////////////////////////////////////////////
	// Check it not to be below the minimum Width for the Window
	//-----------------------------------------------------------
	if(iCX <  iWndMinWidth)
	{
		wWidth = iWndMinWidth + iW + 1;
		iWndMinWidth = 0; // Make it 0 .. to avoid endless loop
		goto CALC_WIDTH;
	}

	iCount--;		// This is the count of visible modules

	/////////////////////////////
	// store the new End channel
	//-------------------------

	if(iCount <= 0)
		lpmwd->iEndScrChan = iStartScrChan +1;
	else
	{
		if((iStartScrChan + iCount) >= lZMCount)
		{
			lpmwd->iStartScrChan = iStartScrChan = 0;
		}
		lpmwd->iEndScrChan = iStartScrChan + iCount;
	}

	////////////////////////////////////////////
	// Store the new size of the visible window
	//-----------------------------------------

	lpmwd->rVisible.right = iCX+(GetSystemMetrics(SM_CXFRAME)*2);
	lpmwd->rVisible.bottom = wHeight - HEIGHT_FULL_LABEL_WND;

	iAdjWidth = iCX+(GetSystemMetrics(SM_CXFRAME)*2);

#ifdef SCROLLBARS
	// ADJUST Width for Vertical Scroll Bars
	if(ghwndFull != hwnd)		// don't do this for full view
		iAdjWidth += GetSystemMetrics(SM_CYVTHUMB);
#endif

	iAdjHeight = wHeight + GetSystemMetrics(SM_CYFRAME) +
		(GetSystemMetrics(SM_CYBORDER)*4) +
		GetSystemMetrics(SM_CYCAPTION);

	lpmwd->rWndPos.right = iAdjWidth;
	lpmwd->rWndPos.bottom = iAdjHeight;

	// Size it only if it has changed the ammount of
	// visible Chanels. Here we care only about
	// the width.
	//----------------------------------------------
	//	if((wWidth != iCX) || (iAdjWidth == lpmwd->rMaxSize.right))
	//	{
	//	    lpmwd->rVisible.right = -1; // use it as a flag so the second time arround
	// we will not get to here


	SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, iAdjWidth,
		iAdjHeight, SWP_NOMOVE | SWP_NOZORDER ); // | SWP_NOSIZE);
	if(lpmwd->hwndImg)
	{
		if(ImageWindowSize(hwnd, &lpmwd->rVisible, lpmwd) == FALSE)
		{
			InvalidateRect(lpmwd->hwndImg, NULL, FALSE);
		}

		SetWindowPos(lpmwd->hwndImg, HWND_NOTOPMOST, 
			lpmwd->rVisible.left, lpmwd->rVisible.top, 
			lpmwd->rVisible.right, lpmwd->rVisible.bottom, 
			SWP_NOMOVE | SWP_NOZORDER);
	}

	//  }


	RequestVisibleVU(lpmwd, -1, 0);
	SetEvent(gDisplayEvent);

	return;
}


//==================================================
//FUNCTION: ShowMousePos
//
//
//purpose:
//  Draw a rectangle arround the Current Channel
//
//==================================================
void     ShowMousePos(HWND   hwnd, LPMIXERWNDDATA    lpmwd)
{
	RECT    rChan;
	HDC     hdc;
	HPEN    hpen, hpenSel, hpenOld;

	if(strcmp(lpmwd->szTitle,"Zoom Master View"))		// DO NOT DRAW BORDER AROUND MASTER MODULE
	{
		hdc = GetDC(hwnd);

		/////////////////////////////////////////////////
		// Get rectangle for the Last Active Channel 
		// so that we can replace the white with black
		/////////////////////////////////////////////////

		GetMWScrChanRect(lpmwd, lpmwd->iLastChan, &rChan);
		if(rChan.right == 0)
			return;

		rChan.right++;	// Make the rectangle one bigger to the right

		hpen = CreatePen(PS_SOLID, 1, RGB(0,0,0));	// Black rectangle
		hpenSel = NULL;

		//--------------------------------------------------
		// Set it to null brush, so we will end up only with
		// the rectangle
		//--------------------------------------------------
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		hpenOld = SelectObject(hdc, GetStockObject(BLACK_PEN));

		//-------------------------
		// Draw the black rectangle
		//-------------------------
		Rectangle(hdc, rChan.left, rChan.top, rChan.right, rChan.bottom);


		/////////////////////////////////////////////////
		// Get rectangle for the Current Channel 
		// so that we can draw the white rectangle
		/////////////////////////////////////////////////

		GetMWScrChanRect(lpmwd, lpmwd->iCurChan, &rChan);
		if(rChan.right == 0)
			goto ON_EXIT;

		rChan.right++;	// Make the rectangle one bigger to the right

		// Check if we are dragging a channel, if so draw a RED rectangle

		switch(lpmwd->iCurMode)
		{
		case MW_DRAGDROP_MODE:
			hpenSel = CreatePen(PS_SOLID, 1, RGB(255,0,0));		// Red rectangle when moving channel
			SelectObject(hdc, hpenSel);
			break;

		case MW_NOTHING_MODE:
		default:
			hpenSel = CreatePen(PS_SOLID, 1, RGB(255,255,255));	// White Rectangle showing current selection
			SelectObject(hdc, hpenSel);

		}

		// Now Draw the hilight rectangle
		//----------------------------
		Rectangle(hdc, rChan.left, rChan.top, rChan.right, rChan.bottom);


		// Clean up and exit

ON_EXIT:
		SelectObject(hdc, hpenOld);
		DeleteObject(hpen);
		if(hpenSel)
			DeleteObject(hpenSel);

		ReleaseDC(hwnd, hdc);
	}
}

//==================================================
// FUNCTION: HandleCtrlTimer
//
// When the control types updown, updownscroll, and
// leftright are active then a timer is set to control
// how quickly they are incremented/decremented.
// This routine is called when the timer expires so
// that we can handle updating the control value
//
// This routine also handles the scrolling of the 
// Zoom, Full, and Master View windows
//==================================================

void    HandleCtrlTimer(HWND hwnd, LPMIXERWNDDATA lpmwd)
{
	if(lpmwd->iCtrlMode != MW_NOTHING_MODE)
	{
		switch(lpmwd->iCtrlMode)
		{
		case CTRL_TYPE_UPDOWN:
		case CTRL_TYPE_UPDOWNSCROLL:
			UpDownControl(NULL, lpmwd->lpCtrlZM, 0, lpmwd, LOBYTE(lpmwd->lpwRemapToScr[lpmwd->iCurChan + lpmwd->iStartScrChan]));
			break;
		case CTRL_TYPE_LEFTRIGHT:	// ie. Input PAN control
			LeftRightControl(NULL, lpmwd->lpCtrlZM, 0, lpmwd, LOBYTE(lpmwd->lpwRemapToScr[lpmwd->iCurChan + lpmwd->iStartScrChan]));
			break;
		}
	}
	else		// We are scrolling a WINDOW
	{
		if(lpmwd->iCurMode != MW_NOTHING_MODE)
		{
			switch(lpmwd->iCurMode)
			{
			case MW_SCROLL_RELATIVE:
				HandleScrollImgWindowRelative(hwnd, lpmwd);
				break;
			}
		}
	}

	return;
}

//==================================================
//FUNCTION: HandleFaderMoveVert
//
// Handle moving the Vertical Fadres
//
//==================================================

void      HandleFaderMoveVert(HWND hwnd, POINT pnt, WPARAM wKeyFlags, LPMIXERWNDDATA lpmwd)
{
	HDC             hdc;
	int             iPhisChannel, iBMPIndex;
	int             iChanWidth, iChanHeight;
	LPCTRLZONEMAP   lpctrlZM;
	int             iVal, iCurVal;
	CONTROLDATA     ctrlData;
	RECT            r;

	if(pnt.y == lpmwd->pntMouseCur.y)
		return; // There is nothing to do here

	hdc = GetDC(hwnd);

	// Load the Module Bitmap in the Memory DC
	//----------------------------------------
	iPhisChannel = LOBYTE(lpmwd->lpwRemapToScr[lpmwd->iCurChan + 
		lpmwd->iStartScrChan]);
	iBMPIndex = lpmwd->lpZoneMap[iPhisChannel].iBmpIndx;
	iChanWidth = gpBMPTable[iBMPIndex].iWidth;
	iChanHeight = gpBMPTable[iBMPIndex].iHeight;



	// Get the Control ZoneMap
	//------------------------
	lpctrlZM = lpmwd->lpCtrlZM;// assigned to local for speed

	// Get relative Screen Position
	//-----------------------------
	iVal = (pnt.y + lpmwd->iYOffset) - lpctrlZM->iMinScrPos;


	// now calculate the value against the scaling factors
	//----------------------------------------------------
	if(lpctrlZM->iNumScrPos > 0)
	{
		CONVERTSCREENTOPHISICAL(lpctrlZM, iVal);
	}
	else
		iVal = 0;

	// do not go further if value is the same
	//---------------------------------------
	iCurVal = GETPHISDATAVALUE(lpmwd->iMixer, lpctrlZM, lpctrlZM->iCtrlChanPos);
	if(iVal == iCurVal)
		return;

	// The Zone
	r = lpctrlZM->rZone;

	// Set the Physical Data Value
	//-----------------------------
	SETPHISDATAVALUE(lpmwd->iMixer, lpctrlZM, lpctrlZM->iCtrlChanPos, iVal);

	// Update the Fader Position
	//--------------------------
	DrawVertFader(g_hdcMemory, lpctrlZM, iVal, lpmwd, iPhisChannel);

	// BitBlit it to the screen
	//-------------------------

	BitBlt(hdc, r.left + lpmwd->iXadj,
		r.top - lpmwd->iYOffset,
		r.right - r.left,
		r.bottom - r.top,
		g_hdcMemory, r.left, r.top,
		SRCCOPY);

	// Send the Data out
	//------------------
	ctrlData.wMixer   = lpmwd->iMixer;
	ctrlData.wChannel = lpctrlZM->iModuleNumber;//iPhisChannel;
	ctrlData.wCtrl    = lpctrlZM->iCtrlNumAbs; // we use this one since for the definition dll
	ctrlData.wVal     = iVal;

	SendDataToDevice(&ctrlData, (lpmwd->wKeyFlags & MK_SHIFT)?FALSE:TRUE,
		lpctrlZM, iVal - iCurVal, lpmwd, TRUE);

	//  Now update all of the other
	// controls that have the same
	// iCtrlNum and are Capable of
	// display on this Mixer_Window
	//-----------------------------

	// NOTE: THIS IS ALSO CALLED AGAIN IN THE UpdateSameMixWndByCtrlNum() THAT FOLLOWS
	// 
	// When channels are GROUPED and this is commented out then the readout value of the fader that is being moved is
	// not updated until the screen refreshes.  Instead of doing it here need to make sure that control area
	// is invalidated

	UpdateControlsByCtrlNum(hdc, g_hdcMemory, lpmwd, lpmwd->iXadj, iPhisChannel, lpctrlZM, iVal, DIRECTIONS_ALL, TRUE);

	// now update all of the other mixers
	// windows that represent this mixer
	// using the iMixer, iPhisChannel
	// and iVal
	//-----------------------------------
	UpdateSameMixWndByCtrlNum(hwnd, lpmwd->iMixer, iPhisChannel, lpctrlZM, iVal, NULL);

	// Clean up the mess
	//------------------
	ReleaseDC(hwnd, hdc);


	return;
}


//==================================================
//FUNCTION: HandleFaderMoveHorz
//
// Handle moving Horizontal Faders
//
//==================================================
void      HandleFaderMoveHorz(HWND hwnd, POINT pnt, WPARAM wKeyFlags, LPMIXERWNDDATA lpmwd)
{
	HDC             hdc;
	int             iPhisChannel, iBMPIndex;
	int             iChanWidth, iChanHeight;
	LPCTRLZONEMAP   lpctrlZM;
	int             iVal, iCurVal;
	CONTROLDATA     ctrlData;
	RECT            r;


	if(pnt.x == lpmwd->pntMouseCur.x)
		return; // There is nothing to do here

	hdc = GetDC(hwnd);

	// Load the Module Bitmap in the Memory DC
	//----------------------------------------
	iPhisChannel = LOBYTE(lpmwd->lpwRemapToScr[lpmwd->iCurChan + 
		lpmwd->iStartScrChan]);
	iBMPIndex = lpmwd->lpZoneMap[iPhisChannel].iBmpIndx;
	iChanWidth = gpBMPTable[iBMPIndex].iWidth;
	iChanHeight = gpBMPTable[iBMPIndex].iHeight;


	// Get the Control ZoneMap
	//------------------------
	lpctrlZM = lpmwd->lpCtrlZM;// assigned to local for speed


	// Get relative Screen Position
	//-----------------------------
	iVal = (pnt.x - lpmwd->iXadj) - lpctrlZM->iMinScrPos;

	// now calculate the value against the scaling factors
	//----------------------------------------------------
	if(lpctrlZM->iNumScrPos > 0)
	{
		CONVERTSCREENTOPHISICAL(lpctrlZM, iVal);
	}
	else
		iVal = 0;

	// do not go further if value is the same
	//---------------------------------------
	iCurVal = GETPHISDATAVALUE(lpmwd->iMixer, lpctrlZM, lpctrlZM->iCtrlChanPos);
	if(iVal == iCurVal)
		return;


	r = lpctrlZM->rZone;


	// Set the Phisiacal Data Value
	//-----------------------------
	SETPHISDATAVALUE(lpmwd->iMixer, lpctrlZM, lpctrlZM->iCtrlChanPos, iVal);

	// Update the Fader Position
	//--------------------------
	DrawHorizFader(g_hdcMemory, lpctrlZM, iVal, lpmwd, iPhisChannel);

	BitBlt(hdc, r.left + lpmwd->iXadj,
		r.top - lpmwd->iYOffset,
		r.right - r.left,
		r.bottom - r.top,
		g_hdcMemory, r.left, r.top,
		SRCCOPY);


	// Send the Data out
	//------------------
	ctrlData.wMixer   = lpmwd->iMixer;
	ctrlData.wChannel = lpctrlZM->iModuleNumber;//iPhisChannel;
	ctrlData.wCtrl    = lpctrlZM->iCtrlNumAbs; // we use this one since for the definition dll 
	//these(our) control Numbers are meaningless
	ctrlData.wVal     = iVal;

	SendDataToDevice(&ctrlData, (lpmwd->wKeyFlags & MK_SHIFT)?FALSE:TRUE,
		lpctrlZM, iVal - iCurVal, lpmwd, TRUE);

	///////////////////////////////
	//  Now update all of the other
	// controls that have the same
	// iCtrlNum and are Capable of
	// display on this Mixer_Window
	//-----------------------------

	// NOTE: THIS IS ALSO CALLED AGAIN IN THE UpdateSameMixWndByCtrlNum() THAT FOLLOWS
	// 
	// When removed may cause problems

	UpdateControlsByCtrlNum(hdc, g_hdcMemory, lpmwd, lpmwd->iXadj, iPhisChannel, 
		lpctrlZM, iVal, DIRECTIONS_ALL, TRUE);

	//////////////////////////////////////
	// now update all of the other mixers
	// windows that represent this mixer
	// using the iMixer, iPhisChannel
	// and iVal
	//-----------------------------------
	UpdateSameMixWndByCtrlNum(hwnd, lpmwd->iMixer, iPhisChannel, lpctrlZM, iVal, NULL);

	// Clean up the mess
	//------------------
	ReleaseDC(hwnd, hdc);


	return;
}


/////////////////////////////////////////////////
//
//
//
BOOL  IsCtrlPrePostFilter(int iType)
{

	switch(iType)
	{

	case CTRL_TYPE_BTN_INPUT_AUXPOST_FILTER:
	case CTRL_TYPE_BTN_MASTER_HEADPOST_FILTER:
	case CTRL_TYPE_BTN_AUX_MMATRIX_LT_POST_FILTER:
	case CTRL_TYPE_BTN_AUX_MMATRIX_RT_POST_FILTER:
	case CTRL_TYPE_BTN_MASTER_AUX_PREPOST_FILTER:

	case CTRL_NUM_MASTER_POST_LT_VU:
	case CTRL_NUM_MASTER_POST_RT_VU:

		return TRUE;
		break;
	}

	return FALSE;
}


////////////////////////////////////////////////////////
// BOOL  IsCtrlCueButton(int iType, int iModuleNumber)
//
// iType - control type
// iModuleNumber - module number
//
// Returns TRUE if this is a CUE button
//
BOOL  IsCtrlCueButton(int iType, int iModuleNumber )
{
	int iModuleType;

	// Lookup Module type based on module number

	iModuleType = gDeviceSetup.iaChannelTypes[iModuleNumber];

	switch(iModuleType)
	{
	case DCX_DEVMAP_MODULE_INPUT:

		switch(iType)
		{

			// All the INPUT cues

		case CTRL_NUM_INPUT_CUE_FAD_PRE:
		case CTRL_NUM_INPUT_CUE_FAD_POST:
		case CTRL_NUM_INPUT_MIC_B_CUE:
		case CTRL_NUM_INPUT_MIC_A_CUE:
		case CTRL_NUM_INPUT_GATE_KEY_INOUT:

			return TRUE;
			break;
		}
		break;

	case	DCX_DEVMAP_MODULE_AUX:

		switch(iType)
		{
			// All the MATRIX cues

		case CTRL_NUM_MATRIX_STERIO_CUE_PRE:
		case CTRL_NUM_MATRIX_STERIO_CUE_POST:
		case CTRL_NUM_MATRIX_CUE_PRE:
		case CTRL_NUM_MATRIX_CUE_POST:

		case CTRL_NUM_MASTER_AUX16PRE:
		case CTRL_NUM_MASTER_AUX15PRE:
		case CTRL_NUM_MASTER_AUX14PRE:
		case CTRL_NUM_MASTER_AUX13PRE:
		case CTRL_NUM_MASTER_AUX12PRE:
		case CTRL_NUM_MASTER_AUX11PRE:
		case CTRL_NUM_MASTER_AUX10PRE:
		case CTRL_NUM_MASTER_AUX09PRE:
		case CTRL_NUM_MASTER_AUX08PRE:
		case CTRL_NUM_MASTER_AUX07PRE:
		case CTRL_NUM_MASTER_AUX06PRE:
		case CTRL_NUM_MASTER_AUX05PRE:
		case CTRL_NUM_MASTER_AUX04PRE:
		case CTRL_NUM_MASTER_AUX03PRE:
		case CTRL_NUM_MASTER_AUX02PRE:
		case CTRL_NUM_MASTER_AUX01PRE:
		case CTRL_NUM_MASTER_AUX16POST:
		case CTRL_NUM_MASTER_AUX15POST:
		case CTRL_NUM_MASTER_AUX14POST:
		case CTRL_NUM_MASTER_AUX13POST:
		case CTRL_NUM_MASTER_AUX12POST:
		case CTRL_NUM_MASTER_AUX11POST:
		case CTRL_NUM_MASTER_AUX10POST:
		case CTRL_NUM_MASTER_AUX09POST:
		case CTRL_NUM_MASTER_AUX08POST:
		case CTRL_NUM_MASTER_AUX07POST:
		case CTRL_NUM_MASTER_AUX06POST:
		case CTRL_NUM_MASTER_AUX05POST:
		case CTRL_NUM_MASTER_AUX04POST:
		case CTRL_NUM_MASTER_AUX03POST:
		case CTRL_NUM_MASTER_AUX02POST:
		case CTRL_NUM_MASTER_AUX01POST:

			return TRUE;
			break;
		}
		break;


	case DCX_DEVMAP_MODULE_MATRIX:

		switch(iType)
		{
			// All the MATRIX cues

		case CTRL_NUM_MATRIX_STERIO_CUE_PRE:
		case CTRL_NUM_MATRIX_STERIO_CUE_POST:
		case CTRL_NUM_MATRIX_CUE_PRE:
		case CTRL_NUM_MATRIX_CUE_POST:

			return TRUE;
			break;
		}
		break;

	case DCX_DEVMAP_MODULE_MASTER:

		switch(iType)
		{
			// All the MASTER cues

		case CTRL_NUM_MASTER_CUE_LEVEL_MONO:
		case CTRL_NUM_MASTER_CUE_LEVEL_CENTER:
			// fds revmoved 3/18/2001 as per gamble	CTRL_NUM_MASTER_CUE_A_SUM_IN:
		case CTRL_NUM_MASTER_STEREO_CUE_PRE:
		case CTRL_NUM_MASTER_STEREO_CUE_POST:

			return TRUE;
			break;
		}
		break;
	}

	return FALSE;
}




/////////////////////////////////
// FUNCTION: HandleCtrlBtnClick
//
//
// PURPOSE: Processes a mouse button click on a BUTTON control
//
//
extern	int		g_inputCueActiveCounter;

void      HandleCtrlBtnClick(HWND hwnd, LPMIXERWNDDATA lpmwd)
{
	LPCTRLZONEMAP       lpctrlZM;
	CONTROLDATA         ctrlData;
	int                 iPhisChannel, iVal, ivalue;
	HDC                 hdc;
	RECT                r,rInvalidate;
	HBITMAP             hbmp;
	int                 iBMPIndex;
	BOOL                bIsOn;
	WORD								wVal;

	if(hwnd == NULL)
		hwnd = lpmwd->hwndImg; // Grab the image Window from the MixerWindow data

	// Handle the button Up down
	//--------------------------
	iPhisChannel = LOBYTE(lpmwd->lpwRemapToScr[lpmwd->iCurChan + 
		lpmwd->iStartScrChan]);

	// get the zone map pointer
	//-------------------------
	lpctrlZM = lpmwd->lpCtrlZM;

	////////////////////////////////////////////////////////////////////
	// Get the current value and compare to the minimum value of 0

	iVal = GETPHISDATAVALUE(lpmwd->iMixer, lpctrlZM, lpctrlZM->iCtrlChanPos);

	////////////////////////////////////////////////////////////////////
	// If they are not equal then we are turning the button ON
	// We are making sure it isn't the MIN value

	if(iVal != 0)
	{
		bIsOn = TRUE;

		// Handle possible Filter buttons ... like MUTE and such
		//
		StartControlDataFilter(iPhisChannel, lpmwd, lpctrlZM, bIsOn, TRUE);

		// Do the Toggle stuff ..
		//
		CheckForToggleSwitches(lpmwd, lpctrlZM);

		ctrlData.wMixer   = lpmwd->iMixer;
		ctrlData.wChannel = lpctrlZM->iModuleNumber;					// iPhisChannel;
		ctrlData.wCtrl    = lpctrlZM->iCtrlNumAbs;						// Index into lookup table, currently 0-470
		iVal = CDef_GetCtrlMaxVal(lpctrlZM->iCtrlNumAbs) - 1;	// get number of table entries, we need to go down to the minimum value
		iVal--;																								// skip this value since the control is already there

		if((IsCtrlPrePostFilter(lpctrlZM->iCtrlType) == FALSE) && (ctrlData.wCtrl < 0x8000))
		{
			// Loop thru the table 
			for(iVal; iVal >= 0; iVal --)
			{
				// Send the Data out
				//------------------
				ctrlData.wVal     = iVal;

				SendDataToDevice(&ctrlData, (lpmwd->wKeyFlags & MK_SHIFT)?FALSE:TRUE,
					lpctrlZM, -1, lpmwd, TRUE);
			}
		}
		iVal = 0;	// Set to MIN value


		// Moved this logic from HandleInputToggleSwtches() so that the order of commands
		// sent out are:
		//
		// *033XXW440000
		// *033XXW450065
		// *033XXW440063
		// *033XXW451063
		//
		// Still needs to be cleaned up.

		if(gDeviceSetup.iaChannelTypes[lpctrlZM->iModuleNumber] == DCX_DEVMAP_MODULE_INPUT)
		{

			if(lpctrlZM->iCtrlChanPos == CTRL_NUM_INPUT_MIC_A_CUE ||
				lpctrlZM->iCtrlChanPos == CTRL_NUM_INPUT_MIC_B_CUE ||
				lpctrlZM->iCtrlChanPos == CTRL_NUM_INPUT_GATE_KEY_INOUT)
			{
				wVal = GETPHISDATAVALUE(0, lpctrlZM, lpctrlZM->iCtrlChanPos);
				HandleMasterCueSwitch(lpmwd, wVal);
			}
		}
		// Handle this control type again ...
		// not the best solution, since we are going to resend the data to the Mixer .... ???
		//
		if(lpctrlZM->iCtrlType == CTRL_TYPE_INPUT_GATE_IN_BTN_FILTER){
			StartControlDataFilter(iPhisChannel, lpmwd, lpctrlZM, bIsOn, TRUE);
		}

	}
	else	// We must be turning the BUTTON OFF
	{
		bIsOn = FALSE;

		// Handle possible Filter buttons ... like MUTE and such
		//
		StartControlDataFilter(iPhisChannel, lpmwd, lpctrlZM, bIsOn, TRUE);

		// Do the Toggle stuff ..
		//
		CheckForToggleSwitches(lpmwd, lpctrlZM);

		ctrlData.wMixer   = lpmwd->iMixer;
		ctrlData.wChannel = lpctrlZM->iModuleNumber;	// iPhisChannel;
		ctrlData.wCtrl    = lpctrlZM->iCtrlNumAbs;		// Index into lookup table, currently 0-470

		ivalue = CDef_GetCtrlMaxVal(lpctrlZM->iCtrlNumAbs);	// ok we need to go up to max value
		iVal = 0;
		iVal++;																							// skip this value since the control is already there

		if((IsCtrlPrePostFilter(lpctrlZM->iCtrlType) == FALSE) && (ctrlData.wCtrl < 0x8000))
		{
			for(iVal; iVal < ivalue; iVal++)
			{
				// Send the Data out
				//------------------
				ctrlData.wVal     = iVal;

				SendDataToDevice(&ctrlData, (lpmwd->wKeyFlags & MK_SHIFT)?FALSE:TRUE,
					lpctrlZM, 1, lpmwd, TRUE);
			}
			iVal --; // this would be the correct control value
		}
		else
		{
			iVal = ivalue - 1;
			if(iVal == 1)
				iVal ++;
		}
	}

	// Set the Phisical Data Value
	//----------------------------
	SETPHISDATAVALUE(lpmwd->iMixer, lpctrlZM, lpctrlZM->iCtrlChanPos, iVal);

	// update the screen part
	//-----------------------
	r = lpctrlZM->rZone;		// THIS IS RECTANGLE FOR CHANNEL 1 !!!
	hdc = GetDC(hwnd);

	////////////////////////////////////////////////////
	// FDS
	//
	// Invalidate rectangle for THIS button
	// so that it is updated without redrawing screen

	if(lpmwd->hwndImg)	// Clicked on button
	{
		rInvalidate.left = r.left + lpmwd->iXadj;
		rInvalidate.top  = r.top - lpmwd->iYOffset;
		rInvalidate.right = r.right  + lpmwd->iXadj;
		rInvalidate.bottom = (r.bottom - r.top) + rInvalidate.top;
	}
	else	// we are RECALLING button
	{
		rInvalidate.left = r.left + lpmwd->iXadj;
		rInvalidate.top  = r.top;
		rInvalidate.right = r.right  + lpmwd->iXadj;
		rInvalidate.bottom = (r.bottom - r.top) + rInvalidate.top;
	}
	/////////////////////////////////////////////////////////
	// Compare current value with the minimum value
	// If they are equal then we are turning the button ON
	// and the button in being pressed DOWN
	// else we are turning the button OFF and we are
	// releasing the button
	//
	// Was testing against CDef_GetCtrlMinVal(lpctrlZM->iCtrlNumAbs)()
	// but that always returns 0

	if(iVal == 0)
	{
		PushBtn(g_hdcMemory, lpctrlZM, iVal, lpmwd, iPhisChannel);

		////////////////////////////////////////////////////////////////////////
		// now update all of the other mixers windows that represent this mixer
		// using the iMixer, iPhisChannel and iVal
		//-----------------------------------

		UpdateSameMixWndByCtrlNum(hwnd, lpmwd->iMixer, iPhisChannel, lpctrlZM, iVal, NULL);

		// NOTE: THIS IS ALSO CALLED AGAIN IN THE UpdateSameMixWndByCtrlNum() ABOVE
		// PASS THE iXadj FOR THIS CHANNEL BEING UPDATED
		//
		// When commented out the button for Input and Matrix EQ doesn't refresh!!

		UpdateControlsByCtrlNum(hdc, g_hdcMemory, lpmwd, lpmwd->iXadj, 
			iPhisChannel, lpctrlZM, iVal, DIRECTIONS_ALL, TRUE);

	}
	else
	{
		iBMPIndex = lpmwd->lpZoneMap[iPhisChannel].iBmpIndx;

		hbmp = SelectObject(g_hdcBuffer,  gpBMPTable[iBMPIndex].hbmp);

		BitBlt(hdc, r.left + lpmwd->iXadj,
			r.top - lpmwd->iYOffset,
			r.right - r.left,
			r.bottom - r.top,
			g_hdcBuffer, r.left, r.top,
			SRCCOPY);


		////////////////////////////////////////////////////////////////////////
		// now update all of the other mixers windows that represent this mixer
		// using the iMixer, iPhisChannel and iVal
		//-----------------------------------

		// NOTE: THIS IS ALSO CALLED AGAIN IN THE UpdateSameMixWndByCtrlNum() THAT FOLLOWS
		//
		// When commented out the button for EQ doesn't refresh!!

		UpdateControlsByCtrlNum(hdc, g_hdcMemory, lpmwd, lpmwd->iXadj, 
			iPhisChannel, lpctrlZM, iVal, DIRECTIONS_ALL, TRUE);
		UpdateSameMixWndByCtrlNum(hwnd, lpmwd->iMixer, iPhisChannel, lpctrlZM, iVal, g_hdcBuffer);

		SelectObject(g_hdcBuffer, hbmp);

	}

	/////////////////////////////////////////////////////////////////////////////
	// If lpmwd->hwndImg is not NULL then we are clicking on the Button 
	// if not then we are recalling the data and don't have a windows, so use the
	// global zoom window handle

	if(lpmwd->hwndImg)
		InvalidateRect(lpmwd->hwndImg,&rInvalidate,FALSE);		// FDS - MAKE SURE BUTTON GETS UPDATED BY INVALIDATING THE RECTANGLE
	else if(ghwndZoom)
		InvalidateRect(ghwndZoom,&rInvalidate,FALSE);		// FDS - MAKE SURE BUTTON GETS UPDATED BY INVALIDATING THE RECTANGLE

	ReleaseDC(hwnd, hdc);
	syncInputPriority (lpctrlZM, g_inputCueActiveCounter, lpmwd);

	// Force redraw of this window
	// This is needed for the solo cue mode
	// if its not here then the master module cues
	// are not redrawn when they are turned off when
	// pressing a cue button on another module.
	// if it is here then it causes the white border
	// around the zoom window channel to flash
	// Fix is to use the master window handle rather
	// than hwnd

	GetClientRect(ghwndMaster, &rInvalidate);
	InvalidateRect(ghwndMaster,&rInvalidate,FALSE);

	return;
}



//=========================================
//FUNCTION: UpdateControlsByCtrlNum
//
//params:
//
//
//purpose:
//      Update all of the Controls with the
//  same iCtrlNum that can display anything
//  on the Screen, without updating the
//  the Current Control(the Control that
//  is actualy calling this function)
//=========================================
void     UpdateControlsByCtrlNum(HDC hdcScreen, HDC hdcMem,
								 LPMIXERWNDDATA lpmwd, int iXAdj,
								 int iPhisChan,
								 LPCTRLZONEMAP   lpctrlZM, int iPhisVal,
								 int iDirection, BOOL bSkipCurrent)
{
	LPCTRLZONEMAP   lpctrlZM_Root;
	LPCTRLZONEMAP   lpctrlZM_Cur;
	LPCTRLZONEMAP   lpctrlZM_Special;
	int             iCtrlNum;
	RECT            rZone;
	int             iScrTop, iScrBottom;
	int             iSpecType;

	iCtrlNum = lpctrlZM->iCtrlChanPos;

	iScrTop = lpmwd->iYOffset;
	iScrBottom = lpmwd->rVisible.bottom + lpmwd->iYOffset;

	// Do the Forward search and update
	//---------------------------------
	if((iDirection == DIRECTIONS_ALL) || (iDirection == DIRECTIONS_FORWARD))
	{
		lpctrlZM_Special = lpctrlZM_Cur = lpctrlZM; // Get the one that called us

		if(bSkipCurrent)
			lpctrlZM_Cur ++; // start with the next Control item

		lpctrlZM_Cur = ScanCtrlZonesNum(lpctrlZM_Cur, iCtrlNum);

		if(lpctrlZM_Cur == NULL)
			goto SPECIAL_CONTROL;

		while(lpctrlZM_Cur)
		{
			if((lpctrlZM_Cur->CtrlFunc) && (lpctrlZM_Cur->iDispType != DISP_TYPE_NULL))
			{
				rZone.left = lpctrlZM_Cur->rZone.left + iXAdj;		//lpmwd->iXadj;
				rZone.top = lpctrlZM_Cur->rZone.top - lpmwd->iYOffset;
				rZone.right = lpctrlZM_Cur->rZone.right - lpctrlZM_Cur->rZone.left;
				rZone.bottom = lpctrlZM_Cur->rZone.bottom - lpctrlZM_Cur->rZone.top;

				if((iScrBottom > rZone.top) && ((lpctrlZM_Cur->rZone.bottom - lpmwd->iYOffset) > 0))
				{
					// This should update the Control to
					// the Memory DC
					//----------------------------------
					lpctrlZM_Cur->CtrlFunc(hdcMem, lpctrlZM_Cur, iPhisVal, lpmwd, iPhisChan);

					// Copy it to the Screen
					//----------------------
					BitBlt(hdcScreen,
						rZone.left,
						rZone.top,
						rZone.right,
						rZone.bottom,
						hdcMem, lpctrlZM_Cur->rZone.left, lpctrlZM_Cur->rZone.top,
						SRCCOPY);
				}
			}

			// try to find the next one
			//-------------------------
			lpctrlZM_Cur ++; // start with the next Control item
			lpctrlZM_Cur = ScanCtrlZonesNum(lpctrlZM_Cur, iCtrlNum);

			// Check for special filters at the end of the update
			//
SPECIAL_CONTROL:
			if((lpctrlZM_Cur == NULL) && (lpctrlZM_Special))
			{
				iSpecType = IsSpecialFilter(lpmwd, iPhisChan, lpctrlZM_Special);

				if(iSpecType > 0)
				{
					lpctrlZM_Cur = ScanCtrlZonesType(lpctrlZM_Special, iSpecType);
					if(lpctrlZM_Cur)
					{
						rZone.left = lpctrlZM_Cur->rZone.left;//lpmwd->iXadj;
						rZone.top = lpctrlZM_Cur->rZone.top;
						rZone.right = lpctrlZM_Cur->rZone.right;// - lpctrlZM_Cur->rZone.left;
						rZone.bottom = lpctrlZM_Cur->rZone.bottom;// - lpctrlZM_Cur->rZone.top;

						FillRect(hdcMem, &rZone, GetStockObject(BLACK_BRUSH));
					}

				}
				lpctrlZM_Special = NULL;
				//			    lpctrlZM_Cur = NULL; // <== this line in fixes EQ bug but doesn't draw until moved

			}
		}
	}

	// Do the Backward search and update
	//----------------------------------
	if((iDirection == DIRECTIONS_ALL) || (iDirection == DIRECTIONS_BACK))
	{
		// load the first pointer
		// so we know when to stop
		//------------------------
		lpctrlZM_Root = lpmwd->lpZoneMap[iPhisChan].lpZoneMap;
		lpctrlZM_Cur = lpctrlZM; // Get the one that called us

		// well here we'll be going backwards
		// so we have to make sure we are not
		// at the very first Control ZoneMap
		//-----------------------------------
		if(lpctrlZM_Root != lpctrlZM_Cur)
			do
			{
				lpctrlZM_Cur--;
				if(lpctrlZM_Cur->iCtrlChanPos == iCtrlNum)
				{
					if((lpctrlZM_Cur->CtrlFunc) && (lpctrlZM_Cur->iDispType != DISP_TYPE_NULL))
					{
						rZone.left = lpctrlZM_Cur->rZone.left + iXAdj;//lpmwd->iXadj;
						rZone.top = lpctrlZM_Cur->rZone.top - lpmwd->iYOffset;
						rZone.right = lpctrlZM_Cur->rZone.right - lpctrlZM_Cur->rZone.left;
						rZone.bottom = lpctrlZM_Cur->rZone.bottom - lpctrlZM_Cur->rZone.top;

						if((iScrBottom > rZone.top) && ((lpctrlZM_Cur->rZone.bottom - lpmwd->iYOffset) > 0))
						{
							// This should update the Control to
							// the Memory DC
							//----------------------------------
							lpctrlZM_Cur->CtrlFunc(hdcMem, lpctrlZM_Cur, iPhisVal, lpmwd, iPhisChan);

							// Copy it to the Screen
							//----------------------
							BitBlt(hdcScreen,
								rZone.left,
								rZone.top,
								rZone.right,
								rZone.bottom,
								hdcMem, lpctrlZM_Cur->rZone.left, lpctrlZM_Cur->rZone.top,
								SRCCOPY);
						}
					}
				}
				else
					lpctrlZM_Cur = lpctrlZM_Root;

			} while(lpctrlZM_Cur != lpctrlZM_Root);
	}


	return;
}


//====================================================
//FUNCTION: UpdateSameMixWndByCtrlNum
//
//params:
//      HWND    hwnd; -> the Window that starts the
//                       update
//      int     iMixer; ->  the mixer we want to
//                          check for
//      int     iPhisChan;->the Channel to be updated
//      int     iVal; -> the phisical value
//
//purpose:
//      Scan all the windows and update the ones
//      that represent the same Phisical Mixer
//      Here we use oiur own list of the opened
//      windows
//
//====================================================
/*
void      UpdateSameMixWndByCtrlNum(HWND  hwnd,
int   iMixer,
int   iPhisChannel,
LPCTRLZONEMAP   lpctrlZM,
int   iVal)
{

//LPMIXERWNDDATA      lpMWD;
LPDLENTRY           lpdle;
LPMW_DL_LIST        lpMW_DL;
HWND                hwndNext;

hwndNext = hwnd;


lpdle = GetFirstEntry(gpMixerWindows_DL_List);

// Scan through the Linked list for all windows
// and update the ones that need to be updated
//---------------------------------------------
while(lpdle)
{
lpMW_DL = (LPMW_DL_LIST)GetEntryData(gpMixerWindows_DL_List, lpdle);

hwndNext = lpMW_DL->hwndMW;

// Skip the Window that called us
//-------------------------------
if(hwndNext != hwnd)
{
if(lpMW_DL->lpMWD->iMixer == iMixer)
// we got one ... lets update it
//------------------------------
{
InvalidateRect(hwndNext, NULL, FALSE);
UpdateWindow(hwndNext);
}
}

// lets go to the next entry
//--------------------------
lpdle = GetNextEntry(gpMixerWindows_DL_List, lpdle);
}

return;
}
*/





////////////////////////////
// ONLY USED BELOW TO FIND
// wCtrlVal in a zonemap

BOOL bFindCtrlVal(LPCTRLZONEMAP lpctrlZM, WORD wCtrlVal)
{
	if((lpctrlZM != 0) && (lpctrlZM->iCtrlNumAbs == wCtrlVal))
		return TRUE;
	else
		return FALSE;
}

///////////////////////////////////////////////
// FUNCTION: UpdateControlFromNetwork
//
// Recieves control data echoed by the GServer
// from other clients and updates the bitmap
// for the control on this client.
//
// Incoming data from remote client handled in CDef_DevCommnication.c - ProcessTcpData() -  
// which eventually this routine.
// 
// The bIsAbsVal flag is used to determine what type of data is being sent
// in the wCtrlVal.  
//
// If it is true then it is the control index, usually found
// in the variable CtrlDataCopy.wCtrl.
//
// If it is false then is is the channel position, usually found
// in the variable lpctrlZM->iCtrlChanPos
// 


void  UpdateControlFromNetwork(WORD iPhisChannel, WORD wCtrlVal, int iVal, BOOL bIsAbsVal) 
{
	LPCTRLZONEMAP lpctrlZM;
	HDC           hdcBuffer;
	HBITMAP       hbmp;
	RECT          r;
	int           iCount;


	// Check for the  Matrix modules and flip them to the Aux modules
	// Since the program *thinks* they reside on the same Module
	//
	// I DON'T THINK THIS DOES ANYTHING  - FDS

	if( (iPhisChannel == g_iCueModuleIdx)  )
	{
		iPhisChannel = g_iMasterModuleIdx;
	}
	else
	{
		for(iCount = 0; iCount < g_iAuxIdx; iCount ++)
		{
			if(g_aiMatrix[iCount] == iPhisChannel)
			{
				iPhisChannel = g_aiAux[iCount];
				break;
			}
		}
	}


	if(iPhisChannel >= giMax_CHANNELS)
		return;

	///////////////////////////////////////////////
	// Check flag if we are sending the index into
	// the dcx.bin, if true then use the Abs() call
	// else we are passing the iCtrlChanPos and need
	// to use the Num() routine instead

	if(bIsAbsVal)	
	{
		lpctrlZM = ScanCtrlZonesAbs(gpZoneMaps_Zoom[iPhisChannel].lpZoneMap, wCtrlVal);
	}
	else
	{
		lpctrlZM = ScanCtrlZonesNum(gpZoneMaps_Zoom[iPhisChannel].lpZoneMap, wCtrlVal);
	}

	// Force a search in the Master module if we couldn't find the Control Zone
	// it better be there
	//
	if(lpctrlZM == NULL)
	{

		///////////////////////////////////////////////
		// Check flag if we are sending the index into
		// the dcx.bin, if true then use the Abs() call
		// else we are passing the iCtrlChanPos and need
		// to use the Num() routine instead

		if(bIsAbsVal)		// for here it just means we are called by the network reciever
		{

			// Loop throught the matrix count and find if this channel is an AUX, if so
			// then we use the count to index into our controls to find the correct one
			// on the Master module

			for(iCount=0; iCount < MAX_MATRIX_COUNT; iCount ++)
			{

				if(g_aiMatrix[iCount] != 0)		// Make sure there is a Matrix module at this position
				{
					if(iPhisChannel == g_aiAux[iCount])	// Get the corresponding AUX module channel number
						break;
				}
			}

			// We got the iCount offset from the controls so we can find the one we are looking for.
			// we need to distinguish the correct control by using the wCtrlVal that is passed
			// over the network.


			lpctrlZM = ScanCtrlZonesNum(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, CTRL_NUM_MASTER_AUX1LT_FADER - iCount*4);

			if(!bFindCtrlVal(lpctrlZM, wCtrlVal))
				lpctrlZM = ScanCtrlZonesNum(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, CTRL_NUM_MASTER_AUX1RT_FADER - iCount*4);
			if(!bFindCtrlVal(lpctrlZM, wCtrlVal))
				lpctrlZM = ScanCtrlZonesNum(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, CTRL_NUM_MASTER_AUX2LT_FADER - iCount*4);
			if(!bFindCtrlVal(lpctrlZM, wCtrlVal))
				lpctrlZM = ScanCtrlZonesNum(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, CTRL_NUM_MASTER_AUX2RT_FADER - iCount*4);
			// SUM IN
			if(!bFindCtrlVal(lpctrlZM, wCtrlVal))
				lpctrlZM = ScanCtrlZonesNum(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, CTRL_NUM_MASTER_SUMIN1 - iCount*2);
			if(!bFindCtrlVal(lpctrlZM, wCtrlVal))
				lpctrlZM = ScanCtrlZonesNum(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, CTRL_NUM_MASTER_SUMIN2 - iCount*2);
			// Master Pre-post
			if(!bFindCtrlVal(lpctrlZM, wCtrlVal))
				lpctrlZM = ScanCtrlZonesNum(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, CTRL_NUM_MASTER_AUX01PRE - iCount*4);
			if(!bFindCtrlVal(lpctrlZM, wCtrlVal))
				lpctrlZM = ScanCtrlZonesNum(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, CTRL_NUM_MASTER_AUX01POST - iCount*4);
			if(!bFindCtrlVal(lpctrlZM, wCtrlVal))
				lpctrlZM = ScanCtrlZonesNum(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, CTRL_NUM_MASTER_AUX02PRE - iCount*4);
			if(!bFindCtrlVal(lpctrlZM, wCtrlVal))
				lpctrlZM = ScanCtrlZonesNum(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, CTRL_NUM_MASTER_AUX02POST - iCount*4);
			// Master Aux mutes
			if(!bFindCtrlVal(lpctrlZM, wCtrlVal))
				lpctrlZM = ScanCtrlZonesNum(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, CTRL_NUM_MASTER_CUE_AUX01_MUTE - iCount*2);
			if(!bFindCtrlVal(lpctrlZM, wCtrlVal))
				lpctrlZM = ScanCtrlZonesNum(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, CTRL_NUM_MASTER_CUE_AUX02_MUTE - iCount*2);

			// now force to master module so that UpdateSameMixWndByCtrlNum() can update control values
			iPhisChannel = g_iMasterModuleIdx;

		}
		else
		{
			lpctrlZM = ScanCtrlZonesNum(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, wCtrlVal);
		}

	}
	///////////////////////////////////////////////////////////////////
	// We are filtering some controls so they don't update another
	// client over the network.  UpdateFromNetwork will return
	// FALSE if we should NOT update the control.

	if (UpdateFromNetwork( iPhisChannel, lpctrlZM) != FALSE)
	{

		// Set the Phisical Data Value
		// This causes the control to be drawn in the new position
		// If its a fader that has been pulled down because of a mute
		// then we do NOT want to do this.
		//-----------------------------------------------------------

		SETPHISDATAVALUE(0, lpctrlZM, lpctrlZM->iCtrlChanPos, iVal);

		r = lpctrlZM->rZone;

		// Select the appropriate bitmap into the buffer
		//
		hdcBuffer = CreateCompatibleDC(ghdc256);

		hbmp = SelectObject(hdcBuffer, gpBMPTable[gpZoneMaps_Zoom[iPhisChannel].iBmpIndx].hbmp);

		BitBlt(g_hdcMemory, r.left,
			r.top,
			r.right - r.left,
			r.bottom - r.top,
			hdcBuffer,   r.left, r.top,
			SRCCOPY);

		SelectObject(hdcBuffer, hbmp);
		DeleteDC(hdcBuffer);


		// This actually displays the entire movement of the faders (and other controls)
		// Without this line the faders will 'jump' into their final position

		// now update all of the other mixers
		// windows that represent this mixer
		// using the iMixer, iPhisChannel
		// and iVal

		// THIS really slows things down
		// Doesn't seem to be needed????
		// This updates the visual of the controls, ie values
		//
		// without this line Group Faders do NOT work

		// This updates the iPhisChannel, if we want to update a virtual module
		// then need to pass that address instead.

		UpdateSameMixWndByCtrlNum(NULL, 0, iPhisChannel, lpctrlZM, iVal, NULL);


	}

}

