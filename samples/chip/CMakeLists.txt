#==============================================================================
# nfx::silicon::chip - Samples
#==============================================================================

set(NFX_SILICON_CHIP_SAMPLE_GL_SOURCES
    cdp1861C-display.cpp
)

file(GLOB_RECURSE NFX_SILICON_CHIP_SAMPLE_SOURCES CONFIGURE_DEPENDS "*.cpp")

foreach(source IN LISTS NFX_SILICON_CHIP_SAMPLE_SOURCES)
    cmake_path(RELATIVE_PATH source BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" OUTPUT_VARIABLE source_rel)
    string(REPLACE "/" "-" source_id "${source_rel}")
    string(REPLACE ".cpp" "" source_id "${source_id}")
    set(target "nfx-silicon-sample-chip-${source_id}")

    add_executable(${target} ${source})
    set_target_properties(${target} PROPERTIES DEBUG_POSTFIX "-d")

    if(source_rel IN_LIST NFX_SILICON_CHIP_SAMPLE_GL_SOURCES)
        target_link_libraries(${target}
            PRIVATE
                nfx::silicon
                nfx::silicon::sample::common
                nfx::silicon::warnings
    )
    else()
        target_link_libraries(${target}
            PRIVATE
                nfx::silicon
                nfx::silicon::warnings
    )
    endif()
endforeach()
