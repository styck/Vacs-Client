//=================================================
// 
// Header Code For Windows 3.x
// This file contains only the CTRL definitions
// CopyRight 1998 CorTek Software, Inc
//=================================================

//=================================================
//
//  Control type definitions
//
//=================================================

// RESOURCE TYPES <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// Phisical mixer description
//---------------------------
// the type of the resource
#define RT_MIXER_PHIS_DESC                              5000

// Type of the RdOut Resource
//---------------------------
#define RT_RDOUTTABLE                                   9000
#define RT_REMAP_SCREENTOMIDI                           9010
#define RT_REMAP_MIDITOSCREEN                           9020

// the type  of the resource
#define     TABLE_OF_ALL_ZONEMAPS                       10000
// definition for the ZONEMAP Descriptor type of resource
//-------------------------------------------------------
// the type  of the resource
#define     ZONEMAPDESCRIPTOR                           10050
// definition for the ZONEMAP type of resource
//--------------------------------------------
// the type  of the resource
#define     ZONEMAP                                     10100



////////////////////////////////////////////////////////////
// Phis Mixer desc resource
//-------------------------
#define MIXER_PHIS_DESC             5100

#define CTRL_ZONE_TERMINATE        -1

// Control Type Numbers
// they represent the location in the
// memory map these MUST be exact
// THESE ARE VERY IMPORTANT
//------------------------------------
#define CTRL_NUM_INPUT_VOL_FADER          0x0000
#define CTRL_NUM_INPUT_CUE_A              0x0001
#define CTRL_NUM_INPUT_GATE_FEED_THRU     0x0002
#define CTRL_NUM_INPUT_LINE_B_VU          0x0003    // ??
#define CTRL_NUM_INPUT_PAN                0x0004
#define CTRL_NUM_INPUT_PAN_DEPTH          0x0005
#define CTRL_NUM_INPUT_CENTER_LEVEL       0x0006
#define CTRL_NUM_INPUT_MONO_LEVEL         0x0007
#define CTRL_NUM_INPUT_MIC_A_TRIM         0x0008
#define CTRL_NUM_INPUT_MIC_A_LEVEL        0x0009
#define CTRL_NUM_INPUT_LINE_A_LEVEL       0x000a
#define CTRL_NUM_INPUT_MIC_B_TRIM         0x000b
#define CTRL_NUM_INPUT_MIC_B_LEVEL        0x000c
#define CTRL_NUM_INPUT_LINE_B_LEVEL       0x000d

#define CTRL_NUM_INPUT_LOWCUT             0x000e  
#define CTRL_NUM_INPUT_LOWCUT_INOUT       0x000f  
#define CTRL_NUM_INPUT_INSERT_INOUT       0x0010

#define CTRL_NUM_INPUT_COMP_THLD          0x0011
#define CTRL_NUM_INPUT_COMP_RATIO         0x0012
#define CTRL_NUM_INPUT_COMP_ATTACK        0x0013
#define CTRL_NUM_INPUT_COMP_RELEASE       0x0014
#define CTRL_NUM_INPUT_COMP_OUTPUTLEVEL   0x0015
#define CTRL_NUM_INPUT_COMP_INOUT         0x0016

#define CTRL_NUM_INPUT_GATE_EQ_FRQ        0x0017
#define CTRL_NUM_INPUT_GATE_EQ_BW         0x0018
#define CTRL_NUM_INPUT_GATE_EQ_BC         0x0019
#define CTRL_NUM_INPUT_GATE_INOUT         0x001a
#define CTRL_NUM_INPUT_GATE_KEYLEVEL      0x001b
#define CTRL_NUM_INPUT_GATE_KEY_INOUT     0x001c

#define CTRL_NUM_INPUT_GATE_FEEDTHRU      0x001d
#define CTRL_NUM_INPUT_GATE_THLD          0x001e
#define CTRL_NUM_INPUT_GATE_ATTACK        0x001f
#define CTRL_NUM_INPUT_GATE_RELEASE       0x0020
#define CTRL_NUM_INPUT_GATE_FEEDTHRUINOUT 0x0021

#define CTRL_NUM_INPUT_HIGHFREQ           0x0022
#define CTRL_NUM_INPUT_HF_BW              0x0023
#define CTRL_NUM_INPUT_HF_BC              0x0024
#define CTRL_NUM_INPUT_HF_PEAKSHELF       0x0025

#define CTRL_NUM_INPUT_HIMIDFREQ          0x0026
#define CTRL_NUM_INPUT_HM_BW              0x0027
#define CTRL_NUM_INPUT_HM_BC              0x0028

#define CTRL_NUM_INPUT_LOMIDFREQ          0x0029
#define CTRL_NUM_INPUT_LM_BW              0x002a
#define CTRL_NUM_INPUT_LM_BC              0x002b

#define CTRL_NUM_INPUT_LOWFREQ            0x002c
#define CTRL_NUM_INPUT_LF_BW              0x002d
#define CTRL_NUM_INPUT_LF_BC              0x002e
#define CTRL_NUM_INPUT_LF_PEAKSHELF       0x002f

#define CTRL_NUM_INPUT_EQ_INOUT           0x0030
#define CTRL_NUM_INPUT_POSTINSERTINOUT    0x0031
// added 08/18/97
#define CTRL_NUM_INPUT_MIC_A_PPWR         0x0032
#define CTRL_NUM_INPUT_MIC_A_CUE          0x0033
#define CTRL_NUM_INPUT_MIC_B_PPWR         0x0034
#define CTRL_NUM_INPUT_MIC_B_CUE          0x0035
// 08/29/1997
#define CTRL_NUM_INPUT_GATE_KEY_VU        0x0036

#define CTRL_NUM_INPUT_AUX1LT_FADER       0x0037
#define CTRL_NUM_INPUT_AUX1RT_FADER       0x0038

#define CTRL_NUM_INPUT_AUX2LT_FADER       0x0039
#define CTRL_NUM_INPUT_AUX2RT_FADER       0x003a

#define CTRL_NUM_INPUT_AUX3LT_FADER       0x003b
#define CTRL_NUM_INPUT_AUX3RT_FADER       0x003c

#define CTRL_NUM_INPUT_AUX4LT_FADER       0x003d
#define CTRL_NUM_INPUT_AUX4RT_FADER       0x003e

#define CTRL_NUM_INPUT_AUX5LT_FADER       0x003f
#define CTRL_NUM_INPUT_AUX5RT_FADER       0x0040

