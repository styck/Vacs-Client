// CommPort.h: interface for the CCommPort class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMFIFO_H__483D324E_42B6_11D1_A39D_000000000000__INCLUDED_)
#define AFX_COMMFIFO_H__483D324E_42B6_11D1_A39D_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// Buffer size and state
#define MAXBLOCK      4096
#define FIFO_ACCESS_TOUT		100

typedef struct
{
	char		buffer[MAXBLOCK];
	LPSTR		pHead;
	LPSTR		pTail;
	LPSTR		pEnd;
	int			iSize;
	HANDLE	access;
} COMM_FIFO;



#ifdef DLL_EXPORT_COMM
__declspec(dllexport)void	SetupFifo(COMM_FIFO *p);
__declspec(dllexport)int		PutFifo(COMM_FIFO *p, LPSTR pSrc, int size);
__declspec(dllexport)int		GetFifo(COMM_FIFO *p, LPSTR pSrc, int size);
__declspec(dllexport)int		GetDataSize(COMM_FIFO *p);
__declspec(dllexport)void	DestroyFifo(COMM_FIFO *p);
#else
__declspec(dllimport)void	SetupFifo(COMM_FIFO *p);
__declspec(dllimport)int		PutFifo(COMM_FIFO *p, LPSTR pSrc, int size);
__declspec(dllimport)int		GetFifo(COMM_FIFO *p, LPSTR pSrc, int size);
__declspec(dllimport)int		GetDataSize(COMM_FIFO *p);
__declspec(dllimport)void	DestroyFifo(COMM_FIFO *p);
#endif





#endif