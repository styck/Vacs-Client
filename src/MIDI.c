//=================================================
// SAMM Plus 32 bit
// Header Code For 32 bit OS(Win95, NT)
// CopyRight 1996 By Bob Lentini and Hristo Doichev
//=================================================
//#include <windows.h>
//#include <mmsystem.h>

#include "samm.h"
#include "macro.h"

//================================================
//function: OpenMIDIInDev
//
//return:  TRUE = succesful
//         FALSE = error
//purpose:
//      Opens a Midi In Device
//
//=================================================
BOOL OpenMIDIInDev(int iDevId, LPMIDIDEV pMidiDev)
{
    MMRESULT mmResult = MMSYSERR_NOERROR; // Assumes no error
    int iCount;

    // Open Midi Device In Driver
    //----------------------------
    if (iDevId < 0)
    {
        ErrorBox(ghwndMain, ghInstStrRes, IDS_ERR_MIDI_DEV_IN);
        return FALSE;
    }

    // get the pointer for speed
    //--------------------------
    if (CloseMIDIInDev(pMidiDev) == FALSE)
    {
        // Error message
        return FALSE;
    };

    mmResult = midiInOpen(&pMidiDev->hMidiIn, iDevId, (DWORD)Dll_MidiInProc, (DWORD)&gMidiDev, CALLBACK_FUNCTION);

    if (mmResult != MMSYSERR_NOERROR)
    {
        pMidiDev->hMidiIn = NULL;
        ErrorBox(ghwndMain, ghInstStrRes, IDS_ERR_OPEN_MIDI_IN);
        return FALSE;
    }

    pMidiDev->iInDevId = iDevId; // Store the Device ID

    // Device was opened now go ahead and Add the Buffers
    //---------------------------------------------------
    for (iCount = 0; iCount < MIDI_IN_MAX_ADDED_HDRS; iCount++)
    {
        pMidiDev->midiDT.lpmidihdrIn[iCount]->dwUser = iCount;
        midiInPrepareHeader(pMidiDev->hMidiIn, pMidiDev->midiDT.lpmidihdrIn[iCount], sizeof(MIDIHDR));
        if (midiInAddBuffer(pMidiDev->hMidiIn, pMidiDev->midiDT.lpmidihdrIn[iCount], sizeof(MIDIHDR)))
            ErrorBox(ghwndMain, ghInstStrRes, IDS_ERR_ADD_MIDI_IN_BUFFER);
        pMidiDev->midiDT.piaInHdr[iCount] = 1;
    }

    pMidiDev->midiDT.dwLastFrameTick = 0;

    // if by the time we get here and the midiIn was
    // started then just Started again
    //----------------------------------------------
    if (pMidiDev->iInStarted)
    {
        midiInStart(pMidiDev->hMidiIn);
    }
    return TRUE;
}

//================================================
//function: CloseMIDIInDev
//
//return:  TRUE = succesful
//         FALSE = error
//purpose:
//      Closes a Midi In Device
//
//=================================================
BOOL CloseMIDIInDev(LPMIDIDEV pMidiDev)
{
    int iCount;

    if (pMidiDev->hMidiIn == NULL)
        return TRUE; // its closed already

    // Shut down the Device
    //---------------------
    midiInReset(pMidiDev->hMidiIn);

    // Unprepare all the headers
    //--------------------------
    pMidiDev->midiDT.iMidiInLongReceived = 0;
    for (iCount = 0; iCount < MIDI_IN_MAX_ADDED_HDRS; iCount++)
    {
        if (gMidiDev.midiDT.piaInHdr[iCount] == 1)
        {
            midiInUnprepareHeader(pMidiDev->hMidiIn, pMidiDev->midiDT.lpmidihdrIn[iCount], sizeof(MIDIHDR));
            gMidiDev.midiDT.piaInHdr[iCount] = 0;
        }
    }

    // Close the device
    //-----------------
    if (midiInClose(pMidiDev->hMidiIn) == MMSYSERR_NOERROR)
    {
        pMidiDev->hMidiIn = NULL;
        pMidiDev->iInDevId = -1; // invalid device id
        pMidiDev->iInOpen = 0; // Closed
    };

    return TRUE;
}

