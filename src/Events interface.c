//=================================================
// Copyright 1998, CorTek Software, Inc.
//=================================================

//=================================================
//Mouse releated Routines
//
//=================================================
//#include <windows.h>

#include "SAMM.h"
#include "SAMMEXT.h"
#include "MACRO.h"

#include "consoledefinition.h"


extern int                 g_aiAux[MAX_MATRIX_COUNT];
extern int                 g_aiMatrix[MAX_MATRIX_COUNT];
extern int                 g_iCueModuleIdx;
extern int                 g_iAuxIdx;
extern int                 g_iMasterModuleIdx;


void HandleMasterCueSwitch(LPMIXERWNDDATA lpmwd, WORD wVal);

void TurnOffAllVUforMixerWindow(LPMIXERWNDDATA lpmwd);
void  CheckForSpecialFilters(/*LPMIXERWNDDATA lpwmd, */LPCTRLZONEMAP pctrlzm);
void	syncInputPriority (LPCTRLZONEMAP pctrlzm, int	icount, LPMIXERWNDDATA lpmwd);

//================================================
//FUNCTION: HandleMouseMove
//
//================================================
void    HandleMouseMove(HWND hwnd,POINTS pnts, WPARAM wKeyFlags,
                        LPMIXERWNDDATA lpmwd)
{
int     iChan;
int     iCtrlMode;
POINT   pnt;

WaitForSingleObject(gDisplayEvent, 90);

if(lpmwd == NULL)
    return;

pnt.x = pnts.x;
pnt.y = pnts.y;

lpmwd->wKeyFlags = wKeyFlags;

if(lpmwd->iCurMode == MW_SCROLL)
    {
		//WaitForSingleObject(gDisplayEvent, 90);
    lpmwd->pntMouseLast = lpmwd->pntMouseCur;
    lpmwd->pntMouseCur = pnt;
    ScrollImgWindow(hwnd, lpmwd);
		SetEvent(gDisplayEvent);
    return;
    }

if(lpmwd->iCurMode == MW_SCROLL_RELATIVE)
    {
		//WaitForSingleObject(gDisplayEvent, 90);
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
	//WaitForSingleObject(gDisplayEvent, 90);
  switch(iCtrlMode){
		case    CTRL_TYPE_FADER_VERT:
			HandleFaderMoveVert(hwnd, pnt, wKeyFlags, lpmwd);
			break;
		case    CTRL_TYPE_FADER_HORZ:
			HandleFaderMoveHorz(hwnd, pnt, wKeyFlags, lpmwd);
			break;
  }
  // save the last mouse position
  // and then store the new one
  //-----------------------------
  lpmwd->pntMouseLast = lpmwd->pntMouseCur;
  lpmwd->pntMouseCur = pnt;
	SetEvent(gDisplayEvent);
  
  return;
  }

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
			if(lpczm)
					{
					lpmwd->lpCtrlZM = lpczm;
					lpmwd->iCurMode = MW_CONTROL_ACTIVE;
					}
			else
					return;

	ActivateMWMode(hwnd, lpmwd);
}

return;
}

//================================================
//FUNCTION: HandleMBDown
//
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
    StopMWMode(hwnd, lpmwd,  wKeyFlags);

return;
}
//================================================
//FUNCTION: HandleMBUp
//
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
    StopMWMode(hwnd, lpmwd,  wKeyFlags);

return;
}

//================================================
//FUNCTION: HandleRBUp
//
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
    StopMWMode(hwnd, lpmwd,  wKeyFlags);

return;
}

//==================================================
//FUNCTION: HandleWndSize
//
//
//
//==================================================
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

TurnOffAllVUforMixerWindow(lpmwd);

iWndMinWidth = GetSystemMetrics(SM_CXMIN);

iPrevStart = lpmwd->iStartScrChan;
iPrevEnd = lpmwd->iEndScrChan;

iStartScrChan = lpmwd->iStartScrChan;
iOldWidth = lpmwd->rWndPos.right;


// this is set to -1 only if we have just
// isued the resize command
// so there is nothing more to do here
// but store the values and return
//---------------------------------------
//if(iOldWidth == -1)
//    {
//    lpmwd->rVisible.right = wWidth;// - (GetSystemMetrics(SM_CXFRAME)*2);
//    return;
//    }

