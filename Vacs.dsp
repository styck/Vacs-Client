# Microsoft Developer Studio Project File - Name="Vacs" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Vacs - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Vacs.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Vacs.mak" CFG="Vacs - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Vacs - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Vacs - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/VACS", YJBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Vacs - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\binR"
# PROP Intermediate_Dir ".\WasteRelease\Vacs"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /Gz /W3 /GX /O2 /I ".\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib comctl32.lib winmm.lib version.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Vacs - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\binD"
# PROP Intermediate_Dir ".\WasteDebug\Vacs"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /Gz /MTd /W3 /Gm /Zi /Od /I ".\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fr /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib comctl32.lib winmm.lib version.lib /nologo /subsystem:windows /profile /debug /machine:I386

!ENDIF 

# Begin Target

# Name "Vacs - Win32 Release"
# Name "Vacs - Win32 Debug"
# Begin Group "Include"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\inc\ConsoleDefinition.h
# End Source File
# Begin Source File

SOURCE=.\inc\CTRL.H
# End Source File
# Begin Source File

SOURCE=.\inc\DLLIST.H
# End Source File
# Begin Source File

SOURCE=.\inc\MACRO.H
# End Source File
# Begin Source File

SOURCE=.\inc\SAMM.h
# End Source File
# Begin Source File

SOURCE=.\inc\samm_rc.h
# End Source File
# Begin Source File

SOURCE=.\inc\SAMMext.h
# End Source File
# Begin Source File

SOURCE=.\inc\sammres00.h
# End Source File
# Begin Source File

SOURCE=.\inc\SAMMstruc.h
# End Source File
# End Group
# Begin Group "Source"

# PROP Default_Filter "*.c"
# Begin Source File

SOURCE=.\src\256Colors.C
# End Source File
# Begin Source File

SOURCE=.\src\ConsoleCommunication.c
# End Source File
# Begin Source File

SOURCE=.\src\ControlDataFilters.c
# End Source File
# Begin Source File

SOURCE=.\src\Controls.c
# End Source File
# Begin Source File

SOURCE=.\src\CreateMain.c
# End Source File
# Begin Source File

SOURCE=.\src\datafile.c
# End Source File
# Begin Source File

SOURCE=.\src\eqDisplay.c
# End Source File
# Begin Source File

SOURCE=".\src\Events interface.c"
# End Source File
# Begin Source File

SOURCE=.\src\FullView.c
# End Source File
# Begin Source File

SOURCE=.\src\Groups.c
# End Source File
# Begin Source File

SOURCE=.\src\Init.c
# End Source File
# Begin Source File

SOURCE=".\src\Labels and Groups.c"
# End Source File
# Begin Source File

SOURCE=.\src\Main.c
# End Source File
# Begin Source File

SOURCE=.\src\MasterWindow.c
# End Source File
# Begin Source File

SOURCE=.\src\MDI_Main.c
# End Source File
# Begin Source File

SOURCE=.\src\MemoryMap.c
# End Source File
# Begin Source File

SOURCE=".\src\Misc BinRes.c"
# End Source File
# Begin Source File

SOURCE=".\src\Mix Files.c"
# End Source File
# Begin Source File

SOURCE=.\src\Preferences.c
# End Source File
# Begin Source File

SOURCE=".\src\Remap and Mode.c"
# End Source File
# Begin Source File

SOURCE=.\src\SAMM_Globals.c
# End Source File
# Begin Source File

SOURCE=".\src\Scan CtrlZones.c"
# End Source File
# Begin Source File

SOURCE=.\src\Sequence.c
# End Source File
# Begin Source File

SOURCE=.\src\Status.c
# End Source File
# Begin Source File

SOURCE=.\src\trackingwindow.c
# End Source File
# Begin Source File

SOURCE=.\src\ZoneMaps.c
# End Source File
# Begin Source File

SOURCE=.\src\ZoomView.c
# End Source File
# End Group
# Begin Group "Resource"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\RC\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\src\RC\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=".\src\rc\CorTek260x89@72ppi-rgb-indexedcolor.BMP"
# End Source File
# Begin Source File

SOURCE=.\src\RC\cur00001.cur
# End Source File
# Begin Source File

SOURCE=.\src\RC\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\src\RC\idcur.cur
# End Source File
# Begin Source File

SOURCE=.\src\RC\idcur_up.cur
# End Source File
# Begin Source File

SOURCE=.\src\RC\playbar_bw2.bmp
# End Source File
# Begin Source File

SOURCE=.\src\RC\SAMMmain.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\rc\corteklogo.bmp
# End Source File
# Begin Source File

SOURCE=.\src\rc\cur00002.cur
# End Source File
# Begin Source File

SOURCE=.\src\rc\cur00003.cur
# End Source File
# Begin Source File

SOURCE=.\src\rc\cur00004.cur
# End Source File
# Begin Source File

SOURCE=.\src\rc\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\src\RC\SAMM.rc
# End Source File
# End Target
# End Project
