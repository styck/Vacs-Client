//=================================================
// Copyright 1998, CorTek Software, Inc.
//=================================================

//#include <windows.h>
//#include <commctrl.h>
//#include <string.h>

#include "SAMM.H"
#include "SAMMEXT.H"
#include "MACRO.h"


#define SEQ_COLUMNS     4
#define SEQ_NUMBITMAPS  3
#define DRAG_TIMER_ID   10
#define DRAG_DELAY_TIME 300

int         g_nOpen;
int         g_nClosed;
int         g_nDocument;
BOOL        g_bDragging = FALSE; 
HTREEITEM   g_htiTargetOld = NULL;
HIMAGELIST  g_himlDrag = NULL;
HWND        g_hwndTV;
HDC         g_dcTV;
RECT        g_rDrag;
RECT        g_rDragItem;
int         g_iDragScrlDir;
int         g_iTimerID=0;
HWND        g_hwndMTCReadout;
HBITMAP     g_hbmpMTC;
RECT        g_rMTCReadout;
MMRESULT    g_TimeEvent = 0;
int         g_iStopTimeEvent = 0;
LPDLROOTPTR g_pdlrSequence = NULL;
SEQ_PROPAGATE g_SeqPropagate = {0};
char				g_sequence_file_name[MAX_PATH];

void    CALLBACK MTC_EmulateProc(UINT , UINT , DWORD , DWORD , DWORD);
void    StopTimeEvent(void);
void    SeqInsertEntryDL(void);
HTREEITEM SeqAddItem(long, long,HTREEITEM);
void    SeqDeleteItem(void);
void    DisplayTVNPopupMenu(HWND);
HTREEITEM AddItemToTree(HWND hwndTV, LPSTR lpszItem, int nLevel) ;
BOOL    RecallEntry(void);
BOOL    UpdateEntry(void);
BOOL    SeqGoToNext(void);
BOOL    SeqGoToPrev(void);
BOOL    SeqGoToFirst(void);
BOOL    SeqGoToLast(void);
HTREEITEM g_current_item_before_next = NULL;

void  SetSeqUpdateAllProps(HWND hwnd, BOOL bSet);
void  DisableSeqUpdateAllProps(HWND hwnd, BOOL bDisable);
void	CloseSequenceFiles (void);
BOOL	OpenSequenceFiles (void);

//=================================================
//function: RegisterSeqWindowClass
//
//
//=================================================
int RegisterSeqWindowClass(void)
{
int         iReturn;
WNDCLASS    wc;


// Register Full View Class
//--------------------------
ZeroMemory(&wc, sizeof(WNDCLASS));      // Clear wndclass structure

wc.style = CS_HREDRAW | CS_VREDRAW;
wc.lpfnWndProc = (WNDPROC)SeqWndProc;
wc.cbClsExtra = 0;
wc.cbWndExtra = sizeof(LPSTR);// all of the data for this window will be stored here
wc.hInstance = ghInstMain;                        
wc.hIcon = LoadIcon(ghInstConsoleDef, MAKEINTRESOURCE(IDICON_SEQUENCE));// this might leak memory
wc.hCursor = LoadCursor(NULL, IDC_ARROW);
//wc.hbrBackground = GetStockObject(BLACK_BRUSH);
wc.lpszMenuName = NULL;
wc.lpszClassName = gszSeqClass;

iReturn = RegisterClass(&wc);

if(iReturn == 0)
	 return(IDS_ERR_REGISTER_CLASS);     // Error... Exit


return 0;
}

