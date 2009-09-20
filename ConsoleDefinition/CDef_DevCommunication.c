////////////////////////////////////////////////////////////////////
// file CDef_DevCommunication.c
//
//
#include <windows.h>

#define _CONSOLE_DEFINITION_FILE_

#include "DCXTCP_protocol.h"
#define CONTROLDATA      // This is very dumb but for some reason CONTROLDATA gets redefinition ERROR
#include  "DCXParser.h"
#undef CONTROLDATA
#include "ConsoleDefinition.h"
#include "CDef_External.h"
#include "ConsoleDefinitionRC.h"


HANDLE		ghBinFile = NULL;
int       giLeftBytesToReceive = 0;
int       giNetBufferOfset = 0;
char      gacNetBuffer[MAX_NET_BUFFER];
BOOL      gbReadHeader = TRUE;


////////////////////////////////////////////////////////////////////////////////////
// FUNCTION: tcpThreadProc
//
// thread function for the IO stuff or other callback messages
//
DWORD WINAPI ioThreadProc(LPVOID lpv)
{
MSG   msg;
HWND  hwnd;
int   status;

	// Start the WINSOCK 2

	if ((status = WSAStartup(MAKEWORD(1,1), &g_WSAData)) == 0) 
		{
    ; // Do something here
		}
	else 
		{
		g_threadIO = NULL;
		return 1; // Error
		//wsprintf(szBuff, "%d is the err from WSAStartup", status);
		//MessageBox( g_hwndMain, szBuff, "Error", MB_OK);
		}


	// Create the IO window

	if( ioCreateWindow() )
	  {
	  g_threadIO = NULL;
	  return 1; // Error
	  }

	// Force the system to Create the Message Queue

	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

	while ((GetMessage(&msg, g_hwndIO, 0, 0) == TRUE) && (g_hwndIO != NULL))
	{
		if (msg.message == WM_CLOSE || msg.message == WM_DESTROY || 
				msg.message == WM_QUIT)
			break;

	  TranslateMessage(&msg);
	  DispatchMessage(&msg);
	}

	if(g_hwndIO)
	  {
	  hwnd = g_hwndIO;
	  g_hwndIO = NULL;
	  DestroyWindow(hwnd);
	  }


	// Cleanup the WINSOCK 2

	if(g_socket != INVALID_SOCKET)
	  closesocket( g_socket );      
	g_socket = INVALID_SOCKET;
	WSACleanup();

	g_threadIO = NULL;
	return 0;
}; 

/////////////////////////////////////////////////////////////////////////////////////
// FUNCTION: ioCreateWindow
//
//
int   ioCreateWindow(void)
{
WNDCLASS    wc;
int         iReturn;

	// Register Main Window Class
	//---------------------------
	ZeroMemory(&wc, sizeof(WNDCLASS));      // Clear wndclass structure

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNCLIENT;
	wc.lpfnWndProc = (WNDPROC) ioWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = g_hinstCDef;
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_szIOWndClass;

	iReturn = RegisterClass(&wc);

	if(iReturn == 0)
		 return 1;     // Error... Exit

	// Create Main Window
	//-------------------
	g_hwndIO = CreateWindow(
						 g_szIOWndClass,         // Window class name
						 NULL,                   // Window's title
						 WS_POPUP,               // Popup Style
						 CW_USEDEFAULT,          // Use default x
						 CW_USEDEFAULT,          // Use default y
						 CW_USEDEFAULT,          // Use default Width
						 CW_USEDEFAULT,          // Use default Height
						 NULL,                   // Parent window's handle
						 NULL,                   // Default to Class Menu
						 g_hinstCDef,            // Instance of ConsoleDefinition DLL
						 NULL);                  // Ptr To Data Structure For WM_CREATE

	if(g_hwndIO  == NULL)
		 return 1;      // Error... Exit


	return 0; // OK
}


void    HandleTcpWriteMessage(SOCKET s)
{
//  int     status;

  return;
}


