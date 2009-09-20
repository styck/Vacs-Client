//=================================================
// Copyright 1998 - 2002, CorTek Softawre, Inc.
//=================================================
//
//
// $Author:: Styck                                $
// $Archive:: /Vacs Client/src/Main.c             $
// $Revision:: 50                                 $
//


#include "SAMM.H"
#include "SAMMEXT.H"
#include "MACRO.h"
#include "ConsoleDefinition.h"
#include "..\\commport\\commport.h"


extern HWND			g_stereoCueMetersWindow;

LPSTR		GetListItemGroupName(int iItem);	// SEE GROUPS.C

BOOL		OpenSequenceFiles (LPSTR  lpstrFName);

BOOL    g_bReversDirection = FALSE;		//  Controls Stereo-linked control panning
int			FindConsecutiveGroupIndex(int iNum, int iType);

void		CancelAllCues (HWND);

// Prototypes for functions in this file

LPSTR GetLastMixFileName(void);	
void ExtractFileName(char * , char * );


BOOL		g_bIsSoloCueMode;	// Default as true
BOOL		g_bUpDownScollSpeedFAST = TRUE;	// Flag for fast/slow updown scroll speed for touchscreen

static WORD	wKeyFlags=0;				// LOCAL Shift Key Flag

//======================
// Windows Main Routine
//
//
//
//======================

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR  lpszCmdLine, int nCmdShow)

{
MSG         msg;
int         iInit;
int         iMessage;
FILESTRUCT	fsTemp;
int					shift_key;	 
int			iRet;
HANDLE	hAccel;
char            szBuff[MAX_PATH];

	ghInstMain = hInstance;

	hAccel = LoadAccelerators(hInstance,"MdiAccel");

	// 
	//dwProcessID = GetCurrentProcessId();
	//hProcess = OpenProcess(PROCESS_SET_INFORMATION, FALSE, dwProcessID);
	//SetPriorityClass(hProcess, REALTIME_PRIORITY_CLASS);


	//////////////////////////////////////////
	// Make Sure that the Common Controls DLL
	// is loaded
	//----------------------------------------
	InitCommonControls();


	///////////////////////////////
	// Call Initialization Routine
	//----------------------------
	iInit = InitializeProc();

	//ShowSplashScreen(FALSE);

	/////////////////////////////////////
	// Check if everything Initialized OK

	if(iInit != 0)
  {
    if(ghInstStrRes == NULL)
        ErrorBox(NULL, ghInstMain, iInit);
    else
        ErrorBox(NULL, ghInstStrRes, iInit);
    ShutdownProc();
		return FALSE;
	}

	// turn the VUs
	InitVULookupTables(TRUE);
	CDef_StartVuData();


	//////////////////////////////////
	// If the SHIFT key is held down
	// during startup then we bypass
	// loading the mix file and just
	// create an default zoom window

	shift_key = GetKeyState (VK_SHIFT);

	if ( shift_key & 0x8000)
	{
		CreateZoomViewWindow(ghwndMDIClient,"Zoom View", NULL, DCX_DEVMAP_MODULE_INPUT);
	}
	else
	{

		CloseAllMDI();

		/////////////////////////////////////////////////////////////////////////
		// go to the right directory and Load the last mix file loaded or
		// the LA$T.mix file if one wasn't loaded before

		//  <== WRONG, MAY NOT BE DIRECTORY OF LAST MIX FILE LOADED ==>>
		wsprintf(fsTemp.szFileDir, "%smix\\", gszProgDir);	// <== WRONG, MAY NOT BE DIRECTORY OF LAST MIX FILE LOADED
    wsprintf(fsTemp.szFileName, "%s", GetLastMixFileName());


		iRet = LoadMixFile(&fsTemp, TRUE);			// ?? why not OpenMixFile() and do everything
		if( iRet != 0)
		{

			if(iRet !=2)
			{
				// Ok... LA$T.mix file did NOT load
				// so open a new Zoom window by default
				CreateZoomViewWindow(ghwndMDIClient,"Zoom View", NULL, DCX_DEVMAP_MODULE_INPUT);
				
				// If this isn't done then when they try to add a sequence
				// to the sequence window it will create an exception

				wsprintf(szBuff,"%smix\\%s",gszProgDir, fsTemp.szFileName);
	 			OpenSequenceFiles (szBuff);

				ShowSeqWindow(FALSE);
			}
			else	// Mix File did not match GServer settings, we MUST exit
			{
				// Just open as if SHIFT key was pressed
//				CreateZoomViewWindow("Zoom View", NULL, 1);

				ShutdownProc();
				return FALSE;
			}


		}

	}

			// As per Gamble 10/19/2001
			// Make sure we cancel all our cues when first loading
			// At this point LoadMix() has already been called

			CancelAllCues (ghwndMain);

	// Main Message Loop
	//------------------

	msg.message = 0;
	iMessage = 0;

	/////////////////////////////////////////////
	// Process Until WM_QUIT Message Is Received
	//------------------------------------------

	while((ghwndMain != NULL) && GetMessage(&msg,NULL, 0, 0) == TRUE)
  {

		iMessage = msg.message;

#define MOVEDTOWNDPROC		// THIS NEEDS TO BE HERE ALSO??? FIND OUT WHY AND FIX
#ifdef MOVEDTOWNDPROC

			//////////////
			if(msg.message == WM_KEYUP)
			{
				switch(msg.wParam)
				{
					case VK_SHIFT:
					wKeyFlags &= ~VK_SHIFT;
					break;
				}
			}

			// If keydown then lets check for F-keys
			// If shift is down also then save the window positions to 
			// a file fkeyX.dat where X is the function key number

			if(msg.message == WM_KEYDOWN)
			{
				wsprintf(fsTemp.szFileDir, "%smix\\", gszProgDir);
				iRet=0;	// Just in case it was not initialized


				switch(msg.wParam)
				{
					case VK_SHIFT:
					wKeyFlags |= VK_SHIFT;
					break;

					case VK_F1:
						wsprintf(fsTemp.szFileName, "fkey%d.dat",1 );
						if(wKeyFlags & VK_SHIFT)	// Save window if SHIFT
							WriteFkeyFile(&fsTemp, (int) VK_F1);
						else	// Restore window
							iRet=LoadFkeyFile(&fsTemp, (int) VK_F1);
					break;

					case VK_F2:
						wsprintf(fsTemp.szFileName, "fkey%d.dat",2 );
						if(wKeyFlags & VK_SHIFT)	// Save window if SHIFT
							WriteFkeyFile(&fsTemp, (int) VK_F2);
						else	// Restore window
							iRet=LoadFkeyFile(&fsTemp, (int) VK_F2);
					break;

					case VK_F3:
						wsprintf(fsTemp.szFileName, "fkey%d.dat",3 );
						if(wKeyFlags & VK_SHIFT)	// Save window if SHIFT
							WriteFkeyFile(&fsTemp, (int) VK_F3);
						else	// Restore window
							iRet=LoadFkeyFile(&fsTemp, (int) VK_F3);
					break;

					case VK_F4:
						wsprintf(fsTemp.szFileName, "fkey%d.dat",4 );
						if(wKeyFlags & VK_SHIFT)	// Save window if SHIFT
							WriteFkeyFile(&fsTemp, (int) VK_F4);
						else	// Restore window
							iRet=LoadFkeyFile(&fsTemp, (int) VK_F4);
					break;

					case VK_F5:
						wsprintf(fsTemp.szFileName, "fkey%d.dat",5 );
						if(wKeyFlags & VK_SHIFT)	// Save window if SHIFT
							WriteFkeyFile(&fsTemp, (int) VK_F5);
						else	// Restore window
							iRet=LoadFkeyFile(&fsTemp, (int) VK_F5);

					break;

					case VK_F6:
						wsprintf(fsTemp.szFileName, "fkey%d.dat",6 );
						if(wKeyFlags & VK_SHIFT)	// Save window if SHIFT
							WriteFkeyFile(&fsTemp, (int) VK_F6);
						else	// Restore window
							iRet=LoadFkeyFile(&fsTemp, (int) VK_F6);
					break;

					case VK_F7:
						wsprintf(fsTemp.szFileName, "fkey%d.dat",7 );
						if(wKeyFlags & VK_SHIFT)	// Save window if SHIFT
							WriteFkeyFile(&fsTemp, (int) VK_F7);
						else	// Restore window
							iRet=LoadFkeyFile(&fsTemp, (int) VK_F4);
					break;

					case VK_F8:
						wsprintf(fsTemp.szFileName, "fkey%d.dat",8 );
						if(wKeyFlags & VK_SHIFT)	// Save window if SHIFT
							WriteFkeyFile(&fsTemp, (int) VK_F8);
						else	// Restore window
							iRet=LoadFkeyFile(&fsTemp, (int) VK_F8);
					break;

					case VK_F9:
						wsprintf(fsTemp.szFileName, "fkey%d.dat",9 );
						if(wKeyFlags & VK_SHIFT)	// Save window if SHIFT
							WriteFkeyFile(&fsTemp, (int) VK_F9);
						else	// Restore window
							iRet=LoadFkeyFile(&fsTemp, (int) VK_F9);
					break;
#ifdef NOT_USEDSYSTEMKEY
					case VK_F10:
						wsprintf(fsTemp.szFileName, "fkey%d.dat",10 );
						if(wKeyFlags & VK_SHIFT)	// Save window if SHIFT
							WriteFkeyFile(&fsTemp, (int) VK_F10);
						else	// Restore window
							iRet=LoadFkeyFile(&fsTemp, (int) VK_F10);
					break;
#endif

				}
				
				////////////////////////////////////////////////////////////////////
				// Trying to load an FKEY file that is incompatible with the RACK setting
				// Shutdown and EXIT
				if(iRet==2)	
				{
					ShutdownProc();
					return FALSE;
				}

			}


			/////////////////////////////////////////////
			// Sets flag for the stereo-linked panning
			// when ALT key is held down
			
			if(msg.message == WM_SYSKEYDOWN)
			{
				g_bReversDirection = TRUE; 	//  Turn ON Stereo-linked control panning
			}
			if(msg.message == WM_SYSKEYUP)
			{
				g_bReversDirection = FALSE; //  Turn OFF Stereo-linked control panning
			}

#endif		// MOVEDTOWNDPROC

			if(!TranslateMDISysAccel(ghwndMDIClient, &msg) &&
				 !TranslateAccelerator(ghwndMain, hAccel, &msg) )
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

	}			

    // Perform Background Processing Functions
    //----------------------------------------
//        CheckMidiStatus();
//		}

//fds  }


	// WM_QUIT Message Received
	//-------------------------
	ShutdownProc();


	// Return Application Exit Code
	//-----------------------------
	return msg.wParam;

}



