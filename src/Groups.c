//=================================================
// Copyright 1998, CorTek Software, Inc.
//=================================================

// Purpose handle all General Group assignments
// and cotrol routines
//
//


#include "SAMM.h"
#include "SAMMEXT.h"
#include "MACRO.h"


// External definitions
extern BOOL    g_bReversDirection;
extern int     g_iMasterModuleIdx;


#define   GROUP_TERMINATOR  0xFFFFFFFF

HWND              ghwndGroup = NULL;
HWND              ghwndGroupsDlg = NULL;
GROUPSTORE        g_CurrentGroup;
int               g_iGroupsDlgOffset = 0;
BOOL              g_columnAdded = FALSE;



LRESULT CALLBACK  GroupWndProc(HWND , UINT , WPARAM , LPARAM );
BOOL CALLBACK     GroupsProc(HWND , UINT , WPARAM , LPARAM );
void              AddToListControl(int);
int               GetLisControlSelection(void);
void              DeleteListControlItem(int item);
BOOL              UpdateGroup(int iListItem, int iType, int iGroup);
BOOL              InitGroupsImageList(HWND hwndLV );
void              FillGroupListControl(int   iType);
void              HandleCtrlBtnClickInGroup(HWND hwnd, LPMIXERWNDDATA lpmwd, LPCTRLZONEMAP pctrlzm, int phis_channel);
int								GetListItemGroupNumber(int iItem);

BOOL              IsCtrlPrePostFilter(int iType);
BOOL							isChanelSafeActive (LPCTRLZONEMAP pctrlzm);


/////////////////////////////////////////////
// Group notification handlers ...
//
void HandleGroupLVSetItemIfno(LV_DISPINFO *);
//----------------
void HandleGroupLVGetDispInfo(LV_DISPINFO *);
//----------------
void HandleGroupEndLabelEdit(LV_DISPINFO *);
//----------------
//HandleGroupLVBeginDrag((NM_LISTVIEW *)lParam);
//--------------
void HandleGroupLVKeyDown(LV_KEYDOWN*);

void EnableTBGroupButtons(int iLast);

//=================================================
//function: RegisterSeqWindowClass
//
//
//=================================================
int RegisterGroupWindowClass(void)
{
int         iReturn;
WNDCLASS    wc;


// Register Full View Class
//--------------------------
ZeroMemory(&wc, sizeof(WNDCLASS));      // Clear wndclass structure

wc.style = CS_HREDRAW | CS_VREDRAW;
wc.lpfnWndProc = (WNDPROC)GroupWndProc;
wc.cbClsExtra = 0;
wc.cbWndExtra = sizeof(LPSTR);// all of the data for this window will be stored here
wc.hInstance = ghInstMain;                        
wc.hIcon = LoadIcon(ghInstConsoleDef, MAKEINTRESOURCE(IDICON_GROUP));// this might leak memory
wc.hCursor = LoadCursor(NULL, IDC_ARROW);
wc.hbrBackground = GetStockObject(GRAY_BRUSH);
wc.lpszMenuName = NULL;
wc.lpszClassName = gszGroupClass;

iReturn = RegisterClass(&wc);

if(iReturn == 0)
	 return(IDS_ERR_REGISTER_CLASS);     // Error... Exit

// Initilize the Groups Memory Container
//
iReturn = InitGroups();
if( iReturn != 0 )
  return iReturn;

return 0;
}


//=================================================
//function: ShowSeqWindow
//
//
//=================================================
int     ShowGroupWindow(BOOL bShow)
{
char                    szTitle[128];
LPGROUPWINDOWFILE       pGroupWF;
DWORD										style;

if(ghwndGroup == NULL)
    {
    pGroupWF = GlobalAlloc(GPTR, sizeof(GROUPWINDOWFILE));
    if(pGroupWF == NULL)
        {
        ErrorBox(ghwndMain, ghInstStrRes, IDS_ERR_CREATE_WINDOW);
        return 1;
        }
    pGroupWF->iWndID = GROUP_WINDOW_FILE_ID;        

		style = MDIS_ALLCHILDSTYLES | WS_CHILD | WS_SYSMENU | WS_CAPTION | WS_VISIBLE
																| WS_THICKFRAME | WS_MINIMIZEBOX// | WS_MAXIMIZEBOX
																| WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		style &= ~WS_MAXIMIZEBOX;

    LoadString(ghInstStrRes, IDS_GROUP_WINDOW_TITLE, szTitle, 128);
    ghwndGroup = CreateMDIWindow (
                            gszGroupClass,
                            szTitle,
														style,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            ghwndMDIClient,
                            ghInstMain,
                            (LPARAM)NULL
                            );

    if(ghwndGroup == NULL)
        {
        GlobalFree(pGroupWF);
        ErrorBox(ghwndMain, ghInstStrRes, IDS_ERR_CREATE_WINDOW);
        return 1;
        }
    SetWindowLong(ghwndGroup, 0, (LONG)pGroupWF);
    }

  if(bShow)
  {
    if(ghwndGroup)
      SetWindowPos(ghwndGroup, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
  }
  else
  {
    if(ghwndGroup)
      ShowWindow(ghwndGroup, SW_HIDE);
  }

return 0;
}


//////////////////////////////////////////////////////////
//function: GroupWndProc
//
//the MDI child window for the Dialog Box
//
LRESULT CALLBACK  GroupWndProc(HWND hwnd, UINT uiMsg, 
                               WPARAM wParam, LPARAM lParam)
{
RECT        rect;
MINMAXINFO  *lpMMI;

switch(uiMsg)
    {
    ////////////////////////////////////////////
//    case WM_MDIACTIVATE:
//        if((HWND)lParam == hwnd)
//            SetActiveWindow(ghwndGroupDlg);
            //SetFocus(ghwndGroupDlg);
//        break;
    ////////////////////////////////////////////
    case WM_CREATE:
        ghwndGroupsDlg = CreateDialog(ghInstStrRes, MAKEINTRESOURCE(IDD_GROUPS), hwnd, GroupsProc);
        if(ghwndGroupsDlg == NULL)
            {
            return -1;
            }
        GetClientRect(ghwndGroupsDlg, &rect);        
        SetWindowPos(hwnd, NULL, 0, 0, 
                        rect.right + GetSystemMetrics(SM_CYDLGFRAME)*4, 
                        rect.bottom + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYDLGFRAME)*4, 
                        SWP_NOMOVE | SWP_NOZORDER);


        ShowWindow(ghwndGroupsDlg, SW_SHOW);
        return DefMDIChildProc(hwnd, uiMsg, wParam, lParam);
        break;

    //////////////////////////////////////////////////////////////
    case WM_SIZE:
      if(wParam != SIZE_MINIMIZED)
          SetWindowPos(ghwndGroupsDlg, NULL, 0, g_iGroupsDlgOffset, 
                        LOWORD(lParam), HIWORD(lParam) - g_iGroupsDlgOffset,
                        SWP_NOZORDER);

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
        lpMMI->ptMinTrackSize.x = 200;
        lpMMI->ptMinTrackSize.y = 200;
        break;

    //////////////////////////////////////////////////////////////
   case WM_ERASEBKGND: // to reduce flashing on the screen
        break;


    case WM_CLOSE:
        SendMessage(ghwndMDIClient, WM_MDIDESTROY, (WPARAM)hwnd, 0L);
        break;

    case WM_DESTROY:
        GlobalFree((HGLOBAL)GetWindowLong(hwnd, 0));
        ghwndGroup = NULL;

    default:
        return DefMDIChildProc(hwnd, uiMsg, wParam, lParam);

	}

return 0;
}