//=================================================
//function: CloseAllMIDIINDev
//
//
//=================================================
BOOL CloseAllMIDIInDev(void)
{
    if (gMidiDev.hMidiIn != NULL)
        CloseMIDIInDev(&gMidiDev);

    if (gMTCDev.hMidiIn != NULL)
        CloseMIDIInDev(&gMTCDev);

    return TRUE;
}

//================================================
//function: OpenMIDIOutDev
//
//return:  TRUE = succesful
//         FALSE = error
//purpose:
//      Opens a Midi Out Device
//
//=================================================
BOOL OpenMIDIOutDev(int iDevId, LPMIDIDEV pMidiDev)
{
    MMRESULT mmResult;
    int              iCount;

    // Open Midi Device Out Driver
    //----------------------------
    if (iDevId < 0)
    {
        ErrorBox(ghwndMain, ghInstStrRes, IDS_ERR_MIDI_DEV_OUT);
        return FALSE;
    }

    // get the pointer for speed
    //--------------------------
    if (CloseMIDIOutDev(pMidiDev) == FALSE)
    {
        // Error message
        return FALSE;
    };

    mmResult = midiOutOpen(&pMidiDev->hMidiOut, iDevId, (DWORD)Dll_MidiOutProc, (DWORD)&gMidiDev, CALLBACK_FUNCTION);

    if (mmResult != MMSYSERR_NOERROR)
    {
        pMidiDev->hMidiOut = NULL;
        ErrorBox(ghwndMain, ghInstStrRes, IDS_ERR_OPEN_MIDI_OUT);
        return FALSE;
    }

    pMidiDev->iOutDevId = iDevId; // Store the Device ID

    // Device was opened now go ahead and prepare the buffers
    //-------------------------------------------------------
    for (iCount = 0; iCount < MIDI_IN_MAX_ADDED_HDRS; iCount++)
    {
        pMidiDev->midiDT.lpmidihdrOut[iCount]->dwUser = iCount;
        pMidiDev->midiDT.piaOutHdr[iCount] = 0;
    }

    pMidiDev->midiDT.dwLastFrameTick = 0;

    return TRUE;
}

//================================================
//function: CloseMIDIOutDev
//
//return:  TRUE = succesful
//         FALSE = error
//purpose:
//      Closes a Midi Out Device
//
//=================================================
BOOL CloseMIDIOutDev(LPMIDIDEV pMidiDev)
{
    int iCount;

    if (pMidiDev->hMidiOut == NULL)
        return TRUE;

    // Shut down the Device
    //---------------------
    midiOutReset(pMidiDev->hMidiOut);

    // Unprepare all the headers
    //--------------------------
    pMidiDev->midiDT.iMidiOutLongReceived = 0;
    for (iCount = 0; iCount < MIDI_IN_MAX_ADDED_HDRS; iCount++)
    {
        midiOutUnprepareHeader(pMidiDev->hMidiOut, pMidiDev->midiDT.lpmidihdrOut[iCount], sizeof(MIDIHDR));
        pMidiDev->midiDT.piaOutHdr[iCount] = 0;
    }

    // Close the device
    //-----------------
    if (midiOutClose(pMidiDev->hMidiOut) == MMSYSERR_NOERROR)
    {
        pMidiDev->hMidiOut = NULL;
        pMidiDev->iOutDevId = -1; // invalid device id
        pMidiDev->iOutOpen = 0; // Closed
    }
    else
        return FALSE;

    return TRUE;
}

//=================================================
//function: CloseAllMIDIOutDev
//
//
//=================================================
BOOL CloseAllMIDIOutDev(void)
{
    if (gMidiDev.hMidiOut != NULL)
        CloseMIDIOutDev(&gMidiDev);

    if (gMTCDev.hMidiOut != NULL)
        CloseMIDIOutDev(&gMTCDev);

    return TRUE;
}


