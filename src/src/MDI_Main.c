//=================================================
// Copyright 1998, CorTek Software, Inc.
//=================================================

//=================================================
// The MDI Client
//
//=================================================

#include "SAMM.h"
#include "SAMMEXT.h"
#include "MACRO.h"

extern int g_iMasterModuleIdx;

//=================================
// FUNCTION: CreateMainWindow
// creates the main window
//=================================
int       CreateMDIClient(HWND hwnd)
{
CLIENTCREATESTRUCT  ccs = {0};
RECT                rcClient;

GetClientRect(hwnd, &rcClient);

// Find window menu where children will be listed
ccs.hWindowMenu  = GetSubMenu(ghMainMenu, WINDOWMENU);
ccs.idFirstChild = IDM_WINDOWCHILD;

// Create the MDI client filling the client area
ghwndMDIClient = CreateWindow("MDICLIENT",
                             NULL,
                             WS_CHILD | WS_CLIPCHILDREN ,
                             0, 0, 0, 0,
                             hwnd,
                             NULL,
                             ghInstMain,
                             (LPVOID)&ccs);

if(ghwndMDIClient == NULL)
    return IDS_ERR_CREATE_WINDOW;

// Sub class the default windows procedure
// so we can hadle the WM_PAINT and WM_ERASEBKGND
// messasges.
//-----------------------------------------------
gorigMDIProc = (WNDPROC)SetWindowLong(ghwndMDIClient, GWL_WNDPROC, (LONG)MDIClientProc);
ShowWindow(ghwndMDIClient, SW_SHOW);
return 0;
};

//===================================================
//FUNCTION: ShutDownMDIClient
//
//===================================================
void      ShutDownMDIClient(void)
{

//Delete the bitmap
//-----------------
if(ghbmpClientBk)
    DeleteObject(ghbmpClientBk);
return;
}

//====================================================
//FUNCTION: SizeClientWindow
//
//params:   HWND        hwnd; // the parent window
//          UINT        uMessage;
//          WPARAM      wparam;
//          LPARAM      lparam;
//
// Positions the Client Window within the Main Window
//====================================================
void          SizeClientWindow(HWND hwnd, UINT uMessage,
                                     WPARAM wparam, LPARAM lparam)
{
RECT    rcClient;
RECT    rcAdj = {0};

if (wparam != SIZE_MINIMIZED)
  {
  GetClientRect(hwnd, &rcClient);
  
  // Calculate all the adjustments
  //------------------------------
  if(ghwndStatus)
    {
    GetWindowRect(ghwndStatus, &rcAdj);
    CONVERT_RECT_TO_WH(rcAdj);
    // Adjust the Status Window Position and size
    //-------------------------------------------
    SendMessage(ghwndStatus, WM_SIZE, SIZE_RESTORED, 0);

		// Apply the changes to the rcClient
		//----------------------------------
		rcClient.bottom -= rcAdj.bottom - 1;        
    }

	if(ghwndTBPlay)
		{
//		SendMessage(ghwndTBPlay, TB_AUTOSIZE, 0, 0);
		SendMessage(ghwndTBPlay, WM_SIZE, SIZE_RESTORED, MAKELPARAM(100,26));
    GetWindowRect(ghwndTBPlay, &rcAdj);
    CONVERT_RECT_TO_WH(rcAdj);
		// Apply the changes to the rcClient
		//----------------------------------
		rcClient.top += rcAdj.bottom;
		rcClient.bottom -= rcAdj.bottom - 1;        
		}

  // Compensate for the Master View
  //
  if(ghwndMaster)
  {
    //rcClient.left += (MASTER_MODULE_WIDTH + MAX_BORDER_WIDTH);
    rcClient.right -= (MASTER_MODULE_WIDTH + MAX_BORDER_WIDTH);
  }

  // Move and Size the MDI Client Window
  //------------------------------------
  MoveWindow(ghwndMDIClient,
             rcClient.left,
             rcClient.top,
             rcClient.right,
             rcClient.bottom,
             TRUE);

  }

return;
}

