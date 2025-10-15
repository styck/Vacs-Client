cmake_minimum_required(VERSION 3.28)
project(DLList32 VERSION 1.0 LANGUAGES C)

# Source files
set(SOURCES
    DL_LIST.C
    DLLMain.C
)

# Header files
set(HEADERS
    ${CMAKE_SOURCE_DIR}/inc/DLLIST.H
    ${CMAKE_SOURCE_DIR}/inc/Spooler.h
)

# Add shared library only (DLL)
add_library(DLList32 SHARED ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(DLList32 PUBLIC ${CMAKE_SOURCE_DIR}/inc)

# Preprocessor definitions
target_compile_definitions(DLList32
    PRIVATE
        WIN32
        _WINDOWS
        _USRDLL
        DLList32_EXPORTS
)

# Link libraries
target_link_libraries(DLList32
    version
)

# Output directories
set_target_properties(DLList32 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/binD"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/binR"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/binD"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/binR"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/binD"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/binR"
    IMPORT_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/binD"
    IMPORT_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/binR"
    OUTPUT_NAME "DLList32"
)

# Linker flags for Win32
if(WIN32)
    set_target_properties(DLList32 PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS /MACHINE:X86")
    set_target_properties(DLList32 PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /MACHINE:X86")
endif()