#define CTRL_NUM_INPUT_AUX6LT_FADER       0x0041
#define CTRL_NUM_INPUT_AUX6RT_FADER       0x0042

#define CTRL_NUM_INPUT_AUX7LT_FADER       0x0043
#define CTRL_NUM_INPUT_AUX7RT_FADER       0x0044

#define CTRL_NUM_INPUT_AUX8LT_FADER       0x0045
#define CTRL_NUM_INPUT_AUX8RT_FADER       0x0046

#define CTRL_NUM_INPUT_AUX9LT_FADER       0x0047
#define CTRL_NUM_INPUT_AUX9RT_FADER       0x0048

#define CTRL_NUM_INPUT_AUX10LT_FADER       0x0049
#define CTRL_NUM_INPUT_AUX10RT_FADER       0x004a

#define CTRL_NUM_INPUT_AUX11LT_FADER       0x004b
#define CTRL_NUM_INPUT_AUX11RT_FADER       0x004c

#define CTRL_NUM_INPUT_AUX12LT_FADER       0x004d
#define CTRL_NUM_INPUT_AUX12RT_FADER       0x004e

#define CTRL_NUM_INPUT_AUX13LT_FADER       0x004f
#define CTRL_NUM_INPUT_AUX13RT_FADER       0x0050

#define CTRL_NUM_INPUT_AUX14LT_FADER       0x0051
#define CTRL_NUM_INPUT_AUX14RT_FADER       0x0052

#define CTRL_NUM_INPUT_AUX15LT_FADER       0x0053
#define CTRL_NUM_INPUT_AUX15RT_FADER       0x0054

#define CTRL_NUM_INPUT_AUX16LT_FADER       0x0055
#define CTRL_NUM_INPUT_AUX16RT_FADER       0x0056

#define CTRL_NUM_INPUT_REV_PHASE           0x0057

#define CTRL_NUM_INPUT_SUB8LT_FADER        0x0058
#define CTRL_NUM_INPUT_SUB8RT_FADER        0x0059

#define CTRL_NUM_INPUT_SUB7LT_FADER        0x005a
#define CTRL_NUM_INPUT_SUB7RT_FADER        0x005b

#define CTRL_NUM_INPUT_SUB6LT_FADER        0x005c
#define CTRL_NUM_INPUT_SUB6RT_FADER        0x005d

#define CTRL_NUM_INPUT_SUB5LT_FADER        0x005e
#define CTRL_NUM_INPUT_SUB5RT_FADER        0x005f

#define CTRL_NUM_INPUT_SUB4LT_FADER        0x0060
#define CTRL_NUM_INPUT_SUB4RT_FADER        0x0061

#define CTRL_NUM_INPUT_SUB3LT_FADER        0x0062
#define CTRL_NUM_INPUT_SUB3RT_FADER        0x0063

#define CTRL_NUM_INPUT_SUB2LT_FADER        0x0064
#define CTRL_NUM_INPUT_SUB2RT_FADER        0x0065

#define CTRL_NUM_INPUT_SUB1LT_FADER        0x0066
#define CTRL_NUM_INPUT_SUB1RT_FADER        0x0067

#define CTRL_NUM_INPUT_MASTERLT_LEVEL      0x0068
#define CTRL_NUM_INPUT_MASTERRT_LEVEL      0x0069

//////////////////////////////////////////
// Definitions for Mute Zone Maps
// Dave Jones 4/01/98
//////////////////////////////////////////
#define CTRL_NUM_INPUT_AUX01LT_MUTE			0X006A
#define CTRL_NUM_INPUT_AUX01RT_MUTE			0X006B
#define CTRL_NUM_INPUT_AUX02LT_MUTE			0X006C
#define CTRL_NUM_INPUT_AUX02RT_MUTE			0X006D
#define CTRL_NUM_INPUT_AUX03LT_MUTE			0X006E
#define CTRL_NUM_INPUT_AUX03RT_MUTE			0X006F
#define CTRL_NUM_INPUT_AUX04LT_MUTE			0X0070
#define CTRL_NUM_INPUT_AUX04RT_MUTE			0X0071
#define CTRL_NUM_INPUT_AUX05LT_MUTE			0X0072
#define CTRL_NUM_INPUT_AUX05RT_MUTE			0X0073
#define CTRL_NUM_INPUT_AUX06LT_MUTE			0X0074	
#define CTRL_NUM_INPUT_AUX06RT_MUTE			0X0075
#define CTRL_NUM_INPUT_AUX07LT_MUTE			0X0076
#define CTRL_NUM_INPUT_AUX07RT_MUTE			0X0077
#define CTRL_NUM_INPUT_AUX08LT_MUTE			0X0078
#define CTRL_NUM_INPUT_AUX08RT_MUTE			0X0079
#define CTRL_NUM_INPUT_AUX09LT_MUTE			0X007A
#define CTRL_NUM_INPUT_AUX09RT_MUTE			0X007B
#define CTRL_NUM_INPUT_AUX10LT_MUTE			0X007C
#define CTRL_NUM_INPUT_AUX10RT_MUTE			0X007D	
#define CTRL_NUM_INPUT_AUX11LT_MUTE			0X007E	
#define CTRL_NUM_INPUT_AUX11RT_MUTE			0X007F
#define CTRL_NUM_INPUT_AUX12LT_MUTE			0X0080	
#define CTRL_NUM_INPUT_AUX12RT_MUTE			0X0081
#define CTRL_NUM_INPUT_AUX13LT_MUTE			0X0082
#define CTRL_NUM_INPUT_AUX13RT_MUTE			0X0083
#define CTRL_NUM_INPUT_AUX14LT_MUTE			0X0084
#define CTRL_NUM_INPUT_AUX14RT_MUTE			0X0085
#define CTRL_NUM_INPUT_AUX15LT_MUTE			0X0086
#define CTRL_NUM_INPUT_AUX15RT_MUTE			0X0087
#define CTRL_NUM_INPUT_AUX16LT_MUTE			0X0088
#define CTRL_NUM_INPUT_AUX16RT_MUTE			0X0089

#define CTRL_NUM_INPUT_AUX16BLT_MUTE		0X008A
#define CTRL_NUM_INPUT_AUX16BRT_MUTE		0X008B