//================================================
//function: SeqProc
//
//main window procedure 
// for the Sequence View
//================================================
BOOL CALLBACK   GroupsProc(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  HWND        hwndItem;
  RECT        rClient;
  RECT        rScreen;
  LV_COLUMN   lvclmn;
  //LV_ITEM     lvitem;
  int         iCount;
	int					iListItem;


switch(uiMsg)
  {

  /////////////////////////////////////////////////////////////
  case WM_NOTIFY:
    switch (((LPNM_LISTVIEW) lParam)->hdr.code)
      {
      //----------------
      case NM_SETFOCUS:
          SendMessage(ghwndMDIClient, WM_MDIACTIVATE, (WPARAM)GetParent(hwnd), 0);
          break;
      //----------------
      case LVN_SETDISPINFO:
          HandleGroupLVSetItemIfno((LV_DISPINFO *)lParam);
          break;
      //----------------
      case LVN_GETDISPINFO:
          HandleGroupLVGetDispInfo((LV_DISPINFO *)lParam);
          break;
      //----------------
      case LVN_ENDLABELEDIT:
          HandleGroupEndLabelEdit((LV_DISPINFO *)lParam);
          
          break;
      //----------------
      case LVN_BEGINDRAG:
          //HandleGroupLVBeginDrag((NM_LISTVIEW *)lParam);
          break;
      //--------------
      case LVN_KEYDOWN:
       if(((LPNMHDR) lParam)->idFrom == IDC_GROUP_LIST)
			 {
	        HandleGroupLVKeyDown((LV_KEYDOWN*)lParam);
			 }
        break;

      // Handle some General Notification messages
      //------------------------------------------
      case NM_RCLICK:
          //if(((LPNMHDR) lParam)->idFrom == IDTREE_SEQUENCE)
          //    DisplayTVNPopupMenu(hwnd);
          break;

      case NM_DBLCLK:
          if(((LPNMHDR) lParam)->idFrom == IDC_GROUP_LIST)
          {
            //ZeroMemory(&lvitem, sizeof(LV_ITEM));
            //lvitem.mask = LVIF_PARAM | LVIS_SELECTED;
            //lvitem.iItem = ListView_();
            //ListView_GetItem(GetDlgItem(hwnd, IDC_GROUP_LIST), 
            //                  );
            if(((NM_LISTVIEW*)lParam)->iItem > -1)
              ActivateGroup(((NM_LISTVIEW*)lParam)->iItem, FADERS_GROUPS, -1);
          }
          break;

    }
    return 0;
    break; //case WM_NOTIFY:
  //------------------------
  case WM_COMMAND:
    switch(LOWORD(wParam))
    {
    case IDC_CANCEL_GROUP:
      DeactivateGroup();
      break;
    //
    case IDC_GROUP_ADD:
      // Adds the current group 
      // to the list of Groups
      //
      iCount = AddGroup(&g_CurrentGroup, FADERS_GROUPS);
      if(iCount > -1)
        AddToListControl(iCount);
      else
        MessageBox(ghwndMain, "Maximum number of groups reached!", "Information", MB_OK | MB_ICONEXCLAMATION);
      break;
    case IDC_GROUP_UPADTE:
			iListItem =  GetLisControlSelection();
			iCount = GetListItemGroupNumber(iListItem);
      UpdateGroup( iListItem, FADERS_GROUPS, iCount);
      break;
    case IDC_GROUP_DELETE:
			iListItem =  GetLisControlSelection();
			iCount = GetListItemGroupNumber(iListItem);
      DeleteGroup( iListItem, FADERS_GROUPS, iCount);
      break;
    case IDC_GROUP_HIDECONTROLS:

      if(IsDlgButtonChecked(hwnd, IDC_GROUP_HIDECONTROLS) == BST_CHECKED)
      {
        hwndItem = GetDlgItem(hwnd, IDC_GROUP_HIDECONTROLS);
        GetWindowRect(hwndItem, &rClient);
        ScreenToClient(hwnd, (LPPOINT)&rClient);
        g_iGroupsDlgOffset = -(rClient.top);
        
        hwndItem = GetParent(hwnd);
        GetWindowRect(hwndItem, &rClient);
        rClient.right -= rClient.left;
        rClient.bottom -= rClient.top;
        SendMessage(hwndItem, WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rClient.right, rClient.bottom));
      }
      else
      {
        g_iGroupsDlgOffset = 0;
        hwndItem = GetParent(hwnd);
        GetWindowRect(hwndItem, &rClient);
        rClient.right -= rClient.left;
        rClient.bottom -= rClient.top;
        SendMessage(hwndItem, WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rClient.right, rClient.bottom));
      };
      break;

    default:
      break;
    }
    break; //  case WM_COMMAND:

  //---------------------------
  case WM_SIZE:
    if(wParam != SIZE_MINIMIZED)
    {
      hwndItem = GetParent(hwnd);
      GetClientRect(hwndItem, &rScreen);


      // Resize the Group List View to fit perfectly inside of the
      // Client area of the Dialog window
      hwndItem = GetDlgItem(hwnd, IDC_GROUP_LIST);
      GetWindowRect(hwndItem, &rClient);
      ScreenToClient(hwnd, (LPPOINT)&rClient);

      SetWindowPos(hwndItem, NULL, 0, 0, rScreen.right - rClient.left, 
                                         rScreen.bottom - rClient.top - g_iGroupsDlgOffset,
                   SWP_NOMOVE | SWP_NOZORDER);

      InitGroupsImageList(hwndItem);
      // insert the first column
      //
      GetClientRect(hwndItem, &rClient);
      if(g_columnAdded == FALSE)
      {

       DWORD dwStyle = GetWindowLong(hwndItem, GWL_STYLE);

        if ((dwStyle & LVS_TYPEMASK) != LVS_REPORT)
            SetWindowLong(hwndItem, GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | LVS_REPORT);

        ZeroMemory(&lvclmn, sizeof(lvclmn));
        lvclmn.iSubItem = 0;
        lvclmn.mask     = LVCF_TEXT | LVCF_FMT | LVCF_SUBITEM | LVCF_WIDTH; 
        lvclmn.fmt      = LVCFMT_LEFT;
        lvclmn.cx       = rClient.right;
        lvclmn.pszText  = "Group Name";
        ListView_InsertColumn(hwndItem, 0, &lvclmn);
        g_columnAdded = TRUE;

        ListView_SetColumnWidth(hwndItem, 0, rClient.right);
        //
        //lvclmn.iSubItem = 1;
        //lvclmn.pszText  = "Dummy";
        //ListView_InsertColumn(hwndItem, 1, &lvclmn);
        //
        //ListView_SetColumnWidth(hwndItem, 1, LVSCW_AUTOSIZE_USEHEADER);
        //ListView_SetColumnWidth(hwndItem, 1, LVSCW_AUTOSIZE_USEHEADER);

      }
      else
      {
        ListView_SetColumnWidth(hwndItem, 0, rClient.right);
        //ListView_SetColumnWidth(hwndItem, 1, rClient.right);
        //ListView_SetColumnWidth(hwndItem, 0, LVSCW_AUTOSIZE_USEHEADER);
        //ListView_SetColumnWidth(hwndItem, 1, LVSCW_AUTOSIZE_USEHEADER);
      }
    }

    break;

  /////////////////////////////////////////////////////
  case WM_INITDIALOG:
    ghwndGroupsDlg = hwnd;
    FillGroupListControl(FADERS_GROUPS);
    break;
  case WM_DESTROY:
      ghwndGroupsDlg = NULL;
      g_columnAdded = FALSE;
  default:
      return FALSE;
  }

return TRUE;
}

////////////////////////////////
//
//
void HandleGroupEndLabelEdit(LV_DISPINFO *pDispInfo)
{
  if(pDispInfo->item.mask & LVIF_TEXT)  
  {
    lstrcpy(gpGroupStoreFaders[pDispInfo->item.lParam].szGroupName, pDispInfo->item.pszText);
    ListView_SetItemText(GetDlgItem(ghwndGroupsDlg, IDC_GROUP_LIST), 
                                 pDispInfo->item.iItem, 
                                 0, 
                                 LPSTR_TEXTCALLBACK);
  }
};


////////////////////////////////
//
//
void HandleGroupLVSetItemIfno(LV_DISPINFO *pDispInfo)
{
  if(pDispInfo->item.mask & LVIF_TEXT)  
  {
    lstrcpy(gpGroupStoreFaders[pDispInfo->item.lParam].szGroupName, pDispInfo->item.pszText);
    //lstrcpy(pDispInfo->item.pszText, gpGroupStoreFaders[pDispInfo->item.lParam].szGroupName);
    //pDispInfo->item.pszText = gpGroupStoreFaders[pDispInfo->item.lParam].szGroupName;
  }
};

////////////////////////////////
//
//
void HandleGroupLVGetDispInfo(LV_DISPINFO *pDispInfo)
{

 
  if(pDispInfo->item.mask & LVIF_TEXT)  
  {
    //lstrcpy(pDispInfo->item.pszText, gpGroupStoreFaders[pDispInfo->item.lParam].szGroupName);
    pDispInfo->item.pszText = gpGroupStoreFaders[pDispInfo->item.lParam].szGroupName;
    pDispInfo->item.cchTextMax = lstrlen(pDispInfo->item.pszText);
  }
};


////////////////////////////////
//
// Handle keyboard input for the
// Group list box

