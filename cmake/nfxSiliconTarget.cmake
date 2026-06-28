#==============================================================================
# nfx::silicon - Target configuration
#==============================================================================

#----------------------------------------------
# Warning policy
#----------------------------------------------

add_library(nfx-silicon-warnings INTERFACE)
add_library(nfx::silicon::warnings ALIAS nfx-silicon-warnings)

target_compile_options(nfx-silicon-warnings
    INTERFACE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

if(NFX_SILICON_WARNINGS_AS_ERRORS)
    target_compile_options(nfx-silicon-warnings
        INTERFACE
            $<$<CXX_COMPILER_ID:GNU,Clang>:-Werror>
            $<$<CXX_COMPILER_ID:MSVC>:/WX>
    )
endif()

#----------------------------------------------
# Library target
#----------------------------------------------

add_library(nfx-silicon SHARED
    ${NFX_SILICON_SOURCES}
)

if(WIN32)
    set_target_properties(nfx-silicon PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()
set_target_properties(nfx-silicon PROPERTIES POSITION_INDEPENDENT_CODE ON)
add_library(nfx::silicon ALIAS nfx-silicon)

#----------------------------------------------
# Include directories
#----------------------------------------------

target_include_directories(nfx-silicon
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

#----------------------------------------------
# Link libraries
#----------------------------------------------

target_link_libraries(nfx-silicon
    PRIVATE
        nfx::silicon::warnings
)

#----------------------------------------------
# Native CPU optimizations (Release only)
#----------------------------------------------

if(NFX_SILICON_ENABLE_NATIVE_OPTIMIZATIONS)
    target_compile_options(nfx-silicon
        PRIVATE
            $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>:/arch:AVX2>
            $<$<AND:$<CONFIG:Release>,$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>>:-march=native>
    )
endif()
