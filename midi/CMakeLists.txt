cmake_minimum_required(VERSION 3.28)
project(midi VERSION 1.0 LANGUAGES C)

# Source files
set(SOURCES
    SAMMmidi.c
)

# Add shared library with export definition
add_library(midi SHARED ${SOURCES} midi.def)

# Include directories
target_include_directories(midi PUBLIC ${CMAKE_SOURCE_DIR}/inc)

# Preprocessor definitions
target_compile_definitions(midi
    PRIVATE
        WIN32
        _WINDOWS
        _USRDLL
)

# Link libraries
target_link_libraries(midi
    winmm
)

# Output directories
set_target_properties(midi PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/binD"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/binR"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/binD"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/binR"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/binD"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/binR"
)

# Linker flags for Win32
if(WIN32)
    set_target_properties(midi PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS /MACHINE:X86")
    set_target_properties(midi PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /MACHINE:X86")
endif()
