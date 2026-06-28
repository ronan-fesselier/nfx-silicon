#==============================================================================
# nfx::silicon - Top-level CMake configuration
#==============================================================================

#----------------------------------------------
# Project definition
#----------------------------------------------

cmake_minimum_required(VERSION 3.25)

project(nfx-silicon
    VERSION 0.1.0
    DESCRIPTION "C++ library dedicated to hardware emulation"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_DEBUG_POSTFIX "-d")

#----------------------------------------------
# Directory paths
#----------------------------------------------

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

#----------------------------------------------
# Build type
#----------------------------------------------

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (Debug/Release)" FORCE)
endif()

#----------------------------------------------
# Warning policy
#----------------------------------------------

add_library(nfx-silicon-warnings INTERFACE)

target_compile_options(nfx-silicon-warnings
    INTERFACE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

#----------------------------------------------
# Application sources
#----------------------------------------------

set(APP_SOURCES
    src/main.cpp
)

#----------------------------------------------
# Executable target
#----------------------------------------------

add_executable(${PROJECT_NAME}
    ${APP_SOURCES}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        nfx-silicon-warnings
)
