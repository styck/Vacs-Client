
//=================================================
// Copyright 1998-2001, CorTek Software, Inc.
//=================================================

#include "SAMM.h"
#include "SAMMEXT.h"
#include "MACRO.h"

#include "consoledefinition.h"
#include "..\\commport\\commport.h"


extern int	g_iMasterModuleIdx;
extern int	g_aiAux[MAX_MATRIX_COUNT];
extern int	g_aiMatrix[MAX_MATRIX_COUNT];
extern int	g_CueMasterSystem;
extern int  g_iMasterModuleIdx;
extern int  g_iCueModuleIdx;

//
int						g_inputCueActiveCounter = 0;
CUE_PRIORITY	g_cue_priority = {0,0,0,0,0};


BOOL CheckForSpecialFilters(/*LPMIXERWNDDATA lpwmd, */LPCTRLZONEMAP pctrlzm);
int IsSpecialFilter(LPMIXERWNDDATA lpmwd, int iPhisChan, LPCTRLZONEMAP pctrlzm);
void		ResendControlData(LPCTRLZONEMAP  lpctrlZM);
void		HandleSubSuminMuteFilter(int iPhisChann, LPMIXERWNDDATA lpmwd, LPCTRLZONEMAP lpctrlZM, BOOL bIsOn);
void		ShowStereoCueMetersWindow (BOOL show);
void		HadleCueMasterSystem ();

/////////////////////////////////////////////////////////////////////
// FUNCTION: InitExternalIface
//
// purpose: 
//
//
//

void	InitExternalIface()
{
	LPCTRLZONEMAP pctrlzm;

	FillMemory(&gExternalIface[0], sizeof(EXTERNAL_IFACE_CONTROL)*3, 0xffff);

	// prepare the Input fader ..
	pctrlzm = ScanCtrlZonesNum(gpZoneMaps_Zoom[0].lpZoneMap, CTRL_NUM_INPUT_VOL_FADER);
	gExternalIface[0].ctrlId = pctrlzm->iCtrlNumAbs;
	gExternalIface[0].range = 0x7f;
	gExternalIface[0].scale = CDef_GetCtrlMaxVal(pctrlzm->iCtrlNumAbs);
	gExternalIface[0].chann = 13;
	gExternalIface[0].fscale = (float)gExternalIface[0].range / (float)gExternalIface[0].scale;


};

/////////////////////////////////////////////////////////////////////
// FUNCTION: IsMuteFilter
//
// purpose: 
//
//
//

BOOL  IsMuteFilter(LPCTRLZONEMAP pctrlzm)
{
  BOOL    bRet = FALSE;

  switch(pctrlzm->iCtrlType)
  {
  case CTRL_TYPE_BTN_INPUTMUTE_FILTER: 
  case  CTRL_TYPE_BTN_AUXMUTE_FILTER:   
	case CTRL_INPUT_AUX16B_MUTE_FILTER:	// LINE B TO AUX 16
  case  CTRL_MASTER_MUTE_FILTER:
  case  CTRL_MARIXLT_MUTE_FILTER:       
  case  CTRL_MARIXRT_MUTE_FILTER:       
  case  CTRL_MASTER_AUX_MUTE_FILTER:

  case  CTRL_INPUT_MICA_MUTE_FILTER:    
  case  CTRL_INPUT_LINEA_MUTE_FILTER:   
  case  CTRL_INPUT_MICB_MUTE_FILTER:    
  case  CTRL_INPUT_LINEB_MUTE_FILTER:   
  case  CTRL_MATRIX_SUB_MUTE_FILTER:    
  case  CTRL_MATRIX_AUX_MUTE_FILTER:    
  case CTRL_MATRIX_MASTER_LT_MUTE_FILTER:
  case CTRL_MATRIX_MASTER_RT_MUTE_FILTER:
  case CTRL_MATRIX_MONO_MUTE_FILTER:
  case CTRL_MATRIX_CENTER_MUTE_FILTER:
  case CTRL_CUE_MASTER_MUTE_FILTER:
	case CTRL_SUB_SUMIN_MUTE_FILTER:
//	case CTRL_TYPE_BTN_EQ_RESET_FILTER:		// fds maybe needs to be commented out????
    bRet = TRUE;
    break;
  }

  return bRet;
}

/////////////////////////////////////////////////////////////////////
// FUNCTION: SetFilteredControlsByNumber
//
// purpose: 
//
//
// no error checking is performed here
//
void    SetFilteredControlsByNumber(LPCTRLZONEMAP lpctrl, int filtered)//, LPMIXERWNDDATA lpmwd)
{
  LPCTRLZONEMAP       pctrlTemp = lpctrl;
  int                 iCtrlPos = lpctrl->iCtrlChanPos;

//	if(lpmwd)
//		SetAlternativeZMFilter( lpctrl, filtered, lpmwd);

  while(lpctrl->iCtrlChanPos == iCtrlPos)
  {
    lpctrl->iFiltered = filtered;
    lpctrl ++;
  }
  lpctrl = pctrlTemp--;

  while(lpctrl->iCtrlChanPos == iCtrlPos)
  {
    lpctrl->iFiltered = filtered;
    lpctrl --;
  }
}

/////////////////////////////////////////////////////////////////////
// FUNCTION: SetAlternativeZMFilter
//
// purpose: 
//
//
void	SetAlternativeZMFilter(LPCTRLZONEMAP	lpctrl, int filtered, LPMIXERWNDDATA lpmwd, int iPhisChann)
{
	LPCTRLZONEMAP	lpctrlAlt;

	if(lpmwd->lpZoneMapZoom == NULL)
		return;

  lpctrlAlt = ScanCtrlZonesNum(lpmwd->lpZoneMapZoom[iPhisChann].lpZoneMap, lpctrl->iCtrlChanPos);
	if(lpctrlAlt)
		SetFilteredControlsByNumber(lpctrlAlt, filtered);
}


/////////////////////////////////////////////////////////////////////
// FUNCTION: HandleInputSignalInMuteFilter
//
// purpose: 
//
//
void HandleInputSignalInMuteFilter(int iPhisChann, LPMIXERWNDDATA lpmwd, LPCTRLZONEMAP lpctrlZM, BOOL bIsOn)
{
  CONTROLDATA         ctrlData;
  LPCTRLZONEMAP       lpctrl;
  BOOL                bUpdateCtrl = TRUE;
  int                 iCtrlType;

  switch(lpctrlZM->iCtrlType)
  {
		case CTRL_INPUT_MICA_MUTE_FILTER:
			iCtrlType = CTRL_NUM_INPUT_MIC_A_LEVEL;
			break;
		case CTRL_INPUT_LINEA_MUTE_FILTER:
			iCtrlType = CTRL_NUM_INPUT_LINE_A_LEVEL;
			break;
		case CTRL_INPUT_MICB_MUTE_FILTER:
			iCtrlType = CTRL_NUM_INPUT_MIC_B_LEVEL;
			break;
		case CTRL_INPUT_LINEB_MUTE_FILTER:
			iCtrlType = CTRL_NUM_INPUT_LINE_B_LEVEL;
			break;

		default:
			return;
  }


  if(bIsOn == TRUE)
  {         
    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, iCtrlType);
    // Send the Data out
    //------------------
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = CDef_GetCtrlMaxVal(lpctrl->iCtrlNumAbs) - 1;
    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
    
    SetFilteredControlsByNumber(lpctrl, YES_FILTER);
		SetAlternativeZMFilter(lpctrl, YES_FILTER, lpmwd, iPhisChann);
	
  }
  else
  {
    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, iCtrlType);
    // Send the Data out
    //------------------
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

    SetFilteredControlsByNumber(lpctrl, NO_FILTER);
		SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, iPhisChann);

  }
}

/////////////////////////////////////////////////////////////////////
// FUNCTION: HandleMatrixAuxMuteFilter
//
// purpose: 
//
//
void HandleMatrixAuxMuteFilter(int iPhisChann, LPMIXERWNDDATA lpmwd, LPCTRLZONEMAP lpctrlZM, BOOL bIsOn)
{
  CONTROLDATA         ctrlData;
  LPCTRLZONEMAP       lpctrl;
  int                 i_ctrl_pos;
  BOOL                bUpdateCtrl = TRUE;

  
  if(lpmwd->lpZoneMap[iPhisChann].lpZoneMap == NULL)
    return;

  i_ctrl_pos = lpctrlZM->iCtrlChanPos - CTRL_NUM_MATRIX_AUX16LT_MUTE;
  i_ctrl_pos = i_ctrl_pos + CTRL_NUM_MATRIX_AUX16LT;

  if(bIsOn == TRUE)
  {         
    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, i_ctrl_pos);
    // Send the Data out
    //------------------
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = CDef_GetCtrlMaxVal(lpctrl->iCtrlNumAbs) - 1;
    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
    
    SetFilteredControlsByNumber(lpctrl, YES_FILTER);
		SetAlternativeZMFilter(lpctrl, YES_FILTER, lpmwd, iPhisChann);

  }
  else
  {
    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, i_ctrl_pos);
    // Send the Data out
    //------------------
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

    SetFilteredControlsByNumber(lpctrl, NO_FILTER);
		SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, iPhisChann);

  }

}

/////////////////////////////////////////////////////////////////////
// FUNCTION: HandleSubSuminMuteFilter
//
// purpose: 
//
//
void HandleSubSuminMuteFilter(int iPhisChann, LPMIXERWNDDATA lpmwd, LPCTRLZONEMAP lpctrlZM, BOOL bIsOn)
{
  CONTROLDATA         ctrlData;
  LPCTRLZONEMAP       lpctrl;
  int                 i_ctrl_pos;
  BOOL                bUpdateCtrl = TRUE;

  
  if(lpmwd->lpZoneMap[iPhisChann].lpZoneMap == NULL)
    return;

	if(lpctrlZM->iCtrlChanPos == CTRL_NUM_SUB_SUMIN_LT_MUTE)
		i_ctrl_pos = CTRL_NUM_MATRIX_SUB_LT_SUM_IN;
	else
		i_ctrl_pos = CTRL_NUM_MATRIX_SUB_RT_SUM_IN;

  if(bIsOn == TRUE)
  {         
    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, i_ctrl_pos);
    // Send the Data out
    //------------------
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = CDef_GetCtrlMaxVal(lpctrl->iCtrlNumAbs) - 1;
    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
    
    SetFilteredControlsByNumber(lpctrl, YES_FILTER);
		SetAlternativeZMFilter(lpctrl, YES_FILTER, lpmwd, iPhisChann);

  }
  else
  {
    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, i_ctrl_pos);
    // Send the Data out
    //------------------
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

    SetFilteredControlsByNumber(lpctrl, NO_FILTER);
		SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, iPhisChann);

  }

}

/////////////////////////////////////////////////////////////////////
// FUNCTION: HandleMatrixSubMuteFilter
//
// purpose: 
//
//
void HandleMatrixSubMuteFilter(int iPhisChann, LPMIXERWNDDATA lpmwd, LPCTRLZONEMAP lpctrlZM, BOOL bIsOn)
{
  CONTROLDATA         ctrlData;
  LPCTRLZONEMAP       lpctrl;
  int                 i_ctrl_pos;
  BOOL                bUpdateCtrl = TRUE;
                                                                         
  
  if(lpmwd->lpZoneMap[iPhisChann].lpZoneMap == NULL)
    return;

  i_ctrl_pos = CTRL_NUM_MATRIX_SUB08LT + (lpctrlZM->iCtrlChanPos - CTRL_NUM_MATRIX_SUB08LT_MUTE);
  
  if(bIsOn == TRUE)
  {         
    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, i_ctrl_pos);
    // Send the Data out
    //------------------
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = CDef_GetCtrlMaxVal(lpctrl->iCtrlNumAbs) - 1;
    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
    
    SetFilteredControlsByNumber(lpctrl, YES_FILTER);
		SetAlternativeZMFilter(lpctrl, YES_FILTER, lpmwd, iPhisChann);
  }
  else
  {
    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, i_ctrl_pos);
    // Send the Data out
    //------------------
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

    SetFilteredControlsByNumber(lpctrl, NO_FILTER);
		SetAlternativeZMFilter(lpctrl, YES_FILTER, lpmwd, iPhisChann);
  }
}

/////////////////////////////////////////////////////////////////////
// FUNCTION: HandleCueMasterMuteFilter
//
// purpose: 
//
//
void HandleCueMasterMuteFilter(int iPhisChann, LPMIXERWNDDATA lpmwd, 
                                  LPCTRLZONEMAP lpctrlZM, BOOL bIsOn)
{
  CONTROLDATA         ctrlData;
  LPCTRLZONEMAP       lpctrl;
  BOOL                bUpdateCtrl = TRUE;

  if(lpmwd->lpZoneMap[iPhisChann].lpZoneMap == NULL)
    return;

  if(bIsOn == TRUE)
  {         
    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, CTRL_NUM_MASTERLT_CUE_A_LT);
    // Send the Data out
    //------------------
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = CDef_GetCtrlMaxVal(lpctrl->iCtrlNumAbs) - 1;
    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
    
    SetFilteredControlsByNumber(lpctrl, YES_FILTER);
		SetAlternativeZMFilter(lpctrl, YES_FILTER, lpmwd, iPhisChann);

    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, CTRL_NUM_MASTERLT_CUE_A_RT);
    // Send the Data out
    //------------------
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = CDef_GetCtrlMaxVal(lpctrl->iCtrlNumAbs) - 1;
    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
    
    SetFilteredControlsByNumber(lpctrl, YES_FILTER);
		SetAlternativeZMFilter(lpctrl, YES_FILTER, lpmwd, iPhisChann);

  }
  else
  {
		// Activate if only some other Cue is enabled
		//
		if(gCueActiveCount > 0)
		{
			lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, CTRL_NUM_MASTERLT_CUE_A_LT);
			// Send the Data out
			//------------------
			ctrlData.wMixer   = 0;
			ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
			ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
			ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
			SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

			SetFilteredControlsByNumber(lpctrl, NO_FILTER);
			SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, iPhisChann);

			lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, CTRL_NUM_MASTERLT_CUE_A_RT);
			// Send the Data out
			//------------------
			ctrlData.wMixer   = 0;
			ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
			ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
			ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
			SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

			SetFilteredControlsByNumber(lpctrl, NO_FILTER);
			SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, iPhisChann);
		}
  }

}


/////////////////////////////////////////////////////////////////////
// FUNCTION: isCtrlValueEqualToDefault
//
// purpose: 
//
//
int		isCtrlValueEqualToDefault (LPCTRLZONEMAP   lpctrlZM, int ctrlNum)
{
  LPCTRLZONEMAP   pctrlzm = ScanCtrlZonesNum (lpctrlZM, ctrlNum);
	if (pctrlzm && GETPHISDATAVALUE(0, pctrlzm, ctrlNum) == 
				                      CDef_GetCtrlDefaultVal(pctrlzm->iCtrlNumAbs))
	{
		return 1;
	}
	return 0;
}