//===============================================
// Main Window Procedure For Processing Messages
//
//
//
//
//
//===============================================

LRESULT CALLBACK  WndMainProc(HWND hWnd, UINT wMessage,
                             WPARAM wParam, LPARAM  lParam)
{
  HDC					hDC;
	LPNMHDR			pnotify;
	FILESTRUCT	fsTemp;
	HWND				focus_wnd;
	HWND				last_wnd=-1;
	static UINT				timerID;

  int     iRet;
	BOOL bResult;
	
	// Registry variables 

	LONG lnResult;
	HKEY hKey = NULL;
	DWORD dwDisposition;
	static LPCTSTR szRegKey = "Software\\CorTek\\VACS"; 
	static LPCTSTR szRegValue; 
	DWORD dwType;
	DWORD rc;
	DWORD dwBufferSize;
	char	szTempSeq[MAX_PATH];
	char  szBuff[MAX_PATH];
	char  szScratch[MAX_PATH];

	LPMIXERWNDDATA      lpmwd;


  switch (wMessage)
	{

		case WM_KEYUP:

			g_bReversDirection = FALSE;	//  Controls Stereo-linked control panning 

			switch(LOWORD(wParam))
			{
					case VK_SHIFT:
					wKeyFlags &= ~VK_SHIFT;
					break;

			}
			break;

		case WM_KEYDOWN:		// this will get the SCREEN BUTTONS

			wsprintf(fsTemp.szFileDir, "%smix\\", gszProgDir);
			iRet=0;	// Just in case it was not initialized

			switch(LOWORD(wParam))
			{

						case VK_SHIFT:
							wKeyFlags |= VK_SHIFT;
						break;

						///////////////////////////////////////////////////////////
						// Processes SCREEN buttons

						case VK_F1:
							wsprintf(fsTemp.szFileName, "fkey%d.dat",1 );
							if(wKeyFlags & VK_SHIFT)	// Save window if SHIFT
								WriteFkeyFile(&fsTemp, (int) VK_F1);
							else	// Restore window
								iRet=LoadFkeyFile(&fsTemp, (int) VK_F1);
						break;

						case VK_F2:
							wsprintf(fsTemp.szFileName, "fkey%d.dat",2 );
							if(wKeyFlags & VK_SHIFT)	// Save window if SHIFT
								WriteFkeyFile(&fsTemp, (int) VK_F2);
							else	// Restore window
								iRet=LoadFkeyFile(&fsTemp, (int) VK_F2);
						break;

						case VK_F3:
							wsprintf(fsTemp.szFileName, "fkey%d.dat",3 );
							if(wKeyFlags & VK_SHIFT)	// Save window if SHIFT
								WriteFkeyFile(&fsTemp, (int) VK_F3);
							else	// Restore window
								iRet=LoadFkeyFile(&fsTemp, (int) VK_F3);
						break;

						case VK_F4:
							wsprintf(fsTemp.szFileName, "fkey%d.dat",4 );
							if(wKeyFlags & VK_SHIFT)	// Save window if SHIFT
								WriteFkeyFile(&fsTemp, (int) VK_F4);
							else	// Restore window
								iRet=LoadFkeyFile(&fsTemp, (int) VK_F4);
						break;

						case VK_F5:
							wsprintf(fsTemp.szFileName, "fkey%d.dat",5 );
							if(wKeyFlags & VK_SHIFT)	// Save window if SHIFT
								WriteFkeyFile(&fsTemp, (int) VK_F5);
							else	// Restore window
								iRet=LoadFkeyFile(&fsTemp, (int) VK_F5);

						break;

						case VK_F6:
							wsprintf(fsTemp.szFileName, "fkey%d.dat",6 );
							if(wKeyFlags & VK_SHIFT)	// Save window if SHIFT
								WriteFkeyFile(&fsTemp, (int) VK_F6);
							else	// Restore window
								iRet=LoadFkeyFile(&fsTemp, (int) VK_F6);
						break;

						case VK_F7:
							wsprintf(fsTemp.szFileName, "fkey%d.dat",7 );
							if(wKeyFlags & VK_SHIFT)	// Save window if SHIFT
								WriteFkeyFile(&fsTemp, (int) VK_F7);
							else	// Restore window
								iRet=LoadFkeyFile(&fsTemp, (int) VK_F4);
						break;

						case VK_F8:
							wsprintf(fsTemp.szFileName, "fkey%d.dat",8 );
							if(wKeyFlags & VK_SHIFT)	// Save window if SHIFT
								WriteFkeyFile(&fsTemp, (int) VK_F8);
							else	// Restore window
								iRet=LoadFkeyFile(&fsTemp, (int) VK_F8);
						break;

						case VK_F9:
							wsprintf(fsTemp.szFileName, "fkey%d.dat",9 );
							if(wKeyFlags & VK_SHIFT)	// Save window if SHIFT
								WriteFkeyFile(&fsTemp, (int) VK_F9);
							else	// Restore window
								iRet=LoadFkeyFile(&fsTemp, (int) VK_F9);
						break;

					}
			

					////////////////////////////////////////////////////////////////////
					// Trying to load an FKEY file that is incompatible with the RACK setting
					// Shutdown and EXIT
					if(iRet==2)	
					{
						ShutdownProc();
						return FALSE;
					}
			
			break;

		//````````````````````````````
		case WM_U_COMIO_IN_DONE:
			UpdateFromExternalInterface();
			break;
		//````````````````````````````
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
			case IDM_START_TRACKING_WINDOW:
				if(IsTrackingActive() == TRUE)
				{
					CheckMenuItem(ghMainMenu, IDM_START_TRACKING_WINDOW, MF_UNCHECKED);
					StopTrackingWindow();
				}	
				else
				{
					CheckMenuItem(ghMainMenu, IDM_START_TRACKING_WINDOW, MF_CHECKED);
					ActivateTrackingWindow();
				}
				break;


			//////////////////////////////////
			// Set global flag to indicate that
			// Solo Mute Mode is enabled for the
			// CUES, this will cause the
			// CancelCue() routine to be called
			// whenever a cue button is processed
			// See Events interface.c

			case IDM_SOLO_CUE_MODE:
				if(g_bIsSoloCueMode == TRUE)
				{
					CheckMenuItem(ghMainMenu, IDM_SOLO_CUE_MODE, MF_UNCHECKED);
					g_bIsSoloCueMode = FALSE;
				}	
				else
				{
					CheckMenuItem(ghMainMenu, IDM_SOLO_CUE_MODE, MF_CHECKED);
					g_bIsSoloCueMode = TRUE;
				}
				break;

			///////////////////////////////////
			// Let user select the scroll speed
			// for the updown scroll control
			// (the value box one)
			// Used for touchscreen, default
			// is fast speed

			case IDM_UPDOWNSCROLL_SPEED:
				if(g_bUpDownScollSpeedFAST == TRUE)
				{
					CheckMenuItem(ghMainMenu, IDM_UPDOWNSCROLL_SPEED, MF_CHECKED);	//MAKE IT SLOW
					g_bUpDownScollSpeedFAST = FALSE;
				}	
				else
				{
					CheckMenuItem(ghMainMenu, IDM_UPDOWNSCROLL_SPEED, MF_UNCHECKED);
					g_bUpDownScollSpeedFAST = TRUE;
				}
				break;



			case IDM_V_PROP_FILTER:
				GetSeqUpdateProps(NULL);
				break;
			// Toolbar control and menu control for the Sequence 
			case IDM_S_PLAY:
			case IDM_S_RECALL_CURRENT:
			case IDM_S_PAUSE:
			case IDM_S_GOTO_FIRST:
			case IDM_S_BACK:
			case IDM_S_NEXT:
			case IDM_S_GOTO_LAST:
				HandleRemoteSequenceControl(LOWORD(wParam));
				break;
			//````````````````````````````
			case IDM_RESETBUS:
				CDef_ResetBus ();
				break;
			//````````````````````````````
			case IDM_RESET_ALL_CONTROLS:
					// Reset all mixer Cotrol Values .. to Reg0 value
			if(ConfirmationBox(ghwndMDIClient, ghInstStrRes, IDS_RECALL_ALL_CONTROLS) == IDYES)
			{
				CDef_StopVuData();
				SetMemoryMapDefaults();
				RecallMemoryMapBuffer(TRUE,0);// Force the Control data to the Mixer
				CDef_StartVuData ();
			}
					break;
			//````````````````````````````
			case IDM_RECAL_ALL_CONTROLS:
					// Force the Control data to the mixer
					// regardles of the Current settings
					// Al Filters will remainengaged so 
					// the controls that are filtered-out 
					// willnot be recalled until the filters
					// are removed ...
					// 
				if(ConfirmationBox(ghwndMDIClient, ghInstStrRes, IDS_RECALL_ALL_CONTROLS) == IDYES)
				{
					CDef_StopVuData();
					MoveMemory(gpwMemMapBuffer, gpwMemMapMixer, giMemMapSize);
					RecallMemoryMapBuffer(TRUE,0);// Force the Control data to the Mixer
					CDef_StartVuData ();
				}
				break;
			case IDM_REQUEST_SETUPDATA:
					if(CDEF_GetCSData(hWnd))
						Init_MixerData();
					break;

			case IDM_START_VU_READ:
				CDef_StartVuData();
				break;
			case IDM_STOP_VU_READ:
				CDef_StopVuData();
				break;

	#ifdef NOTUSED_ANYMORE    // Currently only using the log VU version

			case IDM_VU_LINEAR:
				CDef_StopVuData();
				Sleep(10);

				InitVULookupTables(TRUE);
				CDef_StartVuData();
				break;

			case IDM_VU_LOG:
				CDef_StopVuData();
				Sleep(10);

				InitVULookupTables(FALSE);
				CDef_StartVuData();
				break;
	#endif

			//``````````````
			case IDM_F_OPEN_FILE:
							if(OpenMixFile()==FALSE)	// MOST LIKELY INCOMPATIBLE MIXFILE AND RACK
										PostQuitMessage(0);
					break;
			//``````````````
			case IDM_F_SAVE_FILE:

				// Save the currently loaded mix file
				wsprintf(fsTemp.szFileDir, "%smix\\", gszProgDir);
				ExtractFileName(GetSequenceFileName(), szScratch);	// get the filename
				wsprintf(fsTemp.szFileName, "%s", szScratch);
				WriteMixFile(&fsTemp, FALSE);

//fds					if(lstrlen(gfsMix.szFileName) > 0)
//						WriteMixFile(&gfsMix, TRUE);
//					else
//						SaveMixFile();
					break;

			case IDM_F_SAVE_FILE_AS:
					SaveMixFile();
					break;
			//``````````````
	#ifdef NOTUSED		// removed 1/7/2001 as per Gamble - Same as Save Mix
			case IDM_F_UPDATE_FILE:
					UpdateMixFile();
					break;
	#endif
			//``````````````
			case IDM_F_EXIT:
					PostMessage(hWnd,WM_CLOSE,wParam,lParam);	// Handle it like the WM_CLOSE - save mix file in registry
					break; 
			//``````````````
			case IDM_V_FULLCONSOLE:
					CreateFullViewWindow("Full View", NULL);
					break;
			//``````````````
			case IDM_V_FULLZOOM:
					CreateZoomViewWindow(hWnd, "Zoom View", NULL, DCX_DEVMAP_MODULE_INPUT);
					break;
			//``````````````
			case IDM_V_FULLZOOM_MASTER:
					if(ghwndMaster == NULL)
						CreateMasterViewWindow("Zoom Master View", NULL);
					else
					{
						DestroyMasterViewWindow(ghwndMaster);
					}
					break;
			//``````````````````
			case IDM_V_SEQUENCE:
					ShowSeqWindow(TRUE);
					break;
			//```````````````
			case IDM_V_GROUP:
					ShowGroupWindow(TRUE);
					break;
			//```````````````````
			case IDM_WINDOWICONS:
					SendMessage(ghwndMDIClient, WM_MDIICONARRANGE, 0, 0l);
					break;
			//``````````````
			case IDM_WINDOWCASCADE:
					SendMessage(ghwndMDIClient, WM_MDICASCADE, 0, 0l);
					break;
			//``````````````
			case IDM_WINDOWTILE:
				SendMessage(ghwndMDIClient, WM_MDITILE, 0, 0l);
				break;
			//``````````````
			case IDM_WINDOWCLOSEALL:
					CloseAllMDI();
					break;
    
			//```````````````
			case IDM_F_SAVEPREFERENCES:
					DoPreferences();
					break;
			case IDM_F_CONDEF_PREFERENCES:
					// display the preferances for the ConsoleDefinition DLL
					CDef_Preferences(hWnd);
					break;
			//```````````````
			case IDM_GENERATE_DLLIST:
					GenerateLL();
					break;
			//```````````````
			case IDM_V_CANCELCUES:
				CancelAllCues (hWnd);
				break;
			//```````````````
			case IDM_V_CANCELGROUPS:
				DeactivateGroup();
				break;
			case IDM_V_CANCELGROUPS+1:
			case IDM_V_CANCELGROUPS+2:
			case IDM_V_CANCELGROUPS+3:
			case IDM_V_CANCELGROUPS+4:
			case IDM_V_CANCELGROUPS+5:
			case IDM_V_CANCELGROUPS+6:
			case IDM_V_CANCELGROUPS+7:
			case IDM_V_CANCELGROUPS+8:
				
				ActivateGroup(-1, FADERS_GROUPS, FindConsecutiveGroupIndex(LOWORD(wParam) - IDM_V_CANCELGROUPS - 1, FADERS_GROUPS));
				break;

		case IDM_H_INFO:
				bResult = WinHelp(hWnd,"VACSKEYS.HLP",HELP_FINDER,0);
				if(bResult == 0)
				MessageBox(ghwndMain,"Help Failed: VACSKEYS.HLP NOT FOUND","Info",MB_OK);
		break;

    case IDM_H_ABOUT:

        DialogBox(ghInstMain, MAKEINTRESOURCE(IDD_ABOUT), ghwndMain, AboutProc);
        break;
    // To handle the MDI messages
    //---------------------------
    default:
        return DefFrameProc(hWnd, ghwndMDIClient, wMessage, wParam, lParam);

    }
    break;  // BREAK FROM WM_COMMAND

		// handle this when we are losing focus
		// to prevent some strange effects of "alt-tab" switching
		//

		case WM_KILLFOCUS:
			focus_wnd = (HWND)wParam;

			if (focus_wnd)
			{
				while (focus_wnd = GetParent(focus_wnd))
				{
					if (focus_wnd != ghwndMDIClient)
						last_wnd = focus_wnd;
					else
						break;
				}

				if (last_wnd)
					SetActiveWindow (last_wnd);
			}
											
			break;

    //-------------
    case WM_NOTIFY:
			pnotify = (LPNMHDR) lParam;
      switch (pnotify->code) 
      { 
        case TTN_NEEDTEXT: // ToolTips needs some text
        {             
          LPTOOLTIPTEXT   lpttt;
          int             idButton;
        
          lpttt         = (LPTOOLTIPTEXT) lParam;
					lpttt->lpszText = NULL;

					//////////////////////////////////////////////////////
          // Specify the resource identifier of the descriptive
          // text for the given button on the toolbar.
					//////////////////////////////////////////////////////

          idButton = lpttt->hdr.idFrom;
          switch (idButton) 
          {          
          case IDM_V_FULLCONSOLE: 
						lpttt->lpszText = "Open Full console";
            break;
          case IDM_V_FULLZOOM:
						lpttt->lpszText = "Open Zoom console";
            break;
					case IDM_V_CANCELGROUPS:
						lpttt->lpszText = "Cancel Groups";
						break;

					/////////////////////////////////////////////
					// GROUP Button TIPS
					// Get tip text from the Group Name list box

					case IDM_V_CANCELGROUPS+1:
					case IDM_V_CANCELGROUPS+2:
					case IDM_V_CANCELGROUPS+3:
					case IDM_V_CANCELGROUPS+4:
					case IDM_V_CANCELGROUPS+5:
					case IDM_V_CANCELGROUPS+6:
					case IDM_V_CANCELGROUPS+7:
					case IDM_V_CANCELGROUPS+8:
						lpttt->lpszText = GetListItemGroupName(idButton - IDM_V_CANCELGROUPS - 1);
						break;


					case IDM_V_CANCELCUES:
						lpttt->lpszText = "Cancel Cues";
						break;
					case IDM_V_GROUP:
						lpttt->lpszText = "Show Group Window";
						break;
					case IDM_V_SEQUENCE:
						lpttt->lpszText = "Show Sequence Window";
						break;
					case IDM_S_NEXT:
						lpttt->lpszText = "Next Sequence";
						break;
					case IDM_S_BACK:
						lpttt->lpszText = "Prev Sequence";
						break;
					case IDM_F_SAVE_FILE:
						lpttt->lpszText = "Save Mix File";
						break;
					default:
						lstrcpy(lpttt->szText, "Unknown ...");
						break;
          }             
          break;
        } // end of TTN_NEEDTEXT
        default:
          break;
      }
      break; // WM_NOTIFY
    //////////////////////////////////////////////////////////////
    case WM_SIZE:
      SizeClientWindow(hWnd, wMessage, wParam, lParam);

			// Hack the Master Window to the correct size
			// after we size the main window

				if(ghwndMaster)
				{
          DestroyMasterViewWindow(ghwndMaster);
          CreateMasterViewWindow("Zoom Master View", NULL);
				}

     break;

    //////////////////////////////////////////////////////////////
    case WM_CREATE:
      iRet = CreateMainStatusWindow(hWnd);
      if(iRet)
      {
        ErrorBox(NULL, ghInstMain, iRet);
        DestroyWindow(ghwndMain);
        PostQuitMessage(0);
				ghwndMain = NULL;
        break;
      }

			// Start the toolbars ...
			//
			iRet = CreateToolBars(hWnd);
			if(iRet)
			{
        ErrorBox(NULL, ghInstMain, iRet);
        DestroyWindow(ghwndMain);
        PostQuitMessage(0);
				ghwndMain = NULL;
			}

      iRet = CreateMDIClient(hWnd);
      if(iRet)
      {
        ErrorBox(NULL, ghInstMain, iRet);
        DestroyWindow(ghwndMain);
        PostQuitMessage(0);
				ghwndMain = NULL;
      }

			// Used to periodically save mix files

			timerID = SetTimer(hWnd, 33, 200000, NULL);
      break;

		///////////////////////////////////////////////////////////////
		case WM_TIMER:
			
			///////////////////////////////////////////////////////////
			// Our timer to update the LA$T.mix file
			// and the currently loaded MIX file.

			if(giMixerTypeOffline == -1)	// Only save if NOT online
			{	// Save into LA$T.mix
				wsprintf(fsTemp.szFileDir, "%smix\\", gszProgDir);
				wsprintf(fsTemp.szFileName, "%s", "LA$T.mix");
				WriteMixFile(&fsTemp, FALSE);

				// Save the currently loaded mix file
				ExtractFileName(GetSequenceFileName(), szScratch);	// get the filename
				wsprintf(fsTemp.szFileName, "%s", szScratch);
				WriteMixFile(&fsTemp, FALSE);

#ifdef AUTOSAVE

				////////////////////////////////////////////////
				// Jims AutoSave mix file logic
				// If a mix had been loaded then save it
				// else prompt them for a file name and save it

					if(lstrlen(gfsMix.szFileName) > 0)
						WriteMixFile(&gfsMix, TRUE);
					else
					{
						bResult=KillTimer(hWnd, timerID);	// Make sure we don't get here again

						////////////////////////////////////
						// Get the last mix file that was
						// saved and set the global filename
						// structure

						lnResult = RegCreateKeyEx(
												HKEY_CURRENT_USER,
												szRegKey,
												0, 
												REG_NONE, 
												0, 
												KEY_ALL_ACCESS, 
												NULL, 
												&hKey,
												&dwDisposition );

						szRegValue = "MRUmix";

						dwBufferSize = sizeof(szTempSeq);

						rc = RegQueryValueEx( 
									hKey, 
									szRegValue, 
									NULL, 
									&dwType, 
									&szTempSeq, 
									&dwBufferSize ); 

						// Set our global filename name to the last loaded mix file
						// so that when we ask them to save the mix file it is the
						// defailt name 

						if(rc == ERROR_SUCCESS)
							wsprintf(gfsTemp.szFileName, "%s", szTempSeq);

						// Since this is triggered by the timer we never know when
						// we will get here.

						ReleaseCapture(); // Release the mouse capture in any case

						SaveMixFile();									// 
						timerID = SetTimer(hWnd, 33, 200000, NULL);
					}
#endif	// AUTOSAVE
			}
			break;

    //////////////////////////////////////////////////////////////
    case WM_CLOSE:

		///////////////////////////////////////
		// Removed confirmation on exist as
		// per Gamble 12/12/2001 email
		// 
		// 1/17/2002 only of AUTOSAVE is it removed
		//
		//      if(ConfirmationBox(ghwndMDIClient, ghInstStrRes, IDS_QUIT_MESSAGE) == IDNO)
		//         break;

				// Kill the 3 minute timer to save mix file
				
				KillTimer(hWnd, timerID);

			/////////////////////////////////////////////////////////////
			// Store the last loaded mix file into the registry so that
			// we can load the associated sequence file for that mix file

			wsprintf(szBuff, "%smix\\%s", gszProgDir, "LA$T.mix");

			wsprintf(szScratch,"%s",GetSequenceFileName());

			if(lstrcmp(szBuff, GetSequenceFileName()) != 0)
			{
				lnResult = RegCreateKeyEx(
										HKEY_CURRENT_USER,
										szRegKey,
										0, 
										REG_NONE, 
										0, 
										KEY_ALL_ACCESS, 
										NULL, 
										&hKey,
										&dwDisposition );

				// Save the mix file name in the registry if it isn't the LA$T.mix file

				dwBufferSize = sizeof(szScratch);

				lnResult = RegSetValueEx(
										 hKey, 
										 "MRUmix",
										 REG_OPTION_RESERVED,
										 REG_SZ,
										 GetSequenceFileName(),
										 dwBufferSize );

				RegCloseKey(hKey);
			}

			///////////////////////////////////////////
			// We are exiting, write the LA$T.mix file
			// and the currently loaded MIX file
			///////////////////////////////////////////

			wsprintf(fsTemp.szFileDir, "%smix\\", gszProgDir);
			wsprintf(fsTemp.szFileName, "%s", "LA$T.mix");
			WriteMixFile(&fsTemp, FALSE);

			// Save the currently loaded mix file
			ExtractFileName(GetSequenceFileName(), szScratch);	// get the filename
			wsprintf(fsTemp.szFileName, "%s", szScratch);
			WriteMixFile(&fsTemp, FALSE);

      CloseAllMDI();
      DestroyWindow(hWnd);
      PostQuitMessage(0);
			ghwndMain = NULL;
      break;

    //////////////////////////////////////////////////////////////
    case WM_PALETTECHANGED:
      if((HWND)wParam == hWnd)
        return 0;

    case WM_QUERYNEWPALETTE:
      hDC = GetDC(hWnd);
      UpdatePalette(FALSE, hDC);
      ReleaseDC(hWnd, hDC);
      return 1;
      break;
    //////////////////////////////////////////////////////////////
    case WM_SYSCOMMAND:
      if( ((wParam & 0xFFF0)  == SC_MINIMIZE) ||
          ((wParam & 0xFFF0)  == SC_MAXIMIZE) ||
          ((wParam & 0xFFF0)  == SC_RESTORE) )
      {
          hDC = GetDC(hWnd);
          UpdatePalette(FALSE, hDC);
          ReleaseDC(hWnd, hDC);
      }
   default:
      return DefFrameProc(hWnd, ghwndMDIClient, wMessage, wParam, lParam);
	 }
