//=================================================
// Copyright 1997-2002, CorTek Software, Inc.
//=================================================
//
//
// $Author:: Styck                                $
// $Archive:: /Vacs Client/src/MemoryMap.c        $
// $Revision:: 38                                 $
//
//
/////////////////////////////////////////////
//
// All the routines to initialiaze and manipulate
// the GLOBAL memory map buffer that keeps the status
// of all the controls
//
// Remaps controls that are on the MASTER to their
// corresponding SUB-AUX Physical modules
//
/////////////////////////////////////////////

#include <math.h>
#include "SAMM.h"
#include "SAMMEXT.h"
#include "MACRO.h"
#include <stdio.h>	// for sprintf

#include "consoledefinition.h"

/////////////////////////////////////////////
// GLOBALS

int                 g_aiAux[MAX_MATRIX_COUNT];
int                 g_aiMatrix[MAX_MATRIX_COUNT];
int                 g_iCueModuleIdx = -1;
int                 g_iAuxIdx = 0;
int                 g_iMasterModuleIdx = -1;
int									giMemControlCount = 0;
WORD								*gpwMemMapUpdateBufferMask = NULL;
WORD								*gpwMemMapUpdateBuffer = NULL;


/////////////////////////////////////////////
// PROTOTYPES

void                HandleCtrlBtnClickInGroup(HWND hwnd, LPMIXERWNDDATA lpmwd, LPCTRLZONEMAP pctrlzm, int phis_channel);
BOOL                IsCtrlPrePostFilter(int iType);
void								SetFilteredControlsByNumber(LPCTRLZONEMAP       lpctrl, int filtered);
void								HadleCueMasterSystem ();
BOOL								isChanelSafeActive (LPCTRLZONEMAP pctrlzm);
void								setBufferChannelSafeActive (LPCTRLZONEMAP pctrlzm, BOOL active);

//=========================================
//FUNCTION: InitMemoryMap
//
//
//
//
//=========================================
int     InitMemoryMap(void)
{
	giMemMapSize = sizeof(WORD)*giMax_MIXERS*giMax_CHANNELS*giMax_CONTROLS;
	giMemControlCount = giMax_MIXERS*giMax_CHANNELS*giMax_CONTROLS;

	gpwMemMapMixer = (WORD *)GlobalAlloc(GPTR, giMemMapSize);
	if(gpwMemMapMixer  == NULL)
		return IDS_ERR_ALLOCATE_MEMORY;

	gpwMemMapBuffer = (WORD *)GlobalAlloc(GPTR, giMemMapSize);
	if(gpwMemMapBuffer == NULL)
		return IDS_ERR_ALLOCATE_MEMORY;

	gpwMemMapUpdateBufferMask = (WORD *)GlobalAlloc(GPTR, giMemMapSize);
	if(gpwMemMapUpdateBufferMask == NULL)
		return IDS_ERR_ALLOCATE_MEMORY;
	FillMemory(gpwMemMapUpdateBufferMask, giMemMapSize, 0xFFFF);

	gpwMemMapUpdateBuffer = (WORD *)GlobalAlloc(GPTR, giMemMapSize);
	if(gpwMemMapUpdateBuffer == NULL)
		return IDS_ERR_ALLOCATE_MEMORY;

	return 0;
}

//==================================================
//FUNCTION: FreeMemoryMap
//
//
//
//==================================================
void    FreeMemoryMap(void)
{
	if(gpwMemMapMixer != NULL)
	{
		GlobalFree((HGLOBAL)gpwMemMapMixer );
		gpwMemMapMixer = NULL;
	}

	if(gpwMemMapBuffer != NULL)
	{
		GlobalFree((HGLOBAL)gpwMemMapBuffer );
		gpwMemMapBuffer = NULL;
	}

	if(gpwMemMapUpdateBufferMask != NULL)
	{
		GlobalFree((HGLOBAL)gpwMemMapUpdateBufferMask );
		gpwMemMapUpdateBufferMask = NULL;
	}

	if(gpwMemMapUpdateBuffer != NULL)
	{
		GlobalFree((HGLOBAL)gpwMemMapUpdateBuffer);
		gpwMemMapUpdateBuffer = NULL;
	}
	return;
}

//////////////////////////////////////////////////////////////////////////
// FUNCTION: SetControlsModuleNumber
//
// purpose: Sets the iModuleNumber to the iChannel that is passed for all controls
//					between the iStartRange and iEndRange of controls
//
//
//////////////////////////////////////////////////////////////////////////

