//=================================================
// Copyright 1998-2002 CorTek Software, Inc.
//=================================================
//
//
// $Author:: Styck                                $
// $Archive:: /Vacs Client/src/Status.c           $
// $Revision:: 5                                  $
//

//========================================================
// Status bar Functions
//
// Creates the Status bar and handles the messages
//
//========================================================            

#include "SAMM.h"
#include "SAMMEXT.h"
#include "MACRO.h"

#include <stdio.h> // JUST FOR SPRINTF() BELOW IN DEBUGGING


LRESULT CALLBACK  StatusBarProc(HWND , UINT , WPARAM , LPARAM);
void DrawFrame(HDC *, RECT , int );

// Variables
//----------
WNDPROC     WndProc;
HWND        ghwndStatPBar;
HWND				ghwndStatShiftButton,ghwndStatLeftButton,ghwndStatRightButton;
HWND				ghwndStatF1Button, ghwndStatF2Button, ghwndStatF3Button, ghwndStatF4Button;


//=========================================
//  Create the status bar and the progress
//  bar and buttons that are needed
//
//=========================================


int    CreateMainStatusWindow(HWND hwndParent)
{
	////////////////////////////////////////////////////
	// Create the PROGRESS BAR on the status window
	// Used when resetting the mixer
	// Also create the F1-4 and SHIFT buttons
	//
  // Break the status bar into 17 parts

#define NUMBER_OF_PARTS	17	// number of parts on status bar

  int     iarrParts[NUMBER_OF_PARTS];
  RECT    rCl;

	/////////////////////////////////////
	// Create the actual status bar

  ghwndStatus = CreateWindow(
	  STATUSCLASSNAME,           
	  "", 
	  WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
	  0,0,0,0,
	  //-100,-100,10,10,
	  hwndParent,               
	  (HMENU)100,		// Window ID
	  ghInstMain,
	  NULL);

  if(ghwndStatus == NULL)
      return IDS_ERR_CREATE_WINDOW;

#define BORDER_SIZE			2

#define PROGRESS_START	150	// -100
#define PROGRESS_END		250

#define SHIFT_START		350
#define SHIFT_END			450

#define	LEFT_START	470
#define	LEFT_END		530

#define	RIGHT_START	540
#define	RIGHT_END		600

#define F1_START		650
#define F1_END			700

#define F2_START		750
#define F2_END			800

#define F3_START		850
#define F3_END			900

#define F4_START		950
#define F4_END			1000


  iarrParts[0] = PROGRESS_START;		// Frist part 
  iarrParts[1] = PROGRESS_END;		// Second part
	iarrParts[2] = SHIFT_START;
	iarrParts[3] = SHIFT_END;
	iarrParts[4] = LEFT_START;
	iarrParts[5] = LEFT_END;
	iarrParts[6] = RIGHT_START;
	iarrParts[7] = RIGHT_END;
	iarrParts[8] = F1_START;
	iarrParts[9] = F1_END;
	iarrParts[10] = F2_START;
	iarrParts[11] = F2_END;
	iarrParts[12] = F3_START;
	iarrParts[13] = F3_END;
	iarrParts[14] = F4_START;
	iarrParts[15] = F4_END;

  iarrParts[16] = -1;		// end

  SendMessage(ghwndStatus, SB_SETPARTS, NUMBER_OF_PARTS, (LPARAM)iarrParts);

  GetClientRect(ghwndStatus, &rCl);

  ghwndStatPBar = CreateWindowEx(WS_EX_DLGMODALFRAME, PROGRESS_CLASS, (LPSTR) NULL, 
                                WS_CHILD | WS_VISIBLE | PBS_SMOOTH, 
                                PROGRESS_START+BORDER_SIZE,						// x 
                                2,													// y
                                PROGRESS_END-PROGRESS_START - BORDER_SIZE,		// width 98
                                rCl.bottom-BORDER_SIZE,		// height
                                ghwndStatus, 
                                (HMENU) 0, 
                                ghInstMain, 
                                NULL); 

  ShowWindow(ghwndStatPBar, SW_SHOW);

  SetProgressRange(0, 100);
  SetProgressStep(1);

	//////////////////////////////////////////////////
	// Create the SHIFT button
	// as a Check box with Pushlike style

	ghwndStatShiftButton = CreateWindowEx(0L,"BUTTON", "SHIFT", 
                                WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE , 
                                SHIFT_START+BORDER_SIZE,						// x 
                                4,													// y
                                SHIFT_END-SHIFT_START - BORDER_SIZE,		// width 98
                                rCl.bottom-BORDER_SIZE - 2,		// height
                                ghwndStatus, 
                                (HMENU) VK_SHIFT,		// Post this message to the process
                                ghInstMain, 
                                NULL); 

  ShowWindow(ghwndStatShiftButton, SW_SHOW);


	//////////////////////////////////////////////////
	// Create the LEFT button

	ghwndStatLeftButton = CreateWindowEx(0, "BUTTON", "LEFT", 
                                WS_CHILD | BS_CENTER | BS_VCENTER | BS_PUSHBUTTON | BS_PUSHLIKE, 
                                LEFT_START+BORDER_SIZE,						// x 
                                2,													// y
                                LEFT_END-LEFT_START - BORDER_SIZE,		// width 98
                                rCl.bottom-BORDER_SIZE,		// height
                                ghwndStatus, 
                                (HMENU) VK_LEFT, 
                                ghInstMain, 
                                NULL); 

  ShowWindow(ghwndStatLeftButton, SW_SHOW);


	//////////////////////////////////////////////////
	// Create the RIGHT button

	ghwndStatRightButton = CreateWindowEx(0, "BUTTON", "RIGHT", 
                                WS_CHILD | BS_CENTER | BS_VCENTER | BS_PUSHBUTTON | BS_PUSHLIKE, 
                                RIGHT_START+BORDER_SIZE,						// x 
                                2,													// y
                                RIGHT_END-RIGHT_START - BORDER_SIZE,		// width 98
                                rCl.bottom-BORDER_SIZE,		// height
                                ghwndStatus, 
                                (HMENU) VK_RIGHT, 
                                ghInstMain, 
                                NULL); 

  ShowWindow(ghwndStatRightButton, SW_SHOW);




	//////////////////////////////////////////////////
	// Create the F1 button

	ghwndStatF1Button = CreateWindow("BUTTON", "F1", 
                                WS_CHILD | BS_CENTER | BS_VCENTER | BS_PUSHBUTTON | BS_PUSHLIKE, 
                                F1_START+BORDER_SIZE,						// x 
                                2,													// y
                                F1_END-F1_START - BORDER_SIZE,		// width 98
                                rCl.bottom-BORDER_SIZE,		// height
                                ghwndStatus, 
                                (HMENU)VK_F1, 
                                ghInstMain, 
                                NULL); 

  ShowWindow(ghwndStatF1Button, SW_SHOW);

	//////////////////////////////////////////////////
	// Create the F2 button

	ghwndStatF2Button = CreateWindow("BUTTON", "F2", 
                                WS_CHILD | BS_CENTER | BS_VCENTER | BS_PUSHBUTTON | BS_PUSHLIKE, 
                                F2_START+BORDER_SIZE,						// x 
                                2,													// y
                                F2_END-F2_START - BORDER_SIZE,		// width 98
                                rCl.bottom-BORDER_SIZE,		// height
                                ghwndStatus, 
                                (HMENU)VK_F2, 
                                ghInstMain, 
                                NULL); 

  ShowWindow(ghwndStatF2Button, SW_SHOW);


	//////////////////////////////////////////////////
	// Create the F3 button

	ghwndStatF3Button = CreateWindow("BUTTON", "F3", 
                                WS_CHILD | BS_CENTER | BS_VCENTER | BS_PUSHBUTTON | BS_PUSHLIKE, 
                                F3_START+BORDER_SIZE,						// x 
                                2,													// y
                                F3_END-F3_START - BORDER_SIZE,		// width 98
                                rCl.bottom-BORDER_SIZE,		// height
                                ghwndStatus, 
                                (HMENU)VK_F3, 
                                ghInstMain, 
                                NULL); 

  ShowWindow(ghwndStatF3Button, SW_SHOW);


	//////////////////////////////////////////////////
	// Create the F4 button

	ghwndStatF4Button = CreateWindow("BUTTON", "F4", 
                                WS_CHILD | BS_CENTER | BS_VCENTER | BS_PUSHBUTTON | BS_PUSHLIKE, 
                                F4_START+BORDER_SIZE,						// x 
                                2,													// y
                                F4_END-F4_START - BORDER_SIZE,		// width 98
                                rCl.bottom-BORDER_SIZE,		// height
                                ghwndStatus, 
                                (HMENU)VK_F4, 
                                ghInstMain, 
                                NULL); 

  ShowWindow(ghwndStatF4Button, SW_SHOW);




	//////////////////////////////////////////////////////////
	// Subclass the status bar so we can get messages

	WndProc = (WNDPROC)SetWindowLong(ghwndStatus, GWL_WNDPROC, (LONG)StatusBarProc);

  return 0;
};




