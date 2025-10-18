//=================================================
// Copyright 1997-2002, CorTek Software, Inc.
// Modernized to C++17 - 2025
//=================================================
//
// Memory map management with modern C++ RAII
// Replaces manual GlobalAlloc/GlobalFree with std::vector
//
/////////////////////////////////////////////

#include <math.h>
#include <stdio.h>
#include <memory>
#include <vector>
#include <algorithm>

// Wrap C headers in extern "C" for proper linkage
extern "C" {
#include "SAMM.h"
#include "SAMMEXT.h"
#include "MACRO.h"
#include "consoledefinition.h"
}

/////////////////////////////////////////////
// GLOBALS - Now using modern C++ containers

// Modern C++ containers (internal to this file)
std::vector<WORD> g_memMapMixer;
std::vector<WORD> g_memMapBuffer;
std::vector<WORD> g_memMapUpdateBufferMask;
std::vector<WORD> g_memMapUpdateBuffer;

// C linkage for globals that C code needs to access
extern "C" {
    int g_aiAux[MAX_MATRIX_COUNT];
    int g_aiMatrix[MAX_MATRIX_COUNT];
    int g_iCueModuleIdx = -1;
    int g_iAuxIdx = 0;
    int g_iMasterModuleIdx = -1;
    int giMemControlCount = 0;

    // Legacy pointers - these two are defined here, the other two in SAMM_Globals.c
    WORD* gpwMemMapUpdateBufferMask = nullptr;
    WORD* gpwMemMapUpdateBuffer = nullptr;
    extern WORD* gpwMemMapMixer;
    extern WORD* gpwMemMapBuffer;
}

/////////////////////////////////////////////
// PROTOTYPES - Functions implemented in other C files need extern "C"

extern "C" {
void HandleCtrlBtnClickInGroup(HWND hwnd, LPMIXERWNDDATA lpmwd, LPCTRLZONEMAP pctrlzm, int phis_channel);
BOOL IsCtrlPrePostFilter(int iType);
void SetFilteredControlsByNumber(LPCTRLZONEMAP lpctrl, int filtered);
void HadleCueMasterSystem();
BOOL isChanelSafeActive(LPCTRLZONEMAP pctrlzm);
void setBufferChannelSafeActive(LPCTRLZONEMAP pctrlzm, BOOL active);
}

