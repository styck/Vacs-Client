# Microsoft Developer Studio Project File - Name="Vacs Client" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Vacs Client - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Vacs Client.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Vacs Client.mak" CFG="Vacs Client - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Vacs Client - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Vacs Client - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Vacs Client - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "binr"
# PROP Intermediate_Dir "WasteRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /Gz /MT /W3 /GX /O2 /I ".\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib version.lib winmm.lib /nologo /subsystem:windows /machine:I386 /out:"binr/Vacs.exe"

!ELSEIF  "$(CFG)" == "Vacs Client - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "bind"
# PROP Intermediate_Dir "WasteDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /Gz /MTd /W3 /Gm /GX /ZI /Od /I ".\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib version.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /out:"bind/Vacs.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Vacs Client - Win32 Release"
# Name "Vacs Client - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
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

SOURCE=.\src\RC\SAMM.rc
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
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\inc\CDef_External.h
# End Source File
# Begin Source File

SOURCE=.\inc\CDef_implement.h
# End Source File
# Begin Source File

SOURCE=.\inc\ConsoleDefinition.h
# End Source File
# Begin Source File

SOURCE=.\inc\ConsoleDefinitionRC.h
# End Source File
# Begin Source File

SOURCE=.\inc\CTRL.H
# End Source File
# Begin Source File

SOURCE=.\inc\DCXTCP_protocol.h
# End Source File
# Begin Source File

SOURCE=.\inc\DLLIST.H
# End Source File
# Begin Source File

SOURCE=.\inc\lang.h
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
# Begin Source File

SOURCE=.\inc\Spooler.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\src\RC\cur00001.cur
# End Source File
# Begin Source File

SOURCE=.\src\RC\cur00002.cur
# End Source File
# Begin Source File

SOURCE=.\src\RC\cur00003.cur
# End Source File
# Begin Source File

SOURCE=.\src\RC\cur00004.cur
# End Source File
# Begin Source File

SOURCE=.\src\RC\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\src\RC\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\src\RC\idcur.cur
# End Source File
# Begin Source File

SOURCE=.\src\RC\idcur_up.cur
# End Source File
# Begin Source File

SOURCE=.\src\RC\SAMMmain.ico
# End Source File
# End Group
# End Target
# End Project