void  SetControlsModuleNumber(LPCTRLZONEMAP lpctrlZM, int iStartRange, int iEndRange, int iChannel)
{
	while(lpctrlZM->rZone.right > 0)
	{
		if(iEndRange > -1 && iStartRange > -1)
		{
			// set the control number only if it is within a range
			//
			if( lpctrlZM->iCtrlChanPos >= iStartRange && lpctrlZM->iCtrlChanPos <= iEndRange )
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

/////////////////////////////////////////////////////////////////////
// FUNCTION: InitPhysicalModuleMap
//
// PURPOSE: Adjust the iModuleNumber member of the CTRLZONEMAP
//  for every control individually. 
//
//  By doing this we can mix  on the screen controls that reside in 
//  different physical modules.
//
//  After this function is done we can use iModuleNumber to send the
//  data to the mixer.
//
//  actions: Pair the Consecutive Sub-Aux with Matrix modules. Also 
//  put together the Master and Cue modules.
//  
//
/////////////////////////////////////////////////////////////////////
int InitPhysicalModuleMap(void)
{
	int                 iRet = 0;
	int                 iCount;
	int                 iChannel;

	LPCTRLZONEMAP       lpctrlZM_zoom;
	LPCTRLZONEMAP       lpctrlZM_full;
	int                 iMatrixIdx = 0;

	g_iMasterModuleIdx = -1;	// Invalidate the master 
	g_iCueModuleIdx = -1;			// and cue modules index
	g_iAuxIdx = 0;

	ZeroMemory(g_aiMatrix, MAX_MATRIX_COUNT * sizeof(int));
	ZeroMemory(g_aiAux, MAX_MATRIX_COUNT * sizeof(int));

	for(iChannel = 0; iChannel < giMax_CHANNELS; iChannel++)
	{
		lpctrlZM_zoom = gpZoneMaps_Zoom[iChannel].lpZoneMap;
		lpctrlZM_full = gpZoneMaps_Full[iChannel].lpZoneMap;

		switch(gDeviceSetup.iaChannelTypes[iChannel])
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
			if(g_iAuxIdx >= MAX_MATRIX_COUNT)
				return IDS_INVALID_DEVICE_MAP_AUX_MANY; // ?? error code should be defined
			g_aiAux[g_iAuxIdx] = iChannel;
			g_iAuxIdx ++;  
			break;

			// Handle the MATRIX Modules ...
		case DCX_DEVMAP_MODULE_MATRIX:
			if(iMatrixIdx >= MAX_MATRIX_COUNT)
				return IDS_INVALID_DEVICE_MAP_MATRIX_MANY; // ?? error code should be defined
			g_aiMatrix[iMatrixIdx] = iChannel;
			iMatrixIdx ++;  
			break;

			// Handle the MASTER Modules ...
		case DCX_DEVMAP_MODULE_MASTER:
			if(g_iMasterModuleIdx == -1)
			{
				SetControlsModuleNumber(lpctrlZM_zoom, -1, -1, iChannel);
				SetControlsModuleNumber(lpctrlZM_full, -1, -1, iChannel);
				g_iMasterModuleIdx = iChannel;
			}
			else
				// ERROR .. more then one Master module ..
				return IDS_INVALID_DEVICE_MASTER_MANY;
			break;

			// Handle the CUE Modules ...
		case DCX_DEVMAP_MODULE_CUE:
			// Store the channel number for the 
			// Cue module ...
			//
			if(g_iCueModuleIdx == -1)
				g_iCueModuleIdx = iChannel;    
			else
				// ERROR two Cue modules ..
				return IDS_INVALID_DEVICE_CUE_MANY;
			break;
		default:
			// do nothing .. the module might be empty
			break;
		}
	}

	if(g_iMasterModuleIdx == -1)
		return IDS_INVALID_DEVICE_NO_MASTER;	// ERROR .... MUST HAVE A MASTER MODULE

	// now loop through all 
	if(g_iAuxIdx != iMatrixIdx)
		return IDS_INVLAID_DEVICE_AUX_COMBO; // ERROR ... the Aux and the Matrix channels must be the same count.

	////////////////////////////////////////////////////////
	// 

	for(iCount = 0; iCount < g_iAuxIdx; iCount ++)
	{
		// use the Aux channel number since the Aux is mapped to the screen
		iChannel = g_aiAux[iCount];

		////////////////////////////////////////////////////////
		// now loop through the section for each one of these types and 
		// convert the controls to the corect Matrix physical module number
		//

		lpctrlZM_zoom = gpZoneMaps_Zoom[iChannel].lpZoneMap;
		lpctrlZM_full = gpZoneMaps_Full[iChannel].lpZoneMap;

		// Set Matrix group ONE
		SetControlsModuleNumber(lpctrlZM_zoom, __SUB_MATRIX_GROUP_01_START, 
			__SUB_MATRIX_GROUP_01_END, g_aiMatrix[iCount]);
		SetControlsModuleNumber(lpctrlZM_full, __SUB_MATRIX_GROUP_01_START, 
			__SUB_MATRIX_GROUP_01_END, g_aiMatrix[iCount]);
	}

	////////////////////////////////////////////////////////
	// Set the master controls to only to the Master module
	// Scan the Master Module for controls that are in between
	// the Master START and END groups and set the iModuleNumber to
	// the master module index g_iMasterModuleIdx

	SetControlsModuleNumber(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, 
		__MASTER_GROUP_01_START, __MASTER_GROUP_01_END, g_iMasterModuleIdx);

	////////////////////////////////////////////////////////
	// Set all cue controls on the master module to map to the Cue physical module

	SetControlsModuleNumber(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, 
		__MASTER_GROUP_02_START, __MASTER_GROUP_02_END, g_iCueModuleIdx);

	////////////////////////////////////////////////////////
	// Handle the controls for the differnet Matrix
	// controls attached to the Master Module
	//
	// We are moving the VIRTUAL controls on the Master to their respective
	// SUB-AUX PHYSICAL modules

	for(iCount=0; iCount < MAX_MATRIX_COUNT; iCount ++)
	{

		if(g_aiMatrix[iCount] != 0)		// Make sure there is a MATRIX module at this position
		{
			iChannel = g_aiAux[iCount];	// Get the corresponding AUX PHYSICAL module channel number


			// AUX MASTER 1-16 LEVEL CONTROLS -  Left-right faders ...

			SetControlsModuleNumber(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, 
				CTRL_NUM_MASTER_AUX1LT_FADER - iCount*4, 
				CTRL_NUM_MASTER_AUX1LT_FADER - iCount*4, 
				iChannel);
			SetControlsModuleNumber(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, 
				CTRL_NUM_MASTER_AUX1RT_FADER - iCount*4, 
				CTRL_NUM_MASTER_AUX1RT_FADER - iCount*4, 
				iChannel);


			// AUX MASTER 1-16 LEVEL CONTROLS - subaux 2

			SetControlsModuleNumber(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, 
				CTRL_NUM_MASTER_AUX2LT_FADER - iCount*4, 
				CTRL_NUM_MASTER_AUX2LT_FADER - iCount*4, 
				iChannel);
			SetControlsModuleNumber(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, 
				CTRL_NUM_MASTER_AUX2RT_FADER - iCount*4, 
				CTRL_NUM_MASTER_AUX2RT_FADER - iCount*4, 
				iChannel);

			// Master SUM-IN

			SetControlsModuleNumber(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, 
				CTRL_NUM_MASTER_SUMIN1 - iCount*2, 
				CTRL_NUM_MASTER_SUMIN1 - iCount*2, 
				iChannel);
			SetControlsModuleNumber(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, 
				CTRL_NUM_MASTER_SUMIN2 - iCount*2, 
				CTRL_NUM_MASTER_SUMIN2 - iCount*2, 
				iChannel);
			// Master Pre-post

			SetControlsModuleNumber(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, 
				CTRL_NUM_MASTER_AUX01PRE - iCount*4, 
				CTRL_NUM_MASTER_AUX01PRE - iCount*4, 
				iChannel);
			SetControlsModuleNumber(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, 
				CTRL_NUM_MASTER_AUX01POST - iCount*4, 
				CTRL_NUM_MASTER_AUX01POST - iCount*4, 
				iChannel);
			SetControlsModuleNumber(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, 
				CTRL_NUM_MASTER_AUX02PRE - iCount*4, 
				CTRL_NUM_MASTER_AUX02PRE - iCount*4, 
				iChannel);
			SetControlsModuleNumber(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, 
				CTRL_NUM_MASTER_AUX02POST - iCount*4, 
				CTRL_NUM_MASTER_AUX02POST - iCount*4, 
				iChannel);

			// Master Aux mutes

			SetControlsModuleNumber(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, 
				CTRL_NUM_MASTER_CUE_AUX01_MUTE - iCount*2, 
				CTRL_NUM_MASTER_CUE_AUX01_MUTE - iCount*2, 
				iChannel);
			SetControlsModuleNumber(gpZoneMaps_Zoom[g_iMasterModuleIdx].lpZoneMap, 
				CTRL_NUM_MASTER_CUE_AUX02_MUTE - iCount*2, 
				CTRL_NUM_MASTER_CUE_AUX02_MUTE - iCount*2, 
				iChannel);
		}
	}
	return iRet;
}


////////////////////////////////////////////
//FUNCTION: SetMemoryMapDefaults
//
//
// Goes throiugh all the Control Nums
// and set the Defaults
//
//
//
////////////////////////////////////////////
int     SetMemoryMapDefaults(void)
{
	int                 iChannel;
	LPCTRLZONEMAP       lpctrlZM;
	LPCTRLZONEMAP       lpctrl;
	int                 iMixerCount;
	int                 iValue;
	int                 iCtrlNum;
	int                 iCtrlAbs;


	for(iChannel = 0; iChannel < giMax_CHANNELS; iChannel++)
	{
		lpctrlZM = gpZoneMaps_Zoom[iChannel].lpZoneMap;

		if(lpctrlZM == NULL)
			continue;

		while(lpctrlZM->rZone.right)
		{
			iCtrlNum = lpctrlZM->iCtrlChanPos;
			iCtrlAbs = lpctrlZM->iCtrlNumAbs;

			if((iCtrlNum != CTRL_NUM_NULL) && (iCtrlAbs > -1) && (lpctrlZM->iModuleNumber > -1) )
			{
				iValue = CDef_GetCtrlDefaultVal(iCtrlAbs);

				if (lpctrlZM->iCtrlType == CTRL_TYPE_BTN_MASTER_HEADPOST_FILTER)
					iValue = CDef_GetCtrlMaxVal(iCtrlAbs);

				// Set this ControlNum for all of the Allocated Mixers
				//----------------------------------------------------
				for(iMixerCount = 0; iMixerCount < giMax_MIXERS; iMixerCount++)
					SETPHISDATAVALUE(iMixerCount, lpctrlZM, iCtrlNum, iValue);

				// Handle the default settings for the Gate_feed_through duda ...
				if(gDeviceSetup.iaChannelTypes[lpctrlZM->iModuleNumber] == DCX_DEVMAP_MODULE_INPUT)
				{ // make sure it is an Input channel
					lpctrl = ScanCtrlZonesNum(lpctrlZM,	CTRL_NUM_INPUT_GATE_FEED_THRU);
					if(lpctrl)
						SetFilteredControlsByNumber(lpctrl, YES_FILTER);
				}

			}
			else
			{
				if((iCtrlNum != CTRL_NUM_NULL) && (lpctrlZM->iModuleNumber > -1))
				{

					////////////////////////////////////////////////////////////
					// This is very UGLY !!! Please fix it !!!!!!!!!!!! ??????
					// These are all SOFTWARE CONTROLS
					// We are setting their default values here, ie on/off
					//
					////////////////////////////////////////////////////////////
					if(

						////////////////////////////////////////////////////////////////////
						//	DEFAULT THE PREPOST FADER BUTTON TO ON 
						// The CTRL_NUM_INPUT_GATE_KEY_VU and CTRL_NUM_INPUT_LINE_B_VU
						// buttons will not be restored below since this one
						// is default on
						////////////////////////////////////////////////////////////////////

						( (lpctrlZM->iCtrlChanPos == CTRL_NUM_INPUT_PREPOST_FADER_VU) && 
						(gDeviceSetup.iaChannelTypes[lpctrlZM->iModuleNumber] == DCX_DEVMAP_MODULE_INPUT) 
				  )
				  ||
				  ( (lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_POST_LT_VU 
				  || lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_POST_RT_VU )
				  && gDeviceSetup.iaChannelTypes[lpctrlZM->iModuleNumber] == DCX_DEVMAP_MODULE_MASTER) 
				  ||
				  ( (lpctrlZM->iCtrlChanPos == CTRL_NUM_SUB_POST_LT_VU
				  || lpctrlZM->iCtrlChanPos == CTRL_NUM_SUB_POST_RT_VU )
				  && gDeviceSetup.iaChannelTypes[lpctrlZM->iModuleNumber] == DCX_DEVMAP_MODULE_MATRIX) 
				  ||
				  ( (lpctrlZM->iCtrlChanPos == CTRL_NUM_MATRIX_POST_LT_VU
				  || lpctrlZM->iCtrlChanPos == CTRL_NUM_MATRIX_POST_RT_VU )
				  && gDeviceSetup.iaChannelTypes[lpctrlZM->iModuleNumber] == DCX_DEVMAP_MODULE_MATRIX) 
				  || (lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_CUEA_SYSTEM_SEL
				  && gDeviceSetup.iaChannelTypes[lpctrlZM->iModuleNumber] == DCX_DEVMAP_MODULE_CUE) )

				  iValue = 0;
					else
						iValue = 2;

					// Set this ControlNum for all of the Allocated Mixers
					//----------------------------------------------------
					for(iMixerCount = 0; iMixerCount < giMax_MIXERS; iMixerCount++)
						SETPHISDATAVALUE(iMixerCount, lpctrlZM, iCtrlNum, iValue);
				}
			}
			lpctrlZM++;
		}
	}

	// Duplicate the settings in to the memory buffer
	//
	MoveMemory(gpwMemMapBuffer, gpwMemMapMixer, giMemMapSize);

	return 0;
}

/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: RecallMemoryMapBuffer
//
//  This routine
//
// Called from: RecallEntry(void)
//							WndMainProc:    case IDM_RESET_ALL_CONTROLS: and 		case IDM_RECAL_ALL_CONTROLS:
//							LoadMixFile((LPFILESTRUCT pfs, BOOL	saveName)
/////////////////////////////////////////////////////////////////////

extern	int	g_CueMasterSystem;

void    RecallMemoryMapBuffer(BOOL bForce,DWORD dwfadeDelay)
{
	CONTROLDATA         ctrlData;
	int                 iChannel;
	LPCTRLZONEMAP       lpctrlZM;
	int                 iValue;
	int                 iCtrlNum;
	int                 iCtrlAbs;
	int                 iCtrlAbsLast = 0;
	LPMIXERWNDDATA      lpmwd;
	int									filtered;
	DWORD								copy_offset = 0;

	DWORD								dwSystemtime, dwNowSystemTime;
	DWORD								dwFadeDelayPerTick;

	LPCTRLZONEMAP       lpctrlZMSave[128];	// <==== SET TO MAX NUMBER OF VERTICAL FADERS
	int									iSavedZM = 0;				// start out with NO saved zone maps
	int									iMaxFaderMovement;	// Saves the maximum of the vertical fader movements
	int i,j;
	int iValueCurrent, iValueSet, iDirection;
	char		szBuff[80];

	int                 iModuleType;	// INPUT, MATRIX, etc
	int		Dummy; 

	iMaxFaderMovement = -1;		// Not Valid

	lpmwd = GetValidMixerWindowData();

	ClearProgress();
	SetProgressRange(0, giMax_CHANNELS);


	for(iChannel = 0; iChannel < giMax_CHANNELS; iChannel++)
	{
		StepProgress(1);
		lpctrlZM = gpZoneMaps_Zoom[iChannel].lpZoneMap;
		if(lpctrlZM == NULL)
			continue;

		// Get the module type (INPUT, MATRIX, etc)
		iModuleType  = gDeviceSetup.iaChannelTypes[lpctrlZM->iModuleNumber];

		setBufferChannelSafeActive (lpctrlZM, FALSE);

		// make sure we do not recall settings for a channel in safe mode
		//
		if (isChanelSafeActive (lpctrlZM) == TRUE)
		{
			LPCTRLZONEMAP safecontrol = ScanCtrlZonesNum ( lpctrlZM, CTRL_NUM_INPUT_SAFE);

			SETPHISDATAVALUEBUFFER (0, safecontrol, CTRL_NUM_INPUT_SAFE, 2);
			continue;
		}

		///////////////////////////////////////////////////////
		// loop through all valid zone maps and recall them
		// ... no recall will be done if the Zone is filtered
		//
		///////////////////////////////////////////////////////
		while(lpctrlZM->rZone.right)
		{
			iCtrlNum = lpctrlZM->iCtrlChanPos;
			iCtrlAbs = lpctrlZM->iCtrlNumAbs;

			if(((iCtrlNum != CTRL_NUM_NULL) && (iCtrlAbs != iCtrlAbsLast) &&
				(iCtrlAbs > -1) && (lpctrlZM->iModuleNumber < 80) && 
				(lpctrlZM->iModuleNumber >= 0)) || IsMuteFilter(lpctrlZM))
			{
				///////////////////////////////////////////////////////////////////////
				// Get the value from the gpwMemMapBuffer.  This buffer is filled
				// by the ReadDataFile() routine when reading a sequence.
				///////////////////////////////////////////////////////////////////////

				iValue = GETPHISDATAVALUEBUFFER(0, lpctrlZM, iCtrlNum);

				if(IsMuteFilter(lpctrlZM) == FALSE && IsCtrlPrePostFilter(lpctrlZM->iCtrlType) == FALSE)
				{

					//////////////////////////////////////////////////////////////////////
					// ONLY update control if value has changed OR we are forcing an update
					//
					//////////////////////////////////////////////////////////////////////
					if(iValue != GETPHISDATAVALUE(0, lpctrlZM, iCtrlNum) || bForce == TRUE)
					{

						/////////////////////////////////////////////
						// If its NOT a fader then update immediatly
						/////////////////////////////////////////////

						if( lpctrlZM->iCtrlType != CTRL_TYPE_FADER_VERT)
						{

							/////////////////////////////////////////////
							// DO NOT RESTORE THESE VALUES
							// These are the 3 VU buttons on the INPUT module
							// and these two will be default OFF so 
							// no need to restore. They are saved and
							// should be removed when time permits.
							//
							// DO NOT RESTORE CUE CONTROLS
							// As per Gamble 10/19/2001, we don't want
							// the cues tied to a sequence change
							// We may want this if we are loading $ASTMIX
							// so that the previous cue settings are there.
							// These values are still stored in the MIX file
							// just not resotre here.
							/////////////////////////////////////////////

							if( 
								(iModuleType == DCX_DEVMAP_MODULE_INPUT) && 
								((lpctrlZM->iCtrlChanPos == CTRL_NUM_INPUT_GATE_KEY_VU) || 
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_INPUT_LINE_B_VU) ||

								(lpctrlZM->iCtrlChanPos == CTRL_NUM_INPUT_CUE_FAD_PRE) ||		// INPUT MODULE
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_INPUT_CUE_FAD_POST) ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_INPUT_MIC_A_CUE) ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_INPUT_MIC_B_CUE) ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_INPUT_CUE_FAD_POST) ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_INPUT_GATE_KEY_INOUT)

								)	
								)

							{
								Dummy = 0;	// Stupid logic but it works, do not want to restore if the above is true	
							}
							else if( 
								(iModuleType == DCX_DEVMAP_MODULE_MASTER) && 
								((lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX16PRE) ||	// MASTER CUES
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX15PRE) ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX14PRE) ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX13PRE) ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX12PRE) ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX11PRE) ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX10PRE) ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX09PRE)  ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX08PRE)  ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX07PRE)  ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX06PRE)  ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX05PRE)  ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX04PRE)  ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX03PRE)  ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX02PRE)  ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX01PRE)  ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX16POST) ||	// MASTER CUES
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX15POST) ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX14POST) ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX13POST) ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX12POST) ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX11POST) ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX10POST) ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX09POST)  ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX08POST)  ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX07POST)  ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX06POST)  ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX05POST)  ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX04POST)  ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX03POST)  ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX02POST)  ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_AUX01POST)	 ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_CUE_LEVEL_MONO)	 ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_CUE_LEVEL_CENTER)	 ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_STEREO_CUE_PRE)	 ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTER_STEREO_CUE_POST)

								)	
								)

							{
								Dummy = 0;	// Stupid logic but it works, do not want to restore if the above is true	
							}
							else if( 
								(iModuleType == DCX_DEVMAP_MODULE_MATRIX) && 
								((lpctrlZM->iCtrlChanPos == CTRL_NUM_MATRIX_STERIO_CUE_PRE) ||	// MATRIX CUES
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MATRIX_STERIO_CUE_POST) ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MATRIX_CUE_PRE) ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MATRIX_CUE_POST)

								)	
								)

							{
								Dummy = 0;	// Stupid logic but it works, do not want to restore if the above is true	
							}
							else
							{
								SETPHISDATAVALUE(0, lpctrlZM, iCtrlNum, iValue);

								if(lpctrlZM->iFiltered == NO_FILTER)
								{
									// Send the Data out
									//------------------
									ctrlData.wMixer   = 0;
									ctrlData.wChannel = lpctrlZM->iModuleNumber;//iChannel;
									ctrlData.wCtrl    = iCtrlAbs; // we use this one since for the definition dll
									ctrlData.wVal     = iValue;
									SendDataToDevice(&ctrlData, FALSE, NULL, 0, NULL, FALSE); // was TRUE
								}
							}

						}
						else if(lpctrlZM->iCtrlType == CTRL_TYPE_FADER_VERT)	// It IS a main fader, so save info so that we can crossfade later
						{


							// If its the CUE FADER then DO NOT RECALL it.
							if( 
								(iModuleType == DCX_DEVMAP_MODULE_MASTER) && 
								((lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTERLT_CUE_A_LT) ||	// MASTER CUES
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTERLT_CUE_A_RT) ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTERLT_CUE_B_LT) ||
								(lpctrlZM->iCtrlChanPos == CTRL_NUM_MASTERLT_CUE_B_RT) 
								)	
								)

							{
								Dummy = 0;	// Stupid logic but it works, do not want to restore if the above is true	
							}
							else
							{
								if(dwfadeDelay)   // But only if crossfade is set
								{
									lpctrlZMSave[iSavedZM++] = lpctrlZM;
									if( abs(GETPHISDATAVALUE(0, lpctrlZM, iCtrlNum) - iValue) > iMaxFaderMovement)				// Save the largest fader movement
										iMaxFaderMovement = abs(GETPHISDATAVALUE(0, lpctrlZM, iCtrlNum) - iValue);
								}
								else		// no crossfade so handle normally (need to make better logic)
								{
									SETPHISDATAVALUE(0, lpctrlZM, iCtrlNum, iValue);

									if(lpctrlZM->iFiltered == NO_FILTER)
									{
										// Send the Data out
										//------------------
										ctrlData.wMixer   = 0;
										ctrlData.wChannel = lpctrlZM->iModuleNumber;//iChannel;
										ctrlData.wCtrl    = iCtrlAbs; // we use this one since for the definition dll
										ctrlData.wVal     = iValue;
										SendDataToDevice(&ctrlData, FALSE, NULL, 0, NULL, FALSE); // was TRUE
									}

								} // end for if(dwfadeDelay)

							} // end if check for CUE FADER


						} // end for != CTRL_TYPE_FADER_VERT

					} // end for iValue != GETPHISDATAVAULE
				}
				else	// IsMuteFilter() == TRUE, so its a MUTE filter, handle it if its changed
				{
					if(iValue != GETPHISDATAVALUE(0, lpctrlZM, iCtrlNum) || bForce == TRUE)
					{
						// The MUTES have been changed OR we are in forced mode
						//
						if(bForce)
						{
							iValue = GETPHISDATAVALUE(0, lpctrlZM, iCtrlNum);
							if(iValue)
								SETPHISDATAVALUE(0, lpctrlZM, iCtrlNum, 0);
							else
								SETPHISDATAVALUE(0, lpctrlZM, iCtrlNum, 2);
						}

						/************************************************************************************
						if(iValue == CDef_GetCtrlMinVal(lpctrlZM->iCtrlNumAbs))
						StartControlDataFilter(iChannel, lpmwd, lpctrlZM, TRUE, FALSE);
						else
						StartControlDataFilter(iChannel, lpmwd, lpctrlZM, FALSE, FALSE);
						if (lpctrlZM->iCtrlType == CTRL_MASTER_AUX_MUTE_FILTER){
						if (lpctrlZM->iCtrlChanPos <= CTRL_NUM_MASTER_CUE_AUX01_MUTE &&
						lpctrlZM->iCtrlChanPos >= CTRL_NUM_MASTER_CUE_AUX16_MUTE){
						//
						if (lpmwd)
						StartControlDataFilter(iChannel, 
						lpmwd, 
						lpctrlZM, 
						(iValue == 0)?FALSE:TRUE, 
						FALSE);
						}
						}
						else{
						************************************************************************************/

						// Press the MUTE button in since the value has changed
						if( (lpctrlZM->iCtrlType == CTRL_MARIXLT_MUTE_FILTER) ||
							(lpctrlZM->iCtrlType == CTRL_MARIXRT_MUTE_FILTER))
						{

							// TODO: HANDLE MATRIX MUTE DIFFERENTLY
							// Since we are restoring we need to fake
							// out a zone map and channel and then
							// handle it like a button click
							// 
							if(lpmwd)
							{
								lpmwd->lpCtrlZM = lpctrlZM;	// Need a zone map
								lpmwd->iCurChan=iChannel;		// Need a channel
								HandleCtrlBtnClick(NULL, lpmwd);
							}


						}
						else
						{
							if(lpmwd)
								HandleCtrlBtnClickInGroup(NULL, lpmwd, lpctrlZM, iChannel);
						}
						/*********************
						}
						**********************/
					}
				}

				iCtrlAbsLast = iCtrlAbs;
			} // end for safety check on variables

			lpctrlZM++;

		} // end for while(lpctrlZM->rZone.right)


		/*
		// Handle Mutes ... that are ON or Off
		//
		lpctrlZM = gpZoneMaps_Zoom[iChannel].lpZoneMap;
		while (lpctrlZM->rZone.right)
		{
		if (lpctrlZM != NULL && lpmwd)
		{
		iValue = CDef_GetCtrlMinVal(lpctrlZM->iCtrlNumAbs);
		iVal = GETPHISDATAVALUE(lpmwd->iMixer, lpctrlZM, lpctrlZM->iCtrlChanPos);

		if(iVal == iValue)
		StartControlDataFilter(iChannel, lpmwd, lpctrlZM, TRUE, FALSE);
		else
		StartControlDataFilter(iChannel, lpmwd, lpctrlZM, FALSE, FALSE);
		}
		lpctrlZM ++;
		}
		*/

		/////////////////////////////////////////////////////////////////////
		// we are done with the normal Channel stuff.
		// now comes a tricky-bit ... only Input modules !!!!!!!!!!!!!!!!!
		// we need to set the gate-depth properly which would be a problem 
		// (i.e. not set correctly) if the In/Out gate switch is not pushed-in
		// and the Gate-depth is anything other than -80.
		// To fix it ... just emulate pushing(in&out) the Gate In/Out button !!!
		// this should be enough ?!
		/////////////////////////////////////////////////////////////////////

		lpctrlZM = gpZoneMaps_Zoom[iChannel].lpZoneMap;
		lpctrlZM = ScanCtrlZonesNum (lpctrlZM, CTRL_NUM_INPUT_GATE_FEEDTHRUINOUT);

		if (lpctrlZM != NULL && 
			gDeviceSetup.iaChannelTypes[lpctrlZM->iModuleNumber] == DCX_DEVMAP_MODULE_INPUT)
		{
			filtered = lpctrlZM->iFiltered;

			if  (GETPHISDATAVALUE(0, lpctrlZM, lpctrlZM->iCtrlChanPos) == CDef_GetCtrlDefaultVal(lpctrlZM->iCtrlNumAbs) )
			{
				////////////////////////////////////////////////////////////
				// push in&out the darn thing 
				// actually just pretend we are pushing it out and its done.
				////////////////////////////////////////////////////////////

				ctrlData.wMixer   = 0;
				ctrlData.wChannel = lpctrlZM->iModuleNumber;//iChannel;
				ctrlData.wCtrl    = lpctrlZM->iCtrlNumAbs; // we use this one since for the definition dll
				ctrlData.wVal     = CDef_GetCtrlDefaultVal(lpctrlZM->iCtrlNumAbs);

				if(lpctrlZM->iFiltered == NO_FILTER)
				{
					//SendDataToDevice(&ctrlData, FALSE, NULL, 0, NULL, TRUE);
					lpctrlZM = gpZoneMaps_Zoom[iChannel].lpZoneMap;
					lpctrlZM = ScanCtrlZonesNum (lpctrlZM, CTRL_NUM_INPUT_GATE_FEED_THRU);
					SetFilteredControlsByNumber(lpctrlZM, YES_FILTER);
					//StartControlDataFilter(iChannel, lpmwd, lpctrlZM, TRUE, FALSE);
				}
			}
			else
			{
				lpctrlZM = gpZoneMaps_Zoom[iChannel].lpZoneMap;
				lpctrlZM = ScanCtrlZonesNum (lpctrlZM, CTRL_NUM_INPUT_GATE_FEED_THRU);

				////////////////////////////////////////////////////////////
				// push in&out the darn thing
				// actually just pretend we are pushing it out and its done.

				ctrlData.wMixer   = 0;
				ctrlData.wChannel = lpctrlZM->iModuleNumber;//iChannel;
				ctrlData.wCtrl    = lpctrlZM->iCtrlNumAbs; // we use this one since for the definition dll
				ctrlData.wVal     = GETPHISDATAVALUEBUFFER(0, lpctrlZM, lpctrlZM->iCtrlChanPos);
				if(filtered == NO_FILTER)
				{
					SetFilteredControlsByNumber(lpctrlZM, NO_FILTER);
					//StartControlDataFilter(iChannel, lpmwd, lpctrlZM, FALSE, FALSE);
					//					if (ctrlData.wVal != GETPHISDATAVALUE(0, lpctrlZM, lpctrlZM->iCtrlChanPos))
					SendDataToDevice(&ctrlData, FALSE, NULL, 0, NULL, FALSE);
				}
			}
		}												
		copy_offset = giMax_CONTROLS*iChannel;
		//MoveMemory ((gpwMemMapMixer+copy_offset), (gpwMemMapBuffer+copy_offset), 
		//						 sizeof(WORD)*giMax_CONTROLS);

	} // end for channel loop


	///////////////////////////////////////////////////////////
	// CrossFade - now move the large faders base on time
	// selected by user
	// This needs to be a timer event of some kind that gets
	// called every 100 ms or so, whatever resolution decided.
	//
	// First time it is called it must find the largest movement
	// and then calculate the rate of change based on the timer
	// resolution and the sequence time selected.
	///////////////////////////////////////////////////////////


	if( (iSavedZM != 0) && (dwfadeDelay))
	{

		// Calculate delay per fader tick movement
		// Check for zero divide
		// iMaxFaderMovement
		// iSavedZM
		// dwfadeDelay

		dwFadeDelayPerTick = dwfadeDelay / (iMaxFaderMovement / iSavedZM);
#ifdef _DEBUG
		sprintf(szBuff, "             dwFadeDelayPerTick=%d   dwfadeDelay=%d ", dwFadeDelayPerTick, dwfadeDelay);//dwMoreThanOne);
		SendMessage(ghwndStatus, SB_SETTEXT, MAKEWPARAM(0,SBT_POPOUT), (LPARAM)szBuff);
#endif

		// loop to get the fader to its maximum position

		for(j=0;j<iMaxFaderMovement;j++)
		{

			dwNowSystemTime = timeGetTime();

			//////////////////////////////////////
			// Loop through each fader zonemap
			//////////////////////////////////////

			for(i=0;i<iSavedZM;i++)
			{
				lpctrlZM = lpctrlZMSave[i];		// Get the zonemap for the vertical fader

				iCtrlNum = lpctrlZM->iCtrlChanPos;
				iCtrlAbs = lpctrlZM->iCtrlNumAbs;

				// MAX = 0, MIN = 113 for Vertical Faders
				iValue = GETPHISDATAVALUEBUFFER(0, lpctrlZM, iCtrlNum);	// Get saved value (where we are going)
				iValueCurrent = GETPHISDATAVALUE(0, lpctrlZM, iCtrlNum);		// Get current value (where we are at)

				if(iValue != iValueCurrent)	// Only increment and move if we are not there yet
				{

					if(iValue > iValueCurrent)
						iDirection = 1;
					else
						iDirection = -1;

					// This should only be done once so that routine can be continuously called
					// until fader reaches destination

					iValueSet = iValueCurrent;		// We start with our current value

					// Move the fader to its new position

					iValueSet=iValueSet+iDirection;	// Add or subtract to get NEXT value

					//
					// This is done in UpdateControlFromNetwork					SETPHISDATAVALUE(0, lpctrlZM, iCtrlNum, iValueSet);	// Set its value

					if((iDirection *iValueSet) <= iValue)
					{
						// Send the Data out
						//------------------
						ctrlData.wMixer   = 0;
						ctrlData.wChannel = lpctrlZM->iModuleNumber;//iChannel;
						ctrlData.wCtrl    = iCtrlAbs; // we use this one since for the definition dll
						ctrlData.wVal     = iValueSet;
						SendDataToDevice(&ctrlData, FALSE, NULL, 0, NULL, FALSE);

						// This updates the visual movement of the FADER

						UpdateControlFromNetwork(ctrlData.wChannel, (WORD)lpctrlZM->iCtrlChanPos, (int)ctrlData.wVal, FALSE);

						// Do our sequence delay
						//#ifdef NODELAY
						do
						{
							dwSystemtime = timeGetTime();
						}
						while(dwFadeDelayPerTick >  dwSystemtime - dwNowSystemTime);
						//#endif
						//
					} // end for iDirection * iValueSet

				} // end for iValue != iValueCurrent

			} // end  for iSaveZM

		} // end for iMaxFaderMovement
	}

	//MoveMemory(gpwMemMapMixer, gpwMemMapBuffer, giMemMapSize);

	// Update all Windows with the new data
	//-------------------------------------

	RefreshAllMDIWindows();
	ClearProgress();

	// handle the master Cue system ..
	//
	g_CueMasterSystem = 0;
	HadleCueMasterSystem ();
}


