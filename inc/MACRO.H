//=================================================
//
// Copyright 1998, CorTek Software, Inc.
//=================================================

#include   "SAMMExt.h"
//=================================================
//
//  MACRO DEFINITIONS
//
//=================================================

//#define GETPHISDATAVALUE(mixer, channel, control)   (gpwMemMapMixer[(mixer*(giMax_CHANNELS*giMax_CONTROLS))+(channel * giMax_CONTROLS)+control])
//#define SETPHISDATAVALUE(mixer, channel, control, value)   (gpwMemMapMixer[(mixer*(giMax_CHANNELS*giMax_CONTROLS))+(channel * giMax_CONTROLS)+control] = (WORD)value)

#ifdef _DEBUG

WORD  GETPHISDATAVALUE(int mixer, LPCTRLZONEMAP       pctrlzm, int control);
void  SETPHISDATAVALUE(int mixer, LPCTRLZONEMAP       pctrlzm, int control,int value);
WORD  GETPHISDATAVALUEBUFFER(int mixer, LPCTRLZONEMAP       pctrlzm, int control);
void  SETPHISDATAVALUEBUFFER(int mixer, LPCTRLZONEMAP       pctrlzm, int control,int  value);

#else

#define GETPHISDATAVALUE(mixer, pctrlzm, control)   ( gpwMemMapMixer[(mixer*(giMax_CHANNELS*giMax_CONTROLS))+(pctrlzm->iModuleNumber * giMax_CONTROLS)+control])
#define SETPHISDATAVALUE(mixer, pctrlzm, control, value)   (  gpwMemMapMixer[(mixer*(giMax_CHANNELS*giMax_CONTROLS))+(pctrlzm->iModuleNumber * giMax_CONTROLS)+control] = (WORD)value )
                                      
#define GETPHISDATAVALUEBUFFER(mixer, pctrlzm, control)   ( gpwMemMapBuffer[(mixer*(giMax_CHANNELS*giMax_CONTROLS))+(pctrlzm->iModuleNumber * giMax_CONTROLS)+control])
#define SETPHISDATAVALUEBUFFER(mixer, pctrlzm, control, value)   (gpwMemMapBuffer[(mixer*(giMax_CHANNELS*giMax_CONTROLS))+(pctrlzm->iModuleNumber * giMax_CONTROLS)+control] = (WORD)value)

#endif // _DEBUG

#define CONVERTPHISICALTOSCREEN(p, iv) (iv = (iv * p->iNumScrPos) / p->iNumValues)
#define CONVERTSCREENTOPHISICAL(p, iv) (iv = (iv * p->iNumValues) / p->iNumScrPos)

#define LOADSTRING_IQS(ires)    (LoadString(ghInstStrRes, ires, szBuff, 128))

// Rectangle definitions
//----------------------
#define CONVERT_RECT_TO_WH(r) {r.right = (r.right - r.left);\
                               r.bottom = (r.bottom - r.top);}

// Macros to Store and Extract Word values
// from a Word buffer
//----------------------------------------
#define STOREWORD2(lpw, w1, w2) (lpw[0]=w1, lpw[1]=w2) // END
#define STOREWORD3(lpw, w1, w2, w3) (lpw[0]=w1, lpw[1]=w2, lpw[2]=w3) // END
#define STOREWORD4(lpw, w1, w2, w3, w4)  (lpw[0]=w1, lpw[1]=w2, lpw[2]=w3,\
                                          lpw[3]=w4) // END
#define STOREWORD5(lpw, w1, w2, w3, w4, w5 ) ( lpw[0]=w1, lpw[1]=w2, lpw[2]=w3,\
                                                lpw[3]=w4, lpw[4]=w5 )  // END

#define STOREWORD6(lpw, w1, w2, w3, w4, w5, w6) (lpw[0]=w1, lpw[1]=w2, lpw[2]=w3,\
                                                 lpw[3]=w4, lpw[4]=w5, lpw[5]=w6) // END
//EXTRACT STUFF
//-------------
#define EXTRACTWORD2(lpw, w1, w2) (w1=lpw[0], w2=lpw[1]) // END
#define EXTRACTWORD3(lpw, w1, w2, w3) (w1=lpw[0], w2=lpw[1], w3=lpw[2]) // END
#define EXTRACTWORD4(lpw, w1, w2, w3, w4)  (w1=lpw[0], w2=lpw[1], w3=lpw[2],\
                                            w4=lpw[3]) // END
#define EXTRACTWORD5(lpw, w1, w2, w3, w4, w5) (w1=lpw[0], w2=lpw[1], w3=lpw[2],\
                                               w4=lpw[3], w5=lpw[4]) // END
#define EXTRACTWORD6(lpw, w1, w2, w3, w4, w5, w6) (w1=lpw[0], w2=lpw[1], w3=lpw[2],\
                                                   w4=lpw[3], w5=lpw[4], w6=lpw[5]) // END



#define   makeDWORD_ID(b1, b2, b3, b4)    MAKELONG(MAKEWORD(b1, b2), MAKEWORD(b3, b4))

//=======================
// Inline functions
//=======================
//======================================
//FUNCTION: DivMulInt
//
//formula -> (a/b)*c
// first a gets shifted to te left(16)
// then gets divided and multilied
// then shifted back to the right(16)
//
//======================================

/****
__inline long DivMulInt(int aa,int bb,int cc)
{
_asm     mov        EDX, 0           // clear the EDX

_asm     movzx      EAX, aa  //  mov  eax, a
_asm     movzx      EBX, bb  //  mov  ebx, b
_asm     movzx      ECX, cc  //  mov  ecx, c
_asm     shl        EAX, 16
_asm     div        EBX               //  idiv ebx
_asm     mul        ECX               //  imul ecx
_asm     shr        EAX, 16          //  shr eax, 16
} // DivMulInt()

***/
