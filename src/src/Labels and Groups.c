//=================================================
// Copyright 1998, CorTek Software, Inc.
//=================================================

//=================================================
// The window that displays the labels and the
// Grouping
//
//=================================================

//#include <windows.h>

#include "SAMM.h"
#include "SAMMEXT.h"
#include "MACRO.h"




extern int                 g_aiAux[MAX_MATRIX_COUNT];
extern int                 g_aiMatrix[MAX_MATRIX_COUNT];

LPSTR   GetLabelText(LPCTRLZONEMAP lpctrlZM, int iChan);


//=====================================
// FUNCTION: RegLblGroupWnd
//
//=====================================
int      RegLblGroupWnd(void)
{
int         iReturn;
WNDCLASS    wc;


// Register Full View Class
//--------------------------
memset(&wc, 0x00, sizeof(WNDCLASS));      // Clear wndclass structure

wc.style = CS_HREDRAW | CS_VREDRAW;
wc.lpfnWndProc = (WNDPROC)LblGroupProc;
wc.cbClsExtra = 0;
wc.cbWndExtra = sizeof(LPSTR);// it will contain the type of the window
wc.hInstance = ghInstMain;
wc.hIcon = NULL;
wc.hCursor = LoadCursor(NULL, IDC_ARROW);
wc.hbrBackground = GetStockObject(WHITE_BRUSH);
wc.lpszMenuName = NULL;
wc.lpszClassName = gszLblGroupClass;

iReturn = RegisterClass(&wc);

if(iReturn == 0)
	 return(IDS_ERR_REGISTER_CLASS);     // Error... Exit

return 0;
}

//===========================
//FUNCTION: CreateLblGroupWnd
//
//===========================
HWND     CreateLblGroupWnd(LPRECT prect, HWND hwndParent,LPMIXERWNDDATA lpmwd)
{
HWND        hwnd = NULL;

hwnd = CreateWindow(
					 gszLblGroupClass,   // Window class name
					 NULL,              // Window's title
					 WS_CHILD  |
           WS_CLIPSIBLINGS |
           WS_VISIBLE,
					 prect->left,
					 prect->top,
					 prect->right,       // Set it to the max Width
					 prect->bottom,      // Set it to the max Height
					 hwndParent,        // Parent window's handle
					 NULL,              // Default to Class Menu
					 ghInstMain,         // Instance of window
					 NULL               // Ptr To Data Structure For WM_CREATE
                   );

if(hwnd == NULL)
    {
    return NULL;
    }

lpmwd->hwndLblGroup = hwnd;
SetWindowLong(hwnd, 0, (LPARAM)lpmwd);
UpdateWindow(hwnd);

return hwnd;
}

//===========================
//FUNCTION: LblGroupProc
//
//===========================
LRESULT CALLBACK LblGroupProc(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
  LPMIXERWNDDATA      lpmwd;
  PAINTSTRUCT         ps;
  HDC                 hdc;
  int                 iXVal;

lpmwd = (LPMIXERWNDDATA)GetWindowLong(hWnd,0);

switch (wMessage)
  {
  //////////////////////////////////////////////////////////////
  case WM_LBUTTONDOWN:
    iXVal = LOWORD(lParam);
    if(CalculatePhisChannelFromScreen( &iXVal, lpmwd))
      if(GroupChannel(iXVal, 1))
      {
        InvalidateRect(hWnd, NULL, FALSE);
        UpdateWindow(hWnd);
				RefreshAllLblWindows();
      }

    break;
  //////////////////////////////////////////////////////////////
  case WM_RBUTTONDOWN:
    iXVal = LOWORD(lParam);
    if(CalculatePhisChannelFromScreen( &iXVal, lpmwd))
      if(UnGroupChannel(iXVal))
      {
        InvalidateRect(hWnd, NULL, FALSE);
        UpdateWindow(hWnd);
				RefreshAllLblWindows();
      }
    
    break;

  //////////////////////////////////////////////////////////////
  case WM_ERASEBKGND:
      break;
  //////////////////////////////////////////////////////////////
  case WM_PAINT:
      hdc = BeginPaint(hWnd, &ps);
      DrawLbl(hdc, lpmwd);
      EndPaint(hWnd, &ps);
      break;
  //////////////////////////////////////////////////////////////
  default:
      return DefWindowProc(hWnd, wMessage, wParam, lParam);

  }


return 0;
}

