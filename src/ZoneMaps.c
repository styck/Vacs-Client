//=================================================
// Copyright 2001 CorTek Software, Inc.
//=================================================

//=================================================
// The Zone map functions: load, reload, save ...
//
//
//=================================================

//#include <windows.h>

#include "SAMM.h"
#include "SAMMEXT.h"
#include "MACRO.h"

#include "ConsoleDefinition.h"

// ZoneMap NULL Function
//
void    ZoneMapNullFunction(HDC hdc, LPCTRLZONEMAP lpctrlZM, int iVal, LPMIXERWNDDATA lpmwd, int iChan)
{
  return;
}


//===============================
//FUNCTION: LoadZoneMapIds
//
//purpose:
//      Load all of the Zone map
//      IDs and strings so they
//      can be loaded from the
//      resource.
//
//return:
//      0 if successfull
//      n if error
//===============================
int   LoadZoneMapIDs(void)
{
HRSRC           hResource;
LPZONE_MAP_ID   pZMID;

// Find and load the Full View Descriptors
//----------------------------------------
hResource = FindResource(ghInstConsoleDef, MAKEINTRESOURCE(IDTAZ_FULL),
                                           MAKEINTRESOURCE(TABLE_OF_ALL_ZONEMAPS));
if(hResource == NULL)
    return IDS_ERR_RESOURCE_ZONES;

// lock and load the resource
//---------------------------
ghZoneMapID_Full = LoadResource(ghInstConsoleDef, hResource);
if(ghZoneMapID_Full == NULL)
    return IDS_ERR_RESOURCE_ZONES;
gpZoneMapID_Full = (LPZONE_MAP_ID)LockResource(ghZoneMapID_Full);

// Count the ZoneMap IDs
// for the Full Zoom
//----------------------
giZoneMapID_Count_Full = 0; // makes sure its set to 0
pZMID = gpZoneMapID_Full;
while( (pZMID++)->w_ZM_ID != 0xffff)
    giZoneMapID_Count_Full++;

if(giZoneMapID_Count_Full == 0)
    return IDS_ERR_RESOURCE_ZONES;


// Find and load the Zoom View Descriptors
//----------------------------------------
hResource = FindResource(ghInstConsoleDef, MAKEINTRESOURCE(IDTAZ_ZOOM),
                                           MAKEINTRESOURCE(TABLE_OF_ALL_ZONEMAPS));
if(hResource == NULL)
    return IDS_ERR_RESOURCE_ZONES;

// lock and load the resource
//---------------------------
ghZoneMapID_Zoom = LoadResource(ghInstConsoleDef, hResource);
if(ghZoneMapID_Zoom == NULL)
    return IDS_ERR_RESOURCE_ZONES;
gpZoneMapID_Zoom = (LPZONE_MAP_ID)LockResource(ghZoneMapID_Zoom);

// Count the ZoneMap IDs
// for the Zoom
//----------------------
giZoneMapID_Count_Zoom = 0; // makes sure its set to 0
pZMID = gpZoneMapID_Zoom;

while( (pZMID++)->w_ZM_ID != 0xffff)
    giZoneMapID_Count_Zoom++;

if(giZoneMapID_Count_Zoom == 0)
    return IDS_ERR_RESOURCE_ZONES;

return 0;
}

//============================================
//FUNCTION: FreeZoneMapIDs
//
//purpose: Free all of the memory allocated
// with LoadZoneMapIDs.
//
//============================================
void      FreeZoneMapIDs(void)
{

if(ghZoneMapID_Full)
    {
    UnlockResource(ghZoneMapID_Full);
    FreeResource(ghZoneMapID_Full);
    gpZoneMapID_Full = NULL;
    }
if(ghZoneMapID_Zoom)
    {
    UnlockResource(ghZoneMapID_Zoom);
    FreeResource(ghZoneMapID_Zoom);
    gpZoneMapID_Zoom = NULL;
    }

return;
};