void HandleGroupLVKeyDown(LV_KEYDOWN *pKeyDown)
{
int iListItem, iCount;

	///////////////////////////////////////////////////////////////
  // If user pressed the Up or Down key then activate that group

	switch(pKeyDown->wVKey)
	{
		case VK_UP:
			iListItem =  GetLisControlSelection();
            if(iListItem > -1)
              ActivateGroup(iListItem-1, FADERS_GROUPS, -1);
		break;

		case VK_DOWN:
			iListItem =  GetLisControlSelection();
            if(iListItem > -1)
              ActivateGroup(iListItem+1, FADERS_GROUPS, -1);
		break;

		// Pressing the Delete key has same affect as pressing delete button

		case VK_DELETE:
			iListItem =  GetLisControlSelection();
			iCount = GetListItemGroupNumber(iListItem);
      DeleteGroup( iListItem, FADERS_GROUPS, iCount);

		break;
	}
	///////////////////////////////////////////////////////////////
  
};

////////////////////////////////////////////////////////////
//
//
//
void	UpdateTBGroupButtons()
{
  LPGROUPSTORE  pWalker;
	int						group=1;
  int           iCount;

  pWalker = gpGroupStoreFaders;

  for(iCount = 0; iCount < MAX_GROUPS_COUNT; iCount ++,pWalker++){
    if(pWalker->szGroupName[0] != 0) {
			EnableTBGroupButtons(group); group ++;
    }
  }
};
//////////////////////////////////////////////////////////
//
//
//
//
void FillGroupListControl(int   iType)
{
  LPGROUPSTORE  pWalker;
  int           iCount;
  GROUPSTORE    CurrentGroupTemp;

  switch(iType)
  {
  case FADERS_GROUPS:
    pWalker = gpGroupStoreFaders;
    break;
  
  case MUTES_GROUPS:
    pWalker = gpGroupStoreMutes;
    break;

  case GENERAL_GROUPS:
    pWalker = gpGroupStoreGeneral;
    break;
  }

  CurrentGroupTemp = g_CurrentGroup;
  for(iCount = 0; iCount < MAX_GROUPS_COUNT; iCount ++,pWalker++)
  {
    if(pWalker->szGroupName[0] != 0)
    {
      CopyMemory(&g_CurrentGroup, pWalker, sizeof(GROUPSTORE));
      AddToListControl(iCount);
    }
  }
  g_CurrentGroup = CurrentGroupTemp;
}

/////////////////////////////////////////////
//
//
void  DeleteListControlItem(int item)
{
  HWND      hwnd;
  int       iCount;

  if(item < 0)
    return;

  hwnd = GetDlgItem(ghwndGroupsDlg, IDC_GROUP_LIST);
  iCount = ListView_GetItemCount(hwnd);// + 1;
  
  if(item >= iCount)
    return;

	EnableTBGroupButtons(iCount-1);
  ListView_DeleteItem(hwnd, item);
};

//////////////////////////////////////////////
//
//
//
int   GetLisControlSelection(void)
{
  HWND      hwnd;
  int       item;
  int       iCount;

  hwnd = GetDlgItem(ghwndGroupsDlg, IDC_GROUP_LIST);
  item = ListView_GetItemCount(hwnd);// + 1;

  for(iCount = 0; iCount < item; iCount++)
  {
    if(ListView_GetItemState(hwnd, iCount, LVIS_SELECTED) & LVIS_SELECTED)
      return iCount;
  }


  return -1;
}

int	GetListItemGroupNumber(int iItem)
{
  HWND      hwnd;
	LVITEM		lvi = {0};

	if(iItem < 0)
		return -1;
  hwnd = GetDlgItem(ghwndGroupsDlg, IDC_GROUP_LIST);
	
	lvi.mask = LVIF_PARAM;
	lvi.iItem = iItem;
	if(ListView_GetItem(hwnd, &lvi) == FALSE)
		return -1;
	return lvi.lParam;
}

/////////////////////////////////////////////
//
//
void  AddToListControl(int  iGroupNumber)
{
  LV_ITEM   lvi;
  HWND      hwnd;
  int       item;

  hwnd = GetDlgItem(ghwndGroupsDlg, IDC_GROUP_LIST);
  item = ListView_GetItemCount(hwnd);// + 1;

  if(strlen(g_CurrentGroup.szGroupName) == 0)
    wsprintf(g_CurrentGroup.szGroupName,"Group %d", item);

  
  lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;//| LVIF_STATE ;
  lvi.iItem = item;
  lvi.iSubItem = 0;
//  lvi.state = LVIS_FOCUSED;
//  lvi.stateMask = 0;
  //lvi.pszText = g_CurrentGroup.szGroupName;
  lvi.pszText = LPSTR_TEXTCALLBACK;
  lvi.cchTextMax = MAX_STRING_SIZE;
  lvi.lParam = iGroupNumber;
  lvi.iImage = 0;

  item = ListView_InsertItem(hwnd, &lvi);
	
	EnableTBGroupButtons(item + 1);
  return;
}

/////////////////////////////////////////////////////////////////
//  Groups Public Interface Functions
//
//
//
//


/////////////////////////////////////////////////////////////////
//
//
//
//
BOOL    InitGroupsImageList(HWND hwndLV )
{
  BOOL  bRet = TRUE;
  HICON hicon;
  HIMAGELIST himlSmall;
  HIMAGELIST himlLarge;

  himlSmall = ImageList_Create(16, 16, ILC_COLORDDB | ILC_MASK, 1, 0); 
  himlLarge = ImageList_Create(32, 32, ILC_COLORDDB | ILC_MASK, 1, 0); 

  
  hicon = LoadIcon(ghInstMain, MAKEINTRESOURCE(IDI_GROUP_LIST));
  if (ImageList_AddIcon(himlSmall, hicon) == -1 ||
      ImageList_AddIcon(himlLarge, hicon) == -1)
      return FALSE;

  ListView_SetImageList(hwndLV, himlLarge, LVSIL_NORMAL);
  ListView_SetImageList(hwndLV, himlSmall, LVSIL_SMALL);


  return bRet;
}