void    HandleTcpReadMessage(SOCKET s)
{
  int     status;

  int   iCurErr;
  char  szBuffer[64];


  if( gbReadHeader == TRUE )
  {
    status = ReadRawTcpData(s, sizeof(g_hdrDCXTCP));
    if( status == sizeof(g_hdrDCXTCP))
    {
      // Ok we've got a full header
      //
      CopyMemory(&g_hdrDCXTCP, gacNetBuffer, sizeof(g_hdrDCXTCP));
      giLeftBytesToReceive = 0;

      // perform an error check here
      //
    	if( (g_hdrDCXTCP.iID == DCX_TCP_ID ) && (g_hdrDCXTCP.wSize <= MAX_NET_BUFFER))
      {
        if(g_hdrDCXTCP.wSize == 0)
          ProcessTcpData(); // We got the entire packet ... the header itself is the packet !!!!!
        else
				{
          gbReadHeader = FALSE; // ok we are not waiting for header any more
                                // but we need more data to finish the packet !!!
					giLeftBytesToReceive = g_hdrDCXTCP.wSize;
				}
      }
      else
        gbReadHeader = TRUE; // ok we are not waiting for header any more

    }
//  else
//    gbReadHeader = TRUE; // ok we are not waitin for header any more for DEBUG ONLY
  }
  else
  {
    if( giLeftBytesToReceive > 0 )
    {
      status = ReadRawTcpData(s, giLeftBytesToReceive);
      if(giLeftBytesToReceive == 0)
        status = g_hdrDCXTCP.wSize;
    }
    else
      status = ReadRawTcpData(s, g_hdrDCXTCP.wSize);

    // if we got the full packet ... go ahead and deal with it
    // and also set the flag to wait fo a header
    //
    if(status == g_hdrDCXTCP.wSize)
    {
      ProcessTcpData();
      giLeftBytesToReceive = 0;
      giNetBufferOfset = 0;
      gbReadHeader = TRUE;
    }
		else
			if(status == 0) // forced resynk
			{
				giLeftBytesToReceive = 0;
				giNetBufferOfset = 0;
				gbReadHeader = TRUE;
			}
  }

	
  if( status == SOCKET_ERROR )
	{
	  //SetDlgItemText(g_hwndMain, IDC_STAT_12, "TCP ERROR-> Connection broken!");

		// check error value here

    iCurErr = WSAGetLastError();
    FormatSockError(szBuffer, iCurErr);

		if(iCurErr == WSAEWOULDBLOCK || iCurErr == WSAEFAULT || 
			 iCurErr == WSAEINTR || iCurErr == WSAEINPROGRESS || iCurErr == WSAEMSGSIZE)
			iCurErr = 0;
		else		// its REALLY and error, close it down.
		{
			closesocket(g_socket);
			giLeftBytesToReceive = 0;
			giNetBufferOfset = 0;
			gbReadHeader = TRUE;

			g_socket = INVALID_SOCKET;
		}

	}
  return;
}



//===============================================
// IO window procedure
//===============================================
LRESULT CALLBACK  ioWndProc(HWND hwnd, UINT uiMessage, WPARAM wP, LPARAM  lP)
{
//int     status;

	switch (uiMessage)
    {
    //---------------------------------------------------------------------
    case WSA_READ:
      {
      switch (WSAGETSELECTEVENT(lP))
        {
        case FD_READ:
          HandleTcpReadMessage((SOCKET)wP);
          break;
        case FD_WRITE:
          HandleTcpWriteMessage((SOCKET)wP);
          break;
        default:
		      closesocket(g_socket);
		      g_socket = INVALID_SOCKET;
        break;
        };
/*
		    if (WSAGETSELECTEVENT(lP) == FD_READ) 
        {
	        if( recv((SOCKET)wP, (char *)&g_hdrDCXTCP, sizeof(g_hdrDCXTCP), MSG_PEEK ) == 
              sizeof(g_hdrDCXTCP))
            status = ReadRawTcpData((SOCKET)wP, (LPSTR)&g_hdrDCXTCP, sizeof(g_hdrDCXTCP));
          else
            status = 1;
		      
          if (status) 
			    {
            if(status == sizeof(g_hdrDCXTCP))
			        ReadTcpData((SOCKET)wP);
			    }
		      else
			    {
			    // we need to check this value here
			    // but in most cases it will be ERROR to close the socket all together

			    //SetDlgItemText(g_hwndMain, IDC_STAT_12, "TCP ERROR-> Connection broken!");
			    closesocket(g_socket);
			    g_socket = INVALID_SOCKET;
			    }
	      }
		    else 
	      {    // FD_CLOSE -- connection dropped 
			       // "TCP -> connection dropped !");
		      closesocket(g_socket);
		      g_socket = INVALID_SOCKET;
	      }
*/
      }
      break;    

    case WM_GET_SERVER_SETUP_DATA:
      CDef_RequestServerSetupData();
      break;

    case WM_GET_SERVER_TABLE_DATA:
      CDef_RequestServerTableData();
      break;

    case WM_CREATE:
		break;

	  case WM_CLOSE:
	//  case WM_QUIT:
	//  case WM_DESTROY:
		if(g_hwndIO)
		  {
		  PostQuitMessage(0);
		  }
		break;

	  default:
		return DefWindowProc(hwnd, uiMessage, wP, lP);
	  }

	return (0L);
}


