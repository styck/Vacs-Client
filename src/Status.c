
//========================================================
//Stautd bar Functions
//
//
//========================================================            
//#include <windows.h>
//#include <commctrl.h>

#include "SAMM.h"
#include "SAMMEXT.h"
#include "MACRO.h"

// Variables
//----------
WNDPROC     WndProc;
HWND        ghwndStatPBar;



//=========================================
//
//
//
//
//=========================================
int    CreateMainStatusWindow(HWND hwndParent)
{
  int     iarrParts[3];
  RECT    rCl;

  ghwndStatus = CreateWindow(
	  STATUSCLASSNAME,           
	  "", 
	  WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
	  0,0,0,0,
	  //-100,-100,10,10,
	  hwndParent,               
	  (HMENU)100,               
	  ghInstMain,
	  NULL);

  if(ghwndStatus == NULL)
      return IDS_ERR_CREATE_WINDOW;

  // Subclass

  iarrParts[0] = 270;
  iarrParts[1] = 370;
  iarrParts[2] = -1;
  SendMessage(ghwndStatus, SB_SETPARTS, 3, (LPARAM)iarrParts);
  //SendMessage(ghwndStatus, SB_SETTEXT, MAKEWPARAM(0,SBT_POPOUT), (LPARAM)"Status Window Created");

  GetClientRect(ghwndStatus, &rCl);

  ghwndStatPBar = CreateWindowEx(WS_EX_DLGMODALFRAME, PROGRESS_CLASS, (LPSTR) NULL, 
                                WS_CHILD | WS_VISIBLE | PBS_SMOOTH, 
                                272, 
                                2, 
                                98, 
                                rCl.bottom-2, 
                                ghwndStatus, 
                                (HMENU) 0, 
                                ghInstMain, 
                                NULL); 


  SetProgressRange(0, 100);
  SetProgressStep(1);

  ShowWindow(ghwndStatPBar, SW_SHOW);

  return 0;
};



///////////////////////////////////////////
void  SetProgressRange(int iLow, int iHigh)
{
  SendMessage(ghwndStatPBar, PBM_SETRANGE, 0, MAKELPARAM(iLow, iHigh));
}
///////////////////////////////////////////
void  SetProgressStep(int iStep)
{
  SendMessage(ghwndStatPBar, PBM_SETSTEP, (WPARAM) iStep, 0);
}
///////////////////////////////////////////
void  ClearProgress(void)
{
  SendMessage(ghwndStatPBar, PBM_SETPOS, (WPARAM)0, 0);
}
///////////////////////////////////////////
void  SetProgressPosition(int iPos)
{
  SendMessage(ghwndStatPBar, PBM_SETPOS, (WPARAM)iPos, 0);
}
///////////////////////////////////////////
void  StepProgress(int iStep)
{
  SendMessage(ghwndStatPBar, PBM_STEPIT, (WPARAM) iStep, 0);
}