//=================================================
//FUNCTION: DrawLbl
//
//params:
//      HDC             hdc; the DC of the window
//      LPMIXERWNDDATA  lpmwd; the general data
//                             for the mixer window
//purpose:
//      Draw the labels of the mixer
//      in the label window
//
//=================================================
void    DrawLbl(HDC hdc, LPMIXERWNDDATA lpmwd)
{
  HDC         hdcMem;
  HBITMAP     hbmpMem, hbmp;
  HBRUSH      hbrGray, hbrDkGray, hbrWhite, hbrBlack;
  int         iWidth, iHeight;
  RECT        rect = {0};
  char        szBuff[5];
  char        szText[32];
  int         iCount;
  int         iPhisChannel;
  int         iBmpIndex;
  int         iTarget, iic;

  if(lpmwd == NULL)
      return;

  szBuff[4] = 0; // set the last one to 0

  // Get the size of the Window
  //---------------------------
  GetClientRect(lpmwd->hwndLblGroup, &rect);

  iHeight = rect.bottom;
  iWidth  = rect.right;

  // Prepare the Bitmaps
  //--------------------
  hbmp    = CreateCompatibleBitmap(hdc, iWidth, iHeight);
  hbmpMem = CreateCompatibleBitmap(hdc, iWidth, iHeight);

  hdcMem = CreateCompatibleDC(hdc);

  hbrGray = GetStockObject(GRAY_BRUSH);
  hbrDkGray = GetStockObject(DKGRAY_BRUSH);
  hbrWhite = GetStockObject(WHITE_BRUSH);
  hbrBlack = GetStockObject(BLACK_BRUSH);

  // Go to a loop to draw all the visible labels
  // labels to the Memory bitmap
  //--------------------------------------------
  rect.right = 0; // Set it to this on purpose
  //for(iCount=0; iCount < lpmwd->lZMCount; iCount++)
  for(iCount = lpmwd->iStartScrChan; iCount < lpmwd->iEndScrChan + 1; iCount++)
      {
      iPhisChannel = lpmwd->lpwRemapToScr[iCount]; // Get the actual phis channel
      iBmpIndex = lpmwd->lpZoneMap[iPhisChannel].iBmpIndx;

      rect.right += gpBMPTable[iBmpIndex].iWidth;
      CopyMemory(szBuff, lpmwd->lpZoneMap[iPhisChannel].chDefLabel, 4);
      
      if(gDeviceSetup.iaChannelTypes[iPhisChannel] == 2 && 
        lpmwd->iYOffset < 2853)
      {
        iTarget = 0;
        for(iic = 0; iic < MAX_AUX_CHANNELS; iic++)
        {
          if(g_aiAux[iic] == iPhisChannel )
          {
            iTarget = iic;
            break;
          }
        }
				
				if(gpBMPTable[iBmpIndex].iWidth < 20)
					wsprintf(szText, "%s", szBuff);
				else
					wsprintf(szText, "%s\n%s", szBuff, &g_pMatrixLabels[iTarget * MAX_LABEL_SIZE]);
      }
      else
			{
				if(gpBMPTable[iBmpIndex].iWidth < 20)
					wsprintf(szText, "%s", szBuff);
				else
					wsprintf(szText, "%s\n%s", szBuff, &gpLabels[iPhisChannel * MAX_LABEL_SIZE]);
			}

      if(IsGrouped(iPhisChannel))
      {
        FillRect(hdc, &rect, hbrBlack);
        WriteTextLinesToDC(hdc  , &rect , 0, 0, RGB(255, 0, 0), (LPSTR)szText);
        //WriteTextToDC(hdc  , &rect , 0, 0, RGB(255, 0, 0), (LPSTR)szBuff);
      }
      else
      {
        FillRect(hdc, &rect, hbrGray);
        WriteTextLinesToDC(hdc  , &rect , 0, 0, RGB(0, 255, 0), (LPSTR)szText);
        //WriteTextToDC(hdc  , &rect , 0, 0, RGB(0, 255, 0), (LPSTR)szBuff);
      }

      rect.left = rect.right;
      }

  // Free the Bitmaps
  //-----------------
  DeleteObject(hbmp);
  DeleteObject(hbmpMem);
  // Free the DC
  //------------
  DeleteDC(hdcMem);
  return;
}