return (0L);
}



//===========================================
//function: AboutProc
//
// About dialog box procedure
// Replaces place holder text on dialog
// with info from version resource
//===========================================

BOOL CALLBACK   AboutProc(HWND hdlg, UINT uiMsg, WPARAM wP, LPARAM lP)
{

static  HFONT   hFontDlg;
static  HFONT   hFontCopyRight;
static  LPSTR   lpVersion;
static  DWORD   dwVerInfoSize;
static  DWORD   dwVerHnd;
static  UINT    uVersionLen;
static  WORD    wRootLen;
static  BOOL    bRetCode;

static  char    szFullPath[256];
static  char    szResult[256];
static  char    szGetName[256];

static  int     i,iDlg;

	switch(uiMsg)
  {
    case WM_COMMAND:
        switch(LOWORD(wP))
            {
            case IDOK:
                EndDialog(hdlg, TRUE);
                break;
            }
        break;
///////////////////////////////////////
        case WM_INITDIALOG:

            // Center the dialog over the application window

          GetModuleFileName(ghInstMain, szFullPath, sizeof(szFullPath));
          dwVerInfoSize = GetFileVersionInfoSize(szFullPath , &dwVerHnd);

          if(dwVerInfoSize)   // If we can get the version info then process it
          {
            LPSTR lpstrVffInfo;
            HANDLE  hMem;

                hFontDlg = CreateFont(18,0, 0, 0,
                                       0, 0, 0, 0,
                                       0, 0, 0, 0,
////                                       VARIABLE_PITCH | FF_SWISS,"");
                                       VARIABLE_PITCH,"Arial");


            hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
            lpstrVffInfo = (char *)GlobalLock(hMem);

            bRetCode=GetFileVersionInfo(szFullPath, dwVerHnd, dwVerInfoSize, lpstrVffInfo);

		        // Handle the language gracefully
            // (Under both Windows NT and Windows 95, you can get the language information in 
            // the FileVersionInfo of User.exe by calling GetFileVersionInfo, and then 
            // VerQueryValue (on \\VarFileInfo\\Translation") on the VersionInfo block of 
            // the operating system's User.exe.)
            // Implement the above later ??????????????????????????????????

		        if (LANG_JAPANESE == PRIMARYLANGID(GetUserDefaultLangID())) // User can change this is control panel!!
			        lstrcpy(szGetName, "\\StringFileInfo\\041104E4\\");
		        else
			        lstrcpy(szGetName, "\\StringFileInfo\\040904B0\\");

            wRootLen = lstrlen(szGetName);

        #ifdef TEST
            VS_FIXEDFILEINFO *lpVersion;
        #endif
    
            // Walk through the dialog items that we want to replace
            for(i = IDT_COMPANY_NAME; i <= IDT_TRADEMARKS; i++)
            {
              GetDlgItemText(hdlg,i, szResult, sizeof(szResult));
              szGetName[wRootLen] = (char)0;
              lstrcat(szGetName, szResult);

              uVersionLen = 0;
              lpVersion = NULL;

              bRetCode = VerQueryValue((LPVOID)lpstrVffInfo,
                                        szGetName,
                                        (LPVOID *)&lpVersion,
                                        (PUINT)&uVersionLen);



               if(bRetCode && uVersionLen && lpVersion)
               {
                  lstrcpy(szResult, lpVersion);
                  SetDlgItemText(hdlg,i,szResult);    // Replace dialog item text with version info
                  SendDlgItemMessage(hdlg,i, WM_SETFONT, (UINT)hFontDlg, TRUE);

                  if(i==IDT_TRADEMARKS)
                  {
 
											hFontCopyRight = CreateFont(12,0, 0, 0,
                                       0 , 0, 0, 0,
                                       0, 0, 0, 0,
                                       VARIABLE_PITCH,"Arial");

                      SendDlgItemMessage(hdlg, 
                                   IDT_TRADEMARKS, 
                                   WM_SETFONT, 
                                   (WPARAM)hFontCopyRight,
                                   0L);
                  }
               }
            }

                GlobalUnlock(hMem);
                GlobalFree(hMem);
          }
                return TRUE;


        ///////////////////////////////////////

            case WM_CLOSE:
            case WM_QUIT:

                EndDialog(hdlg, TRUE);
                break;
  }

	return FALSE;

}