//=========================================
// All function implementations with extern "C" for C code compatibility
//=========================================
extern "C" {

//=========================================
//FUNCTION: InitMemoryMap
//
// Modernized to use std::vector with RAII
//=========================================
int InitMemoryMap(void)
{
    try {
        giMemMapSize = sizeof(WORD) * giMax_MIXERS * giMax_CHANNELS * giMax_CONTROLS;
        giMemControlCount = giMax_MIXERS * giMax_CHANNELS * giMax_CONTROLS;

        // Use std::vector for automatic memory management
        size_t elementCount = giMax_MIXERS * giMax_CHANNELS * giMax_CONTROLS;
        
        g_memMapMixer.resize(elementCount, 0);
        g_memMapBuffer.resize(elementCount, 0);
        g_memMapUpdateBufferMask.resize(elementCount, 0xFFFF);
        g_memMapUpdateBuffer.resize(elementCount, 0);

        // Set legacy pointers for backward compatibility
        gpwMemMapMixer = g_memMapMixer.data();
        gpwMemMapBuffer = g_memMapBuffer.data();
        gpwMemMapUpdateBufferMask = g_memMapUpdateBufferMask.data();
        gpwMemMapUpdateBuffer = g_memMapUpdateBuffer.data();

        return 0;
    }
    catch (const std::bad_alloc&) {
        return IDS_ERR_ALLOCATE_MEMORY;
    }
}

//==================================================
//FUNCTION: FreeMemoryMap
//
// Simplified - vectors handle cleanup automatically
//==================================================
void FreeMemoryMap(void)
{
    // Clear vectors - automatic cleanup via RAII
    g_memMapMixer.clear();
    g_memMapBuffer.clear();
    g_memMapUpdateBufferMask.clear();
    g_memMapUpdateBuffer.clear();

    // Nullify legacy pointers
    gpwMemMapMixer = nullptr;
    gpwMemMapBuffer = nullptr;
    gpwMemMapUpdateBufferMask = nullptr;
    gpwMemMapUpdateBuffer = nullptr;
}

//////////////////////////////////////////////////////////////////////////
// FUNCTION: SetControlsModuleNumber
//////////////////////////////////////////////////////////////////////////
void  SetControlsModuleNumber(LPCTRLZONEMAP lpctrlZM, int iStartRange, int iEndRange, int iChannel)
{
    while (lpctrlZM->rZone.right > 0)
    {
        if (iEndRange > -1 && iStartRange > -1)
        {
            // set the control number only if it is within a range
            //
            if (lpctrlZM->iCtrlChanPos >= iStartRange && lpctrlZM->iCtrlChanPos <= iEndRange)
            {
                lpctrlZM->iModuleNumber = iChannel;
            }
        }
        else
        {
            // Set all Controls ...
            //
            lpctrlZM->iModuleNumber = iChannel;
        }
        // Bump the pointer
        lpctrlZM++;
    }

    return;
}

// Rest of the file continues with original implementation...
// (Keeping all other functions unchanged for now to maintain compatibility)

/////////////////////////////////////////////////////////////////////
// FUNCTION: InitPhysicalModuleMap
/////////////////////////////////////////////////////////////////////
int InitPhysicalModuleMap(void)
{
    int iRet = 0;
    int iCount;
    int iChannel;

    LPCTRLZONEMAP lpctrlZM_zoom;
    LPCTRLZONEMAP lpctrlZM_full;
    int iMatrixIdx = 0;

    g_iMasterModuleIdx = -1;	// Invalidate the master 
    g_iCueModuleIdx = -1;			// and cue modules index
    g_iAuxIdx = 0;

    ZeroMemory(g_aiMatrix, MAX_MATRIX_COUNT * sizeof(int));
    ZeroMemory(g_aiAux, MAX_MATRIX_COUNT * sizeof(int));

    for (iChannel = 0; iChannel < giMax_CHANNELS; iChannel++)
    {
        lpctrlZM_zoom = gpZoneMaps_Zoom[iChannel].lpZoneMap;
        lpctrlZM_full = gpZoneMaps_Full[iChannel].lpZoneMap;

        switch (gDeviceSetup.iaChannelTypes[iChannel])
        {
            // Handle the Input INPUT ...
        case DCX_DEVMAP_MODULE_INPUT:
            SetControlsModuleNumber(lpctrlZM_zoom, -1, -1, iChannel);
            SetControlsModuleNumber(lpctrlZM_full, -1, -1, iChannel);
            break;

            // Handle the AUX Modules ...
        case DCX_DEVMAP_MODULE_AUX:
            SetControlsModuleNumber(lpctrlZM_zoom, -1, -1, iChannel);
            SetControlsModuleNumber(lpctrlZM_full, -1, -1, iChannel);
            if (g_iAuxIdx >= MAX_MATRIX_COUNT)
                return IDS_INVALID_DEVICE_MAP_AUX_MANY;
            g_aiAux[g_iAuxIdx] = iChannel;
            g_iAuxIdx++;
            break;

            // Handle the MATRIX Modules ...
        case DCX_DEVMAP_MODULE_MATRIX:
            if (iMatrixIdx >= MAX_MATRIX_COUNT)
                return IDS_INVALID_DEVICE_MAP_MATRIX_MANY;
            g_aiMatrix[iMatrixIdx] = iChannel;
            iMatrixIdx++;
            break;

            // Handle the MASTER Modules ...
        case DCX_DEVMAP_MODULE_MASTER:
            if (g_iMasterModuleIdx == -1)
            {
                SetControlsModuleNumber(lpctrlZM_zoom, -1, -1, iChannel);
                SetControlsModuleNumber(lpctrlZM_full, -1, -1, iChannel);
                g_iMasterModuleIdx = iChannel;
            }
            else
                return IDS_INVALID_DEVICE_MASTER_MANY;
            break;

            // Handle the CUE Modules ...
        case DCX_DEVMAP_MODULE_CUE:
            if (g_iCueModuleIdx == -1)
                g_iCueModuleIdx = iChannel;
            else
                return IDS_INVALID_DEVICE_CUE_MANY;
            break;
        default:
            break;
        }
    }

    if (g_iMasterModuleIdx == -1)
        return IDS_INVALID_DEVICE_NO_MASTER;

    if (g_iAuxIdx != iMatrixIdx)
        return IDS_INVLAID_DEVICE_AUX_COMBO;

    for (iCount = 0; iCount < g_iAuxIdx; iCount++)
    {
        iChannel = g_aiAux[iCount];

        lpctrlZM_zoom = gpZoneMaps_Zoom[iChannel].lpZoneMap;
        lpctrlZM_full = gpZoneMaps_Full[iChannel].lpZoneMap;

        SetControlsModuleNumber(lpctrlZM_zoom, __SUB_MATRIX_GROUP_01_START,
            __SUB_MATRIX_GROUP_01_END, g_aiMatrix[iCount]);
        SetControlsModuleNumber(lpctrlZM_full, __SUB_MATRIX_GROUP_01_START,
            __SUB_MATRIX_GROUP_01_END, g_aiMatrix[iCount]);
    }

    SetControlsModuleNumber(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap,
        __MASTER_GROUP_01_START, __MASTER_GROUP_01_END, g_iMasterModuleIdx);

    SetControlsModuleNumber(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap,
        __MASTER_GROUP_02_START, __MASTER_GROUP_02_END, g_iCueModuleIdx);

    for (iCount = 0; iCount < MAX_MATRIX_COUNT; iCount++)
    {
        if (g_aiMatrix[iCount] != 0)
        {
            iChannel = g_aiAux[iCount];

            SetControlsModuleNumber(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap,
                CTRL_NUM_MASTER_AUX1LT_FADER - iCount * 4,
                CTRL_NUM_MASTER_AUX1LT_FADER - iCount * 4,
                iChannel);
            SetControlsModuleNumber(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap,
                CTRL_NUM_MASTER_AUX1RT_FADER - iCount * 4,
                CTRL_NUM_MASTER_AUX1RT_FADER - iCount * 4,
                iChannel);

            SetControlsModuleNumber(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap,
                CTRL_NUM_MASTER_AUX2LT_FADER - iCount * 4,
                CTRL_NUM_MASTER_AUX2LT_FADER - iCount * 4,
                iChannel);
            SetControlsModuleNumber(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap,
                CTRL_NUM_MASTER_AUX2RT_FADER - iCount * 4,
                CTRL_NUM_MASTER_AUX2RT_FADER - iCount * 4,
                iChannel);

            SetControlsModuleNumber(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap,
                CTRL_NUM_MASTER_SUMIN1 - iCount * 2,
                CTRL_NUM_MASTER_SUMIN1 - iCount * 2,
                iChannel);
            SetControlsModuleNumber(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap,
                CTRL_NUM_MASTER_SUMIN2 - iCount * 2,
                CTRL_NUM_MASTER_SUMIN2 - iCount * 2,
                iChannel);

            SetControlsModuleNumber(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap,
                CTRL_NUM_MASTER_AUX01PRE - iCount * 4,
                CTRL_NUM_MASTER_AUX01PRE - iCount * 4,
                iChannel);
            SetControlsModuleNumber(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap,
                CTRL_NUM_MASTER_AUX01POST - iCount * 4,
                CTRL_NUM_MASTER_AUX01POST - iCount * 4,
                iChannel);
            SetControlsModuleNumber(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap,
                CTRL_NUM_MASTER_AUX02PRE - iCount * 4,
                CTRL_NUM_MASTER_AUX02PRE - iCount * 4,
                iChannel);
            SetControlsModuleNumber(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap,
                CTRL_NUM_MASTER_AUX02POST - iCount * 4,
                CTRL_NUM_MASTER_AUX02POST - iCount * 4,
                iChannel);

            SetControlsModuleNumber(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap,
                CTRL_NUM_MASTER_CUE_AUX01_MUTE - iCount * 2,
                CTRL_NUM_MASTER_CUE_AUX01_MUTE - iCount * 2,
                iChannel);
            SetControlsModuleNumber(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap,
                CTRL_NUM_MASTER_CUE_AUX02_MUTE - iCount * 2,
                CTRL_NUM_MASTER_CUE_AUX02_MUTE - iCount * 2,
                iChannel);
        }
    }
    return iRet;
}

////////////////////////////////////////////
//FUNCTION: SetMemoryMapDefaults
////////////////////////////////////////////
int SetMemoryMapDefaults(void)
{
    int iChannel;
    LPCTRLZONEMAP lpctrlZM;
    LPCTRLZONEMAP lpctrl;
    int iMixerCount;
    int iValue;
    int iCtrlNum;
    int iCtrlAbs;

    for (iChannel = 0; iChannel < giMax_CHANNELS; iChannel++)
    {
        lpctrlZM = gpZoneMaps_Zoom[iChannel].lpZoneMap;

        if (lpctrlZM == NULL)
            continue;

        while (lpctrlZM->rZone.right)
        {
            iCtrlNum = lpctrlZM->iCtrlChanPos;
            iCtrlAbs = lpctrlZM->iCtrlNumAbs;

            if ((iCtrlNum != CTRL_NUM_NULL) && (iCtrlAbs > -1) && (lpctrlZM->iModuleNumber > -1))
            {
                iValue = CDef_GetCtrlDefaultVal(iCtrlAbs);

                if (lpctrlZM->iCtrlType == CTRL_TYPE_BTN_MASTER_HEADPOST_FILTER)
                    iValue = CDef_GetCtrlMaxVal(iCtrlAbs);

                for (iMixerCount = 0; iMixerCount < giMax_MIXERS; iMixerCount++)
                    SETPHISDATAVALUE(iMixerCount, lpctrlZM, iCtrlNum, iValue);

                if (gDeviceSetup.iaChannelTypes[lpctrlZM->iModuleNumber] == DCX_DEVMAP_MODULE_INPUT)
                {
                    lpctrl = ScanCtrlZonesNum(lpctrlZM, CTRL_NUM_INPUT_GATE_FEED_THRU);
                    if (lpctrl)
                        SetFilteredControlsByNumber(lpctrl, YES_FILTER);
                }
            }
            else
            {
                if ((iCtrlNum != CTRL_NUM_NULL) && (lpctrlZM->iModuleNumber > -1))
                {
                    if (((lpctrlZM->iCtrlChanPos == CTRL_NUM_INPUT_PREPOST_FADER_VU) &&
                         (gDeviceSetup.iaChannelTypes[lpctrlZM->iModuleNumber] == DCX_DEVMAP_MODULE_INPUT)) ||
                        ((lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_POST_LT_VU ||
                          lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_POST_RT_VU) &&
                         gDeviceSetup.iaChannelTypes[lpctrlZM->iModuleNumber] == DCX_DEVMAP_MODULE_MASTER) ||
                        ((lpctrlZM->iCtrlChanPos == CTRL_NUM_SUB_POST_LT_VU ||
                          lpctrlZM->iCtrlChanPos == CTRL_NUM_SUB_POST_RT_VU) &&
                         gDeviceSetup.iaChannelTypes[lpctrlZM->iModuleNumber] == DCX_DEVMAP_MODULE_MATRIX) ||
                        ((lpctrlZM->iCtrlChanPos == CTRL_NUM_MATRIX_POST_LT_VU ||
                          lpctrlZM->iCtrlChanPos == CTRL_NUM_MATRIX_POST_RT_VU) &&
                         gDeviceSetup.iaChannelTypes[lpctrlZM->iModuleNumber] == DCX_DEVMAP_MODULE_MATRIX) ||
                        (lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_CUEA_SYSTEM_SEL &&
                         gDeviceSetup.iaChannelTypes[lpctrlZM->iModuleNumber] == DCX_DEVMAP_MODULE_CUE))
                        iValue = 0;
                    else
                        iValue = 2;

                    for (iMixerCount = 0; iMixerCount < giMax_MIXERS; iMixerCount++)
                        SETPHISDATAVALUE(iMixerCount, lpctrlZM, iCtrlNum, iValue);
                }
            }
            lpctrlZM++;
        }
    }

    // Use std::copy for better performance with vectors
    std::copy(g_memMapMixer.begin(), g_memMapMixer.end(), g_memMapBuffer.begin());

    return 0;
}

/////////////////////////////////////////////////////////////////////
//  FUNCTION: RecallMemoryMapBuffer
/////////////////////////////////////////////////////////////////////
extern int g_CueMasterSystem;

void RecallMemoryMapBuffer(BOOL bForce, DWORD dwfadeDelay)
{
    CONTROLDATA ctrlData;
    int iChannel;
    LPCTRLZONEMAP lpctrlZM;
    int iValue;
    int iCtrlNum;
    int iCtrlAbs;
    int iCtrlAbsLast = 0;
    LPMIXERWNDDATA lpmwd;
    int filtered;
    DWORD copy_offset = 0;
    DWORD dwSystemtime, dwNowSystemTime;
    DWORD dwFadeDelayPerTick;

    LPCTRLZONEMAP lpctrlZMSave[128];
    int iSavedZM = 0;
    int iMaxFaderMovement;
    int i, j;
    int iValueCurrent, iValueSet, iDirection;
    char szBuff[80];

    int iModuleType;
    int Dummy;

    iMaxFaderMovement = -1;

    lpmwd = GetValidMixerWindowData();

    ClearProgress();
    SetProgressRange(0, giMax_CHANNELS);

    for (iChannel = 0; iChannel < giMax_CHANNELS; iChannel++)
    {
        StepProgress(1);
        lpctrlZM = gpZoneMaps_Zoom[iChannel].lpZoneMap;
        if (lpctrlZM == NULL)
            continue;

        iModuleType = gDeviceSetup.iaChannelTypes[lpctrlZM->iModuleNumber];

        setBufferChannelSafeActive(lpctrlZM, FALSE);

        if (isChanelSafeActive(lpctrlZM) == TRUE)
        {
            LPCTRLZONEMAP safecontrol = ScanCtrlZonesNum(lpctrlZM, CTRL_NUM_INPUT_SAFE);
            SETPHISDATAVALUEBUFFER(0, safecontrol, CTRL_NUM_INPUT_SAFE, 2);
            continue;
        }

        while (lpctrlZM->rZone.right)
        {
            iCtrlNum = lpctrlZM->iCtrlChanPos;
            iCtrlAbs = lpctrlZM->iCtrlNumAbs;

            if (((iCtrlNum != CTRL_NUM_NULL) && (iCtrlAbs != iCtrlAbsLast) &&
                (iCtrlAbs > -1) && (lpctrlZM->iModuleNumber < 80) &&
                (lpctrlZM->iModuleNumber >= 0)) || IsMuteFilter(lpctrlZM))
            {
                iValue = GETPHISDATAVALUEBUFFER(0, lpctrlZM, iCtrlNum);

                if (IsMuteFilter(lpctrlZM) == FALSE && IsCtrlPrePostFilter(lpctrlZM->iCtrlType) == FALSE)
                {
                    if (iValue != GETPHISDATAVALUE(0, lpctrlZM, iCtrlNum) || bForce == TRUE)
                    {
                        if (lpctrlZM->iCtrlType != CTRL_TYPE_FADER_VERT)
                        {
                            if ((iModuleType == DCX_DEVMAP_MODULE_INPUT) &&
                                ((lpctrlZM->iCtrlChanPos == CTRL_NUM_INPUT_GATE_KEY_VU) ||
                                 (lpctrlZM->iCtrlChanPos == CTRL_NUM_INPUT_LINE_B_VU) ||
                                 (lpctrlZM->iCtrlChanPos == CTRL_NUM_INPUT_CUE_FAD_PRE) ||
                                 (lpctrlZM->iCtrlChanPos == CTRL_NUM_INPUT_CUE_FAD_POST) ||
                                 (lpctrlZM->iCtrlChanPos == CTRL_NUM_INPUT_MIC_A_CUE) ||
                                 (lpctrlZM->iCtrlChanPos == CTRL_NUM_INPUT_MIC_B_CUE) ||
                                 (lpctrlZM->iCtrlChanPos == CTRL_NUM_INPUT_GATE_KEY_INOUT)))
                            {
                                Dummy = 0;
                            }
                            else if ((iModuleType == DCX_DEVMAP_MODULE_MASTER) &&
                                     ((lpctrlZM->iCtrlChanPos >= CTRL_NUM_MASTER_AUX01PRE && 
                                       lpctrlZM->iCtrlChanPos <= CTRL_NUM_MASTER_AUX16POST) ||
                                      (lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_CUE_LEVEL_MONO) ||
                                      (lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_CUE_LEVEL_CENTER) ||
                                      (lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_STEREO_CUE_PRE) ||
                                      (lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_STEREO_CUE_POST)))
                            {
                                Dummy = 0;
                            }
                            else if ((iModuleType == DCX_DEVMAP_MODULE_MATRIX) &&
                                     ((lpctrlZM->iCtrlChanPos == CTRL_NUM_MATRIX_STERIO_CUE_PRE) ||
                                      (lpctrlZM->iCtrlChanPos == CTRL_NUM_MATRIX_STERIO_CUE_POST) ||
                                      (lpctrlZM->iCtrlChanPos == CTRL_NUM_MATRIX_CUE_PRE) ||
                                      (lpctrlZM->iCtrlChanPos == CTRL_NUM_MATRIX_CUE_POST)))
                            {
                                Dummy = 0;
                            }
                            else
                            {
                                SETPHISDATAVALUE(0, lpctrlZM, iCtrlNum, iValue);

                                if (lpctrlZM->iFiltered == NO_FILTER)
                                {
                                    ctrlData.wMixer = 0;
                                    ctrlData.wChannel = lpctrlZM->iModuleNumber;
                                    ctrlData.wCtrl = iCtrlAbs;
                                    ctrlData.wVal = iValue;
                                    SendDataToDevice(&ctrlData, FALSE, NULL, 0, NULL, FALSE);
                                }
                            }
                        }
                        else if (lpctrlZM->iCtrlType == CTRL_TYPE_FADER_VERT)
                        {
                            if ((iModuleType == DCX_DEVMAP_MODULE_MASTER) &&
                                ((lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTERLT_CUE_A_LT) ||
                                 (lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTERLT_CUE_A_RT) ||
                                 (lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTERLT_CUE_B_LT) ||
                                 (lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTERLT_CUE_B_RT)))
                            {
                                Dummy = 0;
                            }
                            else
                            {
                                if (dwfadeDelay)
                                {
                                    lpctrlZMSave[iSavedZM++] = lpctrlZM;
                                    if (abs(GETPHISDATAVALUE(0, lpctrlZM, iCtrlNum) - iValue) > iMaxFaderMovement)
                                        iMaxFaderMovement = abs(GETPHISDATAVALUE(0, lpctrlZM, iCtrlNum) - iValue);
                                }
                                else
                                {
                                    SETPHISDATAVALUE(0, lpctrlZM, iCtrlNum, iValue);

                                    if (lpctrlZM->iFiltered == NO_FILTER)
                                    {
                                        ctrlData.wMixer = 0;
                                        ctrlData.wChannel = lpctrlZM->iModuleNumber;
                                        ctrlData.wCtrl = iCtrlAbs;
                                        ctrlData.wVal = iValue;
                                        SendDataToDevice(&ctrlData, FALSE, NULL, 0, NULL, FALSE);
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    if (iValue != GETPHISDATAVALUE(0, lpctrlZM, iCtrlNum) || bForce == TRUE)
                    {
                        if (bForce)
                        {
                            iValue = GETPHISDATAVALUE(0, lpctrlZM, iCtrlNum);
                            if (iValue)
                                SETPHISDATAVALUE(0, lpctrlZM, iCtrlNum, 0);
                            else
                                SETPHISDATAVALUE(0, lpctrlZM, iCtrlNum, 2);
                        }

                        if ((lpctrlZM->iCtrlType == CTRL_MARIXLT_MUTE_FILTER) ||
                            (lpctrlZM->iCtrlType == CTRL_MARIXRT_MUTE_FILTER))
                        {
                            if (lpmwd)
                            {
                                lpmwd->lpCtrlZM = lpctrlZM;
                                lpmwd->iCurChan = iChannel;
                                HandleCtrlBtnClick(NULL, lpmwd);
                            }
                        }
                        else
                        {
                            if (lpmwd)
                                HandleCtrlBtnClickInGroup(NULL, lpmwd, lpctrlZM, iChannel);
                        }
                    }
                }

                iCtrlAbsLast = iCtrlAbs;
            }

            lpctrlZM++;
        }

        lpctrlZM = gpZoneMaps_Zoom[iChannel].lpZoneMap;
        lpctrlZM = ScanCtrlZonesNum(lpctrlZM, CTRL_NUM_INPUT_GATE_FEEDTHRUINOUT);

        if (lpctrlZM != NULL &&
            gDeviceSetup.iaChannelTypes[lpctrlZM->iModuleNumber] == DCX_DEVMAP_MODULE_INPUT)
        {
            filtered = lpctrlZM->iFiltered;

            if (GETPHISDATAVALUE(0, lpctrlZM, lpctrlZM->iCtrlChanPos) == CDef_GetCtrlDefaultVal(lpctrlZM->iCtrlNumAbs))
            {
                ctrlData.wMixer = 0;
                ctrlData.wChannel = lpctrlZM->iModuleNumber;
                ctrlData.wCtrl = lpctrlZM->iCtrlNumAbs;
                ctrlData.wVal = CDef_GetCtrlDefaultVal(lpctrlZM->iCtrlNumAbs);

                if (lpctrlZM->iFiltered == NO_FILTER)
                {
                    lpctrlZM = gpZoneMaps_Zoom[iChannel].lpZoneMap;
                    lpctrlZM = ScanCtrlZonesNum(lpctrlZM, CTRL_NUM_INPUT_GATE_FEED_THRU);
                    SetFilteredControlsByNumber(lpctrlZM, YES_FILTER);
                }
            }
            else
            {
                lpctrlZM = gpZoneMaps_Zoom[iChannel].lpZoneMap;
                lpctrlZM = ScanCtrlZonesNum(lpctrlZM, CTRL_NUM_INPUT_GATE_FEED_THRU);

                ctrlData.wMixer = 0;
                ctrlData.wChannel = lpctrlZM->iModuleNumber;
                ctrlData.wCtrl = lpctrlZM->iCtrlNumAbs;
                ctrlData.wVal = GETPHISDATAVALUEBUFFER(0, lpctrlZM, lpctrlZM->iCtrlChanPos);
                if (filtered == NO_FILTER)
                {
                    SetFilteredControlsByNumber(lpctrlZM, NO_FILTER);
                    SendDataToDevice(&ctrlData, FALSE, NULL, 0, NULL, FALSE);
                }
            }
        }
        copy_offset = giMax_CONTROLS * iChannel;
    }

    if ((iSavedZM != 0) && (dwfadeDelay))
    {
        dwFadeDelayPerTick = dwfadeDelay / (iMaxFaderMovement / iSavedZM);
#ifdef _DEBUG
        sprintf(szBuff, "             dwFadeDelayPerTick=%d   dwfadeDelay=%d ", dwFadeDelayPerTick, dwfadeDelay);
        SendMessage(ghwndStatus, SB_SETTEXT, MAKEWPARAM(0, SBT_POPOUT), (LPARAM)szBuff);
#endif

        for (j = 0; j < iMaxFaderMovement; j++)
        {
            dwNowSystemTime = timeGetTime();

            for (i = 0; i < iSavedZM; i++)
            {
                lpctrlZM = lpctrlZMSave[i];

                iCtrlNum = lpctrlZM->iCtrlChanPos;
                iCtrlAbs = lpctrlZM->iCtrlNumAbs;

                iValue = GETPHISDATAVALUEBUFFER(0, lpctrlZM, iCtrlNum);
                iValueCurrent = GETPHISDATAVALUE(0, lpctrlZM, iCtrlNum);

                if (iValue != iValueCurrent)
                {
                    if (iValue > iValueCurrent)
                        iDirection = 1;
                    else
                        iDirection = -1;

                    iValueSet = iValueCurrent;
                    iValueSet = iValueSet + iDirection;

                    if ((iDirection * iValueSet) <= iValue)
                    {
                        ctrlData.wMixer = 0;
                        ctrlData.wChannel = lpctrlZM->iModuleNumber;
                        ctrlData.wCtrl = iCtrlAbs;
                        ctrlData.wVal = iValueSet;
                        SendDataToDevice(&ctrlData, FALSE, NULL, 0, NULL, FALSE);

                        UpdateControlFromNetwork(ctrlData.wChannel, (WORD)lpctrlZM->iCtrlChanPos, (int)ctrlData.wVal, FALSE);

                        do
                        {
                            dwSystemtime = timeGetTime();
                        } while (dwFadeDelayPerTick > dwSystemtime - dwNowSystemTime);
                    }
                }
            }
        }
    }

    RefreshAllMDIWindows();
    ClearProgress();

    g_CueMasterSystem = 0;
    HadleCueMasterSystem();
}

//=========================================
//FUNCTION: PhisDataToScrPos
//=========================================
int PhisDataToScrPos(LPCTRLZONEMAP lpctrlZM, int iIndx, int iVal)
{
#ifdef MIDI_SUPPORT
    LPMIDITOSCRTABLE lpMIDITOSCR;
    LPWORD lpData;
    int iReturn;

    if ((iVal < 0) || (iIndx < 0))
        return 0;

    lpMIDITOSCR = (LPMIDITOSCRTABLE)gpBinResTable[iIndx].lpBinRes;

    if (lpMIDITOSCR->iTotVals <= iVal)
        iVal = lpMIDITOSCR->iMax;

    lpData = (LPWORD)((LPSTR)lpMIDITOSCR + sizeof(MIDITOSCRTABLE));
    lpData = lpData + iVal;
    iReturn = (int)*lpData;
    return iReturn;
#else
    return iVal;
#endif
}

//=========================================
//FUNCTION: ScrPosToPhisData
//=========================================
int ScrPosToPhisData(LPCTRLZONEMAP lpctrlZM, int iIndx, int iVal)
{
    LPSCRTOMIDITABLE lpSCRTOMIDI;
    LPWORD lpData;
    int iReturn;

    if ((iVal < 0) || (iIndx < 0))
    {
        MessageBeep(0);
        return 0;
    }

    lpSCRTOMIDI = (LPSCRTOMIDITABLE)gpBinResTable[iIndx].lpBinRes;

    if (lpSCRTOMIDI->iTotVals <= iVal)
        iVal = lpSCRTOMIDI->iMax;

    lpData = (LPWORD)((LPSTR)lpSCRTOMIDI + sizeof(SCRTOMIDITABLE));
    lpData = lpData + iVal;
    iReturn = (int)*lpData;

    return iReturn;
}

//////////////////////////////////////////////////////////////
// void    InitVULookupTables(BOOL bLinear)
//////////////////////////////////////////////////////////////

#define   VU_DIVIDER_001    (float)728.2054
#define   PIXELS_PER_SEGMENT (float)3

void InitVULookupTables(BOOL bLinear)
{
    int iCount;
    float fDacRead, fDBLow, fDBPixPerSeg, fPixelShift, fDBPerSeg, fDacGate, fDacComp;

    ZeroMemory(gVU_VertDispTable, 4096);
    for (iCount = 8; iCount < 4096; iCount++)
    {
        fDacRead = (float)iCount;
        fDacRead = ((float)log10(fDacRead / VU_DIVIDER_001)) * 20;

        if (bLinear)
            fDacRead = (float)(((fDacRead + 40.0) / 55.0) * (float)gbmpVUONVert.bmHeight);
        else
        {
            if (iCount < 116)
            {
                fDBLow = -40.0;
                fDBPixPerSeg = 1.0;
                fPixelShift = 2.0;
            }
            else
            {
                fDBLow = -15.5;
                fDBPixPerSeg = 0.5;
                fPixelShift = 77.0;
            }

            if (iCount >= 4095)
                fDacRead = 263.0;
            else
                fDacRead = (float)(fPixelShift + (fDacRead - fDBLow) * (PIXELS_PER_SEGMENT / fDBPixPerSeg));
        }

        if (fDacRead < 0.0)
            gVU_VertDispTable[iCount] = 0;
        else
            gVU_VertDispTable[iCount] = (int)fDacRead;
    }

    ZeroMemory(gVU_CompDispTable, 4096);
    ZeroMemory(gVU_GateDispTable, 4096);

    for (iCount = 0; iCount < 4096; iCount++)
    {
        if (iCount == 0)
            fDacComp = 1.0;
        else
            fDacComp = (float)iCount;

        fDacComp = (float)(4.79 + (20.0 * log10(fDacComp / 300.0)) / 1.98 + pow(20.0 * log10(fDacComp / 300.0), 2.0) / 79.794);
        if (fDacComp < 0.0)
            fDacComp = 0.0;
        if (fDacComp > 20.0)
            fDacComp = 20.0;

        if (fDacComp < 12.0)
        {
            fDBLow = 0.5;
            fDBPerSeg = 0.5;
            fPixelShift = 2.0;
        }
        else
        {
            fDBLow = 12.0;
            fDBPerSeg = 1.0;
            fPixelShift = 71.0;
        }

        if (iCount < 52) fDacComp = 0;

        fDacComp = (fPixelShift + (fDacComp - fDBLow) * (PIXELS_PER_SEGMENT / fDBPerSeg));
        gVU_CompDispTable[iCount] = (int)fDacComp;
    }

    for (iCount = 0; iCount < 4096; iCount++)
    {
        if (iCount == 0)
            fDacGate = 1.0;
        else
            fDacGate = (float)iCount;

        fDacGate = (float)(-49.7815 + (20.0 * log10(fDacGate)) - pow(20.0 * log10(fDacGate), 2.0) / 521.229 + pow(20.0 * log10(fDacGate), 3.0) / 48000.0);
        if (fDacGate < -38.0)
            fDacGate = -60.0;
        if (fDacGate > 20.0)
            fDacGate = 20.0;

        if (fDacGate < 5.0)
        {
            fDBLow = -38.0;
            fDBPerSeg = 5.0;
            fPixelShift = 2.0;
        }
        else if (fDacGate < 12.0)
        {
            fDBLow = 5.0;
            fDBPerSeg = 0.5;
            fPixelShift = 29.0;
        }
        else
        {
            fDBLow = 12.0;
            fDBPerSeg = 1.0;
            fPixelShift = 71.0;
        }

        fDacGate = (fPixelShift + (fDacGate - fDBLow) * (PIXELS_PER_SEGMENT / fDBPerSeg));
        gVU_GateDispTable[iCount] = (int)fDacGate;
    }
}

//=========================================
// Debug functions
//=========================================
#ifdef _DEBUG
WORD GETPHISDATAVALUE(int mixer, LPCTRLZONEMAP pctrlzm, int control)
{
    WORD* mem_pointer;
    if (pctrlzm->iModuleNumber >= 80 || pctrlzm->iModuleNumber < 0)
    {
        MessageBox(ghwndMain, TEXT("iModuleNumber is not valid"), TEXT("ERROR"), MB_ICONERROR | MB_OK);
        return 0;
    }
    else
    {
        mem_pointer = &gpwMemMapMixer[(mixer * (giMax_CHANNELS * giMax_CONTROLS)) + (pctrlzm->iModuleNumber * giMax_CONTROLS) + control];
        return *mem_pointer;
    }
}

void SETPHISDATAVALUE(int mixer, LPCTRLZONEMAP pctrlzm, int control, int value)
{
    WORD* mem_pointer;

    if (pctrlzm->iModuleNumber >= 80 || pctrlzm->iModuleNumber < 0)
    {
        MessageBox(ghwndMain, TEXT("iModuleNumber is not valid"), TEXT("ERROR"), MB_ICONERROR | MB_OK);
        return;
    }
    else
    {
        mem_pointer = &gpwMemMapMixer[(mixer * (giMax_CHANNELS * giMax_CONTROLS)) + (pctrlzm->iModuleNumber * giMax_CONTROLS) + control];
        *mem_pointer = (WORD)value;
    }
}

WORD GETPHISDATAVALUEBUFFER(int mixer, LPCTRLZONEMAP pctrlzm, int control)
{
    WORD* mem_pointer;
    if (pctrlzm->iModuleNumber >= 80 || pctrlzm->iModuleNumber < 0)
    {
        MessageBox(ghwndMain, TEXT("iModuleNumber is not valid"), TEXT("ERROR"), MB_ICONERROR | MB_OK);
        return 0;
    }
    else
    {
        mem_pointer = &gpwMemMapBuffer[(mixer * (giMax_CHANNELS * giMax_CONTROLS)) + (pctrlzm->iModuleNumber * giMax_CONTROLS) + control];
        return  *mem_pointer;
    }
}

void SETPHISDATAVALUEBUFFER(int mixer, LPCTRLZONEMAP pctrlzm, int control, int value)
{
    WORD* mem_pointer;
    if (pctrlzm->iModuleNumber >= 80 || pctrlzm->iModuleNumber < 0)
    {
        MessageBox(ghwndMain, TEXT("iModuleNumber is not valid"), TEXT("ERROR"), MB_ICONERROR | MB_OK);
        return;
    }
    else
        mem_pointer = &gpwMemMapBuffer[(mixer * (giMax_CHANNELS * giMax_CONTROLS)) + (pctrlzm->iModuleNumber * giMax_CONTROLS) + control];
    *mem_pointer = (WORD)value;
}
#endif // _DEBUG

} // extern "C"

#undef VU_DIVIDER_001
#undef PIXELS_PER_SEGMENT
