// CommPort.h: interface for the CCommPort class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMFIFO_H__483D324E_42B6_11D1_A39D_000000000000__INCLUDED_)
#define AFX_COMMFIFO_H__483D324E_42B6_11D1_A39D_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <windows.h>

// Buffer size and state
#define MAXBLOCK      4096
#define FIFO_ACCESS_TOUT		100

#ifdef DLList32_EXPORTS
#define DLList32_API __declspec(dllexport)
#else
#define DLList32_API __declspec(dllimport)
#endif

typedef struct
{
    char buffer[MAXBLOCK];
    LPSTR pHead;
    LPSTR pTail;
    LPSTR pEnd;
    int iSize;
    HANDLE	access;
} COMM_FIFO;



#ifdef DLList32_EXPORTS
DLList32_API void SetupFifo(COMM_FIFO *p);
DLList32_API int PutFifo(COMM_FIFO *p, LPSTR pSrc, int size);
DLList32_API int GetFifo(COMM_FIFO *p, LPSTR pSrc, int size);
DLList32_API int GetDataSize(COMM_FIFO *p);
DLList32_API void DestroyFifo(COMM_FIFO *p);
#else
DLList32_API void SetupFifo(COMM_FIFO *p);
DLList32_API int PutFifo(COMM_FIFO *p, LPSTR pSrc, int size);
DLList32_API int GetFifo(COMM_FIFO *p, LPSTR pSrc, int size);
DLList32_API int GetDataSize(COMM_FIFO *p);
DLList32_API void DestroyFifo(COMM_FIFO *p);
#endif

#endif