/////////////////////////////////////////////////////////////////////
// FUNCTION: isCtrlValueNotEqualToDefault
//
// purpose: 
//
//
int		isCtrlValueNotEqualToDefault (LPCTRLZONEMAP   lpctrlZM, int ctrlNum)
{
  LPCTRLZONEMAP   pctrlzm = ScanCtrlZonesNum (lpctrlZM, ctrlNum);
	if (pctrlzm && GETPHISDATAVALUE(0, pctrlzm, ctrlNum) != 
				                      CDef_GetCtrlDefaultVal(pctrlzm->iCtrlNumAbs))
	{
		return 1;
	}
	return 0;
}


///////////////////////////////////////////////////////////////////////
// Apply a filter to CTRL_NUM_MASTER_CUE_A_SUB, CTRL_NUM_MASTER_CUE_A_AUX,
// CTRL_NUM_MASTER_CUE_A_MATRIX, and CTRL_NUM_MASTER_CUE_A_MASTER
// when any of the five Input Cues are enabled.
// Keep a global count of all Input Cues, across all channels, so
// when this count goes down to 0 we can remove the filtering on
// CTRL_NUM_MASTER_CUE_A_SUB, CTRL_NUM_MASTER_CUE_A_AUX,
// CTRL_NUM_MASTER_CUE_A_MATRIX, and CTRL_NUM_MASTER_CUE_A_MASTER
// and recall their current values.
//
void		handleInputCuePriority (LPMIXERWNDDATA lpmwd_work, BOOL	input_cue_on)
{
  LPCTRLZONEMAP   lpctrl;
  CONTROLDATA         ctrlData;
	LPMIXERWNDDATA			lpmwd = lpmwd_work;


	if (lpmwd->lpZoneMap->iZonesCount < 100)
		lpmwd = GetValidMixerWindowData ();


	if (g_cue_priority.input > 0)
	{
    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[g_iMasterModuleIdx].lpZoneMap, CTRL_NUM_MASTER_CUE_A_INPUT);
    // Send the Data out
    //------------------
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

    SetFilteredControlsByNumber(lpctrl, NO_FILTER);
		SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, g_iMasterModuleIdx);
	}
	else
	{
		// CTRL_NUM_MASTER_CUE_A_MASTER
		lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[g_iMasterModuleIdx].lpZoneMap, CTRL_NUM_MASTER_CUE_A_INPUT);
		if(lpctrl->iFiltered == NO_FILTER)
		{
			// Send the Data out
			//------------------
			ctrlData.wMixer   = 0;
			ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
			ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
			ctrlData.wVal     = CDef_GetCtrlMaxVal(lpctrl->iCtrlNumAbs) - 1;
			SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
  
			SetFilteredControlsByNumber(lpctrl, YES_FILTER);
			SetAlternativeZMFilter(lpctrl, YES_FILTER, lpmwd, g_iMasterModuleIdx);
		}
	}

	if (g_cue_priority.input > 0 || g_cue_priority.master == 0)
	{
    // CTRL_NUM_MASTER_CUE_A_MASTER
    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[g_iMasterModuleIdx].lpZoneMap, CTRL_NUM_MASTER_CUE_A_MASTER);
		if(lpctrl->iFiltered == NO_FILTER)
		{
			// Send the Data out
			//------------------
			ctrlData.wMixer   = 0;
			ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
			ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
			ctrlData.wVal     = CDef_GetCtrlMaxVal(lpctrl->iCtrlNumAbs) - 1;
			SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
    
			SetFilteredControlsByNumber(lpctrl, YES_FILTER);
			SetAlternativeZMFilter(lpctrl, YES_FILTER, lpmwd, g_iMasterModuleIdx);
		}
	}
	else
	{
    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[g_iMasterModuleIdx].lpZoneMap, CTRL_NUM_MASTER_CUE_A_MASTER);
    // Send the Data out
    //------------------
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

    SetFilteredControlsByNumber(lpctrl, NO_FILTER);
		SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, g_iMasterModuleIdx);
	}

	if (g_cue_priority.input > 0 || g_cue_priority.sub == 0)
	{
    // CTRL_NUM_MASTER_CUE_A_SUB
    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[g_iMasterModuleIdx].lpZoneMap, CTRL_NUM_MASTER_CUE_A_SUB);
		if(lpctrl->iFiltered == NO_FILTER)
		{
			// Send the Data out
			//------------------
			ctrlData.wMixer   = 0;
			ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
			ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
			ctrlData.wVal     = CDef_GetCtrlMaxVal(lpctrl->iCtrlNumAbs) - 1;
			SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
    
			SetFilteredControlsByNumber(lpctrl, YES_FILTER);
			SetAlternativeZMFilter(lpctrl, YES_FILTER, lpmwd, g_iMasterModuleIdx);
		}
	}
	else
	{
    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[g_iMasterModuleIdx].lpZoneMap, CTRL_NUM_MASTER_CUE_A_SUB);
    // Send the Data out
    //------------------
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

    SetFilteredControlsByNumber(lpctrl, NO_FILTER);
		SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, g_iMasterModuleIdx);
	}

	if (g_cue_priority.input > 0 || g_cue_priority.aux == 0)
	{
    // CTRL_NUM_MASTER_CUE_A_AUX
    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[g_iMasterModuleIdx].lpZoneMap, CTRL_NUM_MASTER_CUE_A_AUX);
		if(lpctrl->iFiltered == NO_FILTER)
		{
			// Send the Data out
			//------------------
			ctrlData.wMixer   = 0;
			ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
			ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
			ctrlData.wVal     = CDef_GetCtrlMaxVal(lpctrl->iCtrlNumAbs) - 1;
			SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
    
			SetFilteredControlsByNumber(lpctrl, YES_FILTER);
			SetAlternativeZMFilter(lpctrl, YES_FILTER, lpmwd, g_iMasterModuleIdx);
		}
	}
	else
	{
    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[g_iMasterModuleIdx].lpZoneMap, CTRL_NUM_MASTER_CUE_A_AUX);
    // Send the Data out
    //------------------
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

    SetFilteredControlsByNumber(lpctrl, NO_FILTER);
		SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, g_iMasterModuleIdx);
	}

	if (g_cue_priority.input > 0 || g_cue_priority.matrix == 0)
	{
    // CTRL_NUM_MASTER_CUE_A_MATRIX
    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[g_iMasterModuleIdx].lpZoneMap, CTRL_NUM_MASTER_CUE_A_MATRIX);
		if(lpctrl->iFiltered == NO_FILTER)
		{
			// Send the Data out
			//------------------
			ctrlData.wMixer   = 0;
			ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
			ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
			ctrlData.wVal     = CDef_GetCtrlMaxVal(lpctrl->iCtrlNumAbs) - 1;
			SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
    
			SetFilteredControlsByNumber(lpctrl, YES_FILTER);
			SetAlternativeZMFilter(lpctrl, YES_FILTER, lpmwd, g_iMasterModuleIdx);
		}
	}
	else
	{
    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[g_iMasterModuleIdx].lpZoneMap, CTRL_NUM_MASTER_CUE_A_MATRIX);
    // Send the Data out
    //------------------
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

    SetFilteredControlsByNumber(lpctrl, NO_FILTER);
		SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, g_iMasterModuleIdx);
	}
}

/////////////////////////////////////////////////////////////////////
// FUNCTION: countInputCuesOn
//
// purpose: 
//
//
int		countInputCuesOn (void)
{
	int							channel;
  LPCTRLZONEMAP   lpctrlZM;
	int							ret = 0;


	for (channel = 0; channel < MAX_CHANNELS; channel ++)
	{
		lpctrlZM = gpZoneMaps_Zoom[channel].lpZoneMap;
    if(lpctrlZM == NULL)
      continue;
		switch (gDeviceSetup.iaChannelTypes[channel])
		{
		case DCX_DEVMAP_MODULE_INPUT: // input module
			ret += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_INPUT_CUE_FAD_PRE);
			ret += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_INPUT_CUE_FAD_POST);
			ret += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_INPUT_MIC_B_CUE);
			ret += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_INPUT_MIC_A_CUE);
			ret += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_INPUT_GATE_KEY_INOUT);
			break;
		}
	}
	return ret;
}

/////////////////////////////////////////////////////////////////////
// FUNCTION: PrepareCueMasterSystem
//
// purpose: 
// All we need to do here is scan the board for any Cue buttons being
// pushed-IN and store the value in gCueActiveCount
//
void	PrepareCueMasterSystem (void)
{
	int							channel;
  LPCTRLZONEMAP   lpctrlZM;
	int							temp_count;

	gCueActiveCount = 0;
	g_cue_priority.aux = g_cue_priority.input = g_cue_priority.master = g_cue_priority.matrix =
		g_cue_priority.sub = 0;


	for (channel = 0; channel < MAX_CHANNELS; channel ++)
	{
		lpctrlZM = gpZoneMaps_Zoom[channel].lpZoneMap;
    if(lpctrlZM == NULL)
      continue;
		switch (gDeviceSetup.iaChannelTypes[channel])
		{
		case DCX_DEVMAP_MODULE_INPUT: // input module
			temp_count = gCueActiveCount;
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_INPUT_CUE_FAD_PRE);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_INPUT_CUE_FAD_POST);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_INPUT_MIC_B_CUE);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_INPUT_MIC_A_CUE);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_INPUT_GATE_KEY_INOUT);
			g_cue_priority.input += gCueActiveCount - temp_count;
			break;
		//
		case DCX_DEVMAP_MODULE_AUX:
			temp_count = gCueActiveCount;
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MATRIX_STERIO_CUE_PRE);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MATRIX_STERIO_CUE_POST);
			g_cue_priority.sub += gCueActiveCount - temp_count;

			temp_count = gCueActiveCount;
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MATRIX_CUE_PRE);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MATRIX_CUE_POST);
			g_cue_priority.matrix += gCueActiveCount - temp_count;
			break;
		//
		case DCX_DEVMAP_MODULE_CUE:
			break;
		//
		case DCX_DEVMAP_MODULE_MASTER:
			temp_count = gCueActiveCount;
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX16PRE);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX15PRE);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX14PRE);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX13PRE);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX12PRE);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX11PRE);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX10PRE);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX09PRE);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX08PRE);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX07PRE);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX06PRE);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX05PRE);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX04PRE);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX03PRE);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX02PRE);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX01PRE);
			//
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX16POST);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX15POST);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX14POST);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX13POST);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX12POST);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX11POST);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX10POST);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX09POST);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX08POST);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX07POST);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX06POST);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX05POST);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX04POST);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX03POST);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX02POST);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_AUX01POST);
			g_cue_priority.aux += gCueActiveCount - temp_count;
			
			temp_count = gCueActiveCount;
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_CUE_LEVEL_MONO);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_CUE_LEVEL_CENTER);

// fds	removed 3/18/2001 as per gamble		gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_CUE_A_SUM_IN);

			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_STEREO_CUE_PRE);
			gCueActiveCount += isCtrlValueNotEqualToDefault(lpctrlZM, CTRL_NUM_MASTER_STEREO_CUE_POST);
			g_cue_priority.master += gCueActiveCount - temp_count;
			
			break;
			///////
		default:
			break;

		}
		
	}

};


/////////////////////////////////////////////////////////////////////
// FUNCTION: HandleCueMasterMuteFilterEx
//
// purpose: 
// Extended Cue master mute .... done when NO cues are selected
// this should be an explicite MUTE which preceeds any Master Cue
// settings.
//
static	int		g_cue_last_count = -1;
void HandleCueMasterMuteFilterEx(int iPhisChann, LPMIXERWNDDATA lpmwd_work, 
                                  LPCTRLZONEMAP lpctrlZM, BOOL bIsOn)
{
  CONTROLDATA         ctrlData;
  LPCTRLZONEMAP       lpctrl;
	WORD								wVal;
	LPMIXERWNDDATA			lpmwd = lpmwd_work;


	if (lpmwd->lpZoneMap->iZonesCount < 100)
		lpmwd = GetValidMixerWindowData ();

	gCueActiveCount = g_cue_priority.aux + g_cue_priority.input + g_cue_priority.master +
		                g_cue_priority.matrix + g_cue_priority.sub;


	if (gCueActiveCount == g_cue_last_count)
		return;

	///////////////////////////////////////////////////////////
	// Check if the Master Cue mute is ON .... in this case 
	// we should do absolutely nothing ... and wait until the 
	// Master Cue-Mute is turned off

	lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, CTRL_NUM_MASTER_CUE_MUTE);
	wVal = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
	if(wVal == 0)
		return;				// Master Cue-Mute is ON, just return

	/////////////////////////////////////////////////////////////////////////
	// if none of the cues are enabled then go ahead and mute the master Cue
	//
	if(gCueActiveCount == 0)
	{
		ShowStereoCueMetersWindow (FALSE);

    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, CTRL_NUM_MASTERLT_CUE_A_LT);
    // Send the Data out
    //------------------
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = CDef_GetCtrlMaxVal(lpctrl->iCtrlNumAbs) - 1;
    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
    
    SetFilteredControlsByNumber(lpctrl, YES_FILTER);
		SetAlternativeZMFilter(lpctrl, YES_FILTER, lpmwd, iPhisChann);

    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, CTRL_NUM_MASTERLT_CUE_A_RT);
    // Send the Data out
    //------------------
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = CDef_GetCtrlMaxVal(lpctrl->iCtrlNumAbs) - 1;
    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
    
    SetFilteredControlsByNumber(lpctrl, YES_FILTER);
		SetAlternativeZMFilter(lpctrl, YES_FILTER, lpmwd, iPhisChann);
	}
	else
	{
		ShowStereoCueMetersWindow (TRUE);

		lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, CTRL_NUM_MASTERLT_CUE_A_LT);
		// Send the Data out
		//------------------
		ctrlData.wMixer   = 0;
		ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
		ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
		ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
		SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

		SetFilteredControlsByNumber(lpctrl, NO_FILTER);
		SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, iPhisChann);

		lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, CTRL_NUM_MASTERLT_CUE_A_RT);
		// Send the Data out
		//------------------
		ctrlData.wMixer   = 0;
		ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
		ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
		ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
		SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

		SetFilteredControlsByNumber(lpctrl, NO_FILTER);
		SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, iPhisChann);
	}
	
	g_cue_last_count = gCueActiveCount;
}


/////////////////////////////////////////////////////////////////////
// FUNCTION: HandleMatrixMasterMuteFilter
//
// purpose: 
//
//
void HandleMatrixMasterMuteFilter(int iPhisChann, LPMIXERWNDDATA lpmwd, 
                                  LPCTRLZONEMAP lpctrlZM, BOOL bIsOn)
{
  CONTROLDATA         ctrlData;
  LPCTRLZONEMAP       lpctrl;
  int                 i_ctrl_pos;
  BOOL                bUpdateCtrl = TRUE;
                                                                         
  
  if(lpmwd->lpZoneMap[iPhisChann].lpZoneMap == NULL)
    return;

  if(lpctrlZM->iCtrlType == CTRL_MATRIX_MASTER_LT_MUTE_FILTER)
    i_ctrl_pos = CTRL_NUM_MATRIX_MASTER_PRELT;
  else
    i_ctrl_pos = CTRL_NUM_MATRIX_MASTER_PRERT;
  
  if(bIsOn == TRUE)
  {         
    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, i_ctrl_pos);
    // Send the Data out
    //------------------
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = CDef_GetCtrlMaxVal(lpctrl->iCtrlNumAbs) - 1;
    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
    
    SetFilteredControlsByNumber(lpctrl, YES_FILTER);
		SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, iPhisChann);
  }
  else
  {
    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, i_ctrl_pos);
    // Send the Data out
    //------------------
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

    SetFilteredControlsByNumber(lpctrl, NO_FILTER);
		SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, iPhisChann);
  }
}




