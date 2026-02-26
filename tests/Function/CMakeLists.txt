file(GLOB TEST_FUNCTION_SOURCES CONFIGURE_DEPENDS
    "Test*.cpp"
)

add_executable(
    TESTJzREFunction
    ${TEST_FUNCTION_SOURCES}
)

source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES ${TEST_FUNCTION_SOURCES}
)

set_target_properties(TESTJzREFunction PROPERTIES FOLDER "Tests")

target_compile_features(TESTJzREFunction PUBLIC cxx_std_20)

find_package(GTest CONFIG REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(lua  CONFIG REQUIRED)

target_link_libraries(
    TESTJzREFunction
    PRIVATE
    JzRuntimeFunction
    JzRuntimeResource
    JzRuntimePlatform
    JzRuntimeCore
    sol2::sol2
    lua
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(TESTJzREFunction)
