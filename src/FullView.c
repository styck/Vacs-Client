//=================================================
// CopyRight 1998, CorTek Softawre, Inc.
//=================================================

//=================================================
// The Full View Window
//
//=================================================

//#include <windows.h>

#include "SAMM.h"
#include "SAMMEXT.h"
#include "MACRO.h"

#include <zmouse.h>

//
extern int                 g_aiAux[MAX_MATRIX_COUNT];
extern int                 g_aiMatrix[MAX_MATRIX_COUNT];
extern int                 g_iCueModuleIdx;
extern int                 g_iAuxIdx;
extern int                 g_iMasterModuleIdx;


// static variables
int	g_CueMasterSystem=0;


HRGN      g_hrgn;
HBITMAP   g_hbmpUDScroller;

void TurnOffAllVUforMixerWindow(LPMIXERWNDDATA lpmwd);
void HandleCueMasterMuteFilterEx(int iPhisChann, LPMIXERWNDDATA lpmwd, 
                                  LPCTRLZONEMAP lpctrlZM, BOOL bIsOn);

void	DisplayStereoCueVUData(VU_READ *pVuDataBuffer);
void	HandleRemoteSequenceControl(WORD wControl);

//====================================
// FUNCTION: RegisterFullViewClass
//
//
// Register the Full View Window Class
//====================================
int       RegisterFullViewClass(void)
{
	int         iReturn;
	WNDCLASS    wc;


	// Register Full View Class
	//--------------------------
	ZeroMemory(&wc, sizeof(WNDCLASS));      // Clear wndclass structure

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)FullViewProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(LPSTR);// all of the data for this window will be stored here
	wc.hInstance = ghInstMain;
	wc.hIcon = LoadIcon(ghInstConsoleDef, MAKEINTRESOURCE(FullViewIcon));// this might leak memory
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = gszFullViewClass;

	iReturn = RegisterClass(&wc);

	if(iReturn == 0)
		 return(IDS_ERR_REGISTER_CLASS);     // Error... Exit


	// Register Full View Image Class
	//-------------------------------
	ZeroMemory(&wc, sizeof(WNDCLASS));      // Clear wndclass structure

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;
	wc.lpfnWndProc = (WNDPROC)FullViewImgProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(LPSTR);// all of the data for this window will be stored here
	wc.hInstance = ghInstMain;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetStockObject(GRAY_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = gszFullViewImageClass;

	iReturn = RegisterClass(&wc);

	if(iReturn == 0)
		 return(IDS_ERR_REGISTER_CLASS);     // Error... Exit

	return 0;
}