//////////////////////////////////////////
// Definitions for Mute Zone Maps
// Dave Jones 3/30/98
//////////////////////////////////////////
#define CTRL_NUM_INPUT_AUX01LT_POST   	0X008C
#define CTRL_NUM_INPUT_AUX01RT_POST			0X008D
#define CTRL_NUM_INPUT_AUX02LT_POST			0X008E
#define CTRL_NUM_INPUT_AUX02RT_POST			0X008F
#define CTRL_NUM_INPUT_AUX03LT_POST			0X0090
#define CTRL_NUM_INPUT_AUX03RT_POST			0X0091
#define CTRL_NUM_INPUT_AUX04LT_POST			0X0092
#define CTRL_NUM_INPUT_AUX04RT_POST			0X0093
#define CTRL_NUM_INPUT_AUX05LT_POST			0X0094
#define CTRL_NUM_INPUT_AUX05RT_POST			0X0095
#define CTRL_NUM_INPUT_AUX06LT_POST			0X0096	
#define CTRL_NUM_INPUT_AUX06RT_POST			0X0097
#define CTRL_NUM_INPUT_AUX07LT_POST			0X0098
#define CTRL_NUM_INPUT_AUX07RT_POST			0X0099
#define CTRL_NUM_INPUT_AUX08LT_POST			0X009A
#define CTRL_NUM_INPUT_AUX08RT_POST			0X009B
#define CTRL_NUM_INPUT_AUX09LT_POST			0X009C
#define CTRL_NUM_INPUT_AUX09RT_POST			0X009D
#define CTRL_NUM_INPUT_AUX10LT_POST			0X009E
#define CTRL_NUM_INPUT_AUX10RT_POST			0X009F	
#define CTRL_NUM_INPUT_AUX11LT_POST			0X00a0	
#define CTRL_NUM_INPUT_AUX11RT_POST			0X00a1
#define CTRL_NUM_INPUT_AUX12LT_POST			0X00a2	
#define CTRL_NUM_INPUT_AUX12RT_POST			0X00a3
#define CTRL_NUM_INPUT_AUX13LT_POST			0x00a4
#define CTRL_NUM_INPUT_AUX13RT_POST			0x00a5
#define CTRL_NUM_INPUT_AUX14LT_POST			0x00a6
#define CTRL_NUM_INPUT_AUX14RT_POST			0x00a7
#define CTRL_NUM_INPUT_AUX15LT_POST			0x00a8
#define CTRL_NUM_INPUT_AUX15RT_POST			0x00a9
#define CTRL_NUM_INPUT_AUX16LT_POST			0x00aa
#define CTRL_NUM_INPUT_AUX16RT_POST			0x00ab

// Hristo add ...

#define CTRL_NUM_INPUT_CUE_FAD_PRE			0x00ac            
#define CTRL_NUM_INPUT_CUE_FAD_POST     0x00ad           

#define CTRL_NUM_INPUT_MICB_LEVEL           0x00ae  
#define CTRL_NUM_INPUT_LINEB_LEVEL          0x00af  
#define CTRL_NUM_INPUT_MICA_LEVEL           0x00b0   
#define CTRL_NUM_INPUT_LINEA_LEVEL          0x00b1        
#define CTRL_NUM_INPUT_DIRECT_OUTPOST       0x00b2        

#define CTRL_NUM_INPUT_MUTE                 0x00b3        
#define CTRL_NUM_INPUT_VU_METER             0x00b4

#define	CTRL_NUM_INPUT_AUX16LT_LINEB_FADER	0x00b5
#define CTRL_NUM_INPUT_AUX16RT_LINEB_FADER	0x00b6
#define CTRL_NUM_INPUT_SIGPATH_LINE_B       0X00b7

#define CTRL_NUM_INPUT_PREPOST_FADER_VU     0x00b8
#define CTRL_NUM_INPUT_GATE_KEY_CUE_A       0x00b9


#define CTRL_NUM_INPUT_COMP_VU              0x00ba
#define CTRL_NUM_INPUT_GATE_VU              0x00bb

#define CTRL_NUM_INPUT_LABEL                0x00bc
#define CTRL_NUM_INPUT_SAFE                 0x00bd
 
#define CTRL_NUM_CLIP_PRE_FADER							0x00be
#define CTRL_NUM_CLIP_POST_FADER						0x00bf

#define CTRL_NUM_RESET_EQ										0x00c0		// fds added 9/4/2000, reset eq button
 
#define CTRL_NUM_NULL                      0xffff


