//=================================================
//
// Copyright 1998-2001, CorTek Software, Inc
//=================================================
//
// $Author: $
// $Archive: $
// $Revision: $
//
//
//=================================================
// Controls drawing and handling routines
// These routines in general get called by 
// DrawControls() that determines if the controls
// are visible and should be drawn.
//=================================================

#include "SAMM.h"
#include "SAMMEXT.h"
#include "MACRO.h"

#include "ConsoleDefinition.h"

void TurnOffAllVUforMixerWindow(LPMIXERWNDDATA lpmwd);
LPSTR   GetLabelText(LPCTRLZONEMAP lpctrlZM, int iChan);
int isCtrlValueNotEqualToDefault (LPCTRLZONEMAP   lpctrlZM, int ctrlNum);
void HandleCueMasterMuteFilterEx(int iPhisChann, LPMIXERWNDDATA lpmwd, 
                                  LPCTRLZONEMAP lpctrlZM, BOOL bIsOn);

extern int                 g_aiAux[MAX_MATRIX_COUNT];

//==================================================
//FUNCTION: DrawVertFader
//
// This routine is called to draw
// the Veritical Faders
//
//==================================================
void    DrawVertFader(HDC hdc, LPCTRLZONEMAP lpctrlZM, int iVal, LPMIXERWNDDATA lpmwd, int iChan)
{
HBITMAP         hbmpOld;
HBITMAP         hbmp;
int             iHeight, iWidth;
RECT            rZone;
int             iTop, iLeft;
int             iPhisChannel;
int             iBMPIndex;

	// Get this Control Zone
	//----------------------
	rZone = lpctrlZM->rZone;

	//iPhisChannel = LOWORD(lpmwd->lpwRemapToScr[lpmwd->iCurChan]);
	iPhisChannel = iChan;
	iBMPIndex = lpmwd->lpZoneMap[iPhisChannel].iBmpIndx;
	// Select the Original Bitmap
	//---------------------------
	hbmp = SelectObject(g_hdcTempBuffer, gpBMPTable[iBMPIndex].hbmp);

	// copy the Control bitmap to the memory
	//--------------------------------------
	BitBlt(hdc, rZone.left, rZone.top, rZone.right - rZone.left, rZone.bottom - rZone.top,
				 g_hdcTempBuffer, rZone.left, rZone.top, SRCCOPY);

	// Clean the buffer so others can use it freely
	//---------------------------------------------
	SelectObject(g_hdcTempBuffer, hbmp);

	// Get this Controls Bitmap
	//-------------------------
	hbmp = gpBMPTable[lpctrlZM->iCtrlBmp[0]].hbmp;
	iHeight = gpBMPTable[lpctrlZM->iCtrlBmp[0]].iHeight;
	iWidth  = gpBMPTable[lpctrlZM->iCtrlBmp[0]].iWidth;

	hbmpOld = SelectObject(g_hdcTempBuffer, hbmp);

	if(lpctrlZM->iNumValues > 0)
		CONVERTPHISICALTOSCREEN(lpctrlZM, iVal);
	else
		iVal = 0;

	// Calculate the Vertical Position
	//--------------------------------
	iTop = rZone.top + iVal;

	// Calculate the Horizontal position
	//----------------------------------
	iLeft = rZone.left +(((rZone.right-rZone.left)-iWidth)/2);

	BitBlt(hdc, iLeft, iTop, iWidth, iHeight,
				 g_hdcTempBuffer, 0, 0, SRCCOPY);

	SelectObject(g_hdcTempBuffer, hbmpOld);
	return;
}

//==================================================
//FUNCTION: DrawHorizFader
//
// This routine is called to draw
// the Horizontal Faders
//
//
//==================================================
void    DrawHorizFader(HDC hdc, LPCTRLZONEMAP lpctrlZM, int iVal, LPMIXERWNDDATA lpmwd, int iChan)
{
	HBITMAP         hbmpOld;
	HBITMAP         hbmp;
	int             iHeight, iWidth;
	RECT            rZone;
	int             iTop, iLeft;
	int             iPhisChannel;
	int             iBMPIndex;

	// Get this Control Zone
	//----------------------
	rZone = lpctrlZM->rZone;

	iPhisChannel = iChan;
	iBMPIndex = lpmwd->lpZoneMap[iPhisChannel].iBmpIndx;
	// Select the Original Bitmap
	//---------------------------
	hbmp = SelectObject(g_hdcTempBuffer, gpBMPTable[iBMPIndex].hbmp);

	// copy the Control bitmap to the memory
	//--------------------------------------
	BitBlt(hdc, rZone.left, rZone.top, rZone.right - rZone.left, rZone.bottom - rZone.top,
				 g_hdcTempBuffer, rZone.left, rZone.top, SRCCOPY);

	// Clean the buffer so others can use it freely
	//---------------------------------------------
	SelectObject(g_hdcTempBuffer, hbmp);

	// Get this Controls Bitmap
	//-------------------------
	hbmp = gpBMPTable[lpctrlZM->iCtrlBmp[0]].hbmp;
	iHeight = gpBMPTable[lpctrlZM->iCtrlBmp[0]].iHeight;
	iWidth  = gpBMPTable[lpctrlZM->iCtrlBmp[0]].iWidth;

	hbmpOld = SelectObject(g_hdcTempBuffer, hbmp);


	// Calculate the Horizontal Position
	//----------------------------------
	if(lpctrlZM->iNumValues > 0)
		CONVERTPHISICALTOSCREEN(lpctrlZM, iVal);
	else
		iVal = 0;

	// do the Calculation using LONG instead of float
	//-----------------------------------------------
	iLeft = rZone.left + iVal;//(int)lValue;

	// Calculate the horizontal position
	//----------------------------------
	iTop = rZone.top +(((rZone.bottom-rZone.top)-iHeight)/2);

	BitBlt(hdc, iLeft, iTop, iWidth, iHeight,
				 g_hdcTempBuffer, 0, 0, SRCCOPY);

	SelectObject(g_hdcTempBuffer, hbmpOld);

	return;
}