//=================================================
//function: ShowSeqWindow
//
//
//=================================================
static BOOL	bSeqWindowOutofScreen = TRUE;
int     ShowSeqWindow(BOOL bShow)
{
char                    szTitle[128];
LPSEQUENCEWINDOWFILE    lpSeqWF;
RECT										r;
	DWORD										style;

if(ghwndSeq == NULL)
    {
    lpSeqWF = GlobalAlloc(GPTR, sizeof(SEQUENCEWINDOWFILE));
    if(lpSeqWF == NULL){
        ErrorBox(ghwndMain, ghInstStrRes, IDS_ERR_CREATE_WINDOW);
        return 1;
        }
    lpSeqWF->iWndID = SEQUENCE_WINDOW_FILE_ID;        


		style = MDIS_ALLCHILDSTYLES | WS_CHILD | WS_SYSMENU | WS_CAPTION | WS_VISIBLE 
																| WS_THICKFRAME | WS_MINIMIZEBOX// | WS_MAXIMIZEBOX
																| WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		style &= ~WS_MAXIMIZEBOX;

    LoadString(ghInstStrRes, IDS_SEQUENCE_WINDOW_TITLE, szTitle, 128);
    ghwndSeq = CreateMDIWindow (
                            gszSeqClass,
                            szTitle,
														style,
                            10000,//CW_USEDEFAULT,
                            10000,//CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            ghwndMDIClient,
                            ghInstMain,
                            (LPARAM)NULL
                            );			 

    if(ghwndSeq == NULL){
        GlobalFree(lpSeqWF);
        ErrorBox(ghwndMain, ghInstStrRes, IDS_ERR_CREATE_WINDOW);
        return 1;
        }
    SetWindowLong(ghwndSeq, 0, (LONG)lpSeqWF);
    // Show something in the MTC/SMPTE readout
    //----------------------------------------
    DisplayMTCReadout();
    }

  if(bShow)
  {
    if(ghwndSeq)
		{
			GetWindowRect(ghwndSeq, &r);
			if(r.left > 5000)
				SetWindowPos(ghwndSeq, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
			else
				SetWindowPos(ghwndSeq, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
		}
  }
  else
  {
    if(ghwndSeq)
      ShowWindow(ghwndSeq, SW_HIDE);
  }

return 0;
}

//================================================
//function: SeqProc
//
//main window procedure 
// for the Sequence View
//================================================
BOOL CALLBACK   SeqProc(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
HWND            hwndCtrl;
RECT            rect, rectCtrl;
//char            szBuff[MAX_PATH];

switch(uiMsg)
    {
    /////////////////////////////////////////////////////////////
	case WM_NOTIFY:	{
		//SendMessage(ghwndMDIClient, WM_MDIACTIVATE, (WPARAM)GetParent(hwnd), 0);
        switch (((LPNMHDR) lParam)->code)
            {
            //----------------
            case TVN_SETDISPINFO:
                HandleTVSetItemIfno((TV_DISPINFO *)lParam);
                break;
            //----------------
            case TVN_GETDISPINFO:
                HandleTVGetDispInfo((TV_DISPINFO *)lParam);
                break;
            //----------------
            case TVN_ENDLABELEDIT:
                HandleTVEndLabelEdit((TV_DISPINFO *)lParam);
                break;
            //----------------
            case TVN_ITEMEXPANDED:
                HandleSeqTVNExpand(((LPNM_TREEVIEW)lParam));
                break;
            //----------------
            case TVN_BEGINDRAG:
                HandleSeqTVNBeginDrag((LPNM_TREEVIEW)lParam);
                break;
            //----------------
            case NM_RCLICK:
                if(((LPNMHDR) lParam)->idFrom == IDTREE_SEQUENCE)
                    DisplayTVNPopupMenu(hwnd);
                break;
            //----------------
      case NM_SETFOCUS:
          SendMessage(ghwndMDIClient, WM_MDIACTIVATE, (WPARAM)GetParent(hwnd), 0);
          break;
      }
            }
        break;

    /////////////////////////////////////////////////////////////
    case WM_MOUSEMOVE:
        HandleSeqTVN_OnMouseMove(hwnd, LOWORD(lParam), HIWORD(lParam));
        break;

    /////////////////////////////////////////////////////////////
    case WM_LBUTTONUP:
        HandleSeqTVNStopDrag();

        break;

    /////////////////////////////////////////////////////////////
    case WM_TIMER:
        HandleDragTimer();

        break;
    /////////////////////////////////////////////////////
    case WM_COMMAND:
        //  activate the Parent MDI Child Window
        SendMessage(ghwndMDIClient, WM_MDIACTIVATE, (WPARAM)GetParent(hwnd), 0);

        switch(LOWORD(wParam))
            {
            //----------------
            /*
            case IDBTN_SEQ_PLAY:
                if(HIWORD(wParam) == BN_CLICKED)
                    {
                    if(g_TimeEvent == 0)
                        {
                        g_iStopTimeEvent = 0;
                        g_TimeEvent = timeSetEvent(30, 30, MTC_EmulateProc, (DWORD)g_hwndMTCReadout, TIME_PERIODIC);
                        }
                    else
                        g_iStopTimeEvent = 1;
                    }
                break;
             */
            //----------------
            case MENU_TVN_ADD:
            case IDBTN_SEQ_ADD:
                SeqInsertEntryDL();
                break;
            //-------------------
            case MENU_TVN_DELELTE:
            case IDBTN_SEQ_DELETE:
			if(ConfirmationBox(ghwndMDIClient, ghInstStrRes, IDS_DELETE_SEQUENCE_ENTRY) == IDYES)
                SeqDeleteItem();
                break;

            //-------------------
            case MENU_TVN_UPDATE:
            case IDBTN_SEQ_UPDATE:
              UpdateEntry();
              break;

            //-------------------
            case IDBTN_SEQ_PLAY:
              if(HIWORD(wParam) == BN_CLICKED)
                  {
                  if(g_TimeEvent == 0)
                      {
                      g_iStopTimeEvent = 0;
                      g_TimeEvent = timeSetEvent(30, 30, MTC_EmulateProc, (DWORD)g_hwndMTCReadout, TIME_PERIODIC);
                      }
                  else
                      g_iStopTimeEvent = 1;
                  }
            //~~~~~~~~~~~~~
            case ID_RECALL:
              RecallEntry();
              break;

            case IDBTN_SEQ_NEXT:
              SeqGoToNext();
              break;
            case IDBTN_SEQ_PREV:
              SeqGoToPrev();
              break;
            case IDBTN_SEQ_BOTTOM:
              SeqGoToLast();
              break;
            case IDBTN_SEQ_TOP:
              SeqGoToFirst();
              break;
            }


        break;
    //////////////////////////////////////////////////////////////
    case WM_SIZE:
        if((wParam != SIZE_MINIMIZED) && ghwndSeqDlg)
            {
            GetClientRect(hwnd, &rect);
            // adjust the size of the 
            // tree and the list view
            //-----------------------
            hwndCtrl = GetDlgItem(hwnd, IDTREE_SEQUENCE);
            if(hwndCtrl)
                {
                GetWindowRect(hwndCtrl, &rectCtrl);
                ScreenToClient(hwnd, (LPPOINT)&rectCtrl.left);
                ScreenToClient(hwnd, (LPPOINT)&rectCtrl.right);
                SetWindowPos(hwndCtrl, NULL, 0, 0, rectCtrl.right - rectCtrl.left, 
                                                   rect.bottom - rectCtrl.top,
                             SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
                }

            hwndCtrl = GetDlgItem(hwnd, IDLIST_SEQ_EVENTS);
            if(hwndCtrl)
                {
                GetWindowRect(hwndCtrl, &rectCtrl);
                ScreenToClient(hwnd, (LPPOINT)&rectCtrl.left);
                SetWindowPos(hwndCtrl, NULL, 0, 0, rect.right - rectCtrl.left, 
                                                   rect.bottom - rectCtrl.top,
                             SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
                }
            }

        break;
    
    /////////////////////////////////////////////////////
    case WM_INITDIALOG:
			ghwndSeqDlg = hwnd;

#ifdef NOTUSED
      // Prepare the Critical Section
      // for the MTC Emulate
      //-----------------------------
      wsprintf(szBuff, "%s.ctek", g_sequence_file_name);
      //wsprintf(szBuff, "%s\\data\\sequence.ctek",gszProgDir);      
      g_pdlrSequence = InitDoubleLinkedList(sizeof(SEQENTRY), 32, TRUE, TRUE, NULL, szBuff);
      
      if(g_pdlrSequence == NULL)
      {
        DestroyWindow(ghwndSeqDlg);
        return FALSE;
      }


      //wsprintf(szBuff, "%s\\data\\vacs_device.data",gszProgDir);      
      wsprintf(szBuff, "%s.data",g_sequence_file_name);
      if(OpenDataFile(szBuff))
      {


      }
      // init the Tree and the List View
      //--------------------------------
      hwndCtrl = GetDlgItem(hwnd, IDTREE_SEQUENCE);
      if(hwndCtrl)
          FillSeqTree(hwndCtrl);

      hwndCtrl = GetDlgItem(hwnd, IDLIST_SEQ_EVENTS);
      if(hwndCtrl)
          InitSeqList(hwndCtrl);

      if(OpenSequenceFiles () == FALSE){
				CloseSequenceFiles ();
        DestroyWindow(ghwndSeqDlg);
        return FALSE;
      }
#endif
	     
      g_hwndMTCReadout = GetDlgItem(hwnd, IDC_MTC_READOUT);
      if(g_hwndMTCReadout) {
          GetClientRect(g_hwndMTCReadout, &g_rMTCReadout);
          PrepareMTCReadout();
          }

//      return FALSE;        
      break;
    ////////////////////////////////////////////////////
    case WM_DESTROY:
        if(g_TimeEvent)
            g_iStopTimeEvent = 1;

				CloseSequenceFiles ();
/*
        // Free the Linked list
        //---------------------
        FreeDLListRootAll(&g_pdlrSequence);
        g_pdlrSequence = NULL;
        CloseDataFile();
*/

        ghwndSeqDlg = NULL;
        FreeMTCReadout();
    default:
        return FALSE;
    }

return TRUE;
}

//=======================================================
//function: SeqWndProc
//
//the MDI child window for the Dialog Box
//
//=======================================================
LRESULT CALLBACK  SeqWndProc(HWND hwnd, UINT uiMsg, 
                               WPARAM wParam, LPARAM lParam)
{
RECT        rect;
MINMAXINFO  *lpMMI;

switch(uiMsg)
    {
    ////////////////////////////////////////////
//    case WM_MDIACTIVATE:
//        if((HWND)lParam == hwnd)
//            SetActiveWindow(ghwndSeqDlg);
            //SetFocus(ghwndSeqDlg);
//        break;
    ////////////////////////////////////////////
    case WM_CREATE:
				ShowWindow(hwnd, SW_HIDE);

        ghwndSeqDlg = CreateDialog(ghInstStrRes, MAKEINTRESOURCE(IDD_SEQUENCE_WINDOW), hwnd, SeqProc);
        if(ghwndSeqDlg == NULL){
            return -1;
            }
        
				GetClientRect(ghwndSeqDlg, &rect);        
        SetWindowPos(hwnd, NULL, 0, 0, 
                        rect.right + GetSystemMetrics(SM_CYDLGFRAME)*4, 
                        rect.bottom + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYDLGFRAME)*4, 
                        SWP_NOMOVE | SWP_NOZORDER | SWP_HIDEWINDOW | SWP_NOACTIVATE);
				
			
        //ShowWindow(ghwndSeqDlg, SW_SHOW);

        return DefMDIChildProc(hwnd, uiMsg, wParam, lParam);
        break;

		case WM_SHOWWINDOW:
			if((BOOL)wParam)
			{
				//ShowSeqWindow(TRUE);
				GetWindowRect(ghwndSeqDlg, &rect);
				if(rect.left > 5000)
					SetWindowPos(ghwndSeq, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
				ShowWindow(ghwndSeqDlg, SW_SHOW);
			}

			break;
    //////////////////////////////////////////////////////////////
    case WM_SIZE:
        if(wParam != SIZE_MINIMIZED)
            SetWindowPos(ghwndSeqDlg, NULL, 0, 0, LOWORD(lParam), HIWORD(lParam),
                         SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

        break;

    //////////////////////////////////////////////////////////////
    case WM_NCACTIVATE:
        if((BOOL)wParam == FALSE)
            HandleSeqTVNStopDrag();
        return DefMDIChildProc(hwnd, uiMsg, wParam, lParam);
        break;

    //////////////////////////////////////////////////////////////
    case WM_GETMINMAXINFO: // Set the min/max size of the Window
        lpMMI = (MINMAXINFO *)lParam;
        lpMMI->ptMinTrackSize.x = GetDlgWindowMinX(ghwndSeqDlg, IDTREE_SEQUENCE);
        lpMMI->ptMinTrackSize.y = GetDlgWindowMinY(ghwndSeqDlg, IDBTN_SEQ_BOTTOM);
        break;

    //////////////////////////////////////////////////////////////
   case WM_ERASEBKGND: // to reduce flashing on the screen
        break;


    case WM_CLOSE:
			ShowSeqWindow(FALSE);
        //SendMessage(ghwndMDIClient, WM_MDIDESTROY, (WPARAM)hwnd, 0L);
        break;

    case WM_DESTROY:
        GlobalFree((HGLOBAL)GetWindowLong(hwnd, 0));
        ghwndSeq = NULL;

    default:
        return DefMDIChildProc(hwnd, uiMsg, wParam, lParam);

	}

return 0;
}

//====================================
//function: GetDlgWindowMaxX
//
// param:
//      HWND    hwnd; Dialog handle
//      int     iIDC; the control ID
// note:
//  if the Control ID == -1
// then the size of the Entire Window
// is used
// works with Dialog Boxes(Windows)
//====================================
long    GetDlgWindowMaxX(HWND hwnd, int iIDC)
{



return 0;
}

//====================================
//function: GetDlgWindowMaxY
//
// param:
//      HWND    hwnd; Dialog handle
//      int     iIDC; the control ID
// note:
//  if the Control ID == -1
// then the size of the Entire Window
// is used
// works with Dialog Boxes(Windows)
//====================================
long    GetDlgWindowMaxY(HWND hwnd, int iIDC)
{



return 0;
}

//====================================
//function: GetDlgWindowMinX
//
// param:
//      HWND    hwnd; Dialog handle
//      int     iIDC; the control ID
// note:
//  if the Control ID == -1
// then the size of the Entire Window
// is used
// works with Dialog Boxes(Windows)
//====================================
long    GetDlgWindowMinX(HWND hwnd, int iIDC)
{
RECT    rect;
HWND    hwndCtrl;

if(hwnd == NULL)
  return 0;

hwndCtrl = NULL;
if(iIDC > 0)
    hwndCtrl = GetDlgItem(hwnd, iIDC);

if(hwndCtrl == NULL)
    GetWindowRect(hwnd, &rect);
else
    GetWindowRect(hwndCtrl, &rect);

ScreenToClient(hwnd, (LPPOINT)&rect.right);

rect.right += GetSystemMetrics(SM_CXDLGFRAME)*3;
return rect.right;
}
//====================================
//function: GetDlgWindowMinY
//
// param:
//      HWND    hwnd; Dialog handle
//      int     iIDC; the control ID
// note:
//  if the Control ID == -1
// then the size of the Entire Window
// is used
// works with Dialog Boxes(Windows)
//====================================
long    GetDlgWindowMinY(HWND hwnd, int iIDC)
{

RECT    rect;
HWND    hwndCtrl;

if(hwnd == NULL)
  return 0;

hwndCtrl = NULL;
if(iIDC > 0)
    hwndCtrl = GetDlgItem(hwnd, iIDC);

if(hwndCtrl == NULL)
    GetWindowRect(hwnd, &rect);
else
    GetWindowRect(hwndCtrl, &rect);

ScreenToClient(hwnd, (LPPOINT)&rect.right);

rect.bottom += GetSystemMetrics(SM_CYDLGFRAME)*3 + 
               GetSystemMetrics(SM_CYCAPTION);
return rect.bottom;
}

//===================================
//FUNCTION: FillSeqTree
//
//
//
//===================================
int FillSeqTree(HWND hwnd)
{
LPDLROOTPTR     pdlrootPtr;
long            lItemPos, lLinkState;
LPSEQENTRY      p_seqentry;
HTREEITEM       htreItem = NULL;

g_hwndTV = hwnd;

if(InitTreeViewImageLists(hwnd) == FALSE)
    return 0;

TreeView_DeleteAllItems (hwnd);

lItemPos = GetFirstEntry(g_pdlrSequence);
if(lItemPos > 0)
    {
    // Go through the Items and
    // insert them into the Tree View
    // lock the root and use the
    // locked root pointer to 
    // access the dllist functions
    //-------------------------------
    pdlrootPtr = LockDLListRoot(g_pdlrSequence);
    if(pdlrootPtr == NULL)
        return 0;
    
    while(lItemPos > 0)
        {
        lLinkState = GetEntryLinkState(pdlrootPtr, lItemPos);
        p_seqentry = (LPSEQENTRY)GetEntryData(pdlrootPtr, lItemPos);
        if(p_seqentry == NULL)
            break;

        htreItem = SeqAddItem(lItemPos, lLinkState, htreItem);
//        AddItemToTree(hwnd, p_seqentry->szName, lLinkState);
        lItemPos = GetNextEntry(pdlrootPtr, lItemPos);
        }

    // unlock the root and free the memory for the
    // locked copy of the root
    //--------------------------------------------
    UnlockDLListRoot(g_pdlrSequence, pdlrootPtr);
    }


return 0;
}


/////////////////////////////////////////////////////////////////////
//
//
//
BOOL  SeqGoToNext(void)
{
  BOOL                bRet = FALSE;
  HTREEITEM           htreeitem;

  htreeitem = TreeView_GetSelection(g_hwndTV);
	g_current_item_before_next = htreeitem;
  htreeitem = TreeView_GetNextItem(g_hwndTV, htreeitem, TVGN_NEXT);

  if(htreeitem != NULL)
  {
    TreeView_SelectItem(g_hwndTV, htreeitem);
    UpdateWindow(g_hwndTV);
    RecallEntry();
    bRet = TRUE;
  }

  return bRet;
}

/////////////////////////////////////////////////////////////////////
//
//
//
BOOL  SeqGoToPrev(void)
{
  BOOL                bRet = FALSE;
  HTREEITEM           htreeitem;

  htreeitem = TreeView_GetSelection(g_hwndTV);
  htreeitem = TreeView_GetNextItem(g_hwndTV, htreeitem, TVGN_PREVIOUS);

  if(htreeitem != NULL)
  {
    TreeView_SelectItem(g_hwndTV, htreeitem);
    UpdateWindow(g_hwndTV);
    RecallEntry();
    bRet = TRUE;
  }

  return bRet;
}

/////////////////////////////////////////////////////////////////////
//
//
//
BOOL  SeqGoToFirst(void)
{
  BOOL                bRet = FALSE;
  HTREEITEM           htreeitem;

  htreeitem = TreeView_GetRoot(g_hwndTV);

  if(htreeitem != NULL)
  {
    TreeView_SelectItem(g_hwndTV, htreeitem);
    UpdateWindow(g_hwndTV);
    RecallEntry();
    bRet = TRUE;
  }

  return bRet;
}
/////////////////////////////////////////////////////////////////////
//
//
//
BOOL  SeqGoToLast(void)
{
  BOOL                bRet = FALSE;
  HTREEITEM           htreeitem = NULL;
  HTREEITEM           htreeitemNext = NULL;


  
  htreeitemNext = TreeView_GetRoot(g_hwndTV);

  while(htreeitemNext)
  {
    htreeitem = htreeitemNext;
    htreeitemNext = TreeView_GetNextItem(g_hwndTV, htreeitem, TVGN_NEXT);
  }

  if(htreeitem != NULL)
  {
    TreeView_SelectItem(g_hwndTV, htreeitem);
    UpdateWindow(g_hwndTV);
    RecallEntry();
    bRet = TRUE;
  }

  return bRet;
}


/////////////////////////////////////////////////////////////////////
//
//
BOOL CALLBACK   SeqUpdatePropProc(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  //HWND            hwndCtrl;
  //RECT            rect, rectCtrl;
  //char            szBuff[MAX_PATH];

  switch(uiMsg)
  {
  case WM_COMMAND:
    switch(LOWORD(wParam))
    {
    case IDC_PROP_ALL:
      DisableSeqUpdateAllProps(hwnd, TRUE);
      SetSeqUpdateAllProps(hwnd, TRUE);
      break;
    case IDC_PROP_CUSTOM:
      DisableSeqUpdateAllProps(hwnd, FALSE);
      break;
    case IDC_DONOT_PROP:
      DisableSeqUpdateAllProps(hwnd, TRUE);
      SetSeqUpdateAllProps(hwnd, FALSE);
      break;
    case IDOK:
      g_SeqPropagate.bUseCurrent = IsDlgButtonChecked(hwnd, IDC_USE_AUTO);
			if(IsDlgButtonChecked(hwnd, IDC_PROP_ALL))
				g_SeqPropagate.iPropWhat = PROP_ALL;
			else
				g_SeqPropagate.iPropWhat = PROP_NONE;
      EndDialog(hwnd, 1);
      break;
    case IDCANCEL:
      EndDialog(hwnd, 0);
      break;
    };
    break;

  /////////////////////////////////////////////////////
  case WM_INITDIALOG:
    DisableSeqUpdateAllProps(hwnd, TRUE);
    SetSeqUpdateAllProps(hwnd, TRUE);
    CheckDlgButton(hwnd, IDC_PROP_ALL, BST_CHECKED); 


    return FALSE;        
    break;
  ////////////////////////////////////////////////////
  case WM_DESTROY:
  default:
      return FALSE;
  }

  return TRUE;
}

/////////////////////////////////////////////////////////////////////
//
//
void  SetSeqUpdateAllProps(HWND hwnd, BOOL bSet)
{
  int id = IDC_INP_VOLUME;
  for(id = IDC_INP_VOLUME; id < IDC_INP_COMP + 1; id++)
  {
    if(bSet)
      CheckDlgButton(hwnd, id, BST_CHECKED); 
    else
      CheckDlgButton(hwnd, id, BST_UNCHECKED); 
  }
  return;
};


void  DisableSeqUpdateAllProps(HWND hwnd, BOOL bDisable)
{
  int     id = IDC_INP_VOLUME;
  DWORD   dwS;
  HWND    hw;

  for(id = IDC_INP_VOLUME; id < IDC_INP_COMP + 1; id++)
  {
    hw = GetDlgItem(hwnd, id); 
    dwS = GetWindowLong(hw, GWL_STYLE);
    if(bDisable)
      dwS |= WS_DISABLED;
    else
      dwS &= ~WS_DISABLED;
    SetWindowLong(hw, GWL_STYLE, dwS);
    InvalidateRect(hw, NULL, FALSE);
    UpdateWindow(hw);
  }
 
  return;
};


/////////////////////////////////////////////////////////////////////
//
//
void  GetSeqUpdateProps(SEQENTRY  *pSeqentry)
{
  if(g_SeqPropagate.bUseCurrent == FALSE || pSeqentry == NULL)
    DialogBox(ghInstStrRes, MAKEINTRESOURCE(IDD_SEQ_UPDATE_PROP), 
              ghwndMain, SeqUpdatePropProc);
};


/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: RecallEntry
//
//
//

BOOL    RecallEntry(void)
{
  BOOL                bRet = FALSE;
  HTREEITEM           htreeitem;
  TV_ITEM             tvi;
  long                lItemCur;
  SEQENTRY            *pSeqentry = NULL;


  htreeitem = TreeView_GetSelection(g_hwndTV);
  if(htreeitem != NULL)
  {
    tvi.mask = TVIF_PARAM;
    tvi.hItem = htreeitem;
    TreeView_GetItem(g_hwndTV, &tvi);
    lItemCur = (long)tvi.lParam;
    pSeqentry = GetEntryData(g_pdlrSequence, lItemCur);

    if(pSeqentry)
    {
      if(ReadDataFile(pSeqentry->dwOffset))
      {
				ShowTBSeqName(pSeqentry->szName);
        RecallMemoryMapBuffer(FALSE);
      }
    }

  }
  return bRet;
}


/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: UpdateEntry
//
//
//
BOOL    UpdateEntry(void)
{
  BOOL                bRet = FALSE;
  HTREEITEM           htreeitem;
  TV_ITEM             tvi;
  long                lItemCur;
  SEQENTRY            *pSeqentry = NULL;


  htreeitem = TreeView_GetSelection(g_hwndTV);
  if(htreeitem != NULL)
  {
    tvi.mask = TVIF_PARAM;
    tvi.hItem = htreeitem;
    TreeView_GetItem(g_hwndTV, &tvi);
    lItemCur = (long)tvi.lParam;
    pSeqentry = GetEntryData(g_pdlrSequence, lItemCur);

    if(pSeqentry)
    {
      GetSeqUpdateProps(pSeqentry);


      if(UpdateDataFile(pSeqentry->dwOffset, lItemCur))
      {
        //MessageBeep(0xFFFFFFFF);
      }
    }

  }
  return bRet;
}


//=================================================== 
// AddItemToTree - adds items to a tree-view control. 
// Returns the handle of the newly added item. 
// hwndTV - handle of the tree-view control 
// lpszItem - text of the item to add 
// nLevel - level at which to add the item 
//===================================================
HTREEITEM AddItemToTree(HWND hwndTV, LPSTR lpszItem, int nLevel) 
{ 
TV_ITEM             tvi; 
TV_INSERTSTRUCT     tvins; 
static HTREEITEM    hPrev = (HTREEITEM) TVI_FIRST; 
static HTREEITEM    hPrevRootItem = NULL; 
static HTREEITEM    hPrevLev2Item = NULL; 
HTREEITEM           hti; 

tvi.mask = TVIF_TEXT | TVIF_IMAGE  | TVIF_SELECTEDIMAGE | TVIF_PARAM; 

// Set the text of the item. 
tvi.pszText = LPSTR_TEXTCALLBACK ;//lpszItem; 
tvi.cchTextMax = lstrlen(lpszItem); 

// Assume the item is not a parent item, so give it a 
// document image. 
tvi.iImage = g_nDocument; 
tvi.iSelectedImage = g_nDocument; 

// Save the heading level in the item's application-defined 
// data area. 
tvi.lParam = (LPARAM) nLevel; 

tvins.item = tvi; 
tvins.hInsertAfter = hPrev; 

// Set the parent item based on the specified level. 
if (nLevel == 1) 
    tvins.hParent = TVI_ROOT; 
else if (nLevel == 2)
    tvins.hParent = hPrevRootItem; 
else 
    tvins.hParent = hPrevLev2Item; 

// Add the item to the tree-view control. 
hPrev = TreeView_InsertItem(hwndTV, &tvins);

// Save the handle of the item. 
if (nLevel == 1) 
    hPrevRootItem = hPrev; 
else if (nLevel == 2) 
    hPrevLev2Item = hPrev; 

// The new item is a child item. Give the parent item a 
// closed folder bitmap to indicate it now has child items. 
if (nLevel > 1) 
    { 
    hti = TreeView_GetParent(hwndTV, hPrev); 
    tvi.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE; 
    tvi.hItem = hti; 
    tvi.iImage = g_nClosed; 
    tvi.iSelectedImage = g_nOpen; 
    TreeView_SetItem(hwndTV, &tvi); 
    } 

return hPrev; 
} 

 
// InitTreeViewImageLists - creates an image list, adds three bitmaps to 
//     it, and associates the image list with a tree-view control.  
// Returns TRUE if successful or FALSE otherwise. 
// hwndTV - handle of the tree-view control 
//
// Global variables and constants 
//     g_nOpen, g_nClosed, and g_nDocument - integer variables for 
//         indexes of the images 
//     CX_BITMAP and CY_BITMAP - width and height of an icon 
//     NUM_BITMAPS - number of bitmaps to add to the image list 
//================================================================
BOOL InitTreeViewImageLists(HWND hwndTV) 
{ 
HIMAGELIST  himl;  // handle of image list 
HBITMAP     hbmp;     // handle of bitmap 

// Create the image list. 
if ((himl = ImageList_Create(16, 16, 
                            ILC_COLOR, SEQ_NUMBITMAPS, SEQ_NUMBITMAPS)) == NULL) 
    return FALSE; 

// Add the open file, closed file, and document bitmaps. 
hbmp = LoadBitmap(ghInstConsoleDef, MAKEINTRESOURCE(IDB_SEQ_OPEN)); 
g_nOpen = ImageList_Add(himl, hbmp, (HBITMAP) NULL); 
DeleteObject(hbmp); 

hbmp = LoadBitmap(ghInstConsoleDef, MAKEINTRESOURCE(IDB_SEQ_CLOSE)); 
g_nClosed = ImageList_Add(himl, hbmp, (HBITMAP) NULL); 
DeleteObject(hbmp); 

hbmp = LoadBitmap(ghInstConsoleDef, MAKEINTRESOURCE(IDB_SEQ_DOCUMENT)); 
g_nDocument = ImageList_Add(himl, hbmp, (HBITMAP) NULL); 
DeleteObject(hbmp); 

// Fail if not all of the images were added. 
if (ImageList_GetImageCount(himl) < 3) 
    return FALSE; 

// Associate the image list with the tree-view control. 
TreeView_SetImageList(hwndTV, himl, TVSIL_NORMAL); 

return TRUE; 
} 
 


//===================================
BOOL InitSeqList(HWND hwnd)
{
char        szTemp[256];     // temporary buffer 
LV_COLUMN   lvc; 
int         iCol; 

// Init List Columns
//------------------

// Initialize the LV_COLUMN structure. 
lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
lvc.fmt = LVCFMT_LEFT; 
lvc.cx = 100; 
lvc.pszText = szTemp; 

// Add the columns. 
for (iCol = 0; iCol < SEQ_COLUMNS; iCol++) 
    { 
    lvc.iSubItem = iCol; 
    LoadString(ghInstStrRes, IDS_FIRSTSEQCOLUMN + iCol, 
            szTemp, sizeof(szTemp)); 
    if (ListView_InsertColumn(hwnd, iCol, &lvc) == -1) 
        return FALSE; 
    } 
return TRUE; 
}

//==================================================
int HandleSeqTVNExpand(LPNM_TREEVIEW lpnm_tv)
{
/*
if(lpnm_tv->action == TVE_COLLAPSE)
    {
    lpnm_tv->itemNew.iImage         =  g_nOpen;
    lpnm_tv->itemNew.iSelectedImage =  g_nOpen;
    }
else if(lpnm_tv->action == TVE_EXPAND)
    {
    lpnm_tv->itemNew.iImage         =  g_nClosed;
    lpnm_tv->itemNew.iSelectedImage =  g_nClosed;
    }

*/
return 0;
}

//==================================================
int HandleSeqTVNBeginDrag(LPNM_TREEVIEW lpnmtv)
{
//HIMAGELIST  himl;    // handle of image list 
//RECT        rcItem;        // bounding rectangle of item 
DWORD       dwLevel;      // heading level of item 
DWORD       dwIndent;     // amount that child items are indented 
 
// The TreeList Window Handle
g_hwndTV = lpnmtv->hdr.hwndFrom;
g_dcTV = GetDC(g_hwndTV);

GetClientRect(g_hwndTV, &g_rDrag);
// Tell the tree-view control to create an image to use 
// for dragging. 
g_himlDrag = TreeView_CreateDragImage(g_hwndTV, lpnmtv->itemNew.hItem); 

// Get the bounding rectangle of the item being dragged. 
TreeView_GetItemRect(g_hwndTV, lpnmtv->itemNew.hItem, &g_rDragItem, TRUE); 
g_rDragItem.bottom -=  g_rDragItem.top;// Convert the bottom to the Height
g_rDragItem.right  -=  g_rDragItem.left;// Convert the right to Width

// Get the heading level and the amount that the child items are 
// indented. 
dwLevel = lpnmtv->itemNew.lParam; 
dwIndent = (DWORD) SendMessage(g_hwndTV, TVM_GETINDENT, 0, 0); 

// Start the drag operation. 
if(ImageList_BeginDrag(g_himlDrag, 0, 0, 0) == 0)
    // handle an error
    {
    ImageList_Destroy(g_himlDrag);
    return 1;
    }

ImageList_DragEnter(g_hwndTV, lpnmtv->ptDrag.x, lpnmtv->ptDrag.y);


// Hide the mouse cursor, and direct mouse input to the 
// parent window. 
SetCapture(GetParent(g_hwndTV)); 
g_bDragging = TRUE; 

return 0;
}

//=======================================================
void HandleSeqTVNStopDrag(void) 
{ 
HTREEITEM       htiTarget;  // handle of target item 

if (g_bDragging) 
    { 
    StopDragTimer();
    ImageList_DragLeave(g_hwndTV);
    ImageList_EndDrag(); 
    if((htiTarget = TreeView_GetDropHilight(g_hwndTV)) != NULL)
        {
        TreeView_SelectDropTarget(g_hwndTV, NULL); 
        TreeView_SelectItem(g_hwndTV, htiTarget);
        }
    else
        TreeView_SelectDropTarget(g_hwndTV, NULL); 

    ReleaseDC(g_hwndTV, g_dcTV);
    g_himlDrag = NULL;
    g_htiTargetOld = NULL;    
    ReleaseCapture(); 
    g_bDragging = FALSE; 
    } 
return; 
} 
 
// HandleSeqTVN_OnMouseMove - drags an item in a tree-view control,  
//     highlighting the item that is the target. 
// hwndParent - handle of the parent window 
// hwndTV - handle of the tree-view control 
// xCur and yCur - x- and y-coordinates of the mouse cursor 
//=================================================================
void HandleSeqTVN_OnMouseMove(HWND hwndParent, LONG xCur, LONG yCur) 
{ 
HTREEITEM       htiTarget;  // handle of target item 
TV_HITTESTINFO  tvht;  // hit test information 
POINT           pnt;
int             iDragScrlDir;

if (g_bDragging) 
    { 
    pnt.x = xCur;
    pnt.y = yCur;
    // Convert the Point 
    //------------------
    ClientToScreen(hwndParent, &pnt);

    ScreenToClient(g_hwndTV, &pnt);

    tvht.pt = pnt;

    pnt.x += GetSystemMetrics(SM_CXFRAME);
    pnt.y += GetSystemMetrics(SM_CYFRAME);

    // Do not try to Drag outside the Rect
    //------------------------------------
    if(PtInRect(&g_rDrag, pnt) == FALSE)
        {
        StopDragTimer();
        ImageList_DragShowNolock(FALSE);
        return;
        }

    // check if there are enough items
    // visible to scroll through them
    // while dragging an Item
    //---------------------------------
    if((g_rDragItem.bottom * 4) < g_rDrag.bottom) 
        {
        if((pnt.y + (g_rDragItem.bottom*2)) > g_rDrag.bottom)
            iDragScrlDir = 1;
        else 
            if(pnt.y < (g_rDragItem.bottom*2))
                iDragScrlDir = -1;
            else
                iDragScrlDir = 0;


        if(iDragScrlDir)
            StartDragTimer(iDragScrlDir);
        else
            StopDragTimer();
        }


    // Find out if the cursor is on the item. If it is, highlight 
    // the item as a drop target. 
    if((htiTarget = TreeView_HitTest(g_hwndTV, &tvht)) != NULL) 
        {
        if(g_htiTargetOld != htiTarget)
            {
            ImageList_DragShowNolock(FALSE);
            //TreeView_EnsureVisible(hwndTV, htiTarget);
            TreeView_SelectDropTarget(g_hwndTV, htiTarget); 
            g_htiTargetOld = htiTarget;
            }
        }

    // Drag the item to the current position of the mouse cursor. 
    ImageList_DragShowNolock(TRUE);
    ImageList_DragMove(pnt.x, pnt.y);
    } 
return; 
} 

//=======================================
//function: StartDragTimer
//
//
//=======================================
void    StartDragTimer(int iDragScrlDir)
{
g_iDragScrlDir = iDragScrlDir;
if(g_iTimerID)
    return;

g_iTimerID = DRAG_TIMER_ID;
if(SetTimer(ghwndSeqDlg, g_iTimerID, DRAG_DELAY_TIME, NULL) == 0)
    g_iTimerID = 0;
    
return;
}

//=======================================
//function: StopDragTimer
//
//
//=======================================
void    StopDragTimer(void)
{
if(g_iTimerID)
    {
    KillTimer(ghwndSeqDlg, g_iTimerID);
    g_iTimerID = 0;
    }

return;
}

//==========================================
//function: HandleDragTimer
//
//
//==========================================
void    HandleDragTimer(void)
{
POINT       pnt;

// Check the hit results
//----------------------
switch(g_iDragScrlDir)
    {
    case -1:
        ImageList_DragShowNolock(FALSE);
        SendMessage(g_hwndTV, WM_VSCROLL, MAKEWPARAM(SB_LINEUP, 0), (LPARAM)NULL);
        SendMessage(g_hwndTV, WM_VSCROLL, MAKEWPARAM(SB_ENDSCROLL, 0), (LPARAM)NULL);
        ImageList_DragShowNolock(TRUE);
        GetCursorPos(&pnt);
        SetCursorPos(pnt.x, pnt.y);
        break;

    case 1:
        ImageList_DragShowNolock(FALSE);
        SendMessage(g_hwndTV, WM_VSCROLL, MAKEWPARAM(SB_LINEDOWN, 0), (LPARAM)NULL);
        SendMessage(g_hwndTV, WM_VSCROLL, MAKEWPARAM(SB_ENDSCROLL,0), (LPARAM)NULL);
        ImageList_DragShowNolock(TRUE);
        GetCursorPos(&pnt);
        SetCursorPos(pnt.x, pnt.y);
        break;
    default:
        break;
    }

return;
};

//=================================================
//FUNCTION: SeqInsertEntryDL
//
//
//=================================================
void    SeqInsertEntryDL(void)
{
HTREEITEM           htreeitem;
TV_ITEM             tvi;
//int                 i;
long                lItemAdd, lItemCur;
SEQENTRY            seqentry={0};
long                lDLInsert;



htreeitem = TreeView_GetSelection(g_hwndTV);
if(htreeitem == NULL)
    {
    lItemCur = GetLastEntry(g_pdlrSequence);
    lItemAdd = 0;
//    lDLInsert = DL_RELATION_PEER;
    }
else
    {
    tvi.mask = TVIF_PARAM;
    tvi.hItem = htreeitem;
    TreeView_GetItem(g_hwndTV, &tvi);
    lItemCur = (long)tvi.lParam;
//    lDLInsert = DL_RELATION_CHILD;
    }

  lDLInsert = DL_RELATION_PEER; // ??

// Set the text of the item. 
// and other attributes
//--------------------------
wsprintf(seqentry.szName, "Sequence Entry (%d / 0000)", TreeView_GetCount (g_hwndTV) + 1);
seqentry.dwSize = sizeof(SEQENTRY);


  // try to add the data to the data file ...
  // if it is ok    seqentry.dwOffset will be set to the
  // correct offset within the data file ...
  //
  if(AddToDataFile(&seqentry.dwOffset))
  {
    lItemAdd = InsertEntry(g_pdlrSequence, lItemCur, (LPSTR)&seqentry, sizeof(SEQENTRY), 
                           lDLInsert);
    if(lItemAdd == -1)
        return;

    lDLInsert = GetEntryLinkState(g_pdlrSequence, lItemAdd);
    SeqAddItem(lItemAdd, lDLInsert, htreeitem);
  }


return;
}

//=================================================
//FUNCTION: SeqAddItem
//
//add an item to the Sequence 
//
//=================================================
HTREEITEM SeqAddItem(long lItemPtr,long lLinkState,HTREEITEM htreeitem)
{
TV_ITEM             tvi;
TV_INSERTSTRUCT     tvins; 

if(htreeitem == NULL)
    {
    htreeitem = TVI_ROOT;
    }
else
    {
    tvi.mask = TVIF_PARAM;
    tvi.hItem = htreeitem;
    TreeView_GetItem(g_hwndTV, &tvi);
    }

// set the mask parameter
tvi.pszText = LPSTR_TEXTCALLBACK;
tvi.cchTextMax = 0;

tvi.mask = TVIF_TEXT | TVIF_IMAGE  | TVIF_SELECTEDIMAGE | TVIF_PARAM;

if(lLinkState & DLENTRY_CHILD)
    {
    tvi.iImage = g_nClosed; 
    tvi.iSelectedImage = g_nOpen; 
    //tvi.mask |= TVIF_CHILDREN; // I_CHILDRENCALLBACK 
    tvi.cChildren = 1;
    }
else
    {
    tvi.iImage = g_nDocument; 
    tvi.iSelectedImage = g_nDocument; 
    }

// Save the heading level in the item's application-defined 
// data area. 
tvi.lParam = (LPARAM)lItemPtr; 

  if(lLinkState & DLENTRY_CHILD)
  {
    tvins.item = tvi; 
    tvins.hInsertAfter = TVI_LAST; 
    tvins.hParent = htreeitem;
  }
  else
  {
    tvins.item = tvi; 
    tvins.hInsertAfter = htreeitem; 
    tvins.hParent = NULL;
  }

// Add the item to the tree-view control. 
return TreeView_InsertItem(g_hwndTV, &tvins);

//return;
}

//==================================================
//FUNCTION: SeqDeleteItem
//
//
//
//==================================================
void    SeqDeleteItem(void)
{
HTREEITEM       htreeitem;
TV_ITEM         tvi;
long            lItemDel;

htreeitem = TreeView_GetSelection(g_hwndTV);
if(htreeitem == NULL)
    {
    return;
    }

tvi.mask = TVIF_PARAM;
tvi.hItem = htreeitem;
TreeView_GetItem(g_hwndTV, &tvi);

lItemDel =(long)tvi.lParam;
if(lItemDel > 0)
    DelEntryPtr(g_pdlrSequence, lItemDel);

TreeView_DeleteItem(g_hwndTV, htreeitem);

return;
}

//==================================================
//FUNCTION: HandleTVGetItemIfno
//
//
//
//==================================================
void    HandleTVGetDispInfo(TV_DISPINFO *pTVDispInfo)
{
long            lItemCur;
TV_ITEM         *p_tvi;
LPSEQENTRY      p_seqentry;

p_tvi = &pTVDispInfo->item;
// does the item needs text information
//-------------------------------------
if(p_tvi->mask && TVIF_TEXT)
    {
    lItemCur = (long)p_tvi->lParam;
    if(lItemCur < 0)
        return;
    
    p_seqentry = (LPSEQENTRY)GetEntryData(g_pdlrSequence, lItemCur);
    if(p_seqentry == NULL)
        return;
    lstrcpy(p_tvi->pszText, p_seqentry->szName);
    }

return;
}

//==================================================
//FUNCTION: HandleTVSetItemIfno
//
//purpose:
//  to change the Text of a TV item directly
//
//
// **** NOTE THIS FUNCTION NEEDS MORE WORK ****
// **** RIGHT NOW IT WON'T DO ANYTHING     ****
//==================================================
void    HandleTVSetItemIfno(TV_DISPINFO *pTVDispInfo)
{
long            lItemCur;
TV_ITEM         *p_tvi;
LPSEQENTRY      p_seqentry;

p_tvi = &pTVDispInfo->item;
// does the item needs text information
//-------------------------------------
if(p_tvi->mask && TVIF_TEXT)
    {
    lItemCur = (long)p_tvi->lParam;
    if(lItemCur < 0)
        return;
    
    p_seqentry = (LPSEQENTRY)GetEntryData(g_pdlrSequence, lItemCur);
    if(p_seqentry == NULL)
        return;
    lstrcpy(p_tvi->pszText, p_seqentry->szName);
    }


return;
}

//==================================================
//FUNCTION: HandleTVSetItemIfno
//
//
//purpose:
// Store the new name of the Item(Entry)
//==================================================
void    HandleTVEndLabelEdit(TV_DISPINFO *pTVDispInfo)
{
long            lItemCur;
TV_ITEM         *p_tvi;
LPSEQENTRY      p_seqentry;

p_tvi = &pTVDispInfo->item;
// does the item needs text information
//-------------------------------------
if(p_tvi->mask && TVIF_TEXT)
    {
    lItemCur = (long)p_tvi->lParam;
    if(lItemCur < 0)
        return;
    
    p_seqentry = (LPSEQENTRY)GetEntryData(g_pdlrSequence, lItemCur);
    if(p_seqentry == NULL)
        return;
    lstrcpy(p_seqentry->szName, p_tvi->pszText);
    }


return;
}

//=============================================
//FUNCTION: DisplayTVNPopupMenu
//
//
//the name says it all !
//=============================================
void DisplayTVNPopupMenu(HWND hwnd)
{
HMENU   hTVNMenu = NULL;
HMENU   hTVNPopupMenu = NULL;
HMENU   hSubMenu = NULL;
int     iMenu, iID;
UINT    uMenuState;
char    szBuf[128];
POINT   pt;

hTVNMenu = LoadMenu(ghInstStrRes, MAKEINTRESOURCE(MENU_TVN_POPUP));
if(hTVNMenu == NULL)
    goto ON_EXIT;

hTVNPopupMenu = CreatePopupMenu();

iMenu = 0;
 
// Get menu state will return the style of the menu
// in the lobyte of the unsigned int. Return value
// of -1 indicates the menu does not exist, and we
// have finished creating our pop up.
while ((uMenuState = GetMenuState(hTVNMenu, iMenu, MF_BYPOSITION)) != -1)
    {
    if (uMenuState != -1)
        {
        // Get the menu string.
        GetMenuString(hTVNMenu, iMenu, szBuf, 128, MF_BYPOSITION);
        if (LOBYTE(uMenuState) & MF_POPUP) // It's a pop-up menu.
            {
            hSubMenu = GetSubMenu(hTVNMenu, iMenu);
            AppendMenu(hTVNPopupMenu, LOBYTE(uMenuState), (UINT)hSubMenu, szBuf);
            }
        else  // Is a menu item, get the ID.
            {
            iID = GetMenuItemID(hTVNMenu, iMenu);
            AppendMenu(hTVNPopupMenu, uMenuState, iID, szBuf);
            }
        }
    iMenu++;  // Get the next item.
    }


// TrackPopupMenu expects screen coordinates.
if(GetCursorPos(&pt) == FALSE)
    {
    pt.x = 100;
    pt.y = 100;
    }
TrackPopupMenu(hTVNPopupMenu, TPM_LEFTALIGN|TPM_RIGHTBUTTON, pt.x,pt.y, 0, hwnd, NULL);

// Because we are using parts of the main menu in our
// pop-up menu, we can't just delete the pop-up menu, because
// that would also delete the main menu. So we must
// go through the pop-up menu and remove all the items.
while (RemoveMenu(hTVNPopupMenu, 0, MF_BYPOSITION))
    ;


ON_EXIT:
if(hTVNMenu)
    DestroyMenu(hTVNMenu);

if(hTVNPopupMenu)
    DestroyMenu(hTVNPopupMenu);

return;
};


//==================================================
//==================================================
//==================================================
//
// Other Functions
//
//==================================================
//==================================================
//==================================================

//==================================================
//FUNCTION: PrepareMTCReadout
//
//purpose: Prepare the bitmap for
// display in the MTC/SMPTE readout
//==================================================
void    PrepareMTCReadout(void)
{
BITMAP  bmpInfo;
HDC     hdcSrc, hdcDest;
HBITMAP hbmpSrc, hbmpDest;

bmpInfo = g_bmpNumbersInfo;
//bmpInfo.bmWidth = g_rMTCReadout.right;
//bmpInfo.bmHeight = g_rMTCReadout.bottom;
g_hbmpMTC = CreateCompatibleBitmap(ghdc256, bmpInfo.bmWidth-15, bmpInfo.bmHeight);

if(g_hbmpMTC == NULL)
    return;

// Prepare the bitmap
//-------------------
hdcSrc = CreateCompatibleDC(ghdc256);
hdcDest = CreateCompatibleDC(ghdc256);
hbmpDest = SelectObject(hdcDest, g_hbmpMTC);
hbmpSrc = SelectObject(hdcSrc, g_hbmpNumbers);

BitBlt(hdcDest, 30, 0, 15, 30, hdcSrc, 150, 0, SRCCOPY);
BitBlt(hdcDest, 75, 0, 15, 30, hdcSrc, 150, 0, SRCCOPY);
BitBlt(hdcDest, 120, 0, 15, 30, hdcSrc, 150, 0, SRCCOPY);



// Release the bitmaps from the DC
//--------------------------------
SelectObject(hdcSrc, hbmpSrc);
SelectObject(hdcSrc, hbmpDest);

// just for update purpose
//------------------------
g_mtcLast.u.ms = 0xFFFFFFFF;

SendMessage(g_hwndMTCReadout, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, 
                                            (LPARAM)g_hbmpMTC);

DeleteDC(hdcSrc);
DeleteDC(hdcDest);

return;
}

//===================================================
//FUNCTION: FreeMTCReadout
//
//purpose: Free all the resources
//assosiated with the MTC/SMPTE readout
//
//===================================================
void    FreeMTCReadout(void)
{
if(g_hbmpMTC)
    DeleteObject(g_hbmpMTC);

g_hbmpMTC = NULL;
return;
}

//==================================================
//FUNCTION: DisplayMTCReadout
//
//purpose:
//  Display the MTC/SMPTE time
// in the control window
//
//==================================================
void    DisplayMTCReadout(void)
{
HBITMAP     hbmpMem, hbmpDest;
HDC         hdcMem, hdcDest;
int         iHighN, iLowN;

// assmeble the Bitmap
//--------------------
hdcMem = CreateCompatibleDC(ghdc256);
hdcDest = CreateCompatibleDC(ghdc256);
if((hdcMem == NULL) || (hdcDest == NULL))
    goto ON_EXIT;

hbmpMem = SelectObject(hdcMem, g_hbmpNumbers);
hbmpDest = SelectObject(hdcDest, g_hbmpMTC);

// check to see what are the numbers
// that we need to update
//----------------------------------
if(g_mtcLast.u.smpte.hour != g_mtcNow.u.smpte.hour)
    // Update the hour
    //----------------
    {
    iHighN = g_mtcNow.u.smpte.hour /10;
    iLowN   = g_mtcNow.u.smpte.hour - (iHighN*10);

    BitBlt(hdcDest, 0, 0, 15, 30,
           hdcMem, iHighN*15, 0, SRCCOPY);
    BitBlt(hdcDest, 15, 0, 15, 30,
           hdcMem, iLowN*15, 0, SRCCOPY);

    g_mtcLast.u.smpte.hour = g_mtcNow.u.smpte.hour;
    }

if(g_mtcLast.u.smpte.min != g_mtcNow.u.smpte.min)
    // Update the minutes
    //-------------------
    {
    iHighN = g_mtcNow.u.smpte.min /10;
    iLowN   = g_mtcNow.u.smpte.min - (iHighN*10);

    BitBlt(hdcDest, 45, 0, 15, 30,
           hdcMem, iHighN * 15, 0, SRCCOPY);
    BitBlt(hdcDest, 60, 0, 15, 30,
           hdcMem, iLowN*15, 0, SRCCOPY);

    g_mtcLast.u.smpte.min = g_mtcNow.u.smpte.min;
    }

if(g_mtcLast.u.smpte.sec != g_mtcNow.u.smpte.sec)
    // Update the seconds
    //-------------------
    {
    iHighN = g_mtcNow.u.smpte.sec /10;
    iLowN   = g_mtcNow.u.smpte.sec - (iHighN*10);

    BitBlt(hdcDest, 90, 0, 15, 30,
           hdcMem, iHighN * 15, 0, SRCCOPY);
    BitBlt(hdcDest, 105, 0, 15, 30,
           hdcMem, iLowN * 15, 0, SRCCOPY);

    g_mtcLast.u.smpte.sec = g_mtcNow.u.smpte.sec;
    }

if(g_mtcLast.u.smpte.frame != g_mtcNow.u.smpte.frame)
    // Update the frames
    //------------------
    {
    iHighN = g_mtcNow.u.smpte.frame /10;
    iLowN   = g_mtcNow.u.smpte.frame - (iHighN*10);

    BitBlt(hdcDest, 135, 0, 15, 30,
           hdcMem, iHighN * 15, 0, SRCCOPY);
    BitBlt(hdcDest, 150, 0, 15, 30,
           hdcMem, iLowN * 15, 0, SRCCOPY);

    g_mtcLast.u.smpte.frame = g_mtcNow.u.smpte.frame;
    }


SelectObject(hdcMem, hbmpMem);
SelectObject(hdcDest, hbmpDest);


SendMessage(g_hwndMTCReadout, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, 
                                            (LPARAM)g_hbmpMTC);


ON_EXIT:
// Release stuff..
//-----------------
DeleteDC(hdcMem);
DeleteDC(hdcDest);
return;
}



//=================================================
//FUNCTION: MTC_EmulateProc
//
//
//
//=================================================
void    CALLBACK MTC_EmulateProc(UINT uID, UINT iMsg, DWORD dwUser, 
                                    DWORD dw1, DWORD dw2)
{

if(g_iStopTimeEvent == 1)
    {
    StopTimeEvent();
    return;
    }

g_mtcNow.u.smpte.frame++;
VerifySMPTE(&g_mtcNow);

DisplayMTCReadout();

return;
}

//====================================================
//FUNCTION: StopTimeEvent
//
//
//
//
//
//====================================================
void    StopTimeEvent(void)
{

if(timeKillEvent(g_TimeEvent) != TIMERR_NOERROR)
    MessageBeep(0);
g_TimeEvent = 0;
g_iStopTimeEvent = 0;

return;
}

///////////////////////////////////////////////////////
// FUNCTION: VerifySMPTE
//
//
///////////////////////////////////////////////////////
void    VerifySMPTE(MMTIME *pmm)
{

if(pmm->u.smpte.frame > 29)
  {
  pmm->u.smpte.frame = 0;
  pmm->u.smpte.sec ++;
  if(pmm->u.smpte.sec > 59)
    {
    pmm->u.smpte.sec = 0;
    pmm->u.smpte.min ++;
    if(pmm->u.smpte.min > 59)
      {
      pmm->u.smpte.min = 0;
      pmm->u.smpte.hour ++;
      if(pmm->u.smpte.hour > 99)
        {
        pmm->u.smpte.hour = 0;
        }
      }
    }
  }

return;
};



////////////////////////////////////////////////////////
//
//
//
void	HandleRemoteSequenceControl(WORD wControl)
{
	switch(wControl)	
	{
	case IDM_S_PLAY:
		if(RecallEntry() == FALSE)
			SeqGoToFirst();
		break;
	case IDM_S_RECALL_CURRENT:
		break;
	case IDM_S_PAUSE:
		break;
	case IDM_S_GOTO_FIRST:
		SeqGoToFirst();
		break;
	case IDM_S_BACK:
		if(SeqGoToPrev() == FALSE);
			//SeqGoToFirst();
		break;
	case IDM_S_NEXT:
		if (SeqGoToNext() == FALSE){
			if (g_current_item_before_next == NULL)
			SeqGoToFirst();
		}
		break;
	case IDM_S_GOTO_LAST:
		SeqGoToLast();
		break;
	}
};
/////////////////////////////////////////////////////
//
//
void	CloseSequenceFiles (void){
  if(g_TimeEvent)
      g_iStopTimeEvent = 1;

  // Free the Linked list
  //---------------------
	if (g_pdlrSequence != NULL)
		FreeDLListRootAll(&g_pdlrSequence);
  g_pdlrSequence = NULL;
  CloseDataFile();
}
/////////////////////////////////////////////////////
//
//
BOOL	OpenSequenceFiles (void){
	HWND						hwndCtrl;
	char            szBuff[MAX_PATH];

	CloseSequenceFiles ();

   // Prepare the Critical Section
  // for the MTC Emulate
  //-----------------------------
  wsprintf(szBuff, "%s.ctek", g_sequence_file_name);
  //wsprintf(szBuff, "%s\\data\\sequence.ctek",gszProgDir);      
  g_pdlrSequence = InitDoubleLinkedList(sizeof(SEQENTRY), 32, TRUE, TRUE, NULL, szBuff);
  
  if(g_pdlrSequence == NULL)
  {
    return FALSE;
  }

  //wsprintf(szBuff, "%s\\data\\vacs_device.data",gszProgDir);      
  wsprintf(szBuff, "%s.data",g_sequence_file_name);
  if(OpenDataFile(szBuff))
  {

  }
  // init the Tree and the List View
  //--------------------------------
  hwndCtrl = GetDlgItem(ghwndSeqDlg, IDTREE_SEQUENCE);
  if(hwndCtrl)
      FillSeqTree(hwndCtrl);

  hwndCtrl = GetDlgItem(ghwndSeqDlg, IDLIST_SEQ_EVENTS);
  if(hwndCtrl)
      InitSeqList(hwndCtrl);
  
	return TRUE;
}