//====================================
// function: ConfirmationBox
//
//====================================
int   ConfirmationBox(HWND hwnd, HINSTANCE hinst, int iResource)
{
char    szMsg[128];

	LoadString(hinst, iResource, szMsg, 128);

return MessageBox(hwnd, szMsg, "Confirmation", MB_YESNO);
};

//====================================
// function: ConfirmationBox
//
//====================================
int   InformationBox(HWND hwnd, HINSTANCE hinst, int iResource)
{
char    szMsg[128];

	LoadString(hinst, iResource, szMsg, 128);

return MessageBox(hwnd, szMsg, "Information", MB_OK);
};


//====================================
// function: ErrorBox
//
//====================================
int   ErrorBox(HWND hwnd, HINSTANCE hinst, int iResource)
{
char    szMsg[256];

	LoadString(hinst, iResource, szMsg, 256);

	return MessageBox(hwnd, szMsg, "Error!", MB_ICONSTOP);
};

//=================================================
//function: InformationStatus
//
//=================================================
void    InformationStatus(HINSTANCE hinst, int iResource)
{
char    szMsg[128];

	if(ghwndStatus == NULL)
			return;

	LoadString(hinst, iResource, szMsg, 128);
	SendMessage(ghwndStatus, SB_SETTEXT, MAKEWPARAM(0,SBT_POPOUT), (LPARAM)szMsg);

return;
}