//==================================================
//FUNCTION: DrawVUData
//
// This routine is called to draw
// the VU data
//
//
//==================================================
void    DrawVUData(HDC hdc, LPCTRLZONEMAP lpctrlZM, VU_READ *pVuData, LPMIXERWNDDATA lpmwd, int iChan, int iVUtype)
{
  HBITMAP         hbmpOld;
  HBITMAP         hbmp;
  int             iHeight, iWidth;
  RECT            rZone;
  int             iTop, iLeft;
  int             iPhisChannel;
  HDC             hdcLocal;
  int             iVal;
  WORD            wVUType;
  int             iModuleType;


  iModuleType = gDeviceSetup.iaChannelTypes[lpctrlZM->iModuleNumber];
  
  //???? Handle Master VU data also ..... ????
  if((lpctrlZM->iCtrlChanPos == CTRL_NUM_INPUT_VU_METER) && 
     ( iModuleType == DCX_DEVMAP_MODULE_INPUT) )
  {
    wVUType = (GETPHISDATAVALUE(0, lpctrlZM, CTRL_NUM_INPUT_PREPOST_FADER_VU)>0)?0:2;
  }
  else
  {
    if(iModuleType == DCX_DEVMAP_MODULE_MASTER)
    {
      if(lpctrlZM->iCtrlType == CTRL_TYPE_VU_DISPLAY2)
        wVUType = (GETPHISDATAVALUE(0, lpctrlZM, CTRL_NUM_MASTER_POST_RT_VU)>0)?2:6;
      else
        if(lpctrlZM->iCtrlType == CTRL_TYPE_VU_DISPLAY)
          wVUType = (GETPHISDATAVALUE(0, lpctrlZM, CTRL_NUM_MASTER_POST_LT_VU)>0)?0:4;
        else
          wVUType = lpctrlZM->iCtrlType - CTRL_TYPE_VU_DISPLAY;
    }
    else
      if(iModuleType == DCX_DEVMAP_MODULE_MATRIX)
      {
        if(lpctrlZM->iCtrlType == CTRL_TYPE_VU_DISPLAY2)
          wVUType = (GETPHISDATAVALUE(0, lpctrlZM, CTRL_NUM_SUB_POST_RT_VU)>0)?2:6;
        else
          if(lpctrlZM->iCtrlType == CTRL_TYPE_VU_DISPLAY)
            wVUType = (GETPHISDATAVALUE(0, lpctrlZM, CTRL_NUM_SUB_POST_LT_VU)>0)?0:4;
          else
            if(lpctrlZM->iCtrlType == CTRL_TYPE_VU_DISPLAY10)
              wVUType = (GETPHISDATAVALUE(0, lpctrlZM, CTRL_NUM_MATRIX_POST_RT_VU)>0)?2:6;
            else
              if(lpctrlZM->iCtrlType == CTRL_TYPE_VU_DISPLAY8)
                wVUType = (GETPHISDATAVALUE(0, lpctrlZM, CTRL_NUM_MATRIX_POST_LT_VU)>0)?0:4;
              else
                wVUType = lpctrlZM->iCtrlType - CTRL_TYPE_VU_DISPLAY;
      }
    else
      wVUType = lpctrlZM->iCtrlType - CTRL_TYPE_VU_DISPLAY;
  }
  
  if(wVUType > 7)
    wVUType -= 8;

  // Get this Control Zone
  //----------------------
  rZone = lpctrlZM->rZone;
  iPhisChannel = iChan;

  // Get this Controls Bitmap
  //-------------------------
  hbmp = gpBMPTable[lpctrlZM->iCtrlBmp[0]].hbmp;
  iHeight = gpBMPTable[lpctrlZM->iCtrlBmp[0]].iHeight;
  iWidth  = gpBMPTable[lpctrlZM->iCtrlBmp[0]].iWidth;

  hdcLocal = CreateCompatibleDC(hdc);
  hbmpOld = SelectObject(g_hdcBuffer, hbmp);
  
	// Get the VU data value
	//
  iVal = pVuData->wVUValue[wVUType];

  // Validate index
  //
  if(iVal > 4095)   
    iVal = 4095;
  if(iVal < 0)  
    iVal = 0;

  switch(lpctrlZM->iCtrlChanPos)
  {

  case CTRL_NUM_INPUT_COMP_VU:
    iVal = gVU_CompDispTable[iVal];
    // Calculate the Vertical Position
    //--------------------------------
    iTop = rZone.top;
    // Calculate the Horizontal position
    //----------------------------------
    iLeft = rZone.left - iVal;
    break;
  case CTRL_NUM_INPUT_GATE_VU:
    iVal = gVU_GateDispTable[iVal];
    // Calculate the Vertical Position
    //--------------------------------
    iTop = rZone.top;
    // Calculate the Horizontal position
    //----------------------------------
    iLeft = rZone.left - iVal;
    break;

  case CTRL_NUM_INPUT_VU_METER:
  default:
    iVal = gVU_VertDispTable[iVal];
    // Calculate the Vertical Position
    //--------------------------------
    iTop = rZone.bottom - iVal;
    // Calculate the Horizontal position
    //----------------------------------
    iLeft = rZone.left;
    break;
  }


  // this is Input module so go ahead and display the Peak and the Average data
  //
  if((lpctrlZM->iCtrlChanPos == CTRL_NUM_INPUT_VU_METER) &&
    (gDeviceSetup.iaChannelTypes[lpctrlZM->iModuleNumber] == 1) )
  {
    BitBlt(hdc, 0, iHeight - iVal, iWidth/2, iVal, g_hdcBuffer, 0, iHeight - iVal, SRCCOPY);

    iVal = pVuData->wVUValue[wVUType + 1];
    // Validate index
    //
    if(iVal > 4095)   
      iVal = 4095;
    if(iVal < 0)  
      iVal = 0;

    iVal = gVU_VertDispTable[iVal];

      // Calculate the Vertical Position
    //--------------------------------
    iTop = rZone.bottom - iVal;

    // Calculate the Horizontal position
    //----------------------------------
    BitBlt(hdc, iWidth/2, iHeight - iVal, iWidth/2, iVal, g_hdcBuffer, 0, iHeight - iVal, SRCCOPY);
  }
  else
    if( ((lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_VU_METER) &&
       (iModuleType == DCX_DEVMAP_MODULE_MASTER) || (iModuleType  == DCX_DEVMAP_MODULE_CUE)) 
        ||
        ( ((lpctrlZM->iCtrlChanPos == CTRL_NUM_MATRIX_VU_METER) || 
          (lpctrlZM->iCtrlChanPos == CTRL_NUM_MATRIX2_VU_METER)) 
          &&
          (iModuleType  == DCX_DEVMAP_MODULE_MATRIX)) )
    {
      iVal = pVuData->wVUValue[wVUType];
      // Calculate the Vertical Position
      //--------------------------------
      if(iVal > 4095)iVal = 4095;
      if(iVal < 0)iVal = 0;
      iVal = gVU_VertDispTable[iVal];
      iTop = rZone.bottom - iVal;
      BitBlt(hdc, 0, iHeight - iVal, iWidth/2, iVal, g_hdcBuffer, 0, iHeight - iVal, SRCCOPY);


      iVal = pVuData->wVUValue[wVUType + 1];
      if(iVal > 4095)iVal = 4095;
      if(iVal < 0)iVal = 0;


      // Calculate the Vertical Position
      //--------------------------------
      iVal = gVU_VertDispTable[iVal];
      iTop = rZone.bottom - iVal;

      // Calculate the Horizontal position
      //----------------------------------
      BitBlt(hdc, iWidth/2, iHeight - iVal, iWidth/2, iVal, g_hdcBuffer, 0, iHeight - iVal, SRCCOPY);

    }
    else
    {
      if((lpctrlZM->iCtrlChanPos == CTRL_NUM_INPUT_GATE_VU) ||
        (lpctrlZM->iCtrlChanPos == CTRL_NUM_INPUT_COMP_VU))
      {
        // draw it  Lef to Right
        //
        BitBlt(hdc, iWidth - iVal, 0, iVal, iHeight, g_hdcBuffer, iWidth - iVal, 0, SRCCOPY);
      }
      else
      {
      // draw it  Normaly !!!
      //
      BitBlt(hdc, 0, iHeight - iVal, iWidth, iVal, g_hdcBuffer, 0, iHeight - iVal, SRCCOPY);
      }
    }

  // Cleanup stuff ..
  // !!!!
  SelectObject(g_hdcBuffer, hbmpOld);
  DeleteDC(hdcLocal);

  return;
}