// definitions for the Matrix Module
//----------------------------------
//typedef enum //CTRL_NUM_MATRIX_enum
//{
// Matrix stuff within the SUB-AUX screen module
// please keep 
// __SUB_MATRIX_GROUP_01_START = firtst matrix control number in the group
// __SUB_MATRIX_GROUP_01_END   = last  matrix control number in the group
//
#define	 MATRIX_OFFSET																0X100
#define  CTRL_NUM_MATRIX_HI_FREQ_LT                    (1+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_HF_BW_LT                      (2+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_HF_BC_LT                      (3+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_HF_PK_SHELF_LT                (4+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_HI_FREQ_RT                    (5+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_HF_BW_RT                      (6+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_HF_BC_RT                      (7+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_HF_PK_SHELF_RT                (8+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_HM_FREQ_LT                    (9+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_HM_BW_LT											(10+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_HM_BC_LT											(11+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_HM_FREQ_RT										(12+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_HM_BW_RT											(13+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_HM_BC_RT											(14+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_LM_FREQ_LT                   (15+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_LM_BW_LT                     (16+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_LM_BC_LT                     (17+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_LM_FREQ_RT                   (18+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_LM_BW_RT                     (19+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_LM_BC_RT                     (20+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_LO_FREQ_LT                   (21+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_LF_BW_LT                     (22+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_LF_BC_LT                     (23+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_LF_PK_SHELF_LT               (24+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_LO_FREQ_RT                   (25+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_LF_BW_RT                     (26+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_LF_BC_RT                     (27+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_LF_PK_SHELF_RT               (28+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_LT_FADER                     (29+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_RT_FADER                     (30+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX16LT                      (31+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX16RT                      (32+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX15LT                      (33+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX15RT                      (34+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX14LT                      (35+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX14RT                      (36+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX13LT                      (37+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX13RT                      (38+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX12LT                      (39+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX12RT                      (40+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX11LT                      (41+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX11RT                      (42+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX10LT                      (43+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX10RT                      (44+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX09LT                      (45+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX09RT                      (46+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX08LT                      (47+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX08RT                      (48+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX07LT                      (49+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX07RT                      (50+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX06LT                      (51+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX06RT                      (52+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX05LT                      (53+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX05RT                      (54+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX04LT                      (55+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX04RT                      (56+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX03LT                      (57+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX03RT                      (58+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX02LT                      (59+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX02RT                      (60+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX01LT                      (61+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX01RT                      (62+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB08LT                      (63+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB08RT                      (64+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB07LT                      (65+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB07RT                      (66+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB06LT                      (67+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB06RT                      (68+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB05LT                      (69+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB05RT                      (70+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB04LT                      (71+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB04RT                      (72+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB03LT                      (73+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB03RT                      (74+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB02LT                      (75+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB02RT                      (76+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB01LT                      (77+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB01RT                      (78+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_MASTER_PRELT                 (79+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_MASTER_PRERT                 (80+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_MONO_MATRIX                  (81+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_CENTER_MATRIX                (82+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUMINRT                      (83+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUMINLT                      (84+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX16LT_MUTE                 (85+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX16RT_MUTE                 (86+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX15LT_MUTE                 (87+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX15RT_MUTE                 (88+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX14LT_MUTE                 (89+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX14RT_MUTE                 (90+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX13LT_MUTE                 (91+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX13RT_MUTE                 (92+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX12LT_MUTE                 (93+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX12RT_MUTE                 (94+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX11LT_MUTE                 (95+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX11RT_MUTE                 (96+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX10LT_MUTE                 (97+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX10RT_MUTE                 (98+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX09LT_MUTE                 (99+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX09RT_MUTE                (100+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX08LT_MUTE                (101+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX08RT_MUTE                (102+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX07LT_MUTE                (103+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX07RT_MUTE                (104+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX06LT_MUTE                (105+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX06RT_MUTE                (106+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX05LT_MUTE                (107+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX05RT_MUTE                (108+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX04LT_MUTE                (109+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX04RT_MUTE                (110+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX03LT_MUTE                (111+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX03RT_MUTE                (112+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX02LT_MUTE                (113+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX02RT_MUTE                (114+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX01LT_MUTE                (115+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_AUX01RT_MUTE                (116+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB08LT_MUTE                (117+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB08RT_MUTE                (118+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB07LT_MUTE                (119+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB07RT_MUTE                (120+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB06LT_MUTE                (121+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB06RT_MUTE                (122+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB05LT_MUTE                (123+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB05RT_MUTE                (124+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB04LT_MUTE                (125+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB04RT_MUTE                (126+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB03LT_MUTE                (127+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB03RT_MUTE                (128+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB02LT_MUTE                (129+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB02RT_MUTE                (130+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB01LT_MUTE                (131+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB01RT_MUTE                (132+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_MASTERLT_MUTE               (133+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_MASTERRT_MUTE               (134+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_MONO_MUTE                   (135+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_CENTER_MUTE                 (136+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIXLT_MUTE                      (137+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIXRT_MUTE                      (138+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIXLT_INSERT                    (139+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIXRT_INSERT                    (140+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_CUE_PRE                     (141+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_CUE_POST                    (142+MATRIX_OFFSET)
//
//
//
#define  CTRL_NUM_MATRIX_MASTERLT_PRE                (149+MATRIX_OFFSET)

#define  CTRL_NUM_MATRIX_MASTERRT_POST               (150+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_LT_INOUT                    (151+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_RT_INOUT                    (152+MATRIX_OFFSET) 
#define  CTRL_NUM_MATRIX_VU_METER                    (153+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX2_VU_METER                   (154+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_MONO                        (155+MATRIX_OFFSET)
#define  CTRL_NUM_AUX_LABEL                          (156+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_EQ_LEFTRIGHT_SELECTOR       (157+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_POST_LT_VU                  (158+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_POST_RT_VU                  (159+MATRIX_OFFSET)
#define  CTRL_NUM_SUB_POST_LT_VU                     (160+MATRIX_OFFSET)
#define  CTRL_NUM_SUB_POST_RT_VU                     (161+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_LABEL                       (162+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_CLIP_LIGHT									 (163+MATRIX_OFFSET)
#define  __SUB_MATRIX_GROUP_01_START                 CTRL_NUM_MATRIX_HI_FREQ_LT
#define  __SUB_MATRIX_GROUP_01_END                   CTRL_NUM_MATRIX_CLIP_LIGHT
//
// END for the first Matrix group ...  .. there might be another one
//



// Sub-aux stuff within the SUB-AUX screen module
// please keep 
// __SUB_AUX_GROUP_01_START = firtst sub_aux control number in the group
// __SUB_AUX_GROUP_01_END   = last  sub_aux control number in the group
//
#define  CTRL_NUM_MATRIX_SUB_VOL_LT                   (180+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB_VOL_RT                   (181+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIXLT_MUTE_SUB                   (182+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIXRT_MUTE_SUB                   (183+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIXLT_INSERT_SUB                 (184+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIXRT_INSERT_SUB                 (185+MATRIX_OFFSET)

#define  CTRL_NUM_MATRIX_SUB_LT_SUM_IN                (186+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_SUB_RT_SUM_IN                (187+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_STERIO_CUE_PRE               (188+MATRIX_OFFSET)
#define  CTRL_NUM_MATRIX_STERIO_CUE_POST              (189+MATRIX_OFFSET)
#define  CTRL_NUM_SUB_SUMIN_LT_MUTE										(190+MATRIX_OFFSET)
#define  CTRL_NUM_SUB_SUMIN_RT_MUTE                   (191+MATRIX_OFFSET)
#define  CTRL_NUM_SUB_CLIP_LIGHT										  (192+MATRIX_OFFSET)
#define  __SUB_AUX_GROUP_01_START                     CTRL_NUM_MATRIX_SUB_VOL_LT
#define  __SUB_AUX_GROUP_01_END                       CTRL_NUM_SUB_CLIP_LIGHT
//
// END for the first sub_aux group ... 
//

//};
       
	
// definitions for the Master Module
//----------------------------------

// Belonging to the master module ... !!!!!!!!!!
#define CTRL_NUM_MASTERLT_FADER               1
#define CTRL_NUM_MASTERRT_FADER               2
#define CTRL_NUM_MASTERLT_CENTER_FADER        3
#define CTRL_NUM_MASTERLT_MONO_FADER          4

#define CTRL_NUM_MASTERLT_CENTER_SUM          5
#define CTRL_NUM_MASTERLT_MONO_SUM            6

