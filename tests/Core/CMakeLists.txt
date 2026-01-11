# set test source files
file(GLOB TESTJzRECore_SOURCES CONFIGURE_DEPENDS 
    "Test*.cpp"
)

# create test executable target
add_executable(
    TESTJzRECore
    ${TESTJzRECore_SOURCES}
)

# set test executable target compile properties
target_compile_features(TESTJzRECore PUBLIC cxx_std_20)

# External dependencies
find_package(GTest CONFIG REQUIRED)

# set test executable target link libraries
target_link_libraries(
    TESTJzRECore
    PRIVATE
    JzRuntimeCore
    GTest::gtest_main
)

# add tests to CTest
include(GoogleTest)
gtest_discover_tests(TESTJzRECore)