//////////////////////////////////////////////////////////////////
//
//
int   StartClient(HWND  hwnd)
{
int           iRet = 0;
SOCKADDR_IN   dest_sin;  
char          szbuff[128];
int           status;
int           iSockOpSz;
int           iInBuffSz;
BOOL          bOpt;

	if(g_socket != INVALID_SOCKET)
	  {
	  closesocket( g_socket );      
	  g_socket = INVALID_SOCKET;
    Sleep(10);
	  }

	g_dwBReceived = 0;
	g_dwBSent = 0;
  
  // how many bytes of a packet are left to be read
  //
  giLeftBytesToReceive = 0;
  giNetBufferOfset = 0;
  gbReadHeader = TRUE;

  ZeroMemory(gacNetBuffer, MAX_NET_BUFFER);


	g_uiNetAddr = inet_addr(g_pref.szIP_Address);
	if(g_uiNetAddr== INADDR_NONE)     
	  {
	  SetDlgItemText(hwnd, IDC_STAT_1, "Invalid TCP Address!");
	  return 1;  
	  }

	g_socket = socket( AF_INET, SOCK_STREAM, 0);
	if( g_socket == INVALID_SOCKET ) 
	  {
	  SetDlgItemText(hwnd, IDC_STAT_1, "socket() failed");
	  return 1;
	  }

  // initilaize the socket header for rading
  //
	g_hdrDCXTCP.wMessage = 0;
  
  // Set different things ...
	//-------------------------
	iSockOpSz = sizeof(int);
	getsockopt(g_socket, SOL_SOCKET, SO_RCVBUF, (LPSTR)&iInBuffSz, &iSockOpSz); 
  iInBuffSz = 16384;
  setsockopt(g_socket, SOL_SOCKET, SO_RCVBUF, (LPSTR)&iInBuffSz, iSockOpSz);
	getsockopt(g_socket, SOL_SOCKET, SO_RCVBUF, (LPSTR)&iInBuffSz, &iSockOpSz); 

  
  
  iSockOpSz = sizeof(BOOL);
  bOpt = TRUE;
  //setsockopt(g_socket, SOL_SOCKET, SO_DONTLINGER , (LPSTR)&bOpt, iSockOpSz); 
  bOpt = TRUE;
  setsockopt(g_socket, SOL_SOCKET, TCP_NODELAY  , (LPSTR)&bOpt, iSockOpSz); 

	dest_sin.sin_family           = AF_INET;
	dest_sin.sin_port             = htons((u_short)g_pref.iPort);
	dest_sin.sin_addr.S_un.S_addr = g_uiNetAddr;


	if( connect( g_socket, (PSOCKADDR) &dest_sin, sizeof( dest_sin)) < 0 ) 
	  {
	  closesocket( g_socket );
	  g_socket = INVALID_SOCKET;
	  SetDlgItemText(hwnd, IDC_STAT_1, "Connection failed! Error");
	  return 1;
	  }

	SetDlgItemText(hwnd, IDC_STAT_1, "Connection worked! Success!");

	//
	//   Send main window a WSA_READ when either data is pending on
	//   the socket (FD_READ) or the connection is closed (FD_CLOSE)
	//
	if ((status = WSAAsyncSelect( g_socket, g_hwndIO, WSA_READ, FD_READ | FD_WRITE | FD_CLOSE )) > 0) 
	  {
	  wsprintf(szbuff, "Error WSAAsyncSelect=%d (0x%x)", status, WSAGetLastError());
	  SetDlgItemText(hwnd, IDC_STAT_1, szbuff);
	  closesocket( g_socket );
	  g_socket = INVALID_SOCKET;
	  return 1;
	  }

  // general lockout variable for the timer in the Server connect Window
  //
  g_bTimerLockOut = FALSE;
	return iRet;
}

