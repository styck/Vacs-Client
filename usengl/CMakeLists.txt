cmake_minimum_required(VERSION 3.28)
project(usengl VERSION 1.0 LANGUAGES C RC)

# Source files
set(SOURCES
    DLL_Main.c
    usengl.rc
)

# Header files
set(HEADERS
    ${CMAKE_SOURCE_DIR}/inc/lang.h
)

# Add shared and static libraries
add_library(usengl SHARED ${SOURCES} ${HEADERS})
add_library(usengl_static STATIC ${SOURCES} ${HEADERS})
set_target_properties(usengl PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/binD"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/binR"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/binD"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/binR"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/binD"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/binR"
)
set_target_properties(usengl_static PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/binD"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/binR"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/binD"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/binR"
)

# Include directories
target_include_directories(usengl PUBLIC ${CMAKE_SOURCE_DIR}/inc)

# Preprocessor definitions
target_compile_definitions(usengl
    PRIVATE
        WIN32
        _WINDOWS
        _USRDLL
)

# Link libraries
target_link_libraries(usengl
    version
)

# Output directories


# Linker flags for Win32
if(WIN32)
    set_target_properties(usengl PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS /MACHINE:X86")
    set_target_properties(usengl PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /MACHINE:X86")
endif()
