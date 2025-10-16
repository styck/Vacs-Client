//=================================================
// Copyright 1998-2002, CorTek Software, Inc.
//=================================================
//
// $Author: Styck $
// $Archive: /Vacs Client/src/Remap and Mode.c $
// $Revision: 21 $
//
//=================================================
//
//Remap Functions
// and Mode Functions
//
//=================================================

#include "SAMM.h"
#include "SAMMEXT.h"
#include "consoledefinition.h"

UINT g_CtrlTimer;
extern BOOL	g_bUpDownScollSpeedFAST;	// SEE MAIN.C

//=================================================
//FUNCTION: InitRemapTable
//
//=================================================
int InitRemapTable(LPMIXERWNDDATA lpmwd)
{
    int iCount;

    for (iCount = 0; iCount < MAX_CHANNELS; iCount++)
    {
        lpmwd->lpwRemapToScr[iCount] = iCount;
        lpmwd->lpwRemapToPhis[iCount] = iCount;
    }
    return 0;
}

//=========================================================================
//FUNCTION: RemapScrChannel
//
//params:
//      LPMIXERWNDDATA lpmwd; the memory storage
//      int     iScrFrom; the Screen from channel
//      int     iScrTo; the Screen to channel
//
//purpose:
//      Remaps a given Screen Channel
//  into the RemapTable .
//
// the logic is this:
//
// array item -> 1, 2, 3, 4, 5, 6, .... the actual screen channels
// array value-> 1, 2, 3, 4, 5, 6, .... the actual physical channel that is
//                                      represenred
//
//=========================================================================
int RemapScrChannel(LPMIXERWNDDATA lpmwd, int iScrFrom, int iScrTo)
{
    int iCount;
    int iPhis;

    // Check for incorrect values
    //-----------------------------
    if ((iScrFrom > giMax_CHANNELS) || (iScrTo > giMax_CHANNELS) ||
        (iScrFrom < 0) || (iScrTo < 0))
        return 1;

    if (iScrFrom > iScrTo)
    {
        iPhis = lpmwd->lpwRemapToScr[iScrFrom];
        for (iCount = iScrFrom; iCount > iScrTo; iCount--)
        {
            lpmwd->lpwRemapToScr[iCount] = lpmwd->lpwRemapToScr[iCount - 1];
        }
        lpmwd->lpwRemapToScr[iScrTo] = iPhis;
    }
    else if (iScrFrom < iScrTo)
    {
        iPhis = lpmwd->lpwRemapToScr[iScrFrom];
        for (iCount = iScrFrom; iCount < iScrTo; iCount++)
        {
            lpmwd->lpwRemapToScr[iCount] = lpmwd->lpwRemapToScr[iCount + 1];
        }
        lpmwd->lpwRemapToScr[iScrTo] = iPhis;
    }

    RemapPhisFromScr(lpmwd);

    return 0;
}


//==================================================
//FUNCTION: RemapPhisFromScr
//
//params:
//      LPMIXERWNDDATA lpmwd; the memory storage
//
//purpouse:
//      Remaps all Physical Channel
//  into the RemapTable
//
// the logic is this:
//
// array item -> 1, 2, 3, 4, 5, 6, .... the actual physical channels
// array value-> 1, 2, 3, 4, 5, 6, .... the actual screen channel that is
//                                      represenred
//
//=========================================================================
int RemapPhisFromScr(LPMIXERWNDDATA lpmwd)
{
    int iCount;
    int iPhis;

    for (iCount = 0; iCount < giMax_CHANNELS; iCount++)
    {
        // Get the physical channel that is shown on the screen
        // at position iCount
        //-----------------------------------------------------
        iPhis = LOWORD(lpmwd->lpwRemapToScr[iCount]);
        // map the physical channel to the
        // screen channel at position iCount
        //----------------------------------
        lpmwd->lpwRemapToPhis[iPhis] = iCount;
    }

    return 0;
}