//==================================================
//FUNCTION: AddToCaption
//
//purpose:
//      Adds a string to the Caption
//
//==================================================
void      AddToCaption(HWND hwnd, LPSTR lpstr)
{
char    szText[128];
char    szNewText[128];

	GetWindowText(hwnd, szText, 128);
	wsprintf(szNewText, "%s %s", szText, lpstr);
	SetWindowText(hwnd, szNewText);

return;
}

//==================================================
//FUNCTION:ClipCtrlZone
//
//purpose:
//      Clip the Cursor to the Zone Rect.
//
//
//==================================================
void      ClipCtrlZone(HWND hwnd, LPMIXERWNDDATA  lpmwd)
{
RECT        rZone;
POINT       pnt;
int         iCtrlType;
int         iXadj;
int         iHeight;
int         iWidth;
int         iYadj;
int         iChanPhis;

	//////////////////////////////////
	// get the Zone map rectangle
	// and the Current Mouse Position
	// and the screen adjustment
	//-------------------------------
	rZone = lpmwd->lpCtrlZM->rZone;
	pnt   = lpmwd->pntMouseCur;
	iXadj = lpmwd->iXadj;

	////////////////////////////////
	// Depending on the Control Type
	// Calculate the Clip Boundries
	// And where the mouse should be
	//------------------------------
	iCtrlType = lpmwd->lpCtrlZM->iCtrlType;

	iHeight = gpBMPTable[lpmwd->lpCtrlZM->iCtrlBmp[0]].iHeight;
	iWidth = gpBMPTable[lpmwd->lpCtrlZM->iCtrlBmp[0]].iWidth;


	// First get the current value of the Control
	//-------------------------------------------
	iChanPhis = LOBYTE(lpmwd->lpwRemapToScr[lpmwd->iCurChan + lpmwd->iStartScrChan]);


	if(iCtrlType == CTRL_TYPE_FADER_VERT)
  {
		rZone.left = rZone.left + (rZone.right - rZone.left)/2;
		rZone.right = rZone.left + 1;
		rZone.top += iHeight/2;
		rZone.bottom -= iHeight/2;
		pnt.x = rZone.left;

		iXadj = 0;
		iYadj = GETPHISDATAVALUE(lpmwd->iMixer, lpmwd->lpCtrlZM, lpmwd->lpCtrlZM->iCtrlChanPos);

		if(lpmwd->lpCtrlZM->iNumValues > 0)
			CONVERTPHISICALTOSCREEN(lpmwd->lpCtrlZM, iYadj); // we need to do this since the phisical resolution could be different than the screen resolution
		else
			iYadj = 0;
  }

	if(iCtrlType == CTRL_TYPE_FADER_HORZ)
  {
		rZone.top = rZone.top + (rZone.bottom - rZone.top)/2;
		rZone.bottom = rZone.top + 1;
		rZone.left += iWidth/2;
		rZone.right -= iWidth/2;
		pnt.y = rZone.top;
  
		iYadj = 0;
		iXadj = GETPHISDATAVALUE(lpmwd->iMixer, lpmwd->lpCtrlZM, lpmwd->lpCtrlZM->iCtrlChanPos);

		if(lpmwd->lpCtrlZM->iNumValues > 0)
			CONVERTPHISICALTOSCREEN(lpmwd->lpCtrlZM, iXadj); // we need to do this since the phisical resolution could be different than the screen resolution
		else
			iXadj = 0;
  }

	rZone.left  += lpmwd->iXadj;
	rZone.right += lpmwd->iXadj;
	rZone.top   -= lpmwd->iYOffset;
	rZone.bottom-= lpmwd->iYOffset;

#ifdef MIDI_SUPPORT
// Check this if needed	iYadj = PhisDataToScrPos(lpmwd->lpCtrlZM, (int)lpmwd->lpCtrlZM->iMIDIToScr_Indx, iYadj);
#endif

	pnt.x = rZone.left + iXadj;
	pnt.y = rZone.top + iYadj;

	// Save the mouse position
	// as current
	//------------------------
	lpmwd->pntMouseCur = pnt;

	ClientToScreen(hwnd, &pnt);
	ClientToScreen(hwnd, (LPPOINT)&rZone);
	ClientToScreen(hwnd, (LPPOINT)&rZone.right);

	SetCursorPos(pnt.x, pnt.y);

	#ifndef DONOT_CLIP_CURSOR
	ClipCursor(&rZone);
	#endif

	return;

}

