//=====================================================
// MIDI DLL 
// C Source Code For 32 bit OS(Win95, NT)
// CopyRight 1996 By Bob Lentini and Hristo Doichev
//=====================================================

// Mutex definition
//-----------------
//#define MUTEXNAME "SAMMRC00"

#include <windows.h>

// Some of the defines have to be here
// so they can be used in the other headers
//-----------------------------------------
#define     NUMBER_MIDI_HDRS        16 // Yes .. Yes only 16 of them ;-)
#define     MIDICONTROLVALUE        0xB0
#define     MIDIMTCVALUE            0xF1 // a MTC Message
#define     MAX_SMPTE_MESSAGES      8

// Group name
//-----------
#define     MAX_GROUPNAME           40

// Max channels per Mixer
//-----------------------
//fds #define     MAX_CHANNELS            60 // maximum number of channels per mixer

/////////////////////////////////////////////////////
//fds #include "DLLIST.h" // the Double linked list header
/////////////////////////////////////////////////////
//fds #include "sammstruc.h"

#include "..\inc\SAMM.H"		// gets everything

HINSTANCE hInst;
long lCounter;
LPMIDIDATATRANSFER lpMidiDT = NULL; // lpMidiDataTransfer
BYTE bSMPTEMsg[MAX_SMPTE_MESSAGES] = { 0 };
WORD wSMPTEBuffIndex;
DLGPROC lpDlgProc;
HWND hwndDebug;
HWND hwndMain;
// Function prototypes
//--------------------
void ReceiveSMPTEMsg(BYTE*);
void AssembleSMPTETime(void);

//-----------------------------------------------------------------------
//
//  FUNCTION:    DllMain
//
//  INPUTS:      hDLL       - handle of DLL
//               dwReason   - indicates why DLL called
//               lpReserved - reserved
//
//  RETURNS:     TRUE (always, in this example.)
//
//               Note that the return value is used only when
//               dwReason = DLL_PROCESS_ATTACH.
//
//               Normally the function would return TRUE if DLL initial-
//               ization succeeded, or FALSE it it failed.
//
//  GLOBAL VARS: ghMod - handle of DLL (initialized when PROCESS_ATTACHes)
//
//------------------------------------------------------------------------
BOOL WINAPI DllMain(HANDLE hDLL, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        lCounter = 0;
        wSMPTEBuffIndex = 0;
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}

//=====================================================
void Dll_SetMainWindow(HWND hwnd)
{
    hwndMain = hwnd;
    return;
};

//=====================================================
HWND Dll_GetMainWindow(void)
{
    return hwndMain;
}


//==============================================================
// FUNCTION:    Dll_MidiInProc
// Midi In CallBack Routine
// Places Complete Midi Msg (4 Bytes) In Buffer In Proper Order
//==============================================================
void CALLBACK Dll_MidiInProc(HMIDIIN hIn, UINT wMsg, DWORD dwMidiDev, DWORD dwParam1, DWORD dwParam2)
{
    long lHead, lTail, lBuffSz;
    BYTE chMidiMsg[4];
    int iCount, iIndx;
    LPSTR lpData, lpRecordedData;
    LPMIDIHDR lpmidiHdr;
    DWORD dwBRecorded;

    //DWORD dwMidiDataTransfer;		//fds added

    //if (lpMidiDT == NULL)
    //    lpMidiDT = (LPMIDIDATATRANSFER)dwMidiDataTransfer;

    switch (wMsg)
    {
    case MIM_DATA:
        chMidiMsg[0] = LOBYTE(LOWORD(dwParam1));
        chMidiMsg[1] = HIBYTE(LOWORD(dwParam1));
        chMidiMsg[2] = LOBYTE(HIWORD(dwParam1));
        chMidiMsg[3] = HIBYTE(HIWORD(dwParam1));

        // Check if it is a Controler Msg
        //-------------------------------
        //        if((chMidiMsg[0]&0x0f)== MIDICONTROLVALUE)

        // check if it is a MTC message
        //-----------------------------
        if (chMidiMsg[0] == MIDIMTCVALUE)
        {
            ReceiveSMPTEMsg(chMidiMsg);
            break;
        }

        // assign them to local variables for speed
        //-----------------------------------------
        lpData = lpMidiDT->lpMidiInBuff;
        lHead = lpMidiDT->lInHeadPos;
        lTail = lpMidiDT->lInTailPos;
        lBuffSz = lpMidiDT->lMidiInBuffSz;

        for (iCount = 0; iCount < 4; iCount++)
        {
            lHead++;
            // The Incoming data goes to the Head of the FIFO Buffer
            //------------------------------------------------------
            if (lHead >= lBuffSz)
                lHead = 0;
            // Check if the FIFO buffer has overrun
            //-------------------------------------
            if (lHead == lTail)
            {
                lpMidiDT->lError = 10; // Set the Error flag
                // Reset the lHead back
                //---------------------
                if (lHead == 0)
                    lHead = lBuffSz;
                else
                    lHead--;
                return;
            }

            lpData[lHead] = chMidiMsg[iCount];
            lpMidiDT->lInHeadPos = lHead;
        }
        break;

    case MIM_LONGDATA:
        lpmidiHdr = (LPMIDIHDR)dwParam1;
        dwBRecorded = lpmidiHdr->dwBytesRecorded;
        lpRecordedData = lpmidiHdr->lpData;
        lpData = lpMidiDT->lpMidiInBuff;
        lHead = lpMidiDT->lInHeadPos;
        lTail = lpMidiDT->lInTailPos;
        lBuffSz = lpMidiDT->lMidiInBuffSz;

        for (iCount = 0; iCount < dwBRecorded; iCount++)
        {
            lHead++;
            // The Incoming data goes to the Head of the FIFO Buffer
            //------------------------------------------------------
            if (lHead >= lBuffSz)
                lHead = 0;
            // Check if the FIFO buffer has overrun
            //-------------------------------------
            if (lHead == lTail)
            {
                lpMidiDT->lError = 10; // Set the Error flag
                // Reset the lHead back
                //---------------------
                if (lHead == 0)
                    lHead = lBuffSz;
                else
                    lHead--;
                goto MIDI_LONG_DONE;
            }

            lpData[lHead] = lpRecordedData[iCount];
            lpMidiDT->lInHeadPos = lHead;
        }

    MIDI_LONG_DONE:
        lpMidiDT->iMidiInLongReceived = 1;// set the flag that
        iIndx = lpmidiHdr->dwUser;
        //fds        if((iIndx >= 0)&&(iIndx < NUMBER_MIDI_HDRS))
        //fds            lpMidiDT->pbInHdr[iIndx] = 0;
        //        else
        lpMidiDT->lError = 20;
        break;
    }
    return;
}

