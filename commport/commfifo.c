// CommPort.cpp: implementation of the CCommPort class.
//
//////////////////////////////////////////////////////////////////////

#define DLL_EXPORT_COMM
#include <windows.h>
#include "CommFifo.h"
#include <synchapi.h>
//#include "CommPortSettings.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


void SetupFifo(COMM_FIFO* p)
{
    ZeroMemory(&p->buffer[0], MAXBLOCK);
    p->pEnd = &p->buffer[MAXBLOCK];
    p->pHead = &p->buffer[0];
    p->pTail = &p->buffer[0];
    p->iSize = 0;
    p->access = CreateEvent(NULL, FALSE, TRUE, NULL);
}

void DestroyFifo(COMM_FIFO* p)
{
    CloseHandle(p->access);
}

int PutFifo(COMM_FIFO* p, LPSTR pSrc, int size)
{
    int	iRet = -1;


    // make sure this is thread safe ...
    //
    if (WaitForSingleObject(p->access, FIFO_ACCESS_TOUT) == WAIT_TIMEOUT)
        return iRet;

    // is there enough room in the bufer
    if ((int)(MAXBLOCK - p->iSize) > size)
    {
        iRet = p->pEnd - p->pHead;

        if (iRet > size)
        {
            MoveMemory(p->pHead, pSrc, size);
            p->pHead += size;
        }
        else
        {
            if (iRet > 0)
            {
                MoveMemory(p->pHead, pSrc, iRet);
            }
            else
                iRet = 0;

            p->pHead = &p->buffer[0];
            MoveMemory(p->pHead, &pSrc[iRet], size - iRet);
            p->pHead += size - iRet;

        }

        p->iSize += size;
        iRet = size;
    }

    SetEvent(p->access); // Set it so we can access the data again ... 
    return iRet;
}

/////////////////////////////////////////////
//
//
int GetFifo(COMM_FIFO* p, LPSTR pDest, int size)
{
    int	iRet = -1;
    int	store;

    // make sure this is thread safe ...
    //
    if (WaitForSingleObject(p->access, FIFO_ACCESS_TOUT) == WAIT_TIMEOUT)
        return iRet;

    // is there enough data in the bufer
    if ((int)(p->iSize) > 0)
    {

        store = (size > p->iSize) ? p->iSize : size;

        if ((p->pTail + store) < p->pEnd)
        {
            MoveMemory(pDest, p->pTail, store);
            p->pTail += store;
        }
        else
        {
            iRet = p->pEnd - p->pTail;
            if (iRet > 0)
            {
                MoveMemory(pDest, p->pTail, iRet);
            }
            else
                iRet = 0;

            p->pTail = &p->buffer[0];
            MoveMemory(&pDest[iRet], p->pTail, store - iRet);
            p->pTail += store - iRet;

        }

        p->iSize -= store;
        iRet = store;
    }

    SetEvent(p->access); // Set it so we can access the data again ... 
    return iRet;
}


int GetDataSize(COMM_FIFO* p)
{
    int	iRet = -1;

    // make sure this is thread safe ...
    //
    if (WaitForSingleObject(p->access, FIFO_ACCESS_TOUT) == WAIT_TIMEOUT)
        return iRet;

    iRet = p->iSize;

    SetEvent(p->access); // Set it so we can access the data again ... 

    return iRet;
}