//=========================================
//FUNCTION: PhisDataToScrPos
//
//
//purpose:
//      Converts a data value to a screen
//      position
//
//=========================================
int     PhisDataToScrPos(LPCTRLZONEMAP lpctrlZM, int iIndx, int iVal)
{
#ifdef MIDI_SUPPORT
	LPMIDITOSCRTABLE    lpMIDITOSCR;
	LPWORD              lpData;
	int                 iReturn;

	if((iVal < 0) || (iIndx < 0))
		return 0;

	// Get the MIDI to Screen table pointer
	//-------------------------------------
	lpMIDITOSCR = (LPMIDITOSCRTABLE)gpBinResTable[iIndx].lpBinRes;

	if(lpMIDITOSCR->iTotVals <= iVal)
		iVal = lpMIDITOSCR->iMax;

	// go to the correct place and grab the data
	//------------------------------------------
	(LPSTR)lpData = (LPSTR )lpMIDITOSCR + sizeof(MIDITOSCRTABLE);
	lpData = lpData + iVal;
	iReturn = (int)*lpData;

#endif
	return iVal;
}

//=========================================
//FUNCTION: ScrPosToPhisData
//
//
//purpose:
//      Converts a Screen Position to
//      phisical data
//
//=========================================
int     ScrPosToPhisData(LPCTRLZONEMAP lpctrlZM, int iIndx, int iVal)
{
	LPSCRTOMIDITABLE    lpSCRTOMIDI;
	LPWORD              lpData;
	int                 iReturn;

	if((iVal < 0) || (iIndx < 0))
	{
		MessageBeep(0);  // Temporary for DEBUG
		return 0;
	}

	// Get the RdOut table pointer
	//----------------------------
	lpSCRTOMIDI = (LPSCRTOMIDITABLE)gpBinResTable[iIndx].lpBinRes;

	if(lpSCRTOMIDI->iTotVals <= iVal)
		iVal = lpSCRTOMIDI->iMax;


	(LPSTR)lpData = (LPSTR)lpSCRTOMIDI + sizeof(SCRTOMIDITABLE);
	lpData = lpData + iVal;
	iReturn = (int)*lpData;

	return iReturn;
}


