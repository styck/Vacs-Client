# Microsoft Developer Studio Project File - Name="ConsoleDefinition" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ConsoleDefinition - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ConsoleDefinition.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ConsoleDefinition.mak" CFG="ConsoleDefinition - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ConsoleDefinition - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ConsoleDefinition - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SAMMPlus/ConsoleDefinition", VFAAAAAA"
# PROP Scc_LocalPath "."
# PROP WCE_FormatVersion ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ConsoleDefinition - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\binR"
# PROP Intermediate_Dir "..\wasteRelease\ConsoleDefinition"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Gz /MT /W3 /O2 /I "..\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib comctl32.lib wsock32.lib version.lib /nologo /subsystem:windows /dll /machine:I386

!ELSEIF  "$(CFG)" == "ConsoleDefinition - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\binD"
# PROP Intermediate_Dir "..\wasteDebug\ConsoleDefinition"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /Gz /MTd /W3 /Gm /GX /ZI /Od /I "..\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# SUBTRACT CPP /X
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\inc" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib comctl32.lib wsock32.lib version.lib /nologo /subsystem:windows /dll /debug /machine:I386

!ENDIF 

# Begin Target

# Name "ConsoleDefinition - Win32 Release"
# Name "ConsoleDefinition - Win32 Debug"
# Begin Group "Resources"

# PROP Default_Filter "bmp,ico,cur"
# Begin Source File

SOURCE=.\Resource\dcp8\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcp8\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_aux_aux_mute.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_aux_mono_center.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_comp_in.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_eq_right.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_full_cue.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_full_mute.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_gate_in.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_input_aux_post_in.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_input_direct_out.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_input_extkey.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_input_keycue.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_input_locut.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_line_b_aux_16_in.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_lofreq_shelf_in.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_master_aux_red.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_master_blue_medium.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_master_green_medium.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_master_orange_medium.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_master_ppwr.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_master_yellow_medium.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_master_yellow_tiny.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_matrix_mute_mono_in.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_Safe.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_subaux_eq_hi_shelf.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_subaux_eq_in.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_subaux_eq_lo_shelf.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_submatrix_mute.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_tiny_blue.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_tiny_green.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_vu_select_in.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\btn_yellow_small.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\BtnBlue_Med1.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\BtnBlue_Med2.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\BtnCian_Med1.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\BtnGreen_Med1.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\BtnGreen_Med2.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\BtnPurple_Med1.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\BtnYellow_Med2.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\CLIENT_B.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\compgate_in.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\corTekLogo.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\cue_01.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\Cue_A_PostPan.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\Cue_Middle.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\Cuea_pre.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcp8\F_dcp8.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\FaderKnobMed.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\FaderKnobSmall.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\fdr_blue_medium.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\fdr_blue_small.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\fdr_red_medium.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\fdr_white_big.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\fdr_white_medium.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\fdr_white_small.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\fdr_yellow_medium.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcp8\fknb_fdr.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\fsw_1.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\fsw_2.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\fswu_1.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\fswu_2.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\full_input_module.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\full_subaux_module.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\gate_eq_in.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\hf_shelf.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\ICON1.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\IDB_COMPGATE_IN.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\IDB_EQ_IN.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\IDB_HF_PEAKSHELF.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\IDB_INPUT_MUTE.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\IDB_INSERT_INOUT.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\IDB_LF_PEAKSHELF.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\IDB_SUBINSERT.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\IDB_SUBMUTE.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\dcp8\idicon_g.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\dcp8\idicon_s.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\input_a_module.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\input_insert.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\input_pan_knob.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\input_volume_knob.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\InputMute.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\IQS_PAL.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\key_in.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\lf_shelf.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\low_cut.bmp
# End Source File
# Begin Source File

SOURCE=".\Resource\dcx\Master-Post-on.BMP"
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\MasterCue_Module.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\pan_indicator.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\PanKnob.bmp
# End Source File
# Begin Source File

SOURCE=".\Resource\dcx\Phase Rev.BMP"
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\PPower.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\ppwr_in.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\rd.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\SAMMGRAP.CUR
# End Source File
# Begin Source File

SOURCE=.\Resource\SAMMHAND.CUR
# End Source File
# Begin Source File

SOURCE=.\Resource\SAMMVSCR.CUR
# End Source File
# Begin Source File

SOURCE=.\Resource\dcp8\seq_clos.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcp8\seq_open.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\small_mute.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\small_post.BMP
# End Source File
# Begin Source File

SOURCE=".\Resource\dcx\Stereo Cue Meters.BMP"
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\submatrix_module.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\tinny_mute.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\VolumeKnob1.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\vu_displ.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\VUMeterActive.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcx\VUMeterActive_Horizontal.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\dcp8\Z_dcp8.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\zb_mute.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\zb_solo.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\zbu_mute.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\zbu_solo.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\dcp8\zknb_cmp.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\dcp8\zknb_fdr.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\dcp8\zknb_gat.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\ZOOMVIEW.ICO
# End Source File
# End Group
# Begin Group "include"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=..\inc\CDef_External.h
# End Source File
# Begin Source File

SOURCE=..\inc\CDef_implement.h
# End Source File
# Begin Source File

SOURCE=..\inc\ConsoleDefinition.h
# End Source File
# Begin Source File

SOURCE=..\inc\ConsoleDefinitionRC.h
# End Source File
# Begin Source File

SOURCE=..\inc\CTRL.H
# End Source File
# Begin Source File

SOURCE=..\..\DCXServer\DCXparser.h
# End Source File
# Begin Source File

SOURCE=..\inc\DCXTCP_protocol.h
# End Source File
# End Group
# Begin Group "Binary Tables"

# PROP Default_Filter "*.bin"
# Begin Source File

SOURCE=..\inc\DCX_Full_Input.bin
# End Source File
# Begin Source File

SOURCE=..\inc\DCX_Zoom_Input.bin
# End Source File
# Begin Source File

SOURCE=..\inc\DCX_Zoom_Master.bin
# End Source File
# Begin Source File

SOURCE=..\inc\DCX_Zoom_SubMatrix.bin
# End Source File
# Begin Source File

SOURCE=..\inc\Samm_rc.bin
# End Source File
# Begin Source File

SOURCE=..\inc\SAMM_RD.BIN
# End Source File
# End Group
# Begin Group "Source"

# PROP Default_Filter "*.c"
# Begin Source File

SOURCE=.\CDef_DevCommunication.c
# End Source File
# Begin Source File

SOURCE=.\CDef_Preferences.c
# End Source File
# Begin Source File

SOURCE=.\Resource\ConsoleDefinition.rc

!IF  "$(CFG)" == "ConsoleDefinition - Win32 Release"

!ELSEIF  "$(CFG)" == "ConsoleDefinition - Win32 Debug"

# ADD BASE RSC /l 0x409 /i "Resource"
# SUBTRACT BASE RSC /i "..\inc"
# ADD RSC /l 0x409 /i "Resource"
# SUBTRACT RSC /i "..\inc"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dcx_parser.c
# End Source File
# Begin Source File

SOURCE=.\DLL_Main.c
# End Source File
# Begin Source File

SOURCE=.\implement.c
# End Source File
# End Group
# End Target
# End Project