//=================================================
//FUNCTION: GetMWScrChanNum
//
//return:
//      int screen channel number
//
//params:
//      LPMIXERWNDDATA lpmwd; the mixer window data
//
//=================================================
int GetMWScrChanNum(LPMIXERWNDDATA lpmwd)
{
    int iCount;
    int iPhisChannel, iBMPIndex;
    LONG lZMCount;
    int iCurX, iX, iWidth;
    int iStartScrChan, iEndScrChan;

    if (lpmwd == NULL)
        return 0;

    iStartScrChan = lpmwd->iStartScrChan;
    iEndScrChan = lpmwd->iEndScrChan;

    // Highlite only the first visible channel
    //----------------------------------------
    if (lpmwd->pntMouseCur.x < 0)
    {
        lpmwd->iXadj = 0;
        return lpmwd->iStartScrChan;
    }

    // Highlite only the last visible channel
    //---------------------------------------
    if (lpmwd->pntMouseCur.x >= lpmwd->rVisible.right)
    {
        iPhisChannel = LOWORD(lpmwd->lpwRemapToScr[lpmwd->iEndScrChan]);
        iBMPIndex = lpmwd->lpZoneMap[iPhisChannel].iBmpIndx;
        lpmwd->iXadj = lpmwd->rVisible.right - gpBMPTable[iBMPIndex].iWidth;
        return lpmwd->iEndScrChan;
    }

    iCurX = lpmwd->pntMouseCur.x + lpmwd->iXOffset;
    lZMCount = lpmwd->lZMCount;
    iX = 0;

    for (iCount = 0; iCount < lZMCount; iCount++)
    {
        // Get the actual phis channel
        // because they might be remaped
        //------------------------------
        iPhisChannel = LOWORD(lpmwd->lpwRemapToScr[iCount]);
        iBMPIndex = lpmwd->lpZoneMap[iPhisChannel].iBmpIndx;
        iWidth = gpBMPTable[iBMPIndex].iWidth - 1;

        if ((iCurX >= iX) && (iCurX <= (iX + iWidth)))
        {
            lpmwd->iXadj = iX;
            return iCount;
        }

        iX += iWidth + 1;
    }

    return (int)lZMCount; //iCount-1;
}

//=================================================
//FUNCTION: GetMWScrChanRect
//
//
//params:
//      LPMIXERWNDDATA lpmwd; the mixer window data
//      int            iChan;
//      RECT           *rect;
//
//=================================================
int GetMWScrChanRect(LPMIXERWNDDATA lpmwd, int iChan, RECT* rect)
{
    int iCount;
    int iPhisChannel;
    int iX, iWidth;
    int iBMPIndex;

    if (iChan > lpmwd->lZMCount)
    {
        rect->left = rect->top = rect->right = rect->bottom = 0;
        return 1;
    }

    iX = 0;

    for (iCount = 0; iCount < iChan; iCount++)
    {
        // Get the actual phis channel
        // because they might be remaped
        //------------------------------
        iPhisChannel = LOWORD(lpmwd->lpwRemapToScr[iCount]);
        iBMPIndex = lpmwd->lpZoneMap[iPhisChannel].iBmpIndx;
        iWidth = gpBMPTable[iBMPIndex].iWidth;

        iX += iWidth;
    }

    // Get the Height of the bitmap
    //------------------------------
    iPhisChannel = LOWORD(lpmwd->lpwRemapToScr[iChan]);
    iBMPIndex = lpmwd->lpZoneMap[iPhisChannel].iBmpIndx;
    iWidth = gpBMPTable[iBMPIndex].iWidth;
    rect->bottom = gpBMPTable[iBMPIndex].iHeight;

    rect->top = 0;
    rect->left = iX - lpmwd->iXOffset;
    rect->right = iX - lpmwd->iXOffset + iWidth;

    return 0;
}