//////////////////////////////////////////////////////////////
// void    InitVULookupTables(BOOL bLinear)
//
// Initiallizes the VU lookup table so that the raw VU
// data can be scaled to the screen display.
//
//////////////////////////////////////////////////////////////

#define   VU_DIVIDER_001    (float)728.2054
#define   PIXELS_PER_SEGMENT (float)3

void    InitVULookupTables(BOOL bLinear)
{
	int     iCount;
	float   fDacRead, fDBLow, fDBPixPerSeg, fPixelShift, fDBPerSeg, fDacGate, fDacComp;

	ZeroMemory(gVU_VertDispTable, 4096);
	for(iCount = 8; iCount < 4096; iCount ++)
	{
		fDacRead = (float)iCount;
		fDacRead = ((float)log10(fDacRead/VU_DIVIDER_001)) * 20;

		/////////////////////////////////////////////////////////
		//
		// 40.0 changed to 38.0  - 12/28/2000
		/////////////////////////////////////////////////////////

		if(bLinear)
			// Tom's formula #1..................
			fDacRead = (float)(((fDacRead + 40.0) / 55.0) * (float)gbmpVUONVert.bmHeight);	// <=== 38/40
		else
		{

			// Tom's formula #2...................
			if(iCount < 116)
			{
				fDBLow = -40.0;		// <== 38/40
				fDBPixPerSeg = 1.0;
				fPixelShift = 2.0;
			}
			else
			{
				fDBLow = -15.5;
				fDBPixPerSeg = 0.5;
				fPixelShift = 77.0;
			}

			if(iCount >= 4095)
				fDacRead = 263.0;
			else
				fDacRead = (float)(
				fPixelShift+(fDacRead - fDBLow)* (PIXELS_PER_SEGMENT / fDBPixPerSeg)
				);

		}

		if(fDacRead < 0.0)
			gVU_VertDispTable[iCount] = 0;
		else
			gVU_VertDispTable[iCount] = (int)fDacRead;
	}


	//---------------------------------
	//---------------------------------
	ZeroMemory(gVU_CompDispTable, 4096);
	ZeroMemory(gVU_GateDispTable, 4096);

	//////////////////////////////////////////////////////
	// do compression meter,
	// Comp ADC range: <52 map to 0dB, >3033 map to 20dB
	//////////////////////////////////////////////////////

	for(iCount = 0; iCount < 4096; iCount ++)
	{
		if (iCount == 0)
			fDacComp = 1.0; // keep log from blowing up
		else
			fDacComp = (float)iCount;

		fDacComp = (float)(4.79
			+    ( 20.0*log10(fDacComp/300.0) ) / 1.98
			+ pow( 20.0*log10(fDacComp/300.0) ,2.0) / 79.794);
		if(fDacComp < 0.0)
			fDacComp = 0.0;
		if(fDacComp > 20.0)
			fDacComp = 20.0;

		if(fDacComp < 12.0)
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

		if(iCount <52) fDacComp=0;

		//fDacComp = 96 - (fPixelShift + (fDacComp - fDBLow)* (PIXELS_PER_SEGMENT / fDBPerSeg) );
		fDacComp = (fPixelShift + (fDacComp - fDBLow)* (PIXELS_PER_SEGMENT / fDBPerSeg) );

		gVU_CompDispTable[iCount] = (int)fDacComp;
	}

	//////////////////////////////////////////////////////
	// do gate meter,
	// Gate ADC range: <400 map to 0dB, >3959 map to 20dB
	//////////////////////////////////////////////////////

	for(iCount = 0; iCount < 4096; iCount ++)
	{
		if (iCount == 0)
			fDacGate = 1.0; // keep log from blowing up
		else
			fDacGate = (float)iCount;

		fDacGate = (float)(-49.7815
			+    ( 20.0*log10(fDacGate) )
			- pow( 20.0*log10(fDacGate) ,2.0) / 521.229
			+ pow( 20.0*log10(fDacGate) ,3.0) / 48000.0 );
		if(fDacGate < -38.0)	// <=== changed from 38 to 34 12/28/2000
			fDacGate = -60.0;
		if(fDacGate > 20.0)
			fDacGate = 20.0;

		if(fDacGate < 5.0)
		{
			fDBLow = -38.0;	// <=== changed from 38 to 34 12/28/2000
			fDBPerSeg = 5.0;
			fPixelShift = 2.0;
		}
		else if(fDacGate < 12.0)
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


		//if(iCount <400) fDacGate=0;
		//fDacGate = 96 - (fPixelShift + (fDacGate - fDBLow)* (PIXELS_PER_SEGMENT / fDBPerSeg) );

		fDacGate = (fPixelShift + (fDacGate - fDBLow)* (PIXELS_PER_SEGMENT / fDBPerSeg) );
		gVU_GateDispTable[iCount] = (int)fDacGate;

	}
}