//==================================================
//FUNCTION: PushBtn
//
// This routine is called to draw
// the push buttons
//
//
//==================================================
void    PushBtn(HDC hdc, LPCTRLZONEMAP lpczm, int iVal, LPMIXERWNDDATA lpmwd, int iChan)
{
HBITMAP         hbmpOld;
HBITMAP         hbmp;
int             iHeight, iWidth;
RECT            rZone;
int             ivalue;
int             iBMPIndex;

	// Get this Control Zone
	//----------------------
	rZone = lpczm->rZone;

	// Check the Value
	//----------------
	ivalue = CDef_GetCtrlMinVal(lpczm->iCtrlNumAbs);
	if(ivalue == iVal)
  {
    // Get this Controls Bitmap
    //-------------------------
    hbmp = gpBMPTable[lpczm->iCtrlBmp[1]].hbmp;
    iHeight = gpBMPTable[lpczm->iCtrlBmp[1]].iHeight;
    iWidth  = gpBMPTable[lpczm->iCtrlBmp[1]].iWidth;

		hbmpOld = SelectObject(g_hdcTempBuffer, hbmp);

    // Put the Bitmap on the Screen
    //-----------------------------
    BitBlt(hdc, rZone.left, rZone.top, iWidth, iHeight,
					 g_hdcTempBuffer, 0, 0, SRCCOPY);

		SelectObject(g_hdcTempBuffer, hbmpOld);
	}
	else
	{
		iBMPIndex = lpmwd->lpZoneMap[iChan].iBmpIndx;

		// Select the Original Bitmap
		//---------------------------
		hbmpOld = SelectObject(g_hdcTempBuffer, gpBMPTable[iBMPIndex].hbmp);

		// copy the Control bitmap to the memory
		//--------------------------------------
		BitBlt(hdc, rZone.left, rZone.top, 
								rZone.right - rZone.left, 
								rZone.bottom - rZone.top,
					 g_hdcTempBuffer, rZone.left, rZone.top, SRCCOPY);
		// Clean the buffer so others can use it freely
		//---------------------------------------------
		SelectObject(g_hdcTempBuffer, hbmpOld);

  }

	return;
}


//==================================================
//FUNCTION: OpenExplodeWindow
//
//
//
//==================================================
void    OpenExplodeWindow(HDC hdc, LPCTRLZONEMAP lpczm, int iVal,
                          LPMIXERWNDDATA lpmwd, int iChan)
{


return;
}


////////////////////////////////
//
// DELETE DELETE WHEN NEW CODE AT END IS WORKING
// PER GAMBLE

//#define ORIGINAL

