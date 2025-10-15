//////////////////////////////////////////////////////////////
// This routine loads the comp and gate VU lookup tables.
// These meters deflect from right to left on a 0 to 96 pixel display.
//
//     0pix        96pix
//    20dB <-----< 0dB
//
// This routine compensates for the backward motion, so in the display
// routine only the left x edge starting pixel needs to be summed
// with the lookup table values.
//
// To use this function define two int arrays gVU_CompDispTable
// and gVU_GateDispTable to hold the comp and gate VU meter lookup tables.
//
//
#define   PIXELS_PER_SEGMENT (double)3.0

extern int                 gVU_CompDispTable[4096];
extern int                 gVU_GateDispTable[4096];

void    InitCompnGateLookupTables(void)
{
  int     iCount;
  double   fDacComp, fDacGate, fDBLow, fDBPixPerSeg, fPixelShift, fDBPerSeg;

  memset(gVU_CompDispTable, 0, 4096 * sizeof(int));
  memset(gVU_GateDispTable, 0, 4096 * sizeof(int));

  // do compression meter,
  // Comp ADC range: <52 map to 0dB, >3033 map to 20dB

  for(iCount = 0; iCount < 4096; iCount ++)
  {
	if (iCount = 0)
	fDacComp = 1.0; // keep log from blowing up
	else
	fDacComp = (double)iCount;

	fDacComp = 4.79
				+    ( 20.0*log10(fDacComp/300.0) ) / 1.98
				+ pow( 20.0*log10(fDacComp/300.0) ,2.0) / 79.794  ;
	if(fDacComp < 0.0)
		 fDacComp = 0.0;
	if(fDacComp > 20.0)
		 fDacComp = 20.0;
  
	if(fDacComp < 12.0)
      {
        fDBLow = 0.5;
        fDBPerSeg = 0.5;
        fPixelShift = 2.0;
      }
	else
      {
        fDBLow = 12.0;
        fDBPerSeg = 1.0;
        fPixelShift = 71.0;
      }


	fDacComp = 96 - (fPixelShift + (fDacComp - fDBLow)* (PIXELS_PER_SEGMENT / fDBPerSeg) );

	gVU_CompDispTable[iCount] = (int)fDacComp;
  }

// ------

  // do gate meter,
  // Gate ADC range: <400 map to 0dB, >3959 map to 20dB
  for(iCount = 0; iCount < 4096; iCount ++)
  {
	if (iCount = 0)
		fDacGate = 1.0; // keep log from blowing up
	else
		fDacGate = (double)iCount;

	fDacGate = -49.7815
				+    ( 20.0*log10(fDacGate) )
				- pow( 20.0*log10(fDacGate) ,2.0) / 521.229
				+ pow( 20.0*log10(fDacGate) ,3.0) / 48000.0 ;
	if(fDacGate < 0.0)
		 fDacGate = 0.0;
	if(fDacGate > 20.0)
		 fDacGate = 20.0;
  
	if(fDacGate < 12.0)
      {
        fDBLow = 0.5;
        fDBPerSeg = 0.5;
        fPixelShift = 2.0;
      }
	else
      {
        fDBLow = 12.0;
        fDBPerSeg = 1.0;
        fPixelShift = 71.0;
      }


	fDacGate = 96 - (fPixelShift + (fDacGate - fDBLow)* (PIXELS_PER_SEGMENT / fDBPerSeg) );

	gVU_GateDispTable[iCount] = (int)fDacGate;

  }

}