#define CTRL_NUM_MASTERLT_SUM_IN							7
#define CTRL_NUM_MASTERRT_SUM_IN							8
#define CTRL_NUM_MASTER_VU_METER              9

#define CTRL_NUM_MASTER_MUTE         			  	10
#define CTRL_NUM_MASTER_STEREO_CUE_PRE	      11
#define CTRL_NUM_MASTER_STEREO_CUE_POST  			12


#define CTRL_NUM_MASTER_INSERTLT			      	13
#define CTRL_NUM_MASTER_INSERTRT			      	14

#define CTRL_NUM_MASTER_CUE_LEVEL_CENTER      15
#define CTRL_NUM_MASTER_CUE_LEVEL_MONO        16
#define __MASTER_GROUP_01_START               CTRL_NUM_MASTERLT_FADER
#define __MASTER_GROUP_01_END                 CTRL_NUM_MASTER_CUE_LEVEL_MONO

////////////////////////////////////////////////
#define CTRL_NUM_MASTER_AUX16LT_FADER					17
#define CTRL_NUM_MASTER_AUX16RT_FADER					18

#define CTRL_NUM_MASTER_AUX15LT_FADER					19
#define CTRL_NUM_MASTER_AUX15RT_FADER					20

#define CTRL_NUM_MASTER_AUX14LT_FADER					21
#define CTRL_NUM_MASTER_AUX14RT_FADER					22

#define CTRL_NUM_MASTER_AUX13LT_FADER					23
#define CTRL_NUM_MASTER_AUX13RT_FADER					24

#define CTRL_NUM_MASTER_AUX12LT_FADER					25
#define CTRL_NUM_MASTER_AUX12RT_FADER					26

#define CTRL_NUM_MASTER_AUX11LT_FADER					27
#define CTRL_NUM_MASTER_AUX11RT_FADER					28

#define CTRL_NUM_MASTER_AUX10LT_FADER					29
#define CTRL_NUM_MASTER_AUX10RT_FADER					30

#define CTRL_NUM_MASTER_AUX9LT_FADER					31
#define CTRL_NUM_MASTER_AUX9RT_FADER					32

#define CTRL_NUM_MASTER_AUX8LT_FADER					33
#define CTRL_NUM_MASTER_AUX8RT_FADER					34

#define CTRL_NUM_MASTER_AUX7LT_FADER					35
#define CTRL_NUM_MASTER_AUX7RT_FADER					36

#define CTRL_NUM_MASTER_AUX6LT_FADER					37
#define CTRL_NUM_MASTER_AUX6RT_FADER					38

#define CTRL_NUM_MASTER_AUX5LT_FADER					39
#define CTRL_NUM_MASTER_AUX5RT_FADER					40

#define CTRL_NUM_MASTER_AUX4LT_FADER					41
#define CTRL_NUM_MASTER_AUX4RT_FADER					42

#define CTRL_NUM_MASTER_AUX3LT_FADER					43
#define CTRL_NUM_MASTER_AUX3RT_FADER					44

#define CTRL_NUM_MASTER_AUX2LT_FADER					45
#define CTRL_NUM_MASTER_AUX2RT_FADER					46

#define CTRL_NUM_MASTER_AUX1LT_FADER					47
#define CTRL_NUM_MASTER_AUX1RT_FADER					48

#define CTRL_NUM_MASTER_SUMIN16							49
#define CTRL_NUM_MASTER_SUMIN15							50
#define CTRL_NUM_MASTER_SUMIN14							51
#define CTRL_NUM_MASTER_SUMIN13							52
#define CTRL_NUM_MASTER_SUMIN12							53
#define CTRL_NUM_MASTER_SUMIN11							54
#define CTRL_NUM_MASTER_SUMIN10							55
#define CTRL_NUM_MASTER_SUMIN9							56
#define CTRL_NUM_MASTER_SUMIN8							57
#define CTRL_NUM_MASTER_SUMIN7							58
#define CTRL_NUM_MASTER_SUMIN6							59
#define CTRL_NUM_MASTER_SUMIN5							60
#define CTRL_NUM_MASTER_SUMIN4							61
#define CTRL_NUM_MASTER_SUMIN3							62
#define CTRL_NUM_MASTER_SUMIN2							63
#define CTRL_NUM_MASTER_SUMIN1							64

//////////////////////////////////////////
//TB AUX 1-16 MASTER
//////////////////////////////////////////

#define CTRL_NUM_MASTER_AUX16_TB							65
#define CTRL_NUM_MASTER_AUX15_TB							66
#define CTRL_NUM_MASTER_AUX14_TB							67
#define CTRL_NUM_MASTER_AUX13_TB							68
#define CTRL_NUM_MASTER_AUX12_TB							69
#define CTRL_NUM_MASTER_AUX11_TB							70
#define CTRL_NUM_MASTER_AUX10_TB							71
#define CTRL_NUM_MASTER_AUX09_TB							72
#define CTRL_NUM_MASTER_AUX08_TB							73
#define CTRL_NUM_MASTER_AUX07_TB							74
#define CTRL_NUM_MASTER_AUX06_TB							75
#define CTRL_NUM_MASTER_AUX05_TB							76
#define CTRL_NUM_MASTER_AUX04_TB							77
#define CTRL_NUM_MASTER_AUX03_TB							78
#define CTRL_NUM_MASTER_AUX02_TB							79
#define CTRL_NUM_MASTER_AUX01_TB							80

#define CTRL_NUM_MASTER_TB_LT                 81
#define CTRL_NUM_MASTER_TB_RT                 82

#define CTRL_NUM_MASTER_PINK_NOISE						83
#define CTRL_NUM_MASTER_OSC_LEVEL							84
#define CTRL_NUM_MASTER_OSC_FREQ							85

#define CTRL_NUM_MASTER_TB_MICLEVEL						86
#define CTRL_NUM_MASTER_TB_LINELEVEL					87

#define CTRL_NUM_MASTER_CUE_A_INPUT						88
#define CTRL_NUM_MASTER_CUE_B_INPUT						89
#define CTRL_NUM_MASTER_CUE_A_SUB							90
#define CTRL_NUM_MASTER_CUE_B_SUB							91

#define CTRL_NUM_MASTER_CUE_A_AUX							92
#define CTRL_NUM_MASTER_CUE_B_AUX							93

#define CTRL_NUM_MASTER_CUE_A_MATRIX					94
#define CTRL_NUM_MASTER_CUE_B_MATRIX					95
#define CTRL_NUM_MASTER_CUE_A_MASTER					96
#define CTRL_NUM_MASTER_CUE_B_MASTER					97
#define CTRL_NUM_MASTER_CUE_A_SUM_IN					98
#define CTRL_NUM_MASTER_CUE_B_SUM_IN					99

