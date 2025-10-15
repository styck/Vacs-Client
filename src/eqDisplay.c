//=================================================
// Copyright 1998, CorTek Software, Inc.
//=================================================

#include <math.h>
//#include <windows.h>
#include "SAMM.h"
#include "SAMMEXT.h"
#include "MACRO.h"


#include "consoledefinition.h"            


static float InputBC_LF[] =
{
	15.0,
	14.0,
	13.0,
	12.0,
	11.0,
	10.0,
	09.0,
	08.0,
	07.0,
	06.0,
	05.0,
	04.0,
	03.0,
	02.0,
	01.0,
	0.00,
	-01.0,
	-02.0,
	-03.0,
	-04.0,
	-05.0,
	-06.0,
	-07.0,
	-08.0,
	-09.0,
	-10.0,
	-11.0,
	-12.0,
	-13.0,
	-14.0,
	-15.0,
};

static float InputBC_ML[] =
{
	15.0,
	14.0,
	13.0,
	12.0,
	11.0,
	10.0,
	09.0,
	08.0,
	07.0,
	06.0,
	05.0,
	04.0,
	03.0,
	02.0,
	01.0,
	0.00,
	-01.0,
	-02.0,
	-03.0,
	-04.0,
	-05.0,
	-06.0,
	-07.0,
	-08.0,
	-09.0,
	-10.0,
	-12.0,
	-15.0,
	-20.0,
	-25.0,
	-40.0,
};


static float InputBW_ML[] =
{
	(float)2.0,
	(float)1.8,	
	(float)1.6,	
	(float)1.4,	
	(float)1.2,	
	(float)1.1,	
	(float)1.0,	
	(float)0.875,  // 7/8
	(float)0.75,   // 3/4
	(float)0.625,  // 5/8
	(float)0.5,    // 1/2
	(float)0.3333, // 1/3
	(float)0.25,   // 1/4
	(float)0.1667, // 1/6
	(float)0.1111, // 1/9
};



static float InputFreq_HI[] =
{     
	16000,
	13000,
	11000,
	9600,
	8400,
	7500,
	6800,
	6200,
	5700,
	5300,
	5000,
	4600,
	4300,
	4000,
	3800,
	3600,
	3400,
	3300,
	3000,
	2800,
	2700,
	2600,
	2500,
	2400,
	2300,
	2200,
	2000,
	1900,
	1800,
	1700,
	1600,
	1500,
	1400,
	1300,
	1200,
	1100,
	1000,
};


static float InputFreq_LO[] =
{     
	500,
	450,
	400,
	350,
	300,
	250,
	200,
	180,
	170,
	160,
	150,
	140,
	130,
	125,
	115,
	110,
	100,
	95,
	85,
	80,
	75,
	70,
	65,
	60,
	55,
	50,
	45,
	40,
	35,
	30,
};

static float InputFreq_LM[] =
{     
	1600,
	1300,
	1100,
	960,
	840,
	750,
	680,
	620,
	570,
	530,
	500,
	460,
	430,
	400,
	380,
	360,
	340,
	330,
	300,
	280,
	270,
	260,
	250,
	240,
	230,
	220,
	200,
	190,
	180,
	170,
	160,
	150,
	140,
	130,
	120,
	110,
	100,
};



static float InputFreq_HM[] =
{     
	12000,
	9000,
	7000,
	6000,
	5500,
	5000,
	4500,
	4000,
	3600,
	3300,
	3000,
	2800,
	2600,
	2500,
	2400,
	2300,
	2200,
	2000,
	1900,
	1800,
	1700,
	1600,
	1500,
	1400,
	1300,
	1200,
	1100,
	1000,
	950,
	900,
	850,
	800,
	750,
	700,
	650,
}; 

static float InputFreq_LOCUT[] = 
{
	300,
	250,
	200,
	175,
	150,
	135,
	120,
	100,
	90,
	80,
	70,
	60,
	50,
	40,
	35,
	30,
	25,
	20,
	15,
};


////////////////
////////////////
////////////////
////////////////
////////////////
////////////////
////////////////
//
void    DrawEqGraph(HDC hdc, RECT *rWhere, LPEQ_DISPLAY_DATA pEqData);

