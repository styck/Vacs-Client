//=================================================
// Copyright 1998 - 2002, CorTek Softawre, Inc.
//=================================================
//
//
// $Author: Styck $
// $Archive: /Vacs Client/src/MasterWindow.c $
// $Revision: 5 $
//

//=================================================
// The Zoom View Window
//
//=================================================

#include "SAMM.h"
#include "SAMMEXT.h"

// Local definitions
//------------------
LRESULT CALLBACK  MasterViewProc(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam);

//====================================
// FUNCTION: RegisterZoomViewClass
//
//
// Register the Zoom View Window Class
//====================================
int RegisterMasterViewClass(void)
{
    int iReturn;
    WNDCLASS wc;

    // Register Zoom View Class
    //--------------------------
    ZeroMemory(&wc, sizeof(WNDCLASS));      // Clear wndclass structure

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

    if (iReturn == 0)
        return(IDS_ERR_REGISTER_CLASS);     // Error... Exit

    return 0;
}

///////////////////////////////////////////////////////////////
// CreateMasterViewWindow
//
//
//

HWND CreateMasterViewWindow(LPSTR szTitle, LPMIXERWNDDATA  pMWD)
{
    HWND hWnd;
    RECT rect;
    LPMIXERWNDDATA lpmwd;
    DWORD style;
    int nWidth;

    if (pMWD == NULL)
    {
        lpmwd = MixerWindowDataAlloc(gwActiveMixer,
            gpZoneMaps_Zoom,
            MAX_CHANNELS,
            DCX_DEVMAP_MODULE_MASTER);

        if (lpmwd == NULL)
        {
            ErrorBox(ghwndMain, ghInstStrRes, IDS_ERR_ALLOCATE_MEMORY);
            return NULL;
        }

        ///////////////////////////////////////////////////////////
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
    }
    else
    {
        lpmwd = pMWD;
        //lpmwd->rWndPos.bottom += HEIGHT_FULL_LABEL_WND;
    }

    //////////////////////////////////////////////////
    // get some information about where are we in the 
    // Client Window
    //
    GetWindowRect(ghwndMDIClient, &rect);
    ScreenToClient(ghwndMain, (LPPOINT)&rect.left);
    ScreenToClient(ghwndMain, (LPPOINT)&rect.right);

    lpmwd->rWndPos.right = MASTER_MODULE_WIDTH + MAX_BORDER_WIDTH;
    style = WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE;

#ifdef SCROLLBARS
    style = style | WS_VSCROLL;
    lpmwd->rWndPos.right += GetSystemMetrics(SM_CYVTHUMB);	// allow space for scroll bar
#endif

    nWidth = lpmwd->rWndPos.right;

    /////////////////////////////////////////////////
    // CREATE THE MASTER VIEW WINDOW

    hWnd = CreateWindow(
        gszMasterViewClass, // class name
        szTitle,            // title
        style, // style
        rect.right - lpmwd->rWndPos.right, // x
        rect.top,  // y
        lpmwd->rWndPos.right,	// Width of master window
        lpmwd->rWndPos.bottom,// Height of master window

        ghwndMain,           // parent
        NULL,                 // menu
        ghInstMain,           // instance
        (void*)lpmwd         // window creation data
    );

    CheckMenuItem(ghMainMenu, IDM_V_FULLZOOM_MASTER, MF_CHECKED);

    // Ok we have a window opened
    //---------------------------
    SetWindowLong(hWnd, 0, (LPARAM)lpmwd);

    /////////////////////////////////
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
    lpmwd->wWndType = WND_GROUPLBL_ZOOM;	//WND_GROUPLBL_FULL;

    if (CreateLblGroupWnd(&rect, hWnd, lpmwd) == NULL)
    {
        ErrorBox(ghwndMain, ghInstStrRes, IDS_ERR_CREATE_WINDOW);
        DestroyMasterViewWindow(hWnd);
        return NULL;//IDS_ERR_CREATE_WINDOW;
    }

    lpmwd->rVisible.bottom -= HEIGHT_FULL_LABEL_WND;

    // adjust the Image Window Size to fit
    // perfectly inside of the Client Window
    //--------------------------------------
    ImageWindowSize(hWnd, &lpmwd->rVisible, lpmwd);

    lpmwd->rVisible.right = MASTER_MODULE_WIDTH;

#ifdef SCROLLBARS
    lpmwd->rVisible.right += GetSystemMetrics(SM_CYVTHUMB);
#endif

    CreateFullViewImageWindow(hWnd, (LPARAM)lpmwd);
    ShowWindow(hWnd, SW_SHOW);
    InvalidateRect(hWnd, NULL, FALSE);
    UpdateWindow(hWnd);

#ifdef SCROLLBARS
    // Setup the Scroll bars
    SetScrollRange(hWnd, SB_VERT, 0, 3950, FALSE);             // 3467 or 3950
    SetScrollPos(hWnd, SB_VERT, 1975, TRUE);
    //		SetScrollPos(hWnd, SB_VERT, lpmwd->iYOffset, TRUE); 
#endif
    return hWnd;
}