//===========================================
//FUNCTION: ClientWindowPaint
//
//Purpose: To repaint the Client Window area
//===========================================
void      ClientWindowPaint(void)
{
HDC                 hdc, hdcMem;
RECT                rectUpdate;
HBITMAP             hBitOld, hBitScreen;
int                 iHoriz, iVert, iWidth, iHeight;
WINDOWPLACEMENT     wpl;

if(ghwndMDIClient == NULL)
    return;

GetWindowPlacement(ghwndMain, &wpl);
if(wpl.showCmd == SW_SHOWMINIMIZED)
    return;

if(GetUpdateRect(ghwndMDIClient, &rectUpdate, FALSE) == FALSE)
    return;

ValidateRect(ghwndMDIClient, NULL);
ValidateRect(ghwndMain, NULL);

hdc = GetDC(ghwndMDIClient); // get the main DC
hBitScreen = CreateCompatibleBitmap(hdc, rectUpdate.right, rectUpdate.bottom);
hdcMem = CreateCompatibleDC(hdc);// Create a memory DC from where we can pull the original Bitmap
hBitOld = SelectObject(hdcMem, ghbmpClientBk);

// Select the Bitmap for the Screen display
SelectObject(hdc, hBitScreen);

iWidth  = gbmpClientInfoBk.bmWidth;
iHeight = gbmpClientInfoBk.bmHeight;
if((iWidth == 0) || (iHeight == 0))
    goto CLEANUP_TIME;

// Start BitBlT the hbmpClientBk to the Screen
//--------------------------------------------
for(iVert = 0; iVert < rectUpdate.bottom; iVert += iHeight)
    for(iHoriz = 0; iHoriz < rectUpdate.right; iHoriz += iWidth)
        {
        BitBlt(hdc, iHoriz, iVert, iWidth, iHeight,
               hdcMem, 0, 0,
               SRCCOPY);
        }

CLEANUP_TIME:
// Clean up time
SelectObject(hdcMem, hBitOld);
DeleteDC(hdcMem);
ReleaseDC(ghwndMDIClient, hdc);
DeleteObject(hBitScreen);
return;
}

//====================================================
// MDI Client Window Procedure For Processing Messages
//====================================================
LRESULT CALLBACK   MDIClientProc(HWND hWnd, UINT wMessage,
                               WPARAM wParam, LPARAM  lParam)
{

switch (wMessage)
 {
  case WM_MOUSEMOVE:
    UpdateTrackingWindow(NULL);
    break;
  case WM_PAINT:
    ClientWindowPaint();
    return 0;
    break;
  case WM_ERASEBKGND:
    ClientWindowPaint();
    return 0;
    break;

  default:
       return gorigMDIProc(hWnd, wMessage, wParam, lParam);
 }
return gorigMDIProc(hWnd, wMessage, wParam, lParam);
}

////////////////////////////////////////////////////
//
//
LPMIXERWNDDATA		g_validMixerWindowData = NULL;
LPMIXERWNDDATA GetValidMixerWindowData(void)
{
//  LPMIXERWNDDATA      lpmwd = NULL;
//  HWND hwndT, hwndTLast = NULL;


	if (g_validMixerWindowData == NULL)
		g_validMixerWindowData = MixerWindowDataAlloc(gwActiveMixer,
																 gpZoneMaps_Zoom,
																 MAX_CHANNELS, 1);


	return g_validMixerWindowData;
/*
  // Update the Master Window if it is open
  //
  if(ghwndMaster)
  {
    lpmwd = (LPMIXERWNDDATA)GetWindowLong(ghwndMaster, 0);
    if(lpmwd != NULL && lpmwd->hwndImg != NULL)
      return lpmwd;
  }

  // now update all MDI windows
  //
  hwndT = GetTopWindow(ghwndMDIClient);
  while(hwndT && hwndT != hwndTLast)
  {
    lpmwd = (LPMIXERWNDDATA)GetWindowLong(hwndT, 0);
    hwndTLast = hwndT;
    
    if(lpmwd != NULL && lpmwd->iWndID == MIXER_WINDOW_FILE_ID && lpmwd->hwndImg != NULL)
    {
      return lpmwd;
    }
    hwndTLast = hwndT;
    hwndT = GetNextWindow(hwndT, GW_HWNDNEXT);
  }
  
  return NULL;
	*/
}

//========================================
// Close all Midi Child Windows
//
//========================================
void      CloseAllMDI(void)
{

HWND hwndT;

// As long as the MDI client has a child, destroy it
//--------------------------------------------------
while ((hwndT = GetWindow(ghwndMDIClient, GW_CHILD))!=NULL)
    {
    // Skip the icon and title windows
    //--------------------------------
    while (hwndT && GetWindow(hwndT, GW_OWNER))
        hwndT = GetWindow(hwndT, GW_HWNDNEXT);

    if (hwndT)
        SendMessage(ghwndMDIClient, WM_MDIDESTROY, (WPARAM)hwndT, 0L);
    else
        break;
    }
return;
}

