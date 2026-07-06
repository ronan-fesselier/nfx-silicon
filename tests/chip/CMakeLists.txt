#==============================================================================
# nfx::silicon::chip - Tests
#==============================================================================

file(GLOB_RECURSE NFX_SILICON_CHIP_TESTS_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_executable(nfx-silicon-tests-chip ${NFX_SILICON_CHIP_TESTS_SOURCES})

set_target_properties(nfx-silicon-tests-chip PROPERTIES DEBUG_POSTFIX "-d")

target_compile_definitions(nfx-silicon-tests-chip
    PRIVATE
        DOCTEST_CONFIG_USE_STD_HEADERS
)

target_link_libraries(nfx-silicon-tests-chip
    PRIVATE
        doctest::doctest_with_main
        nfx::silicon
)

doctest_discover_tests(nfx-silicon-tests-chip)