CALC_WIDTH:
lZMCount = lpmwd->lZMCount;
iCX = 0;
//for(iCount=iStartScrChan; iCount < lZMCount; iCount++)
for(iCount=0; iCount < lZMCount; iCount++)
    {
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

// Check it not to be below the minimum Width for the Window
//-----------------------------------------------------------
if(iCX <  iWndMinWidth)
    {
    wWidth = iWndMinWidth + iW + 1;
    iWndMinWidth = 0; // Make it 0 .. to avoid endless loop
    goto CALC_WIDTH;
    }

iCount--;

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


// Store the new size of the visible window
//-----------------------------------------
lpmwd->rVisible.right = iCX;
lpmwd->rVisible.bottom = wHeight - HEIGHT_FULL_LABEL_WND;

iAdjWidth = iCX+(GetSystemMetrics(SM_CXFRAME)*2);
iAdjHeight = wHeight + GetSystemMetrics(SM_CYFRAME) +
                       (GetSystemMetrics(SM_CYBORDER)*4) +
                       GetSystemMetrics(SM_CYCAPTION);


lpmwd->rWndPos.right = iAdjWidth;
lpmwd->rWndPos.bottom = iAdjHeight;

// Size it only if it has changed the ammount of
// visible Chanels. Here we care only about
// the width.
//----------------------------------------------
//if((wWidth != iCX) || (iAdjWidth == lpmwd->rMaxSize.right))
//    {
//    lpmwd->rVisible.right = -1; // use it as a flag so the second time arround
                                // we will not get to here
    SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, iAdjWidth,
                 iAdjHeight, SWP_NOMOVE | SWP_NOZORDER);
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
//    }

  //RequestVisibleVU(lpmwd, iPrevStart, iPrevEnd);
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

	// Check if we are draggin a channel, if so draw a RED rectangle

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

	// Now Draw the highlite rectangle
	//----------------------------
	Rectangle(hdc, rChan.left, rChan.top, rChan.right, rChan.bottom);


	// Clean up and exit

	ON_EXIT:
	SelectObject(hdc, hpenOld);
	DeleteObject(hpen);
	if(hpenSel)
			DeleteObject(hpenSel);

	ReleaseDC(hwnd, hdc);
return;
}

//==================================================
// FUNCTION: HandleCtrlTimer
//
//
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
    case CTRL_TYPE_LEFTRIGHT:
      LeftRightControl(NULL, lpmwd->lpCtrlZM, 0, lpmwd, LOBYTE(lpmwd->lpwRemapToScr[lpmwd->iCurChan + lpmwd->iStartScrChan]));
      break;
    }
  }
else
  if(lpmwd->iCurMode != MW_NOTHING_MODE)
    switch(lpmwd->iCurMode)
      {
      case MW_SCROLL_RELATIVE:
        HandleScrollImgWindowRelative(hwnd, lpmwd);
        break;
      }


return;
}

//==================================================
//FUNCTION: HandleFaderMoveVert
//
//
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
//
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




//  Now update all of the other
// controls that have the same
// iCtrlNum and are Capable of
// display on this Mixer_Window
//-----------------------------
UpdateControlsByCtrlNum(hdc, g_hdcMemory, lpmwd, lpmwd->iXadj, iPhisChannel, 
                        lpctrlZM, iVal, DIRECTIONS_ALL, TRUE);

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


/////////////////////////////////
//FUNCTION: HandleCtrlBtnClick
//
//
//
extern	int		g_inputCueActiveCounter;