#ifdef ORIGINAL
//==================================================
//FUNCTION: JumpToMZWindow
//
//
//purpose:
//  Jump to a Mixer Zoom Window with a particular
// Y offset
//==================================================
void    JumpToMZWindow(HDC hdc, LPCTRLZONEMAP lpczm, int iVal,
                            LPMIXERWNDDATA lpmwd, int iChan)
{
LPMIXERWNDDATA      lpMWD;
int				iYOffset = 0;
int				iScrVisible = 0;


// ?????????????????????
// ?????????????????????
//

	switch(lpczm->iCtrlType)
	{
  case    CTRL_TYPE_OPEN_ZOOM_IAUX:
		iYOffset = JUMP_TO_INPUT_AUX;
		break;
	case		CTRL_TYPE_OPEN_ZOOM_CCOMP:
		iYOffset = JUMP_TO_INPUT_COMP;
		break;
	case		CTRL_TYPE_OPEN_ZOOM_EQ:
		iYOffset = JUMP_TO_INPUT_EQ;
		break;
	case CTRL_TYPE_OPEN_ZOOM_SUB:
		iYOffset = JUMP_TO_INPUT_SUB;
		break;
	case CTRL_TYPE_OPEN_FADER:
		iYOffset = JUMP_TO_INPUT_FADER;
		break;
	case	CTRL_TYPE_OPEN_MATRIX_EQ:
		iYOffset = JUMP_TO_MATRIX_EQ;
		break;
	case	CTRL_TYPE_OPEN_MATRIX_AUX:
		iYOffset = JUMP_TO_MATRIX_AUX;
		break;
	case	CTRL_TYPE_OPEN_MATRIX_SUB:
		iYOffset = JUMP_TO_MATRIX_SUB;
		break;
	default:
		iYOffset = 4000;
		break;
	}


	if(ghwndZoom == NULL)
  {
    lpMWD = MixerWindowDataAlloc(gwActiveMixer,
                                 gpZoneMaps_Zoom,
                                 MAX_CHANNELS, 1);
    if(lpMWD == NULL)
    {
        ErrorBox(ghwndMain, ghInstStrRes, IDS_ERR_ALLOCATE_MEMORY);
        return;
    }
    else
    {


        // Set the Window Start channel
        // and Remap table, and start channel
        // and End Channel
        //-----------------------------------
        CopyMemory(lpMWD->lpwRemapToScr, lpmwd->lpwRemapToScr, MAX_CHANNELS * sizeof(WORD));
        CopyMemory(lpMWD->lpwRemapToPhis, lpmwd->lpwRemapToPhis, MAX_CHANNELS * sizeof(WORD));

				iScrVisible	= lpMWD->iEndScrChan - lpMWD->iStartScrChan;

        lpMWD->iYOffset = iYOffset;//gpRdOutTable[lpczm->iRdOutIndx].lpRdOut->iDefault;
				
				iScrVisible	= lpMWD->iEndScrChan - lpMWD->iStartScrChan;
				lpMWD->iEndScrChan = lpmwd->iCurChan + iScrVisible;

				if(lpMWD->iEndScrChan >= lpmwd->lZMCount)
					lpMWD->iEndScrChan = lpMWD->lZMCount - 1;

				lpMWD->iStartScrChan = lpMWD->iEndScrChan - iScrVisible;
        //lpMWD->iEndScrChan = lpmwd->iCurChan + lpMWD->iEndScrChan - lpMWD->iStartScrChan;// lpmwd->iEndScrChan;
        //lpMWD->iStartScrChan = lpmwd->iCurChan;
        lpMWD->iCurChan = lpmwd->iCurChan;
				
				//GetMaxWindowSize(&lpMWD->rMaxWndPos, lpMWD->lpZoneMap, lpMWD->lZMCount, lpMWD);

        wsprintf(lpMWD->szTitle, "Zoom View (Link)");
        ghwndZoom = CreateZoomViewWindow(ghwndMDIClient,"Zoom View (Link)", lpMWD, 1);

     }
	}
	else
    
	{

    lpMWD = (LPMIXERWNDDATA)GetWindowLong(ghwndZoom, 0);
    if(lpMWD)
    {
				TurnOffAllVUforMixerWindow(lpMWD);

				// Set the Window Start channel
        // and Remap table, and start channel
        // and End Channel
        //-----------------------------------
        CopyMemory(lpMWD->lpwRemapToScr, lpmwd->lpwRemapToScr, MAX_CHANNELS * sizeof(WORD));
        CopyMemory(lpMWD->lpwRemapToPhis, lpmwd->lpwRemapToPhis, MAX_CHANNELS * sizeof(WORD));
        
        
				iScrVisible	= lpMWD->iEndScrChan - lpMWD->iStartScrChan;
				lpMWD->iEndScrChan = lpmwd->iCurChan + iScrVisible;

				if(lpMWD->iEndScrChan >= lpmwd->lZMCount)
					lpMWD->iEndScrChan = lpMWD->lZMCount - 1;

				lpMWD->iStartScrChan = lpMWD->iEndScrChan - iScrVisible;
        lpMWD->iCurChan = lpmwd->iCurChan;

        InvalidateRect(lpMWD->hwndLblGroup, NULL, FALSE);
        UpdateWindow(lpMWD->hwndLblGroup);
        
				lpMWD->pntMouseCur.y = iYOffset - lpMWD->iYOffset;
				lpMWD->pntMouseLast.y = 0;

				//lpMWD->iYOffset = iYOffset;//gpRdOutTable[lpczm->iRdOutIndx].lpRdOut->iDefault;

				ScrollImgWindow(ghwndZoom, lpMWD);

        InvalidateRect(lpMWD->hwndImg, NULL, FALSE);
        UpdateWindow(lpMWD->hwndImg);

			  RequestVisibleVU(lpMWD, -1, -1);

    }
	}


return;
}

#endif

