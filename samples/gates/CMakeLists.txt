#==============================================================================
# nfx::silicon::gates - Samples
#==============================================================================

file(GLOB_RECURSE NFX_SILICON_GATES_SAMPLE_SOURCES CONFIGURE_DEPENDS "*.cpp")

foreach(source IN LISTS NFX_SILICON_GATES_SAMPLE_SOURCES)
    cmake_path(RELATIVE_PATH source BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" OUTPUT_VARIABLE source_rel)
    string(REPLACE "/" "-" source_id "${source_rel}")
    string(REPLACE ".cpp" "" source_id "${source_id}")
    set(target "nfx-silicon-sample-gates-${source_id}")

    add_executable(${target} ${source})
    set_target_properties(${target} PROPERTIES DEBUG_POSTFIX "-d")

    target_link_libraries(${target}
        PRIVATE
            nfx::silicon
            nfx::silicon::warnings
    )
endforeach()
