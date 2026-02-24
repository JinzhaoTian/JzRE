file(GLOB TEST_PLATFORM_SOURCES CONFIGURE_DEPENDS
    "Test*.cpp"
)

add_executable(
    TESTJzREPlatform
    ${TEST_PLATFORM_SOURCES}
)

source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES ${TEST_PLATFORM_SOURCES}
)

set_target_properties(TESTJzREPlatform PROPERTIES FOLDER "Tests")

target_compile_features(TESTJzREPlatform PUBLIC cxx_std_20)

find_package(GTest CONFIG REQUIRED)

target_link_libraries(
    TESTJzREPlatform
    PRIVATE
    JzRuntimePlatform
    JzRuntimeCore
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(TESTJzREPlatform)