//===========================================
//
//
//
//
//===========================================

void    ClipZone(HWND hwnd, LPRECT pRect)
{
RECT        rZone;

	rZone = *pRect;

	rZone.top  = 0;
	rZone.left = 0;
	ClientToScreen(hwnd, (LPPOINT)&rZone);
	ClientToScreen(hwnd, (LPPOINT)&rZone.right);

	#ifndef DONOT_CLIP_CURSOR
	ClipCursor(&rZone);
	#endif
	return;
}




//////////////////////////////////////////////////
// Write text to a DC in a specified rectangle
//
//
//
//////////////////////////////////////////////////

void  WriteTextToDC(HDC hdc,LPRECT rect,int iXofst,int iYofst,
                          COLORREF clrref,    LPSTR text )
{
RECT    rRect;
int     iOldBkMode;
//HPEN    hpnOld;
HFONT   hfontOld;


	hfontOld = SelectObject(hdc, g_hConsoleFont);

	iOldBkMode = SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, clrref);

	rRect = *rect;
	rRect.top    += iYofst + 1;
	rRect.bottom += iYofst + 1;
	rRect.left   += iXofst + 1;
	rRect.right  += iXofst + 1;

	DrawText(hdc, text, -1, &rRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//DT_WORDBREAK);

	SetBkMode(hdc, iOldBkMode);
	SelectObject(hdc, hfontOld);
	return;
}



//============================================
// Write text to a DC in a specified rectangle
//============================================
void  WriteTextLinesToDC(HDC hdc,LPRECT rect,int iXofst,int iYofst,
                         COLORREF clrref,    LPSTR text )
{
RECT    rRect;
int     iOldBkMode;
//HPEN    hpnOld;
HFONT   hfontOld;


	hfontOld = SelectObject(hdc, g_hConsoleFont);

	iOldBkMode = SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, clrref);

	rRect = *rect;
	rRect.top    += iYofst + 1;
	rRect.bottom += iYofst + 1;
	rRect.left   += iXofst + 1;
	rRect.right  += iXofst + 1;
                                  
	//DrawText(hdc, text, -1, &rRect, DT_LEFT | DT_VCENTER | DT_WORDBREAK);
	DrawText(hdc, text, -1, &rRect, DT_CENTER | DT_VCENTER | DT_WORDBREAK);
 

	SetBkMode(hdc, iOldBkMode);
	SelectObject(hdc, hfontOld);
	return;
}


////////////////////////////////////////////////////////////
//
// Write text to a DC in a specified rectangle Verticaly
//
//
//
//
////////////////////////////////////////////////////////////

void  WriteTextLinesToDCVerticaly(HDC hdc,LPRECT rect,int iXofst,int iYofst,
                         COLORREF clrref,    LPSTR text )
{
RECT    rRect;
int     iOldBkMode;
//HPEN    hpnOld;
HFONT   hfontOld;


	hfontOld = SelectObject(hdc, g_hConsoleFont);

	iOldBkMode = SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, clrref);

	rRect = *rect;
	rRect.top    += iYofst + 1;
	rRect.bottom += iYofst + 1;
	rRect.left   += iXofst + 1;
	rRect.right  += iXofst + 1;
                                  
	//DrawText(hdc, text, -1, &rRect, DT_LEFT | DT_VCENTER | DT_WORDBREAK);
	DrawText(hdc, text, -1, &rRect, DT_CENTER | DT_VCENTER | DT_WORDBREAK);
 

	SetBkMode(hdc, iOldBkMode);
	SelectObject(hdc, hfontOld);
	return;
}



//==================================
// finction IQS_MoveWindow
//   initiates the move sequence
//   for a window as if its being
//   draged with the Caption
//==================================

void  IQS_MoveWindow(HWND hwnd, int X, int Y)
{
POINT   ptPoint;
LPARAM  lParam;

	ptPoint.x = X;
	ptPoint.y = Y;

	ClientToScreen(hwnd, &ptPoint);

	lParam = MAKELPARAM(ptPoint.x, ptPoint.y);

	SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, lParam);
	return;
}


//====================================
//FUNCTION: GetMaxWindowSize
//
// Calculates the maximum Window size
// according to the Zone map and
// all of the Bitmap sizes included
// in it as well as the screen size
// and the available memory
//====================================