//============================================================
//FUNCTION: PrepareMidiDataTransferBuffers
//
//return:
//          0 = OK;
//          n = error;
//
// prepare all of the Midi buffers and all of the Midi Headers
//
//============================================================
int     PrepareMidiDataTransferBuffers(LPMIDIDEV pMidiDev)
{
    int         iCount;
    HGLOBAL     hGlob;


    // allocate the FIFO buffers
    // for the Midi IN
    //--------------------------
    pMidiDev->midiDT.lpMidiInBuff = GlobalAlloc(GPTR | GMEM_SHARE, MIDI_IN_BUFF_SIZE);
    if (pMidiDev->midiDT.lpMidiInBuff == NULL)
        return IDS_ERR_ALLOCATE_MEMORY;
    pMidiDev->midiDT.lMidiInBuffSz = MIDI_IN_BUFF_SIZE;

    // allocate the FIFO buffers
    // for the Midi OUT
    //--------------------------
    pMidiDev->midiDT.lpMidiOutBuff = GlobalAlloc(GPTR | GMEM_SHARE, MIDI_OUT_BUFF_SIZE);
    if (pMidiDev->midiDT.lpMidiOutBuff == NULL)
        return IDS_ERR_ALLOCATE_MEMORY;
    pMidiDev->midiDT.lMidiOutBuffSz = MIDI_OUT_BUFF_SIZE;

    // Allocate the Memory for the Midi In headers
    //--------------------------------------------
    for (iCount = 0; iCount < NUMBER_MIDI_HDRS; iCount++)
    {
        pMidiDev->midiDT.hmidihdrIn[iCount] = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE | GMEM_ZEROINIT, (DWORD)sizeof(MIDIHDR));
        if (pMidiDev->midiDT.hmidihdrIn[iCount] == NULL)
            return IDS_ERR_ALLOCATE_MEMORY;
        pMidiDev->midiDT.lpmidihdrIn[iCount] = (LPMIDIHDR)GlobalLock(pMidiDev->midiDT.hmidihdrIn[iCount]);
    }
    // Allocate the Memory for the Midi Out headers
    //---------------------------------------------
    for (iCount = 0; iCount < NUMBER_MIDI_HDRS; iCount++)
    {
        pMidiDev->midiDT.hmidihdrOut[iCount] = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE | GMEM_ZEROINIT, (DWORD)sizeof(MIDIHDR));
        if (pMidiDev->midiDT.hmidihdrOut[iCount] == NULL)
            return IDS_ERR_ALLOCATE_MEMORY;
        pMidiDev->midiDT.lpmidihdrOut[iCount] = (LPMIDIHDR)GlobalLock(pMidiDev->midiDT.hmidihdrOut[iCount]);
    }


    // these are the buffers that get send with the midi hdr
    // -----------------------------------------------------
    for (iCount = 0; iCount < NUMBER_MIDI_HDRS; iCount++)
    {
        // Alocate for the In hdr buffers
        //-------------------------------
        hGlob = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE | GMEM_ZEROINIT, MIDI_BUFF_HDR);
        if (hGlob == NULL)
            return IDS_ERR_ALLOCATE_MEMORY;
        pMidiDev->midiDT.hHdrDataBuffIn[iCount] = hGlob;
        pMidiDev->midiDT.lpHdrDataBuffIn[iCount] = GlobalLock(hGlob);
        pMidiDev->midiDT.lpmidihdrIn[iCount]->lpData = pMidiDev->midiDT.lpHdrDataBuffIn[iCount];
        pMidiDev->midiDT.lpmidihdrIn[iCount]->dwBufferLength = MIDI_BUFF_HDR;

        // Alocate for the Out hdr buffers
        //--------------------------------
        hGlob = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE | GMEM_ZEROINIT, MIDI_BUFF_HDR);
        if (hGlob == NULL)
            return IDS_ERR_ALLOCATE_MEMORY;
        pMidiDev->midiDT.hHdrDataBuffOut[iCount] = hGlob;
        pMidiDev->midiDT.lpHdrDataBuffOut[iCount] = GlobalLock(hGlob);
        pMidiDev->midiDT.lpmidihdrOut[iCount]->lpData = pMidiDev->midiDT.lpHdrDataBuffOut[iCount];
        pMidiDev->midiDT.lpmidihdrOut[iCount]->dwBufferLength = MIDI_BUFF_HDR;
    }

    pMidiDev->midiDT.iMidiInLongReceived = 0;
    pMidiDev->midiDT.iMidiOutLongReceived = 0;
    pMidiDev->midiDT.lInHeadPos = 0;
    pMidiDev->midiDT.lInTailPos = 0;
    pMidiDev->midiDT.lOutHeadPos = 0;
    pMidiDev->midiDT.lOutTailPos = 0;
    pMidiDev->midiDT.wSMPTEFormat = 0;
    return 0;
}