////////////////////////////////////////////////////////////////////
// DestroyMasterViewWindow
//
//
//
void DestroyMasterViewWindow(HWND hwnd)
{
    RECT rect;
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
void DrawWindowBorder(HWND hwnd, PAINTSTRUCT* pPs)
{
    RECT rect;
    GetClientRect(hwnd, &rect);
    rect.left = rect.right - MAX_BORDER_WIDTH;
    FillRect(pPs->hdc, &rect, GetStockObject(WHITE_BRUSH));
    rect.left++;
    rect.right--;
    FillRect(pPs->hdc, &rect, GetStockObject(DKGRAY_BRUSH));
    return;
}

//===============================
//FUNCTION:MasterViewProc
//
//
//
//
//
//===============================

//#define	PAGEUPDOWN_OFFSET	765	// Number of pixels to pageup/down
//#define	LINEUPDOWN_OFFSET	5		// Number of pixels to lineup/down

#define	PAGEUPDOWN_OFFSET	rect.bottom - rect.top - HEIGHT_FULL_LABEL_WND;	//  765	// Number of pixels to pageup/down
#define	LINEUPDOWN_OFFSET	25		// Number of pixels to lineup/down

LRESULT CALLBACK  MasterViewProc(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
    MINMAXINFO FAR* lpMMI;
    LPMIXERWNDDATA lpmwd;
    PAINTSTRUCT ps;
    RECT rect;
    int nVscrollInc; // Dummy variable

    static cyClient, cxClient, nVscrollMax, nVscrollPos;

    lpmwd = (LPMIXERWNDDATA)GetWindowLong(hWnd, 0);

    switch (wMessage)
    {

#ifdef SCROLLBARS
    case WM_VSCROLL:
        /* Determine how much to scroll vertically.
        */
        GetWindowRect(hWnd, &rect);
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

            nVscrollInc = min(-1, -cyClient);

            if (lpmwd && lpmwd->iCurMode == MW_NOTHING_MODE)
            {
                //						lpmwd->pntMouseLast = lpmwd->pntMouseCur;
                //						lpmwd->pntMouseCur.y += nVscrollInc; //PAGEUPDOWN_OFFSET;	// Change the current mouse position

                ScrollImgWindow(lpmwd->hwndImg, lpmwd, nVscrollInc);		// Scroll to window to the new position
                //						lpmwd->pntMouseLast = lpmwd->pntMouseCur;	// restore original mouse coordinates
            }
            break;

        case SB_PAGEDOWN:

            nVscrollInc = max(1, cyClient);

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

#ifdef NOTUSEDYET

        if (nVscrollInc != 0)
        {
            // nVscrollPos += nVscrollInc;
            // if(nVscrollPos > nVscrollMax)
            // nVscrollPos = nVscrollMax;

            if (lpmwd && lpmwd->iCurMode == MW_NOTHING_MODE)
                SetScrollPos(hWnd, SB_VERT, lpmwd->iYOffset, TRUE);
            else
                SetScrollPos(hWnd, SB_VERT, nVscrollPos, TRUE);
        }
#endif
        break;

#endif	 // SCROLLBARS

        //////////////////////////////////////////////////////////////
    case WM_ERASEBKGND: // to reduce flashing on the screen
        break;
        //////////////////////////////////////////////////////////////
    case WM_PAINT:
        BeginPaint(hWnd, &ps);
        // FDS NOT NEEDED      DrawWindowBorder(hWnd, &ps);
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
#ifdef SCROLLBARS
        cxClient = LOWORD(lParam);
        cyClient = HIWORD(lParam);
        cyClient = cyClient - HEIGHT_FULL_LABEL_WND;

        nVscrollMax = 3950;		// max (0, 3950 - cyClient) ;
        nVscrollPos = min(nVscrollPos, nVscrollMax);

        SetScrollRange(hWnd, SB_VERT, 0, nVscrollMax, FALSE);
        if (lpmwd)
            SetScrollPos(hWnd, SB_VERT, lpmwd->iYOffset, TRUE);
        else
            SetScrollPos(hWnd, SB_VERT, nVscrollPos, TRUE);
#endif
        HandleWndSize(hWnd, lpmwd, LOWORD(lParam), HIWORD(lParam), wParam);
        break;
        //////////////////////////////////////////////////////////////
    case WM_GETMINMAXINFO:
        lpMMI = (MINMAXINFO FAR*)lParam;
        // get the information for this window
        //------------------------------------
        if (lpmwd)
        {
            lpMMI->ptMinTrackSize.x = lpmwd->rMaxWndPos.left;
            lpMMI->ptMinTrackSize.y = lpmwd->rMaxWndPos.top;
            lpMMI->ptMaxTrackSize.x = lpmwd->rMaxWndPos.right;
            lpMMI->ptMaxTrackSize.y = lpmwd->rMaxWndPos.bottom;
        }
        break;
        //////////////////////////////////////////////////////////////
    case WM_MOVE:
        if (lpmwd)
        {
            GetWindowRect(hWnd, &rect);
            ScreenToClient(ghwndMDIClient, (LPPOINT)&rect.left);
            lpmwd->rWndPos.left = rect.left;
            lpmwd->rWndPos.top = rect.top;
        }
        break;

        //////////////////////////////////////////////////////////////
    case WM_DESTROY:
        if (ghwndZoom == hWnd)
            ghwndZoom = NULL;
        //      FALL TRHU

    default:
        return DefWindowProc(hWnd, wMessage, wParam, lParam);

    }
    return 0;
}