//====================================================
// MDI Client Window Procedure For Processing Messages
//====================================================
LRESULT CALLBACK   StatusBarProc(HWND hWnd, UINT wMessage,
                               WPARAM wParam, LPARAM  lParam)
{
	HWND hWndZoom;
	int nCheck;

	switch (wMessage)
	{
		case WM_COMMAND:

			switch(LOWORD(wParam))
			{
				///////////////////////////////////////
				// The WinMain() and WndMainProc() both
				// handle this message, one for keyboard
				// input and one for the screen.
				//
				// Need to make it the same!!!
				//
				// ghwndMain is handled by WndMainProc()

				case VK_F1:
					SendMessage(ghwndMain,WM_KEYDOWN,(WPARAM)VK_F1, (LPARAM)NULL);
				break;
				case VK_F2:
					SendMessage(ghwndMain,WM_KEYDOWN,(WPARAM)VK_F2, (LPARAM)NULL);
				break;
				case VK_F3:
					SendMessage(ghwndMain,WM_KEYDOWN,(WPARAM)VK_F3, (LPARAM)NULL);
				break;
				case VK_F4:
					SendMessage(ghwndMain,WM_KEYDOWN,(WPARAM)VK_F4, (LPARAM)NULL);
				break;

				/////////////////////////////////////
				// Must find the topmost ZOOM window
				// and restore the FOCUS back onto 
				// that ZOOM window else the arrow
				// Keys will not work
				//
				// DOESN'T WORK FOR FULL WINDOW, IS THAT A PROBLEM?

				case VK_LEFT:
					hWndZoom=FindWindowEx(ghwndMDIClient,NULL,gszZoomViewClass,"Zoom View");
					if(hWndZoom)
					{
						SendMessage(hWndZoom,WM_KEYUP,(WPARAM)VK_LEFT, (LPARAM)NULL);
					}
					break;

				case VK_RIGHT:
					hWndZoom=FindWindowEx(ghwndMDIClient,NULL,gszZoomViewClass,"Zoom View");
					if(hWndZoom)
					{
						SendMessage(hWndZoom,WM_KEYUP,(WPARAM)VK_RIGHT, (LPARAM)NULL);
					}
					break;

				////////////////////////////////////////////////////////
				// Handle the on screen SHIFT key
				// Need to send messages to main and zoom windows since
				// one handles the arrow keys and the other handles the
				// Function keys (FIX LATER)

				case VK_SHIFT:

					hWndZoom=FindWindowEx(ghwndMDIClient,NULL,gszZoomViewClass,"Zoom View");

							nCheck = SendMessage(ghwndStatShiftButton, BM_GETCHECK,1,0L);
							if(nCheck)
							{
								SendMessage(ghwndMain,WM_KEYDOWN,(WPARAM)VK_SHIFT, (LPARAM)NULL);
								SendMessage(hWndZoom,WM_KEYDOWN,(WPARAM)VK_SHIFT, (LPARAM)NULL);
							}
							else
							{
								SendMessage(ghwndMain,WM_KEYUP,(WPARAM)VK_SHIFT, (LPARAM)NULL);
								SendMessage(hWndZoom,WM_KEYUP,(WPARAM)VK_SHIFT, (LPARAM)NULL);
							}
				break;

			}

			if(hWndZoom)
				SetFocus(hWndZoom);	// Need to return focus to the main window or keys won't work

			return 0;

			default:
					 return CallWindowProc(WndProc, hWnd, wMessage, wParam, lParam);
	}

}



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