//
#ifdef _DEBUG
WORD  GETPHISDATAVALUE(int mixer, LPCTRLZONEMAP       pctrlzm, int control)   
{
	WORD	*mem_pointer;
	if(pctrlzm->iModuleNumber >= 80 || pctrlzm->iModuleNumber < 0)
	{
		MessageBox(ghwndMain, "iModuleNumber is not valid", "ERROR", MB_ICONERROR | MB_OK);
		return 0;
	}
	else
	{
		mem_pointer = &gpwMemMapMixer[(mixer*(giMax_CHANNELS*giMax_CONTROLS))+(pctrlzm->iModuleNumber * giMax_CONTROLS)+control];
		return *mem_pointer;
	}
};

// Set the Phisical Data Value
// This causes the control to be drawn in the new position
//-----------------------------------------------------------

void  SETPHISDATAVALUE(int mixer, LPCTRLZONEMAP       pctrlzm, int control,int value)
{
	WORD	*mem_pointer;

	if(pctrlzm->iModuleNumber >= 80 || pctrlzm->iModuleNumber < 0)
	{
		MessageBox(ghwndMain, "iModuleNumber is not valid", "ERROR", MB_ICONERROR | MB_OK);
		return;
	}
	else
	{
		mem_pointer = &gpwMemMapMixer[(mixer*(giMax_CHANNELS*giMax_CONTROLS))+(pctrlzm->iModuleNumber * giMax_CONTROLS)+control];
		*mem_pointer = (WORD)value;
	}
}   