void      HandleCtrlBtnClick(HWND hwnd, LPMIXERWNDDATA lpmwd)
{
LPCTRLZONEMAP       lpctrlZM;
CONTROLDATA         ctrlData;
int                 iPhisChannel, iVal, ivalue;
HDC                 hdc;
RECT                r;
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

ivalue = CDef_GetCtrlMinVal(lpctrlZM->iCtrlNumAbs);

iVal = GETPHISDATAVALUE(lpmwd->iMixer, lpctrlZM, lpctrlZM->iCtrlChanPos);

if(iVal != ivalue)
  {
		bIsOn = TRUE;

		// Handle possible Filter buttons ... like MUTE and such
		//
		StartControlDataFilter(iPhisChannel, lpmwd, lpctrlZM, bIsOn, TRUE);

		// Do the Toggle stuff ..
		//
		CheckForToggleSwitches(lpmwd, lpctrlZM);

		ctrlData.wMixer   = lpmwd->iMixer;
		ctrlData.wChannel = lpctrlZM->iModuleNumber;//iPhisChannel;
		ctrlData.wCtrl    = lpctrlZM->iCtrlNumAbs; // we use this one since for the definition dll
		// ok we need to go down to the minimum value
		iVal = CDef_GetCtrlMaxVal(lpctrlZM->iCtrlNumAbs) - 1;
		iVal--;// skip this value since the control is already there
		if((IsCtrlPrePostFilter(lpctrlZM->iCtrlType) == FALSE) && (ctrlData.wCtrl < 0x8000))
		{
			for(iVal; iVal >= ivalue; iVal --)
				{
				// Send the Data out
				//------------------
				ctrlData.wVal     = iVal;
				SendDataToDevice(&ctrlData, (lpmwd->wKeyFlags & MK_SHIFT)?FALSE:TRUE,
												 lpctrlZM, -1, lpmwd, TRUE);
				}
		}
		iVal = ivalue;


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
else
  {
  bIsOn = FALSE;

  // Do the Toggle stuff ..
  //
  CheckForToggleSwitches(lpmwd, lpctrlZM);

  ctrlData.wMixer   = lpmwd->iMixer;
  ctrlData.wChannel = lpctrlZM->iModuleNumber;//iPhisChannel;
  ctrlData.wCtrl    = lpctrlZM->iCtrlNumAbs; // we use this one since for the definition dll
  // ok we need to go up to max value
  ivalue = CDef_GetCtrlMaxVal(lpctrlZM->iCtrlNumAbs);
  iVal   = CDef_GetCtrlMinVal(lpctrlZM->iCtrlNumAbs);
  iVal++;// skip this value since the control is already there
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
  // Handle possible Filter buttons ... like MUTE and such
  //
  StartControlDataFilter(iPhisChannel, lpmwd, lpctrlZM, bIsOn, TRUE);
  }

// Set the Phisical Data Value
//----------------------------
SETPHISDATAVALUE(lpmwd->iMixer, lpctrlZM, lpctrlZM->iCtrlChanPos, iVal);

// update the screen part
//-----------------------
r = lpctrlZM->rZone;
hdc = GetDC(hwnd);
if(iVal == CDef_GetCtrlMinVal(lpctrlZM->iCtrlNumAbs))
  {
  PushBtn(g_hdcMemory, lpctrlZM, iVal, lpmwd, iPhisChannel);
  BitBlt(hdc, r.left + lpmwd->iXadj,
              r.top - lpmwd->iYOffset,
              r.right - r.left,
              r.bottom - r.top,
         g_hdcMemory, r.left, r.top,
         SRCCOPY);
  // now update all of the other mixers
  // windows that represent this mixer
  // using the iMixer, iPhisChannel
  // and iVal
  //-----------------------------------
  UpdateSameMixWndByCtrlNum(hwnd, lpmwd->iMixer, iPhisChannel, lpctrlZM, iVal, NULL);
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


  // now update all of the other mixers
  // windows that represent this mixer
  // using the iMixer, iPhisChannel
  // and iVal
  //-----------------------------------
  UpdateControlsByCtrlNum(hdc, g_hdcMemory, lpmwd, lpmwd->iXadj, 
                          iPhisChannel, lpctrlZM, iVal, DIRECTIONS_ALL, TRUE);
  UpdateSameMixWndByCtrlNum(hwnd, lpmwd->iMixer, iPhisChannel, lpctrlZM, iVal, g_hdcBuffer);

  SelectObject(g_hdcBuffer, hbmp);
  }

ReleaseDC(hwnd, hdc);
syncInputPriority (lpctrlZM, g_inputCueActiveCounter, lpmwd);

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

//=============================================
//FUNCTION: UpdateControl
//
//purpose:
//      Updates a single control.
//
//=============================================
int        UpdateControl(LPMIXERWNDDATA lpmwnd)
{




return 0;
}



///////////////////////////////////////////////
// FUNCTION: UpdateControlFromNetwork
//
//
void  UpdateControlFromNetwork(WORD iPhisChannel, WORD iCtrlAbs, int iVal, BOOL bIsAbsVal) 
{
  LPCTRLZONEMAP lpctrlZM;
  HDC           hdcBuffer;
  HBITMAP       hbmp;
  RECT          r;
  int           iCount;

  
  // Check for the  Matrix modules and flip them to the Aux modules
  // Since the program *thinks* they reside on the same Module
  //
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

  if(bIsAbsVal)
    lpctrlZM = ScanCtrlZonesAbs(gpZoneMaps_Zoom[iPhisChannel].lpZoneMap, iCtrlAbs);
  else
    lpctrlZM = ScanCtrlZonesNum(gpZoneMaps_Zoom[iPhisChannel].lpZoneMap, iCtrlAbs);

  // Force a search in the Master module if we couldn't find the Control Zone
  // it better be there
  //
  if(lpctrlZM == NULL)
  {
    if(bIsAbsVal)
      lpctrlZM = ScanCtrlZonesAbs(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, iCtrlAbs);
    else
      lpctrlZM = ScanCtrlZonesNum(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, iCtrlAbs);
  }
	else	/// if(lpctrlZM != NULL)
  {
    // Set the Phisical Data Value
    //----------------------------
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


		// This actually displays the entire movement of the faders
		// Without this line the faders will 'jump' into their final position

		// now update all of the other mixers
		// windows that represent this mixer
		// using the iMixer, iPhisChannel
		// and iVal

		// THIS really slows things down
		// Doesn't seem to be needed????

    UpdateSameMixWndByCtrlNum(NULL, 0, iPhisChannel, lpctrlZM, iVal, NULL);

  }
}