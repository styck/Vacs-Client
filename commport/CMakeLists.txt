cmake_minimum_required(VERSION 3.28)
project(commport VERSION 1.0 LANGUAGES C)

# Source files
set(SOURCES
    commfifo.c
    CommPort.c
    DLL_Main.c
)

# Header files
set(HEADERS
    commfifo.h
    CommPort.h
)

# Add shared library
add_library(commport SHARED ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(commport PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Preprocessor definitions
target_compile_definitions(commport
    PRIVATE
        COMMPORT_EXPORTS
        WIN32
        _WINDOWS
        _USRDLL
)

# Output directories
set_target_properties(commport PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/binD"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/binR"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/binD"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/binR"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/binD"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/binR"
)

# Linker flags for Win32
if(WIN32)
    set_target_properties(commport PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS /MACHINE:X86")
    set_target_properties(commport PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /MACHINE:X86")
endif()