/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: RefreshAllMDIWindows
//
//
//
void      RefreshAllLblWindows(void)
{
  HWND hwndT, hwndTLast = NULL;
  LPMIXERWNDDATA      lpmwd;


  // now update all MDI windows
  //
  hwndT = GetTopWindow(ghwndMDIClient);
  while(hwndT && hwndT != hwndTLast)
  {
    lpmwd = (LPMIXERWNDDATA)GetWindowLong(hwndT, 0);
    hwndTLast = hwndT;
    
    if(lpmwd != NULL && lpmwd->iWndID == MIXER_WINDOW_FILE_ID)
    {
      if(lpmwd->hwndImg != NULL)
      {
        InvalidateRect(lpmwd->hwndLblGroup, NULL, FALSE);
        UpdateWindow(lpmwd->hwndLblGroup);

        InvalidateRect(lpmwd->hwndImg, NULL, FALSE);
        UpdateWindow(lpmwd->hwndImg);
      }
    }


    hwndTLast = hwndT;
    hwndT = GetNextWindow(hwndT, GW_HWNDNEXT);
  }
  return;
}

/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: RefreshAllMDIWindows
//
//
//
void      RefreshAllMDIWindows(void)
{
  HWND hwndT, hwndTLast = NULL;
  LPMIXERWNDDATA      lpmwd;



  // Update the Master Window if it is open
  //
  if(ghwndMaster)
  {
    lpmwd = (LPMIXERWNDDATA)GetWindowLong(ghwndMaster, 0);
    if(lpmwd != NULL && lpmwd->hwndImg != NULL)
    {
      InvalidateRect(lpmwd->hwndImg , NULL, FALSE);
      UpdateWindow(lpmwd->hwndImg );
    }
  }

  // now update all MDI windows
  //
  hwndT = GetTopWindow(ghwndMDIClient);
  while(hwndT && hwndT != hwndTLast)
  {
    lpmwd = (LPMIXERWNDDATA)GetWindowLong(hwndT, 0);
    hwndTLast = hwndT;
    
    if(lpmwd != NULL && lpmwd->iWndID == MIXER_WINDOW_FILE_ID && lpmwd->hwndImg != NULL)
    {
      InvalidateRect(lpmwd->hwndImg , NULL, FALSE);
      UpdateWindow(lpmwd->hwndImg );
    }


    hwndTLast = hwndT;
    hwndT = GetNextWindow(hwndT, GW_HWNDNEXT);
  }
  return;
}