//////////////////////////////////////////////////////////
// FUNCTION:  MakeModulesArraySameType
//
// purpose: Adjust the Modules Array Table to include only 
//          ONE TYPE OF MODULES
//
//
BOOL MakeModulesArraySameType(LPMIXERWNDDATA lpmwd, int iType, int iType2)
{
    int iScrCount = 0;
    int iNewPosition = 0;
    int iPhisChannel = 0;

    lpmwd->lZMCount = 0;
    for (iScrCount = 0; iScrCount < giMax_CHANNELS; iScrCount++)
    {

        // Move the Channel from position X ... to position Y
        //
        iPhisChannel = lpmwd->lpwRemapToPhis[iScrCount];
        if (gDeviceSetup.iaChannelTypes[iPhisChannel] == iType)
        {
            RemapScrChannel(lpmwd, iScrCount, iNewPosition++);
            lpmwd->lZMCount++;
        }
    }

    if (iType2 != NULL_MODULE_TYPE)
    {
        for (iScrCount = 0; iScrCount < giMax_CHANNELS; iScrCount++)
        {

            // Move the Channel from position X ... to position Y
            //
            iPhisChannel = lpmwd->lpwRemapToPhis[iScrCount];
            if (gDeviceSetup.iaChannelTypes[iPhisChannel] == iType2)
            {
                RemapScrChannel(lpmwd, iScrCount, iNewPosition++);
                lpmwd->lZMCount++;
            }
        }
    }
    return TRUE;
}

