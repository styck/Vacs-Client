// CommPort.cpp: implementation of the CCommPort class.
//
//////////////////////////////////////////////////////////////////////
#define DLL_EXPORT_COMM
#include <windows.h>
#include "CommPort.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

COMMDEVICE  m_cm = {0};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL    IsOpen(){return m_cm.m_bConnected;};
BOOL    SetDataInWindow(HWND hwnd){m_cm.m_hwndDataIn = hwnd; return TRUE;};


void	SendCommData(BYTE		ch, BYTE b1, BYTE b2)
{
	MIDI_SHORT_DATA	mm;

	mm.data=0;

	mm.s.stat = 0x0b;
	mm.s.chan = ch;
	mm.s.d1 = b1;
	mm.s.d2 = b2;

	WriteCommBlock((LPSTR)&mm.data, 3);
}

///////////////////////
//
int	GetCommInFifoSize()
{
	return GetDataSize(&m_cm.m_InFifo);
}

int GetCommInFifo(LPSTR p, int size)
{
	return GetFifo(&m_cm.m_InFifo, p, size);
}

/////////////////////
//
//
void SetupGlobal()
{
	m_cm.m_hDevice = NULL;
	m_cm.m_hIOThread = NULL;
	m_cm.m_dwThreadID = 0;

	ZeroMemory(m_cm.m_chDevName, MAX_PATH);

	m_cm.m_iInBuffSize = 4096;
	m_cm.m_iOutBuffSize = 4096;

	// Device Settings
	m_cm.m_dwBaudRate = CBR_38400;
	m_cm.m_btByteSize = 8;
	m_cm.m_btParity   = ODDPARITY;  
	m_cm.m_btStopBits = ONESTOPBIT;


	m_cm.m_btFlowCtrl = 0;

	// Stats
	m_cm.m_bConnected = FALSE;
	m_cm.m_bXonXoff = FALSE; 
	m_cm.m_bLocalEcho = FALSE;
	m_cm.m_bUseCNReceive = FALSE; 
	m_cm.m_bDisplayErrors = TRUE;


	// IO stuff

	m_cm.osWrite.Offset = 0 ;
	m_cm.osWrite.OffsetHigh = 0 ;
	m_cm.osRead.Offset = 0 ;
	m_cm.osRead.OffsetHigh = 0 ;

};

int     GetInData(LPSTR lps)
{ 
	int iRet=0;
	//CopyMemory(lps, m_cm.m_chInBuff, m_cm.m_iInLength);
	//iRet = m_cm.m_iInLength;
	//m_cm.m_iInLength = 0;
	return iRet;
};


////////////////////////////////////////////////////////////////////
//
//
BOOL    MyOpenCommPort(LPSTR lpsName)
{
	lstrcpy(m_cm.m_chDevName, lpsName);
	return OpenComm();
};

////////////////////////////////////////////////////////////////////
//
//
BOOL    OpenComm(void)
{
	BOOL          bRet = FALSE;
	COMMTIMEOUTS  CommTimeOuts ;

	CloseComm();

	// open COMM device
	if ((m_cm.m_hDevice = CreateFile( m_cm.m_chDevName, 
		GENERIC_READ | GENERIC_WRITE,
		0,                    // exclusive access
		NULL,                 // no security attrs
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL |
		FILE_FLAG_OVERLAPPED, // overlapped I/O
		NULL )) != INVALID_HANDLE_VALUE)
	{
		// get any early notifications
		//----------------------------
		SetCommMask(m_cm.m_hDevice, EV_RXCHAR ) ;

		// setup device buffers
		//---------------------
		SetupComm( m_cm.m_hDevice, m_cm.m_iInBuffSize, m_cm.m_iOutBuffSize) ;

		// purge any information in the buffer
		//------------------------------------
		PurgeComm( m_cm.m_hDevice,  PURGE_TXABORT | PURGE_RXABORT |
			PURGE_TXCLEAR | PURGE_RXCLEAR ) ;

		// set up for overlapped I/O
		//--------------------------
		CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF ;
		CommTimeOuts.ReadTotalTimeoutMultiplier = 0 ;
		CommTimeOuts.ReadTotalTimeoutConstant = 1000 ;
		// CBR_9600 is approximately 1byte/ms. For our purposes, allow
		// double the expected time per character for a fudge factor.
		CommTimeOuts.WriteTotalTimeoutMultiplier = 2*CBR_9600/ m_cm.m_dwBaudRate;
		CommTimeOuts.WriteTotalTimeoutConstant = 0 ;
		SetCommTimeouts( m_cm.m_hDevice, &CommTimeOuts ) ;
	}

	bRet = SetupConnection() ;

	if (bRet)
	{
		// create I/O event used for overlapped reads / writes

		m_cm.osRead.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL ) ;
		if (m_cm.osRead.hEvent == NULL)
		{
			// For now we are doing nothing here ....
		}
		m_cm.osWrite.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL ) ;
		if ( m_cm.osWrite.hEvent == NULL)
		{
			//  For now we are doing nothing here .... 
		}

		m_cm.m_hInClose = CreateEvent(NULL, TRUE, FALSE, NULL );
		m_cm.m_hInClosed = CreateEvent(NULL, TRUE, FALSE, NULL );
		SetupFifo(&m_cm.m_InFifo);
		SetupFifo(&m_cm.m_OutFifo);

		m_cm.m_bConnected = TRUE ;

		// Create a secondary thread
		// to watch for an event.

		if (NULL == (m_cm.m_hIOThread =
			CreateThread( (LPSECURITY_ATTRIBUTES) NULL,
			0,
			(LPTHREAD_START_ROUTINE) CommIOThreadProc,
			(LPVOID) &m_cm,
			0, &m_cm.m_dwThreadID)))
		{
			m_cm.m_bConnected = FALSE ;
			CloseHandle( m_cm.m_hDevice); m_cm.m_hDevice = NULL;
			bRet = FALSE ;
		}
		else
		{

			// assert DTR
			//-----------
			EscapeCommFunction( m_cm.m_hDevice, SETDTR ) ;
		}
	}
	else
	{
		m_cm.m_bConnected = FALSE ;
		CloseHandle( m_cm.m_hDevice ) ; m_cm.m_hDevice = NULL;
	}

	return bRet;
}