#define CTRL_NUM_MASTER_HEADPHONES						100
#define CTRL_NUM_MASTER_CUE_TALKBACK					101

#define CTRL_NUM_MASTERLT_CUE_A_LT						102
#define CTRL_NUM_MASTERLT_CUE_A_RT						103
#define CTRL_NUM_MASTERLT_CUE_B_LT						104
#define CTRL_NUM_MASTERLT_CUE_B_RT						105

#define CTRL_NUM_MASTER_AUX16PRE	            106
#define CTRL_NUM_MASTER_AUX16POST	            107
#define CTRL_NUM_MASTER_AUX15PRE	            108
#define CTRL_NUM_MASTER_AUX15POST	            109
#define CTRL_NUM_MASTER_AUX14PRE	            110
#define CTRL_NUM_MASTER_AUX14POST	            111
#define CTRL_NUM_MASTER_AUX13PRE	            112
#define CTRL_NUM_MASTER_AUX13POST	            113
#define CTRL_NUM_MASTER_AUX12PRE	            114
#define CTRL_NUM_MASTER_AUX12POST	            115
#define CTRL_NUM_MASTER_AUX11PRE	            116
#define CTRL_NUM_MASTER_AUX11POST	            117
#define CTRL_NUM_MASTER_AUX10PRE	            118
#define CTRL_NUM_MASTER_AUX10POST	            119
#define CTRL_NUM_MASTER_AUX09PRE	            120
#define CTRL_NUM_MASTER_AUX09POST	            121
#define CTRL_NUM_MASTER_AUX08PRE	            122
#define CTRL_NUM_MASTER_AUX08POST	            123
#define CTRL_NUM_MASTER_AUX07PRE	            124
#define CTRL_NUM_MASTER_AUX07POST	            125
#define CTRL_NUM_MASTER_AUX06PRE	            126
#define CTRL_NUM_MASTER_AUX06POST	            127
#define CTRL_NUM_MASTER_AUX05PRE	            128
#define CTRL_NUM_MASTER_AUX05POST	            129
#define CTRL_NUM_MASTER_AUX04PRE	            130
#define CTRL_NUM_MASTER_AUX04POST	            131
#define CTRL_NUM_MASTER_AUX03PRE	            132
#define CTRL_NUM_MASTER_AUX03POST	            133
#define CTRL_NUM_MASTER_AUX02PRE	            134
#define CTRL_NUM_MASTER_AUX02POST	            135
#define CTRL_NUM_MASTER_AUX01PRE	            136
#define CTRL_NUM_MASTER_AUX01POST	            137

//////////////////////////////////////////////////////////
// MASTER MUTES,  POWER,  PRE/POST, INSERT,  CUE

#define CTRL_NUM_MASTER_TALKBACK_POWER				138
#define CTRL_NUM_MASTER_CUE_MUTE			        139

#define CTRL_NUM_MASTER_CUEA_SYSTEM_SEL				140
#define CTRL_NUM_MASTER_CUEB_SYSTEM_SEL				141

#define CTRL_NUM_MASTER_SUB08_TB              142
#define CTRL_NUM_MASTER_SUB07_TB              143
#define CTRL_NUM_MASTER_SUB06_TB              144
#define CTRL_NUM_MASTER_SUB05_TB              145
#define CTRL_NUM_MASTER_SUB04_TB              146
#define CTRL_NUM_MASTER_SUB03_TB              147
#define CTRL_NUM_MASTER_SUB02_TB              148
#define CTRL_NUM_MASTER_SUB01_TB              149

////////////////////////////////////////////
//Definitions for Zone Maps MASTER_CUE Mutes
// Dave 4/1/98
////////////////////////////////////////////

#define CTRL_NUM_MASTER_CUE_AUX16_MUTE        150
#define CTRL_NUM_MASTER_CUE_AUX15_MUTE        151
#define CTRL_NUM_MASTER_CUE_AUX14_MUTE        152
#define CTRL_NUM_MASTER_CUE_AUX13_MUTE        153
#define CTRL_NUM_MASTER_CUE_AUX12_MUTE        154
#define CTRL_NUM_MASTER_CUE_AUX11_MUTE        155
#define CTRL_NUM_MASTER_CUE_AUX10_MUTE        156
#define CTRL_NUM_MASTER_CUE_AUX09_MUTE        157
#define CTRL_NUM_MASTER_CUE_AUX08_MUTE        158
#define CTRL_NUM_MASTER_CUE_AUX07_MUTE        159
#define CTRL_NUM_MASTER_CUE_AUX06_MUTE        160
#define CTRL_NUM_MASTER_CUE_AUX05_MUTE        161
#define CTRL_NUM_MASTER_CUE_AUX04_MUTE        162
#define CTRL_NUM_MASTER_CUE_AUX03_MUTE        163
#define CTRL_NUM_MASTER_CUE_AUX02_MUTE        164
#define CTRL_NUM_MASTER_CUE_AUX01_MUTE        165
                                              
#define CTRL_NUM_MASTER_HEADPHONES_PRE        166

#define __MASTER_GROUP_02_START               CTRL_NUM_MASTER_AUX16LT_FADER
#define __MASTER_GROUP_02_END                 CTRL_NUM_MASTER_HEADPHONES_PRE

#define CTRL_NUM_MASTER_LABEL                 180
#define CTRL_NUM_AUX1_LABEL_LOOKUP            200 // Lookup Labels
#define CTRL_NUM_AUX2_LABEL_LOOKUP            201 // Lookup Labels
#define CTRL_NUM_AUX3_LABEL_LOOKUP            202 // Lookup Labels
#define CTRL_NUM_AUX4_LABEL_LOOKUP            203 // Lookup Labels
#define CTRL_NUM_AUX5_LABEL_LOOKUP            204 // Lookup Labels
#define CTRL_NUM_AUX6_LABEL_LOOKUP            205 // Lookup Labels
#define CTRL_NUM_AUX7_LABEL_LOOKUP            206 // Lookup Labels
#define CTRL_NUM_AUX8_LABEL_LOOKUP            207 // Lookup Labels
#define CTRL_NUM_AUX9_LABEL_LOOKUP            208 // Lookup Labels
#define CTRL_NUM_AUX10_LABEL_LOOKUP           209 // Lookup Labels
#define CTRL_NUM_AUX11_LABEL_LOOKUP           210 // Lookup Labels
#define CTRL_NUM_AUX12_LABEL_LOOKUP           211 // Lookup Labels
#define CTRL_NUM_AUX13_LABEL_LOOKUP           212 // Lookup Labels
#define CTRL_NUM_AUX14_LABEL_LOOKUP           213 // Lookup Labels
#define CTRL_NUM_AUX15_LABEL_LOOKUP           214 // Lookup Labels
#define CTRL_NUM_AUX16_LABEL_LOOKUP           215 // Lookup Labels