//////////////////////////////////////////////////////////////
// InitGroups
//
//
//
int    InitGroups(void)
{
  int   iCount;

  gpGroupStoreFaders = GlobalAlloc(GPTR, sizeof(GROUPSTORE)*MAX_GROUPS_COUNT);
  if(gpGroupStoreFaders == NULL)
    return IDS_ERR_ALLOCATE_MEMORY;

  gpGroupStoreMutes = GlobalAlloc(GPTR, sizeof(GROUPSTORE)*MAX_GROUPS_COUNT);
  if(gpGroupStoreMutes == NULL)
    return IDS_ERR_ALLOCATE_MEMORY;


  gpGroupStoreGeneral = GlobalAlloc(GPTR, sizeof(GROUPSTORE)*MAX_GROUPS_COUNT);
  if(gpGroupStoreGeneral == NULL)
    return IDS_ERR_ALLOCATE_MEMORY;

  // OK ... Set them all to nothing
  for(iCount = 0; iCount < MAX_GROUPS_COUNT; iCount ++)
  {
    gpGroupStoreFaders[iCount].Count = 0;
    gpGroupStoreMutes[iCount].Count = 0;
    gpGroupStoreGeneral[iCount].Count = 0;
  }

  // Now prepare the Stereo groups
  //
  // Input channel Stereo grouped controls
  g_StereoGroups[0].iModuleType = 1;
  g_StereoGroups[0].wLeft = CTRL_NUM_INPUT_AUX1LT_FADER;
  g_StereoGroups[0].wRight = CTRL_NUM_INPUT_AUX1RT_FADER;

  g_StereoGroups[1].iModuleType = 1;
  g_StereoGroups[1].wLeft = CTRL_NUM_INPUT_AUX2LT_FADER;
  g_StereoGroups[1].wRight = CTRL_NUM_INPUT_AUX2RT_FADER;

  g_StereoGroups[2].iModuleType = 1;
  g_StereoGroups[2].wLeft = CTRL_NUM_INPUT_AUX3LT_FADER;
  g_StereoGroups[2].wRight = CTRL_NUM_INPUT_AUX3RT_FADER;

  g_StereoGroups[3].iModuleType = 1;
  g_StereoGroups[3].wLeft = CTRL_NUM_INPUT_AUX4LT_FADER;
  g_StereoGroups[3].wRight = CTRL_NUM_INPUT_AUX4RT_FADER;

  g_StereoGroups[4].iModuleType = 1;
  g_StereoGroups[4].wLeft = CTRL_NUM_INPUT_AUX5LT_FADER;
  g_StereoGroups[4].wRight = CTRL_NUM_INPUT_AUX5RT_FADER;

  g_StereoGroups[5].iModuleType = 1;
  g_StereoGroups[5].wLeft = CTRL_NUM_INPUT_AUX6LT_FADER;
  g_StereoGroups[5].wRight = CTRL_NUM_INPUT_AUX6RT_FADER;

  g_StereoGroups[6].iModuleType = 1;
  g_StereoGroups[6].wLeft = CTRL_NUM_INPUT_AUX7LT_FADER;
  g_StereoGroups[6].wRight = CTRL_NUM_INPUT_AUX7RT_FADER;

  g_StereoGroups[7].iModuleType = 1;
  g_StereoGroups[7].wLeft = CTRL_NUM_INPUT_AUX8LT_FADER;
  g_StereoGroups[7].wRight = CTRL_NUM_INPUT_AUX8RT_FADER;

  g_StereoGroups[8].iModuleType = 1;
  g_StereoGroups[8].wLeft = CTRL_NUM_INPUT_AUX9LT_FADER;
  g_StereoGroups[8].wRight = CTRL_NUM_INPUT_AUX9RT_FADER;

  g_StereoGroups[9].iModuleType = 1;
  g_StereoGroups[9].wLeft = CTRL_NUM_INPUT_AUX10LT_FADER;
  g_StereoGroups[9].wRight = CTRL_NUM_INPUT_AUX10RT_FADER;

  g_StereoGroups[10].iModuleType = 1;
  g_StereoGroups[10].wLeft = CTRL_NUM_INPUT_AUX11LT_FADER;
  g_StereoGroups[10].wRight = CTRL_NUM_INPUT_AUX11RT_FADER;

  g_StereoGroups[11].iModuleType = 1;
  g_StereoGroups[11].wLeft = CTRL_NUM_INPUT_AUX12LT_FADER;
  g_StereoGroups[11].wRight = CTRL_NUM_INPUT_AUX12RT_FADER;

  g_StereoGroups[12].iModuleType = 1;
  g_StereoGroups[12].wLeft = CTRL_NUM_INPUT_AUX13LT_FADER;
  g_StereoGroups[12].wRight = CTRL_NUM_INPUT_AUX13RT_FADER;

  g_StereoGroups[13].iModuleType = 1;
  g_StereoGroups[13].wLeft = CTRL_NUM_INPUT_AUX14LT_FADER;
  g_StereoGroups[13].wRight = CTRL_NUM_INPUT_AUX14RT_FADER;

  g_StereoGroups[14].iModuleType = 1;
  g_StereoGroups[14].wLeft = CTRL_NUM_INPUT_AUX15LT_FADER;
  g_StereoGroups[14].wRight = CTRL_NUM_INPUT_AUX15RT_FADER;

  g_StereoGroups[15].iModuleType = 1;
  g_StereoGroups[15].wLeft = CTRL_NUM_INPUT_AUX16LT_FADER;
  g_StereoGroups[15].wRight = CTRL_NUM_INPUT_AUX16RT_FADER;

  // Subs
  g_StereoGroups[16].iModuleType = 1;
  g_StereoGroups[16].wLeft = CTRL_NUM_INPUT_SUB8LT_FADER;
  g_StereoGroups[16].wRight = CTRL_NUM_INPUT_SUB8RT_FADER;

  g_StereoGroups[17].iModuleType = 1;
  g_StereoGroups[17].wLeft = CTRL_NUM_INPUT_SUB7LT_FADER;
  g_StereoGroups[17].wRight = CTRL_NUM_INPUT_SUB7RT_FADER;

  g_StereoGroups[18].iModuleType = 1;
  g_StereoGroups[18].wLeft = CTRL_NUM_INPUT_SUB6LT_FADER;
  g_StereoGroups[18].wRight = CTRL_NUM_INPUT_SUB6RT_FADER;

  g_StereoGroups[19].iModuleType = 1;
  g_StereoGroups[19].wLeft = CTRL_NUM_INPUT_SUB5LT_FADER;
  g_StereoGroups[19].wRight = CTRL_NUM_INPUT_SUB5RT_FADER;

  g_StereoGroups[20].iModuleType = 1;
  g_StereoGroups[20].wLeft = CTRL_NUM_INPUT_SUB4LT_FADER;
  g_StereoGroups[20].wRight = CTRL_NUM_INPUT_SUB4RT_FADER;

  g_StereoGroups[21].iModuleType = 1;
  g_StereoGroups[21].wLeft = CTRL_NUM_INPUT_SUB3LT_FADER;
  g_StereoGroups[21].wRight = CTRL_NUM_INPUT_SUB3RT_FADER;

  g_StereoGroups[22].iModuleType = 1;
  g_StereoGroups[22].wLeft = CTRL_NUM_INPUT_SUB2LT_FADER;
  g_StereoGroups[22].wRight = CTRL_NUM_INPUT_SUB2RT_FADER;

  g_StereoGroups[23].iModuleType = 1;
  g_StereoGroups[23].wLeft = CTRL_NUM_INPUT_SUB1LT_FADER;
  g_StereoGroups[23].wRight = CTRL_NUM_INPUT_SUB1RT_FADER;

  // Line B to Aux 16
  g_StereoGroups[24].iModuleType = 1;
  g_StereoGroups[24].wLeft = CTRL_NUM_INPUT_AUX16LT_LINEB_FADER;
  g_StereoGroups[24].wRight = CTRL_NUM_INPUT_AUX16RT_LINEB_FADER;

  // Master return 
  g_StereoGroups[25].iModuleType = 1;
  g_StereoGroups[25].wLeft = CTRL_NUM_INPUT_MASTERLT_LEVEL;
  g_StereoGroups[25].wRight = CTRL_NUM_INPUT_MASTERRT_LEVEL;
  


  ////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // SubAux Stereo Controls
  
  g_StereoGroups[26].iModuleType = 2;
  g_StereoGroups[26].wLeft =   CTRL_NUM_MATRIX_SUB_VOL_LT;
  g_StereoGroups[26].wRight =   CTRL_NUM_MATRIX_SUB_VOL_RT;

  g_StereoGroups[27].iModuleType = 3;
  g_StereoGroups[27].wLeft =   CTRL_NUM_MATRIX_LT_FADER;
  g_StereoGroups[27].wRight =   CTRL_NUM_MATRIX_RT_FADER;



  g_StereoGroups[28].iModuleType = 3;
  g_StereoGroups[28].wLeft =   CTRL_NUM_MATRIX_HI_FREQ_LT;
  g_StereoGroups[28].wRight =   CTRL_NUM_MATRIX_HI_FREQ_RT;
  
  g_StereoGroups[29].iModuleType = 3;
  g_StereoGroups[29].wLeft =   CTRL_NUM_MATRIX_HF_BW_LT;
  g_StereoGroups[29].wRight =   CTRL_NUM_MATRIX_HF_BW_RT;

  g_StereoGroups[30].iModuleType = 3;
  g_StereoGroups[30].wLeft =   CTRL_NUM_MATRIX_HF_BC_LT;
  g_StereoGroups[30].wRight =   CTRL_NUM_MATRIX_HF_BC_RT;

  g_StereoGroups[31].iModuleType = 3;
  g_StereoGroups[31].wLeft =   CTRL_NUM_MATRIX_HF_PK_SHELF_LT;
  g_StereoGroups[31].wRight =   CTRL_NUM_MATRIX_HF_PK_SHELF_RT;

  g_StereoGroups[32].iModuleType = 3;
  g_StereoGroups[32].wLeft =   CTRL_NUM_MATRIX_HM_FREQ_LT;
  g_StereoGroups[32].wRight =   CTRL_NUM_MATRIX_HM_FREQ_RT;

  g_StereoGroups[33].iModuleType = 3;
  g_StereoGroups[33].wLeft =   CTRL_NUM_MATRIX_HM_BW_LT;
  g_StereoGroups[33].wRight =   CTRL_NUM_MATRIX_HM_BW_RT;

  g_StereoGroups[34].iModuleType = 3;
  g_StereoGroups[34].wLeft =   CTRL_NUM_MATRIX_HM_BC_LT;
  g_StereoGroups[34].wRight =   CTRL_NUM_MATRIX_HM_BC_RT;

  g_StereoGroups[35].iModuleType = 3;
  g_StereoGroups[35].wLeft =   CTRL_NUM_MATRIX_LM_FREQ_LT;
  g_StereoGroups[35].wRight =   CTRL_NUM_MATRIX_LM_FREQ_RT;

  g_StereoGroups[36].iModuleType = 3;
  g_StereoGroups[36].wLeft =   CTRL_NUM_MATRIX_LM_BW_LT;
  g_StereoGroups[36].wRight =   CTRL_NUM_MATRIX_LM_BW_RT;

  g_StereoGroups[37].iModuleType = 3;
  g_StereoGroups[37].wLeft =   CTRL_NUM_MATRIX_LM_BC_LT;
  g_StereoGroups[37].wRight =   CTRL_NUM_MATRIX_LM_BC_RT;

  g_StereoGroups[38].iModuleType = 3;
  g_StereoGroups[38].wLeft =   CTRL_NUM_MATRIX_LO_FREQ_LT;
  g_StereoGroups[38].wRight =   CTRL_NUM_MATRIX_LO_FREQ_RT;

  g_StereoGroups[39].iModuleType = 3;
  g_StereoGroups[39].wLeft =   CTRL_NUM_MATRIX_LF_BW_LT;
  g_StereoGroups[39].wRight =   CTRL_NUM_MATRIX_LF_BW_RT;

  g_StereoGroups[40].iModuleType = 3;
  g_StereoGroups[40].wLeft =   CTRL_NUM_MATRIX_LF_BC_LT;
  g_StereoGroups[40].wRight =   CTRL_NUM_MATRIX_LF_BC_RT;

  g_StereoGroups[41].iModuleType = 3;
  g_StereoGroups[41].wLeft =   CTRL_NUM_MATRIX_LF_PK_SHELF_LT;
  g_StereoGroups[41].wRight =   CTRL_NUM_MATRIX_LF_PK_SHELF_RT;
  

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Cue module stereo stuff
  g_StereoGroups[42].iModuleType = 4;
  g_StereoGroups[42].wLeft = CTRL_NUM_MASTERLT_CUE_A_LT;
  g_StereoGroups[42].wRight = CTRL_NUM_MASTERLT_CUE_A_RT;

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Master module volume stereo Group
  g_StereoGroups[43].iModuleType = 5;
  g_StereoGroups[43].wLeft = CTRL_NUM_MASTERLT_FADER;
  g_StereoGroups[43].wRight = CTRL_NUM_MASTERRT_FADER;



  g_StereoGroups[44].iModuleType = 2;
  g_StereoGroups[44].wLeft = CTRL_NUM_MASTER_AUX16LT_FADER;
  g_StereoGroups[44].wRight = CTRL_NUM_MASTER_AUX16RT_FADER;
			
  g_StereoGroups[45].iModuleType = 2;
  g_StereoGroups[45].wLeft = CTRL_NUM_MASTER_AUX15LT_FADER;
  g_StereoGroups[45].wRight = CTRL_NUM_MASTER_AUX15RT_FADER;

  g_StereoGroups[46].iModuleType = 2;
  g_StereoGroups[46].wLeft = CTRL_NUM_MASTER_AUX14LT_FADER;
  g_StereoGroups[46].wRight = CTRL_NUM_MASTER_AUX14RT_FADER;
			
  g_StereoGroups[47].iModuleType = 2;
  g_StereoGroups[47].wLeft = CTRL_NUM_MASTER_AUX13LT_FADER;
  g_StereoGroups[47].wRight = CTRL_NUM_MASTER_AUX13RT_FADER;


  g_StereoGroups[48].iModuleType = 2;
  g_StereoGroups[48].wLeft = CTRL_NUM_MASTER_AUX12LT_FADER;
  g_StereoGroups[48].wRight = CTRL_NUM_MASTER_AUX12RT_FADER;

  g_StereoGroups[49].iModuleType = 2;
  g_StereoGroups[49].wLeft = CTRL_NUM_MASTER_AUX11LT_FADER;
  g_StereoGroups[49].wRight = CTRL_NUM_MASTER_AUX11RT_FADER;
			
  g_StereoGroups[50].iModuleType = 2;
  g_StereoGroups[50].wLeft = CTRL_NUM_MASTER_AUX10LT_FADER;
  g_StereoGroups[50].wRight = CTRL_NUM_MASTER_AUX10RT_FADER;

  g_StereoGroups[51].iModuleType = 2;
  g_StereoGroups[51].wLeft = CTRL_NUM_MASTER_AUX9LT_FADER;
  g_StereoGroups[51].wRight = CTRL_NUM_MASTER_AUX9RT_FADER;

  g_StereoGroups[52].iModuleType = 2;
  g_StereoGroups[52].wLeft = CTRL_NUM_MASTER_AUX8LT_FADER;
  g_StereoGroups[52].wRight = CTRL_NUM_MASTER_AUX8RT_FADER;

  g_StereoGroups[53].iModuleType = 2;
  g_StereoGroups[53].wLeft = CTRL_NUM_MASTER_AUX7LT_FADER;
  g_StereoGroups[53].wRight = CTRL_NUM_MASTER_AUX7RT_FADER;

  g_StereoGroups[54].iModuleType = 2;
  g_StereoGroups[54].wLeft = CTRL_NUM_MASTER_AUX6LT_FADER;
  g_StereoGroups[54].wRight = CTRL_NUM_MASTER_AUX6RT_FADER;

  g_StereoGroups[55].iModuleType = 2;
  g_StereoGroups[55].wLeft = CTRL_NUM_MASTER_AUX5LT_FADER;
  g_StereoGroups[55].wRight = CTRL_NUM_MASTER_AUX5RT_FADER;

  g_StereoGroups[56].iModuleType = 2;
  g_StereoGroups[56].wLeft = CTRL_NUM_MASTER_AUX4LT_FADER;
  g_StereoGroups[56].wRight = CTRL_NUM_MASTER_AUX4RT_FADER;

  g_StereoGroups[57].iModuleType = 2;
  g_StereoGroups[57].wLeft = CTRL_NUM_MASTER_AUX3LT_FADER;
  g_StereoGroups[57].wRight = CTRL_NUM_MASTER_AUX3RT_FADER;

  g_StereoGroups[58].iModuleType = 2;
  g_StereoGroups[58].wLeft = CTRL_NUM_MASTER_AUX2LT_FADER;
  g_StereoGroups[58].wRight = CTRL_NUM_MASTER_AUX2RT_FADER;

  g_StereoGroups[59].iModuleType = 2;
  g_StereoGroups[59].wLeft = CTRL_NUM_MASTER_AUX1LT_FADER;
  g_StereoGroups[59].wRight = CTRL_NUM_MASTER_AUX1RT_FADER;


	// Master module ... stuff
	//
  g_StereoGroups[60].iModuleType = 5;
  g_StereoGroups[60].wLeft = CTRL_NUM_MASTERLT_SUM_IN;
  g_StereoGroups[60].wRight = CTRL_NUM_MASTERRT_SUM_IN;


  g_StereoGroups[61].iModuleType = 5;
  g_StereoGroups[61].wLeft = CTRL_NUM_MASTER_POST_LT_VU;
  g_StereoGroups[61].wRight = CTRL_NUM_MASTER_POST_RT_VU;


	// Matrix module ..
	//
  g_StereoGroups[62].iModuleType = 3;
  g_StereoGroups[62].wLeft = CTRL_NUM_MATRIX_AUX16LT;
  g_StereoGroups[62].wRight = CTRL_NUM_MATRIX_AUX16RT;

  g_StereoGroups[63].iModuleType = 3;
  g_StereoGroups[63].wLeft = CTRL_NUM_MATRIX_AUX15LT;
  g_StereoGroups[63].wRight = CTRL_NUM_MATRIX_AUX15RT;

  g_StereoGroups[64].iModuleType = 3;
  g_StereoGroups[64].wLeft = CTRL_NUM_MATRIX_AUX14LT;
  g_StereoGroups[64].wRight = CTRL_NUM_MATRIX_AUX14RT;

  g_StereoGroups[65].iModuleType = 3;
  g_StereoGroups[65].wLeft = CTRL_NUM_MATRIX_AUX13LT;
  g_StereoGroups[65].wRight = CTRL_NUM_MATRIX_AUX13RT;

  g_StereoGroups[66].iModuleType = 3;
  g_StereoGroups[66].wLeft = CTRL_NUM_MATRIX_AUX12LT;
  g_StereoGroups[66].wRight = CTRL_NUM_MATRIX_AUX12RT;

  g_StereoGroups[67].iModuleType = 3;
  g_StereoGroups[67].wLeft = CTRL_NUM_MATRIX_AUX11LT;
  g_StereoGroups[67].wRight = CTRL_NUM_MATRIX_AUX11RT;

  g_StereoGroups[68].iModuleType = 3;
  g_StereoGroups[68].wLeft = CTRL_NUM_MATRIX_AUX10LT;
  g_StereoGroups[68].wRight = CTRL_NUM_MATRIX_AUX10RT;

  g_StereoGroups[69].iModuleType = 3;
  g_StereoGroups[69].wLeft = CTRL_NUM_MATRIX_AUX09LT;
  g_StereoGroups[69].wRight = CTRL_NUM_MATRIX_AUX09RT;

  g_StereoGroups[70].iModuleType = 3;
  g_StereoGroups[70].wLeft = CTRL_NUM_MATRIX_AUX08LT;
  g_StereoGroups[70].wRight = CTRL_NUM_MATRIX_AUX08RT;

  g_StereoGroups[71].iModuleType = 3;
  g_StereoGroups[71].wLeft = CTRL_NUM_MATRIX_AUX07LT;
  g_StereoGroups[71].wRight = CTRL_NUM_MATRIX_AUX07RT;

  g_StereoGroups[72].iModuleType = 3;
  g_StereoGroups[72].wLeft = CTRL_NUM_MATRIX_AUX06LT;
  g_StereoGroups[72].wRight = CTRL_NUM_MATRIX_AUX06RT;

  g_StereoGroups[73].iModuleType = 3;
  g_StereoGroups[73].wLeft = CTRL_NUM_MATRIX_AUX05LT;
  g_StereoGroups[73].wRight = CTRL_NUM_MATRIX_AUX05RT;

  g_StereoGroups[74].iModuleType = 3;
  g_StereoGroups[74].wLeft = CTRL_NUM_MATRIX_AUX04LT;
  g_StereoGroups[74].wRight = CTRL_NUM_MATRIX_AUX04RT;

  g_StereoGroups[75].iModuleType = 3;
  g_StereoGroups[75].wLeft = CTRL_NUM_MATRIX_AUX03LT;
  g_StereoGroups[75].wRight = CTRL_NUM_MATRIX_AUX03RT;

  g_StereoGroups[76].iModuleType = 3;
  g_StereoGroups[76].wLeft = CTRL_NUM_MATRIX_AUX02LT;
  g_StereoGroups[76].wRight = CTRL_NUM_MATRIX_AUX02RT;

  g_StereoGroups[77].iModuleType = 3;
  g_StereoGroups[77].wLeft = CTRL_NUM_MATRIX_AUX01LT;
  g_StereoGroups[77].wRight = CTRL_NUM_MATRIX_AUX01RT;


  g_StereoGroups[78].iModuleType = 3;
  g_StereoGroups[78].wLeft = CTRL_NUM_MATRIX_SUB08LT;
  g_StereoGroups[78].wRight = CTRL_NUM_MATRIX_SUB08RT;
	
  g_StereoGroups[79].iModuleType = 3;
  g_StereoGroups[79].wLeft = CTRL_NUM_MATRIX_SUB07LT;
  g_StereoGroups[79].wRight = CTRL_NUM_MATRIX_SUB07RT;
	
  g_StereoGroups[80].iModuleType = 3;
  g_StereoGroups[80].wLeft = CTRL_NUM_MATRIX_SUB06LT;
  g_StereoGroups[80].wRight = CTRL_NUM_MATRIX_SUB06RT;
	
  g_StereoGroups[81].iModuleType = 3;
  g_StereoGroups[81].wLeft = CTRL_NUM_MATRIX_SUB05LT;
  g_StereoGroups[81].wRight = CTRL_NUM_MATRIX_SUB05RT;
	
  g_StereoGroups[82].iModuleType = 3;
  g_StereoGroups[82].wLeft = CTRL_NUM_MATRIX_SUB04LT;
  g_StereoGroups[82].wRight = CTRL_NUM_MATRIX_SUB04RT;
	
  g_StereoGroups[83].iModuleType = 3;
  g_StereoGroups[83].wLeft = CTRL_NUM_MATRIX_SUB03LT;
  g_StereoGroups[83].wRight = CTRL_NUM_MATRIX_SUB03RT;
	
  g_StereoGroups[84].iModuleType = 3;
  g_StereoGroups[84].wLeft = CTRL_NUM_MATRIX_SUB02LT;
  g_StereoGroups[84].wRight = CTRL_NUM_MATRIX_SUB02RT;
	
  g_StereoGroups[85].iModuleType = 3;
  g_StereoGroups[85].wLeft = CTRL_NUM_MATRIX_SUB01LT;
  g_StereoGroups[85].wRight = CTRL_NUM_MATRIX_SUB01RT;
	
  g_StereoGroups[86].iModuleType = 2;
  g_StereoGroups[86].wLeft = CTRL_NUM_MATRIX_SUB_LT_SUM_IN;
  g_StereoGroups[86].wRight = CTRL_NUM_MATRIX_SUB_RT_SUM_IN;

  g_StereoGroups[87].iModuleType = 2;
  g_StereoGroups[87].wLeft = CTRL_NUM_MATRIXLT_MUTE_SUB;
  g_StereoGroups[87].wRight = CTRL_NUM_MATRIXRT_MUTE_SUB;

  g_StereoGroups[88].iModuleType = 2;
  g_StereoGroups[88].wLeft = CTRL_NUM_MATRIXLT_INSERT_SUB;
  g_StereoGroups[88].wRight = CTRL_NUM_MATRIXRT_INSERT_SUB;
	
  g_StereoGroups[89].iModuleType = 5;
  g_StereoGroups[89].wLeft = CTRL_NUM_MASTER_INSERTLT;
  g_StereoGroups[89].wRight = CTRL_NUM_MASTER_INSERTRT;

  g_StereoGroups[90].iModuleType = 3;
  g_StereoGroups[90].wLeft = CTRL_NUM_MATRIXLT_INSERT;
  g_StereoGroups[90].wRight = CTRL_NUM_MATRIXRT_INSERT;
  
  g_StereoGroups[91].iModuleType = 3;
  g_StereoGroups[91].wLeft = CTRL_NUM_MATRIX_SUMINLT;
  g_StereoGroups[91].wRight = CTRL_NUM_MATRIX_SUMINRT;

  g_StereoGroups[92].iModuleType = 3;
  g_StereoGroups[92].wLeft = CTRL_NUM_MATRIX_MASTER_PRELT;
  g_StereoGroups[92].wRight = CTRL_NUM_MATRIX_MASTER_PRERT;

  g_StereoGroups[93].iModuleType = 3;
  g_StereoGroups[93].wLeft = CTRL_NUM_MATRIX_LT_INOUT;
  g_StereoGroups[93].wRight = CTRL_NUM_MATRIX_RT_INOUT;



  return 0;
}

