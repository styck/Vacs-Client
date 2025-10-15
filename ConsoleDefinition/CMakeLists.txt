cmake_minimum_required(VERSION 3.28)
project(ConsoleDefinition VERSION 1.0 LANGUAGES C CXX RC)

# Source files
set(SOURCES
    CDef_DevCommunication.c
    CDef_Preferences.c
    dcx_parser.c
    DLL_Main.c
    implement.c
    Resource/ConsoleDefinition.rc
)

# Header files (not needed for compilation, but for IDE)
set(HEADERS
    ${CMAKE_SOURCE_DIR}/inc/CDef_External.h
    ${CMAKE_SOURCE_DIR}/inc/CDef_implement.h
    ${CMAKE_SOURCE_DIR}/inc/ConsoleDefinition.h
    ${CMAKE_SOURCE_DIR}/inc/ConsoleDefinitionRC.h
    ${CMAKE_SOURCE_DIR}/inc/CTRL.H
    ${CMAKE_SOURCE_DIR}/inc/DCXTCP_protocol.h
)

# Add shared library
add_library(ConsoleDefinition SHARED ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(ConsoleDefinition PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(ConsoleDefinition PUBLIC ${CMAKE_SOURCE_DIR}/inc)
target_include_directories(ConsoleDefinition PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Resource)
target_include_directories(ConsoleDefinition PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Preprocessor definitions
target_compile_definitions(ConsoleDefinition
    PRIVATE
        WIN32
        _WINDOWS
        _USRDLL
        CONSOLEDEFINITION_EXPORTS
)

# Link libraries
target_link_libraries(ConsoleDefinition
    comctl32
    wsock32
    version
)

# Output directories
set_target_properties(ConsoleDefinition PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/binD"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/binR"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/binD"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/binR"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/binD"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/binR"
)

# Linker flags for Win32
if(WIN32)
    set_target_properties(ConsoleDefinition PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS /MACHINE:X86")
    set_target_properties(ConsoleDefinition PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /MACHINE:X86")
endif()
