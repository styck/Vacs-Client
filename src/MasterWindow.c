//=================================================
// Copyright 1998, CorTek Software, Inc.
//=================================================

//=================================================
// The Zoom View Window
//
//=================================================

//#include <windows.h>

#include "SAMM.h"
#include "SAMMEXT.h"
#include "MACRO.h"




// Local definitions
//------------------
LRESULT CALLBACK  MasterViewProc(HWND hWnd, UINT wMessage, 
                                 WPARAM wParam, LPARAM lParam);

//====================================
// FUNCTION: RegisterZoomViewClass
//
//
// Register the Zoom View Window Class
//====================================
int       RegisterMasterViewClass(void)
{
int         iReturn;
WNDCLASS    wc;


// Register Zoom View Class
//--------------------------
ZeroMemory(&wc,  sizeof(WNDCLASS));      // Clear wndclass structure

wc.style = CS_HREDRAW | CS_VREDRAW;
wc.lpfnWndProc = (WNDPROC)MasterViewProc;
wc.cbClsExtra = 0;
wc.cbWndExtra = sizeof(LPSTR);// all of the data for this window will be stored here
wc.hInstance = ghInstMain;
wc.hIcon = LoadIcon(ghInstConsoleDef, MAKEINTRESOURCE(ZoomViewIcon));// this might leak memory
wc.hCursor = LoadCursor(NULL, IDC_ARROW);
wc.hbrBackground = GetStockObject(BLACK_BRUSH);
wc.lpszMenuName = NULL;
wc.lpszClassName = gszMasterViewClass;

iReturn = RegisterClass(&wc);

if(iReturn == 0)
	 return(IDS_ERR_REGISTER_CLASS);     // Error... Exit

return 0;
}

///////////////////////////////////////////////////////////////
// CreateMasterViewWindow
//
//
HWND  CreateMasterViewWindow(LPSTR szTitle, LPMIXERWNDDATA  pMWD)
{
  HWND                hWnd;
  RECT                rect;
  LPMIXERWNDDATA      lpmwd;

  if(pMWD == NULL)
    {
    lpmwd = MixerWindowDataAlloc(gwActiveMixer,
                                 gpZoneMaps_Zoom,
                                 MAX_CHANNELS,
                                 5);

    if(lpmwd == NULL)
        {
        ErrorBox(ghwndMain, ghInstStrRes, IDS_ERR_ALLOCATE_MEMORY);
        return NULL;
        }

    // here is the trick of displaying only the master channel
    // Set the Master channel to be the first visible module
    // and then limit the size of the window to a single module.
    // NOTE: Make sure we've got the correct index of a master
    // module when remapping to set it 0.
    //

    // set this so only one zone map will be updated
    // since only the Master channel will shown anyway
    //
    lpmwd->lZMCount = 1;

    // store the Window title
    //-----------------------
    lstrcpy(lpmwd->szTitle, szTitle);
    // make some room for the label Window
    // after all we want this window to appear
    // in it's full size
    //----------------------------------------
    //lpmwd->rWndPos.bottom += HEIGHT_FULL_LABEL_WND;
    }
  else
    {
    lpmwd = pMWD;
    //lpmwd->rWndPos.bottom += HEIGHT_FULL_LABEL_WND;
    }

  // give some room for the label stuff
  //------------------------------------------------
  //lpmwd->rMaxWndPos.bottom += HEIGHT_FULL_LABEL_WND;
  //lpmwd->rWndPos.bottom += HEIGHT_FULL_LABEL_WND;


  // get some information about where are we in the 
  // Client Window
  //
  GetWindowRect(ghwndMDIClient, &rect);
  ScreenToClient(ghwndMain, (LPPOINT)&rect.left);
  ScreenToClient(ghwndMain, (LPPOINT)&rect.right);


  lpmwd->rWndPos.right = MASTER_MODULE_WIDTH + MAX_BORDER_WIDTH;

  hWnd = CreateWindow (
                        gszMasterViewClass, // class name
                        szTitle,            // title
                        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, // style
                        rect.right - lpmwd->rWndPos.right, // x
                        //rect.left, // x
                        rect.top,  // y
                        lpmwd->rWndPos.right,// cX
                        lpmwd->rWndPos.bottom,// cY

                        ghwndMain ,           // parent
                        NULL,                 // menu
                        ghInstMain,           // instance
                        (void *)lpmwd         // window creation data
                        );


  CheckMenuItem(ghMainMenu, IDM_V_FULLZOOM_MASTER, MF_CHECKED);

  // Ok we have a window opened
  //---------------------------
  SetWindowLong(hWnd, 0, (LPARAM)lpmwd);

  // adjust the MDI Client Window
  // so we can see this window
  //-----------------------------
  ghwndMaster = hWnd;
  SizeClientWindow(ghwndMain, 0, 0, 0);
  

  // Create the Label-Group display Window
  //--------------------------------------
  rect.left = rect.top = 0;
  rect.right = lpmwd->rMaxSize.right;
  rect.bottom = HEIGHT_FULL_LABEL_WND;
  lpmwd->wWndType = WND_GROUPLBL_ZOOM;//WND_GROUPLBL_FULL;

  if(CreateLblGroupWnd(&rect, hWnd, lpmwd) == NULL)
    {
    ErrorBox(ghwndMain, ghInstStrRes,IDS_ERR_CREATE_WINDOW);
    DestroyMasterViewWindow(hWnd);
    return NULL;//IDS_ERR_CREATE_WINDOW;
    }


  lpmwd->rVisible.bottom -= HEIGHT_FULL_LABEL_WND;

  // adjust the Image Window Size to fit
  // perfectly inside of the Client Window
  //--------------------------------------
  ImageWindowSize(hWnd, &lpmwd->rVisible, lpmwd);

  lpmwd->rVisible.right = MASTER_MODULE_WIDTH;

  CreateFullViewImageWindow(hWnd, (LPARAM)lpmwd);
  
  ShowWindow(hWnd, SW_SHOW);
  InvalidateRect(hWnd, NULL, FALSE);
  UpdateWindow(hWnd);
  return hWnd;
}