//==================================================
//FUNCTION: RdOutText
//
//
// Update Only the Text in the RdOut
//
//==================================================
void    RdOutText(HDC hdc, LPCTRLZONEMAP lpczm, int iVal, LPMIXERWNDDATA lpmwd, int iChan)
{
  RECT            r;
  char            szRdOut[32];
  BOOL            bText = FALSE;
  int             ivalue;
  HBITMAP         hbmp;
  int             iBMPIndex;
  HFONT           hfont;
	HBRUSH					hBrush;

  ZeroMemory(szRdOut, 32);

  if(lpczm->iCtrlNumAbs < 0)
  {

		//////////////////////////////////////////////
		// Handle the special case for the Cue A/B
		// system and toggle the text depending
		// on the which cue is selected

    if((lpczm->iCtrlChanPos == CTRL_NUM_MASTER_CUEB_SYSTEM_SEL || lpczm->iCtrlChanPos == CTRL_NUM_MASTER_CUEA_SYSTEM_SEL) &&
			  lpczm->iCtrlType == CTRL_TYPE_STRING_UPDATE)
    {
      bText = TRUE;	// Text only, no need to lookup value
      if(GETPHISDATAVALUE(0, lpczm, CTRL_NUM_MASTER_CUEB_SYSTEM_SEL) == 0 &&
        GETPHISDATAVALUE(0, lpczm, CTRL_NUM_MASTER_CUEA_SYSTEM_SEL) > 0 ) 
      {
        strcpy(szRdOut, "Cue System B");
        goto START_STRING_UPDATE;
      }
      else
      {
        if(GETPHISDATAVALUE(0, lpczm, CTRL_NUM_MASTER_CUEA_SYSTEM_SEL) == 0 &&
           GETPHISDATAVALUE(0, lpczm, CTRL_NUM_MASTER_CUEB_SYSTEM_SEL) > 0 ) 
        {
          strcpy(szRdOut, "Cue System A");
          goto START_STRING_UPDATE;
        }
      }

      return;
    }

    return;
  }

START_STRING_UPDATE:

	// Get this Control Zone
	//----------------------
	r = lpczm->rZone;

	//////////////////////////////////////////////////////////////////////////
	// If ONLY text then lets erase what is there before displaying the text
	// Currently ONLY used by the Cue A/B system to display the text, the
	// code below will clear the area for the text (all the time by the way)
	// These read out controns do not have a bitmap to select so if we don't
	// clear it then the text will eventually overwrite each other

	if(bText)
	{
			hBrush = CreateSolidBrush(PALETTERGB(156,223,206));
			FillRect(hdc,&r, hBrush);
			if(hBrush)
				DeleteObject(hBrush);

	}
	else
	{

		// Load the Module Bitmap in the Memory DC
		//----------------------------------------
		// iPhisChannel = LOWORD(lpmwd->lpwRemapToScr[lpmwd->iCurChan]);
		iBMPIndex = lpmwd->lpZoneMap[iChan].iBmpIndx;

		// Select the Original Bitmap
		//---------------------------
		hbmp = SelectObject(g_hdcBuffer, gpBMPTable[iBMPIndex].hbmp);

		// copy the Clean  bitmap to the memory
		//--------------------------------------

			BitBlt(hdc, r.left, r.top, r.right - r.left, r.bottom - r.top,
					 g_hdcBuffer, r.left, r.top, SRCCOPY);

		SelectObject(g_hdcBuffer, hbmp);

	// Check if we are only showing text, else get the value to show

		// Check the Value
		//-----------------
		ivalue = CDef_GetCtrlMaxVal(lpczm->iCtrlNumAbs);
		if(ivalue < iVal)
				iVal = ivalue;
		ivalue = CDef_GetCtrlMinVal(lpczm->iCtrlNumAbs);
		if(ivalue > iVal)
				iVal = ivalue;

		CDef_GetCtrlReadout(lpczm->iCtrlNumAbs, iVal, szRdOut);
	}

  hfont = SelectObject(hdc, g_hConsoleFont);
  WriteTextToDC(hdc, &r, 0, 0, RGB(0, 0, 0), szRdOut);
  SelectObject(hdc, hfont);
  return;
}

//==================================================
//FUNCTION: ChannelNameTextVertical
//
//
// Display the channel name Verticaly
//
//==================================================
void    ChannelNameTextVertical(HDC hdc, LPCTRLZONEMAP lpczm, int iVal, LPMIXERWNDDATA lpmwd, int iChan)
{
  RECT            r;
  char            szRdOut[64];
  BOOL            bText = FALSE;
  HBITMAP         hbmp;
  //int             iPhisChannel;
  int             iBMPIndex;
  HFONT           hfont;
	LPSTR						plabel;
	int							i=0;

  ZeroMemory(szRdOut, 64);


  // Get this Control Zone
  //----------------------
  r = lpczm->rZone;

	plabel = GetLabelText(lpczm, iChan);

	if(plabel == NULL)
		return;

	while (*plabel != 0)
	{
		szRdOut[i] = *plabel;	i++;
		//szRdOut[i] = 13; i++;
		szRdOut[i] = 10; i++;

		plabel++;
	}

  // Load the Module Bitmap in the Memory DC
  //----------------------------------------
  //iPhisChannel = LOWORD(lpmwd->lpwRemapToScr[lpmwd->iCurChan]);
  iBMPIndex = lpmwd->lpZoneMap[iChan].iBmpIndx;

  // Select the Original Bitmap
  //---------------------------
  hbmp = SelectObject(g_hdcBuffer, gpBMPTable[iBMPIndex].hbmp);

  // copy the Clean  bitmap to the memory
  //--------------------------------------
  BitBlt(hdc, r.left, r.top, r.right - r.left, r.bottom - r.top,
         g_hdcBuffer, r.left, r.top, SRCCOPY);
  SelectObject(g_hdcBuffer, hbmp);


  hfont = SelectObject(hdc, g_hConsoleFont);
  WriteTextLinesToDCVerticaly(hdc, &r, 0, 0, RGB(0, 0, 0), szRdOut);
  SelectObject(hdc, hfont);
  return;
}

