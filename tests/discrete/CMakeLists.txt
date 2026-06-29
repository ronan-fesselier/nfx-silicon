#==============================================================================
# nfx::silicon::discrete - Tests
#==============================================================================

file(GLOB_RECURSE NFX_SILICON_DISCRETE_TESTS_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_executable(nfx-silicon-tests-discrete ${NFX_SILICON_DISCRETE_TESTS_SOURCES})

set_target_properties(nfx-silicon-tests-discrete PROPERTIES DEBUG_POSTFIX "-d")

target_compile_definitions(nfx-silicon-tests-discrete
    PRIVATE
        DOCTEST_CONFIG_USE_STD_HEADERS
)

target_link_libraries(nfx-silicon-tests-discrete
    PRIVATE
        doctest::doctest_with_main
        nfx::silicon
)

doctest_discover_tests(nfx-silicon-tests-discrete)