//==============================================================
// FUNCTION:    Dll_MidiOutProc
// Midi Out CallBack Routine
//
//      Clears the flag for the MIDI HEADERS that
//  have been send out.
//==============================================================
void CALLBACK Dll_MidiOutProc(HMIDIOUT hOut, UINT wMsg, DWORD dwMidiDev, DWORD dwParam1, DWORD dwParam2)
{
    LPMIDIHDR lpmidiHdr;
    int iIndx;
    LPMIDIDEV pMidiDev;

    pMidiDev = (LPMIDIDEV)dwMidiDev;

    switch (wMsg)
    {
    case MOM_DONE:
        lpmidiHdr = (LPMIDIHDR)dwParam1;
        iIndx = lpmidiHdr->dwUser;
        if ((iIndx >= 0) && (iIndx < NUMBER_MIDI_HDRS))
            pMidiDev->midiDT.piaOutHdr[iIndx] = 0;
        else
            pMidiDev->midiDT.lError = 20;

        break;
    }
    return;
}

//===============================
//FUNCTION: ReceiveSMPTEMsg
//
//===============================
void ReceiveSMPTEMsg(BYTE* chArr)
{
    BYTE byte;

    // Set a flag to detect when the SMPTE has stoped
    //-----------------------------------------------
    lpMidiDT->iSMPTEMsgReceived = 1;

    byte = chArr[1];
    byte &= 0x0f0;
    byte = byte >> 4;
    if (byte == wSMPTEBuffIndex)
        bSMPTEMsg[wSMPTEBuffIndex] = chArr[1];
    else
    {
        // Something went wrong
        // and we should start assembling the
        // MTC messages allover
        //-----------------------------------
        wSMPTEBuffIndex = 0;
        return;
    }

    wSMPTEBuffIndex++;
    if (wSMPTEBuffIndex >= MAX_SMPTE_MESSAGES)
        AssembleSMPTETime();

    return;
}

//============================
// FUNCTION: AssembleSMPTETime
//============================
void AssembleSMPTETime(void)
{
    WORD wSMPTEFormat, wSMPTEFrames, wSMPTESeconds, wSMPTEMins, wSMPTEHours;
    DWORD dwCurSMPTETime;

    // Get the MTC/SMPTE data
    //-----------------------
    __asm
    {
        // Get the SMPTE Format
        //---------------------
        MOV     AL, bSMPTEMsg[7]
        SHL     AL, 1
        AND     AL, 0xF0 //11110000B
        MOV     AH, 0
        MOV     wSMPTEFormat, AX

        // now get the SMPTE time
        //-----------------------
        MOV     AH, bSMPTEMsg[0]
        AND     AH, 0FH

        MOV     AL, bSMPTEMsg[1]
        AND     AL, 0FH
        SHL     AL, 4
        ADD     AL, AH  // Frames
        MOV     AH, 0   // in AX
        MOV     wSMPTEFrames, AX

        MOV     BH, bSMPTEMsg[2]
        AND     BH, 0FH

        MOV     BL, bSMPTEMsg[3]
        AND     BL, 0FH
        SHL     BL, 4
        ADD     BL, BH  // Seconds
        MOV     BH, 0   // in BX
        MOV     wSMPTESeconds, BX

        MOV     CH, bSMPTEMsg[4]
        AND     CH, 0FH

        MOV     CL, bSMPTEMsg[5]
        AND     CL, 0FH
        SHL     CL, 4
        ADD     CL, CH  // Minutes
        MOV     CH, 0   // in CX
        MOV     wSMPTEMins, CX

        MOV     DH, bSMPTEMsg[6]
        AND     DH, 0FH

        MOV     DL, bSMPTEMsg[7]
        AND     DL, 01H
        SHL     DL, 4
        ADD     DL, DH  // Hours
        MOV     DH, 0   // in DX
        MOV     wSMPTEHours, DX

        MOV     DH, DL
        MOV     DL, CL
        MOV     word ptr dwCurSMPTETime + 2, DX
        MOV     AH, BL
        MOV     word ptr dwCurSMPTETime, AX
    }


    //fds lpMidiDT->smpteTime.hour   = (BYTE)wSMPTEHours;
    //fds lpMidiDT->smpteTime.min    = (BYTE)wSMPTEMins;
    //fds lpMidiDT->smpteTime.sec    = (BYTE)wSMPTESeconds;
    //fds lpMidiDT->smpteTime.frame  = (BYTE)wSMPTEFrames;

    lpMidiDT->wSMPTEFormat = wSMPTEFormat;
    lpMidiDT->dwSMPTETime = dwCurSMPTETime;
    lpMidiDT->iNewTime = 1;
    wSMPTEBuffIndex = 0;

    return;
}