////////////////////////////////////////////////////////////////////////////
// FUNCTION: ReadRawTcpData
//
// purpose: Read raw TCP data + implement partial reads for buffers
//          that are comming slow over the network
// 
// parms:
//  SOCKET    s;         // Socket to read from
//  LPSTR     pBuffer;   // pointer to the buffer
//  int       iReadSize; // how much to read
//                       // NOTE: The buffer should be big enough. This
//                                Function will not perform any cheks
//                                for memory overruns
//
// return:
//  int       iRead; // Number of bytes actualy read into the buffer pBuffer 
//
#define MAX_TCP_COUNTDOWN	100
int   ReadRawTcpData(SOCKET s, int iReadSize)
{
  int     iCurRead = 0, bytesread = 0;
	int			icnt = 0;
	int			iCurErr;

  if( iReadSize > 0)
  {
		/*	the original ... hmm ... maybe it wasn't so good after all
		iCurRead = recv(s, &gacNetBuffer[giNetBufferOfset], iReadSize, NO_FLAGS_SET );
		if(iCurRead != SOCKET_ERROR)
		{

			// Adjust the offsets for the data that didn't get here
			//
			giLeftBytesToReceive = iReadSize - iCurRead;

			if(giLeftBytesToReceive > 0)
			{
				giNetBufferOfset += iCurRead;
			}
			else
				giNetBufferOfset = 0;
		*/
		giLeftBytesToReceive = iReadSize;
		do
		{
			iCurRead = recv(s, &gacNetBuffer[giNetBufferOfset], giLeftBytesToReceive, NO_FLAGS_SET );
			if(iCurRead != SOCKET_ERROR)
			{

				// Adjust the offsete for the data that didn't get here
				//
				giLeftBytesToReceive -= iCurRead;
				bytesread += iCurRead;

				if(giLeftBytesToReceive > 0)
				{
					giNetBufferOfset += iCurRead;
				}
				else
					giNetBufferOfset = 0;
			}
			else
			{
				iCurErr = WSAGetLastError();

				if(iCurErr == WSAEWOULDBLOCK || iCurErr == WSAEFAULT || 
					 iCurErr == WSAEINTR || iCurErr == WSAEINPROGRESS || iCurErr == WSAEMSGSIZE)
					iCurErr = 0;
				else
				{
					iCurRead = iCurErr;
					break; // SOCKET_ERROR
				}
			}
			icnt ++;
		}		while(giLeftBytesToReceive > 0 && icnt < MAX_TCP_COUNTDOWN);

		if(iCurRead != SOCKET_ERROR)
		{
			if(giNetBufferOfset > 0)
				iCurRead = bytesread;
			else
				iCurRead = iReadSize;
		}
  }
  return iCurRead;
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION: ProcessTcpData
//
//
//
int     ProcessTcpData()
{
	DWORD   dwWrite;
	int     iRec = 0;

	if( g_hdrDCXTCP.iID != DCX_TCP_ID )
	  return 1;

	switch(g_hdrDCXTCP.wMessage)
	  {
    ////////////////////////////////////////////
    case DCX_VU_DATA:
      g_xcomm_main(DCX_VU_DATA, g_hdrDCXTCP.wSize, gacNetBuffer, NULL);
      break;

    case DCX_TCP_CONTROL_DATA:
      g_xcomm_main(DCX_TCP_CONTROL_DATA, g_hdrDCXTCP.wSize, gacNetBuffer, NULL);
    break;


	  case  DCX_TCP_TIME:                  
		  // Display the Time
		  if( g_xcomm_main )
		    g_xcomm_main(CDEF_SHOW_TIME, 0, &g_hdrDCXTCP.mmt, NULL);

		break;
	  case DCX_TCP_RECEIVE_SETUP:
      // Copy the data into the Application defined buffer ...
      //
      CopyMemory(g_pDeviceSetupData->iaChannelTypes, gacNetBuffer, g_hdrDCXTCP.wSize);

      g_bServerSetupData = TRUE;
      g_bTimerLockOut = FALSE; // used for the Timer in the Server Data Window
      
      break;
	  ////////////////////////////////
	  case DCX_TCP_RECEIVE_TABLE:
		  // Open a file to spool to ...
      //
		  if(ghBinFile == NULL)
		  {                         
        // use the preferences file name to update ..
        //
        ghBinFile = CreateFile(g_pref.szTableFileName, GENERIC_READ | GENERIC_WRITE, 
								                0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		    if(ghBinFile == INVALID_HANDLE_VALUE)
			    ghBinFile = NULL;          
		    //MessageBeep(0xFFFFFFFF);
		  }


    if(ghBinFile)
			WriteFile(ghBinFile, gacNetBuffer, g_hdrDCXTCP.wSize, &dwWrite, NULL);

		break;
	  /////////////////////////////////////////
	  case DCX_TCP_RECEIVE_TABLE_DONE:
		  // Display the Time
		  if( ghBinFile )
		  {
		    CloseHandle(ghBinFile);
		    ghBinFile = NULL;
		    //MessageBeep(0xFFFFFFFF);
		  }
      g_bServerTableData = TRUE;
      g_bTimerLockOut = FALSE; // used for the Timer in the Server Data Window

      break;
    ////////////////////////////////////////////
    default:
      giLeftBytesToReceive = 0;
      giNetBufferOfset = 0;
      gbReadHeader = TRUE;
      break;

	  }

  
	return iRec;
}