#==============================================================================
# nfx::silicon::mux - Tests
#==============================================================================

file(GLOB_RECURSE NFX_SILICON_MUX_TESTS_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_executable(nfx-silicon-tests-mux ${NFX_SILICON_MUX_TESTS_SOURCES})

set_target_properties(nfx-silicon-tests-mux PROPERTIES DEBUG_POSTFIX "-d")

target_compile_definitions(nfx-silicon-tests-mux
    PRIVATE
        DOCTEST_CONFIG_USE_STD_HEADERS
)

target_link_libraries(nfx-silicon-tests-mux
    PRIVATE
        doctest::doctest_with_main
        nfx::silicon
)

doctest_discover_tests(nfx-silicon-tests-mux)