//////////////////////////////////////////////////////////////
// DeInitGroups
//
//
//
void  DeInitGroups(void)
{
  if(gpGroupStoreFaders != NULL)
    GlobalFree(gpGroupStoreFaders);

  if(gpGroupStoreMutes != NULL)
    GlobalFree(gpGroupStoreMutes);

  if(gpGroupStoreGeneral != NULL)
    GlobalFree(gpGroupStoreGeneral);

}

///////////////////////////////////////////////////////////////
// ActivateGroup
//
// parms:
//  int     iGroup; which group is to be selected
//  int     iType; what type is the Group
//
//
BOOL    ActivateGroup(int iListItem, int iType, int iForcedGroup)
{
  BOOL    bRet = TRUE;
	int			iGroup = -1;

	if(iForcedGroup >= 0)
		iGroup = iForcedGroup;
	else
		iGroup = GetListItemGroupNumber(iListItem);
	
	if(iGroup < 0)
		return FALSE;

  switch(iType)
  {
  case FADERS_GROUPS:
    CopyMemory(&g_CurrentGroup,  &gpGroupStoreFaders[iGroup], sizeof(GROUPSTORE));
    break;
  
  case MUTES_GROUPS:
    CopyMemory(&g_CurrentGroup,  &gpGroupStoreMutes[iGroup], sizeof(GROUPSTORE));
    break;

  case GENERAL_GROUPS:
    CopyMemory(&g_CurrentGroup,  &gpGroupStoreGeneral[iGroup], sizeof(GROUPSTORE));
    break;
  }

  RefreshAllLblWindows();
  return bRet;
}