////////////////////////////////////
// CalculatePhisChannelFromScreen
//
//
BOOL  CalculatePhisChannelFromScreen(int *piRet, LPMIXERWNDDATA lpmwd)
{
  int   iPhisChannel;
  int   iBmpIndex;
  int   iWidth;
  int   iX; // the x possition on the screen
  

  iPhisChannel = lpmwd->lpwRemapToScr[lpmwd->iStartScrChan]; // Get the first physical channel
  iBmpIndex = lpmwd->lpZoneMap[iPhisChannel].iBmpIndx;
  iWidth = gpBMPTable[iBmpIndex].iWidth;

  iX = *piRet;
  iX = lpmwd->iStartScrChan + (iX/iWidth);
  if(iX > MAX_CHANNELS)
    return FALSE;

  // Cool now get the Physical Channel
  //
  *piRet = lpmwd->lpwRemapToScr[iX]; 

  if(*piRet > MAX_CHANNELS)
    return FALSE;


  return TRUE;
}


//////////////////////////////////////////////////////////////////////////////////
// DrawChannelLabel
//    Update the text for this label
//
//
void    DrawChannelLabel(HDC hdc, LPCTRLZONEMAP lpctrlZM, int iVal, LPMIXERWNDDATA lpmwd, int iChan)
{
  HFONT   hfont;
  RECT    r = lpctrlZM->rZone;
  LPSTR   pLabel;


  hfont = SelectObject(hdc, g_hConsoleFont);
  //iChan = LOWORD(lpmwd->lpwRemapToScr[iChan]); // Get the first physical channel

  pLabel = GetLabelText(lpctrlZM, iChan);
  if(pLabel)
    WriteTextToDC(hdc, &r, 0, 0, RGB(0, 0, 0), pLabel);
  SelectObject(hdc, hfont);
};


///////////////////////////////////////////////////////////////////////
//
//
//
LPSTR   GetLabelText(LPCTRLZONEMAP lpctrlZM, int iChan)
{
  LPSTR   pRet = NULL;
  int     iTarget;
  int     iCount;

  if(gDeviceSetup.iaChannelTypes[iChan] == 5)
  {
    // Check for the Aux Labels ...
    //
    if((lpctrlZM->iCtrlChanPos >= CTRL_NUM_AUX1_LABEL_LOOKUP) && 
       (lpctrlZM->iCtrlChanPos <= CTRL_NUM_AUX16_LABEL_LOOKUP))
    {
      iTarget = lpctrlZM->iCtrlChanPos - CTRL_NUM_AUX1_LABEL_LOOKUP;
      pRet = &g_pAuxLabels[iTarget * MAX_LABEL_SIZE];    
      /*
      for(iCount = 0; iCount < MAX_CHANNELS; iCount++)
      {
        if(gDeviceSetup.iaChannelTypes[iCount] == 2)
          if(iTarget == 0)
          {
            pRet = &g_pAuxLabels[iCount * MAX_LABEL_SIZE];    
            break;
          }
          else
            iTarget --;
      }
      */
    }
    else
      pRet =  &gpLabels[iChan * MAX_LABEL_SIZE];
  }
  else
    if(gDeviceSetup.iaChannelTypes[iChan] == 2 &&
      lpctrlZM->iCtrlChanPos == CTRL_NUM_MATRIX_LABEL)
    {
      iTarget = 0;
      for(iCount = 0; iCount < MAX_AUX_CHANNELS; iCount++)
      {
        if(g_aiMatrix[iCount] == lpctrlZM->iModuleNumber )
        {
          iTarget = iCount;
          break;
        }
      }

      pRet = &g_pMatrixLabels[iTarget * MAX_LABEL_SIZE];    
    }
    else
      pRet = &gpLabels[iChan * MAX_LABEL_SIZE];

  return pRet;
}



HWND      g_hwndLabelEdit = NULL;
WNDPROC   g_editWindowProc = NULL;
int       g_iChannelLabel = 0;
LPCTRLZONEMAP g_pCtrlZm = NULL;