//==================================================
//FUNCTION: ChannelNameTextVertical
//
//
// Display the channel name Verticaly
//
//==================================================
void    ChannelNumberTextVertical(HDC hdc, LPCTRLZONEMAP lpczm, int iVal, LPMIXERWNDDATA lpmwd, int iChan)
{
  RECT            r;
  char            szRdOut[64], chBuff[64];
  BOOL            bText = FALSE;
  HBITMAP         hbmp;
  //int             iPhisChannel;
  int             iBMPIndex;
  HFONT           hfont;
	LPSTR						plabel;
	int							i=0, iic;

  ZeroMemory(szRdOut, 64);
  ZeroMemory(chBuff, 64);


  // Get this Control Zone
  //----------------------
  r = lpczm->rZone;

	if( gDeviceSetup.iaChannelTypes[lpczm->iModuleNumber] == 2 )
	{
    for(iic = 0; iic < MAX_AUX_CHANNELS; iic++)
    {
      if(g_aiAux[iic] == lpczm->iModuleNumber)
      {
        break;
      }
    }
		wsprintf(chBuff, "%02d", iic + 1);
	}
	else
		wsprintf(chBuff, "%02d", lpczm->iModuleNumber + 1);

	plabel = &chBuff[0];

	if(plabel == NULL)
		return;

	while (*plabel != 0)
	{
		szRdOut[i] = *plabel;	i++;
		//szRdOut[i] = 13; i++;
		szRdOut[i] = 10; i++;

		plabel++;
	}

  // Load the Module Bitmap in the Memory DC
  //----------------------------------------
  //iPhisChannel = LOWORD(lpmwd->lpwRemapToScr[lpmwd->iCurChan]);
  iBMPIndex = lpmwd->lpZoneMap[iChan].iBmpIndx;

  // Select the Original Bitmap
  //---------------------------
  hbmp = SelectObject(g_hdcBuffer, gpBMPTable[iBMPIndex].hbmp);

  // copy the Clean  bitmap to the memory
  //--------------------------------------
  BitBlt(hdc, r.left, r.top, r.right - r.left, r.bottom - r.top,
         g_hdcBuffer, r.left, r.top, SRCCOPY);
  SelectObject(g_hdcBuffer, hbmp);


  hfont = SelectObject(hdc, g_hConsoleFont);
  WriteTextLinesToDCVerticaly(hdc, &r, 0, 0, RGB(0, 0, 0), szRdOut);
  SelectObject(hdc, hfont);
  return;
}



//===============================================
//FUNCTION: UpDownControl
//
//
//note:
//  All we need should be already set in the lpmwd ...
// also this control will not display anything
// so it should respond only if hdc == NULL
//===============================================

void    UpDownControl(HDC hdc, LPCTRLZONEMAP lpctrlZM, int iVal, LPMIXERWNDDATA lpmwd, int iChan)
{
int                 iPhisChannel, iV, ivalue;
RECT                r;
CONTROLDATA         ctrlData;
HDC                 hdcScr;//, hdcMem;
int									sum_in_flag = 0;

	if( hdc )
		return;

	// Handle the button Up down
	//--------------------------
	//iPhisChannel = LOWORD(lpmwd->lpwRemapToScr[lpmwd->iCurChan]);
	iPhisChannel = iChan;

	// get the zone map pointer
	//-------------------------
	lpctrlZM = lpmwd->lpCtrlZM;

	r = lpctrlZM->rZone; 

	iV = GETPHISDATAVALUE(lpmwd->iMixer, lpctrlZM, lpctrlZM->iCtrlChanPos);

	// Special case for Cue Sum-in. It should almost act like a Cue-button
	// Activate the Cue system when it is not OFF and activate the SCILENT 
	// Cue system when it is OFF.
	// Begin setting

	if (gDeviceSetup.iaChannelTypes[iPhisChannel] == DCX_DEVMAP_MODULE_MASTER && 
			lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_CUE_A_SUM_IN)
	{
		sum_in_flag = isCtrlValueNotEqualToDefault (lpctrlZM, CTRL_NUM_MASTER_CUE_A_SUM_IN);
	}


	// detect in which part of the zone is the mouse
	// and then go up or down ... simple stuff
	//----------------------------------------------
	if( (lpmwd->pntMouseCur.y + lpmwd->iYOffset) < (r.top + ((r.bottom-r.top)/2)) )
		iV --;
	else
		iV ++;

	ivalue = CDef_GetCtrlMinVal(lpctrlZM->iCtrlNumAbs);
	if(ivalue > iV)
		iV = ivalue;
	ivalue = CDef_GetCtrlMaxVal(lpctrlZM->iCtrlNumAbs);
	if(ivalue <= iV)
		iV = ivalue - 1;

	// do not go further if value is the same
	//---------------------------------------
	ivalue = GETPHISDATAVALUE(lpmwd->iMixer, lpctrlZM, lpctrlZM->iCtrlChanPos);
	if(ivalue == iV)
		return;



	// Set the Phisical Data Value
	//----------------------------
	SETPHISDATAVALUE(lpmwd->iMixer, lpctrlZM, lpctrlZM->iCtrlChanPos, iV);

	// Send the Data out
	//------------------
	ctrlData.wMixer   = lpmwd->iMixer;
	ctrlData.wChannel = lpctrlZM->iModuleNumber;//iPhisChannel;
	ctrlData.wCtrl    = lpctrlZM->iCtrlNumAbs; // we use this one since for the definition dll
	ctrlData.wVal     = iV;
	SendDataToDevice(&ctrlData, (lpmwd->wKeyFlags & MK_SHIFT)?FALSE:TRUE, 
									 lpctrlZM, iV - ivalue, lpmwd, TRUE);

                                               
	// Special case for Cue Sum-in. It should almost act like a Cue-button
	// Activate the Cue system when it is not OFF and activate the SCILENT 
	// Cue system when it is OFF.

	if (gDeviceSetup.iaChannelTypes[iPhisChannel] == 5 && 
			lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_CUE_A_SUM_IN)
	{
		if (isCtrlValueNotEqualToDefault (lpctrlZM, CTRL_NUM_MASTER_CUE_A_SUM_IN))
		{
			if (sum_in_flag == 0)
				HandleCueMasterMuteFilterEx (iPhisChannel, lpmwd, lpctrlZM, TRUE);
		}
		else
			HandleCueMasterMuteFilterEx (iPhisChannel, lpmwd, lpctrlZM, FALSE);
	}

	//  Now update all of the other
	// controls that have the same
	// iCtrlNum and are Capable of
	// display on this Mixer_Window
	//-----------------------------
	hdcScr = GetDC(lpmwd->hwndImg);

	// NOTE: THIS IS ALSO CALLED AGAIN IN THE UpdateSameMixWndByCtrlNum() THAT FOLLOWS
	// NEED THIS TO UPDATE SELF, THIS CONTROL AND NEXT ARE HANDLED DIFFERENTLY
	// THAN THE OTHERS
	 UpdateControlsByCtrlNum(hdcScr, g_hdcMemory, lpmwd, lpmwd->iXadj, iPhisChannel, lpctrlZM, iV, DIRECTIONS_ALL, TRUE);


	// now update all of the other mixers
	// windows that represent this mixer
	// using the iMixer, iPhisChannel
	// and iVal
	//-----------------------------------
	UpdateSameMixWndByCtrlNum(lpmwd->hwndImg, lpmwd->iMixer, iPhisChannel, lpctrlZM, iV, NULL);

	ReleaseDC(lpmwd->hwndImg, hdcScr);

	return;
}