///////////////////////////////////////////////////////////////
// Deactivate any Group that might be active
//
//
//
void    DeactivateGroup(void)
{

  ZeroMemory(&g_CurrentGroup, sizeof(GROUPSTORE));
  RefreshAllLblWindows();

  return;
}


///////////////////////////////////////////////////////////////
// IsGrouped
//
//
//
BOOL  IsGrouped(int iChannel)
{
  return (g_CurrentGroup.Group[iChannel].iActive > 0)? TRUE:FALSE;
}

///////////////////////////////////////////////////////////////
// AddGroup
//
//
//
int AddGroup(LPGROUPSTORE pGrp, int iType)
{
  int           iRet = -1;
  GROUPSTORE    *pWalker;
  int           iCount;

  switch(iType)
  {
  case FADERS_GROUPS:
    pWalker = gpGroupStoreFaders;
    break;
  
  case MUTES_GROUPS:
    pWalker = gpGroupStoreMutes;
    break;

  case GENERAL_GROUPS:
    pWalker = gpGroupStoreGeneral;
    break;
  }

  for(iCount = 0; iCount < MAX_GROUPS_COUNT; iCount ++,pWalker++)
  {
    if(pWalker->Count == 0  && pWalker->szGroupName[0] == 0)
    {
      iRet = iCount;
      //if(strlen(pGrp->szGroupName) == 0)
      wsprintf(pGrp->szGroupName,"Group %d", iCount+1);

      CopyMemory(pWalker,  pGrp, sizeof(GROUPSTORE));
      break;
    }
  }

	// Update the Tool bar ...
	//


  return iRet;
}

