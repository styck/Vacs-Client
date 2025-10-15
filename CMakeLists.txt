cmake_minimum_required(VERSION 3.28)
project(VacsClient LANGUAGES C CXX RC)

#--------------------------------------------------------------------
# Global Project Settings
#--------------------------------------------------------------------
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(MSVC)
    set(CMAKE_C_FLAGS_DEBUG "/MTd /Od")
    set(CMAKE_C_FLAGS_RELEASE "/MT /O2 /DNDEBUG")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

#--------------------------------------------------------------------
# commport (Shared Library)
#--------------------------------------------------------------------
add_library(commport SHARED
    commport/commfifo.c
    commport/CommPort.c
    commport/DLL_Main.c
    commport/commfifo.h
    commport/CommPort.h
)
target_compile_definitions(commport PRIVATE COMMPORT_EXPORTS)
target_include_directories(commport PUBLIC commport)

#--------------------------------------------------------------------
# ConsoleDefinition (Shared Library)
#--------------------------------------------------------------------
add_library(ConsoleDefinition SHARED
    ConsoleDefinition/CDef_DevCommunication.c
    ConsoleDefinition/CDef_Preferences.c
    ConsoleDefinition/dcx_parser.c
    ConsoleDefinition/DLL_Main.c
    ConsoleDefinition/implement.c
    ConsoleDefinition/Resource/ConsoleDefinition.rc
)
target_compile_definitions(ConsoleDefinition PRIVATE CONSOLEDEFINITION_EXPORTS)
target_include_directories(ConsoleDefinition PUBLIC inc ConsoleDefinition)
target_link_libraries(ConsoleDefinition PRIVATE comctl32 wsock32 version)

#--------------------------------------------------------------------
# DLList32 (Shared Library)
#--------------------------------------------------------------------
add_library(DLList32 SHARED
    DLList32/DL_LIST.C
    DLList32/DLLMain.C
)
target_compile_definitions(DLList32 PRIVATE DLList32_EXPORTS)
target_include_directories(DLList32 PUBLIC inc)
target_link_libraries(DLList32 PRIVATE version)

#--------------------------------------------------------------------
# usengl (Static and Shared Libraries)
#--------------------------------------------------------------------
add_library(usengl_static STATIC
    usengl/DLL_Main.c
    usengl/usengl.rc
)
target_include_directories(usengl_static PUBLIC inc)

add_library(usengl SHARED
    usengl/DLL_Main.c
    usengl/usengl.rc
)
target_include_directories(usengl PUBLIC inc)
target_link_libraries(usengl PRIVATE version)

#--------------------------------------------------------------------
# midi (Shared Library)
#--------------------------------------------------------------------
add_library(midi SHARED
    midi/SAMMmidi.c
    midi/midi.def
)
target_include_directories(midi PUBLIC inc)
target_link_libraries(midi PRIVATE winmm)

#--------------------------------------------------------------------
# Vacs_Client (Executable)
#--------------------------------------------------------------------
add_executable(Vacs_Client
    src/ConsoleCommunication.c
    src/ControlDataFilters.c
    src/Controls.c
    src/CreateMain.c
    src/datafile.c
    src/eqDisplay.c
    "src/Events interface.c"
    src/FullView.c
    src/Groups.c
    src/Init.c
    "src/Labels and Groups.c"
    src/Main.c
    src/MasterWindow.c
    src/MDI_Main.c
    src/MemoryMap.c
    src/MIDI.c
    "src/Misc BinRes.c"
    "src/Mix Files.c"
    src/Preferences.c
    "src/Remap and Mode.c"
    src/SAMM_Globals.c
    "src/Scan CtrlZones.c"
    src/Sequence.c
    src/Status.c
    src/trackingwindow.c
    src/ZoneMaps.c
    src/ZoomView.c
    src/CompnGateVU.c
    src/RC/SAMM.rc
)
set_target_properties(Vacs_Client PROPERTIES OUTPUT_NAME "Vacs")
target_include_directories(Vacs_Client PRIVATE inc)
target_compile_definitions(Vacs_Client PRIVATE $<$<CONFIG:Debug>:MIDI_SUPPORT _DEBUG>)
target_link_libraries(Vacs_Client PRIVATE
    commport
    ConsoleDefinition
    DLList32
    usengl_static
    midi
    odbc32
    odbccp32
    comctl32
    version
    winmm
)

if(WIN32)
    set_target_properties(Vacs_Client PROPERTIES LINK_OPTIONS "/SUBSYSTEM:WINDOWS")
endif()

#--------------------------------------------------------------------
# Post-Build Step: Copy DLLs to Executable Directory
#--------------------------------------------------------------------
add_custom_command(TARGET Vacs_Client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:commport>
        $<TARGET_FILE_DIR:Vacs_Client>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:ConsoleDefinition>
        $<TARGET_FILE_DIR:Vacs_Client>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:DLList32>
        $<TARGET_FILE_DIR:Vacs_Client>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:usengl>
        $<TARGET_FILE_DIR:Vacs_Client>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:midi>
        $<TARGET_FILE_DIR:Vacs_Client>
    COMMENT "Copying dependent DLLs to executable directory."
)