/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: UpdateSameMixWndByCtrlNum
//
//
//
void      UpdateSameMixWndByCtrlNum(HWND  hwnd,
                                    int   iMixer,
                                    int   iPhisChannel,
                                    LPCTRLZONEMAP   lpctrlZM,
                                    int   iVal,
                                    HDC   hdcBuffer)
{
  HWND                hwndT, hwndTLast = NULL;
  LPMIXERWNDDATA      lpmwd;
  int                 iScrChannel;
  LPCTRLZONEMAP       lpctrlZMSource;
  HDC                 hdcScr;
  RECT                rScrPos;
  HDC                 hdcSrcBuffer;
  BOOL                bForceToMaster = FALSE;
	int									iBMPIndexLast = -1;
	//HBITMAP							hbmp;

  if(hdcBuffer != NULL)
    hdcSrcBuffer = hdcBuffer;
  else
    // use the Generic memory buffer
    hdcSrcBuffer = g_hdcMemory;

  /* // This thing is not correct at all
  if((gDeviceSetup.iaChannelTypes[lpctrlZM->iModuleNumber] == 2) && 
     ghwndMaster )
  {
    lpmwd = (LPMIXERWNDDATA)GetWindowLong(ghwndMaster, 0);

    if(ScanCtrlZonesAbs(lpmwd->lpZoneMap[g_iMasterModuleIdx].lpZoneMap, 
                        lpctrlZM->iCtrlNumAbs))
      bForceToMaster = TRUE;
    else
      bForceToMaster = FALSE;
  }
  */
  if(iPhisChannel == g_iMasterModuleIdx)
    bForceToMaster = TRUE;

  if((gDeviceSetup.iaChannelTypes[lpctrlZM->iModuleNumber] == 5) ||
     (gDeviceSetup.iaChannelTypes[lpctrlZM->iModuleNumber] == 4) ||
      bForceToMaster )
  {
    //
    //
    if(ghwndMaster)
    {
      lpmwd = (LPMIXERWNDDATA)GetWindowLong(ghwndMaster, 0);
      //iScrChannel = lpmwd->lpwRemapToPhis[lpctrlZM->iModuleNumber] - 
      //              lpmwd->iStartScrChan;
      iScrChannel = lpmwd->lpwRemapToPhis[iPhisChannel];

      lpctrlZMSource = lpmwd->lpZoneMap[iPhisChannel].lpZoneMap;
      lpctrlZMSource = ScanCtrlZonesNum(lpctrlZMSource, lpctrlZM->iCtrlChanPos);

      if(GetMWScrChanRect(lpmwd, iScrChannel, &rScrPos) == 0)
      {
        hdcScr = GetDC(lpmwd->hwndImg);
        UpdateControlsByCtrlNum(hdcScr, hdcSrcBuffer, lpmwd, rScrPos.left,
                                g_iMasterModuleIdx/*lpctrlZM->iModuleNumber*/, lpctrlZMSource, 
                                iVal, DIRECTIONS_ALL, FALSE);
        ReleaseDC(lpmwd->hwndImg , hdcScr);
      }
    }
  }
  else
  {
		//lpmwd = (LPMIXERWNDDATA)GetWindowLong(hwnd, 0);
		//iBMPIndexLast = lpmwd->lpZoneMap[iPhisChannel].iBmpIndx;

    // now update all MDI windows
    //
    hwndT = GetTopWindow(ghwndMDIClient);
    while(hwndT && hwndT != hwndTLast)
    {
      lpmwd = (LPMIXERWNDDATA)GetWindowLong(hwndT, 0);
      hwndTLast = hwndT;
    
      if( (lpmwd != NULL) && (lpmwd->iWndID == MIXER_WINDOW_FILE_ID) &&
				  (lpmwd->hwndImg != NULL) && 
          (lpmwd->hwndImg != hwnd) && (lpmwd->hwndImg != ghwndMaster))
      {
        // make sure this Phisical channel is visible ...
        // and try to update it
        //
        iScrChannel = lpmwd->lpwRemapToPhis[iPhisChannel] - lpmwd->iStartScrChan;
        if(iScrChannel >= 0 && iScrChannel <= lpmwd->iEndScrChan)
        {
          // Scan for Controls
          //------------------
          lpctrlZMSource = lpmwd->lpZoneMap[iPhisChannel].lpZoneMap;
          lpctrlZMSource = ScanCtrlZonesNum(lpctrlZMSource, lpctrlZM->iCtrlChanPos);
          if(lpctrlZMSource)
          {
            if(GetMWScrChanRect(lpmwd, iScrChannel, &rScrPos) == 0)
            {
							// well the window we are about to update might be different than the last window
							// so make sure we select the correct bitmap for it .... ??
							/*
							if(iBMPIndexLast != lpmwd->lpZoneMap[iPhisChannel].iBmpIndx && iBMPIndexLast >= 0)
							{
								iBMPIndexLast = lpmwd->lpZoneMap[iPhisChannel].iBmpIndx;
								hbmp = SelectObject(g_hdcBuffer, gpBMPTable[iBMPIndexLast].hbmp);
								BitBlt(hdcSrcBuffer, 0, 0, gpBMPTable[iBMPIndexLast].iWidth, gpBMPTable[iBMPIndexLast].iHeight,
											 g_hdcBuffer, 0, 0, SRCCOPY);
								SelectObject(g_hdcBuffer, hbmp);
							};
							*/
              hdcScr = GetDC(lpmwd->hwndImg);

              UpdateControlsByCtrlNum(hdcScr, hdcSrcBuffer, lpmwd, rScrPos.left,
                                      iPhisChannel, lpctrlZMSource, iVal, DIRECTIONS_ALL, FALSE);
              ReleaseDC(lpmwd->hwndImg , hdcScr);
            }
          }
        }
        //if()
        //lpmwd->hwndImg != hwnd;
      }


      hwndTLast = hwndT;
      hwndT = GetNextWindow(hwndT, GW_HWNDNEXT);
    }
  }
  return;
}