int	FindConsecutiveGroupIndex(int iNum, int iType)
{
  int           iRet = 0;
  GROUPSTORE    *pWalker;
  int           iCount;

	// Since everything is Zero based ...
	if(iNum < 0)
		return -1;

  switch(iType)
  {
  case FADERS_GROUPS:
    pWalker = gpGroupStoreFaders;
    break;
  
  case MUTES_GROUPS:
    pWalker = gpGroupStoreMutes;
    break;

  case GENERAL_GROUPS:
    pWalker = gpGroupStoreGeneral;
    break;
  }

  for(iCount = 0; iCount < MAX_GROUPS_COUNT; iCount ++,pWalker++)
  {
    if(pWalker->Count > 0  || pWalker->szGroupName[0] != 0)
    {
			if(iRet == iNum)
				return iCount; // we need to return the actual index of this group
      iRet++;
    }
  }

	iRet = -1;

  return iRet;

}


////////////////////////////////////////////
//
//
//
BOOL DeleteGroup(int iListItem, int iType, int iGroup)
{
  BOOL    bRet = TRUE;

  if((iGroup < 0) || (iGroup >=MAX_GROUPS_COUNT) || iListItem < 0)
    return FALSE;

  switch(iType)
  {
  case FADERS_GROUPS:
    ZeroMemory(&gpGroupStoreFaders[iGroup], sizeof(GROUPSTORE));
    break;
  
  case MUTES_GROUPS:
    ZeroMemory(&gpGroupStoreMutes[iGroup], sizeof(GROUPSTORE));
    break;

  case GENERAL_GROUPS:
    ZeroMemory(&gpGroupStoreGeneral[iGroup], sizeof(GROUPSTORE));
    break;
  }

  DeleteListControlItem(iListItem);
  return bRet;
}

/////////////////////////////////////////////////////////////////
//
//
//
BOOL    UpdateGroup(int iListItem, int iType, int iGroup)
{
  BOOL  bRet = TRUE;

  if((iGroup < 0) || (iGroup >=MAX_GROUPS_COUNT) || iListItem < 0)
    return FALSE;

  switch(iType)
  {
  case FADERS_GROUPS:
		CopyMemory(g_CurrentGroup.szGroupName, gpGroupStoreFaders[iGroup].szGroupName, sizeof(g_CurrentGroup.szGroupName));
    CopyMemory(&gpGroupStoreFaders[iGroup],  &g_CurrentGroup, sizeof(GROUPSTORE));
    break;
  
  case MUTES_GROUPS:
    CopyMemory(&gpGroupStoreMutes[iGroup],  &g_CurrentGroup, sizeof(GROUPSTORE));
    break;

  case GENERAL_GROUPS:
    CopyMemory(&gpGroupStoreGeneral[iGroup],  &g_CurrentGroup, sizeof(GROUPSTORE));
    break;
  }

  return bRet;
}

////////////////////////////////////////////////////
//  GroupChannel ->works only on the g_CurrentGroup 
//
//
BOOL    GroupChannel(int iChannel, int iControl)
{
	int	i;
	int	type = -1;

  if(iChannel >= MAX_CHANNELS)  
    return FALSE;

	// scan for any other grouped channels
	// and remember their type
	//
	for (i=0 ; i < MAX_CHANNELS; i ++)
	{
		if (g_CurrentGroup.Group[i].iActive > 0)
			type = gDeviceSetup.iaChannelTypes[g_CurrentGroup.Group[i].iChannel]; 
	}

	if (type != -1 && type != gDeviceSetup.iaChannelTypes[iChannel])
		return FALSE;


	g_CurrentGroup.Group[iChannel].iChannel = iChannel;
  g_CurrentGroup.Group[iChannel].iActive = 1;
  g_CurrentGroup.Group[iChannel].iControl = iControl;

  return TRUE;
}

//////////////////////////////////////////////////////
//  UnGroupChannel ->works only on the g_CurrentGroup 
//
//
BOOL    UnGroupChannel(int iChannel)
{
  if(iChannel >= MAX_CHANNELS)  
    return FALSE;


  g_CurrentGroup.Group[iChannel].iActive = 0;
  g_CurrentGroup.Group[iChannel].iControl = -1; // Invalid control

  return TRUE;
}

///////////////////////////////////////////////////////
// Upadte Grouped Controls 
//
//
//
void   UpdateGroupedControls(CONTROLDATA *pCtrlData, LPCTRLZONEMAP pctrlzm, 
                             int iDelta, LPMIXERWNDDATA lpmwd, BOOL	skipSendingData)
{
  int             iCount;
  int             iV, val_original;
  CTRLZONEMAP     ctrlZM;
  WORD            wChannel;
  CONTROLDATA     CtrlDataCopy ; 

  if(IsGrouped(pCtrlData->wChannel))
  {
    wChannel = pCtrlData->wChannel;
    // loop through all channels and send the controll to all others
    //--------------------------------------------------------------
    for(iCount = 0; iCount < MAX_CHANNELS; iCount++)
    {
      if(iCount != wChannel)
        if(IsGrouped(iCount))
        {
          CtrlDataCopy = *pCtrlData;
          ctrlZM = *pctrlzm;

          if(IsMuteFilter(pctrlzm))
          {
            // Skip over the Mutes......
            // the Grouped Mutes are going to be handled in
            // StartControlDataFilter function ...
            //ctrlZM.iModuleNumber = iCount;
            //HandleCtrlBtnClickInGroup(NULL, lpmwd, &ctrlZM);
          }
          else
          {
            // Send the data value to the Device
            // and then update the controls 
            //
            // Use a copy of these because the data
            // will get corrupted ....
            CtrlDataCopy.wChannel = iCount;
            ctrlZM.iModuleNumber = iCount;

						val_original = GETPHISDATAVALUE(0, (&ctrlZM), ctrlZM.iCtrlChanPos);
            iV =  val_original + iDelta;
            if(iV < 0)
              iV = 0;
            if(iV >= ctrlZM.iNumValues)
              iV = ctrlZM.iNumValues - 1;

            CtrlDataCopy.wVal = (WORD)iV;
            if(CheckFilter(&ctrlZM) == NO_FILTER && skipSendingData == FALSE && val_original != iV)
            {
              // Check if it is a software control and 
              // deal with it in a special way ... 

							CDef_SendData(&CtrlDataCopy); // Send the data only if not filtered
							UpdateExternalInterface(&CtrlDataCopy);
            }
            UpdateControlFromNetwork((WORD)iCount, CtrlDataCopy.wCtrl, (int)CtrlDataCopy.wVal, TRUE);

						if(pctrlzm->iCtrlType == CTRL_TYPE_INPUT_GATE_IN_BTN_FILTER){
							if(pCtrlData->wVal == 0)
								StartControlDataFilter(iCount, lpmwd, pctrlzm, TRUE, FALSE);
							else
								StartControlDataFilter(iCount, lpmwd, pctrlzm, FALSE, FALSE);
						}

            // This might be a stereo control so  try to update it properly
            //
            UpdateStereoControls(&CtrlDataCopy, &ctrlZM, 0, lpmwd);
          }
        }
    }
  }
};