////////////////////////////////////////////////////////////////////
//
//
BOOL    CloseComm()
{
	BOOL    bRet = TRUE;


	if(m_cm.m_hDevice)
	{
		// set connected flag to FALSE
		//----------------------------
		m_cm.m_bConnected = FALSE;

		// disable event notification and wait for thread
		// to halt
		//------------------------------------------------
		SetCommMask( m_cm.m_hDevice, 0 ) ;

		ResetEvent(m_cm.m_hInClosed); // get ready to pend on this Event
		SetEvent(m_cm.m_hInClose);

		WaitForSingleObject(m_cm.m_hInClosed, INFINITE);

		// block until thread has been halted
		//while(m_cm.m_dwThreadID != 0) Sleep(10);

		CloseHandle(m_cm.m_hInClose);
		CloseHandle(m_cm.m_hInClosed);


		// drop DTR
		//---------
		EscapeCommFunction( m_cm.m_hDevice, CLRDTR ) ;

		// purge any outstanding reads/writes and close device handle
		//-----------------------------------------------------------
		PurgeComm( m_cm.m_hDevice, PURGE_TXABORT | PURGE_RXABORT |
			PURGE_TXCLEAR | PURGE_RXCLEAR ) ;
		CloseHandle( m_cm.m_hDevice) ;

		m_cm.m_hDevice = NULL;

		CloseHandle(m_cm.osWrite.hEvent);
		CloseHandle(m_cm.osRead.hEvent);
		m_cm.osWrite.Offset = 0 ;
		m_cm.osWrite.OffsetHigh = 0 ;
		m_cm.osRead.Offset = 0 ;
		m_cm.osRead.OffsetHigh = 0 ;

		DestroyFifo(&m_cm.m_InFifo);
		DestroyFifo(&m_cm.m_OutFifo);
	}


	return bRet;
}


////////////////////////////////////////////////////////////////////
//
//
BOOL SetupConnection()
{
	BOOL       bRet = FALSE;
	BYTE       bSet;
	DCB        dcb;
	//NPTTYINFO  npTTYInfo ;

	//if (NULL == (npTTYInfo = GETNPTTYINFO( hWnd )))
	//  return ( FALSE ) ;

	dcb.DCBlength = sizeof( DCB ) ;

	GetCommState( m_cm.m_hDevice, &dcb ) ;

	dcb.BaudRate  = m_cm.m_dwBaudRate;
	dcb.ByteSize  = m_cm.m_btByteSize;
	dcb.Parity    = m_cm.m_btParity;
	dcb.StopBits  = m_cm.m_btStopBits;

	// setup hardware flow control

	bSet = (BYTE) ((m_cm.m_btFlowCtrl & FC_DTRDSR) != 0) ;

	dcb.fOutxDsrFlow = bSet ;
	if (bSet)
		dcb.fDtrControl = DTR_CONTROL_HANDSHAKE ;
	else
		dcb.fDtrControl = DTR_CONTROL_ENABLE ;

	bSet = (BYTE) ((m_cm.m_btFlowCtrl & FC_RTSCTS) != 0) ;

	dcb.fOutxCtsFlow = bSet ;
	if (bSet)
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE ;
	else
		dcb.fRtsControl = RTS_CONTROL_ENABLE ;

	// setup software flow control
	//----------------------------
	bSet = (BYTE) ((m_cm.m_btFlowCtrl & FC_XONXOFF) != 0) ;

	dcb.fInX = dcb.fOutX = bSet ;
	dcb.XonChar = ASCII_XON ;
	dcb.XoffChar = ASCII_XOFF ;
	dcb.XonLim = 100 ;
	dcb.XoffLim = 100 ;

	// other various settings
	//-----------------------
	dcb.fBinary = TRUE ;
	dcb.fParity = TRUE ;

	bRet = SetCommState( m_cm.m_hDevice, &dcb ) ;

	return bRet ;
} // end of SetupConnection()