//=============================================
//FUNCTION: ActivateMWMode
//
//purpose:
//  Activate a Mixer Window Mode
//
//=============================================
int ActivateMWMode(HWND hwnd, LPMIXERWNDDATA lpmwd)
{
    int iMode;
    int iCtrlType;
    LPCTRLZONEMAP lpctrlZM;

    iMode = lpmwd->iCurMode;

    switch (iMode)
    {
    case MW_CONTROL_ACTIVE:
        iCtrlType = lpmwd->lpCtrlZM->iCtrlType;
        switch (iCtrlType)
        {
            //----------------------------------------
        case CTRL_TYPE_FADER_VERT:
            SetCursor(LoadCursor(ghInstMain, MAKEINTRESOURCE(IDCUR_BLUE_HAND)));
            ClipCtrlZone(hwnd, lpmwd);
            lpmwd->iCtrlMode = CTRL_TYPE_FADER_VERT;
            break;
            //----------------------------------------
        case CTRL_TYPE_FADER_HORZ:
            ClipCtrlZone(hwnd, lpmwd);
            lpmwd->iCtrlMode = CTRL_TYPE_FADER_HORZ;
            break;
            //----------------------------------------
        case CTRL_TYPE_BUTTON_REG:
        case CTRL_TYPE_BTN_INPUTMUTE_FILTER:
            HandleCtrlBtnClick(hwnd, lpmwd);
            break;
        case CTRL_TYPE_BTN_AUXMUTE_FILTER:
            HandleCtrlBtnClick(hwnd, lpmwd);
            break;
        case CTRL_INPUT_AUX16B_MUTE_FILTER:	// LINE B TO AUX 16
        case CTRL_TYPE_INPUT_GATE_IN_BTN_FILTER:
        case CTRL_CUE_MASTER_MUTE_FILTER:
        case CTRL_MATRIX_MASTER_LT_MUTE_FILTER:
        case CTRL_MATRIX_MASTER_RT_MUTE_FILTER:
        case CTRL_MATRIX_MONO_MUTE_FILTER:
        case CTRL_MATRIX_CENTER_MUTE_FILTER:
        case CTRL_MATRIX_SUB_MUTE_FILTER:
        case CTRL_MATRIX_AUX_MUTE_FILTER:
        case CTRL_SUB_SUMIN_MUTE_FILTER:
        case CTRL_MASTER_AUX_MUTE_FILTER:
        case CTRL_MASTER_MUTE_FILTER:
        case CTRL_INPUT_MICA_MUTE_FILTER:
        case CTRL_INPUT_LINEA_MUTE_FILTER:
        case CTRL_INPUT_MICB_MUTE_FILTER:
        case CTRL_INPUT_LINEB_MUTE_FILTER:
        case CTRL_TYPE_BTN_EQ_RESET_FILTER:
            HandleCtrlBtnClick(hwnd, lpmwd);
            break;
            //-----------------------------------------
        case CTRL_TYPE_BTN_MASTER_HEADPOST_FILTER:
        case CTRL_TYPE_BTN_INPUT_AUXPOST_FILTER:
        case CTRL_TYPE_BTN_MASTER_AUX_PREPOST_FILTER:
        case CTRL_TYPE_BTN_AUX_MMATRIX_LT_POST_FILTER:
        case CTRL_TYPE_BTN_AUX_MMATRIX_RT_POST_FILTER:
            HandleCtrlBtnClick(hwnd, lpmwd);
            break;
        case CTRL_MARIXLT_MUTE_FILTER:
        case CTRL_MARIXRT_MUTE_FILTER:
            HandleCtrlBtnClick(hwnd, lpmwd);
            break;
            //----------------------------------------
        case CTRL_TYPE_OPEN_EXPLODE:
            break;
            //----------------------------------------
        case CTRL_TYPE_LEFTRIGHT:
            LeftRightControl(NULL, lpmwd->lpCtrlZM, 0, lpmwd, LOBYTE(lpmwd->lpwRemapToScr[lpmwd->iCurChan + lpmwd->iStartScrChan]));
            lpmwd->iCtrlMode = CTRL_TYPE_LEFTRIGHT;
            lpmwd->iCurMode = CTRL_TYPE_LEFTRIGHT;
            g_CtrlTimer = SetTimer(hwnd, 77, 150, NULL);
            break;
            //----------------------------------------
        case CTRL_TYPE_UPDOWN:
            UpDownControl(NULL, lpmwd->lpCtrlZM, 0, lpmwd, LOBYTE(lpmwd->lpwRemapToScr[lpmwd->iCurChan + lpmwd->iStartScrChan]));
            lpmwd->iCtrlMode = CTRL_TYPE_UPDOWN;
            lpmwd->iCurMode = CTRL_TYPE_UPDOWN;
            g_CtrlTimer = SetTimer(hwnd, 77, 150, NULL);
            break;
            //----------------------------------------
        case CTRL_TYPE_UPDOWNSCROLL:
            SetCursor(LoadCursor(ghInstMain, MAKEINTRESOURCE(IDCUR_UPDOWN_CTRL)));
            UpDownControl(NULL, lpmwd->lpCtrlZM, 0, lpmwd, (BYTE)LOWORD(lpmwd->lpwRemapToScr[lpmwd->iCurChan + lpmwd->iStartScrChan]));
            lpmwd->iCtrlMode = CTRL_TYPE_UPDOWNSCROLL;
            lpmwd->iCurMode = CTRL_TYPE_UPDOWNSCROLL;
            // Create time for the up/down scrolling of values
            // Changed to 500ms for the touchscreen

#define UPDOWNSCROLL_FAST	25	// 25ms per increment
#define UPDOWNSCROLL_SLOW	300	// 300ms per increment

            if (g_bUpDownScollSpeedFAST == TRUE)
                g_CtrlTimer = SetTimer(hwnd, 77, UPDOWNSCROLL_FAST, NULL);
            else
                g_CtrlTimer = SetTimer(hwnd, 77, UPDOWNSCROLL_SLOW, NULL);

            break;

            //----------------------------------------
        case CTRL_TYPE_LABEL:
            EditModuleLabel(hwnd, lpmwd, TRUE);
            lpmwd->iCtrlMode = CTRL_TYPE_LABEL;
            lpmwd->iCurMode = CTRL_TYPE_LABEL;
            break;

            //----------------------------------------
        case    CTRL_TYPE_OPEN_ZOOM_IAUX:
        case		CTRL_TYPE_OPEN_ZOOM_CCOMP:
        case		CTRL_TYPE_OPEN_ZOOM_EQ:
        case		CTRL_TYPE_OPEN_ZOOM_SUB:
        case		CTRL_TYPE_OPEN_FADER:
        case		CTRL_TYPE_OPEN_MATRIX_EQ:
        case		CTRL_TYPE_OPEN_MATRIX_AUX:
        case		CTRL_TYPE_OPEN_MATRIX_SUB:
            lpctrlZM = lpmwd->lpCtrlZM;
            JumpToMZWindow(NULL, lpctrlZM, 0, lpmwd, (BYTE)LOWORD(lpmwd->lpwRemapToScr[lpmwd->iCurChan + lpmwd->iStartScrChan]));
            StopMWMode(hwnd, lpmwd);
            break;
        }
        break;

    case MW_SCROLL:
        ClipZone(hwnd, &lpmwd->rVisible);
        SetCursor(LoadCursor(ghInstConsoleDef, MAKEINTRESOURCE(SAMMVSCROLLCURSOR)));
        break;
    case MW_SCROLL_RELATIVE:
        // fds it is already this value        lpmwd->iCurMode = MW_SCROLL_RELATIVE;
        ShowScrollWindow(hwnd, lpmwd);
        ClipZone(hwnd, &lpmwd->rVisible);
        ScrollImgWindowRelative(hwnd, lpmwd);
        SetCursor(LoadCursor(ghInstMain, MAKEINTRESOURCE(IDCUR_UPDOWN_SCROLLER)));
        g_CtrlTimer = SetTimer(hwnd, 77, 75, NULL);
        break;

    case MW_DRAGDROP_MODE:
        gddcChanInfo.iChan = lpmwd->iCurChan;
        gddcChanInfo.iMixer = lpmwd->iMixer;
        gddcChanInfo.bInUse = TRUE;
        ShowMousePos(hwnd, lpmwd);
        break;
    }
    return 0;
}