///////////////////////////////////////////////////////
// Update Grouped Mutes 
//
//
//
void   UpdateGroupedMutes(LPCTRLZONEMAP pctrlzm, LPMIXERWNDDATA lpmwd)
{
  int             iCount;
  //int             iV;
  CTRLZONEMAP     ctrlZM;
  WORD            wChannel;

  // Check if the Grouping function is disabled for the moment...
  if(lpmwd->wKeyFlags & MK_SHIFT)
    return;
  
  if(IsMuteFilter(pctrlzm) == FALSE)
    return;



  if(IsGrouped(pctrlzm->iModuleNumber))
  {
    wChannel = pctrlzm->iModuleNumber;
    // loop through all channels and send the controll to all others
    //--------------------------------------------------------------
    for(iCount = 0; iCount < MAX_CHANNELS; iCount++)
    {
      if(iCount != wChannel)
        if(IsGrouped(iCount))
        {
          ctrlZM = *pctrlzm;

          // Skip over the Mutes......
          // the Grouped Mutes are going to be handled in
          // StartControlDataFilter function ...
          ctrlZM.iModuleNumber = iCount;
          HandleCtrlBtnClickInGroup(NULL, lpmwd, &ctrlZM, iCount); // ??
        }
    }
  }
};


///////////////////////////////////////////////////////
// Upadte Stereo Controls 
//
//
//
void   UpdateStereoControls(CONTROLDATA *pCtrlData, LPCTRLZONEMAP pctrlzm, 
                            int iDelta, LPMIXERWNDDATA lpmwd)
{
  // loop through all predefined stereo groups and adjust the controls if needed
  //
  LPSTEREOGROUP       pSG = g_StereoGroups;
  int                 iModuleType  = gDeviceSetup.iaChannelTypes[pCtrlData->wChannel];
  WORD                wUpdate;
  CTRLZONEMAP         *pctrl;
  int                 iV;
  int                 iModule;

  if( (lpmwd == NULL) || (pctrlzm == NULL) )
    return;

  while(pSG->wLeft != pSG->wRight)
  {
    // make sure we have the same channel type !!! Then do the rest.
    if(pSG->iModuleType == iModuleType)
		{
      if((pSG->wLeft == pctrlzm->iCtrlChanPos) || (pSG->wRight == pctrlzm->iCtrlChanPos))
      {
        // good .. we have a stereo control. Now lets update its counterpart
        // but which one is it?!
        if(pSG->wLeft == pctrlzm->iCtrlChanPos)
          wUpdate = pSG->wRight;
        else
          wUpdate = pSG->wLeft;
      
        // Send the data value to the Device
        // and then update the controls 
        //                                        
        iModule = lpmwd->lpwRemapToScr[lpmwd->iCurChan + lpmwd->iStartScrChan];
        pctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iModule].lpZoneMap, 
                                wUpdate);


        pCtrlData->wCtrl = pctrl->iCtrlNumAbs;

        if(g_bReversDirection)
          iDelta = -iDelta;

        iV = GETPHISDATAVALUE(0, pctrl, pctrl->iCtrlChanPos) + iDelta;
        if(iV < 0)
          iV = 0;
        if(iV >= pctrl->iNumValues)
          iV = pctrl->iNumValues - 1;

        pCtrlData->wVal = (WORD)iV;
        if(CheckFilter(pctrl) == NO_FILTER)
        {
          // Check if it is a software control and 
          // deal with it in a special way ... 
					CDef_SendData(pCtrlData); // Send the data only if not filtered
					UpdateExternalInterface(pCtrlData);
        }
        else
        {
          SETPHISDATAVALUE(0, pctrl, pctrl->iCtrlChanPos, (WORD)iV);
        }
        if(iModule == g_iMasterModuleIdx)
          UpdateControlFromNetwork((WORD)iModule, (WORD)pctrl->iCtrlChanPos, (int)pCtrlData->wVal, FALSE);
        else
          UpdateControlFromNetwork(pCtrlData->wChannel, (WORD)pctrl->iCtrlChanPos, (int)pCtrlData->wVal, FALSE);
      }
		}
    pSG ++;
  }

};


/////////////////////////////////
//FUNCTION: HandleCtrlBtnClickInGroup
//
//NOTE: This takes care of the Mute buttons ...
//      Since they've got to do more things
//      than a normal buttons
//
void      HandleCtrlBtnClickInGroup(HWND hwnd, LPMIXERWNDDATA lpmwd, LPCTRLZONEMAP pctrlzm, int phis_channel)
{
LPCTRLZONEMAP       lpctrlZM;
CONTROLDATA         ctrlData;
int                 iPhisChannel, iVal, ivalue;
BOOL                bIsOn;


if(hwnd == NULL)
  hwnd = lpmwd->hwndImg; // Grab the image Window from the MixerWindow data

// Handle the button Up down
//--------------------------
//iPhisChannel = pctrlzm->iModuleNumber;
iPhisChannel = phis_channel;

// get the zone map pointer
//-------------------------
lpctrlZM = pctrlzm;

ivalue = CDef_GetCtrlMinVal(lpctrlZM->iCtrlNumAbs);

iVal = GETPHISDATAVALUE(0, lpctrlZM, lpctrlZM->iCtrlChanPos);

if(iVal != ivalue)
  {
  bIsOn = TRUE;

  // Do the Toggle stuff ..
  //
  //CheckForToggleSwitches(lpmwd, lpctrlZM);

  // Handle possible Filter buttons ... like MUTE and such
  //
  StartControlDataFilter(iPhisChannel, lpmwd, lpctrlZM, bIsOn, FALSE);

  ctrlData.wMixer   = lpmwd->iMixer;
  ctrlData.wChannel = lpctrlZM->iModuleNumber;//iPhisChannel;
  ctrlData.wCtrl    = lpctrlZM->iCtrlNumAbs; // we use this one since for the definition dll
  // ok we need to go down to the minimum value
  iVal = CDef_GetCtrlMaxVal(lpctrlZM->iCtrlNumAbs) - 1;
  iVal--;// skip this value since the control is already there
  if(((IsCtrlPrePostFilter(lpctrlZM->iCtrlType) == FALSE)
    &&  lpctrlZM->iCtrlType != CTRL_TYPE_BTN_AUXMUTE_FILTER 
    &&  ctrlData.wCtrl < 0x8000))
  {
    for(iVal; iVal >= ivalue; iVal --)
      {
      // Send the Data out
      //------------------
      ctrlData.wVal     = iVal;
      SendDataToDevice(&ctrlData, FALSE, lpctrlZM, -1, lpmwd, TRUE);
      }
  }
  iVal = ivalue;
  }
else
  {
  bIsOn = FALSE;

  ctrlData.wMixer   = lpmwd->iMixer;
  ctrlData.wChannel = lpctrlZM->iModuleNumber;//iPhisChannel;
  ctrlData.wCtrl    = lpctrlZM->iCtrlNumAbs; // we use this one since for the definition dll
  // ok we need to go up to max value
  ivalue = CDef_GetCtrlMaxVal(lpctrlZM->iCtrlNumAbs);
  iVal   = CDef_GetCtrlMinVal(lpctrlZM->iCtrlNumAbs);
  iVal++;// skip this value since the control is already there
  if((IsCtrlPrePostFilter(lpctrlZM->iCtrlType) == FALSE) 
    &&  lpctrlZM->iCtrlType != CTRL_TYPE_BTN_AUXMUTE_FILTER 
    && (ctrlData.wCtrl < 0x8000))
  {
    for(iVal; iVal < ivalue; iVal++)
      {
      // Send the Data out
      //------------------
      ctrlData.wVal     = iVal;
      SendDataToDevice(&ctrlData, FALSE, lpctrlZM, 1, lpmwd, TRUE);
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
  StartControlDataFilter(iPhisChannel, lpmwd, lpctrlZM, bIsOn, FALSE);
  }

// Set the Phisical Data Value
//----------------------------
SETPHISDATAVALUE(lpmwd->iMixer, lpctrlZM, lpctrlZM->iCtrlChanPos, iVal);

if(iVal == CDef_GetCtrlMinVal(lpctrlZM->iCtrlNumAbs))
  {
  UpdateSameMixWndByCtrlNum(hwnd, lpmwd->iMixer, iPhisChannel, lpctrlZM, iVal, NULL);
  }
else
  {

  // now update all of the other mixers
  // windows that represent this mixer
  // using the iMixer, iPhisChannel
  // and iVal
  //-----------------------------------
  UpdateSameMixWndByCtrlNum(hwnd, lpmwd->iMixer, iPhisChannel, lpctrlZM, iVal, g_hdcBuffer);

  }

//UpdateControlFromNetwork((WORD)iPhisChannel, ctrlData.wCtrl, iVal, TRUE);
UpdateControlFromNetwork((WORD)iPhisChannel, (WORD)lpctrlZM->iCtrlChanPos, iVal, FALSE);
return;
}