/////////////////////////////////////////////////////////////////////
// FUNCTION: HandleMatrixMonoMuteFilter
//
// purpose: 
//
//
//
void HandleMatrixMonoMuteFilter(int iPhisChann, LPMIXERWNDDATA lpmwd, 
                                  LPCTRLZONEMAP lpctrlZM, BOOL bIsOn)
{
  CONTROLDATA         ctrlData;
  LPCTRLZONEMAP       lpctrl;
  int                 i_ctrl_pos;
  BOOL                bUpdateCtrl = TRUE;
                                                                         
  
  if(lpmwd->lpZoneMap[iPhisChann].lpZoneMap == NULL)
    return;

  i_ctrl_pos = CTRL_NUM_MATRIX_MONO_MATRIX;
  
  if(bIsOn == TRUE)
  {         
    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, i_ctrl_pos);
    // Send the Data out
    //------------------
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = CDef_GetCtrlMaxVal(lpctrl->iCtrlNumAbs) - 1;
    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
    
    SetFilteredControlsByNumber(lpctrl, YES_FILTER);
		SetAlternativeZMFilter(lpctrl, YES_FILTER, lpmwd, iPhisChann);
  }
  else
  {
    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, i_ctrl_pos);
    // Send the Data out
    //------------------
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

    SetFilteredControlsByNumber(lpctrl, NO_FILTER);
		SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, iPhisChann);
  }
}


/////////////////////////////////////////////////////////////////////
// FUNCTION: HandleMatrixCenterMuteFilter
//
// purpose: 
//
//
//
//
//
void HandleMatrixCenterMuteFilter(int iPhisChann, LPMIXERWNDDATA lpmwd, 
                                  LPCTRLZONEMAP lpctrlZM, BOOL bIsOn)
{
  CONTROLDATA         ctrlData;
  LPCTRLZONEMAP       lpctrl;
  int                 i_ctrl_pos;
  BOOL                bUpdateCtrl = TRUE;
                                                                         
  
  if(lpmwd->lpZoneMap[iPhisChann].lpZoneMap == NULL)
    return;
               
  i_ctrl_pos = CTRL_NUM_MATRIX_CENTER_MATRIX;
  
  if(bIsOn == TRUE)
  {         
    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, i_ctrl_pos);
    // Send the Data out
    //------------------
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = CDef_GetCtrlMaxVal(lpctrl->iCtrlNumAbs) - 1;
    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
    
    SetFilteredControlsByNumber(lpctrl, YES_FILTER);
		SetAlternativeZMFilter(lpctrl, YES_FILTER, lpmwd, iPhisChann);
  }
  else
  {
    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, i_ctrl_pos);
    // Send the Data out
    //------------------
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

    SetFilteredControlsByNumber(lpctrl, NO_FILTER);
		SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, iPhisChann);
  }
}



/////////////////////////////////////////////////////////////////////
// FUNCTION: HandleMasterAuxMuteFilter
//
// purpose: 
//
//
//
//
//
void HandleMasterAuxMuteFilter(int iPhisChann, LPMIXERWNDDATA lpmwd, LPCTRLZONEMAP lpctrlZM, BOOL bIsOn)
{
  CONTROLDATA         ctrlData;
  LPCTRLZONEMAP       lpctrl;
  int                 i_ctrl_pos;
  BOOL                bUpdateCtrl = TRUE;

  
  if(lpmwd->lpZoneMap[iPhisChann].lpZoneMap == NULL)
    return;

  i_ctrl_pos = CTRL_NUM_MASTER_AUX16LT_FADER + ((lpctrlZM->iCtrlChanPos - CTRL_NUM_MASTER_CUE_AUX16_MUTE) * 2);
  
  if(bIsOn == TRUE)
  {         
    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, i_ctrl_pos);
    // Send the Data out
    //------------------
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = CDef_GetCtrlMaxVal(lpctrl->iCtrlNumAbs) - 1;
    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
    
    SetFilteredControlsByNumber(lpctrl, YES_FILTER);
		SetAlternativeZMFilter(lpctrl, YES_FILTER, lpmwd, iPhisChann);

		///////////////////////////////////////////
    // go to the right fader and filter it also
    i_ctrl_pos ++;
    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, i_ctrl_pos);

    // Send the Data out
    //------------------
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = CDef_GetCtrlMaxVal(lpctrl->iCtrlNumAbs) - 1;
    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
    
    SetFilteredControlsByNumber(lpctrl, YES_FILTER);
		SetAlternativeZMFilter(lpctrl, YES_FILTER, lpmwd, iPhisChann);
  }
  else
  {
    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, i_ctrl_pos);
    // Send the Data out
    //------------------
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

    SetFilteredControlsByNumber(lpctrl, NO_FILTER);
		SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, iPhisChann);

		/////////////////////////////////////////////
    // go to the right fader and Stop the filter.
    i_ctrl_pos ++;
    lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, i_ctrl_pos);

    // Send the Data out
    //------------------
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

    SetFilteredControlsByNumber(lpctrl, NO_FILTER);
		SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, iPhisChann);
  }
}

/////////////////////////////////////////////////////////////////////
// FUNCTION: HandleSubAuxMasterMatrixFilter
//
// purpose: 
//
//
//
void    HandleSubAuxMasterMatrixFilter(int iPhisChann, LPMIXERWNDDATA lpmwd, LPCTRLZONEMAP lpctrlZM, BOOL bIsOn)
{
  CONTROLDATA         ctrlData;
  LPCTRLZONEMAP       lpctrl, lpctrl2;
  int                 i_ctrl_pos;
  int                 i_temp1;
  BOOL                bUpdateCtrl = TRUE;

  
  if(lpmwd->lpZoneMap[iPhisChann].lpZoneMap == NULL)
    return;

  if(lpctrlZM->iCtrlType == CTRL_TYPE_BTN_AUX_MMATRIX_LT_POST_FILTER)
    i_ctrl_pos = CTRL_NUM_MATRIX_MASTER_PRELT;
  else
    i_ctrl_pos = CTRL_NUM_MATRIX_MASTER_PRERT;

    
  lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, i_ctrl_pos);

  if(bIsOn == TRUE)
    i_temp1 = lpctrlZM->iCtrlNumAbsAlt1;
  else
    i_temp1 = lpctrlZM->iCtrlNumAbs;

	////////////////////////////////
  // Send the Data out
  // turn off the Current control
  //-----------------------------
  ctrlData.wMixer   = 0;
  ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
  ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
  ctrlData.wVal     = CDef_GetCtrlMaxVal(lpctrl->iCtrlNumAbs) - 1;
  SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);


  if(lpctrl)
  {
    lpctrl2 = lpctrl;
    bUpdateCtrl = TRUE;
  }
  else
  {
    lpctrl2 = NULL;
    bUpdateCtrl = FALSE;
  }

	////////////////
  // main stuff ....

  while(lpctrl)
  {
    lpctrl->iCtrlNumAbs = i_temp1;
    if(lpctrl->iFiltered == YES_FILTER)
      bUpdateCtrl = FALSE;

    lpctrl = ScanCtrlZonesNum(++lpctrl, i_ctrl_pos);
  }

	///////////////////////////////
  // now update the control data

  if(bUpdateCtrl && lpctrl2 != NULL)
  {
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl2->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl2->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl2, lpctrl2->iCtrlChanPos);

    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
  }
}

/////////////////////////////////////////////////////////////////////
// FUNCTION: HandleMasterHeadphonesPostFilter
//
// purpose: 
//
//
//
//    
void    HandleMasterHeadphonesPostFilter(int iPhisChann, LPMIXERWNDDATA lpmwd, LPCTRLZONEMAP lpctrlZM, BOOL bIsOn)
{
  CONTROLDATA         ctrlData;
  LPCTRLZONEMAP       lpctrl, lpctrl2;
  int                 i_ctrl_pos;
  int                 i_temp1;
  BOOL                bUpdateCtrl = TRUE;


  
  if(lpmwd->lpZoneMap[iPhisChann].lpZoneMap == NULL)
    return;

  i_ctrl_pos = CTRL_NUM_MASTER_HEADPHONES;

  lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, i_ctrl_pos);

  if(bIsOn == TRUE)
    i_temp1 = lpctrlZM->iCtrlNumAbsAlt1;
  else
    i_temp1 = lpctrlZM->iCtrlNumAbs;

	///////////////////////////////
  // Send the Data out
  // turn off the Current control
  //-----------------------------
  ctrlData.wMixer   = 0;
  ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
  ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
  ctrlData.wVal     = CDef_GetCtrlMaxVal(lpctrl->iCtrlNumAbs) - 1;
  SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

  if(lpctrl)
  {
    lpctrl2 = lpctrl;
    bUpdateCtrl = TRUE;
  }
  else
  {
    lpctrl2 = NULL;
    bUpdateCtrl = FALSE;
  }

	///////////////////
  // main stuff ....

  while(lpctrl)
  {
    lpctrl->iCtrlNumAbs = i_temp1;
    if(lpctrl->iFiltered == YES_FILTER)
      bUpdateCtrl = FALSE;

    lpctrl = ScanCtrlZonesNum(++lpctrl, i_ctrl_pos);
  }

	//////////////////////////////
  // now update the control data

  if(bUpdateCtrl && lpctrl2 != NULL)
  {
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrl2->iModuleNumber;//iChannel;
    ctrlData.wCtrl    = lpctrl2->iCtrlNumAbs; // we use this one since for the definition dll
    ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl2, lpctrl2->iCtrlChanPos);

    SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
  }
}

/////////////////////////////////////////////////////////////////////
// FUNCTION: HandleMasterAuxPrePostSwtch
//
// purpose: 
//
//
// CTRL_NUM_MASTER_AUX16PRE
//
void  HandleMasterAuxPrePostSwtch(int iPhisChann, LPMIXERWNDDATA lpmwd, LPCTRLZONEMAP lpctrlZM, BOOL bIsOn)
{
  /*
  CONTROLDATA         ctrlData;
  LPCTRLZONEMAP       lpctrl, lpctrl2;
  int                 i_ctrl_pos;
  int                 i_temp1;
  BOOL                bUpdateCtrl = TRUE;
*/


}


/////////////////////////////////////////////////////////////////////
// FUNCTION: HandleResetEQFilter
//
// purpose: 
//
// Reset the EQ to flat
// Loops through all the EQ settings and setups then to their DEFAULT value
// Also updates all windows that are displaying the same data.
//

void    HandleResetEQFilter(int iPhisChann, LPMIXERWNDDATA lpmwd, LPCTRLZONEMAP lpctrlZM, BOOL bIsOn)
{
  CONTROLDATA         ctrlData;
  LPCTRLZONEMAP       lpctrl;
  int                 i_ctrl_pos;
  
  if(lpmwd->lpZoneMap[iPhisChann].lpZoneMap == NULL)
    return;

	for(i_ctrl_pos = CTRL_NUM_INPUT_HIGHFREQ; i_ctrl_pos <=CTRL_NUM_INPUT_LF_PEAKSHELF ;i_ctrl_pos++)
	{
		lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, i_ctrl_pos);

		////////////////////////////////
		// Send the Data out
		// turn off the Current control
		//-----------------------------

		ctrlData.wMixer   = 0;
		ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
		ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
		ctrlData.wVal     = CDef_GetCtrlDefaultVal(lpctrl->iCtrlNumAbs);
    SETPHISDATAVALUE(ctrlData.wMixer, lpctrlZM, i_ctrl_pos, ctrlData.wVal);

		SendDataToDevice(&ctrlData, TRUE, NULL, 0, lpmwd, TRUE);
		//SendDataToDevice(&ctrlData, TRUE, lpctrl, 0, lpmwd, TRUE);

		//////////////////////////////////////
		// now update all of the other mixers
		// windows that represent this mixer
		// using the iMixer, iPhisChannel
		// and iVal

//		UpdateControlsByCtrlNum(hdcScr, g_hdcMemory, lpmwd, lpmwd->iXadj, iPhisChann, lpctrl, ctrlData.wVal, DIRECTIONS_ALL, TRUE);
		UpdateSameMixWndByCtrlNum(lpmwd->hwndImg, lpmwd->iMixer, iPhisChann, lpctrl, ctrlData.wVal, NULL);

		////////////////////////////////////////////
		// force a redraw of the updated EQ settings

      if(ghwndMain) 
			{
        InvalidateRect(lpmwd->hwndImg, &lpmwd->rVisible, TRUE);
      }

	}


}


/////////////////////////////////////////////////////////////////////
// FUNCTION: CheckFilter
//
// purpose: 
//
//
//
int   CheckFilter(LPCTRLZONEMAP pctrlzm)
{
  return pctrlzm->iFiltered;
}