////////////////////////////////////////////////////////////////
//
//
BOOL    WriteCommBlock(LPSTR lpBlock, DWORD dwLength)
{
	BOOL        bRet = TRUE;

	BOOL        bWriteStat ;
	DWORD       dwBytesWritten ;
	DWORD       dwErrorFlags;
	DWORD   	  dwError;
	DWORD       dwBytesSent=0;
	COMSTAT     ComStat;
	char        szError[ 128 ] ;



	bWriteStat = WriteFile( m_cm.m_hDevice, lpBlock, dwLength,
		&dwBytesWritten, &m_cm.osWrite ) ;

	// Note that normally the code will not execute the following
	// because the driver caches write operations. Small I/O requests
	// (up to several thousand bytes) will normally be accepted
	// immediately and WriteFile will return true even though an
	// overlapped operation was specified

	if (!bWriteStat)
	{
		if(GetLastError() == ERROR_IO_PENDING)
		{
			// We should wait for the completion of the write operation
			// so we know if it worked or not

			// This is only one way to do this. It might be beneficial to
			// place the write operation in a separate thread
			// so that blocking on completion will not negatively
			// affect the responsiveness of the UI

			// If the write takes too long to complete, this
			// function will timeout according to the
			// CommTimeOuts.WriteTotalTimeoutMultiplier variable.
			// This code logs the timeout but does not retry
			// the write.

			while(!GetOverlappedResult( m_cm.m_hDevice,
				&m_cm.osWrite, &dwBytesWritten, TRUE ))
			{
				dwError = GetLastError();
				if(dwError == ERROR_IO_INCOMPLETE)
				{
					// normal result if not finished
					dwBytesSent += dwBytesWritten;
					continue;
				}
				else
				{
					// an error occurred, try to recover
					wsprintf( szError, "<CE-%u>", dwError ) ;
					//WriteTTYBlock( hWnd, szError, lstrlen( szError ) ) ;
					ClearCommError( m_cm.m_hDevice, &dwErrorFlags, &ComStat ) ;
					if ((dwErrorFlags > 0) && m_cm.m_bDisplayErrors)
					{
						wsprintf( szError, "<CE-%u>", dwErrorFlags ) ;
						//WriteTTYBlock( hWnd, szError, lstrlen( szError ) ) ;
					}
					break;
				}
			}
			dwBytesSent += dwBytesWritten;

			if( dwBytesSent != dwLength )
				wsprintf(szError,"\nProbable Write Timeout: Total of %ld bytes sent", dwBytesSent);
			else
				wsprintf(szError,"\n%ld bytes written", dwBytesSent);

			OutputDebugString(szError);

		}
		else
		{
			// some other error occurred
			ClearCommError( m_cm.m_hDevice, &dwErrorFlags, &ComStat ) ;
			if ((dwErrorFlags > 0) && m_cm.m_bDisplayErrors)
			{
				wsprintf( szError, "<CE-%u>", dwErrorFlags ) ;
				//WriteTTYBlock( hWnd, szError, lstrlen( szError ) ) ;
			}
			bRet = FALSE;
		}
	}

	return bRet;
};


////////////////////////////////////////////////////////////////////////
//
//
BOOL    SetupCommPort(void)
{
	return TRUE;
};

///

////////////////////////////////////////////////////////////////////////
//
//
//    HELPER FUNCTIONS
//
//

