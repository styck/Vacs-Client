//=================================================
// file trackingwindow.c
//
// Copyright 1998, CorTek Software, Inc.
//=================================================


#include "SAMM.h"
#include "SAMMEXT.h"
#include "MACRO.h"
#include "ConsoleDefinition.h"
#include "DCXTCP_protocol.h" 



char    gszTrackWindowClass_Name[] = "VACS_TrackingWindow";
HWND    g_hwndTrack = NULL;
HWND    g_hwndInitImg = NULL;
BOOL    g_bTrackingActive = FALSE;


LRESULT CALLBACK  TrackingWndProc(HWND , UINT , WPARAM , LPARAM );
void    TW_DrawInformation(LPMIXERWNDDATA      lpmwd);
void    TW_Paint(void);

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
// FUNCTION:        CreateFullViewImageWindow
//
// Creates the Wnidow that will draw the
// bitmap image and take care of all mixer
// controls .. bla - bla - bla
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
void StopTrackingWindow(void)
{
  g_bTrackingActive = FALSE;
  DestroyWindow(g_hwndTrack);
  //g_hwndTrack = NULL;

}


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
    TW_Paint();
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

  if(g_bTrackingActive)
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
    
    //GetWindowRect(g_hwndTrack, &rTrack);
    //ScreenToClient((LPPOINT)&rTrack.left);
    //ScreenToClient((LPPOINT)&rTrack.right);


    pnt = lpmwd->pntMouseCur;

    // Get the outline of the Current module into rect .... Recomended by Tom
    GetMWScrChanRect(lpmwd, lpmwd->iCurChan, &rect);
    
    
    if( (rect.right + rTrack.right) < rClient.right)
      x = rect.right;     
    else
      x = rect.left - rTrack.right;

    if((pnt.y + rTrack.bottom + 50) < rClient.bottom)
      y = pnt.y + 50;
    else
      y = pnt.y - 50;

      
    SetWindowPos(g_hwndTrack, HWND_TOP, x, y, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
    //SetWindowPos(g_hwndTrack, HWND_TOP, rect.right, pnt.y + 50, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);

    
    TW_DrawInformation(lpmwd);
  }
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

  hbr = CreateSolidBrush(RGB(64, 48, 48));
  hbrBack = CreateSolidBrush(RGB(232, 232, 232));

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

////////////////////////////////////////////////////////////////
//
//
//
//
void    TW_DrawInformation(LPMIXERWNDDATA      lpmwd)
{
  HDC       hdc;
  RECT      r;
  int       iChan;
  char      szBuff[256];
  LPCTRLZONEMAP       lpczm;
  POINT     pnt;

  if(g_hwndTrack == NULL)
    return;

  
  // Firat repaint the window
  //
  TW_Paint();


  iChan = LOBYTE(lpmwd->lpwRemapToScr[lpmwd->iCurChan + lpmwd->iStartScrChan]);
  
  pnt = lpmwd->pntMouseCur;
  
  pnt.x -= lpmwd->iXadj;
  pnt.y += lpmwd->iYOffset;

  lpczm = ScanCtrlZonesPnt(lpmwd->lpZoneMap[iChan].lpZoneMap, pnt);
  if(lpczm)
  {

    wsprintf(szBuff, "CH: %d\n%s", iChan+1, &gpLabels[iChan * MAX_LABEL_SIZE]);
//    wsprintf(szBuff, "CH: %d\n%s\n%s", iChan+1, &gpLabels[iChan * MAX_LABEL_SIZE],
//                                             CDef_GetControlName(lpczm->iCtrlNumAbs));
  }
  else
    wsprintf(szBuff, "CH: %d\n%s", iChan+1, &gpLabels[iChan * MAX_LABEL_SIZE]);


  GetClientRect(g_hwndTrack, &r);
  hdc = GetDC(g_hwndTrack);
  
  WriteTextLinesToDC(hdc, &r, 1, 1, RGB(0, 0, 0),  szBuff);

  ReleaseDC(g_hwndTrack, hdc);

}

