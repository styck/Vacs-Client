cmake_minimum_required(VERSION 3.28)
project(Vacs_Client VERSION 1.0 LANGUAGES C RC)

# Source files (C sources)
set(SOURCES
    ConsoleCommunication.c
    ControlDataFilters.c
    Controls.c
    CreateMain.c
    datafile.c
    eqDisplay.c
    "Events interface.c"
    FullView.c
    Groups.c
    Init.c
    "Labels and Groups.c"
    Main.c
    MasterWindow.c
    MDI_Main.c
    MemoryMap.c
    MIDI.c
    "Misc BinRes.c"
    "Mix Files.c"
    Preferences.c
    "Remap and Mode.c"
    SAMM_Globals.c
    "Scan CtrlZones.c"
    Sequence.c
    Status.c
    trackingwindow.c
    ZoneMaps.c
    ZoomView.c
    CompnGateVU.c
)

# Resource file
set(RESOURCES
    RC/SAMM.rc
)

# Add executable
add_executable(Vacs_Client ${SOURCES} ${RESOURCES})
add_dependencies(Vacs_Client usengl)

# Include directories
target_include_directories(Vacs_Client PRIVATE ${CMAKE_SOURCE_DIR}/inc)

# Preprocessor definitions
target_compile_definitions(Vacs_Client
    PRIVATE
        WIN32
        _WINDOWS
)

set_property(TARGET Vacs_Client APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:MIDI_SUPPORT _DEBUG>)

# Link libraries (all dependencies)
target_link_libraries(Vacs_Client
    commport
    ConsoleDefinition
    DLList32
    "$<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/binD/usengl_static.lib>$<$<CONFIG:Release>:${CMAKE_SOURCE_DIR}/binR/usengl_static.lib>"
    midi
    odbc32
    odbccp32
    comctl32
    version
    winmm
)

# Output directories
set_target_properties(Vacs_Client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/binD"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/binR"
)

# Output name
set_target_properties(Vacs_Client PROPERTIES OUTPUT_NAME "Vacs")

# Linker flags for Win32
if(WIN32)
    set_target_properties(Vacs_Client PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS /MACHINE:X86")
    set_target_properties(Vacs_Client PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /MACHINE:X86")
endif()


# Post-build: copy required DLLs from the correct build output directory for each configuration
add_custom_command(TARGET Vacs_Client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<IF:$<CONFIG:Debug>,${CMAKE_BINARY_DIR}/binD/ConsoleDefinition.dll,${CMAKE_BINARY_DIR}/binR/ConsoleDefinition.dll>"
        "$<TARGET_FILE_DIR:Vacs_Client>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<IF:$<CONFIG:Debug>,${CMAKE_BINARY_DIR}/binD/commport.dll,${CMAKE_BINARY_DIR}/binR/commport.dll>"
        "$<TARGET_FILE_DIR:Vacs_Client>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<IF:$<CONFIG:Debug>,${CMAKE_BINARY_DIR}/binD/midi.dll,${CMAKE_BINARY_DIR}/binR/midi.dll>"
        "$<TARGET_FILE_DIR:Vacs_Client>"
)
