//=================================================
// Copyright 1998-2002, CorTek Software, Inc.
//=================================================

//=================================================
// Controls Zones Scanning routines
//
// 
//=================================================

#include "SAMM.h"
#include "SAMMEXT.h"
#include "MACRO.h"

//==================================================
//FUNCTION: ScanCtrlZonesPnt
//
//
// return:
//      a pointer to the Entry that matches
//      the given criteria
//      NULL if nothing was found
//
//prpose:
//      Scans the Control Zones using a given point
//      Here we are going to use the rZone
//
//==================================================
LPCTRLZONEMAP     ScanCtrlZonesPnt(LPCTRLZONEMAP lpczmArr, POINT pnt)
{
LPCTRLZONEMAP    lpczmPtr;
RECT                rect;

	lpczmPtr = lpczmArr;
	rect = lpczmPtr->rZone;

	// Search until the end of the list
	//---------------------------------
	while(rect.right)
  {
    if(PtInRect(&rect, pnt))
    {
        return lpczmPtr;
    }
    lpczmPtr++;
    rect = lpczmPtr->rZone;
  }

return NULL;
}

//==================================================
//FUNCTION: ScanCtrlZonesType
//
//
// return:
//      a pointer to the Entry that matches
//      the given criteria
//      NULL if nothing was found
//
//prpose:
//      Scans the Control Zones using a given type
//      Here we are going to use the iCtrlType
//==================================================
LPCTRLZONEMAP      ScanCtrlZonesType(LPCTRLZONEMAP lpczmArr, int iType)
{
LPCTRLZONEMAP    lpczmPtr;

	lpczmPtr = lpczmArr;

	// Search until the end of the list
	//---------------------------------
	while(lpczmPtr->rZone.right)
  {
    if(lpczmPtr->iCtrlType == iType)
    {
        return lpczmPtr;
    }
    lpczmPtr++;
  }

return NULL;
}

//==================================================
//FUNCTION: ScanCtrlZonesNum
//
//
// return:
//      a pointer to the Entry that matches
//      the given criteria
//      NULL if nothing was found
//
//prpose:
//      Scans the Control Zones using a given Num
//      Here we are going to use the iCtrlNum
//==================================================
LPCTRLZONEMAP      ScanCtrlZonesNum(LPCTRLZONEMAP lpczmArr, int iNum)
{
  LPCTRLZONEMAP    lpczmPtr;

  lpczmPtr = lpczmArr;

  if(lpczmPtr == NULL)
    return NULL;
  
  // Search until the end of the list
  //---------------------------------
  while(lpczmPtr->rZone.right)
  {
    if(lpczmPtr->iCtrlChanPos == iNum)
    {
      return lpczmPtr;
    }
    lpczmPtr++;
  }

  return NULL;
}

//===================================================
//FUNCTION: ScanCtrlZonesDisp
//
//
// return:
//      a pointer to the Entry that matches
//      the given criteria
//      NULL if nothing was found
//
//prpose:
//      Scans the Control Zones using a given display
//      Here we are going to use the iCtrlDisp
//===================================================
LPCTRLZONEMAP      ScanCtrlZonesDisp(LPCTRLZONEMAP lpczmArr, int iDisp)
{
	LPCTRLZONEMAP    lpczmPtr;

	lpczmPtr = lpczmArr;

	// Search until the end of the list
	//---------------------------------
	while(lpczmPtr->rZone.right)
  {
    if(lpczmPtr->iDispType == iDisp)
    {
        return lpczmPtr;
    }
    lpczmPtr++;
  }

return NULL;
}

//===================================================
//FUNCTION: ScanCtrlZonesAbs
//
//
// return:
//      a pointer to the Entry that matches
//      the given criteria
//      NULL if nothing was found
//
//prpose:
//      Scans the Control Zones using a given iCtrlNumAbs
//      Here we are going to use the iCtrlDisp
//===================================================
LPCTRLZONEMAP      ScanCtrlZonesAbs(LPCTRLZONEMAP lpczmArr, int iCtrlNumAbs)
{
  LPCTRLZONEMAP    lpczmPtr;


  if(lpczmArr == NULL)
    return NULL;
  lpczmPtr = lpczmArr;

  // Search until the end of the list
  //---------------------------------
  while(lpczmPtr->rZone.right)
  {
    if(lpczmPtr->iCtrlNumAbs == iCtrlNumAbs)
    {
       return lpczmPtr;
    }
    lpczmPtr++;
  }

  return NULL;
}