///////////////////////////////////////////////
//
//
//
void    SetSubEqData(EQ_DISPLAY_DATA *pEqGraph, LPMIXERWNDDATA pmwd, int iPhisChannel, int iModule)
{
	LPCTRLZONEMAP pctrl;
	int           iCtrlNum;
	BOOL          bIsRight = FALSE;


	// Check for the Left-Right Selector  ...
	//
	pctrl = ScanCtrlZonesNum(pmwd->lpZoneMap[iPhisChannel].lpZoneMap, CTRL_NUM_MATRIX_EQ_LEFTRIGHT_SELECTOR);
	if(GETPHISDATAVALUE(0, pctrl, CTRL_NUM_MATRIX_EQ_LEFTRIGHT_SELECTOR) > 0)
	{
		// Show LEFT
		iCtrlNum = CTRL_NUM_MATRIX_HI_FREQ_LT;
		bIsRight = FALSE;
	}
	else
	{
		// Show RIGHT
		iCtrlNum = CTRL_NUM_MATRIX_HI_FREQ_RT;
		bIsRight = TRUE;
	}



	// There is no LoCut filter ..
	// so set it to NULL .... 
	pEqGraph[0].eqId = EQ_ID_NULL;
	pEqGraph[1].eqId = EQ_ID_NULL;
	pEqGraph[2].eqId = EQ_ID_NULL;
	pEqGraph[3].eqId = EQ_ID_NULL;
	pEqGraph[4].eqId = EQ_ID_NULL;


	// High Freq Eq
	//
	//
	//
	pctrl = ScanCtrlZonesNum(pmwd->lpZoneMap[iPhisChannel].lpZoneMap, iCtrlNum);
	if(pctrl) // Validate the Pointer !!!
	{
		pEqGraph[4].ffc = InputFreq_HI[GETPHISDATAVALUE(0, pctrl, iCtrlNum)];
	}
	else
		pEqGraph[4].ffc = 5000;

	iCtrlNum ++;
	pctrl = ScanCtrlZonesNum(pmwd->lpZoneMap[iPhisChannel].lpZoneMap, iCtrlNum);
	if(pctrl) // Validate the Pointer !!!
	{
		pEqGraph[4].fbw = InputBW_ML[GETPHISDATAVALUE(0, pctrl, iCtrlNum)];
	}
	else
		pEqGraph[4].fbw = 1.0;

	iCtrlNum++;
	pctrl = ScanCtrlZonesNum(pmwd->lpZoneMap[iPhisChannel].lpZoneMap, iCtrlNum + 1);
	if(  GETPHISDATAVALUE(0, pctrl, iCtrlNum + 1))
	{
		pctrl = ScanCtrlZonesNum(pmwd->lpZoneMap[iPhisChannel].lpZoneMap, iCtrlNum);
		if(pctrl) // Validate the Pointer !!!
		{
			pEqGraph[4].fgain = InputBC_LF[GETPHISDATAVALUE(0, pctrl, iCtrlNum)];
		}
		else
			pEqGraph[4].fgain = 0.0;

		pEqGraph[4].eqId = EQ_ID_HIGHSHELF;
	}
	else
	{
		pctrl = ScanCtrlZonesNum(pmwd->lpZoneMap[iPhisChannel].lpZoneMap, iCtrlNum);
		if(pctrl) // Validate the Pointer !!!
		{
			pEqGraph[4].fgain = InputBC_LF[GETPHISDATAVALUE(0, pctrl, iCtrlNum)];
		}
		else
			pEqGraph[4].fgain = 0.0;
		pEqGraph[4].eqId = EQ_ID_BP;
	}
	pEqGraph[4].color = RGB(231, 231, 132);

	iCtrlNum += 2;

	if( ! bIsRight)
		iCtrlNum += 4; // Skip over the other Right Controls
	else
		iCtrlNum += 3;

	// Hi-Mid Eq
	//
	//
	//
	pctrl = ScanCtrlZonesNum(pmwd->lpZoneMap[iPhisChannel].lpZoneMap, iCtrlNum);
	if(pctrl) // Validate the Pointer !!!
	{
		pEqGraph[3].ffc = InputFreq_HM[GETPHISDATAVALUE(0, pctrl, iCtrlNum)];
	}
	else
		pEqGraph[3].ffc = 2000;

	iCtrlNum ++;
	pctrl = ScanCtrlZonesNum(pmwd->lpZoneMap[iPhisChannel].lpZoneMap, iCtrlNum);
	if(pctrl) // Validate the Pointer !!!
	{
		pEqGraph[3].fbw = InputBW_ML[GETPHISDATAVALUE(0, pctrl, iCtrlNum)];
	}
	else
		pEqGraph[3].fbw = 1.0;

	iCtrlNum ++;
	pctrl = ScanCtrlZonesNum(pmwd->lpZoneMap[iPhisChannel].lpZoneMap, iCtrlNum);
	if(pctrl) // Validate the Pointer !!!
	{
		pEqGraph[3].fgain = InputBC_ML[GETPHISDATAVALUE(0, pctrl, iCtrlNum)];
	}
	else
		pEqGraph[3].fgain = 0.0;

	// adjust for the wierd Gamble filters ... which have different response in the < 0.00
	// area
	if(pEqGraph[3].fgain < 0.00)
	{
		pEqGraph[3].fbw /= 2.0;
	}

	pEqGraph[3].eqId = EQ_ID_BP;
	pEqGraph[3].color = RGB(239, 173, 115);


	iCtrlNum += 4; // Skip over the other Right Controls


	// Lo-Mid Eq
	//
	//
	//
	pctrl = ScanCtrlZonesNum(pmwd->lpZoneMap[iPhisChannel].lpZoneMap, iCtrlNum);
	if(pctrl) // Validate the Pointer !!!
	{
		pEqGraph[2].ffc = InputFreq_LM[GETPHISDATAVALUE(0, pctrl, iCtrlNum)];
	}
	else
		pEqGraph[2].ffc = 500;

	iCtrlNum ++;
	pctrl = ScanCtrlZonesNum(pmwd->lpZoneMap[iPhisChannel].lpZoneMap, iCtrlNum);
	if(pctrl) // Validate the Pointer !!!
	{
		pEqGraph[2].fbw = InputBW_ML[GETPHISDATAVALUE(0, pctrl, iCtrlNum)];
	}
	else
		pEqGraph[2].fbw = 1.0;


	iCtrlNum ++;
	pctrl = ScanCtrlZonesNum(pmwd->lpZoneMap[iPhisChannel].lpZoneMap, iCtrlNum);
	if(pctrl) // Validate the Pointer !!!
	{
		pEqGraph[2].fgain = InputBC_ML[GETPHISDATAVALUE(0, pctrl, iCtrlNum)];
	}
	else
		pEqGraph[2].fgain = 0.0;

	// adjust for the wierd Gamble filters ... which have different response in the < 0.00
	// area
	if(pEqGraph[2].fgain < 0.00)
	{
		pEqGraph[2].fbw /= 2.0;
	}

	pEqGraph[2].eqId = EQ_ID_BP;
	pEqGraph[2].color = RGB(123, 198, 107);


	if( ! bIsRight)
		iCtrlNum += 4; // Skip over the other Right Controls
	else
		iCtrlNum += 5;

	// LoFreq EQ
	//
	pctrl = ScanCtrlZonesNum(pmwd->lpZoneMap[iPhisChannel].lpZoneMap, iCtrlNum);
	if(pctrl) // Validate the Pointer !!!
	{
		pEqGraph[1].ffc = InputFreq_LO[GETPHISDATAVALUE(0, pctrl, iCtrlNum)];
	}
	else
		pEqGraph[1].ffc = 5000;

	iCtrlNum ++;
	pctrl = ScanCtrlZonesNum(pmwd->lpZoneMap[iPhisChannel].lpZoneMap, iCtrlNum);
	if(pctrl) // Validate the Pointer !!!
	{
		pEqGraph[1].fbw = InputBW_ML[GETPHISDATAVALUE(0, pctrl, iCtrlNum)];
	}
	else
		pEqGraph[1].fbw = 1.0;

	iCtrlNum ++;
	pctrl = ScanCtrlZonesNum(pmwd->lpZoneMap[iPhisChannel].lpZoneMap, iCtrlNum + 1);
	if( GETPHISDATAVALUE(0, pctrl, iCtrlNum + 1))
	{
		pEqGraph[1].eqId = EQ_ID_LOSHELF;
	}
	else
	{
		//pctrl = ScanCtrlZonesNum(pmwd->lpZoneMap[iPhisChannel].lpZoneMap, iCtrlNum);
		pEqGraph[1].eqId = EQ_ID_BP;
	}

	pctrl = ScanCtrlZonesNum(pmwd->lpZoneMap[iPhisChannel].lpZoneMap, iCtrlNum);
	if(pctrl) // Validate the Pointer !!!
	{
		pEqGraph[1].fgain = InputBC_LF[GETPHISDATAVALUE(0, pctrl, iCtrlNum)];
	}
	else
		pEqGraph[1].fgain = 0.0;

	pEqGraph[1].color = RGB(132, 165, 214);

	return;  
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
void __stdcall DrawEqGraphHook(HDC hdc,LPCTRLZONEMAP lpctrlZM, int iVal, LPMIXERWNDDATA lpmwd, int iChan )
{
	RECT  rZone;
	EQ_DISPLAY_DATA eqDisplay[5];
	LPCTRLZONEMAP pctrl;

	rZone = lpctrlZM->rZone;
	rZone.right = rZone.right - rZone.left;
	rZone.bottom = rZone.bottom - rZone.top;

	// Handle the EQ graph for the Sub-Matrix(Matrix!!!) .... 
	//
	if( lpctrlZM->iCtrlType == CTRL_TYPE_DISP_SUB_EQ_FILTER)
	{
		SetSubEqData(eqDisplay, lpmwd, iChan, lpctrlZM->iModuleNumber);
		goto ON_DRAW_EQ_GRAPH;
	}

	// Get the actual Values
	// from the controls
	//
	// set values of test response
	pctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iChan].lpZoneMap, CTRL_NUM_INPUT_LOWCUT_INOUT);
	if( ! GETPHISDATAVALUE(0, pctrl, CTRL_NUM_INPUT_LOWCUT_INOUT))
	{
		eqDisplay[0].fbw = 0;
		eqDisplay[0].ffc = InputFreq_LOCUT[GETPHISDATAVALUE(0, pctrl, CTRL_NUM_INPUT_LOWCUT)];
		eqDisplay[0].fgain = 0;
		eqDisplay[0].eqId = EQ_ID_LOCUT;
	}
	else
		eqDisplay[0].eqId = EQ_ID_NULL;
	eqDisplay[0].color = RGB(231, 107, 231);


	// LoFreq EQ
	//
	//
	//
	pctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iChan].lpZoneMap, CTRL_NUM_INPUT_LOWFREQ);
	if(pctrl) // Validate the Pointer !!!
	{
		eqDisplay[1].ffc = InputFreq_LO[GETPHISDATAVALUE(0, pctrl, CTRL_NUM_INPUT_LOWFREQ)];
	}
	else
		eqDisplay[1].ffc = 5000;

	pctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iChan].lpZoneMap, CTRL_NUM_INPUT_LF_BW);
	if(pctrl) // Validate the Pointer !!!
	{
		eqDisplay[1].fbw = InputBW_ML[GETPHISDATAVALUE(0, pctrl, CTRL_NUM_INPUT_LF_BW)];
	}
	else
		eqDisplay[1].fbw = 1.0;

	pctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iChan].lpZoneMap, CTRL_NUM_INPUT_LF_PEAKSHELF);
	if( GETPHISDATAVALUE(0, pctrl, CTRL_NUM_INPUT_LF_PEAKSHELF))
	{
		eqDisplay[1].eqId = EQ_ID_LOSHELF;
	}
	else
	{
		pctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iChan].lpZoneMap, CTRL_NUM_INPUT_LF_BC);
		eqDisplay[1].eqId = EQ_ID_BP;
	}

	pctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iChan].lpZoneMap, CTRL_NUM_INPUT_LF_BC);
	if(pctrl) // Validate the Pointer !!!
	{
		eqDisplay[1].fgain = InputBC_LF[GETPHISDATAVALUE(0, pctrl, CTRL_NUM_INPUT_LF_BC)];
	}
	else
		eqDisplay[1].fgain = 0.0;

	eqDisplay[1].color = RGB(132, 165, 214);



	// High Freq Eq
	//
	//
	//
	pctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iChan].lpZoneMap, CTRL_NUM_INPUT_HIGHFREQ);
	if(pctrl) // Validate the Pointer !!!
	{
		eqDisplay[4].ffc = InputFreq_HI[GETPHISDATAVALUE(0, pctrl, CTRL_NUM_INPUT_HIGHFREQ)];
	}
	else
		eqDisplay[4].ffc = 5000;

	pctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iChan].lpZoneMap, CTRL_NUM_INPUT_HF_BW);
	if(pctrl) // Validate the Pointer !!!
	{
		eqDisplay[4].fbw = InputBW_ML[GETPHISDATAVALUE(0, pctrl, CTRL_NUM_INPUT_HF_BW)];
	}
	else
		eqDisplay[4].fbw = 1.0;

	pctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iChan].lpZoneMap, CTRL_NUM_INPUT_HF_PEAKSHELF);
	if(  GETPHISDATAVALUE(0, pctrl, CTRL_NUM_INPUT_HF_PEAKSHELF))
	{
		pctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iChan].lpZoneMap, CTRL_NUM_INPUT_HF_BC);
		if(pctrl) // Validate the Pointer !!!
		{
			eqDisplay[4].fgain = InputBC_LF[GETPHISDATAVALUE(0, pctrl, CTRL_NUM_INPUT_HF_BC)];
		}
		else
			eqDisplay[4].fgain = 0.0;

		eqDisplay[4].eqId = EQ_ID_HIGHSHELF;
	}
	else
	{
		pctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iChan].lpZoneMap, CTRL_NUM_INPUT_HF_BC);
		if(pctrl) // Validate the Pointer !!!
		{
			eqDisplay[4].fgain = InputBC_LF[GETPHISDATAVALUE(0, pctrl, CTRL_NUM_INPUT_HF_BC)];
		}
		else
			eqDisplay[4].fgain = 0.0;
		eqDisplay[4].eqId = EQ_ID_BP;
	}
	eqDisplay[4].color = RGB(231, 231, 132);



	// Lo-Mid Eq
	//
	//
	//
	pctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iChan].lpZoneMap, CTRL_NUM_INPUT_LOMIDFREQ);
	if(pctrl) // Validate the Pointer !!!
	{
		eqDisplay[2].ffc = InputFreq_LM[GETPHISDATAVALUE(0, pctrl, CTRL_NUM_INPUT_LOMIDFREQ)];
	}
	else
		eqDisplay[2].ffc = 500;

	pctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iChan].lpZoneMap, CTRL_NUM_INPUT_LM_BW);
	if(pctrl) // Validate the Pointer !!!
	{
		eqDisplay[2].fbw = InputBW_ML[GETPHISDATAVALUE(0, pctrl, CTRL_NUM_INPUT_LM_BW)];
	}
	else
		eqDisplay[2].fbw = 1.0;


	pctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iChan].lpZoneMap, CTRL_NUM_INPUT_LM_BC);
	if(pctrl) // Validate the Pointer !!!
	{
		eqDisplay[2].fgain = InputBC_ML[GETPHISDATAVALUE(0, pctrl, CTRL_NUM_INPUT_LM_BC)];
	}
	else
		eqDisplay[2].fgain = 0.0;

	// adjust for the wierd Gamble filters ... which have different response in the < 0.00
	// area
	if(eqDisplay[2].fgain < 0.00)
	{
		eqDisplay[2].fbw /= 2.0;
	}

	eqDisplay[2].eqId = EQ_ID_BP;
	eqDisplay[2].color = RGB(123, 198, 107);

	// Hi-Mid Eq
	//
	//
	//
	pctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iChan].lpZoneMap, CTRL_NUM_INPUT_HM_BW);
	if(pctrl) // Validate the Pointer !!!
	{
		eqDisplay[3].fbw = InputBW_ML[GETPHISDATAVALUE(0, pctrl, CTRL_NUM_INPUT_HM_BW)];
	}
	else
		eqDisplay[3].fbw = 1.0;

	pctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iChan].lpZoneMap, CTRL_NUM_INPUT_HIMIDFREQ);
	if(pctrl) // Validate the Pointer !!!
	{
		eqDisplay[3].ffc = InputFreq_HM[GETPHISDATAVALUE(0, pctrl, CTRL_NUM_INPUT_HIMIDFREQ)];
	}
	else
		eqDisplay[3].ffc = 2000;

	pctrl = ScanCtrlZonesNum(lpmwd->lpZoneMap[iChan].lpZoneMap, CTRL_NUM_INPUT_HM_BC);
	if(pctrl) // Validate the Pointer !!!
	{
		eqDisplay[3].fgain = InputBC_ML[GETPHISDATAVALUE(0, pctrl, CTRL_NUM_INPUT_HM_BC)];
	}
	else
		eqDisplay[3].fgain = 0.0;

	// adjust for the wierd Gamble filters ... which have different response in the < 0.00
	// area
	if(eqDisplay[3].fgain < 0.00)
	{
		eqDisplay[3].fbw /= 2.0;
	}

	eqDisplay[3].eqId = EQ_ID_BP;
	eqDisplay[3].color = RGB(239, 173, 115);


	//