//---------------------------------------------------------------------------
//  int NEAR ReadCommBlock( HWND hWnd, LPSTR lpszBlock, int nMaxLength )
//
//  Description:
//     Reads a block from the COM port and stuffs it into
//     the provided buffer.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//     LPSTR lpszBlock
//        block used for storage
//
//     int nMaxLength
//        max length of block to read
//
//  Win-32 Porting Issues:
//     - ReadComm() has been replaced by ReadFile() in Win-32.
//     - Overlapped I/O has been implemented.
//
//---------------------------------------------------------------------------
int     ReadCommBlock( LPCOMMDEVICE  pcd, LPSTR lpszBlock, int nMaxLength )
{
	BOOL       bReadStat ;
	COMSTAT    ComStat ;
	DWORD      dwErrorFlags;
	DWORD      dwLength;
	DWORD      dwError;
	char       szError[ 10 ] ;

	// only try to read number of bytes in queue
	ClearCommError( pcd->m_hDevice, &dwErrorFlags, &ComStat ) ;
	dwLength = min( (DWORD) nMaxLength, ComStat.cbInQue ) ;

	if (dwLength > 0)
	{
		bReadStat = ReadFile( pcd->m_hDevice, lpszBlock,
			dwLength, &dwLength, &pcd->osRead) ;
		if (!bReadStat)
		{
			if (GetLastError() == ERROR_IO_PENDING)
			{
				OutputDebugString("\n\rIO Pending");
				// We have to wait for read to complete.
				// This function will timeout according to the
				// CommTimeOuts.ReadTotalTimeoutConstant variable
				// Every time it times out, check for port errors
				while(!GetOverlappedResult( pcd->m_hDevice,
					&pcd->osRead, &dwLength, TRUE ))
				{
					dwError = GetLastError();
					if(dwError == ERROR_IO_INCOMPLETE)
						// normal result if not finished
						continue;
					else
					{
						// an error occurred, try to recover
						wsprintf( szError, "<CE-%u>", dwError ) ;
						//WriteTTYBlock( hWnd, szError, lstrlen( szError ) ) ;
						ClearCommError( pcd->m_hDevice, &dwErrorFlags, &ComStat ) ;
						if ((dwErrorFlags > 0) && pcd->m_bDisplayErrors)
						{
							wsprintf( szError, "<CE-%u>", dwErrorFlags ) ;
							//WriteTTYBlock( hWnd, szError, lstrlen( szError ) ) ;
						}
						break;
					}
				}
			}
			else
			{
				// some other error occurred
				dwLength = 0 ;
				ClearCommError( pcd->m_hDevice, &dwErrorFlags, &ComStat ) ;
				if ((dwErrorFlags > 0) && pcd->m_bDisplayErrors)
				{
					wsprintf( szError, "<CE-%u>", dwErrorFlags ) ;
					//WriteTTYBlock( hWnd, szError, lstrlen( szError ) ) ;
				}
			}
		}
	}

	return ( dwLength ) ;
} // end of ReadCommBlock()



////////////////////////////////////////////////////////////
//
//
//
UINT    CommIOThreadProc(LPVOID lpv)
{
	DWORD       dwRet = 0;
	DWORD       dwEvtMask ;
	OVERLAPPED  os ;
	int         nLength = 0;
	char        abIn[ MAXBLOCK + 1] ;
	LPCOMMDEVICE  pcd = (LPCOMMDEVICE)lpv; // Pointer to Comm Device Structure


	FillMemory( &os, 0, sizeof( OVERLAPPED ) ) ;

	// create I/O event used for overlapped read
	//------------------------------------------
	/*
	os.hEvent = CreateEvent( NULL,    // no security
	TRUE,    // explicit reset req
	FALSE,   // initial event reset
	NULL ) ; // no name
	if (os.hEvent == NULL)
	{
	MessageBox( NULL, "Failed to create event for thread!", "TTY Error!",
	MB_ICONEXCLAMATION | MB_OK ) ;
	goto ON_EXIT;
	}
	*/

	if (!SetCommMask( pcd->m_hDevice, EV_RXCHAR ))
		goto ON_EXIT;

	while ( TRUE ) //pcd->m_bConnected )
	{
		// do we have to close ...
		if(WaitForSingleObject(pcd->m_hInClose, 0) == WAIT_OBJECT_0)
		{
			break;  
		}
		Sleep(1);

		dwEvtMask = 0 ;
		WaitCommEvent( pcd->m_hDevice, &dwEvtMask, &pcd->osRead );

		if ((dwEvtMask & EV_RXCHAR) == EV_RXCHAR)
		{
			do
			{
				nLength = ReadCommBlock(pcd, (LPSTR) abIn, MAXBLOCK);
				if(nLength)
				{
					PutFifo(&pcd->m_InFifo, abIn, nLength);
					if(pcd->m_hwndDataIn)
						PostMessage(pcd->m_hwndDataIn, WM_U_COMIO_IN_DONE, 0, 0);
				}

			} while ( nLength > 0 ) ;
		}
	}

	// clear information in structure (kind of a "we're done flag")

ON_EXIT:
	pcd->m_dwThreadID = 0 ;
	pcd->m_hIOThread = NULL ;
	SetEvent(pcd->m_hInClosed);
	return dwRet;
}