//===============================================================
// FUNCTION: LoadZoneMaps;
// purpose:
//      load the Zonemap ID Table,
//      the Zonemap descriptor tables
//      and all of the zone maps
//      that are needed to create
//      the views
//variables:
//          hInstMainRes from where the resources will be loaded
//
//
//return:
//          0 if no errors
//          n error code
//================================================================
int LoadZoneMaps(void)
{
  LPCTRLZONEMAPRESOURCE   lpczmr;
  int                     iCount;
  int                     iResCount;
  HGLOBAL                 hGlob;
  HRSRC                   hResource;
  int                     iCurMType = 0;
  int                     iScreenCntType0 = 1;
  int                     iScreenCntType1 = 1;
  int                     iScreenCntType2 = 1;
  int                     iErr;
  char                    szBuffer[4];
  int                     iCountSub = 1;

  iResCount = 0;
  hResource = NULL;
  iErr = 0;


  // Reareange the Zonemaps .. the server might be set in any oreder, so
  // we need to make sure that the Input, Sub and master channel
  // are in sequential order, the rest of the channels should be
  // disregarded ... we don't have a bitmap or a zone table for them
  // 
  //PrepareGlobalChannelsArray();  

// Check for which Zone map/Bitmap was selected
// for the Full view
//---------------------------------------------
if(gprfPref.iFullViewSet >= giZoneMapID_Count_Full)
    {
    InformationBox(ghwndMain, ghInstStrRes, IDS_WRONG_VIEWSET);
    gprfPref.iFullViewSet = 0; // Set it to 0
    }

// Load the Zone Map Descriptor for the Full View
//-----------------------------------------------
hResource = FindResource(ghInstConsoleDef,MAKEINTRESOURCE(gpZoneMapID_Full[gprfPref.iFullViewSet].w_ZM_ID), 
                                          MAKEINTRESOURCE(ZONEMAPDESCRIPTOR));
if(hResource == NULL)
    {
    iErr = IDS_ERR_RESOURCE_DESCRIPTOR;
    goto OnExit;
    }

ghZoneMapDesc_Full = LoadResource(ghInstConsoleDef, hResource);
if(ghZoneMapDesc_Full == NULL)
    {
    iErr = IDS_ERR_RESOURCE_DESCRIPTOR;
    goto OnExit;
    }
gpZoneMapDesc_Full = (LPZONE_MAP_DESC)LockResource(ghZoneMapDesc_Full);

/*
// Count the Zonemap(modules) in the FULL zone map descriptor
// table, so we can allocate enough memory
//-----------------------------------------------------------
glZonesCount_Full = 0;
while(gpZoneMapDesc_Full[glZonesCount_Full].wZMResID != 0xFFFF)
    {
    glZonesCount_Full++; // this will keep track of
                        // how many zonesmaps are in the
                        // full view
                        //------------------------------
    }

// Check if something went wrong
//------------------------------
if(glZonesCount_Full == 0)
    {
    iErr = IDS_ERR_ZONE_MAP_COUNT;
    goto OnExit;
    }

*/

// Allocate the memory to hold all of the actual Zone maps
// for the Full View and their Bitmaps
//--------------------------------------------------------       
gpZoneMaps_Full = (LPZONE_MAP)GlobalAlloc(GPTR, sizeof(ZONE_MAP)*MAX_CHANNELS);
if(gpZoneMaps_Full == NULL)
    {
    iErr = IDS_ERR_ALLOCATE_MEMORY;
    goto OnExit;
    }

  // Load, create and ... everything with the ZoneMaps
  // and their respective Bitmaps
  //
  // NOTE: After this point the ZoneMaps
  // should be actualy useful
  //--------------------------------------------------
  //for(iCount = 0; iCount < glZonesCount_Full; iCount++)
  for(iCount = 0; iCount < MAX_CHANNELS; iCount++)
    {
    // Start loading the binary data for the Zone map
    //-----------------------------------------------
    switch(gDeviceSetup.iaChannelTypes[iCount])
    {
    case 1:
      iCurMType = 0;
      hResource = FindResource(ghInstConsoleDef, MAKEINTRESOURCE(gpZoneMapDesc_Full[iCurMType].wZMResID), 
                                                 MAKEINTRESOURCE(ZONEMAP) );
      break;
    case 2:
      iCurMType = 1;

      hResource = FindResource(ghInstConsoleDef, MAKEINTRESOURCE(gpZoneMapDesc_Full[iCurMType].wZMResID), 
                                                 MAKEINTRESOURCE(ZONEMAP) );

      break;
    case 5:
      iCurMType = 2;
      hResource = FindResource(ghInstConsoleDef, MAKEINTRESOURCE(gpZoneMapDesc_Full[iCurMType].wZMResID), 
                                                 MAKEINTRESOURCE(ZONEMAP) );
      break;
    default:
      hResource = NULL;
      break;
    }
    if(hResource == NULL)             
      {
      //iErr = IDS_ERR_ZONEMAP_RESOURCE;
      //goto OnExit;
      continue;
      }

    hGlob = LoadResource(ghInstConsoleDef, hResource);
    if(hGlob == NULL)
        {
        iErr = IDS_ERR_ZONEMAP_RESOURCE;
        goto OnExit;
        }

      lpczmr = (LPCTRLZONEMAPRESOURCE)LockResource(hGlob);

      // inittialize this Zone Map
      //--------------------------
      iErr = InitZoneMap(lpczmr, &gpZoneMaps_Full[iCount]);
      UnlockResource(hGlob);
      FreeResource(hGlob);
      if(iErr)
          goto OnExit;

      // Load the Bitmap
      //----------------
      iErr = AddBmpResGlobal(ghInstConsoleDef, gpZoneMapDesc_Full[iCurMType].wZMBmpID, ghdc256);
      if(iErr < 0)
          {
          iErr = -(iErr);
          goto OnExit;
          }
      gpZoneMaps_Full[iCount].iBmpIndx = iErr;

      // Store the Default label
      //------------------------
      CopyMemory((LPSTR)gpZoneMaps_Full[iCount].chDefLabel, (LPSTR)gpZoneMapDesc_Full[iCurMType].sDefLabel, 4);

      // Store the Zone Map ID
      //----------------------
      gpZoneMaps_Full[iCount].wID = gpZoneMapDesc_Full[iCurMType].wZMResID;
      }


// HERE starts the ZOOM View ZoneMap Loading
//------------------------------------------
//------------------------------------------

// Check for which Zone map/Bitmap was selected
// for the Zoom view
//---------------------------------------------
if(gprfPref.iZoomViewSet >= giZoneMapID_Count_Zoom)
    {
    InformationBox(ghwndMain, ghInstStrRes, IDS_WRONG_VIEWSET);
    gprfPref.iZoomViewSet = 0; // Set it to 0
    }

// Load the Zone Map Descriptor for the Full View
//-----------------------------------------------
hResource = FindResource(ghInstConsoleDef, MAKEINTRESOURCE(gpZoneMapID_Zoom[gprfPref.iZoomViewSet].w_ZM_ID), 
                                           MAKEINTRESOURCE(ZONEMAPDESCRIPTOR));
if(hResource == NULL)
    {
    iErr = IDS_ERR_RESOURCE_DESCRIPTOR;
    goto OnExit;
    }

ghZoneMapDesc_Zoom = LoadResource(ghInstConsoleDef, hResource);
if(ghZoneMapDesc_Zoom == NULL)
    {
    iErr = IDS_ERR_RESOURCE_DESCRIPTOR;
    goto OnExit;
    }
gpZoneMapDesc_Zoom = (LPZONE_MAP_DESC)LockResource(ghZoneMapDesc_Zoom);

/*
// Count the Zonemap(modules) in the FULL zone map descriptor
// table, so we can allocate enough memory
//-----------------------------------------------------------
glZonesCount_Zoom = 0;
while(gpZoneMapDesc_Zoom[glZonesCount_Zoom].wZMResID != 0xFFFF)
    {
    glZonesCount_Zoom++; // this will kepp track of
                        // how many zonesmaps are in the
                        // zoom view
                        //------------------------------
    }

// Check if something went wrong
//------------------------------
if(glZonesCount_Zoom == 0)
    {
    iErr = IDS_ERR_ZONE_MAP_COUNT;
    goto OnExit;
    }

// Allocate the memory to hold all of the actual Zone maps
// for the Full View and their Bitmaps
//--------------------------------------------------------

  // Get the Device Channel Count
  // NEW STUFF
  glZonesCount_Zoom = GetDeviceChannelCount();
  if(glZonesCount_Zoom == 0)
      {
      iErr = IDS_ERR_ZONE_MAP_COUNT;
      goto OnExit;
      }

*/

gpZoneMaps_Zoom = (LPZONE_MAP)GlobalAlloc(GPTR, sizeof(ZONE_MAP)*MAX_CHANNELS);
if(gpZoneMaps_Zoom == NULL)
    {
    iErr = IDS_ERR_ALLOCATE_MEMORY;
    goto OnExit;
    }

// Load, create and ... everything with the ZoneMaps
// and their respective Bitmaps
//
// NOTE: After this point the ZoneMaps
// should be actualy useful
//--------------------------------------------------
for(iCount = 0; iCount < MAX_CHANNELS; iCount++)
    {
    // Start loading the binary data for the Zone map
    //-----------------------------------------------
    switch(gDeviceSetup.iaChannelTypes[iCount])
    {
    case 1:
      iCurMType = 0;
      hResource = FindResource(ghInstConsoleDef, MAKEINTRESOURCE(gpZoneMapDesc_Zoom[iCurMType].wZMResID), 
                                                 MAKEINTRESOURCE(ZONEMAP) );
      wsprintf(szBuffer, "%02d", iScreenCntType0 ++);

      break;
    case 2:
      iCurMType = 1;
      hResource = FindResource(ghInstConsoleDef, MAKEINTRESOURCE(gpZoneMapDesc_Zoom[iCurMType].wZMResID), 
                                                 MAKEINTRESOURCE(ZONEMAP) );

      wsprintf(szBuffer, "%02d", iScreenCntType1 ++);
      break;
    case 5:
      iCurMType = 2;
      hResource = FindResource(ghInstConsoleDef, MAKEINTRESOURCE(gpZoneMapDesc_Zoom[iCurMType].wZMResID), 
                                                 MAKEINTRESOURCE(ZONEMAP) );
      wsprintf(szBuffer, "%02d", iScreenCntType2 ++);
      break;
    default:
      hResource = NULL;
      break;
    }
    if(hResource == NULL)             
      {
      //iErr = IDS_ERR_ZONEMAP_RESOURCE;
      //goto OnExit;
      continue;
      }

    hGlob = LoadResource(ghInstConsoleDef, hResource);
    if(hGlob == NULL)
      {
      iErr = IDS_ERR_ZONEMAP_RESOURCE;
      goto OnExit;
      }
    lpczmr = (LPCTRLZONEMAPRESOURCE)LockResource(hGlob);


    // inittialize this Zone Map
    //--------------------------
    iErr = InitZoneMap(lpczmr, &gpZoneMaps_Zoom[iCount]);
    UnlockResource(hGlob);
    FreeResource(hGlob);
    if(iErr)
        goto OnExit;

    // Load the Bitmap
    //----------------
    iErr = AddBmpResGlobal(ghInstConsoleDef, gpZoneMapDesc_Zoom[iCurMType].wZMBmpID, ghdc256);
    if(iErr < 0)
        {
        iErr = -(iErr);
        goto OnExit;
        }

    gpZoneMaps_Zoom[iCount].iBmpIndx = iErr;

    // Store the Default label
    //------------------------
    CopyMemory((LPSTR)gpZoneMaps_Zoom[iCount].chDefLabel, (LPSTR)szBuffer, 4);    
//    CopyMemory((LPSTR)gpZoneMaps_Zoom[iCount].chDefLabel, (LPSTR)gpZoneMapDesc_Zoom[iCurMType].sDefLabel, 4);    

    // Store the Zone Map ID
    //----------------------
    gpZoneMaps_Zoom[iCount].wID = gpZoneMapDesc_Zoom[iCurMType].wZMResID;
    }


  // Allocate the labels memory
  //---------------------------
  gpLabels = GlobalAlloc(GPTR, giActiveMixers*MAX_CHANNELS*MAX_LABEL_SIZE);
  if(gpLabels == NULL)
      return IDS_ERR_ALLOCATE_MEMORY;

  for(iCount = 0; iCount < MAX_CHANNELS; iCount++)
  {
    if(gDeviceSetup.iaChannelTypes[iCount] == DCX_DEVMAP_MODULE_INPUT)
      wsprintf(&gpLabels[iCount * MAX_LABEL_SIZE], "Channel #%d", iCount + 1);
    else
      if(gDeviceSetup.iaChannelTypes[iCount] == DCX_DEVMAP_MODULE_AUX)
      {
        wsprintf(&gpLabels[iCount * MAX_LABEL_SIZE], "Sub Group %d", iCountSub++);
      }
      else
        if(gDeviceSetup.iaChannelTypes[iCount] == DCX_DEVMAP_MODULE_MASTER)
          wsprintf(&gpLabels[iCount * MAX_LABEL_SIZE], "Master");
  }

  g_pAuxLabels = GlobalAlloc(GPTR, giActiveMixers*MAX_AUX_CHANNELS*MAX_LABEL_SIZE);
  if(g_pAuxLabels == NULL)
      return IDS_ERR_ALLOCATE_MEMORY;

  for(iCount = 0; iCount < MAX_AUX_CHANNELS; iCount++)
  {
    wsprintf(&g_pAuxLabels[iCount * MAX_LABEL_SIZE], "Aux Master %d", iCount+1);
  }

  g_pMatrixLabels = GlobalAlloc(GPTR, giActiveMixers*MAX_AUX_CHANNELS*MAX_LABEL_SIZE);
  if(g_pMatrixLabels == NULL)
      return IDS_ERR_ALLOCATE_MEMORY;
  for(iCount = 0; iCount < MAX_AUX_CHANNELS; iCount++)
  {
    wsprintf(&g_pMatrixLabels[iCount * MAX_LABEL_SIZE], "Matrix %d", iCount+1);
  }

iErr = 0;
OnExit:
return iErr;
};

