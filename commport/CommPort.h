// CommPort.h: interface for the CCommPort class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMPORT_H__483D324E_42B6_11D1_A39D_000000000000__INCLUDED_)
#define AFX_COMMPORT_H__483D324E_42B6_11D1_A39D_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CommFifo.h"


// Flow control flags

#define FC_DTRDSR       0x01
#define FC_RTSCTS       0x02
#define FC_XONXOFF      0x04

// ascii definitions

#define ASCII_BEL       0x07
#define ASCII_BS        0x08
#define ASCII_LF        0x0A
#define ASCII_CR        0x0D
#define ASCII_XON       0x11
#define ASCII_XOFF      0x13


#define _U_COMIO_                   (WM_USER + 16384)
#define WM_U_COMIO_OUT_IN_PROGRESS (_U_COMIO_ + 1)// IO(out) is is progress
#define WM_U_COMIO_OUT_DONE        (_U_COMIO_ + 2)// IO(out) done
#define WM_U_COMIO_NOW             (_U_COMIO_ + 3)// ...
#define WM_U_COMIO_IN_DONE         (_U_COMIO_ + 4)// Just received something from the device



typedef union MIDI_SHORT_DATAtag
{
	DWORD		data;
	struct
		{
		//
		BYTE	chan:4;
		BYTE	stat:4;//b2_1
		BYTE	d1;	 
		//
		BYTE	d2;
		BYTE	b3;
		}s;
}MIDI_SHORT_DATA; //

/// #ifdef DLL_EXPORT_COMM
#if 1 // DLL_EXPORT_COMM is defined in the project settings

typedef struct
  {
  HANDLE      m_hDevice;
  HANDLE      m_hIOThread;  
  DWORD       m_dwThreadID;
  char        m_chDevName[MAX_PATH];
  int         m_iInBuffSize;
  int         m_iOutBuffSize;
              
  int         m_iPort;
  BOOL        m_bConnected, m_bXonXoff, m_bLocalEcho, m_bUseCNReceive, 
              m_bDisplayErrors;
  BYTE        m_btByteSize, m_btFlowCtrl, m_btParity, m_btStopBits ;
  DWORD       m_dwBaudRate ;
  HANDLE      m_hPostEvent, m_hWatchEvent;
  DWORD       dwThreadID ;
  OVERLAPPED  osWrite, osRead ;
  // buffers


	COMM_FIFO			m_InFifo;
	COMM_FIFO			m_OutFifo;

  // Window handlers ..
  HWND        m_hwndDataIn;

	// Closing Events
	HANDLE			m_hInClose;
	HANDLE			m_hInClosed;


  } COMMDEVICE;
typedef COMMDEVICE * LPCOMMDEVICE;

extern COMMDEVICE  m_cm;


DLList32_API void	SendCommData(BYTE		ch, BYTE b1, BYTE b2);

DLList32_API void SetupGlobal();

  // Show Dialog box to Configure the Comm Port
  //
DLList32_API BOOL    SetupCommPort(void);

DLList32_API BOOL    MyOpenCommPort(LPSTR lpsName);
DLList32_API BOOL    OpenComm(void);
DLList32_API BOOL    CloseComm();

DLList32_API BOOL    SetupConnection();
DLList32_API BOOL    CloseConnection();


DLList32_API BOOL    WriteCommBlock(LPSTR lpBlock, DWORD dwLength);

DLList32_API BOOL    IsOpen();

DLList32_API int     GetInData(LPSTR lps);
DLList32_API BOOL    SetDataInWindow(HWND hwnd);



DLList32_API int GetCommInFifo(LPSTR p, int size);
DLList32_API int	GetCommInFifoSize();

UINT	CommIOThreadProc(LPVOID );

//#else
//DLList32_API void	SendCommData(BYTE		ch, BYTE b1, BYTE b2);
//
//DLList32_API void SetupGlobal();
//
//  // Show Dialog box to Configure the Comm Port
//  //
//DLList32_API BOOL    SetupCommPort(void);
//
//DLList32_API BOOL    OpenCommPort(LPSTR lpsName);
//DLList32_API HANDLE  OpenCommPort(ULONG portNumber, DWORD baudRate, DWORD flags);
//DLList32_API BOOL    OpenComm(void);
//DLList32_API BOOL    CloseComm();
//
//DLList32_API BOOL    SetupConnection();
//DLList32_API BOOL    CloseConnection();
//
//
//DLList32_API BOOL    WriteCommBlock(LPSTR lpBlock, DWORD dwLength);
//
//DLList32_API BOOL    IsOpen();
//
//DLList32_API int     GetInData(LPSTR lps);
//DLList32_API BOOL    SetDataInWindow(HWND hwnd);
//
//DLList32_API int GetCommInFifo(LPSTR p, int size);
//DLList32_API int	GetCommInFifoSize();
//

#endif


#endif // !defined(AFX_COMMPORT_H__483D324E_42B6_11D1_A39D_000000000000__INCLUDED_)
