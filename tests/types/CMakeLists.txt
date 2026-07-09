#==============================================================================
# nfx::silicon::address - Tests
#==============================================================================

file(GLOB_RECURSE NFX_SILICON_ADDRESS_TESTS_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_executable(nfx-silicon-tests-address ${NFX_SILICON_ADDRESS_TESTS_SOURCES})

set_target_properties(nfx-silicon-tests-address PROPERTIES DEBUG_POSTFIX "-d")

target_compile_definitions(nfx-silicon-tests-address
    PRIVATE
        DOCTEST_CONFIG_USE_STD_HEADERS
)

target_link_libraries(nfx-silicon-tests-address
    PRIVATE
        doctest::doctest_with_main
        nfx::silicon
)

doctest_discover_tests(nfx-silicon-tests-address)
