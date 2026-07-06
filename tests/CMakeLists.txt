#==============================================================================
# nfx::silicon - Tests
#==============================================================================

set(FETCHCONTENT_UPDATES_DISCONNECTED ON  CACHE BOOL "")
set(FETCHCONTENT_QUIET                OFF CACHE BOOL "")

include(FetchContent)

FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG        v2.5.2
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(doctest)

list(APPEND CMAKE_MODULE_PATH "${doctest_SOURCE_DIR}/scripts/cmake")

include(doctest)

#----------------------------------------------
# Test subdirectories
#----------------------------------------------

add_subdirectory(buffer)
add_subdirectory(chip)
add_subdirectory(decoder)
add_subdirectory(discrete)
add_subdirectory(flipflop)
add_subdirectory(gate)
add_subdirectory(latch)
add_subdirectory(memory)
add_subdirectory(mux)
add_subdirectory(primitive)
add_subdirectory(signal)