//===============================
// FUNCTION: CreateFullViewWindow
//
//purpouse:
//      Create the window and
// load all of the bitmaps and
// zone tables ...
//
//===============================
HWND       CreateFullViewWindow(LPSTR szTitle, LPMIXERWNDDATA  pMWD)
{
	HWND                hWnd;
	RECT                rect;
	LPMIXERWNDDATA      lpmwd;
	DWORD								style;

	if (gInitialized == FALSE)
		return NULL;

	///////////////////////////////////
	// Limit us to only ONE FULL VIEW
	// If window already exists then 
	// bring it to the front

	hWnd=FindWindowEx(ghwndMDIClient,NULL,gszFullViewClass,szTitle);
	if(hWnd)
	{
		BringWindowToTop(hWnd);
		return NULL;
	}


	if(pMWD == NULL)
		{
		// ???????????????????????
		// ???????????????????????
		//
		lpmwd = MixerWindowDataAlloc(gwActiveMixer,
																 gpZoneMaps_Full,
																 MAX_CHANNELS, 1);

		if(lpmwd == NULL)
				{
				ErrorBox(ghwndMain, ghInstStrRes, IDS_ERR_ALLOCATE_MEMORY);
				return NULL;
				}

		// store the Window title
		//-----------------------
		lstrcpy(lpmwd->szTitle, szTitle);

		// make some room for the label Window
		// after all we want this window to appear
		// in it's full size
		//----------------------------------------
		lpmwd->rWndPos.bottom += HEIGHT_FULL_LABEL_WND;
		}
	else
			lpmwd = pMWD;

	lpmwd->lpZoneMapZoom = gpZoneMaps_Zoom;

	// this makes it possible if the window
	// is streched all the way to fit the entire image
	//------------------------------------------------
	lpmwd->rMaxWndPos.bottom += HEIGHT_FULL_LABEL_WND;
/*
	style	=  MDIS_ALLCHILDSTYLES;
	style |= (WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | WS_CHILD);
	style &= ~WS_MAXIMIZEBOX;
*/

	style = MDIS_ALLCHILDSTYLES | WS_CHILD | WS_SYSMENU | WS_CAPTION | WS_VISIBLE
															| WS_THICKFRAME | WS_MINIMIZEBOX// | WS_MAXIMIZEBOX
															| WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	style &= ~WS_MAXIMIZEBOX;

	if(pMWD == NULL)
			hWnd = CreateMDIWindow (
															gszFullViewClass,
															szTitle,
															style,
															0,
															0,//CW_USEDEFAULT,
															lpmwd->rWndPos.right,
															lpmwd->rWndPos.bottom,
															ghwndMDIClient,
															ghInstMain,
															(LPARAM)lpmwd
															);
	else
			hWnd = CreateMDIWindow (
															gszFullViewClass,
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
    
	if(hWnd == NULL)
			{
			MixerWindowDataFree(lpmwd);
			ErrorBox(ghwndMain, ghInstStrRes,IDS_ERR_CREATE_WINDOW);
			return NULL;//IDS_ERR_CREATE_WINDOW;
			}


	if(pMWD == NULL)
			{
			// adjust the Image Window Size to fit
			// perfectly inside of the Client Window
			//--------------------------------------
			lpmwd->rVisible.left = 0;
			lpmwd->rVisible.top = HEIGHT_FULL_LABEL_WND;

			ImageWindowSize(hWnd, &lpmwd->rVisible, lpmwd);
			}
	// Ok we have a window opened
	//---------------------------
	SetWindowLong(hWnd, 0, (LPARAM)lpmwd);


	// Create the Label-Group display Window
	//--------------------------------------
	rect.left = rect.top = 0;
	rect.right = lpmwd->rWndPos.right;
	rect.bottom = HEIGHT_FULL_LABEL_WND;
	lpmwd->wWndType = WND_GROUPLBL_FULL;

   
	if(CreateLblGroupWnd(&rect, hWnd, lpmwd) == NULL)
			{
			ErrorBox(ghwndMain, ghInstStrRes,IDS_ERR_CREATE_WINDOW);
			PostMessage(ghwndMDIClient, WM_MDIDESTROY, (WPARAM)hWnd, 0L);
			return NULL;//IDS_ERR_CREATE_WINDOW;
			}

	// now create the Image window
	// and continue
	//----------------------------
	if(CreateFullViewImageWindow(hWnd, (LPARAM)lpmwd))
			PostMessage(ghwndMDIClient, WM_MDIDESTROY, (WPARAM)hWnd, 0L);

	SetFocus(hWnd);
	return hWnd;
};


//===============================
//FUNCTION:FullViewProc
//
//===============================
LRESULT CALLBACK  FullViewProc(HWND hWnd, UINT wMessage, 
                               WPARAM wParam, LPARAM lParam)
{
	MINMAXINFO FAR      *lpMMI;
	LPMIXERWNDDATA      lpmwd;
	RECT                rect;

	lpmwd = (LPMIXERWNDDATA)GetWindowLong(hWnd,0);

	switch (wMessage)
	{
		//////////////////////////////////////////////////////////////
		case WM_ERASEBKGND: // to reduce flashing on the screen
			break;
		//////////////
		case WM_KEYUP:
      switch(wParam)
      {
      case VK_LEFT:
        ScrollSideWays(NULL, lpmwd, LEFT);  // Scrolling left
        break;
      case VK_RIGHT:
        ScrollSideWays(NULL, lpmwd, RIGHT);   // Scrollnig right
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
    case WM_COMMAND:
	    switch (LOWORD(wParam))
		    {
            default:
                return DefMDIChildProc(hWnd, wMessage, wParam, lParam);
			}
        break;
    //////////////////////////////////////////////////////////////
    case WM_SIZE:
        HandleWndSize(hWnd, lpmwd, LOWORD(lParam), HIWORD(lParam), wParam);
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
    //////////////////////////////////////////////////////////////
//    case WM_MDIACTIVATE:
//        if((HWND)lParam == hWnd)    
//            {
            //SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSIZE);
//            }
//        break;    
    //////////////////////////////////////////////////////////////
    case WM_GETMINMAXINFO:
            lpMMI = (MINMAXINFO *)lParam;
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
    case WM_DESTROY:

//        break;
    default:
        return DefMDIChildProc(hWnd, wMessage, wParam, lParam);

	}

return 0;
}


//==================================================
// FUNCTION:        CreateFullViewImageWindow
//
// Creates the Wnidow that will draw the
// bitmap image and take care of all mixer
// controls .. bla - bla - bla
//
//==================================================
int       CreateFullViewImageWindow(HWND hwndParent,LPARAM lParam)
{
HWND                hwnd;
LPMIXERWNDDATA      lpmwd;
HDC                 hdc;

lpmwd = (LPMIXERWNDDATA)lParam;

hwnd = CreateWindow(
					 gszFullViewImageClass,   // Window class name
					 NULL,                   // Window's title
					 WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
					 lpmwd->rVisible.left,
					 lpmwd->rVisible.top,
					 lpmwd->rVisible.right,   // Set it to the max Width
					 lpmwd->rVisible.bottom,  // Set it to the max Height
					 hwndParent,             // Parent window's handle
					 NULL,                   // Default to Class Menu
					 ghInstMain,              // Instance of window
					 (void *)lParam                  // Ptr To Data Structure For WM_CREATE
                   );

if(hwnd == NULL)
    {
    ErrorBox(ghwndMain, ghInstStrRes,IDS_ERR_MAX_WINDOW);
    return IDS_ERR_CREATE_WINDOW;
    }


// Create the Global DCs for faster update
//----------------------------------------
if(g_hdcMemory == NULL)
  {
  hdc = GetDC(hwnd);
  g_hbmpBuffer = CreateCompatibleBitmap(hdc, 120, 4000);
  g_hdcMemory = CreateCompatibleDC(hdc);
  g_hdcBuffer = CreateCompatibleDC(hdc);
	g_hdcTempBuffer = CreateCompatibleDC(hdc);
  SelectObject(g_hdcMemory, g_hbmpBuffer);
  ReleaseDC(hwnd, hdc);
  }

// Store this Window Information in the
// Double linked List
//-------------------------------------
//if(AddMWEntryToDLList(hwnd, lpmwd))
//    {
//    ErrorBox(hwndMain, hInstMainRes,IDS_ERR_MAX_WINDOW);
//    return IDS_ERR_MAX_WINDOW;
//    };


UpdateWindow(hwnd);
return 0;
};

//====================================================
//FUNCTION: FullViewImgProc
//
// this is the image window procedure. Where
// the image is displayed, updated and used
// as a control
//
//====================================================
LRESULT CALLBACK  FullViewImgProc(HWND hWnd, UINT wMessage, 
                                  WPARAM wParam, LPARAM lParam)
{
LPMIXERWNDDATA      lpmwd;
PAINTSTRUCT         ps;

lpmwd = (LPMIXERWNDDATA)GetWindowLong(hWnd,0);

switch (wMessage)
	 {
    //////////////////////////////////////////////////////////////
    case WM_PAINT:
			WaitForSingleObject(gDisplayEvent, 70);

      BeginPaint(hWnd, &ps);
      DrawImgWindow(&ps, lpmwd);
      EndPaint(hWnd, &ps);
      ShowMousePos(hWnd, lpmwd);

			SetEvent(gDisplayEvent);
      break;

    //////////////////////////////////////////////////////////////
    case WM_ERASEBKGND: // to reduce flashing on the screen

      break;

    //////////////////////////////////////////////////////////////
    case WM_MOUSEMOVE:
			//WaitForSingleObject(gDisplayEvent, 90);
      HandleMouseMove(hWnd, MAKEPOINTS(lParam), wParam, lpmwd);
			//SetEvent(gDisplayEvent);
      break;
    //////////////////////////////////////////////////////////////
    case WM_LBUTTONDOWN:
      HandleLBDown(hWnd, MAKEPOINTS(lParam), wParam, lpmwd);
      break;

    //////////////////////////////////////////////////////////////
    case WM_RBUTTONDOWN:
      HandleRBDown(hWnd, MAKEPOINTS(lParam), wParam, lpmwd);
      break;

    //////////////////////////////////////////////////////////////
    case WM_LBUTTONUP:
      HandleLBUp(hWnd, MAKEPOINTS(lParam), wParam, lpmwd);
      break;

    //////////////////////////////////////////////////////////////
    case WM_RBUTTONUP:
      HandleRBUp(hWnd, MAKEPOINTS(lParam), wParam, lpmwd);
      break;

    //////////////////////////////////////////////////////////////
    case WM_MBUTTONDOWN:
      HandleMBDown(hWnd, MAKEPOINTS(lParam), wParam, lpmwd);
      break;

    //////////////////////////////////////////////////////////////
    case WM_MBUTTONUP:
      HandleMBUp(hWnd, MAKEPOINTS(lParam), wParam, lpmwd);
      break;

    case WM_TIMER:
      HandleCtrlTimer(hWnd, lpmwd);
      break;
    //////////////////////////////////////////////////////////////
//    case WM_SIZE:
//        MessageBox(ghwndMain, "WM_SIZE in the ImgWindow", "", MB_OK);
//        break;
    //////////////////////////////////////////////////////////////
    case WM_COMMAND:
	    switch (LOWORD(wParam))
		    {
        default:
            return DefWindowProc(hWnd, wMessage, wParam, lParam);
			  }
        break;
    //////////////////////////////////////////////////////////////
    case WM_CREATE:
        // Set the pointer to the
        // Mixer window data
        //---------------------------
        SetWindowLong(hWnd, 0,(LPARAM)((LPCREATESTRUCT)lParam)->lpCreateParams);
        if(lpmwd == NULL)
            lpmwd = (LPMIXERWNDDATA)((LPCREATESTRUCT)lParam)->lpCreateParams;

        lpmwd->hwndImg = hWnd; // Store this Window handle
				RequestVisibleVU(lpmwd, -1, 0);

				// Start the Cue-system
				/*
				if(g_CueMasterSystem == 0){
					HandleCueMasterMuteFilterEx(g_iMasterModuleIdx, lpmwd, NULL, FALSE);
					g_CueMasterSystem  = 1;
				}
				*/
        break;

    //////////////////////////////////////////////////////////////
    case WM_DESTROY:
        // Delete the Window Entry
        // from the Linked List
        //------------------------
//        DeleteMWEntryFromDLList(lpmwd);

        // Free all the resources
        // the memory and other stuff
        //---------------------------
				TurnOffAllVUforMixerWindow(lpmwd);
        MixerWindowDataFree(lpmwd);

        break;

    //////////////////////////////////////////////////////////////
    default:
        return DefWindowProc(hWnd, wMessage, wParam, lParam);
	}
return 0;
}

//======================================================
//FUNCTION: DrawFullImgWindow
//
//params:
//      LPPAINTSTRUCT       lpPS; the paint structure
//      LPMIXERWNDDATA      lpMWD; Data needed to redraw
//======================================================
void      DrawImgWindow(LPPAINTSTRUCT lpPS,
                              LPMIXERWNDDATA lpMWD)
{
HDC         hdcMem;
HDC         hdc;
HBITMAP     hbmpMemOld = NULL;
HBITMAP     hbmpOld = NULL;
HBITMAP     hbmpBuff = NULL;
long        lCount;//, lZMCount;
int         iX, iY, iCX, iCY;
int         iBMPIndex;
int         iBMPIndexLast = -1;
int         iPhisChannel;
int         iMixer;
  

	hbmpOld = NULL;
	hbmpBuff = NULL;

	if(lpMWD == NULL)
			return;
	hdc = lpPS->hdc; // assign to local ... for speed only
	hdcMem = CreateCompatibleDC(hdc);
	//hdcMemBuff = CreateCompatibleDC(hdc);

	////////////////////////////
	// apply the vertical offset
	//--------------------------

	iX = iY = iCX = iCY = 0;

//lZMCount = lpMWD->lZMCount;
//for(lCount = 0; lCount < lZMCount; lCount++)

	for(lCount = lpMWD->iStartScrChan; lCount < lpMWD->iEndScrChan + 1; lCount++)
  {
    iMixer = HIBYTE(lpMWD->lpwRemapToScr[lCount]);
    iPhisChannel = LOBYTE(lpMWD->lpwRemapToScr[lCount]); // Get the actual phis channel

    iBMPIndex = lpMWD->lpZoneMap[iPhisChannel].iBmpIndx;
    iCX = gpBMPTable[iBMPIndex].iWidth;
    iCY = gpBMPTable[iBMPIndex].iHeight;

		////////////////////////////////////////////////////////
    // if it is the same index we don't need to reselect it
    // in the HDC
    //-----------------------------------------------------
    if(iBMPIndex != iBMPIndexLast)
    {
        if(hbmpBuff == NULL)
        {
            hbmpBuff = CreateCompatibleBitmap(hdc, iCX, iCY);
            hbmpMemOld = SelectObject(hdcMem, hbmpBuff);
        }
  
        hbmpOld = SelectObject(g_hdcBuffer, gpBMPTable[iBMPIndex].hbmp);
        BitBlt(hdcMem, 0, 0, iCX, iCY,
               g_hdcBuffer, 0, 0, SRCCOPY );
        SelectObject(g_hdcBuffer, hbmpOld)  ;
        iBMPIndexLast = iBMPIndex;
    }

		//////////////////////////
    // Draw all the Controls
    // into the Memory Bitmap
    //-----------------------
    DrawControls(hdcMem, iMixer, iPhisChannel, lpMWD);
    BitBlt(hdc, iX, iY, iCX, iCY,
           hdcMem, 0, lpMWD->iYOffset, SRCCOPY);

		//////////////////////////////////////////////////
    // advance x with the width of the current bitmap
    //-----------------------------------------------
    iX += iCX;
  }

// restore the old bitmap for this DC
//-----------------------------------
SelectObject(hdcMem, hbmpMemOld);
DeleteDC(hdcMem);
DeleteObject(hbmpBuff);

return;
}

//==================================================
//FUNCTION: DrawControls
//
//params:
//      HDC             hdc; -> the DC to draw into
//      int             iChan;-> the Phis channel
//      LPMIXERWNDDATA  lpmwd;->all the other data
//
//purpose:
//      Goes through all of the Controls for this
//  channel and draws their position in the hdc
//
//==================================================
void      DrawControls(HDC hdc, int iMixer, int iChan, 
                             LPMIXERWNDDATA lpmwd)
{
LPZONE_MAP      lpZoneMap;
LPCTRLZONEMAP   lpctrlZM;
int             iCount, iZonesCount, iCtrlNum;
int             iPhisDataValue;
RECT            rZone;
int             iScrTop, iScrBottom;
int             iBMPIndex;
HBITMAP         hbmpOld;
RECT            rAct;

// Get The Zone Map
// and in general load the
// pointers
//------------------------
lpZoneMap = &lpmwd->lpZoneMap[iChan];
lpctrlZM = lpZoneMap->lpZoneMap;

// the Y position of the Image Window
//-----------------------------------
iScrTop = lpmwd->iYOffset;
iScrBottom = lpmwd->rVisible.bottom + lpmwd->iYOffset;

// Assign the Zone count to a local
// for speed
//---------------------------------
iZonesCount = lpZoneMap->iZonesCount;

// Go into a loop and draw each individual control
// that has a valid Control Number not CTRL_NUM_NULL
//--------------------------------------------------
	for(iCount = 0; iCount < iZonesCount; iCount++)
  {
    iCtrlNum = lpctrlZM->iCtrlChanPos;
    if((iCtrlNum != CTRL_NUM_NULL) && (lpctrlZM->iDispType != DISP_TYPE_NULL))
    {
        rZone.right = lpctrlZM->rZone.right - lpctrlZM->rZone.left;
        rZone.bottom = lpctrlZM->rZone.bottom - lpctrlZM->rZone.top;

        rZone.left = lpctrlZM->rZone.left + lpmwd->iXadj;
        rZone.top = lpctrlZM->rZone.top - lpmwd->iYOffset;

        // Update - Draw the Control only if it is visible
        //------------------------------------------------
        if((iScrBottom > rZone.top) && ((lpctrlZM->rZone.bottom - lpmwd->iYOffset) > 0))
        {
            iPhisDataValue = GETPHISDATAVALUE(iMixer, lpctrlZM, iCtrlNum);

            rAct = lpctrlZM->rZone;
            // now Prepare to Call the function that handles this given Control
            //-----------------------------------------------------------------
            iBMPIndex = lpmwd->lpZoneMap[iChan].iBmpIndx;
            hbmpOld = SelectObject(g_hdcBuffer, gpBMPTable[iBMPIndex].hbmp);
            BitBlt(g_hdcMemory, rAct.left, rAct.top, rZone.right, rZone.bottom,
                   g_hdcBuffer, rAct.left, rAct.top, SRCCOPY );

            SelectObject(g_hdcBuffer, hbmpOld);

            // Call the function that handles this given Control
            //--------------------------------------------------
            lpctrlZM->CtrlFunc(g_hdcMemory, lpctrlZM,  iPhisDataValue, lpmwd, iChan);

            BitBlt(hdc, rAct.left, rAct.top, rZone.right, rZone.bottom,
                   g_hdcMemory, rAct.left, rAct.top, SRCCOPY );
        }
    }

    lpctrlZM ++;
  }

return;
}

/////////////////////////////////////////////////////////////////////
//  FUNCTION: TurnOffAllVUforMixerWindow
//
//  Purpose:  Clear our flags that keep track of what VU data to send
//            and then send to GServer to stop VU data
//           
void TurnOffAllVUforMixerWindow(LPMIXERWNDDATA lpmwd)
{
  int   iCount;
  //int   iSubCount;
  char  acVU[MAX_CHANNELS];


	if(lpmwd->wWndType == WND_GROUPLBL_FULL)
		return;

  ZeroMemory(acVU, 80);


  
  for(iCount = 0; iCount <  80; iCount++)
  {
    if(lpmwd->acVisibleVU[iCount] > 0)
      acVU[iCount] = -1;
  }

  /*
  for(iCount = lpmwd->iStartScrChan; iCount < lpmwd->iEndScrChan+1; iCount++)
  {
    acVU[LOBYTE(lpmwd->lpwRemapToScr[iCount])] = -1;
    if(lpmwd->lpwRemapToScr[iCount] == g_iMasterModuleIdx)
      acVU[g_iCueModuleIdx] = -1;

    for(iSubCount = 0; iSubCount < MAX_MATRIX_COUNT; iSubCount++)
    {
      if(g_aiAux[iSubCount] != 0)
        if(g_aiAux[iSubCount] == lpmwd->lpwRemapToScr[iCount])
          acVU[g_aiMatrix[iSubCount]] = -1;
    }
  }
  */
  ZeroMemory(lpmwd->acVisibleVU, 80);
  CDef_ShowVuData(acVU);
}



/////////////////////////////////////////////////////////////////////
//  FUNCTION: RequestVisibleVU
//
//  Purpose:  This routine sends the GServer the list of channels to
//            send VU data for. 
//           
//
void    RequestVisibleVU(LPMIXERWNDDATA lpmwd, int iPrevStart, int iPrevEnd)
{
  //char  chBuffer[512];
	int   iCount, iSubCount;
  char  acVU[MAX_CHANNELS];

	if(lpmwd == NULL)
		return;

	if(lpmwd->lpZoneMap == gpZoneMaps_Full)
		return;
	if(lpmwd->wWndType == WND_GROUPLBL_FULL)
		return;

  ZeroMemory(acVU, 80);	// CLEAR VISIBLE VU ARRAY

  // Make sure all visible channels are set to something ...
	// Loop through all visible channels

  for(iCount = lpmwd->iStartScrChan; iCount <= lpmwd->iEndScrChan; iCount++)
  {
    lpmwd->acVisibleVU[LOBYTE(lpmwd->lpwRemapToScr[iCount])] ++;	// increment its visible count

//////////////////////////////////////////////////////////////  NOW ALWAYS SEND CUE DATA
//    if(lpmwd->lpwRemapToScr[iCount] == g_iMasterModuleIdx)
//      lpmwd->acVisibleVU[g_iCueModuleIdx] ++;
//////////////////////////////////////////////////////////////

    for(iSubCount = 0; iSubCount < MAX_MATRIX_COUNT; iSubCount++)
    {
      if(g_aiAux[iSubCount] != 0)
        if(g_aiAux[iSubCount] == lpmwd->lpwRemapToScr[iCount])
          lpmwd->acVisibleVU[g_aiMatrix[iSubCount]] ++;
    }
  }

	//////////////////////////////////////////////
	// Force Cue data for L CUE R floating window

  lpmwd->acVisibleVU[g_iCueModuleIdx] = 1;	// ALWAYS SEND CUE MODULE DATA


	////////////////////////////////////////////////////
	// Now loop through all the channels and if the
	// visible count = 1 then flag the GServer to 
	// send data for this VU

  for(iCount = 0; iCount <  MAX_CHANNELS; iCount++)
  {
    if(lpmwd->acVisibleVU[iCount] == 1)
			acVU[iCount] = 1;

		if(lpmwd->acVisibleVU[iCount] > 1)
			lpmwd->acVisibleVU[iCount] = 1;
  }



/*
	wsprintf(chBuffer, "%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x",
					 lpmwd->acVisibleVU[0], lpmwd->acVisibleVU[1],
					 lpmwd->acVisibleVU[2], lpmwd->acVisibleVU[3],
					 lpmwd->acVisibleVU[4], lpmwd->acVisibleVU[5],
					 lpmwd->acVisibleVU[6], lpmwd->acVisibleVU[7],
					 lpmwd->acVisibleVU[8], lpmwd->acVisibleVU[9],
					 lpmwd->acVisibleVU[10], lpmwd->acVisibleVU[11],
					 lpmwd->acVisibleVU[12], lpmwd->acVisibleVU[13],
					 lpmwd->acVisibleVU[14], lpmwd->acVisibleVU[15]
					 
					 );
	MessageBox(ghwndMain, chBuffer, "VUs", MB_OK);  
*/

	//////////////////////////////////////
	// Send data to GServer to indicate
	// what VU data to send

  CDef_ShowVuData(acVU);


  return;
}

/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: ScrollSideways
//
//  iDir = -1 means scroll left
//  iDir = 1 means scroll right
//
// Routine uses the iVisibleCount to determine how many modules
// to scroll by
//
void  ScrollSideWays(HWND hwnd, LPMIXERWNDDATA lpmwd, int iDir)
{
  HWND    hwndParent = hwnd; //GetParent(hwnd);
  int     iVisibleCount = (lpmwd->iEndScrChan - lpmwd->iStartScrChan) + 1;
  int     iPrevStart = lpmwd->iStartScrChan;
  int     iPrevEnd   = lpmwd->iEndScrChan;

	if (iDir == FIRST_AUX)
	{
		iDir = 100;
	}
	else
	{
		if(iDir == FIRST_INPUT)
			iDir = - 100;
		else
		{
			if( ! (lpmwd->wKeyFlags & VK_SHIFT) )
				iDir = iDir * iVisibleCount;  // Move by the number of visible modules
		}
	}



  // Make sure we are in the bounds of visible display
  TurnOffAllVUforMixerWindow(lpmwd);

  if( ((lpmwd->iEndScrChan + iDir) < lpmwd->lZMCount ) &&
      ((lpmwd->iStartScrChan + iDir) >= 0) )
  {
	    lpmwd->iStartScrChan += iDir;
    lpmwd->iEndScrChan   += iDir;

    InvalidateRect(lpmwd->hwndLblGroup, NULL, TRUE);
    UpdateWindow(lpmwd->hwndLblGroup);

    // Update the Image Window
    //
    InvalidateRect(lpmwd->hwndImg, NULL, TRUE);
    UpdateWindow(lpmwd->hwndImg);
  }
  else if ((iDir < 0) && (lpmwd->iStartScrChan != 0)) // Moving to the left
  {
    lpmwd->iStartScrChan = 0;                         // We are at the very left
    lpmwd->iEndScrChan = iVisibleCount - 1;           // Set end channel

    InvalidateRect(lpmwd->hwndLblGroup, NULL, TRUE);
    UpdateWindow(lpmwd->hwndLblGroup);

    // Update the Image Window
    //
    InvalidateRect(lpmwd->hwndImg, NULL, TRUE);
    UpdateWindow(lpmwd->hwndImg);

  }
  else if ( (iDir > 0 ) && (lpmwd->iEndScrChan != lpmwd->lZMCount-1 ))  // Moving to the right
  {
    lpmwd->iStartScrChan = lpmwd->lZMCount - iVisibleCount;   // Set start channel
    lpmwd->iEndScrChan = lpmwd->lZMCount - 1;         // We are at the very right
                                     
    InvalidateRect(lpmwd->hwndLblGroup, NULL, TRUE);
    UpdateWindow(lpmwd->hwndLblGroup);

    // Update the Image Window
    //
    InvalidateRect(lpmwd->hwndImg, NULL, TRUE);
    UpdateWindow(lpmwd->hwndImg);

  }

  // Send information to the Server for the visible VUs
  //
	if(hwnd)
		RequestVisibleVU(lpmwd, iPrevStart, iPrevEnd);
}

//======================================================
//FUNCTION: ScrollImgWindow
//
//params:
//      HWND                hwnd; the paint structure
//      LPMIXERWNDDATA      lpMWD; Data needed to redraw
//
//purpose:
//  Calculate the NEW vertical offset of the Window
// and then Repaint it ....
//======================================================
void      ScrollImgWindow(HWND hwnd, LPMIXERWNDDATA lpmwd)
{
  int             iYChange;
  PAINTSTRUCT     ps;
  POINT           pnt;
  int             iOldYOffset = lpmwd->iYOffset;

  iYChange = lpmwd->pntMouseCur.y - lpmwd->pntMouseLast.y;

  if(iYChange == 0)
      return; // nothing to scroll

  if(iYChange < 0)
  {
		if((iYChange + lpmwd->iYOffset) <= 0)
				{
				if(lpmwd->iYOffset == 0)    
						// we already at the top
						return;
				lpmwd->iYOffset = 0;
				}
		else
				{
				lpmwd->iYOffset = lpmwd->iYOffset + iYChange;
				}
  }
  else
  {
		if(lpmwd->rMaxSize.bottom <= 
			 (iYChange + lpmwd->iYOffset + lpmwd->rVisible.bottom))
		{
			iYChange = lpmwd->rMaxSize.bottom - lpmwd->rVisible.bottom;
			if(iYChange == lpmwd->iYOffset)
				// we are already Scrolled to the bottom
				return; 
			lpmwd->iYOffset = iYChange;
		}
		else
		{
			lpmwd->iYOffset = lpmwd->iYOffset + iYChange;
		}
  }


  InvalidateRect(hwnd, NULL, FALSE);
  BeginPaint(hwnd, &ps);
  DrawImgWindow(&ps, lpmwd);
  EndPaint(hwnd, &ps);

  pnt = lpmwd->pntMouseCur;
  if((pnt.y == 0) || (pnt.y == (lpmwd->rVisible.bottom - 1)))//lpmwd->rVisible.bottom - 1))
  {
    // Snap the cursor
    // so we can continue to scroll
    //-----------------------------
    pnt.y = lpmwd->rVisible.bottom / 2;
    // So we will not scroll after this message
    //-----------------------------------------
    lpmwd->pntMouseCur.y = lpmwd->pntMouseLast.y = pnt.y;
    ClientToScreen(hwnd, &pnt);
    SetCursorPos(pnt.x, pnt.y);
  }

  if(lpmwd->iYOffset < 2853)
  {
    if(iOldYOffset >= 2853)
    {
      InvalidateRect(lpmwd->hwndLblGroup, NULL, TRUE);
      UpdateWindow(lpmwd->hwndLblGroup);
    }
  }
  else
  {
    if(iOldYOffset <= 2853)
    {
      InvalidateRect(lpmwd->hwndLblGroup, NULL, TRUE);
      UpdateWindow(lpmwd->hwndLblGroup);
    }
  }

return;
};

//======================================================
//FUNCTION: ScrollImgWindowRelative
//
//params:
//      HWND                hwnd; the paint structure
//      LPMIXERWNDDATA      lpMWD; Data needed to redraw
//
//purpose:
//  Adjust the Scroll Speed for the Relative Scrolling
// in regards to the mouse position where the mode was
// activated 
//======================================================
void      ScrollImgWindowRelative(HWND hwnd, LPMIXERWNDDATA lpmwd)
{
int             iYChange;
int             iOldYSpeed;

iYChange = lpmwd->pntMouseCur.y - lpmwd->pntMouseLast.y;

if((iYChange < 10) && (iYChange > -10))
    {
    g_iYSpeed = 0;
    DeleteObject(SetCursor(LoadCursor(ghInstMain, MAKEINTRESOURCE(IDCUR_UPDOWN_SCROLLER))));
    return; // nothing to scroll
    }

iOldYSpeed = g_iYSpeed;
g_iYSpeed = ((-iYChange) / 3) * 4; // 3 * 2

if(g_iYSpeed > 0)
  {
  if(iOldYSpeed <= 0)
    DeleteObject(SetCursor(LoadCursor(ghInstMain, MAKEINTRESOURCE(IDCUR_UPDOWN_DOWN))));
  }
else
  {
  if(iOldYSpeed >= 0)
    DeleteObject(SetCursor(LoadCursor(ghInstMain, MAKEINTRESOURCE(IDCUR_UPDOWN_UP))));
  }

return;
}
//======================================================
//FUNCTION: ScrollImgWindowRelative
//
//params:
//      HWND                hwnd; the paint structure
//      LPMIXERWNDDATA      lpMWD; Data needed to redraw
//
//purpose:
//  Scroll the Winddow using the g_iYSpeed
//======================================================
void      HandleScrollImgWindowRelative(HWND hwnd, LPMIXERWNDDATA lpmwd)
{
  int   iYChange;
  PAINTSTRUCT     ps;
  int             iOldYOffset = lpmwd->iYOffset;
  //POINT           pnt;


if(g_iYSpeed == 0)
  return; // No Speed we are stoped

if(g_iYSpeed < 0)
    {
    if((g_iYSpeed + lpmwd->iYOffset) <= 0)
        {
        if(lpmwd->iYOffset == 0)    
            // we already at the top
            return;
        lpmwd->iYOffset = 0;
        }
    else
        {
        lpmwd->iYOffset = lpmwd->iYOffset + g_iYSpeed;
        }
    }
else
    {
    if(lpmwd->rMaxSize.bottom <= 
       (g_iYSpeed + lpmwd->iYOffset + lpmwd->rVisible.bottom))
        {
        iYChange = lpmwd->rMaxSize.bottom - lpmwd->rVisible.bottom;
        if(iYChange == lpmwd->iYOffset)
            // we are already Scrolled to the bottom
            return; 
        lpmwd->iYOffset = iYChange;
        }
    else
        {
        lpmwd->iYOffset = lpmwd->iYOffset + g_iYSpeed;
        }
    }


InvalidateRect(hwnd, NULL, FALSE);
BeginPaint(hwnd, &ps);
DrawImgWindow(&ps, lpmwd);
EndPaint(hwnd, &ps);


  if(lpmwd->iYOffset < 2853)
  {
    if(iOldYOffset >= 2853)
    {
      InvalidateRect(lpmwd->hwndLblGroup, NULL, TRUE);
      UpdateWindow(lpmwd->hwndLblGroup);
    }
  }
  else
  {
    if(iOldYOffset <= 2853)
    {
      InvalidateRect(lpmwd->hwndLblGroup, NULL, TRUE);
      UpdateWindow(lpmwd->hwndLblGroup);
    }
  }

return;
};

//======================================================
//FUNCTION: MixerWindowDataAlloc
//
// params:
//          WORD    wMixer;  the mixer for which
//                           this Data will be
//                           allocated
//          WORD far*lpwZM; the long pointer to the
//                          Zone Map for this mixer-view
//          long    lpZMCount; count of the actual ZoneMaps
//
//return:
//      MIXERWNDDATA    far* == NULL is an error
//                           != NULL the memory has
//                              been allocated OK
//
//
//purpouse:
//      to allocate the memory for the MixerViewData
//
//
//======================================================
LPMIXERWNDDATA  MixerWindowDataAlloc(WORD wMixer,
                                     LPZONE_MAP lpwZM,
                                     long lZMCount, 
                                     int iType)
{
LPMIXERWNDDATA      lpmwd;

lpmwd = NULL;

lpmwd = (LPMIXERWNDDATA)GlobalAlloc(GPTR, sizeof(MIXERWNDDATA));
if(lpmwd)
    {
    lpmwd->iWndID = MIXER_WINDOW_FILE_ID;
    lpmwd->iMixer = wMixer;
    lpmwd->lpZoneMap = lpwZM;
    lpmwd->lZMCount = lZMCount;
    lpmwd->iXOffset = 0;
    lpmwd->iYOffset = 4000; // Set it to a big number so the Controls
                            // will appear as if they ImgWindow was scrolled 
                            // all the way to the bottom, where the Level faders
                            // are.

    InitRemapTable(lpmwd);

    // Adjust the Window channels array to display only the windows that we want
    // They should be only one type of channels
    //--------------------------------------------------------------------------
    if(iType == 1)
      MakeModulesArraySameType(lpmwd, iType, 2);
    else
      MakeModulesArraySameType(lpmwd, iType, NULL_MODULE_TYPE);

    GetMaxWindowSize(&lpmwd->rMaxWndPos, lpwZM, lpmwd->lZMCount, lpmwd);
    }
return lpmwd;
}

//=========================================
//FUNCTION: MixerWindowDataFree
//
//purpouse: Free the MixerWindowData
//      allocated with MixerWindowDataAlloc
//
//=========================================
void      MixerWindowDataFree(LPMIXERWNDDATA lpmwd)
{

if(lpmwd != NULL)
    {
    GlobalFree((HGLOBAL)lpmwd);
    }
return;
}

//===========================================
//function: ImageWindowSzie
//
// return:
//          TRUE if rVisible is and yOffset
//          are correctly set;
//          FASLE if rVisible is larger than
//          the MaxSize or the yOffset
//          is incorrect ...
//Checks and Sets the Size of an Image Window
// the result is sotred in pRect
//
//===========================================
BOOL    ImageWindowSize(HWND hwndParent, LPRECT pRect, 
                        LPMIXERWNDDATA lpmwd)
{
RECT    rClient;
BOOL    bRet;


	WaitForSingleObject(gDisplayEvent, 70);

GetClientRect(hwndParent, &rClient);


// make sure the iYOffset is a vaild value
// it can't be a negative number
//----------------------------------------
if(lpmwd->iYOffset < 0)
    lpmwd->iYOffset = 0;

rClient.top = HEIGHT_FULL_LABEL_WND;
rClient.bottom -= HEIGHT_FULL_LABEL_WND;
if((rClient.bottom + lpmwd->iYOffset) > lpmwd->rMaxSize.bottom)
    {
    // Do we need to adjust the iYOffset only
    // or the Window is just way too big
    //---------------------------------------
    if(rClient.bottom > lpmwd->rMaxSize.bottom)
        {
        rClient.bottom = lpmwd->rMaxSize.bottom;
        lpmwd->iYOffset = 0;
        }
    else
        {
        // Set the NEW offset Position
        //----------------------------
        lpmwd->iYOffset = lpmwd->rMaxSize.bottom - rClient.bottom;
        }

    bRet = FALSE;
    }
else
    bRet = TRUE;


*pRect = rClient;

	SetEvent(gDisplayEvent);

return bRet;
}

//========================================================
// FUNCTION: RegisterScrollWindowClass
//
//
//========================================================
int   RegisterScrollWindowClass()
{
int         iRet = 0; // OK
WNDCLASS    wc;


// Register Full View Class
//--------------------------
ZeroMemory(&wc, sizeof(WNDCLASS));      // Clear wndclass structure

wc.style = CS_HREDRAW | CS_VREDRAW;
wc.lpfnWndProc = (WNDPROC)ScrollerWndProc;
wc.cbClsExtra = 0;
wc.cbWndExtra = 0;
wc.hInstance = ghInstMain;
wc.hIcon = NULL;// this might leak memory
wc.hCursor = NULL;
wc.hbrBackground = GetStockObject(BLACK_BRUSH);
wc.lpszMenuName = NULL;
wc.lpszClassName = gszScrollerWindowClass;

iRet = RegisterClass(&wc);

if(iRet == 0)
	 return(IDS_ERR_REGISTER_CLASS);     // Error... Exit


return 0;
}

//======================================================
// FUNCTION: ScrollerWndProc
//
// Window procedure
//
//======================================================
LRESULT CALLBACK  ScrollerWndProc(HWND hwnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
PAINTSTRUCT         ps;
RECT                r;
HDC                 hdc;
HBITMAP             hbmp;
BITMAP              bmpInfo;


switch (wMessage)
  {
  case WM_ERASEBKGND: // to reduce flashing on the screen
    GetObject(g_hbmpUDScroller, sizeof(BITMAP), &bmpInfo);


    InvalidateRect(hwnd, NULL, FALSE);
    BeginPaint(hwnd, &ps);    
    hdc = CreateCompatibleDC(ps.hdc);
    hbmp = SelectObject(hdc, g_hbmpUDScroller);
    BitBlt(ps.hdc, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight,
           hdc, 0, 0, SRCCOPY);
    SelectObject(hdc, hbmp);
    DeleteDC(hdc);

//    FillRect(ps.hdc, &r, GetStockObject(WHITE_BRUSH));    
    EndPaint(hwnd, &ps);
    break;

  case WM_PAINT:
    r.left = r.top = 0;
    r.right = r.bottom = 25;
    BeginPaint(hwnd, &ps);    
    SelectClipRgn(ps.hdc, g_hrgn);
    FillRgn(ps.hdc, g_hrgn, GetStockObject(WHITE_BRUSH));    
    EndPaint(hwnd, &ps);
    break;
  default:
    return DefWindowProc(hwnd, wMessage, wParam, lParam);
  }
return 0;
}

//======================================================
// FUNCTION: ShowScrollWindow
//
//
// return:
//        int     n; = Error
//                0; = OK
// Window that shows that we are in Scroll mode
//
//======================================================
int   ShowScrollWindow(HWND  hwndParent, LPMIXERWNDDATA lpmwd)
{
int     iRet = 0; // OK
BITMAP  bmpInfo;
POINT   pnt;
           
if((g_hwndScroller == NULL) && hwndParent)
  {
  g_hbmpUDScroller = LoadBitmap(ghInstMain, MAKEINTRESOURCE(IDB_UPDOWN_SCROLLER));
  if(g_hbmpUDScroller)
    {
    GetObject(g_hbmpUDScroller, sizeof(BITMAP), &bmpInfo);
    pnt = lpmwd->pntMouseCur;
    ClientToScreen(hwndParent, &pnt);

    g_hwndScroller = CreateWindowEx(
             0,
					   gszScrollerWindowClass,   // Window class name
					   NULL,                   // Window's title
					   WS_CLIPSIBLINGS | WS_POPUP,
             pnt.x - bmpInfo.bmWidth/2,
             pnt.y - bmpInfo.bmHeight/2,
					   bmpInfo.bmWidth,
					   bmpInfo.bmHeight,
  //					 lpmwd->rVisible.right / 4,   // Set it to the max Width
  //					 lpmwd->rVisible.bottom/ 4,  // Set it to the max Height
					   hwndParent,             // Parent window's handle
					   NULL,                   // Default to Class Menu
					   ghInstMain,              // Instance of window
					   NULL                 // Ptr To Data Structure For WM_CREATE
             );

    g_hrgn = CreateEllipticRgn(0, 0, bmpInfo.bmWidth + 1, bmpInfo.bmHeight + 1);
    SetWindowRgn(g_hwndScroller, g_hrgn, TRUE);
    ShowWindow(g_hwndScroller, SW_SHOW);
    }
  }
else
  {
  // First Destroy the Window ....
  DestroyWindow(g_hwndScroller);
  // Then Cleanup the mess ... doing it the other way was creating a bug ...
  // hope this is a fix 11/02/1997 Histo ... 
  DeleteObject(g_hrgn);
  DeleteObject(g_hbmpUDScroller);
  g_hwndScroller = NULL;
  g_hbmpUDScroller = NULL;
  }

return iRet;
}

/////////////////////////////////////////////////////////////////////////
// FUNCTION: DisplayVU_Data
//
// purpose:  Display the VU data in the FULL IMAGE Window.... !!!
//
// parms:
//    VU_READ *pVuData; // pointer to the buffer that contains the VU Data
//    int     iSize; // size of the Valid DATA in the buffer pointed to by 
//                   //  pVuData !!!!      
//
static DWORD    dwVuPacketsCount = 0;
static LARGE_INTEGER liQueryPerformance = {0};
static LARGE_INTEGER liPerformanceCounterStart = {0};
static LARGE_INTEGER liPerformanceCounterEnd = {0};
static LARGE_INTEGER liVUDisplayCur = {0};
static LARGE_INTEGER liVUDisplayDealy = {0};
static DWORD					VU_delaystep = 0;
static double    dVUResolution;
static DWORD     dwMoreThanOne = 0;
#include <stdio.h>
void      DrawVUReadOutImgWindow(LPPAINTSTRUCT lpPS,
                                  LPMIXERWNDDATA lpMWD,
                                  VU_READ *pVuData,
                                  int   iVUCount); // Just a prototype
void  DisplayVU_Data(VU_READ *pVuData, int iSize)
{
  int             iVU_Count = iSize / sizeof(VU_READ);
  int             iCount = 0;
  PAINTSTRUCT     ps = {0};
  BOOL            bMasterData = FALSE;
  BOOL            bGeneralVuData = FALSE;
  int             iSubCount;

  char            szVuDataBuffer[512];
  VU_READ         *pVuDataBuffer = (VU_READ *)szVuDataBuffer;

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

  HWND            hwndT, hwndTLast = NULL;
  LPMIXERWNDDATA  lpmwd = NULL;
  LPCTRLZONEMAP   pCtrlZm = NULL;

// Debug stuff for VU display frequency ... !!!! Should be deleted
  char            szBuff[128];

  if(liQueryPerformance.QuadPart == 0)
  {
    QueryPerformanceFrequency(&liQueryPerformance);
    QueryPerformanceCounter(&liPerformanceCounterStart);
		VU_delaystep = (DWORD)((float)(liQueryPerformance.QuadPart / 1000.)*2.0f);//mseconds
  }

  if(iVU_Count > 0)
  {
    dwVuPacketsCount += iVU_Count;
    if(iVU_Count > 1)
      dwMoreThanOne ++;
  }

	// Delay some msec ..
	//

	QueryPerformanceCounter(&liVUDisplayDealy);
	while(liVUDisplayDealy.QuadPart < liVUDisplayCur.QuadPart + VU_delaystep){
		QueryPerformanceCounter(&liVUDisplayDealy);
	}

	QueryPerformanceCounter(&liVUDisplayCur);

  if(dwVuPacketsCount >= 1000)
  {
    QueryPerformanceCounter(&liPerformanceCounterEnd);
    dVUResolution = (double)(liPerformanceCounterEnd.QuadPart - liPerformanceCounterStart.QuadPart)/
                     (double)liQueryPerformance.QuadPart;
    dVUResolution = 1000 / dVUResolution;


    sprintf(szBuff, "VRes=%05.02f ", (float)dVUResolution);//dwMoreThanOne);
    SendMessage(ghwndStatus, SB_SETTEXT, MAKEWPARAM(0,SBT_POPOUT), (LPARAM)szBuff);


    dwVuPacketsCount = 0;
    QueryPerformanceCounter(&liPerformanceCounterStart);
  }



// END of DEBUG STUFF
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
  

  CopyMemory(szVuDataBuffer, pVuData, iSize);

	
	//////////////////////////////////////////////////////////
  // Flip the incoming VU data channel to the Correct 
  // Screen Channel !!!!
	// The clip light logic will use the upper byte of
	// the .wModuleIdx, the VU logic will use the lower byte
	//////////////////////////////////////////////////////////

  for(iCount = 0; iCount < iVU_Count; iCount ++)
  {
		// This MUST be done for ALL VU data packets
		// Need to put the module idx for the clip lights in upper byte also
		pVuDataBuffer[iCount].wModuleIdx =  (pVuDataBuffer[iCount].wModuleIdx << 8) | pVuDataBuffer[iCount].wModuleIdx;

		////////////////////////////
		// Check for the CUE module
		////////////////////////////
    if( (pVuDataBuffer[iCount].wModuleIdx  & 0x0FF) == g_iCueModuleIdx)
    {
			pVuDataBuffer[iCount].wModuleIdx = (pVuDataBuffer[iCount].wModuleIdx & 0xff00) | g_iMasterModuleIdx;
      pVuDataBuffer[iCount].cLock = 8; // Use this as an offset
      bMasterData = TRUE;

			// Display the CUE data in the seperate CUE VU Window
			DisplayStereoCueVUData(pVuDataBuffer);

    }
    else
    {
      pVuDataBuffer[iCount].cLock = 0;

			///////////////////////////////
			// Check for the MASTER module
			///////////////////////////////
      if( (pVuDataBuffer[iCount].wModuleIdx & 0x00FF) ==  g_iMasterModuleIdx)
      {
        bMasterData = TRUE;
      }
      else
      {
        bGeneralVuData = TRUE;
        for(iSubCount = 0; iSubCount < MAX_MATRIX_COUNT; iSubCount++)
        {
					/////////////////////////////////////////////////////////////////
					// Check for the  Matrix modules and flip them to the Aux modules
					// Since the program *thinks* they reside on the same Module
					/////////////////////////////////////////////////////////////////
          if(g_aiMatrix[iSubCount] != 0)
          {
            if(g_aiMatrix[iSubCount] == (pVuDataBuffer[iCount].wModuleIdx & 0x00FF))
            {
							// Put the original module index into the upper byte to use with the clip lights
							// The lower byte will have the modified module index
							pVuDataBuffer[iCount].wModuleIdx = (pVuDataBuffer[iCount].wModuleIdx & 0xff00) |  g_aiAux[iSubCount] ;
							pVuDataBuffer[iCount].cLock = 8;
            }
          }
        }
      }
    }
  }


	//////////////////////////////////////////
	// Update the Master Window if it is open
	//////////////////////////////////////////

  if((ghwndMaster != NULL) && bMasterData)
  {
    lpmwd = (LPMIXERWNDDATA)GetWindowLong(ghwndMaster, 0);
    if(lpmwd != NULL && lpmwd->hwndImg != NULL)
    {
      ps.hdc = GetDC(lpmwd->hwndImg);
      if(ps.hdc)
      {
        DrawVUReadOutImgWindow(&ps, lpmwd, pVuDataBuffer, iVU_Count);
        ReleaseDC(lpmwd->hwndImg, ps.hdc);
      }
    }
  }

  /////////////////////////////////////////////
  // now update all MDI windows
  /////////////////////////////////////////////

  if( bGeneralVuData )
  {
    hwndT = GetTopWindow(ghwndMDIClient);
    while(hwndT && hwndT != hwndTLast)
    {
      lpmwd = (LPMIXERWNDDATA)GetWindowLong(hwndT, 0);
      hwndTLast = hwndT;

      if(lpmwd != NULL && lpmwd->iWndID == MIXER_WINDOW_FILE_ID && lpmwd->hwndImg != NULL)
      {
        if((lpmwd->iCurMode != MW_SCROLL) && (lpmwd->iCurMode != MW_SCROLL_RELATIVE))
        {
          ps.hdc = GetDC(lpmwd->hwndImg);
          if(ps.hdc)
          {
            DrawVUReadOutImgWindow(&ps, lpmwd, pVuDataBuffer, iVU_Count);
            ReleaseDC(lpmwd->hwndImg, ps.hdc);
          }
        }
      }
      hwndTLast = hwndT;
      hwndT = GetNextWindow(hwndT, GW_HWNDNEXT);
    }
  }
}


//======================================================
//FUNCTION: DrawVUReadOutImgWindow
//
//params:
//      LPPAINTSTRUCT       lpPS; the paint structure
//      LPMIXERWNDDATA      lpMWD; Data needed to redraw
//======================================================
void      DrawVUReadOutImgWindow(LPPAINTSTRUCT lpPS,
                                  LPMIXERWNDDATA lpMWD,
                                  VU_READ *pVuData,
                                  int   iVUCount)
{
  HDC         hdcMem;
  HDC         hdc;
  HBITMAP     hbmpMemOld = NULL;
  HBITMAP     hbmpOld = NULL;
  HBITMAP     hbmpBuff = NULL;
  long        lCount;//, lZMCount;
  int         iX, iY, iCX, iCY;
  int         iBMPIndex;
  int         iPhisChannel;
  int         iMixer;
  int         iVUCounter;
  VU_READ     *pVuWalker;
  LPCTRLZONEMAP   pCtrlZm = NULL;
  LPCTRLZONEMAP   pCtrlZmClip = NULL;
	//int							tempModuleType  = 0;
  

hbmpOld = NULL;
hbmpBuff = NULL;

if(lpMWD == NULL)
    return;
hdc = lpPS->hdc; // assign to local ... for speed only
hdcMem = CreateCompatibleDC(hdc);

// apply the vertical offset
//--------------------------

iX = iY = iCX = iCY = 0;

// Loop through all the channels that are on the screen

  for(lCount = lpMWD->iStartScrChan; lCount < lpMWD->iEndScrChan + 1; lCount++)
  {
    iMixer = HIBYTE(lpMWD->lpwRemapToScr[lCount]);
    iPhisChannel = LOBYTE(lpMWD->lpwRemapToScr[lCount]); // Get the actual phisical channel
    
    iBMPIndex = lpMWD->lpZoneMap[iPhisChannel].iBmpIndx;
    iCX = gpBMPTable[iBMPIndex].iWidth;
    iCY = gpBMPTable[iBMPIndex].iHeight;

    pVuWalker = pVuData;

    // THIS IS ONLY FOR DEBUG !!!
		// Check the VU packet 
    //
		/*
    if(iPhisChannel != (int)pVuWalker->wModuleIdx)
		{
			tempModuleType = gDeviceSetup.iaChannelTypes[pVuWalker->wModuleIdx];
			if (tempModuleType == 3 || tempModuleType == 4)
				tempModuleType = 5; // break point place :)
		}
		*/

		////////////////////////////////////////////////////////////////////////
		// update all clip lights
		////////////////////////////////////////////////////////////////////////

		pCtrlZmClip = ScanCtrlZonesType(lpMWD->lpZoneMap[iPhisChannel].lpZoneMap, CTRL_TYPE_CLIP_LIGHT );
		while (pCtrlZmClip != NULL)
		{
			// Compare the module index with the one we are looking at 
			// The ModuleIdx for Clip data is in the upper byte		 
		
			if(  ((((pVuWalker->wModuleIdx >> 8) & 0x00ff) == pCtrlZmClip->iModuleNumber) &&
				    (pCtrlZmClip->iCtrlChanPos != CTRL_NUM_CUE_CLIP_LIGHT) ) || 
				  (  (pCtrlZmClip->iCtrlChanPos == CTRL_NUM_CUE_CLIP_LIGHT) && 
				    (((pVuWalker->wModuleIdx >> 8) & 0x00ff) == pCtrlZmClip->iModuleNumber+1) ))
			{
				//
				if (pVuWalker->wPeakClipValue != 0)
				{
					hbmpOld = SelectObject (g_hdcBuffer, gpBMPTable[pCtrlZmClip->iCtrlBmp[0]].hbmp);
					BitBlt (hdc, pCtrlZmClip->rZone.left + iX, pCtrlZmClip->rZone.top - lpMWD->iYOffset,
											 pCtrlZmClip->rZone.right - pCtrlZmClip->rZone.left, 
											 pCtrlZmClip->rZone.bottom - pCtrlZmClip->rZone.top,
									g_hdcBuffer, 0, 0, SRCCOPY);

				}
				else
				{	
					hbmpOld = SelectObject(g_hdcBuffer, gpBMPTable[iBMPIndex].hbmp);
					BitBlt (hdc, pCtrlZmClip->rZone.left + iX, pCtrlZmClip->rZone.top - lpMWD->iYOffset,
											 pCtrlZmClip->rZone.right - pCtrlZmClip->rZone.left, 
											 pCtrlZmClip->rZone.bottom - pCtrlZmClip->rZone.top ,
									g_hdcBuffer, pCtrlZmClip->rZone.left, pCtrlZmClip->rZone.top, SRCCOPY);
				}
				SelectObject (g_hdcBuffer, hbmpOld);
			}
			pCtrlZmClip ++;
			pCtrlZmClip = ScanCtrlZonesType(pCtrlZmClip, CTRL_TYPE_CLIP_LIGHT);
		}


		////////////////////////////////////////////////////////////////////////
		// Now update the VU data
		////////////////////////////////////////////////////////////////////////

    for(iVUCounter = 0; iVUCounter < 8;iVUCounter++)
    {
      // Skip channels that are not for this VU
      //  ModuleIdx for VU data is in Lower byte

			//    If this is uncommented then the CUE VU meters will not work, only the seperate window CUE VUs
			//		But this is needed to keep the SUB AUX's from moving when input/matrix is chosen


#ifdef MINE
     if(iPhisChannel != (int)(pVuWalker->wModuleIdx & 0x00ff) &&
			 (pVuWalker->wModuleIdx == 0) ||
			 (pCtrlZm == NULL))	// Mask out the upper byte used for the clip lights
        continue;
#else
     if(iPhisChannel != (int)(pVuWalker->wModuleIdx & 0x00ff))
			 continue;

#endif

      pCtrlZm = ScanCtrlZonesType(lpMWD->lpZoneMap[iPhisChannel].lpZoneMap, CTRL_TYPE_VU_DISPLAY + iVUCounter + pVuWalker->cLock);
      //pCtrlZm = ScanCtrlZonesType(lpMWD->lpZoneMap[iPhisChannel].lpZoneMap, CTRL_TYPE_VU_DISPLAY + (pVuData->iVUType - 1) );


      if(pCtrlZm == NULL)
        continue;

      // if it is the same index we don't need to reselect it
      // in the HDC
      //-----------------------------------------------------
      hbmpOld = SelectObject(g_hdcBuffer, gpBMPTable[iBMPIndex].hbmp);
      if(gbmpVUVertBuffer == NULL)
      {
        // This is a global variable ... so it will be initialized only once ..
        //
        gbmpVUVertBuffer = CreateCompatibleBitmap(hdc, pCtrlZm->rZone.right - pCtrlZm->rZone.left,
                                                 pCtrlZm->rZone.bottom - pCtrlZm->rZone.top);
      }
      if(gbmpVUHorzBuffer == NULL)
      {
        // This is a global variable ... so it will be initialized only once ..
        //
        gbmpVUHorzBuffer = CreateCompatibleBitmap(hdc, 96, 9);
      }

      // Select the correct bitmap
      //
      if((pCtrlZm->iCtrlType == CTRL_TYPE_VU_DISPLAY4) || 
          (pCtrlZm->iCtrlType == CTRL_TYPE_VU_DISPLAY6))
      {
        hbmpMemOld = SelectObject(hdcMem, gbmpVUHorzBuffer);
      }
      else    
      {
        hbmpMemOld = SelectObject(hdcMem, gbmpVUVertBuffer);
      }

      BitBlt(hdcMem, 0, 0, 
                  pCtrlZm->rZone.right - pCtrlZm->rZone.left, 
                  pCtrlZm->rZone.bottom - pCtrlZm->rZone.top ,
             g_hdcBuffer, pCtrlZm->rZone.left, 
                          pCtrlZm->rZone.top, SRCCOPY);

			//restore the content of the gdc buffer
      SelectObject(g_hdcBuffer, hbmpOld);

      // Draw the VU data Display
      // into the Memory Bitmap
			// Pass the lower byte of ModuleIdx only, upper byte is for clip lights only
      //-------------------------
      DrawVUData(hdcMem, pCtrlZm,  pVuData, lpMWD, (int)pVuData->wModuleIdx & 0x00ff, iVUCounter + pVuWalker->cLock); // !!
      // copy it to the screen
      BitBlt(hdc, pCtrlZm->rZone.left + iX, pCtrlZm->rZone.top - lpMWD->iYOffset, 
                  pCtrlZm->rZone.right - pCtrlZm->rZone.left, 
                  pCtrlZm->rZone.bottom - pCtrlZm->rZone.top ,
             hdcMem, 0, 0, SRCCOPY);
 
    }
    // advance x with the width of the current bitmap
    //-----------------------------------------------
    iX += iCX;
  }
// restore the old bitmap for this DC
//-----------------------------------
SelectObject(hdcMem, hbmpMemOld);
DeleteDC(hdcMem);
if(hbmpBuff)
  DeleteObject(hbmpBuff);

return;
}

//======================================================
//FUNCTION: AddMWEntryToDLList
//
//param:
//      HWND                hwnd; -> these two variables
//      LPMIXERWNDDATA      lpmwd;-> will get stored
//                                   as an extra info
//                                   in the doub. linked
//                                   list
//
// return:
//          0 = OK
//          n = error
//======================================================
/*
int       AddMWEntryToDLList(HWND hwnd, LPMIXERWNDDATA lpmwd)
{
LPDLENTRY       lpdle;
LPMW_DL_LIST    lpMW_DL;

lpdle = AddEntry(lpMixerWindows_DL_List, sizeof(MW_DL_LIST));
if(lpdle == NULL)
    return 1;

lpmwd->lpdle = lpdle;

lpMW_DL = (LPMW_DL_LIST)GetEntryData(lpMixerWindows_DL_List, lpdle);
lpMW_DL->hwndMW = hwnd; // Store the Window handle
lpMW_DL->lpMWD = lpmwd; // Store the Window Data Pointer

return 0;
}

//=====================================================
//FUNCTION: DeleteMWEntryFromDLList
//
//called whenever the window is destroyed
//=====================================================
void      DeleteMWEntryFromDLList(LPMIXERWNDDATA lpmwd)
{
if(lpmwd)
    // Delete the Entry using the pointer
    // to it and not the index
    //-----------------------------------
    DelEntryPtr(lpMixerWindows_DL_List, lpmwd->lpdle);

return;
}
*/
