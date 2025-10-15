cmake_minimum_required(VERSION 3.28)
project(Vacs-Client VERSION 1.0 LANGUAGES C)

# Set platform to Win32
if(WIN32)
    set(CMAKE_GENERATOR_PLATFORM "Win32")
endif()

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

# Compiler flags for all targets
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd /Od")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT /O2 /DNDEBUG")

# Add subdirectories for libraries and executable
add_subdirectory(commport)
add_subdirectory(ConsoleDefinition)
add_subdirectory(DLList32)
add_subdirectory(usengl)
add_subdirectory(midi)
add_subdirectory(src)