//====================================================
//FUNCTION: UnPrepareMidiDataTransferBuffers
//
//unprepare(free) all of the buffers
// and Midi Headers
//====================================================
void    UnPrepareMidiDataTransferBuffers(LPMIDIDEV pMidiDev)
{
    int         iCount;
    HGLOBAL     hGlob;

    // free some stuff
    //----------------
    if (pMidiDev->midiDT.lpMidiInBuff)
    {
        GlobalFree(pMidiDev->midiDT.lpMidiInBuff);
    }
    if (pMidiDev->midiDT.lpMidiOutBuff)
    {
        GlobalFree(pMidiDev->midiDT.lpMidiOutBuff);
    }

    // Free The Midi in/Out Header stuff
    //----------------------------------
    for (iCount = 0; iCount < NUMBER_MIDI_HDRS; iCount++)
    {
        // the in headers
        hGlob = pMidiDev->midiDT.hmidihdrIn[iCount];// for Speed
        if (hGlob)
        {
            GlobalUnlock(hGlob);
            GlobalFree(hGlob);
            pMidiDev->midiDT.hmidihdrIn[iCount] = NULL;
        }

        // the out headers
        hGlob = pMidiDev->midiDT.hmidihdrOut[iCount];// for Speed
        if (hGlob)
        {
            GlobalUnlock(hGlob);
            GlobalFree(hGlob);
            pMidiDev->midiDT.hmidihdrOut[iCount] = NULL;
        }
    }


    // these are the buffers that get send with the midi hdr
    // -----------------------------------------------------
    for (iCount = 0; iCount < NUMBER_MIDI_HDRS; iCount++)
    {
        // Free the In hdr buffers
        //-------------------------------
        hGlob = pMidiDev->midiDT.hHdrDataBuffIn[iCount];
        if (hGlob)
        {
            GlobalUnlock(hGlob);
            GlobalFree(hGlob);
            pMidiDev->midiDT.hHdrDataBuffIn[iCount] = NULL;
        }
        // Free the Out hdr buffers
        //--------------------------------
        hGlob = pMidiDev->midiDT.hHdrDataBuffOut[iCount];
        if (hGlob)
        {
            GlobalUnlock(hGlob);
            GlobalFree(hGlob);
            pMidiDev->midiDT.hHdrDataBuffOut[iCount] = NULL;
        }
    }

    return;
}


//=================================================
//function: ListMidiInDev
//
// return:  int n > 0; if Devices were found and 
//                     added to the combobox
//          int n = 0; if there were no devices
//purpose:
// Search all MIDI IN devices and put them into
// a combobox
//
//=================================================
int      ListMidiInDev(HWND hwnd)
{
    int         iNumDev, iCount;
    char        szBuff[128];
    MIDIINCAPS  midiInCaps;


    if (hwnd == NULL)
        return 0;

    SendMessage(hwnd, CB_RESETCONTENT, 0, 0);

    iNumDev = midiInGetNumDevs();
    if (iNumDev == 0)
    {
        LOADSTRING_IQS(IDS_DEVICENOTFOUND);
        SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)szBuff);
        return 0;
    }

    // Fill the Combo Box with data
    for (iCount = 0; iCount < iNumDev; iCount++)
    {
        midiInGetDevCaps(iCount, &midiInCaps, sizeof(MIDIINCAPS));
        SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)midiInCaps.szPname);
    }


    return 1;
}