//===============================================
//FUNCTION: LeftRightControl
//
//
//note:
//      All we need should be already set in
//  the lpmwd ...
// also this control will not display anything
// so it should respond only if hdc == NULL
//===============================================
void    LeftRightControl(HDC hdc, LPCTRLZONEMAP lpctrlZM, int iVal, LPMIXERWNDDATA lpmwd, int iChan)
{
  int                 iPhisChannel, iV, ivalue;
  RECT                r;
  CONTROLDATA         ctrlData;
  HDC                 hdcScr;//, hdcMem;

  if( hdc )
    return;

  // Handle the button Up down
  //--------------------------
  iPhisChannel = iChan;

  // get the zone map pointer
  //-------------------------
  lpctrlZM = lpmwd->lpCtrlZM;

  r = lpctrlZM->rZone; 

  iV = GETPHISDATAVALUE(lpmwd->iMixer, lpctrlZM, lpctrlZM->iCtrlChanPos);

  // detect in which part of the zone is the mouse
  // and then go up or down ... simple stuff
  //----------------------------------------------
  if( (lpmwd->pntMouseCur.x - lpmwd->iXadj) < (r.left + ((r.right-r.left)/2)) )
    iV --;
  else
    iV ++;

  ivalue = CDef_GetCtrlMinVal(lpctrlZM->iCtrlNumAbs);
  if(ivalue > iV)
    iV = ivalue;
  ivalue = CDef_GetCtrlMaxVal(lpctrlZM->iCtrlNumAbs);
  if(ivalue <= iV)
    iV = ivalue - 1;

  // do not go further if value is the same
  //---------------------------------------
  ivalue = GETPHISDATAVALUE(lpmwd->iMixer, lpctrlZM, lpctrlZM->iCtrlChanPos);
  if(ivalue == iV)
    return;


  // Set the Phisical Data Value
  //----------------------------
  SETPHISDATAVALUE(lpmwd->iMixer, lpctrlZM, lpctrlZM->iCtrlChanPos, iV);

  // Send the Data out
  //------------------
  ctrlData.wMixer   = lpmwd->iMixer;
  ctrlData.wChannel = lpctrlZM->iModuleNumber;//iPhisChannel;
  ctrlData.wCtrl    = lpctrlZM->iCtrlNumAbs; // we use this one since for the definition dll
  ctrlData.wVal     = iV;
  SendDataToDevice(&ctrlData, (lpmwd->wKeyFlags & MK_SHIFT)?FALSE:TRUE, 
                   lpctrlZM, iV - ivalue, lpmwd, TRUE);
                                               

  //  Now update all of the other
  // controls that have the same
  // iCtrlNum and are Capable of
  // display on this Mixer_Window
  //-----------------------------
  hdcScr = GetDC(lpmwd->hwndImg);


	// NOTE: THIS IS ALSO CALLED AGAIN IN THE UpdateSameMixWndByCtrlNum() THAT FOLLOWS
	// NEED THIS TO UPDATE SELF, THIS CONTROL IS HANDLED DIFFERENTLY THAN OTHERS FOR NOW

  UpdateControlsByCtrlNum(hdcScr, g_hdcMemory, lpmwd, lpmwd->iXadj, iPhisChannel, lpctrlZM, iV, DIRECTIONS_ALL, TRUE);


  // now update all of the other mixers
  // windows that represent this mixer
  // using the iMixer, iPhisChannel
  // and iVal
  //-----------------------------------
  UpdateSameMixWndByCtrlNum(lpmwd->hwndImg, lpmwd->iMixer, iPhisChannel, lpctrlZM, iV, NULL);

  ReleaseDC(lpmwd->hwndImg, hdcScr);
 
  return;
}
  



#define MYCODE

#ifdef MYCODE