//=============================================
//FUNCTION: StopMWMode
//
//purpose:
//  Stop a Mixer Window Mode
//
//=============================================
int StopMWMode(HWND hwnd, LPMIXERWNDDATA lpmwd)
{
    int iOldMode;
    int iOldCtrlMode;

    iOldMode = lpmwd->iCurMode;
    iOldCtrlMode = lpmwd->iCtrlMode;

    // do stuff after the mode has been reset
    //---------------------------------------
    switch (iOldMode)
    {
        //----------------------------------------
    case MW_DRAGDROP_MODE:
        RemapScrChannel(lpmwd, gddcChanInfo.iChan + lpmwd->iStartScrChan,
            lpmwd->iCurChan + lpmwd->iStartScrChan);
        gddcChanInfo.bInUse = FALSE;
        // Update the Labels
        //------------------
        InvalidateRect(lpmwd->hwndLblGroup, NULL, FALSE);
        UpdateWindow(lpmwd->hwndLblGroup);
        // Update the Channel Display
        //---------------------------
        InvalidateRect(hwnd, NULL, FALSE);
        UpdateWindow(hwnd);
        lpmwd->iCurMode = MW_NOTHING_MODE;
        lpmwd->iCtrlMode = MW_NOTHING_MODE;
        ShowMousePos(hwnd, lpmwd);
        break;
        //----------------------------------------
    case MW_SCROLL:
    case CTRL_TYPE_FADER_VERT:
        DeleteObject(SetCursor(LoadCursor(NULL, IDC_ARROW)));
        break;
        //----------------------------------------
    case MW_SCROLL_RELATIVE:
        DeleteObject(SetCursor(LoadCursor(NULL, IDC_ARROW)));
        KillTimer(hwnd, g_CtrlTimer);
        g_CtrlTimer = 0;
        ShowScrollWindow(NULL, lpmwd);
        g_iYSpeed = 0;
        break;
    }

    switch (iOldCtrlMode)
    {
        //----------------------------------------
    case    CTRL_TYPE_OPEN_ZOOM_IAUX:
    case		CTRL_TYPE_OPEN_ZOOM_CCOMP:
    case		CTRL_TYPE_OPEN_ZOOM_EQ:
    case		CTRL_TYPE_OPEN_ZOOM_SUB:
    case		CTRL_TYPE_OPEN_FADER:
    case		CTRL_TYPE_OPEN_MATRIX_EQ:
    case		CTRL_TYPE_OPEN_MATRIX_AUX:
    case		CTRL_TYPE_OPEN_MATRIX_SUB:
        ReleaseCapture(); // Release the mouse capture in any case
        break;
    case    CTRL_TYPE_UPDOWN:
    case    CTRL_TYPE_LEFTRIGHT:
        KillTimer(hwnd, g_CtrlTimer);
        g_CtrlTimer = 0;
        break;
    case    CTRL_TYPE_UPDOWNSCROLL:
        DeleteObject(SetCursor(LoadCursor(NULL, IDC_ARROW)));
        KillTimer(hwnd, g_CtrlTimer);
        g_CtrlTimer = 0;
        break;
        //----------------------------------------
    case CTRL_TYPE_LABEL:
        EditModuleLabel(hwnd, lpmwd, FALSE);
        break;
    }

    lpmwd->iCurMode = MW_NOTHING_MODE;
    lpmwd->iCtrlMode = MW_NOTHING_MODE;

    ClipCursor(NULL);
    return 0;
}