void      GetMaxWindowSize(LPRECT lpRect,
                                 LPZONE_MAP lpZM,
                                 long lZMCount,
                                 LPMIXERWNDDATA lpmwd,
																 BOOL bScrollBars)
{
long    lCount;
RECT    rect;

int     iScrWidth, iScrHeight;
int     iHeight = 0;
int     iWidth = 0;
int     iBmpIndx;


	GetClientRect(ghwndMDIClient, &rect);
	iScrWidth = rect.right;
	iScrHeight = rect.bottom;

	rect.left = 0;
	rect.top = 200;// the minimum height
	rect.right = (GetSystemMetrics(SM_CXFRAME)*2);
	rect.bottom = 0;


	for (lCount=0; lCount < lZMCount; lCount++)
  {
    iBmpIndx = lpZM[lCount].iBmpIndx;
    iWidth = gpBMPTable[iBmpIndx].iWidth;
    rect.right += iWidth; // the max width
    if(iWidth > rect.left)
        rect.left = iWidth + (GetSystemMetrics(SM_CXFRAME)*2); // the minimum width
    iHeight = gpBMPTable[iBmpIndx].iHeight;
    if(iHeight > rect.bottom)
        rect.bottom = iHeight;// the max Bitmap height

		//////////////////////
    // Check the max width
    // against the screen
    //--------------------
    if(iScrWidth < rect.right)
    {
        rect.right -= iWidth;
        break;
    }
  }

	// ADJUST for Vertical Scroll Bars

#ifdef SCROLLBARS
	if(bScrollBars)
		rect.right = rect.right + GetSystemMetrics(SM_CYVTHUMB) ;
#endif

	/////////////////////////////////////////////////////
	// This is the actual Maximum size of the Image Window 
	// without regard of the phisical size of the Screen
	// it doesn't mean that the Window can actualy
	// grow so big
	//-----------------------------------------------------
	lpmwd->rMaxSize = rect; 

	rect.bottom = rect.bottom + GetSystemMetrics(SM_CYFRAME) +
															(GetSystemMetrics(SM_CYBORDER)*4) +
															GetSystemMetrics(SM_CYCAPTION);// the max height


	////////////////////////
	// Check the max height
	// against the screen
	//---------------------
	if(iScrHeight < rect.bottom)
			rect.bottom = iScrHeight;

	lpmwd->iStartScrChan = 0;
	lCount--;
	lpmwd->iEndScrChan = lCount;

	// Set the Image Window Position
	// and max size
	//------------------------------
	lpmwd->rWndPos = rect;
	lpmwd->rWndPos.left = 0;
	lpmwd->rWndPos.top = 0; 

	lpmwd->rVisible = lpmwd->rWndPos;
	// make sure that the Visible size is not 
	// larger thatn the Max Phisical size
	// of the Image Window
	//----------------------------------------
	if(lpmwd->rVisible.right > lpmwd->rMaxSize.right)
			lpmwd->rVisible.right = lpmwd->rMaxSize.right;
	if(lpmwd->rVisible.bottom > lpmwd->rMaxSize.bottom)
			lpmwd->rVisible.bottom = lpmwd->rMaxSize.bottom;

	*lpRect = rect;
	return;
}



//=============================
// a test function
// nothing more
//
//

//=============================

void      GenerateLL(void)
{
LPDLROOTPTR     lpdlrPtr;
long            lItemCur;
//long            lItemNext;
//LPDLENTRYMEM    lpdle, lpdleNext;
//int             iDisp, iMajor;
//int             iErr;
//WORD            iVal1, iVal2, iVal3;
//LPWORD          lpw;
char            szBuff[255];
long            lCount;

	wsprintf(szBuff, "%s\\data\\sequenceTest.ctek",gszProgDir);      
	lpdlrPtr = InitDoubleLinkedList(255, 1024, TRUE, TRUE, NULL, szBuff);
	if(lpdlrPtr == NULL)
  {
    MessageBox(ghwndMain, "Error Initializing the Linked List","Info",MB_OK);
    FreeDLListRootAll(&lpdlrPtr);
    return;
  }

	MessageBox(ghwndMain, "List Created","Info",MB_OK);
	ZeroMemory(szBuff, 255);

// adding items
//-------------
	lItemCur = -1;
	for(lCount = 0; lCount < 1024; lCount++)
  {
    wsprintf(szBuff, "Hello there this is Entry Number = %ld", lCount);
    lItemCur = InsertEntry(lpdlrPtr, lItemCur , szBuff , 255, DL_RELATION_PEER);

    if(lItemCur ==  -1 )
    {
        MessageBox(ghwndMain, "Error Inserting an Item!","Info",MB_OK);
        break;
    }
  }

	MessageBox(ghwndMain, "Done Inserting Items!","Info",MB_OK);


	// deleting items
	//---------------
	while((lItemCur = GetFirstEntry(lpdlrPtr))  > 0)
  {
    if(DelEntryPtr(lpdlrPtr, lItemCur))
    {
        MessageBox(ghwndMain, "Error Deleting Item!","Info",MB_OK);
        break;
    }
  }


	MessageBox(ghwndMain, "Done Deleting Items!","Info",MB_OK);

// FreeDLListRootAll(&lpdlrPtr); ??


/*
lItemCur = -1;

//lpdlrPtr = InitDoubleLinkedList(1000000l, TRUE);
if(lpdlrPtr == NULL)
    {
    MessageBox(ghwndMain, "Error Allocating Memory","Info",MB_OK);
    return;
    }
for(iMajor = 0; iMajor < 10; iMajor++)
    {
    iVal1 =  rand()%32000;
    iVal2 =  rand()%32000;
    iVal3 =  rand()%32000;

    iDisp = 0;
    for(lCount = 0; lCount < 10000; lCount ++)
        {
        iDisp ++;
        if((lItemCur=AddEntry(lpdlrPtr, szBuff , 255))==NULL)
//        if((lpdle=AddEntry(lpdlr, 10))==NULL)
            {
            MessageBox(ghwndMain, "Error Adding Entry","Info",MB_OK);
            goto START_COMPACT;
            }
        lpw = lItemCur;
        lpw[0] = iVal1;
        lpw[1] = iVal2;
        lpw[2] = iVal3;

        if(iDisp >= 1000)
            {
            iDisp = 0;
            ShowDebugInfo(TRUE, 1, LOWORD(lCount));
            }
        }

  // Verify the Records
  //-------------------
    while(lItemCur = GetFirstEntry(lpdlrPtr) > 0)
        {
        lpw = lItemCur;
        if((iVal1 != lpw[0]) ||
           (iVal2 != lpw[1]) ||
           (iVal3 != lpw[2]))
            MessageBox(ghwndMain, "Data is Corrupted", "info",MB_OK);

        }

//    MessageBox(ghwndMain, "Done Creating Linked List","Info",MB_OK);

    ShowDebugInfo(FALSE, 0, 0);

    wsprintf(szBuff, "Ready\n"\
                     "total = %ld\n"\
                     "deleted = %ld\n"\
                     "first = %ld\n"\
                     "last =  %ld\n"\
                     "first del = %ld\n"\
                     "last del  = %ld",
                      lpdlrPtr->lEntryCount, lpdlrPtr->lDelCount,
                      (long)lpdlrPtr->lpdleFirst, (long)lpdlrPtr->lpdleLast,
                      (long)lpdlrPtr->lpdleFirstDel, (long)lpdlrPtr->lpdleLastDel);
    MessageBox(ghwndMain, szBuff,"Info",MB_OK);

    lCount = 0;
    iDisp = 0;

    
  // deleting items
  //---------------
  while((lItemCur = GetFirstEntry(lpdlrPtr))  > 0)
  {
    if(DelEntryPtr(lpdlrPtr, lItemCur))
    {
      MessageBox(ghwndMain, "Error Deleting Item!","Info",MB_OK);
      break;
    }
    iDisp ++;
    lCount ++;
    if(iDisp >= 1000)
    {
      iDisp = 0;
      ShowDebugInfo(TRUE, 1, LOWORD(lCount));
    }
   }
  }
wsprintf(szBuff, "Ready\n"\
                 "total = %ld\n"\
                 "deleted = %ld\n"\
                 "first = %ld\n"\
                 "last =  %ld\n"\
                 "first del = %ld\n"\
                 "last del  = %ld\n"\
                 "return code = %d",
                  lpdlr->lEntryCount, lpdlr->lDelCount,
                  (long)lpdlr->lpdleFirst, (long)lpdlr->lpdleLast,
                  (long)lpdlr->lpdleFirstDel, (long)lpdlr->lpdleLastDel,
                  iErr);

MessageBox(ghwndMain, szBuff,"Info",MB_OK);


START_COMPACT:
*/
	lpdlrPtr = CompactDLList(lpdlrPtr);

	MessageBox(ghwndMain, "Done Compacting Linked List","Info",MB_OK);


	FreeDLListRootAll(&lpdlrPtr);

	ShowDebugInfo(FALSE, 0, 0);


return;
}

//=================================
// Open a debug Window
//
//
//
//
//
//=================================