//===========================================
//FUNCTION: InitZoneMap
//
//purpose:
//      Load all the Bitmaps for this Control
//  then set the correct Function pointer
//  to handle this control actions.
//
//
//===========================================
int     InitZoneMap(LPCTRLZONEMAPRESOURCE lpczmr, LPZONE_MAP lpzm)
{
LPCTRLZONEMAPRESOURCE   lpczmrLoad;
int                     iBmpIndex;
int                     iCount;
int                     iZonesCount;
LPCTRLZONEMAP           lpctrlzm;

lpczmrLoad = lpczmr;


iZonesCount = 0;
// first Count how many Zones are in this Zone map
//------------------------------------------------
while(lpczmrLoad[iZonesCount].rZone.Right)
    {
    iZonesCount++;
    }

iZonesCount ++; // One more for the terminator Zone

lpctrlzm = (LPCTRLZONEMAP)GlobalAlloc(GPTR, iZonesCount * sizeof(CTRLZONEMAP));
if(lpctrlzm==NULL)
    return IDS_ERR_ALLOCATE_MEMORY;



iZonesCount = 0;
// while there is a defined Zone
// keep loading.
// Every new Zone map gets remaped
// into the new pointer to CTRLZONEMAP
// where we can store the information
// for the Control Function,
// For the Bitmaps and for
// the MIDI remap table ....
//------------------------------------
while(lpczmrLoad->rZone.Right)
    {
    // first find this control in the Console definition lookup table
    // If the control is not found the returned value will be -1 ...
    // we use the control name to identify a control
    //----------------------------------------------------------------
    if(lstrlen(lpczmrLoad->chControlName) > 0)
      {
      lpctrlzm[iZonesCount].iCtrlNumAbs = CDef_FindControlByName(lpczmrLoad->chControlName);
      if(lpctrlzm[iZonesCount].iCtrlNumAbs < 0)
			{
				MessageBox(NULL, lpczmrLoad->chControlName, "Control Name Error!", MB_ICONSTOP);
        return IDS_CDEF_CONTROLNAME_NOTFOUND;
			}
      lpctrlzm[iZonesCount].iNumValues = CDef_GetCtrlMaxVal(lpctrlzm[iZonesCount].iCtrlNumAbs) - 
                                         CDef_GetCtrlMinVal(lpctrlzm[iZonesCount].iCtrlNumAbs);
      }
    else
      lpctrlzm[iZonesCount].iCtrlNumAbs = -1;
    
    // Find the Alternative Control Index ....
    if(lstrlen(lpczmrLoad->chControlNameAlt1) > 0)
      {
      lpctrlzm[iZonesCount].iCtrlNumAbsAlt1 = CDef_FindControlByName(lpczmrLoad->chControlNameAlt1);
      }
    else
      lpctrlzm[iZonesCount].iCtrlNumAbsAlt1 = -1;


    // the control number within the channel
    //--------------------------------------
    lpctrlzm[iZonesCount].iCtrlChanPos = lpczmrLoad->uCtrlChanPos;

    // transfer other varibales
    //-------------------------
    lpctrlzm[iZonesCount].rZone.right = lpczmrLoad->rZone.Right;
    lpctrlzm[iZonesCount].rZone.left = lpczmrLoad->rZone.Left;
    lpctrlzm[iZonesCount].rZone.top = lpczmrLoad->rZone.Top;
    lpctrlzm[iZonesCount].rZone.bottom = lpczmrLoad->rZone.Bottom;
    ZeroMemory(&lpctrlzm[iZonesCount].rLastDraw,sizeof(RECT));
    lpctrlzm[iZonesCount].iCtrlType = lpczmrLoad->uCtrlType;
    lpctrlzm[iZonesCount].iDispType = lpczmrLoad->uDispType;
    lpctrlzm[iZonesCount].iModuleNumber = 0xFFFFFFFF; // Set this to Invalid module number    

    // now deal with the Bitmaps
    //--------------------------
    for(iCount = 0; iCount < 3; iCount++)
        {
        if(lpczmrLoad->iCtrlBmp[iCount])
            {
            iBmpIndex = AddBmpResGlobal(ghInstConsoleDef, lpczmrLoad->iCtrlBmp[iCount], ghdc256);
            if(iBmpIndex < 0)
                return -(iBmpIndex);
            // now replace the Resource index with
            // the index from the global
            // Bmp storage
            //------------------------------------
            lpctrlzm[iZonesCount].iCtrlBmp[iCount] = iBmpIndex;

            // Set the Min and Max Screen Positions
            //-------------------------------------
            switch(lpczmrLoad->uCtrlType)
                {
                case CTRL_TYPE_FADER_VERT:
                    lpctrlzm[iZonesCount].iMinScrPos =  lpczmrLoad->rZone.Top + (gpBMPTable[iBmpIndex].iHeight / 2);
                    lpctrlzm[iZonesCount].iMaxScrPos =  lpczmrLoad->rZone.Bottom - (gpBMPTable[iBmpIndex].iHeight / 2);
                    lpctrlzm[iZonesCount].iNumScrPos =  lpctrlzm[iZonesCount].iMaxScrPos - lpctrlzm[iZonesCount].iMinScrPos;
                    break;
                case CTRL_TYPE_FADER_HORZ:
                    lpctrlzm[iZonesCount].iMinScrPos =  lpczmrLoad->rZone.Left + (gpBMPTable[iBmpIndex].iWidth / 2);
                    lpctrlzm[iZonesCount].iMaxScrPos =  lpczmrLoad->rZone.Right - (gpBMPTable[iBmpIndex].iWidth / 2);
                    lpctrlzm[iZonesCount].iNumScrPos =  lpctrlzm[iZonesCount].iMaxScrPos - lpctrlzm[iZonesCount].iMinScrPos;
                    break;
                }
            }
        else
            {
            lpctrlzm[iZonesCount].iCtrlBmp[iCount] = -1; // therefore there is no Bitmap for this state of the Control
            }
        }

    // Load the Read Outs for this Control
    //------------------------------------
//    iRdOutIndx = AddRdOutResGlobal(ghInstConsoleDef, lpczmrLoad->iRdOutRes);
//    if(iRdOutIndx < 0)
//        {
//        return -(iRdOutIndx);
//        }
//    lpctrlzm[iZonesCount].iRdOutIndx = iRdOutIndx;

    // Load the Midi Remap Tables(MIDI_to_Scr and Scr_to_MIDI)
    //--------------------------------------------------------
/*
    if(lpczmrLoad->iMIDI_to_Scr_res != RT_MIDITOSCR_NOTHING)
        {
        iRemapMIDI = AddBinResource(ghInstConsoleDef, lpczmrLoad->iMIDI_to_Scr_res, RT_REMAP_MIDITOSCREEN); // the MIDI to Screen stuff
        if(iRemapMIDI < 0)
            {
            return -(iRemapMIDI);
            }
        }
    else
        iRemapMIDI = -1;
    lpctrlzm[iZonesCount].iMIDIToScr_Indx = iRemapMIDI;

    // load the Screen to MIDI remap table
    //------------------------------------
    if(lpczmrLoad->iScr_To_MIDI_res != RT_SCRTOMIDI_NOTHING)
        {
        iRemapMIDI = AddBinResource(ghInstConsoleDef, lpczmrLoad->iScr_To_MIDI_res, RT_REMAP_SCREENTOMIDI); // the Screen to MIDI stuff
        if(iRemapMIDI < 0)
            {
            return -(iRemapMIDI);
            }
        }
    else
        iRemapMIDI = -1;
    lpctrlzm[iZonesCount].iScrToMIDI_Indx = iRemapMIDI;

*/
    // now store the pointer to the correct
    // function for this control
    //-------------------------------------
    switch(lpczmrLoad->uCtrlType)
    {
		case CTRL_TYPE_CHANNEL_NAME_DISPLAY_VERTICAL:
			lpctrlzm[iZonesCount].CtrlFunc = ChannelNameTextVertical;
			break;
		case CTRL_TYPE_CHANNEL_NUMBER_DISPLAY_VERTICAL:
			lpctrlzm[iZonesCount].CtrlFunc = ChannelNumberTextVertical;
			break;
        case CTRL_TYPE_STRING_UPDATE:
          lpctrlzm[iZonesCount].CtrlFunc = RdOutText;
          break;
        case CTRL_TYPE_OPEN_EXPLODE:
          lpctrlzm[iZonesCount].CtrlFunc = OpenExplodeWindow;
          break;
        case CTRL_TYPE_BUTTON_REG:
				case CTRL_TYPE_INPUT_GATE_IN_BTN_FILTER:
        case CTRL_TYPE_BTN_INPUTMUTE_FILTER:
				case CTRL_INPUT_AUX16B_MUTE_FILTER:	// LINE B TO AUX 16
        case CTRL_TYPE_BTN_AUXMUTE_FILTER:
        case CTRL_MASTER_MUTE_FILTER:
        case CTRL_MASTER_AUX_MUTE_FILTER:
        case CTRL_MARIXLT_MUTE_FILTER:
        case CTRL_MARIXRT_MUTE_FILTER:
        case CTRL_TYPE_BTN_INPUT_AUXPOST_FILTER:
        case CTRL_TYPE_BTN_MASTER_AUX_PREPOST_FILTER:
        case CTRL_TYPE_BTN_MASTER_HEADPOST_FILTER:
        case CTRL_TYPE_BTN_AUX_MMATRIX_LT_POST_FILTER:
        case CTRL_TYPE_BTN_AUX_MMATRIX_RT_POST_FILTER:
        case CTRL_INPUT_MICA_MUTE_FILTER:
        case CTRL_INPUT_LINEA_MUTE_FILTER:
        case CTRL_INPUT_MICB_MUTE_FILTER:
        case CTRL_INPUT_LINEB_MUTE_FILTER:
        case CTRL_MATRIX_SUB_MUTE_FILTER:
        case CTRL_MATRIX_AUX_MUTE_FILTER:
				case CTRL_SUB_SUMIN_MUTE_FILTER:
        case CTRL_MATRIX_MASTER_LT_MUTE_FILTER:
        case CTRL_MATRIX_MASTER_RT_MUTE_FILTER:
        case CTRL_MATRIX_MONO_MUTE_FILTER:
        case CTRL_MATRIX_CENTER_MUTE_FILTER:
        case CTRL_CUE_MASTER_MUTE_FILTER:
				case CTRL_TYPE_BTN_EQ_RESET_FILTER:
          lpctrlzm[iZonesCount].CtrlFunc = PushBtn;
          break;
        case CTRL_TYPE_FADER_VERT:
          lpctrlzm[iZonesCount].CtrlFunc = DrawVertFader;
          break;
        case CTRL_TYPE_FADER_HORZ:
          lpctrlzm[iZonesCount].CtrlFunc = DrawHorizFader;
          break;
        case CTRL_TYPE_OPEN_ZOOM_IAUX:
				case		CTRL_TYPE_OPEN_ZOOM_CCOMP:
				case		CTRL_TYPE_OPEN_ZOOM_EQ:
				case		CTRL_TYPE_OPEN_ZOOM_SUB:
				case		CTRL_TYPE_OPEN_FADER:
				case		CTRL_TYPE_OPEN_MATRIX_EQ:
				case		CTRL_TYPE_OPEN_MATRIX_AUX:
				case		CTRL_TYPE_OPEN_MATRIX_SUB:
          lpctrlzm[iZonesCount].CtrlFunc = JumpToMZWindow;
          break;
        case CTRL_TYPE_UPDOWN:
          lpctrlzm[iZonesCount].CtrlFunc = UpDownControl;
          break;
        case CTRL_TYPE_LEFTRIGHT:
          lpctrlzm[iZonesCount].CtrlFunc = LeftRightControl;
          break;
        case CTRL_TYPE_UPDOWNSCROLL:
          lpctrlzm[iZonesCount].CtrlFunc = UpDownControl;
          break;
        case CTRL_TYPE_LABEL:
          lpctrlzm[iZonesCount].CtrlFunc = DrawChannelLabel;
          break;
        case CTRL_TYPE_VU_DISPLAY: // ALL VU readouts go through here
        case CTRL_TYPE_VU_DISPLAY1:
        case CTRL_TYPE_VU_DISPLAY2:
        case CTRL_TYPE_VU_DISPLAY3:
        case CTRL_TYPE_VU_DISPLAY4:
        case CTRL_TYPE_VU_DISPLAY5:
        case CTRL_TYPE_VU_DISPLAY6:
        case CTRL_TYPE_VU_DISPLAY7:

        case CTRL_TYPE_VU_DISPLAY8:
        case CTRL_TYPE_VU_DISPLAY9:
        case CTRL_TYPE_VU_DISPLAY10:
        case CTRL_TYPE_VU_DISPLAY11:
/*
        case CTRL_TYPE_VU_DISPLAY12:
        case CTRL_TYPE_VU_DISPLAY13:
        case CTRL_TYPE_VU_DISPLAY14:
        case CTRL_TYPE_VU_DISPLAY15:
*/
				case CTRL_TYPE_CLIP_LIGHT:
          lpctrlzm[iZonesCount].CtrlFunc = ZoneMapNullFunction;//DrawVUData;
          break;
        case CTRL_TYPE_DISP_INPUT_EQ_FILTER:
        case CTRL_TYPE_DISP_SUB_EQ_FILTER:
          lpctrlzm[iZonesCount].CtrlFunc = DrawEqGraphHook;
          break;

        default:
            return  IDS_ERR_INVALID_CONTROL;
        }

    lpczmrLoad ++; // Go to the next control
    iZonesCount ++;
    }

// Now add the Terminator Zone
// which will be set to CTRL_ZONE_TERMINATE
//-----------------------------------------
ZeroMemory(&lpctrlzm[iZonesCount].rZone, sizeof(RECT));
ZeroMemory(&lpctrlzm[iZonesCount].rLastDraw,sizeof(RECT));
lpctrlzm[iZonesCount].iCtrlNumAbs = CTRL_ZONE_TERMINATE;
lpctrlzm[iZonesCount].iCtrlChanPos = CTRL_NUM_NULL;
lpctrlzm[iZonesCount].iCtrlType = CTRL_ZONE_TERMINATE;
lpctrlzm[iZonesCount].iDispType = CTRL_ZONE_TERMINATE;
lpctrlzm[iZonesCount].iModuleNumber = 0xFFFFFFFF; // Set this to Invalid module number    

lpctrlzm[iZonesCount].iCtrlBmp[0] = CTRL_ZONE_TERMINATE; // Set them all to nothing
lpctrlzm[iZonesCount].iCtrlBmp[1] = CTRL_ZONE_TERMINATE; // Set them all to nothing
lpctrlzm[iZonesCount].iCtrlBmp[2] = CTRL_ZONE_TERMINATE; // Set them all to nothing

lpctrlzm[iZonesCount].CtrlFunc = NULL;


// Done with the Zone map
//-----------------------
lpzm->lpZoneMap = lpctrlzm;
lpzm->iZonesCount = iZonesCount; // the number of the active allocated Zones

return 0;
}



