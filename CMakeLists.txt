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
# Build options
#----------------------------------------------

option(NFX_SILICON_BUILD_TESTS                 "Build nfx::silicon tests"                 OFF)
option(NFX_SILICON_BUILD_SAMPLES               "Build nfx::silicon sample executables"    OFF)
option(NFX_SILICON_ENABLE_NATIVE_OPTIMIZATIONS "Enable CPU-specific native optimizations" OFF)
option(NFX_SILICON_WARNINGS_AS_ERRORS          "Treat warnings as errors"                 OFF)

#----------------------------------------------
# Build type
#----------------------------------------------

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (Debug/Release)" FORCE)
endif()

#----------------------------------------------
# CMake modules configuration
#----------------------------------------------

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(nfxSiliconSources)
include(nfxSiliconTarget)

#----------------------------------------------
# Tests
#----------------------------------------------

if(NFX_SILICON_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

#----------------------------------------------
# Samples
#----------------------------------------------

if(NFX_SILICON_BUILD_SAMPLES)
    add_subdirectory(samples)
endif()