//=================================================
//function: ListMidiOutDev
//
// return:  int n > 0; if Devices were found and 
//                     added to the combobox
//          int n = 0; if there were no devices
//purpose:
// Search all MIDI OUT devices and put them into
// a combobox
//
//=================================================
int      ListMidiOutDev(HWND hwnd)
{
    int             iNumDev, iCount;
    char            szBuff[128];
    MIDIOUTCAPS     midiOutCaps;

    if (hwnd == NULL)
        return 0;

    SendMessage(hwnd, CB_RESETCONTENT, 0, 0);

    iNumDev = midiOutGetNumDevs();
    if (iNumDev == 0)
    {
        LOADSTRING_IQS(IDS_DEVICENOTFOUND);
        SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)szBuff);
        return 0;
    }

    // Fill the Combo Box with data
    for (iCount = 0; iCount < iNumDev; iCount++)
    {
        midiOutGetDevCaps(iCount, &midiOutCaps, sizeof(MIDIINCAPS));
        SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)midiOutCaps.szPname);
    }


    return 1;
}


//================================
//FUNCTION: CheckMidiInData
//
// Using the head and tail buffer
// check for the incoming Midi
// data
//================================
void    CheckMidiInData(void)
{
    /*
    long    lHead, lTail, lBuffSz;
    LPSTR   lpData;
    BYTE    byte;
    int     iCount, iAddedHeaders;

    // assign them to local variables for speed
    //-----------------------------------------
    lpData = gMidiDataTransfer.lpMidiInBuff;
    lHead = gMidiDataTransfer.lInHeadPos;
    lTail = gMidiDataTransfer.lInTailPos;
    lBuffSz = gMidiDataTransfer.lMidiInBuffSz;

    if(lHead == lTail)
    return; // there is nothing to do here

    // if there is something in the buffer
    // then read it byte by byte
    //------------------------------------
    while(lTail != lHead)
    {
    lTail++;
    if(lTail >= lBuffSz)
    lTail = 0;

    byte = lpData[lTail];

    ShowDebugInfo(TRUE, 4,(WORD)byte);
    }


    gMidiDataTransfer.lInTailPos = lTail;

    // Check if we have to prepare another buffer
    // and send it out to the MidiIn device
    //-------------------------------------------
    if(gMidiDataTransfer.iMidiInLongReceived)
    {
    gMidiDataTransfer.iMidiInLongReceived = 0;
    iAddedHeaders = 0;
    for(iCount=0; iCount < NUMBER_MIDI_HDRS; iCount++)
    {
    if(iAddedHeaders >= MIDI_IN_MAX_ADDED_HDRS)
    break;

    if(gMidiDataTransfer.pbInHdr[iCount] == 0)
    {
    midiInUnprepareHeader(ghMidiIn, gMidiDataTransfer.lpmidihdrIn[iCount], sizeof(MIDIHDR));
    gMidiDataTransfer.lpmidihdrIn[iCount]->dwUser = iCount;
    gMidiDataTransfer.lpmidihdrIn[iCount]->dwFlags = 0; // just clear it
    midiInPrepareHeader(ghMidiIn, gMidiDataTransfer.lpmidihdrIn[iCount], sizeof(MIDIHDR));
    midiInAddBuffer(ghMidiIn, gMidiDataTransfer.lpmidihdrIn[iCount],  sizeof(MIDIHDR));
    gMidiDataTransfer.pbInHdr[iCount] = 1;
    }
    iAddedHeaders++;
    }
    }
    */
    return;
}