//==================================================
//FUNCTION: StartControlDataFilter
//
//
//
void    StartControlDataFilter(int iPhisChann, LPMIXERWNDDATA lpmwd_work, LPCTRLZONEMAP lpctrlZM, BOOL bIsOn, BOOL bCheckGroups)
{
  CONTROLDATA         ctrlData;
  LPCTRLZONEMAP       lpctrl, lpctrl2;
  int                 i_ctrl_pos;
  int                 i_temp1;
  BOOL                bUpdateCtrl = TRUE;
	LPMIXERWNDDATA			lpmwd = lpmwd_work;


	if (lpmwd->lpZoneMap->iZonesCount < 100)
		lpmwd = GetValidMixerWindowData ();

  switch(lpctrlZM->iCtrlType)
  {

		/////////////////////////
		// INPUT MUTE !!!!!!!!!!

	  case CTRL_TYPE_BTN_INPUTMUTE_FILTER:

			if(lpmwd->lpZoneMap[iPhisChann].lpZoneMap == NULL)
				break;
			if(bIsOn == TRUE)
			{
				// Turn it ON

				/////////////////////////////////////////////////////////
				// Find the Volume control and pull it all the way down..
				//

				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, CTRL_NUM_INPUT_VOL_FADER);

				// Send the Data out
				//------------------
				ctrlData.wMixer   = 0;
				ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
				ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
				ctrlData.wVal     = CDef_GetCtrlMaxVal(lpctrl->iCtrlNumAbs) - 1;
				SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
      
				SetFilteredControlsByNumber(lpctrl, YES_FILTER);
				SetAlternativeZMFilter(lpctrl, YES_FILTER, lpmwd, iPhisChann);
      
				/////////////////////////////////////////////////////////
				// Find the Volume control and pull it all the way down..
				//

				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, CTRL_NUM_INPUT_GATE_FEED_THRU);

				// Send the Data out
				//------------------
				ctrlData.wMixer   = 0;
				ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
				ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
				ctrlData.wVal     = CDef_GetCtrlMaxVal(lpctrl->iCtrlNumAbs) - 1;
				SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

				SetFilteredControlsByNumber(lpctrl, YES_FILTER);
				SetAlternativeZMFilter(lpctrl, YES_FILTER, lpmwd, iPhisChann);

				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, CTRL_NUM_INPUT_GATE_FEEDTHRUINOUT);
				SetFilteredControlsByNumber(lpctrl, YES_FILTER);
				SetAlternativeZMFilter(lpctrl, YES_FILTER, lpmwd, iPhisChann);

			}
			else
			{
							// Turn it OFF

				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, CTRL_NUM_INPUT_GATE_FEEDTHRUINOUT);
				if(GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos) == 0)
				{
					// Send the Data out
					//------------------
					ctrlData.wMixer   = 0;
					ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
					ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
					ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
					SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

					SetFilteredControlsByNumber(lpctrl, NO_FILTER);
					SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, iPhisChann);

					// Find the Volume control and recall it current value
					//
					lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, CTRL_NUM_INPUT_GATE_FEED_THRU);
					// Send the Data out
					//------------------
					ctrlData.wMixer   = 0;
					ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
					ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
					ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
					SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
      
					SetFilteredControlsByNumber(lpctrl, NO_FILTER);
					SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, iPhisChann);
				}
				else
				{
					//////////////////////////////////////////////////////////////////////////////
					// Remove the filtering on this thing .... but thats all we should need to do

					SetFilteredControlsByNumber(lpctrl, NO_FILTER);
					SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, iPhisChann);

					ctrlData.wMixer   = 0;
					ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
					ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
					ctrlData.wVal     = 1;
					SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
				}

				/*
				// This is done to accomodate a very special logic
				// talk to gamble
				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, CTRL_NUM_INPUT_GATE_FEEDTHRUINOUT);
				if(GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos) > 0)
				{
					// Send the Data out
					//------------------
					ctrlData.wMixer   = 0;
					ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
					ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
					ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
					SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

					SetFilteredControlsByNumber(lpctrl, NO_FILTER);
					SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, iPhisChann);
				}
    		*/

				/////////////////////////////////////////////////////////////
				// Find the Volume control and pull it all the way down..
				//

				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, CTRL_NUM_INPUT_VOL_FADER);

				// Send the Data out
				//------------------
				ctrlData.wMixer   = 0;
				ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
				ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
				ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
				SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

				SetFilteredControlsByNumber(lpctrl, NO_FILTER);
				SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, iPhisChann);
			}
			break;
		//
		case	CTRL_TYPE_INPUT_GATE_IN_BTN_FILTER:
			if (lpctrlZM->iFiltered == YES_FILTER)
				return;
			if(bIsOn == TRUE){
				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, 
																		CTRL_NUM_INPUT_GATE_FEED_THRU);
				if(lpctrl == NULL)
					return; /// Doh ... nothing in here ...

				// Send the Data out
				//------------------
				ctrlData.wMixer   = 0;
				ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
				ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
				ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
				SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
				SetFilteredControlsByNumber(lpctrl, NO_FILTER);
			}else{
				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, 
																		CTRL_NUM_INPUT_GATE_FEED_THRU);
				if(lpctrl == NULL)
					return; /// Doh ... nothing in here ...

				SetFilteredControlsByNumber(lpctrl, YES_FILTER);
			}

			break;


		//////////////////////////////////
		// LINE B TO AUX 16 LT/RT MUTES

		case CTRL_INPUT_AUX16B_MUTE_FILTER:

			if(lpmwd->lpZoneMap[iPhisChann].lpZoneMap == NULL)
				break;

			if(bIsOn == TRUE)
			{         
				////////////////////////////////////////////////////////////////////////////
				// iCtrlChanPos is the control number of the button clicked on
				// for this case it is either CTRL_NUM_INPUT_AUX16BLT_MUTE (0x8a) or 
				// CTRL_NUM_INPUT_AUX16BRT_MUTE (0x8b) We use the control numbers to offset
				// into the zonemap so we can use the same code below for both mutes
				//
				// We need to look for the fader for this control (CTRL_NUM_INPUT_AUX16LT_LINEB_FADER) and pull it down.

				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, 
																		(lpctrlZM->iCtrlChanPos - CTRL_NUM_INPUT_AUX16BLT_MUTE) +
																		CTRL_NUM_INPUT_AUX16LT_LINEB_FADER);
				// Send the Data out
				//------------------
				ctrlData.wMixer   = 0;
				ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
				ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
				ctrlData.wVal     = CDef_GetCtrlMaxVal(lpctrl->iCtrlNumAbs) - 1;
				SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
      
				SetFilteredControlsByNumber(lpctrl, YES_FILTER);
				SetAlternativeZMFilter(lpctrl, YES_FILTER, lpmwd, iPhisChann);

			}
			else
			{
				/////////////////////////////////////////////////////////////////////
				// Same as above but now need to put the fader back to where it was

				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, 
																		(lpctrlZM->iCtrlChanPos - CTRL_NUM_INPUT_AUX16BLT_MUTE) +
																		CTRL_NUM_INPUT_AUX16LT_LINEB_FADER);
				// Send the Data out
				//------------------
				ctrlData.wMixer   = 0;
				ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
				ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
				ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
				SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

				SetFilteredControlsByNumber(lpctrl, NO_FILTER);
				SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, iPhisChann);

			}
      
			break;

		
		case CTRL_TYPE_BTN_AUXMUTE_FILTER:

			if(lpmwd->lpZoneMap[iPhisChann].lpZoneMap == NULL)
				break;
			if(bIsOn == TRUE)
			{         
				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, 
																		(lpctrlZM->iCtrlChanPos - CTRL_NUM_INPUT_AUX01LT_MUTE) +
																		CTRL_NUM_INPUT_AUX1LT_FADER);
				// Send the Data out
				//------------------
				ctrlData.wMixer   = 0;
				ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
				ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
				ctrlData.wVal     = CDef_GetCtrlMaxVal(lpctrl->iCtrlNumAbs) - 1;
				SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
      
				SetFilteredControlsByNumber(lpctrl, YES_FILTER);
				SetAlternativeZMFilter(lpctrl, YES_FILTER, lpmwd, iPhisChann);

			}
			else
			{
				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, 
																		(lpctrlZM->iCtrlChanPos - CTRL_NUM_INPUT_AUX01LT_MUTE) +
																		CTRL_NUM_INPUT_AUX1LT_FADER);
				// Send the Data out
				//------------------
				ctrlData.wMixer   = 0;
				ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
				ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
				ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
				SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

				SetFilteredControlsByNumber(lpctrl, NO_FILTER);
				SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, iPhisChann);

			}
      
			break;

		/////////////////////////////////////////////////////////////////////////////////////
		// Now this is not a true filter however it has "long" effect over the given controls 
		// it effects ...

		case CTRL_TYPE_BTN_INPUT_AUXPOST_FILTER:

			if(lpmwd->lpZoneMap[iPhisChann].lpZoneMap == NULL)
				break;

			i_ctrl_pos =  (lpctrlZM->iCtrlChanPos - CTRL_NUM_INPUT_AUX01LT_POST) +
																	CTRL_NUM_INPUT_AUX1LT_FADER;
			lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, i_ctrl_pos);

			if(bIsOn == TRUE)
				i_temp1 = lpctrlZM->iCtrlNumAbsAlt1;
			else
				i_temp1 = lpctrlZM->iCtrlNumAbs;

			if(lpctrl)
			{
				lpctrl2 = lpctrl;
				bUpdateCtrl = TRUE;
			}
			else
			{
				lpctrl2 = NULL;
				bUpdateCtrl = FALSE;
			}

			//////////////////
			// main stuff ....

			while(lpctrl)
			{
				lpctrl->iCtrlNumAbs = i_temp1;
				if(lpctrl->iFiltered == YES_FILTER)
					bUpdateCtrl = FALSE;

				lpctrl = ScanCtrlZonesNum(++lpctrl, i_ctrl_pos);
			}

			///////////////////////////////
			// now update the control data

			if(bUpdateCtrl && lpctrl2 != NULL)
			{
				ctrlData.wMixer   = 0;
				ctrlData.wChannel = lpctrl2->iModuleNumber;//iChannel;
				ctrlData.wCtrl    = lpctrl2->iCtrlNumAbs; // we use this one since for the definition dll
				ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl2, lpctrl2->iCtrlChanPos);

				SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
			}

			break;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case CTRL_MASTER_MUTE_FILTER:
    
			if(lpmwd->lpZoneMap[iPhisChann].lpZoneMap == NULL)
				break;
			if(bIsOn == TRUE)
			{         
				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, CTRL_NUM_MASTERLT_FADER);
				// Send the Data out
				//------------------
				ctrlData.wMixer   = 0;
				ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
				ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
				ctrlData.wVal     = CDef_GetCtrlMaxVal(lpctrl->iCtrlNumAbs) - 1;
				SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
      
				SetFilteredControlsByNumber(lpctrl, YES_FILTER);
				SetAlternativeZMFilter(lpctrl, YES_FILTER, lpmwd, iPhisChann);

				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, CTRL_NUM_MASTERRT_FADER);
				// Send the Data out
				//------------------
				ctrlData.wMixer   = 0;
				ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
				ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
				ctrlData.wVal     = CDef_GetCtrlMaxVal(lpctrl->iCtrlNumAbs) - 1;
				SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
      
				SetFilteredControlsByNumber(lpctrl, YES_FILTER);
				SetAlternativeZMFilter(lpctrl, YES_FILTER, lpmwd, iPhisChann);

			}
			else
			{
				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, CTRL_NUM_MASTERLT_FADER);
				// Send the Data out
				//------------------
				ctrlData.wMixer   = 0;
				ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
				ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
				ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
				SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

				SetFilteredControlsByNumber(lpctrl, NO_FILTER);
				SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, iPhisChann);

				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, CTRL_NUM_MASTERRT_FADER);
				// Send the Data out
				//------------------
				ctrlData.wMixer   = 0;
				ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
				ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
				ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
				SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

				SetFilteredControlsByNumber(lpctrl, NO_FILTER);
				SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, iPhisChann);
			}
			break;

		case CTRL_INPUT_MICA_MUTE_FILTER:
		case CTRL_INPUT_LINEA_MUTE_FILTER:
		case CTRL_INPUT_MICB_MUTE_FILTER:
		case CTRL_INPUT_LINEB_MUTE_FILTER:
			HandleInputSignalInMuteFilter(iPhisChann, lpmwd, lpctrlZM, bIsOn);
			break;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case CTRL_MATRIX_SUB_MUTE_FILTER:
			HandleMatrixSubMuteFilter(iPhisChann, lpmwd, lpctrlZM, bIsOn);
			break;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case CTRL_MATRIX_MONO_MUTE_FILTER:
			HandleMatrixMonoMuteFilter(iPhisChann, lpmwd, lpctrlZM, bIsOn);
			break;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case CTRL_CUE_MASTER_MUTE_FILTER:
			HandleCueMasterMuteFilter(g_iMasterModuleIdx, lpmwd, lpctrlZM, bIsOn);
			break;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case CTRL_MATRIX_CENTER_MUTE_FILTER:
			HandleMatrixCenterMuteFilter(iPhisChann, lpmwd, lpctrlZM, bIsOn);
			break;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case CTRL_MATRIX_MASTER_LT_MUTE_FILTER:
		case CTRL_MATRIX_MASTER_RT_MUTE_FILTER:
			HandleMatrixMasterMuteFilter(iPhisChann, lpmwd, lpctrlZM, bIsOn);
			break;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case CTRL_SUB_SUMIN_MUTE_FILTER:		
			HandleSubSuminMuteFilter(iPhisChann, lpmwd, lpctrlZM, bIsOn);
			break;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case CTRL_MATRIX_AUX_MUTE_FILTER:
			HandleMatrixAuxMuteFilter(iPhisChann, lpmwd, lpctrlZM, bIsOn);
			break;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case CTRL_MASTER_AUX_MUTE_FILTER:
			HandleMasterAuxMuteFilter(iPhisChann, lpmwd, lpctrlZM, bIsOn);
			break;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`
		case CTRL_MARIXLT_MUTE_FILTER:
  
			if(lpmwd->lpZoneMap[iPhisChann].lpZoneMap == NULL)
				break;

			if(bIsOn == TRUE)
			{         
				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap,CTRL_NUM_MATRIX_LT_FADER);
				// Send the Data out
				//------------------
				ctrlData.wMixer   = 0;
				ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
				ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
				ctrlData.wVal     = CDef_GetCtrlMaxVal(lpctrl->iCtrlNumAbs) - 1;
				SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
      
				SetFilteredControlsByNumber(lpctrl, YES_FILTER);
				SetAlternativeZMFilter(lpctrl, YES_FILTER, lpmwd, iPhisChann);

			}
			else
			{
				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, CTRL_NUM_MATRIX_LT_FADER);
				// Send the Data out
				//------------------
				ctrlData.wMixer   = 0;
				ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
				ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
				ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
				SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

				SetFilteredControlsByNumber(lpctrl, NO_FILTER);
				SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, iPhisChann);

			}
			break;

		case CTRL_MARIXRT_MUTE_FILTER:

			if(lpmwd->lpZoneMap[iPhisChann].lpZoneMap == NULL)
				break;
			if(bIsOn == TRUE)
			{         
				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap, CTRL_NUM_MATRIX_RT_FADER);
				// Send the Data out
				//------------------
				ctrlData.wMixer   = 0;
				ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
				ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
				ctrlData.wVal     = CDef_GetCtrlMaxVal(lpctrl->iCtrlNumAbs) - 1;
				SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);
      
				SetFilteredControlsByNumber(lpctrl, YES_FILTER);
				SetAlternativeZMFilter(lpctrl, YES_FILTER, lpmwd, iPhisChann);

			}
			else
			{
				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iPhisChann].lpZoneMap,CTRL_NUM_MATRIX_RT_FADER);
				// Send the Data out
				//------------------
				ctrlData.wMixer   = 0;
				ctrlData.wChannel = lpctrl->iModuleNumber;//iChannel;
				ctrlData.wCtrl    = lpctrl->iCtrlNumAbs; // we use this one since for the definition dll
				ctrlData.wVal     = GETPHISDATAVALUE(lpmwd->iMixer, lpctrl, lpctrl->iCtrlChanPos);
				SendDataToDevice(&ctrlData, FALSE, NULL, 0, lpmwd, TRUE);

				SetFilteredControlsByNumber(lpctrl, NO_FILTER);
				SetAlternativeZMFilter(lpctrl, NO_FILTER, lpmwd, iPhisChann);

			}
			break;

		case CTRL_TYPE_BTN_MASTER_AUX_PREPOST_FILTER:
			break;

		//////////////////////////////////////
		// Master headphones pre/post filter

		case CTRL_TYPE_BTN_MASTER_HEADPOST_FILTER:
			HandleMasterHeadphonesPostFilter(iPhisChann, lpmwd, lpctrlZM, bIsOn);
			break;

		case CTRL_TYPE_BTN_AUX_MMATRIX_LT_POST_FILTER:
		case CTRL_TYPE_BTN_AUX_MMATRIX_RT_POST_FILTER:
			HandleSubAuxMasterMatrixFilter(iPhisChann, lpmwd, lpctrlZM, bIsOn);
			break;

		////////////////////////////////////////////////
		// EQ RESET FILTER - resets the EQ settings on
		// this channel to default.

		case CTRL_TYPE_BTN_EQ_RESET_FILTER:
			HandleResetEQFilter(iPhisChann, lpmwd, lpctrlZM, bIsOn);
			break;

		default:
			return;
			break;
  }
  
	////////////////////////////
  // Handle Grouped Mutes ....
  //
  if(bCheckGroups)
    UpdateGroupedMutes(lpctrlZM, lpmwd);
};

/////////////////////////////////////////////////////////////////////
// FUNCTION: UpdateExternalInterface
//
// purpose: 
//
//
//
void	UpdateExternalInterface(CONTROLDATA *pCtrlData)
{
	WORD		wVal;

	if(pCtrlData == NULL)
		return;
		
	//////////////////////
	// is this a fader ..

	if(pCtrlData->wCtrl == gExternalIface[0].ctrlId)
	{
		// invert the value
		//
		wVal = pCtrlData->wVal;
		wVal = (WORD)((float)wVal * gExternalIface[0].fscale);
		wVal = 0x7f - wVal;
		
		// FDS - REMOVE LINE BELOW FOR DEBUGGING

		SendCommData((BYTE)gExternalIface[0].chann, (BYTE)pCtrlData->wChannel, (BYTE)wVal);
	}
}

/////////////////////////////////////////////////////////////////////
// FUNCTION: UpdateFromExternalInterface
//
// purpose: 
//
//
void	UpdateFromExternalInterface()
{
	MIDI_SHORT_DATA	mmb;
	WORD						wVal;
	CONTROLDATA			ctrlData;
	LPCTRLZONEMAP		pctrlzm;
	LPMIXERWNDDATA	lpmwd;

	
	/////////////////////////
	// keep pulling data ...

	while(GetCommInFifoSize() > 3)
	{
		GetCommInFifo((LPSTR)&mmb.data, 3);		

		switch(mmb.s.chan)
		{
			// Is it a fader move.
			case 13:
				if(mmb.s.d1 < 80 && mmb.s.d2 < 0x7f)
				{
					wVal = 0x7f - mmb.s.d2;
					wVal = (WORD)((float)wVal / gExternalIface[0].fscale);
					UpdateControlFromNetwork(mmb.s.d1, CTRL_NUM_INPUT_VOL_FADER, wVal, FALSE);

					lpmwd = GetValidMixerWindowData();
					pctrlzm = ScanCtrlZonesNum(gpZoneMaps_Zoom[mmb.s.d1].lpZoneMap, CTRL_NUM_INPUT_VOL_FADER);
					if(lpmwd && pctrlzm)
					{
						ctrlData.wMixer = 0;
						ctrlData.wChannel = mmb.s.d1;
						ctrlData.wCtrl = gExternalIface[0].ctrlId;
						ctrlData.wVal = wVal;						
						// (lpmwd->wKeyFlags & MK_SHIFT)?FALSE:TRUE is this needed or not?!


						//////////////////////////////////////////////////////////////////
						// Where is my JLCooper unit?
						SendDataToDevice(&ctrlData, FALSE, pctrlzm, 0, lpmwd, FALSE);
					}
				}
				break;

			default:
				break;
		}
	}

}


/////////////////////////////////////////////////////////////////////
// FUNCTION: UpdateFromExternalInterface
//
// purpose: 
//
// Intricate counting system for the Cue scilent system.
//
void	syncInputPriority (LPCTRLZONEMAP pctrlzm, int	icount, LPMIXERWNDDATA lpmwd)
{
	int							ret;
	int							chanPos;
	int							cueActive = gCueActiveCount;
	int							cue_count;

	if ( pctrlzm == 0 || lpmwd == 0)
		return;


	gCueActiveCount = 0;
	PrepareCueMasterSystem ();
	cue_count = g_cue_priority.aux + g_cue_priority.input +
							g_cue_priority.master + g_cue_priority.matrix +
							g_cue_priority.sub;
	
	if (gDeviceSetup.iaChannelTypes[pctrlzm->iModuleNumber] == 1)
	{
		chanPos = pctrlzm->iCtrlChanPos;
		if(chanPos == CTRL_NUM_INPUT_CUE_FAD_PRE || 
			 chanPos == CTRL_NUM_INPUT_CUE_FAD_POST ||
			 chanPos == CTRL_NUM_INPUT_MIC_B_CUE || 
			 chanPos == CTRL_NUM_INPUT_MIC_A_CUE || 
			 chanPos == CTRL_NUM_INPUT_GATE_KEY_INOUT)
		{
			ret = g_cue_priority.input;//countInputCuesOn ();
			if (ret < 1)
			{
				g_cue_priority.input = 0;
				g_inputCueActiveCounter = 0;
				handleInputCuePriority (lpmwd, FALSE);
			}
			if (ret > 0)
			{
				g_cue_priority.input = ret;
				g_inputCueActiveCounter = ret;
				handleInputCuePriority (lpmwd, TRUE);
			}

		}
	}

	
	if (cueActive > 0)
	{
		if (cue_count < 1)
		{
			HandleCueMasterMuteFilterEx(g_iMasterModuleIdx, lpmwd, NULL, FALSE);
			g_CueMasterSystem  = 1;
		}
		else
		{
			/////////////////////////////
			// aha we need to turn it on

			if (g_CueMasterSystem == 1)
			{
				HandleCueMasterMuteFilterEx(g_iMasterModuleIdx, lpmwd, NULL, TRUE);
				g_CueMasterSystem  = 0;
			}
		}
	} else
	{
		if (cue_count > 0)
		{
			HandleCueMasterMuteFilterEx(g_iMasterModuleIdx, lpmwd, NULL, TRUE);
			g_CueMasterSystem  = 0;
		}

	}
}


/////////////////////////////////////////////////////////////////////
// FUNCTION: isChanelSafeActive
//
// purpose: 
//
//
BOOL	isChanelSafeActive (LPCTRLZONEMAP pctrlzm)
{
	BOOL	bret = FALSE;

	///////////////////////////////////////////////////
	// make sure this effects only the input channels

	if (pctrlzm && gDeviceSetup.iaChannelTypes[pctrlzm->iModuleNumber] == 1) {
		LPCTRLZONEMAP safecontrol = ScanCtrlZonesNum ( pctrlzm, CTRL_NUM_INPUT_SAFE);
		if (safecontrol){
			int iVal = GETPHISDATAVALUE(0, safecontrol, safecontrol->iCtrlChanPos);
			if (iVal == 0)
				bret = TRUE;
			}
		}
	return bret;
}


/////////////////////////////////////////////////////////////////////
// FUNCTION: setBufferChannelSafeActive
//
// purpose: 
//
//
void	setBufferChannelSafeActive (LPCTRLZONEMAP pctrlzm, BOOL active)
{
	////////////////////////////////////////////////////
	// make sure this effects only the input channels

	if (pctrlzm && gDeviceSetup.iaChannelTypes[pctrlzm->iModuleNumber] == 1) {
		LPCTRLZONEMAP safecontrol = ScanCtrlZonesNum ( pctrlzm, CTRL_NUM_INPUT_SAFE);
		if (safecontrol && gDeviceSetup.iaChannelTypes[safecontrol->iModuleNumber] == 1){
			if (active)
				SETPHISDATAVALUEBUFFER(0, safecontrol, CTRL_NUM_INPUT_SAFE, 0);
			else
				SETPHISDATAVALUEBUFFER(0, safecontrol, CTRL_NUM_INPUT_SAFE, 2);
			}
		}
}


/////////////////////////////////////////////////////////////////////
// FUNCTION: SendDataToDevice
//
// purpose: 
//  This function is a wrapper around the Normal way we send data.
//  It gives us a chance to accomodate Filters and Groups over any 
//  control in the System
//
//
extern	int	g_CueMasterSystem;
extern  int g_inputCueActiveCounter;
void SendDataToDevice(CONTROLDATA *pCtrlData, BOOL bUseGroups, 
                      LPCTRLZONEMAP pctrlzm, int iDelta, LPMIXERWNDDATA lpmwd, BOOL bExternal)
{
  CONTROLDATA  CtrlDataCopy = *pCtrlData; 
	int					 cueMasterSystem = g_CueMasterSystem;
	int					 inputCueActiveCounter = g_inputCueActiveCounter;


  if(pctrlzm != NULL)
    if(CheckFilter(pctrlzm) == YES_FILTER)
      goto CHECK_FOR_GROUPS;

	////////////////////////////////////////////////////
  // Now send the data before dealing with the Groups
  //
  if(pCtrlData->wChannel < MAX_CHANNELS)
	{
		CDef_SendData(pCtrlData);
		//
		if(bExternal)
			UpdateExternalInterface(pCtrlData);

	}

  
CHECK_FOR_GROUPS:
	//////////////////////////////////////////////////////////////////
  // Update Stereo Controls and also make sure to update the Groups 
  // if they are activated ...
  //

  if((bUseGroups == TRUE) && (pCtrlData->wChannel < MAX_CHANNELS))
    UpdateStereoControls(&CtrlDataCopy, pctrlzm, iDelta, lpmwd);

	/////////////////////////////
  // Update grouped controls
  //
  if((bUseGroups == TRUE) && pCtrlData->wChannel < MAX_CHANNELS && pctrlzm != 0)
    UpdateGroupedControls(pCtrlData, pctrlzm, iDelta, lpmwd, FALSE);

	if (g_monitor_mix_file_changes == TRUE)
		g_mixer_state_changed = TRUE;

  // Cleanup here .....
  //
  return;
};


/////////////////////////////////////////////////////////////////////
// FUNCTION: FlipTheControl
//
// purpose: 
//
//
void FlipTheControl(LPCTRLZONEMAP  lpctrlZM, LPMIXERWNDDATA lpmwd_work)
{

  CONTROLDATA         ctrlData;
  int                 iVal, ivalue;
	LPMIXERWNDDATA			lpmwd = lpmwd_work;


	if (lpmwd->lpZoneMap->iZonesCount < 100)
		lpmwd = GetValidMixerWindowData ();
  ivalue = CDef_GetCtrlMinVal(lpctrlZM->iCtrlNumAbs);
  iVal = GETPHISDATAVALUE(0, lpctrlZM, lpctrlZM->iCtrlChanPos);
  if(iVal != ivalue)
  {
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrlZM->iModuleNumber;//iPhisChannel;
    ctrlData.wCtrl    = lpctrlZM->iCtrlNumAbs; // we use this one since for the definition dll

		//////////////////////////////////////////////
    // ok we need to go down to the minimum value

    iVal = CDef_GetCtrlMaxVal(lpctrlZM->iCtrlNumAbs) - 1;
    if(iVal == ivalue)
      iVal++;
    for(iVal; iVal >= ivalue; iVal --)
    {
      // Send the Data out
      //------------------
      ctrlData.wVal     = iVal;
      SendDataToDevice(&ctrlData, TRUE, lpctrlZM, -1, lpmwd, TRUE);
      //SendDataToDevice(&ctrlData, (lpmwd->wKeyFlags & MK_SHIFT)?FALSE:TRUE, lpctrlZM, -1, lpmwd, TRUE);
    }

    iVal = ivalue;
  }
  else
  {
    ctrlData.wMixer   = 0;
    ctrlData.wChannel = lpctrlZM->iModuleNumber;//iPhisChannel;
    ctrlData.wCtrl    = lpctrlZM->iCtrlNumAbs; // we use this one since for the definition dll

		/////////////////////////////////////
    // ok we need to go up to max value

    ivalue = CDef_GetCtrlMaxVal(lpctrlZM->iCtrlNumAbs);
    iVal   = CDef_GetCtrlMinVal(lpctrlZM->iCtrlNumAbs);
    for(iVal; iVal < ivalue; iVal++)
    {
      // Send the Data out
      //------------------
      ctrlData.wVal     = iVal;
      SendDataToDevice(&ctrlData, TRUE, lpctrlZM, 1, lpmwd, TRUE);
      //SendDataToDevice(&ctrlData, (lpmwd->wKeyFlags & MK_SHIFT)?FALSE:TRUE, lpctrlZM, 1, lpmwd, TRUE);
    }
  }

	iVal --;
  // Set the Phisical Data Value
  //----------------------------
  SETPHISDATAVALUE(0, lpctrlZM, lpctrlZM->iCtrlChanPos, iVal);

  //
  //
    UpdateControlFromNetwork(ctrlData.wChannel, (WORD)lpctrlZM->iCtrlChanPos, (int)ctrlData.wVal, FALSE);
  //UpdateControlFromNetwork(ctrlData.wChannel, ctrlData.wCtrl, (int)ctrlData.wVal, TRUE);

}


/////////////////////////////////////////////////////////////////////
// FUNCTION: FlipHardwareControl
//
// purpose: 
//
//
//
void FlipHardwareControl(LPCTRLZONEMAP  lpctrlZM, LPMIXERWNDDATA lpmwd_work)
{

  CONTROLDATA         ctrlData;
  int                 iVal, ivalue;
	LPMIXERWNDDATA			lpmwd = lpmwd_work;


	if (lpmwd->lpZoneMap->iZonesCount < 100)
		lpmwd = GetValidMixerWindowData ();


  ivalue = CDef_GetCtrlMinVal(lpctrlZM->iCtrlNumAbs);
  iVal = GETPHISDATAVALUE(0, lpctrlZM, lpctrlZM->iCtrlChanPos);
  if(iVal != ivalue)
  {
		iVal = ivalue;		
  }
  else
  {
		iVal = CDef_GetCtrlMaxVal(lpctrlZM->iCtrlNumAbs) - 1;
  }


  // Set the Phisical Data Value
  //----------------------------
  SETPHISDATAVALUE(0, lpctrlZM, lpctrlZM->iCtrlChanPos, iVal);

  ctrlData.wMixer   = 0;
  ctrlData.wChannel = lpctrlZM->iModuleNumber;//iPhisChannel;
  ctrlData.wCtrl    = lpctrlZM->iCtrlNumAbs; // we use this one since for the definition dll
	ctrlData.wVal     = iVal;

  SendDataToDevice(&ctrlData, TRUE, lpctrlZM, 1, lpmwd, TRUE);	// TURN OFF the other one

	//if ( ! lpmwd->wKeyFlags & MK_SHIFT)
		UpdateGroupedControls(&ctrlData, lpctrlZM, 1, NULL, TRUE);

  //
  //
    UpdateControlFromNetwork(ctrlData.wChannel, (WORD)lpctrlZM->iCtrlChanPos, (int)ctrlData.wVal, FALSE);
  //UpdateControlFromNetwork(ctrlData.wChannel, ctrlData.wCtrl, (int)ctrlData.wVal, TRUE);

}


/////////////////////////////////////////////////////////////////////
// FUNCTION: HandleMasterCueSwitch
//
// purpose: 
//
//
void HandleMasterCueSwitch(LPMIXERWNDDATA lpmwd, WORD wVal)
{
		// handle the master Cue switch
		if(wVal == 0)
		{
			g_cue_priority.input --; if (g_cue_priority.input < 0) g_cue_priority.input = 0;
			HandleCueMasterMuteFilterEx(g_iMasterModuleIdx, lpmwd, NULL, FALSE);
			handleInputCuePriority (lpmwd, FALSE);
		}
		else
		{
			g_cue_priority.input ++;
			HandleCueMasterMuteFilterEx(g_iMasterModuleIdx, lpmwd, NULL, TRUE);
			handleInputCuePriority (lpmwd, TRUE);
		}
}

/////////////////////////////////////////////////////////////////////
// FUNCTION: HandleInputToggleSwtches
//
// purpose: 
//
//
void HandleInputToggleSwtches(LPMIXERWNDDATA lpmwd_work, LPCTRLZONEMAP pctrlzm)
{
  LPCTRLZONEMAP       lpctrl;
  WORD                wVal;
	LPMIXERWNDDATA			lpmwd = lpmwd_work;


	if (lpmwd->lpZoneMap->iZonesCount < 100)
		lpmwd = GetValidMixerWindowData ();

  switch(pctrlzm->iCtrlChanPos)
  {
	//////////////////////////////
	case CTRL_NUM_INPUT_MIC_A_CUE:
    wVal = GETPHISDATAVALUE(0, pctrlzm, pctrlzm->iCtrlChanPos);
    if(wVal > 0){
      lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[pctrlzm->iModuleNumber].lpZoneMap, CTRL_NUM_INPUT_MIC_B_CUE);
			if(lpctrl)
			{
				if(GETPHISDATAVALUE(0, lpctrl, lpctrl->iCtrlChanPos) == 0)
				{
					///////////////////////////////
					// aha ... turn off this duda
					// 
					FlipHardwareControl (lpctrl, lpmwd);
					gCueActiveCount --;
					g_cue_priority.input --; if (g_cue_priority.input < 0) g_cue_priority.input = 0;
					g_inputCueActiveCounter --;
				}
			}
      lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[pctrlzm->iModuleNumber].lpZoneMap, CTRL_NUM_INPUT_GATE_KEY_INOUT);
			if(lpctrl)
			{
				if(GETPHISDATAVALUE(0, lpctrl, lpctrl->iCtrlChanPos) == 0)
				{
					//////////////////////////////
					// aha ... turn off this duda
					// 
					FlipHardwareControl (lpctrl, lpmwd);
					gCueActiveCount --;
					g_cue_priority.input --; if (g_cue_priority.input < 0) g_cue_priority.input = 0;
					g_inputCueActiveCounter --;
				}
			}
    }

//		HandleMasterCueSwitch(lpmwd, wVal);		// Now called directly from HandleCtrlBtnClick()

		break;

	//////////////////////////////
	case CTRL_NUM_INPUT_MIC_B_CUE:
    wVal = GETPHISDATAVALUE(0, pctrlzm, pctrlzm->iCtrlChanPos);
    if(wVal > 0)
		{
      lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[pctrlzm->iModuleNumber].lpZoneMap, CTRL_NUM_INPUT_MIC_A_CUE);
			if(lpctrl){
				if(GETPHISDATAVALUE(0, lpctrl, lpctrl->iCtrlChanPos) == 0)
				{

					//////////////////////////////
					// aha ... turn off this duda
					// 
					FlipHardwareControl (lpctrl, lpmwd);
					gCueActiveCount --;
					g_cue_priority.input --; if (g_cue_priority.input < 0) g_cue_priority.input = 0;
					g_inputCueActiveCounter --;
				}
			}
      lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[pctrlzm->iModuleNumber].lpZoneMap, CTRL_NUM_INPUT_GATE_KEY_INOUT);
			if(lpctrl){
				if(GETPHISDATAVALUE(0, lpctrl, lpctrl->iCtrlChanPos) == 0)
				{
					///////////////////////////////
					// aha ... turn off this duda
					// 
					FlipHardwareControl (lpctrl, lpmwd);
					gCueActiveCount --;
					g_cue_priority.input --; if (g_cue_priority.input < 0) g_cue_priority.input = 0;
					g_inputCueActiveCounter --;
				}
			}
    }

//		HandleMasterCueSwitch(lpmwd, wVal);		// Now called directly from HandleCtrlBtnClick()

		break;

	///////////////////////////////////
	case CTRL_NUM_INPUT_GATE_KEY_INOUT:
    wVal = GETPHISDATAVALUE(0, pctrlzm, pctrlzm->iCtrlChanPos);
    if(wVal > 0)
		{
      lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[pctrlzm->iModuleNumber].lpZoneMap, CTRL_NUM_INPUT_MIC_A_CUE);
			if(lpctrl){
				if(GETPHISDATAVALUE(0, lpctrl, lpctrl->iCtrlChanPos) == 0)
				{
					///////////////////////////////
					// aha ... turn off this duda
					// 
					FlipHardwareControl (lpctrl, lpmwd);
					gCueActiveCount --;
					g_cue_priority.input --; if (g_cue_priority.input < 0) g_cue_priority.input = 0;
					g_inputCueActiveCounter --;
				}
			}
      lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[pctrlzm->iModuleNumber].lpZoneMap, CTRL_NUM_INPUT_MIC_B_CUE);
			if(lpctrl){
				if(GETPHISDATAVALUE(0, lpctrl, lpctrl->iCtrlChanPos) == 0)
				{
					////////////////////////////////
					// aha ... turn off this duda
					// 
					FlipHardwareControl (lpctrl, lpmwd);
					gCueActiveCount --;
					g_cue_priority.input --; if (g_cue_priority.input < 0) g_cue_priority.input = 0;
					g_inputCueActiveCounter --;
				}
			}
    }

//    wVal = GETPHISDATAVALUE(0, pctrlzm, pctrlzm->iCtrlChanPos);
//		HandleMasterCueSwitch(lpmwd, wVal);		// Now called directly from HandleCtrlBtnClick()

		break;

	///////////////////////////////
  case CTRL_NUM_INPUT_CUE_FAD_PRE:
		
    wVal = GETPHISDATAVALUE(0, pctrlzm, CTRL_NUM_INPUT_CUE_FAD_POST);
    if(wVal == 0)
		{
			gCueActiveCount --;
			g_cue_priority.input --;
			g_inputCueActiveCounter --;
      lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[pctrlzm->iModuleNumber].lpZoneMap, CTRL_NUM_INPUT_CUE_FAD_POST);
      
			///////////////////////////////
			// aha ... turn off this duda
      // 

			FlipHardwareControl (lpctrl, lpmwd);
    }
		
		wVal = GETPHISDATAVALUE(0, pctrlzm, pctrlzm->iCtrlChanPos);

		HandleMasterCueSwitch(lpmwd, wVal);

    break;

  //////////////////////////////////
  case CTRL_NUM_INPUT_CUE_FAD_POST:
		
    wVal = GETPHISDATAVALUE(0, pctrlzm, CTRL_NUM_INPUT_CUE_FAD_PRE);
    if(wVal == 0)
		{
			gCueActiveCount--;
			g_cue_priority.input --;
			g_inputCueActiveCounter --;
			
      lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[pctrlzm->iModuleNumber].lpZoneMap, CTRL_NUM_INPUT_CUE_FAD_PRE);

			////////////////////////////////
      // aha ... turn off this duda
      // 

			FlipHardwareControl (lpctrl, lpmwd);
			
    }
		
		/////////////////////////////////
		// handle the master Cue switch

		wVal = GETPHISDATAVALUE(0, pctrlzm, pctrlzm->iCtrlChanPos);

		HandleMasterCueSwitch(lpmwd, wVal);

    break;

  ///////////////////////////////
  case CTRL_NUM_INPUT_GATE_KEY_VU:
    wVal = GETPHISDATAVALUE(0, pctrlzm, CTRL_NUM_INPUT_LINE_B_VU);
    if(wVal == 0)
    {
      lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[pctrlzm->iModuleNumber].lpZoneMap, CTRL_NUM_INPUT_LINE_B_VU);
      
			/////////////////////////////
			// aha ... turn off this duda
      // 
      FlipTheControl(lpctrl, lpmwd);
    }

    wVal = GETPHISDATAVALUE(0, pctrlzm, CTRL_NUM_INPUT_PREPOST_FADER_VU);
    if(wVal == 0)
    {
      lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[pctrlzm->iModuleNumber].lpZoneMap, CTRL_NUM_INPUT_PREPOST_FADER_VU);

			//////////////////////////////
      // aha ... turn off this duda
      // 
      FlipTheControl(lpctrl, lpmwd);

			//////////////////////////////////////
      // now update all of the other mixers
      // windows that represent this mixer
      // using the iMixer, iPhisChannel
      // and iVal
      //-----------------------------------
      UpdateSameMixWndByCtrlNum(lpmwd->hwndImg, lpmwd->iMixer, pctrlzm->iModuleNumber, pctrlzm, 0, NULL);
    }
    break;

  ///////////////////////////////
  case CTRL_NUM_INPUT_LINE_B_VU:
    wVal = GETPHISDATAVALUE(0, pctrlzm, CTRL_NUM_INPUT_GATE_KEY_VU);
    if(wVal == 0)
    {
      lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[pctrlzm->iModuleNumber].lpZoneMap, CTRL_NUM_INPUT_GATE_KEY_VU);

			////////////////////////////////
      // aha ... turn off this duda
      // 
      FlipTheControl(lpctrl, lpmwd);
    }

    wVal = GETPHISDATAVALUE(0, pctrlzm, CTRL_NUM_INPUT_PREPOST_FADER_VU);
    if(wVal == 0)
    {
      lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[pctrlzm->iModuleNumber].lpZoneMap, CTRL_NUM_INPUT_PREPOST_FADER_VU);

			//////////////////////////////
      // aha ... turn off this duda
      // 
      FlipTheControl(lpctrl, lpmwd);

			////////////////////////////////////////
      // now update all of the other mixers
      // windows that represent this mixer
      // using the iMixer, iPhisChannel
      // and iVal
      //-----------------------------------
      UpdateSameMixWndByCtrlNum(lpmwd->hwndImg, lpmwd->iMixer, pctrlzm->iModuleNumber, pctrlzm, 0, NULL);
    }
    break;

  ///////////////////////////////
  case CTRL_NUM_INPUT_PREPOST_FADER_VU:
    wVal = GETPHISDATAVALUE(0, pctrlzm, CTRL_NUM_INPUT_GATE_KEY_VU);
    if(wVal == 0)
    {
      lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[pctrlzm->iModuleNumber].lpZoneMap, CTRL_NUM_INPUT_GATE_KEY_VU);

			////////////////////////////////
      // aha ... turn off this duda
      // 
      FlipTheControl(lpctrl, lpmwd);
    }

    wVal = GETPHISDATAVALUE(0, pctrlzm, CTRL_NUM_INPUT_LINE_B_VU);
    if(wVal == 0)
    {
      lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[pctrlzm->iModuleNumber].lpZoneMap, CTRL_NUM_INPUT_LINE_B_VU);

			//////////////////////////////////
      // aha ... turn off this duda
      // 
      FlipTheControl(lpctrl, lpmwd);

			///////////////////////////////////////
      // now update all of the other mixers
      // windows that represent this mixer
      // using the iMixer, iPhisChannel
      // and iVal
      //-----------------------------------
      UpdateSameMixWndByCtrlNum(lpmwd->hwndImg, lpmwd->iMixer, pctrlzm->iModuleNumber, pctrlzm, 0, NULL);

    }
    break;

  default:
    break;
  }
}

/////////////////////////////////////////////////////////////////////
// FUNCTION: HandleMasterToggleSwtches
//
// purpose: 
//
//
//
//
void HandleMasterToggleSwtches(LPMIXERWNDDATA lpmwd, LPCTRLZONEMAP pctrlzm)
{
  LPCTRLZONEMAP       lpctrl;
  WORD                wVal;

  switch(pctrlzm->iCtrlChanPos)
  {
		case CTRL_NUM_MASTER_CUE_LEVEL_MONO:
		case CTRL_NUM_MASTER_CUE_LEVEL_CENTER:
			wVal = GETPHISDATAVALUE(0, pctrlzm, pctrlzm->iCtrlChanPos);
			if(wVal == 0){
				g_cue_priority.master --; if (g_cue_priority.master < 0) g_cue_priority.master = 0;
				handleInputCuePriority (lpmwd, FALSE);
				HandleCueMasterMuteFilterEx(g_iMasterModuleIdx, lpmwd, NULL, FALSE);
			}else{
				g_cue_priority.master ++;
				handleInputCuePriority (lpmwd, TRUE);
				HandleCueMasterMuteFilterEx(g_iMasterModuleIdx, lpmwd, NULL, TRUE);
			}
			break;

		case CTRL_NUM_MASTER_AUX16PRE:
		case CTRL_NUM_MASTER_AUX15PRE:
		case CTRL_NUM_MASTER_AUX14PRE:	
		case CTRL_NUM_MASTER_AUX13PRE:	
		case CTRL_NUM_MASTER_AUX12PRE:	
		case CTRL_NUM_MASTER_AUX11PRE:	
		case CTRL_NUM_MASTER_AUX10PRE:	
		case CTRL_NUM_MASTER_AUX09PRE:	
		case CTRL_NUM_MASTER_AUX08PRE:	
		case CTRL_NUM_MASTER_AUX07PRE:	
		case CTRL_NUM_MASTER_AUX06PRE:	
		case CTRL_NUM_MASTER_AUX05PRE:	
		case CTRL_NUM_MASTER_AUX04PRE:	
		case CTRL_NUM_MASTER_AUX03PRE:	
		case CTRL_NUM_MASTER_AUX02PRE:	
		case CTRL_NUM_MASTER_AUX01PRE:	

			wVal = GETPHISDATAVALUE(0, pctrlzm, pctrlzm->iCtrlChanPos + 1);
			if(wVal == 0)
			{
				gCueActiveCount --;
				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[g_iMasterModuleIdx].lpZoneMap, 
																	pctrlzm->iCtrlChanPos + 1);

				////////////////////////////////
				// aha ... turn off this duda
				// 

				FlipTheControl(lpctrl, lpmwd);

				if(ghwndMain)
				{
					InvalidateRect(lpmwd->hwndImg, NULL, TRUE);
					UpdateWindow(lpmwd->hwndImg);
				}
			}

			wVal = GETPHISDATAVALUE(0, pctrlzm, pctrlzm->iCtrlChanPos);

			if(wVal == 0)
			{
				g_cue_priority.aux --; if (g_cue_priority.aux < 0) g_cue_priority.aux = 0;
				HandleCueMasterMuteFilterEx(g_iMasterModuleIdx, lpmwd, NULL, FALSE);
				handleInputCuePriority (lpmwd, FALSE);
			}
			else
			{
				g_cue_priority.aux ++;
				handleInputCuePriority (lpmwd, TRUE);
				HandleCueMasterMuteFilterEx(g_iMasterModuleIdx, lpmwd, NULL, TRUE);
			}

			break;
  
		case CTRL_NUM_MASTER_AUX16POST:
		case CTRL_NUM_MASTER_AUX15POST:	
		case CTRL_NUM_MASTER_AUX14POST:	
		case CTRL_NUM_MASTER_AUX13POST:	
		case CTRL_NUM_MASTER_AUX12POST:	
		case CTRL_NUM_MASTER_AUX11POST:	
		case CTRL_NUM_MASTER_AUX10POST:	
		case CTRL_NUM_MASTER_AUX09POST:	
		case CTRL_NUM_MASTER_AUX08POST:	
		case CTRL_NUM_MASTER_AUX07POST:	
		case CTRL_NUM_MASTER_AUX06POST:	
		case CTRL_NUM_MASTER_AUX05POST:	
		case CTRL_NUM_MASTER_AUX04POST:	
		case CTRL_NUM_MASTER_AUX03POST:	
		case CTRL_NUM_MASTER_AUX02POST:	
		case CTRL_NUM_MASTER_AUX01POST:
			wVal = GETPHISDATAVALUE(0, pctrlzm, pctrlzm->iCtrlChanPos - 1);
			if(wVal == 0)
			{
				gCueActiveCount --;
				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[g_iMasterModuleIdx].lpZoneMap, 
																	pctrlzm->iCtrlChanPos - 1);

				/////////////////////////////
				// aha ... turn off this duda
				// 

				FlipTheControl(lpctrl, lpmwd);


				if(ghwndMain) {
					InvalidateRect(lpmwd->hwndImg, NULL, TRUE);
					UpdateWindow(lpmwd->hwndImg);
				}
			}

			wVal = GETPHISDATAVALUE(0, pctrlzm, pctrlzm->iCtrlChanPos);

			if(wVal == 0)
			{
				g_cue_priority.aux --; if (g_cue_priority.aux < 0) g_cue_priority.aux = 0;
				HandleCueMasterMuteFilterEx(g_iMasterModuleIdx, lpmwd, NULL, FALSE);
				handleInputCuePriority (lpmwd, FALSE);
			}
			else
			{
				g_cue_priority.aux ++;
				handleInputCuePriority (lpmwd, TRUE);
				HandleCueMasterMuteFilterEx(g_iMasterModuleIdx, lpmwd, NULL, TRUE);
			}
			break;

		/////////////////////////////////////
		case CTRL_NUM_MASTER_STEREO_CUE_PRE:
			wVal = GETPHISDATAVALUE(0, pctrlzm, CTRL_NUM_MASTER_STEREO_CUE_POST);
			if(wVal == 0)
			{
				gCueActiveCount--;
				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[pctrlzm->iModuleNumber].lpZoneMap, CTRL_NUM_MASTER_STEREO_CUE_POST);

				//////////////////////////////
				// aha ... turn off this duda
				// 

				FlipTheControl(lpctrl, lpmwd);

				/////////////////////////////////////
				// now update all of the other mixers
				// windows that represent this mixer
				// using the iMixer, iPhisChannel
				// and iVal
				//-----------------------------------
				UpdateSameMixWndByCtrlNum(lpmwd->hwndImg, lpmwd->iMixer, pctrlzm->iModuleNumber, pctrlzm, 0, NULL);
			}

			wVal = GETPHISDATAVALUE(0, pctrlzm, pctrlzm->iCtrlChanPos);

			if(wVal == 0)
			{
				g_cue_priority.master --; if (g_cue_priority.master < 0) g_cue_priority.master = 0;
				HandleCueMasterMuteFilterEx(g_iMasterModuleIdx, lpmwd, NULL, FALSE);
				handleInputCuePriority (lpmwd, FALSE);
			}
			else
			{
				g_cue_priority.master ++; 
				handleInputCuePriority (lpmwd, TRUE);
				HandleCueMasterMuteFilterEx(g_iMasterModuleIdx, lpmwd, NULL, TRUE);
			}
			break;

		////////////////////////////////////
		case CTRL_NUM_MASTER_STEREO_CUE_POST:
			wVal = GETPHISDATAVALUE(0, pctrlzm, CTRL_NUM_MASTER_STEREO_CUE_PRE);
			if(wVal == 0)
			{
				gCueActiveCount--;
				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[pctrlzm->iModuleNumber].lpZoneMap, CTRL_NUM_MASTER_STEREO_CUE_PRE);

				//////////////////////////////
				// aha ... turn off this duda
				// 

				FlipTheControl(lpctrl, lpmwd);

				//////////////////////////////////////
				// now update all of the other mixers
				// windows that represent this mixer
				// using the iMixer, iPhisChannel
				// and iVal
				//-----------------------------------
				UpdateSameMixWndByCtrlNum(lpmwd->hwndImg, lpmwd->iMixer, pctrlzm->iModuleNumber, pctrlzm, 0, NULL);
			}

			wVal = GETPHISDATAVALUE(0, pctrlzm, pctrlzm->iCtrlChanPos);

			if(wVal == 0)
			{
				g_cue_priority.master --; if (g_cue_priority.master < 0) g_cue_priority.master = 0;
				HandleCueMasterMuteFilterEx(g_iMasterModuleIdx, lpmwd, NULL, FALSE);
				handleInputCuePriority (lpmwd, FALSE);
			}
			else
			{
				g_cue_priority.master ++; 
				handleInputCuePriority (lpmwd, TRUE);
				HandleCueMasterMuteFilterEx(g_iMasterModuleIdx, lpmwd, NULL, TRUE);
			}
			break;

		case CTRL_NUM_MATRIX_STERIO_CUE_PRE:
			wVal = GETPHISDATAVALUE(0, pctrlzm, CTRL_NUM_MATRIX_STERIO_CUE_POST);
			if(wVal == 0)
			{
				gCueActiveCount--;
				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[pctrlzm->iModuleNumber].lpZoneMap, CTRL_NUM_MATRIX_STERIO_CUE_POST);
				
				///////////////////////////////
				// aha ... turn off this duda
				// 
				FlipTheControl(lpctrl, lpmwd);
				
				//////////////////////////////////////
				// now update all of the other mixers
				// windows that represent this mixer
				// using the iMixer, iPhisChannel
				// and iVal
				//-----------------------------------
				UpdateSameMixWndByCtrlNum(lpmwd->hwndImg, lpmwd->iMixer, pctrlzm->iModuleNumber, pctrlzm, 0, NULL);
			}

			wVal = GETPHISDATAVALUE(0, pctrlzm, pctrlzm->iCtrlChanPos);

			if(wVal == 0)
			{
				g_cue_priority.sub --; if (g_cue_priority.sub < 0) g_cue_priority.sub = 0;
				HandleCueMasterMuteFilterEx(g_iMasterModuleIdx, lpmwd, NULL, FALSE);
				handleInputCuePriority (lpmwd, FALSE);
			}
			else
			{
				g_cue_priority.sub ++; 
				handleInputCuePriority (lpmwd, TRUE);
				HandleCueMasterMuteFilterEx(g_iMasterModuleIdx, lpmwd, NULL, TRUE);
			}
			break;

		case CTRL_NUM_MATRIX_STERIO_CUE_POST:
			wVal = GETPHISDATAVALUE(0, pctrlzm, CTRL_NUM_MATRIX_STERIO_CUE_PRE);
			if(wVal == 0)
			{
				gCueActiveCount--;
				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[pctrlzm->iModuleNumber].lpZoneMap, CTRL_NUM_MATRIX_STERIO_CUE_PRE);
				
				///////////////////////////////
				// aha ... turn off this duda
				// 

				FlipTheControl(lpctrl, lpmwd);
				
				////////////////////////////////////
				// now update all of the other mixers
				// windows that represent this mixer
				// using the iMixer, iPhisChannel
				// and iVal
				//-----------------------------------
				UpdateSameMixWndByCtrlNum(lpmwd->hwndImg, lpmwd->iMixer, pctrlzm->iModuleNumber, pctrlzm, 0, NULL);

			}

			wVal = GETPHISDATAVALUE(0, pctrlzm, pctrlzm->iCtrlChanPos);

			if(wVal == 0)
			{
				g_cue_priority.sub --; if (g_cue_priority.sub < 0) g_cue_priority.sub = 0;
				HandleCueMasterMuteFilterEx(g_iMasterModuleIdx, lpmwd, NULL, FALSE);
				handleInputCuePriority (lpmwd, FALSE);
			}
			else
			{
				g_cue_priority.sub ++; 
				handleInputCuePriority (lpmwd, TRUE);
				HandleCueMasterMuteFilterEx(g_iMasterModuleIdx, lpmwd, NULL, TRUE);
			}
			break;

		case CTRL_NUM_MASTER_CUEA_SYSTEM_SEL:
			wVal = GETPHISDATAVALUE(0, pctrlzm, CTRL_NUM_MASTER_CUEB_SYSTEM_SEL);
			if(wVal == 0)
			{
				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[g_iMasterModuleIdx].lpZoneMap, CTRL_NUM_MASTER_CUEB_SYSTEM_SEL);
				
				///////////////////////////////////////
				// aha ... turn off this duda
				// 

				FlipTheControl(lpctrl, lpmwd);

				///////////////////////////////////////
				// now update all of the other mixers
				// windows that represent this mixer
				// using the iMixer, iPhisChannel
				// and iVal
				//-----------------------------------
				UpdateSameMixWndByCtrlNum(lpmwd->hwndImg, lpmwd->iMixer, g_iMasterModuleIdx, pctrlzm, 0, NULL);
			}
			else	// Cue A is Already pressed in
			{
				wVal = GETPHISDATAVALUE(0, pctrlzm, CTRL_NUM_MASTER_CUEA_SYSTEM_SEL);
				if(wVal == 0)
				{
					lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[g_iMasterModuleIdx].lpZoneMap, CTRL_NUM_MASTER_CUEB_SYSTEM_SEL);
					
					
					// aha ... turn off this duda
					// 

					FlipTheControl(lpctrl, lpmwd);
					
					
					///////////////////////////////////////
					// now update all of the other mixers
					// windows that represent this mixer
					// using the iMixer, iPhisChannel
					// and iVal
					//-----------------------------------
					UpdateSameMixWndByCtrlNum(lpmwd->hwndImg, lpmwd->iMixer, g_iMasterModuleIdx, pctrlzm, 0, NULL);
				}
			}
    
			break;

		case CTRL_NUM_MASTER_CUEB_SYSTEM_SEL:
			wVal = GETPHISDATAVALUE(0, pctrlzm, CTRL_NUM_MASTER_CUEA_SYSTEM_SEL);
			if(wVal == 0)
			{                                                                           
				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[g_iMasterModuleIdx].lpZoneMap, CTRL_NUM_MASTER_CUEA_SYSTEM_SEL);
				
				
				///////////////////////////////////////
				// aha ... turn off this duda
				// 

				FlipTheControl(lpctrl, lpmwd);
				
				///////////////////////////////////////
				// now update all of the other mixers
				// windows that represent this mixer
				// using the iMixer, iPhisChannel
				// and iVal
				//-----------------------------------
				UpdateSameMixWndByCtrlNum(lpmwd->hwndImg, lpmwd->iMixer, g_iMasterModuleIdx, pctrlzm, 0, NULL);
			}
			else	// Cue B is Already pressed in
			{
				wVal = GETPHISDATAVALUE(0, pctrlzm, CTRL_NUM_MASTER_CUEB_SYSTEM_SEL);
				if(wVal == 0)
				{
					lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[g_iMasterModuleIdx].lpZoneMap, CTRL_NUM_MASTER_CUEA_SYSTEM_SEL);
					
					///////////////////////////////
					// aha ... turn off this duda
					// 

					FlipTheControl(lpctrl, lpmwd);
					
					//////////////////////////////////////
					// now update all of the other mixers
					// windows that represent this mixer
					// using the iMixer, iPhisChannel
					// and iVal
					//-----------------------------------
					UpdateSameMixWndByCtrlNum(lpmwd->hwndImg, lpmwd->iMixer, g_iMasterModuleIdx, pctrlzm, 0, NULL);
				}
			}

			break;

		default:
			break;

  }
}


/////////////////////////////////////////////////////////////////////
// FUNCTION: HandleMatrixToggleSwtches
//
// purpose: 
//
//
//
//
void HandleMatrixToggleSwtches(LPMIXERWNDDATA lpmwd, LPCTRLZONEMAP pctrlzm)
{
  LPCTRLZONEMAP       lpctrl;
  WORD                wVal;
  int                 iIdx;
  int                 iCount;


  for(iCount =0; iCount < MAX_MATRIX_COUNT; iCount++)
  {
    if(g_aiMatrix[iCount] == pctrlzm->iModuleNumber)
    {
      iIdx = g_aiAux[iCount];
      break;
    };
  }

  switch(pctrlzm->iCtrlChanPos)
  {
		case CTRL_NUM_MATRIX_CUE_PRE:

			wVal = GETPHISDATAVALUE(0, pctrlzm, CTRL_NUM_MATRIX_CUE_POST);

			if(wVal == 0)
			{
				gCueActiveCount--;
				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iIdx].lpZoneMap, CTRL_NUM_MATRIX_CUE_POST);

				//////////////////////////////
				// aha ... turn off this duda
				// 

				FlipTheControl(lpctrl, lpmwd);

				//////////////////////////////////////
				// now update all of the other mixers
				// windows that represent this mixer
				// using the iMixer, iPhisChannel
				// and iVal
				//-----------------------------------
				UpdateSameMixWndByCtrlNum(lpmwd->hwndImg, lpmwd->iMixer, iIdx, pctrlzm, 0, NULL);
			}

			wVal = GETPHISDATAVALUE(0, pctrlzm, pctrlzm->iCtrlChanPos);

			if(wVal == 0)
			{
				g_cue_priority.matrix --; 
				if (g_cue_priority.matrix < 0) g_cue_priority.matrix = 0;
				HandleCueMasterMuteFilterEx(g_iMasterModuleIdx, lpmwd, NULL, FALSE);
				handleInputCuePriority (lpmwd, FALSE);
			}
			else
			{
				g_cue_priority.matrix ++;
				handleInputCuePriority (lpmwd, TRUE);
				HandleCueMasterMuteFilterEx(g_iMasterModuleIdx, lpmwd, NULL, TRUE);
			}
			break;


		case CTRL_NUM_MATRIX_CUE_POST:
			wVal = GETPHISDATAVALUE(0, pctrlzm, CTRL_NUM_MATRIX_CUE_PRE);
			if(wVal == 0)
			{
				gCueActiveCount --;
				lpctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iIdx].lpZoneMap, CTRL_NUM_MATRIX_CUE_PRE);

				//////////////////////////////
				// aha ... turn off this duda
				// 

				FlipTheControl(lpctrl, lpmwd);

				/////////////////////////////////////
				// now update all of the other mixers
				// windows that represent this mixer
				// using the iMixer, iPhisChannel
				// and iVal
				//-----------------------------------
				UpdateSameMixWndByCtrlNum(lpmwd->hwndImg, lpmwd->iMixer, iIdx, pctrlzm, 0, NULL);
			}

			wVal = GETPHISDATAVALUE(0, pctrlzm, pctrlzm->iCtrlChanPos);

			if(wVal == 0)
			{
				g_cue_priority.matrix --; if (g_cue_priority.matrix < 0) g_cue_priority.matrix = 0;
				HandleCueMasterMuteFilterEx(g_iMasterModuleIdx, lpmwd, NULL, FALSE);
				handleInputCuePriority (lpmwd, FALSE);
			}else
			{
				g_cue_priority.matrix ++;
				handleInputCuePriority (lpmwd, TRUE);
				HandleCueMasterMuteFilterEx(g_iMasterModuleIdx, lpmwd, NULL, TRUE);
			}
			break;


		default:
			break;
  }

}





/////////////////////////////////////////////////////////////////////
// FUNCTION: CheckForToggleSwitches
//
// purpose: 
//
//   Check for the Toggle switches
//
//
void    CheckForToggleSwitches(LPMIXERWNDDATA lpmwd, LPCTRLZONEMAP pctrlzm)
{

  switch(gDeviceSetup.iaChannelTypes[pctrlzm->iModuleNumber])
  {
  case DCX_DEVMAP_MODULE_INPUT:
    HandleInputToggleSwtches(lpmwd, pctrlzm);
    break;
  case DCX_DEVMAP_MODULE_MATRIX:
    HandleMatrixToggleSwtches(lpmwd, pctrlzm);
    break;
  case DCX_DEVMAP_MODULE_MASTER:
  case DCX_DEVMAP_MODULE_AUX:
  case DCX_DEVMAP_MODULE_CUE:// Cue as well
    HandleMasterToggleSwtches(lpmwd, pctrlzm);
    break;
  default:
    break;
  }

};


/////////////////////////////////////////////////////////////////////
// FUNCTION: CheckForSpecialFilters
//
// purpose: 
//      HANDLE SPECIAL FILTERS
//
//
BOOL  CheckForSpecialFilters(/*LPMIXERWNDDATA lpwmd, */LPCTRLZONEMAP pctrlzm)
{
  /*
  RECT  rZone;
  HDC   hdc;
  
  if(IsSpecialFilter(pctrlzm) > -1)
  {
    rZone = pctrlzm->rZone;
    rZone.right = rZone.right - rZone.left;
    rZone.bottom = rZone.bottom - rZone.top;
    rZone.top = rZone.left = 0;

    //hdc = GetDC(lpmwd->hwndImg);
    //DrawEqGraph(g_hdcMemory, &rZone);
    return TRUE;
    //ReleaseDC(lpmwd->hwndImg, hdc);
  }
  */
  return FALSE;
};


/////////////////////////////////////////////////////////////////////
// FUNCTION: IsSpecialFilter
//
// purpose: 
// Return the Special Filter type
//
int IsSpecialFilter(LPMIXERWNDDATA lpmwd, int iPhisChan, LPCTRLZONEMAP pctrlzm)
{
  if((lpmwd == NULL) || (pctrlzm == NULL))
    return -1;

  switch(gDeviceSetup.iaChannelTypes[pctrlzm->iModuleNumber])
  {
  case DCX_DEVMAP_MODULE_INPUT:
    switch(pctrlzm->iCtrlChanPos)
    {
    case CTRL_NUM_INPUT_HIGHFREQ:
    case CTRL_NUM_INPUT_HF_BW:         
    case CTRL_NUM_INPUT_HF_BC:         
    case CTRL_NUM_INPUT_HF_PEAKSHELF:
    case CTRL_NUM_INPUT_HIMIDFREQ:
    case CTRL_NUM_INPUT_HM_BW:    
    case CTRL_NUM_INPUT_HM_BC:         
    case CTRL_NUM_INPUT_LOMIDFREQ:
    case CTRL_NUM_INPUT_LM_BW:
    case CTRL_NUM_INPUT_LM_BC:         
    case CTRL_NUM_INPUT_LOWFREQ:
    case CTRL_NUM_INPUT_LF_BW:
    case CTRL_NUM_INPUT_LF_BC:      
    case CTRL_NUM_INPUT_LF_PEAKSHELF:
		case CTRL_NUM_INPUT_LOWCUT:
//		case CTRL_NUM_INPUT_LOWCUT_INOUT:		// Removing this fixes EQ display going to other channels
      
      return CTRL_TYPE_DISP_INPUT_EQ_FILTER;
      break;

    default:
      return -1;
    }
    break;
    // Check the module type !!!
  case DCX_DEVMAP_MODULE_MATRIX:
    switch(pctrlzm->iCtrlChanPos)
    {
    case CTRL_NUM_MATRIX_HI_FREQ_LT:
    case CTRL_NUM_MATRIX_HF_BW_LT:
    case CTRL_NUM_MATRIX_HF_BC_LT:       
    case CTRL_NUM_MATRIX_HF_PK_SHELF_LT: 
    case CTRL_NUM_MATRIX_HI_FREQ_RT: 
    case CTRL_NUM_MATRIX_HF_BW_RT:       
    case CTRL_NUM_MATRIX_HF_BC_RT:       
    case CTRL_NUM_MATRIX_HF_PK_SHELF_RT:
    case CTRL_NUM_MATRIX_HM_FREQ_LT:     
    case CTRL_NUM_MATRIX_HM_BW_LT:
    case CTRL_NUM_MATRIX_HM_BC_LT:
    case CTRL_NUM_MATRIX_HM_FREQ_RT:
    case CTRL_NUM_MATRIX_HM_BW_RT:
    case CTRL_NUM_MATRIX_HM_BC_RT:
    case CTRL_NUM_MATRIX_LM_FREQ_LT:
    case CTRL_NUM_MATRIX_LM_BW_LT:
    case CTRL_NUM_MATRIX_LM_BC_LT:
    case CTRL_NUM_MATRIX_LM_FREQ_RT:
    case CTRL_NUM_MATRIX_LM_BW_RT:
    case CTRL_NUM_MATRIX_LM_BC_RT:
    case CTRL_NUM_MATRIX_LO_FREQ_LT:
    case CTRL_NUM_MATRIX_LF_BW_LT:
    case CTRL_NUM_MATRIX_LF_BC_LT:
    case CTRL_NUM_MATRIX_LF_PK_SHELF_LT:
    case CTRL_NUM_MATRIX_LO_FREQ_RT:
    case CTRL_NUM_MATRIX_LF_BW_RT:
    case CTRL_NUM_MATRIX_LF_BC_RT:
    case CTRL_NUM_MATRIX_LF_PK_SHELF_RT:
    case CTRL_NUM_MATRIX_EQ_LEFTRIGHT_SELECTOR:
      return CTRL_TYPE_DISP_SUB_EQ_FILTER;
      break;
    default:
      return -1;
    }


    break;
  }

  return -1;
};


/*
GetSpecialFilter(LPMIXERWNDDATA lpmwd, int iPhisChan, LPCTRLZONEMAP lpctrlZM_Special)
{
  
};
*/

/////////////////////////////////////////////////////////////////////
// FUNCTION: ResendControlData
//
// purpose: 
//
//
void ResendControlData(LPCTRLZONEMAP  lpctrlZM){
  CONTROLDATA         ctrlData;
  int                 iVal;

	
  iVal = GETPHISDATAVALUE(0, lpctrlZM, lpctrlZM->iCtrlChanPos);

  ctrlData.wMixer   = 0;
  ctrlData.wChannel = lpctrlZM->iModuleNumber;//iPhisChannel;
  ctrlData.wCtrl    = lpctrlZM->iCtrlNumAbs; // we use this one since for the definition dll
	ctrlData.wVal     = iVal;
  SendDataToDevice(&ctrlData, TRUE, lpctrlZM, -1, NULL, FALSE);
}

/////////////////////////////////////////////////////////////////////
// FUNCTION: ResendControlData
//
// purpose: 
//	CancelAllCues
//
//
void	CancelAllCues (HWND hWnd)
{

// #ifdef NOTUSED	//////////////////////////////////////NOT WORKING YET, CAUSES INPUT LABELS TO OVERWRITE

	LPMIXERWNDDATA			lpmwd; // temp memory for the Mixer Window data
	int							channel;
  LPCTRLZONEMAP   lpctrlZM;
  LPCTRLZONEMAP   pctrlzm;
	int i;

	/////////////////////////////////////////////
	// Table of CUE's that we handle
	// This might be usefull globally
	// if other modules need to scan
	// through all the cues

	// All the INPUT cues

	int iInputCueTable[] =
	{CTRL_NUM_INPUT_CUE_FAD_PRE,
	CTRL_NUM_INPUT_CUE_FAD_POST,
	CTRL_NUM_INPUT_MIC_B_CUE,
	CTRL_NUM_INPUT_MIC_A_CUE,
	CTRL_NUM_INPUT_GATE_KEY_INOUT};


	// All the AUX cues

	int iAuxCueTable[] = 
	{CTRL_NUM_MATRIX_STERIO_CUE_PRE,
	CTRL_NUM_MATRIX_STERIO_CUE_POST,
	CTRL_NUM_MATRIX_CUE_PRE,
	CTRL_NUM_MATRIX_CUE_POST};

	// All the MASTER cues

	int iMasterCueTable[] =
	{
	CTRL_NUM_MASTER_AUX16PRE,
	CTRL_NUM_MASTER_AUX15PRE,
	CTRL_NUM_MASTER_AUX14PRE,
	CTRL_NUM_MASTER_AUX13PRE,
	CTRL_NUM_MASTER_AUX12PRE,
	CTRL_NUM_MASTER_AUX11PRE,
	CTRL_NUM_MASTER_AUX10PRE,
	CTRL_NUM_MASTER_AUX09PRE,
	CTRL_NUM_MASTER_AUX08PRE,
	CTRL_NUM_MASTER_AUX07PRE,
	CTRL_NUM_MASTER_AUX06PRE,
	CTRL_NUM_MASTER_AUX05PRE,
	CTRL_NUM_MASTER_AUX04PRE,
	CTRL_NUM_MASTER_AUX03PRE,
	CTRL_NUM_MASTER_AUX02PRE,
	CTRL_NUM_MASTER_AUX01PRE,
	CTRL_NUM_MASTER_AUX16POST,
	CTRL_NUM_MASTER_AUX15POST,
	CTRL_NUM_MASTER_AUX14POST,
	CTRL_NUM_MASTER_AUX13POST,
	CTRL_NUM_MASTER_AUX12POST,
	CTRL_NUM_MASTER_AUX11POST,
	CTRL_NUM_MASTER_AUX10POST,
	CTRL_NUM_MASTER_AUX09POST,
	CTRL_NUM_MASTER_AUX08POST,
	CTRL_NUM_MASTER_AUX07POST,
	CTRL_NUM_MASTER_AUX06POST,
	CTRL_NUM_MASTER_AUX05POST,
	CTRL_NUM_MASTER_AUX04POST,
	CTRL_NUM_MASTER_AUX03POST,
	CTRL_NUM_MASTER_AUX02POST,
	CTRL_NUM_MASTER_AUX01POST,
	CTRL_NUM_MASTER_CUE_LEVEL_MONO,
	CTRL_NUM_MASTER_CUE_LEVEL_CENTER,
// fds revmoved 3/18/2001 as per gamble	CTRL_NUM_MASTER_CUE_A_SUM_IN,
	CTRL_NUM_MASTER_STEREO_CUE_PRE,
	CTRL_NUM_MASTER_STEREO_CUE_POST};

	size_t sizeInput = sizeof(iInputCueTable)/sizeof(iInputCueTable[0]);
	size_t sizeAux = sizeof(iAuxCueTable)/sizeof(iAuxCueTable[0]);
	size_t sizeMaster = sizeof(iMasterCueTable)/sizeof(iMasterCueTable[0]);

	/////////////////////////////////////////////////
	// If there are active cues then scan through
	// all the channels looking for them, if they
	// are on then set the butons to active and
	// let the high level routines toggle the buttons

	if(gCueActiveCount)
	{

		lpmwd = GetValidMixerWindowData ();

		for (channel = 0; channel < MAX_CHANNELS; channel ++)
		{
			lpctrlZM = gpZoneMaps_Zoom[channel].lpZoneMap;

			if(lpctrlZM != NULL)
			{
				switch (gDeviceSetup.iaChannelTypes[channel])
				{
					case DCX_DEVMAP_MODULE_INPUT: // input module
						for(i=0;i<sizeInput;i++)
						{
							if(isCtrlValueNotEqualToDefault(lpctrlZM, iInputCueTable[i]))
							{
									pctrlzm = ScanCtrlZonesNum (lpctrlZM, iInputCueTable[i]);
									lpmwd->lpCtrlZM = pctrlzm;
									lpmwd->iCurMode = MW_CONTROL_ACTIVE;	// Make this button ACTIVE again
									lpmwd->iCurChan = channel;
									ActivateMWMode(ghwndMain, lpmwd);			// Let the normal button press login handle it.
							}
						}
						break;
					
					case DCX_DEVMAP_MODULE_AUX:
						for(i=0;i<sizeAux;i++)
						{
							if(isCtrlValueNotEqualToDefault(lpctrlZM, iAuxCueTable[i]))
							{
									pctrlzm = ScanCtrlZonesNum (lpctrlZM, iAuxCueTable[i]);
									lpmwd->lpCtrlZM = pctrlzm;
									lpmwd->iCurMode = MW_CONTROL_ACTIVE;	// Make this button ACTIVE again
									lpmwd->iCurChan = channel;
								ActivateMWMode(ghwndMain, lpmwd);			// Let the normal button press login handle it.
							}
						}
						break;
					//
					case DCX_DEVMAP_MODULE_CUE:
						break;

					//
					case DCX_DEVMAP_MODULE_MASTER:
						for(i=0;i<sizeMaster;i++)
						{
							if(isCtrlValueNotEqualToDefault(lpctrlZM, iMasterCueTable[i]))
							{
									pctrlzm = ScanCtrlZonesNum (lpctrlZM, iMasterCueTable[i]);
									lpmwd->lpCtrlZM = pctrlzm;
									lpmwd->iCurMode = MW_CONTROL_ACTIVE;	// Make this button ACTIVE again
									ActivateMWMode(ghwndMain, lpmwd);			// Let the normal button press login handle it.
							}
						}		
						break;
						///////

					default:
						break;

				}
			}
		}

	}
//#endif		// NOTUSED

};