////////////////////////////////////////////////////////////////////
// DestroyMasterViewWindow
//
//
//
void    DestroyMasterViewWindow(HWND   hwnd)
{
  RECT    rect;

  GetWindowRect(ghwndMDIClient, &rect);
  ScreenToClient(ghwndMain, (LPPOINT)&rect.left);
  ScreenToClient(ghwndMain, (LPPOINT)&rect.right);

  DestroyWindow(hwnd);

  ghwndMaster = NULL;

  // adjust the MDI Client Window
  // to it's full size
  //-----------------------------
  SizeClientWindow(ghwndMain, 0, 0, 0);

  CheckMenuItem(ghMainMenu, IDM_V_FULLZOOM_MASTER, MF_UNCHECKED);

  return;
}


//////////////////////////////////////////////
//  DrawMasterWindowBorder
//
//
//
void  DrawWindowBorder(HWND hwnd, PAINTSTRUCT *pPs)
{
  RECT        rect;

  GetClientRect(hwnd, &rect);


  rect.left = rect.right - MAX_BORDER_WIDTH;

  FillRect(pPs->hdc, &rect, GetStockObject(WHITE_BRUSH));

  rect.left ++;
  rect.right --;

  FillRect(pPs->hdc, &rect, GetStockObject(DKGRAY_BRUSH));

  return;
}


//===============================
//FUNCTION:MasterViewProc
//
//===============================
LRESULT CALLBACK  MasterViewProc(HWND hWnd, UINT wMessage, 
                                 WPARAM wParam, LPARAM lParam)
{
MINMAXINFO FAR      *lpMMI;
LPMIXERWNDDATA      lpmwd;
PAINTSTRUCT         ps;
RECT                rect;

lpmwd = (LPMIXERWNDDATA)GetWindowLong(hWnd,0);

switch (wMessage)
	{
    //////////////////////////////////////////////////////////////
    case WM_ERASEBKGND: // to reduce flashing on the screen
        break;
    //////////////////////////////////////////////////////////////
    case WM_PAINT:
      BeginPaint(hWnd, &ps);
      DrawWindowBorder(hWnd, &ps);
      EndPaint(hWnd, &ps);
      break;
    //////////////////////////////////////////////////////////////
    case WM_COMMAND:
	    switch (LOWORD(wParam))
		    {
            default:
                return DefWindowProc(hWnd, wMessage, wParam, lParam);
			  }
        break;
    //////////////////////////////////////////////////////////////
    case WM_SIZE:
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

    //////////////////////////////////////////////////////////////
    case WM_DESTROY:
        if(ghwndZoom == hWnd)
            ghwndZoom = NULL;
            
//      break;
    default:
        return DefWindowProc(hWnd, wMessage, wParam, lParam);

	}

return 0;
}