/////////////////////////////////////////////////////////////////////////////////
//
//
//
LRESULT CALLBACK NewEditWindowProc( HWND hwnd, UINT uMsg,  WPARAM wParam,  LPARAM lParam )
{
  HWND    hwndParent;
  char    chBuffer[32];
  LPSTR   pLabel;

  switch(uMsg)
  {

  case WM_KEYDOWN:
    switch(wParam)
    {
    case VK_RETURN:
      hwndParent = GetParent(GetParent(hwnd));
      GetWindowText(hwnd, chBuffer, MAX_LABEL_SIZE);
      
      pLabel = GetLabelText(g_pCtrlZm, g_iChannelLabel);
      if(pLabel)
        wsprintf(pLabel, "%s", chBuffer);
      //wsprintf(&gpLabels[g_iChannelLabel * MAX_LABEL_SIZE], "%s", chBuffer);

      DestroyWindow(hwnd);
      RefreshAllLblWindows();
      SetFocus(hwndParent);
      break;
    case VK_ESCAPE:
      hwndParent = GetParent(GetParent(hwnd));
      DestroyWindow(hwnd);
      SetFocus(hwndParent);
      break;
    default:
      return CallWindowProc(g_editWindowProc, hwnd, uMsg, wParam, lParam);
      break;
    }
    
    break;

  default:
    return CallWindowProc(g_editWindowProc, hwnd, uMsg, wParam, lParam);
    break;
  }

  return 1;
}

///////////////////////////////////////////////////////////////////////////////////
//
//
//
void  EditModuleLabel(HWND hwnd, LPMIXERWNDDATA lpmwd, BOOL bShow)
{
  char          chBuffer[32];
  LPSTR         pLabel;
  

  if(bShow && lpmwd)
  {
    g_pCtrlZm= lpmwd->lpCtrlZM;

    if(g_hwndLabelEdit)
    {
      DestroyWindow(g_hwndLabelEdit);
      g_hwndLabelEdit = NULL;
    }
    ReleaseCapture();

		g_iChannelLabel = LOBYTE(lpmwd->lpwRemapToScr[lpmwd->iCurChan + 
							                          lpmwd->iStartScrChan]); //lpmwd->iCurChan;
 
    //wsprintf(chBuffer, "%s", &gpLabels[g_iChannelLabel * MAX_LABEL_SIZE]);
    pLabel = GetLabelText(g_pCtrlZm, g_iChannelLabel);
    if(pLabel)
      wsprintf(chBuffer, "%s", pLabel);
    else
      return; // bail out of here ... there isn't a buffer where to put this data
    
    g_hwndLabelEdit = CreateWindow(
					 "EDIT",   // Window class name
					 NULL,              // Window's title
					 WS_CHILD  |
           WS_CLIPSIBLINGS |
           WS_VISIBLE ,
					 g_pCtrlZm->rZone.left + lpmwd->iXadj,
					 g_pCtrlZm->rZone.top - lpmwd->iYOffset,
					 g_pCtrlZm->rZone.right - g_pCtrlZm->rZone.left,       // Set it to the max Width
					 g_pCtrlZm->rZone.bottom - g_pCtrlZm->rZone.top,      // Set it to the max Height
					 hwnd,        // Parent window's handle
					 NULL,              // Default to Class Menu
					 ghInstMain,         // Instance of window
					 NULL               // Ptr To Data Structure For WM_CREATE
                   );

   										
//    g_iChannelLabel = LOWORD(lpmwd->lpwRemapToScr[lpmwd->iCurChan]);
    g_editWindowProc = (WNDPROC)SetWindowLong(g_hwndLabelEdit, GWL_WNDPROC, (LONG)NewEditWindowProc);

		SendMessage(g_hwndLabelEdit, WM_SETFONT, (LPARAM)g_hConsoleFont, 0);
    SetWindowText(g_hwndLabelEdit, chBuffer);
    SetFocus(g_hwndLabelEdit);
  }
  else
  {
    ReleaseCapture();
    DestroyWindow(g_hwndLabelEdit);
    g_hwndLabelEdit = NULL;
    SetFocus(GetParent(hwnd));
  }

};