WORD  GETPHISDATAVALUEBUFFER(int mixer, LPCTRLZONEMAP       pctrlzm, int control)
{
	WORD	*mem_pointer;
	if(pctrlzm->iModuleNumber >= 80 || pctrlzm->iModuleNumber < 0)
	{
		MessageBox(ghwndMain, "iModuleNumber is not valid", "ERROR", MB_ICONERROR | MB_OK);
		return 0;
	}
	else
	{
		mem_pointer = &gpwMemMapBuffer[(mixer*(giMax_CHANNELS*giMax_CONTROLS))+(pctrlzm->iModuleNumber * giMax_CONTROLS)+control];
		return  *mem_pointer;
	}
}
void  SETPHISDATAVALUEBUFFER(int mixer, LPCTRLZONEMAP       pctrlzm, int control,int value)   
{
	WORD	*mem_pointer;
	if(pctrlzm->iModuleNumber >= 80 || pctrlzm->iModuleNumber < 0)
	{
		MessageBox(ghwndMain, "iModuleNumber is not valid", "ERROR", MB_ICONERROR | MB_OK);
		return;
	}
	else
		mem_pointer = &gpwMemMapBuffer[(mixer*(giMax_CHANNELS*giMax_CONTROLS))+(pctrlzm->iModuleNumber * giMax_CONTROLS)+control];
	*mem_pointer = (WORD)value;
}
#endif // _DEBUG