//==================================================
//FUNCTION: FlushMIDIBuff
//
//
//return:
//          n = error
//          0 = OK
//purpose:
//          Flush the Out FIFO Buffer
//  using the Midi Headers
//
//==================================================
int     FlushMIDIBuff(LPMIDIDEV pMidiDev)
{
    int     iCount;
    long    lHeadPos, lTailPos;
    long    lBytesToSend;

    lHeadPos = pMidiDev->midiDT.lOutHeadPos;
    lTailPos = pMidiDev->midiDT.lOutTailPos;

    if (lHeadPos >= lTailPos)
        // there is nothing to send out
        return 1;

    for (iCount = 0; iCount < NUMBER_MIDI_HDRS; iCount++)
    {
        // make sure this header is not currently in the
        // Out Cue .. then prepare it and send it out
        //----------------------------------------------
        if (pMidiDev->midiDT.piaOutHdr[iCount] == 0)
        {
            lBytesToSend = lTailPos - lHeadPos;
            if (lBytesToSend > MIDI_BUFF_HDR)
                lBytesToSend = MIDI_BUFF_HDR;

            midiOutUnprepareHeader(pMidiDev->hMidiOut, pMidiDev->midiDT.lpmidihdrOut[iCount], sizeof(MIDIHDR));
            pMidiDev->midiDT.lpmidihdrOut[iCount]->dwUser = iCount;
            // copy the data to be send out
            //-----------------------------
            CopyMemory(pMidiDev->midiDT.lpmidihdrOut[iCount]->lpData, &pMidiDev->midiDT.lpMidiOutBuff[lHeadPos], lBytesToSend);
            pMidiDev->midiDT.lpmidihdrOut[iCount]->dwBufferLength = lBytesToSend;
            pMidiDev->midiDT.lpmidihdrOut[iCount]->dwFlags = 0;
            midiOutPrepareHeader(pMidiDev->hMidiOut, pMidiDev->midiDT.lpmidihdrOut[iCount], sizeof(MIDIHDR));
            pMidiDev->midiDT.piaOutHdr[iCount] = 1; // this flag is getting cleared
            // in the callback procedure
            // when we receive this header
            //-----------------------------
            // Slam the thing OUT ... ;-)
            //---------------------------
            midiOutLongMsg(pMidiDev->hMidiOut, pMidiDev->midiDT.lpmidihdrOut[iCount], sizeof(MIDIHDR));

            // adjust the head
            //----------------
            lHeadPos += lBytesToSend;
            if (lHeadPos >= lTailPos)
                break;
        }
    }

    // Set them to the beggining
    // of the buffer so  everithing can start
    // allover again
    //---------------------------------------
    pMidiDev->midiDT.lOutHeadPos = 0;
    pMidiDev->midiDT.lOutTailPos = 0;

    return 0;
}

//==================================================
//FUNCTION: TranslateSendOutMIDI
//
//
//purpose:
//          Translate and send the MIDI out.
//    The Midi Data is Flushed only if the
//  FIFO buffer is full or the FLUSH flag is TRUE
//
//
//==================================================
void    TranslateSendOutMIDI(LPMIDIDEV pMidiDev,
    LPMIDITOSCRTABLE lpmiditoscr, int iMixer,
    int iChan, int iCtrl, int iVal, BOOL bfFlush)
{
    int     iBytesWritten;
    long    lTailPos;
    //int     iDebug;


    // assemble the midi data to go out
    // check for anything that has been remaped
    // like a mixer that is suppoused to go out
    // through a different port, or channel or something else.
    //--------------------------------------------------------

    STOREWORD5(gwarMIDIData, iMixer, iChan, iCtrl, iVal, 0xFFFF);
    iBytesWritten = printMIDI(gbarMIDIBuff, MIDI_BUFFER_SIZE, lpmiditoscr->warMIDIFormat, gwarMIDIData, lpmiditoscr->iCtrlPhisNum);
    if (iBytesWritten < 1)
        return; // either  nothing got written
    // or there was some sort of an error


    //for(iDebug=0; iDebug < iBytesWritten; iDebug ++)
    //    ShowDebugInfo(TRUE, iBytesWritten, gbarMIDIBuff[iDebug]);



    // Deal with the MIDI data stored in gbarMIDIData
    // ... put it in a midi buffer to go out
    // force the midi data to go out or spool it
    // until latter when it might get filled up
    // or flushed out ....
    // Keep track of the MIDI buffers that have went out
    //
    //---------------------------------------------------
    lTailPos = pMidiDev->midiDT.lOutTailPos;

    // Calc if we have the room in the MIDIOutBuff
    // to store this data in ...
    //--------------------------------------------
    if ((MIDI_OUT_BUFF_SIZE - lTailPos) < iBytesWritten)
    {
        if (FlushMIDIBuff(pMidiDev)) // this will reset the tail and head positions
            return; // Oops couldn't flush the buffer
    }

    // now put the data into the FIFO buffer
    //--------------------------------------
    CopyMemory(&pMidiDev->midiDT.lpMidiOutBuff[lTailPos], gbarMIDIBuff, iBytesWritten);
    pMidiDev->midiDT.lOutTailPos += iBytesWritten;

    if (bfFlush)
        FlushMIDIBuff(pMidiDev);

    return;
}

