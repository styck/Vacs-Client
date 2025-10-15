//=================================================
// file ConsoleCommunication.c
//
// Copyright 1998, CorTek Software, Inc.
//=================================================

#include "SAMM.h"
#include "SAMMEXT.h"
#include "ConsoleDefinition.h"
#include "DCXTCP_protocol.h" 

/////////////////////////////////////////////////////////////////////////
// FUNCTION: DefinitionCallback
//
//
//
void DefinitionCallback(UINT uiMessage, UINT ui2, VOID * vp1, VOID *vp2)
{
	char szStr[128];
	MMTIME *pmm;

	if(WaitForSingleObject(gDisplayEvent, 10) == WAIT_OBJECT_0)
	{
		switch( uiMessage )
		{
		case DCX_VU_DATA:
			// Display the VU Data if there is something available
			//
			if(ui2 > 0)
				DisplayVU_Data(vp1, ui2);
			break;

		case DCX_TCP_CONTROL_DATA:
			// Somebody else on the network adjusted a control
			// so lets show it on screen
			//
			if(ui2 == sizeof(CONTROLDATA))
				UpdateControlFromNetwork(((LPCONTROLDATA)vp1)->wChannel, ((LPCONTROLDATA)vp1)->wCtrl,
				((LPCONTROLDATA)vp1)->wVal, TRUE);
			break;

		case CDEF_SHOW_TIME:
			if( ghwndStatus )
			{
				pmm = (MMTIME *)vp1;
				wsprintf(szStr, "%02d:%02d:%02d:%02d", (int)pmm->u.smpte.hour, (int)pmm->u.smpte.min, 
					(int)pmm->u.smpte.sec, (int)pmm->u.smpte.frame);
				SendMessage(ghwndStatus, SB_SETTEXT, MAKEWPARAM(1,SBT_POPOUT), (LPARAM)szStr);
			}
			break;

		}
		SetEvent(gDisplayEvent);
	}else
	{
		//
		//  Handle Error codes here ... if timed-out the data will be thrown-out ... !!!!
		//
		return;
	}

	return;
}