//==================================================
//FUNCTION: JumpToMZWindow
//
//
//purpose:
//  Jump to a Mixer Zoom Window with a particular
// Y offset
//==================================================
void    JumpToMZWindow(HDC hdc, LPCTRLZONEMAP lpczm, int iVal,
                            LPMIXERWNDDATA lpmwd, int iChan)
{
LPMIXERWNDDATA      lpMWD;
int				iYOffset = 0;
int				iScrVisible = 0;
HWND			hWnd;


	// Set the offset based on the control type

	switch(lpczm->iCtrlType)
	{
  case    CTRL_TYPE_OPEN_ZOOM_IAUX:
		iYOffset = JUMP_TO_INPUT_AUX;
		break;
	case		CTRL_TYPE_OPEN_ZOOM_CCOMP:
		iYOffset = JUMP_TO_INPUT_COMP;
		break;
	case		CTRL_TYPE_OPEN_ZOOM_EQ:
		iYOffset = JUMP_TO_INPUT_EQ;
		break;
	case CTRL_TYPE_OPEN_ZOOM_SUB:
		iYOffset = JUMP_TO_INPUT_SUB;
		break;
	case CTRL_TYPE_OPEN_FADER:
		iYOffset = JUMP_TO_INPUT_FADER;
		break;
	case	CTRL_TYPE_OPEN_MATRIX_EQ:
		iYOffset = JUMP_TO_MATRIX_EQ;
		break;
	case	CTRL_TYPE_OPEN_MATRIX_AUX:
		iYOffset = JUMP_TO_MATRIX_AUX;
		break;
	case	CTRL_TYPE_OPEN_MATRIX_SUB:
		iYOffset = JUMP_TO_MATRIX_SUB;
		break;
	default:
		iYOffset = 4000;
		break;
	}


////////////////////////////////////////
// See if we have a Linked Zoom View
// If not then we must create one    

	if(ghwndZoom == NULL)
  {
    lpMWD = MixerWindowDataAlloc(gwActiveMixer,
                                 gpZoneMaps_Zoom,
                                 MAX_CHANNELS, 1);
    if(lpMWD == NULL)
    {
        ErrorBox(ghwndMain, ghInstStrRes, IDS_ERR_ALLOCATE_MEMORY);
        return;
    }
    else
    {
        // Set the Window Start channel
        // and Remap table, and start channel
        // and End Channel
        //-----------------------------------
        CopyMemory(lpMWD->lpwRemapToScr, lpmwd->lpwRemapToScr, MAX_CHANNELS * sizeof(WORD));
        CopyMemory(lpMWD->lpwRemapToPhis, lpmwd->lpwRemapToPhis, MAX_CHANNELS * sizeof(WORD));

        lpMWD->iYOffset = iYOffset;
				
				iScrVisible	= lpMWD->iEndScrChan - lpMWD->iStartScrChan;
				lpMWD->iEndScrChan = lpmwd->iCurChan + iScrVisible;

				if(lpMWD->iEndScrChan >= lpmwd->lZMCount)
					lpMWD->iEndScrChan = lpMWD->lZMCount - 1;

				lpMWD->iStartScrChan = lpMWD->iEndScrChan - iScrVisible;
        lpMWD->iCurChan = lpmwd->iCurChan;
				
        wsprintf(lpMWD->szTitle, "Zoom View");	// was Zoom View (Link)

				// If we don't have a link yet, try to find a Zoom View that
				// is already open.

				hWnd=FindWindowEx(ghwndMDIClient,NULL,gszZoomViewClass,"Zoom View");

				if(hWnd)
				{
					ghwndZoom = hWnd;		// We found a Zoom View, make it our link to the Full View


				}
				else	// No Zoom View windows exist, so create one.
				{
					ghwndZoom = CreateZoomViewWindow(ghwndMDIClient,"Zoom View", lpMWD, 1);// was Zoom View (Link)

				}
#ifdef NOTUSED
					InvalidateRect(lpMWD->hwndLblGroup, NULL, FALSE);
					UpdateWindow(lpMWD->hwndLblGroup);
        
					lpMWD->pntMouseCur.y = iYOffset - lpMWD->iYOffset;
					lpMWD->pntMouseLast.y = 0;

					ScrollImgWindow(ghwndZoom, lpMWD);		// Position the Zoom view as requested.

					InvalidateRect(lpMWD->hwndImg, NULL, FALSE);
					UpdateWindow(lpMWD->hwndImg);

					RequestVisibleVU(lpMWD, -1, -1);
#endif

    }
  }
	else	// We already have a linked Zoom View
  {
    lpMWD = (LPMIXERWNDDATA)GetWindowLong(ghwndZoom, 0);
    if(lpMWD)
    {
				TurnOffAllVUforMixerWindow(lpMWD);
				// Set the Window Start channel
        // and Remap table, and start channel
        // and End Channel
        //-----------------------------------
        CopyMemory(lpMWD->lpwRemapToScr, lpmwd->lpwRemapToScr, MAX_CHANNELS * sizeof(WORD));
        CopyMemory(lpMWD->lpwRemapToPhis, lpmwd->lpwRemapToPhis, MAX_CHANNELS * sizeof(WORD));
        
				iScrVisible	= lpMWD->iEndScrChan - lpMWD->iStartScrChan;
				lpMWD->iEndScrChan = lpmwd->iCurChan + iScrVisible;

				if(lpMWD->iEndScrChan >= lpmwd->lZMCount)
					lpMWD->iEndScrChan = lpMWD->lZMCount - 1;

				lpMWD->iStartScrChan = lpMWD->iEndScrChan - iScrVisible;
        lpMWD->iCurChan = lpmwd->iCurChan;

        InvalidateRect(lpMWD->hwndLblGroup, NULL, FALSE);
        UpdateWindow(lpMWD->hwndLblGroup);
        
				lpMWD->pntMouseCur.y = iYOffset - lpMWD->iYOffset;
				lpMWD->pntMouseLast.y = 0;

				ScrollImgWindow(ghwndZoom, lpMWD);

        InvalidateRect(lpMWD->hwndImg, NULL, FALSE);
        UpdateWindow(lpMWD->hwndImg);

			  RequestVisibleVU(lpMWD, -1, -1);

    }
  }

return;
}

#endif