ON_DRAW_EQ_GRAPH:
	DrawEqGraph(hdc, &rZone, eqDisplay);

};

//////////////////////////////////////////
void    DrawEqGraph(HDC hdc, RECT *rWhere, LPEQ_DISPLAY_DATA pEqData)
{                                  
	POINT pntOrigin;
	HPEN  hpen, hpenOld;
	float a, f, xscale, xshift;
	float bw, fc, gain;
	int   eqId;
	//define size of plot area in pixels
	float graphwidth = (float)rWhere->right;
	float graphheight = (float)rWhere->bottom;
	// set scaling of graph area in db and freq
	float ymax=15.0,fmin=10.0, fmax=20000.0;
	// define some test data storage
	int db;
	int i;

	// set up x scaling
	xscale = (float)log10(fmax/fmin);
	xshift = (float) log10(fmin);


	hpen = CreatePen(PS_SOLID, 1, RGB(96, 96, 96));
	hpenOld = SelectObject(hdc, hpen);
	SetViewportOrgEx(hdc , rWhere->left,(int) (rWhere->top + graphheight/2), &pntOrigin); // center y0 in the viewport
	//Rectangle(hdc, m_rectEllipse);

	//draw some vert grid lines
	MoveToEx( hdc, (int) ( (log10(100)- xshift)*(graphwidth/xscale)),(int) ((-graphheight/2)), (LPPOINT)NULL);
	LineTo( hdc, (int) ( (log10(100)- xshift)*(graphwidth/xscale)),(int) (graphheight/2) );
	MoveToEx( hdc, (int) ( (log10(1000)- xshift)*(graphwidth/xscale)),(int) ((-graphheight/2)), (LPPOINT)NULL);
	LineTo( hdc, (int) ( (log10(1000)- xshift)*(graphwidth/xscale)),(int) (graphheight/2));
	MoveToEx( hdc, (int) ( (log10(10000)- xshift)*(graphwidth/xscale)),(int) ((-graphheight/2)), (LPPOINT)NULL);
	LineTo( hdc, (int) ( (log10(10000)- xshift)*(graphwidth/xscale)),(int) (graphheight/2));
	//draw some horiz grid lines
	for (db=(int)-ymax; db < ymax; db=db+5)
	{
		MoveToEx(hdc, 0, (int) ((-db*graphheight/(2*ymax))), (LPPOINT)NULL) ;
		LineTo(hdc, (int)(graphwidth),  (int) ((-db*graphheight/(2*ymax))) );
	}


	for(i=0; i < 5; i++)
	{
		bw = pEqData[i].fbw;
		gain = pEqData[i].fgain;
		fc = pEqData[i].ffc;
		eqId = pEqData[i].eqId;

		// do the filters
		SelectObject(hdc, hpenOld);
		DeleteObject(hpen);

		hpen = CreatePen(PS_SOLID, 1, pEqData[i].color);
		hpenOld = SelectObject(hdc, hpen);

		// step through frequency range
		for( f = (float)10; f <= fmax; f=f*(float)1.2589 )
		{

			switch(eqId)
			{
			case EQ_ID_BP:
				a=gain * (float) exp( (double)(-1/pow(bw+0.17,2) )* 0.693 *( (f-fc)/fc  - (f-fc)/f ) );
				break;
			case EQ_ID_HIGHSHELF:
				//calc highshelf amplitude
				a = gain * (float) exp((-0.24/pow(bw,1.5)) * 0.693 *pow(-(f-fc)/f,2));
				break;

			case EQ_ID_LOSHELF:
				//calc lowshelf amplitude
				a = gain * (float) exp((-0.24/pow(bw,1.5)) * 0.693 *pow((f-fc)/fc,2));
				break;

			case EQ_ID_LOCUT:
				a = (float)(-10. * log10(1+pow((fc/(f*1.1)),2*3)));
				break;

			default:
				continue;
				break;


			}

			/*
			if ((a < -ymax) || (a > ymax))
			{
			MoveToEx(hdc, (int) ((log10(f)- xshift)*(graphwidth/xscale)), (int) (-a*graphheight/(2*ymax)),
			(LPPOINT)NULL);

			}
			else
			*/
			{

				if(f == 10)
					MoveToEx(hdc, (int) ((log10(f)- xshift)*(graphwidth/xscale)), (int) (-a*graphheight/(2*ymax)),
					(LPPOINT)NULL);
				LineTo( hdc, (int) ((log10(f)- xshift)*(graphwidth/xscale)), (int) (-a*graphheight/(2*ymax)) );
			}
		}
	}


	SetViewportOrgEx(hdc , pntOrigin.x, pntOrigin.y, &pntOrigin); // center y0 in the viewport
	SelectObject(hdc, hpenOld);
	DeleteObject(hpen);

}