//======================================================
//FUNCTION: printMIDI
//
//params:
//          LPSTR   lpstrMIDIBuff; the Buffer where
//                                 the MIDI data will
//                                 be stored
//          int     iBuffLen;      the size of the MIDI
//                                 buffer
//          LPWORD  lpwFormat;     the format buffer
//                                 FFFF is the End of it
//          LPWORD  lpwData;       the DATA buffer
//          int     iCtrPhisNum;   used in some cases
//                                 in conjuction with
//                                 the other data
//return:
//          int ;  -1 MIDI buffer too small Error
//                 -2 MIDI format Error
//                 else the number of bytes
//                 printed(stored) in the lpstrMIDIBuff
//
//purpose:
//          Print(store) the midi data into the MIDI
//      buffer using the format specified in lpwFormat
//      pulling the data from lpwData.
//      Ths function works somewhat like wsprintf ...
//      but it doesn't use variable length parameters
//
//
//======================================================
int     printMIDI(LPSTR lpstrMIDIBuff, int iBuffLen,
    LPWORD lpwFormat, LPWORD lpwData,
    int iCtrlPhisNum)
{
    int     iWrite;
    int     iCurPos;
    LPWORD  lpwF, lpwD;


    lpwF = lpwFormat;
    lpwD = lpwData;
    iWrite = 0;
    iCurPos = 0;

    // Go through all of the Format Values
    // and store the result in the
    // lpstrMIDIBuff
    // -----------------------------------
    while ((iCurPos < iBuffLen) && (*lpwF != MIDIF_ENDFORMAT))
    {
        switch (*lpwF)
        {
        case MIDIF_CUR:
            lpstrMIDIBuff[iCurPos++] = LOBYTE(*lpwD);
            lpwD++;
            break;

        case MIDIF_CUR_P_CTRL:
            lpstrMIDIBuff[iCurPos++] = LOBYTE((*lpwD) + iCtrlPhisNum);
            lpwD++;
            break;
            // none of these will perform
            // a pointer validation check
            // ... So it all depends
            // on accurate FORMAT
            //---------------------------
        case MIDIF_CUR_P_PREV:
            lpstrMIDIBuff[iCurPos++] = LOBYTE(*lpwD) + LOBYTE(*(lpwD - 1));
            lpwD++;
            break;

        case MIDIF_CUR_P_NEXT:
            lpstrMIDIBuff[iCurPos++] = LOBYTE(*lpwD) + LOBYTE(*(lpwD + 1));
            lpwD++;
            break;

        case MIDIF_SKIP: // Skip the data in the data buffer
            lpwD++;
            break;

        default:
            lpstrMIDIBuff[iCurPos++] = LOBYTE(*lpwF);
            break;
        }

        // increment the Word pointer to the Next Word
        //--------------------------------------------
        lpwF++;
    }

    if (*lpwF != MIDIF_ENDFORMAT)
        return -1; // Error because we reached the End of the Buffer
    // before we reached the END of the Format String

    return iCurPos;
}

