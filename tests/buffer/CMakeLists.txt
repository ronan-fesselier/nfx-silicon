#==============================================================================
# nfx::silicon::buffer - Tests
#==============================================================================

file(GLOB_RECURSE NFX_SILICON_BUFFER_TESTS_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_executable(nfx-silicon-tests-buffer ${NFX_SILICON_BUFFER_TESTS_SOURCES})

set_target_properties(nfx-silicon-tests-buffer PROPERTIES DEBUG_POSTFIX "-d")

target_compile_definitions(nfx-silicon-tests-buffer
    PRIVATE
        DOCTEST_CONFIG_USE_STD_HEADERS
)

target_link_libraries(nfx-silicon-tests-buffer
    PRIVATE
        doctest::doctest_with_main
        nfx::silicon
)

doctest_discover_tests(nfx-silicon-tests-buffer)