void      ShowDebugInfo(BOOL open,int allign, WORD w)
{
char        szBuff[100];

	if((open == FALSE)&&(ghwndDebug))
  {
    DestroyWindow(ghwndDebug);
    ghwndDebug = NULL;
    return;
  }

	if((ghwndDebug == 0) && open)
  {
    ghwndDebug = CreateDialog(ghInstMain,MAKEINTRESOURCE(IDD_DEBUG), ghwndMain,  DebugProc );
    giDebugIndx = -1;
  }

	if(allign != 0)
  {
    giDebugIndx ++;
    if(giDebugIndx >= 8)
    {
        wsprintf(szBuff, "%06x %06x %06x %06x %06x %06x %06x %06x",
                          gwDebugBuff[0], gwDebugBuff[1], gwDebugBuff[2],
                          gwDebugBuff[3], gwDebugBuff[4], gwDebugBuff[5],
                          gwDebugBuff[6], gwDebugBuff[7]);
        SendDlgItemMessage(ghwndDebug, IDC_EDIT_DEBUG_OUTPUT, LB_ADDSTRING, 0, (LPARAM)(LPSTR)szBuff );
        giDebugIndx = 0;
    }

    gwDebugBuff[giDebugIndx] = w;
  }
	else
	{
    allign = giDebugIndx + 1;
	}

	switch(allign)
  {
    case 1:
        if(giDebugIndx == 0)
        {
            wsprintf(szBuff, "%06x ", gwDebugBuff[0]);
            SendDlgItemMessage(ghwndDebug, IDC_EDIT_DEBUG_OUTPUT, LB_ADDSTRING, 0, (LPARAM)(LPSTR)szBuff );
            giDebugIndx = -1;
        }
        break;
    case 2:
        if(giDebugIndx == 1)
        {
            wsprintf(szBuff, "%06x %06x", gwDebugBuff[0], gwDebugBuff[1]);
            SendDlgItemMessage(ghwndDebug, IDC_EDIT_DEBUG_OUTPUT, LB_ADDSTRING, 0, (LPARAM)(LPSTR)szBuff );
            giDebugIndx = -1;
        }
        break;
    case 3:
        if(giDebugIndx == 2)
        {
            wsprintf(szBuff, "%06x %06x %06x", gwDebugBuff[0], gwDebugBuff[1], gwDebugBuff[2]);
            SendDlgItemMessage(ghwndDebug, IDC_EDIT_DEBUG_OUTPUT, LB_ADDSTRING, 0, (LPARAM)(LPSTR)szBuff );
            giDebugIndx = -1;
        }
        break;
    case 4:
        if(giDebugIndx == 3)
        {
            wsprintf(szBuff, "%06x %06x %06x %06x", gwDebugBuff[0], gwDebugBuff[1], gwDebugBuff[2], gwDebugBuff[3]);
            SendDlgItemMessage(ghwndDebug, IDC_EDIT_DEBUG_OUTPUT, LB_ADDSTRING, 0, (LPARAM)(LPSTR)szBuff );
            giDebugIndx = -1;
        }
        break;
    case 5:
        if(giDebugIndx == 4)
        {
            wsprintf(szBuff, "%06x %06x %06x %06x %06x", gwDebugBuff[0], gwDebugBuff[1], gwDebugBuff[2], gwDebugBuff[3],gwDebugBuff[4]);
            SendDlgItemMessage(ghwndDebug, IDC_EDIT_DEBUG_OUTPUT, LB_ADDSTRING, 0, (LPARAM)(LPSTR)szBuff );
            giDebugIndx = -1;
        }
        break;
    case 6:
        if(giDebugIndx == 5)
        {
            wsprintf(szBuff, "%06x %06x %06x %06x %06x %06x", gwDebugBuff[0], gwDebugBuff[1], gwDebugBuff[2], gwDebugBuff[3], gwDebugBuff[4], gwDebugBuff[5]);
            SendDlgItemMessage(ghwndDebug, IDC_EDIT_DEBUG_OUTPUT, LB_ADDSTRING, 0, (LPARAM)(LPSTR)szBuff );
            giDebugIndx = -1;
        }
        break;
    case 7:
        if(giDebugIndx == 6)
        {
            wsprintf(szBuff, "%06x %06x %06x %06x %06x %06x %06x", gwDebugBuff[0], gwDebugBuff[1], gwDebugBuff[2], gwDebugBuff[3], gwDebugBuff[4], gwDebugBuff[5], gwDebugBuff[6]);
            SendDlgItemMessage(ghwndDebug, IDC_EDIT_DEBUG_OUTPUT, LB_ADDSTRING, 0, (LPARAM)(LPSTR)szBuff );
            giDebugIndx = -1;
        }
        break;
    case 8:
        if(giDebugIndx == 7)
        {
            wsprintf(szBuff, "%06x %06x %06x %06x %06x %06x %06x %06x",
                              gwDebugBuff[0], gwDebugBuff[1], gwDebugBuff[2],
                              gwDebugBuff[3], gwDebugBuff[4], gwDebugBuff[5],
                              gwDebugBuff[6], gwDebugBuff[7]);
            SendDlgItemMessage(ghwndDebug, IDC_EDIT_DEBUG_OUTPUT, LB_ADDSTRING, 0, (LPARAM)(LPSTR)szBuff );
            giDebugIndx = -1;
        }
        break;
  }

	UpdateWindow(ghwndDebug);

	return;

}

//================================================
// Debug Dialog Procedure
//
//=================================================
BOOL CALLBACK DebugProc(HWND hWndDlg, UINT wMessage, WPARAM wParam, LPARAM lParam)
{

switch (wMessage)
    {
    case WM_INITDIALOG:

        return FALSE;
        break;
    case WM_CLOSE:
    case WM_QUIT:
        ShowDebugInfo(FALSE, 0, 0l);
        break;
    }
return FALSE;
}


/////////////////////////////////
// NOT USED YET

static WORD wShiftKeyFlag = 0;

void SetShiftKeyFlag(BOOL bSet)
{

	switch(bSet)
	{
		case TRUE:
			wShiftKeyFlag |= wShiftKeyFlag;
			break;
		
		case FALSE:
			wShiftKeyFlag &= ~VK_SHIFT;
			break;
	}

}

BOOL GetShiftKeyState(void)
{
	BOOL bRet;

	if(wShiftKeyFlag & VK_SHIFT)
		bRet = TRUE;
	else
		bRet = FALSE;

	return bRet;

}

WORD GetShiftKeyFlag(void)
{
	return wShiftKeyFlag;

}


//======================================================================
// 
//======================================================================

void ExtractFileName(char * szPathAndFilename, char * szFilename)
{
	int i;

// find begining of filename ex: c:\windows\arches.bmp by searching
// backwards for the slash
	
	i = lstrlen(szPathAndFilename) - 1;
	while( (i) && (szPathAndFilename[i] !='\\') ) i--;
	if(szPathAndFilename[i] != '\\')
		lstrcpy(szFilename, szPathAndFilename+i);
	else
		lstrcpy(szFilename, szPathAndFilename+i+1);

}
  

LPSTR GetLastMixFileName(void)
{
	// Registry variables 

	LONG lnResult;
	HKEY hKey = NULL;
	DWORD dwDisposition;
	static LPCTSTR szRegKey = "Software\\CorTek\\VACS"; 
	static LPCTSTR szRegValue; 
	DWORD dwType;
	DWORD rc;
	char	szTempSeq[MAX_PATH];
	DWORD dwBufferSize = sizeof( szTempSeq );  

	static char			szText[MAX_PATH] = "LA$T.mix";

	char szScratch[MAX_PATH];

						////////////////////////////////////
						// Get the last mix file that was
						// saved and set the global filename
						// structure

						lnResult = RegCreateKeyEx(
												HKEY_CURRENT_USER,
												szRegKey,
												0, 
												REG_NONE, 
												0, 
												KEY_ALL_ACCESS, 
												NULL, 
												&hKey,
												&dwDisposition );

						szRegValue = "MRUmix";
						rc = RegQueryValueEx( 
									hKey, 
									szRegValue, 
									NULL, 
									&dwType, 
									&szTempSeq, 
									&dwBufferSize ); 

						// Set our global filename name to the last loaded mix file
						// so that when we ask them to save the mix file it is the
						// defailt name 

						if(rc == ERROR_SUCCESS)
						{
							ExtractFileName(szTempSeq, szScratch);	// get the filename
							return &szScratch[0];
						}
						else
							return &szText[0];

}