#define CTRL_NUM_MASTER_POST_LT_VU            216
#define CTRL_NUM_MASTER_POST_RT_VU            217

#define CTRL_NUM_MASTER_CLIP_LIGHT						218
#define CTRL_NUM_CUE_CLIP_LIGHT								219


//////////////////////////////////////////
// Control Types
// they represent the function that
// shold hadle a control
//------------------------------------
#define CTRL_TYPE_BUTTON_REG        0x0000
#define CTRL_TYPE_FADER_VERT        0x0001
#define CTRL_TYPE_FADER_HORZ        0x0002
#define CTRL_TYPE_STRING_UPDATE     0x0003
#define CTRL_TYPE_UPDOWN            0x0004
#define CTRL_TYPE_UPDOWNSCROLL      0x0005

#define CTRL_TYPE_VU_DISPLAY        0x0006
#define CTRL_TYPE_VU_DISPLAY1       0x0007
#define CTRL_TYPE_VU_DISPLAY2       0x0008
#define CTRL_TYPE_VU_DISPLAY3       0x0009

#define CTRL_TYPE_VU_DISPLAY4       0x000a
#define CTRL_TYPE_VU_DISPLAY5       0x000b
#define CTRL_TYPE_VU_DISPLAY6       0x000c
#define CTRL_TYPE_VU_DISPLAY7       0x000d


#define CTRL_TYPE_VU_DISPLAY8       0x000e
#define CTRL_TYPE_VU_DISPLAY9       0x000f
#define CTRL_TYPE_VU_DISPLAY10      0x0010
#define CTRL_TYPE_VU_DISPLAY11      0x0011
/*
#define CTRL_TYPE_VU_DISPLAY12      0x0012
#define CTRL_TYPE_VU_DISPLAY13      0x0013
#define CTRL_TYPE_VU_DISPLAY14      0x0014
#define CTRL_TYPE_VU_DISPLAY15      0x0015
*/

#define CTRL_TYPE_LEFTRIGHT         0x0016

#define CTRL_TYPE_CLIP_LIGHT				0x001a

#define CTRL_TYPE_LABEL             0x001f

#define CTRL_TYPE_BTN_INPUTMUTE_FILTER  0x0020
#define CTRL_TYPE_BTN_AUXMUTE_FILTER    0x0021
#define CTRL_MASTER_MUTE_FILTER         0x0022
#define CTRL_MARIXLT_MUTE_FILTER        0x0023 
#define CTRL_MARIXRT_MUTE_FILTER        0x0024
#define CTRL_MASTER_AUX_MUTE_FILTER     0x0025

#define CTRL_INPUT_MICA_MUTE_FILTER     0x0026
#define CTRL_INPUT_LINEA_MUTE_FILTER    0x0027
#define CTRL_INPUT_MICB_MUTE_FILTER     0x0028
#define CTRL_INPUT_LINEB_MUTE_FILTER    0x0029
#define CTRL_MATRIX_SUB_MUTE_FILTER     0x002a
#define CTRL_MATRIX_AUX_MUTE_FILTER     0x002b
#define CTRL_MATRIX_MASTER_LT_MUTE_FILTER  0x002c
#define CTRL_MATRIX_MASTER_RT_MUTE_FILTER  0x002d
#define CTRL_MATRIX_MONO_MUTE_FILTER    0x002e
#define CTRL_MATRIX_CENTER_MUTE_FILTER  0x002f
#define CTRL_CUE_MASTER_MUTE_FILTER     0x0030
#define CTRL_TYPE_INPUT_GATE_IN_BTN_FILTER		0x0031
#define CTRL_SUB_SUMIN_MUTE_FILTER			0x0032
#define CTRL_INPUT_AUX16B_MUTE_FILTER  0x0033		// LINE B TO AUX 16 MUTE LEFT/RIGHT


#define CTRL_TYPE_BTN_INPUT_AUXPOST_FILTER    0x0040
#define CTRL_TYPE_BTN_MASTER_HEADPOST_FILTER  0x0041
#define CTRL_TYPE_BTN_AUX_MMATRIX_LT_POST_FILTER 0x0042
#define CTRL_TYPE_BTN_AUX_MMATRIX_RT_POST_FILTER 0x0043
#define CTRL_TYPE_BTN_AUX_MMATRIX_RT_POST_FILTER 0x0043
#define CTRL_TYPE_BTN_MASTER_AUX_PREPOST_FILTER  0x0044

#define CTRL_TYPE_DISP_INPUT_EQ_FILTER            0x0050
#define CTRL_TYPE_DISP_SUB_EQ_FILTER              0x0051

#define CTRL_TYPE_BTN_EQ_RESET_FILTER              0x0052

#define CTRL_TYPE_CHANNEL_NAME_DISPLAY_VERTICAL	0x8000
#define CTRL_TYPE_CHANNEL_NUMBER_DISPLAY_VERTICAL 0x8001

#define CTRL_TYPE_OPEN_EXPLODE      0xff00
#define CTRL_TYPE_OPEN_ZOOM_IAUX    0xff01
#define CTRL_TYPE_OPEN_ZOOM_CCOMP   0xff02
#define CTRL_TYPE_OPEN_ZOOM_EQ		  0xff03
#define CTRL_TYPE_OPEN_ZOOM_SUB		  0xff04
#define CTRL_TYPE_OPEN_FADER				0xff05

#define CTRL_TYPE_OPEN_MATRIX_EQ		0xff06
#define CTRL_TYPE_OPEN_MATRIX_AUX		0xff07
#define CTRL_TYPE_OPEN_MATRIX_SUB		0xff08

#define CTRL_TYPE_NULL              0xffff