//====================================
// FUNCTION: FreeZoneMapsMemory
//
// Clean up function for the Zone maps
//====================================
void    FreeZoneMapsMemory(void)
{
  int     iCount;


  // if the memory was allocated
  // then go ahead and free all of its components
  //---------------------------------------------
  if(gpZoneMaps_Full)
      {
      for(iCount = 0; iCount < MAX_CHANNELS; iCount++)
          {
          if(gpZoneMaps_Full[iCount].lpZoneMap)
              {
              GlobalFree((HGLOBAL)gpZoneMaps_Full[iCount].lpZoneMap);
              }
          // Delete the Bitmap
          //------------------
          DeleteBmpResGlobalIndx(gpZoneMaps_Full[iCount].iBmpIndx);
          }
      GlobalFree(gpZoneMaps_Full);
      }
  //glZonesCount_Full = 0; // We can use this as a flag somewhere else


  if(ghZoneMapDesc_Full)
      {
      UnlockResource(ghZoneMapDesc_Full);
      FreeResource(ghZoneMapDesc_Full);
      }


  // HERE starts the ZOOM View ZoneMap Unloading
  //--------------------------------------------
  //--------------------------------------------

  // if the memory was allocated
  // then go ahead and free all of its components
  //---------------------------------------------
  if(gpZoneMaps_Zoom)
      {
      for(iCount = 0; iCount < MAX_CHANNELS; iCount++)
          {
          if(gpZoneMaps_Zoom[iCount].lpZoneMap)
              {
              GlobalFree((HGLOBAL)gpZoneMaps_Zoom[iCount].lpZoneMap);
              }
          // Delete the Bitmap
          //------------------
          DeleteBmpResGlobalIndx(gpZoneMaps_Zoom[iCount].iBmpIndx);
          }
      GlobalFree(gpZoneMaps_Zoom);
      }
  //glZonesCount_Zoom = 0; // We can use this as a flag somewhere else


  if(ghZoneMapDesc_Zoom)
      {
      UnlockResource(ghZoneMapDesc_Zoom);
      FreeResource(ghZoneMapDesc_Zoom);
      }

  // Free the labels memory
  //-----------------------
  GlobalFree(gpLabels);
  gpLabels = NULL;


  if(g_pAuxLabels)
    GlobalFree(g_pAuxLabels);
  g_pAuxLabels = NULL;

  if(g_pMatrixLabels )
    GlobalFree(g_pMatrixLabels);
  g_pMatrixLabels = NULL;

  return;
}

