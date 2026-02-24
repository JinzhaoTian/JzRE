file(GLOB TEST_CLI_SOURCES CONFIGURE_DEPENDS
    "Test*.cpp"
)

if(NOT TARGET JzCLI)
    message(STATUS "Skipping CLI tests: JzCLI target is not available.")
    return()
endif()

option(JzRE_CLI_BUILD_TESTS "Build CLI unit tests" ${JzRE_BUILD_TESTS})
if(NOT JzRE_CLI_BUILD_TESTS)
    message(STATUS "Skipping CLI tests: JzRE_CLI_BUILD_TESTS=OFF")
    return()
endif()

add_executable(
    TESTJzRECLI
    ${TEST_CLI_SOURCES}
)

source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES ${TEST_CLI_SOURCES}
)

set_target_properties(TESTJzRECLI PROPERTIES FOLDER "Tests")

target_compile_features(TESTJzRECLI PUBLIC cxx_std_20)

target_link_libraries(
    TESTJzRECLI
    PRIVATE
    JzCLI
    JzRERuntime
    JzRuntimeFunction
    JzRuntimeResource
    JzRuntimePlatform
    JzRuntimeCore
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(TESTJzRECLI)