// the actual Jump Zoom offsets
#define JUMP_TO_INPUT_AUX						904
#define JUMP_TO_INPUT_COMP					1844
#define JUMP_TO_INPUT_EQ						1302
#define JUMP_TO_INPUT_SUB						2878
#define JUMP_TO_INPUT_FADER					3950
#define JUMP_TO_MATRIX_EQ						18
#define JUMP_TO_MATRIX_AUX					1524
#define JUMP_TO_MATRIX_SUB					2090

// Display types
// used in many ways
//------------------
#define DISP_TYPE_2STATE            0x0000
#define DISP_TYPE_NORMAL            0x0001
#define DISP_TYPE_NULL              0xffff




/////////////////////////////////////////////////
/////////////////////////////////////////////////
//
//  ZONE MAP DEFINITIONS
//
/////////////////////////////////////////////////
/////////////////////////////////////////////////

// define the range of the custom resources
// the Zonemap descriptors should be in the
// rnage of 11000-12000.
// The Zonemaps should be in the range of
// 15000-16000.
//-----------------------------------------

#define     MIN_ZONEMAPDESCRIPTOR                      11000
#define     MAX_ZONEMAPDESCRIPTOR                      12000

#define     MIN_ZONEMAP_RES                            15000
#define     MAX_ZONEMAP_RES                            16000


// This table contains all of the
// Zonemap descriptors with their
// id string
//-------------------------------

// the actual identifier of the table with descriptors
#define     IDTAZ_FULL                                  10000
#define     IDTAZ_ZOOM                                  10001


// Begin the Zonemap Descriptor definitions
//-----------------------------------------

#define IDZD_FULL_ZONEMAP                             11000
#define IDZD_ZOOM_ZONEMAP                             11001

// End the Zonemap Descriptor definitions
//---------------------------------------


// Begin of the Zone Map definitions
// these are the values for the Zone maps
// they must between MIN_ZONEMAP_RES and MAX_ZONEMAP_RES
//------------------------------------------------------
#define IDZM_INPUT_FULL                               0x3A00 //
#define IDZM_SA_FULL                                  0x3A01
#define IDZM_SB_FULL                                  0x3A02
#define IDZM_SC_FULL                                  0x3A03
#define IDZM_M_FULL                                   0x3A04

#define IDZM_INPUT_ZOOM                               0x3B00
#define IDZM_SUBMATRIX_ZOOM                           0x3B01
#define IDZM_MASTER_ZOOM                              0x3B02

//#define IDZM_SA_ZOOM                                  0x3B01
//#define IDZM_SB_ZOOM                                  0x3B02
//#define IDZM_SC_ZOOM                                  0x3B03
//#define IDZM_M_ZOOM                                   0x3B04

//End of the Zone Maps definitions
//--------------------------------

/////////////////////////////////////////////////
/////////////////////////////////////////////////
//
//  RDOUT MAP DEFINITIONS
//
/////////////////////////////////////////////////
/////////////////////////////////////////////////

// RDOUT definitions
//------------------
#define RDOUT_NORDOUT                               0x2300 
#define RDOUT_I_FADER_VOL_N_FULL                    0x2301 
#define RDOUT_I_FADER_VOL_N_ZOOM                    0x2302 
#define RDOUT_I_FADER_GATE_N                        0x2304 
#define RDOUT_I_FADER_RATIO_N                       0x2305 
#define RDOUT_I_FADER_COMP_N                        0x2306 
#define RDOUT_COMP_THRESH                           0x2307 
#define RDOUT_COMP_RATIO                            0x2308 
#define RDOUT_GATE_THRESH                           0x2309 

#define RDOUT_STRING_TABLE                          0x2400 // pull the data from a lookup table

#define JUMP_ZOOMCOMPRESSOR                         0x2500
#define JUMP_ZOOMGATES                              0x2501


// Value Tables
//-------------
#define SCRMD_I_FADER_VOL_N_ZOOM                    100
#define MDSCR_I_FADER_VOL_N_ZOOM                    110


// MIDI to Screen conversion tables
//---------------------------------
#define RT_MIDITOSCR_NOTHING                        0
#define RT_MIDITOSCR_SOLO_INPUT                     100
#define RT_MIDITOSCR_MUTE_INPUT                     101
#define RT_MIDITOSCR_VFADER_INPUT                   102
#define RT_MIDITOSCR_VFADER_INPUT_FULL              103
#define RT_MIDITOSCR_COMP_THRESH                    104
#define RT_MIDITOSCR_COMP_RATIO                     105
#define RT_MIDITOSCR_GATE_THRESH                    106

// Screen to MIDI conversion tables
//---------------------------------
#define RT_SCRTOMIDI_NOTHING                        0
#define RT_SCRTOMIDI_SOLO_INPUT                     500
#define RT_SCRTOMIDI_MUTE_INPUT                     501
#define RT_SCRTOMIDI_VFADER_INPUT                   502
#define RT_SCRTOMIDI_VFADER_INPUT_FULL              503
#define RT_SCRTOMIDI_DIRECT                         504

// MIDI FORMAT DEFINITIONS
//------------------------
#define MIDIF_ENDFORMAT                             0xffff
#define MIDIF_CUR                                   0xfffe // the current data goes to the current index position of the buffer
#define MIDIF_CUR_P_PREV                            0xfffd // the previous and the current data are added and then stored into
                                                           // the  current index position of the buffer
#define MIDIF_CUR_P_NEXT                            0xfffc // the next and the current data are added and then stored into
                                                           // the  current index position of the buffer
#define MIDIF_CUR_T_NEXT                            0xfffb // the next and the current data are multiplied and then stored into
                                                           // the  current index position of the buffer
#define MIDIF_CUR_T_PREV                            0xfffa // the previous and the current data are multiplied and then stored into
                                                           // the  current index position of the buffer
#define MIDIF_SKIP                                  0xfff9 // Skip the current data value

#define MIDIF_GO_PREV                               0xfff8 // Returns to the previous data value

#define MIDIF_CUR_P_CTRL                            0xfff7 // Adds the current data and the ControlPhisNumber and then stores it
                                                           // into current index position of the buffer

#define MIDIF_CUR_F1                                0xf000 // use the current data in formula #1
                                                           // and store the result into the current index position of the buffer
#define MIDIF_CUR_F2                                0xf001 // use the current data in formula #2
                                                           // and store the result into the current index position of the buffer
#define MIDIF_CUR_F3                                0xf002 // use the current data in formula #3
                                                           // and store the result into the current index position of the buffer
#define MIDIF_CUR_F4                                0xf003 // use the current data in formula #4
                                                           // and store the result into the current index position of the buffer





