# find gtest package
find_package(GTest CONFIG REQUIRED)

# set test source files
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "*.cpp")

# create test executable target
add_executable(JzRETest ${TEST_SOURCES})

# set test executable target compile properties
target_compile_features(JzRETest PUBLIC cxx_std_20)

# set test executable target link libraries
target_link_libraries(JzRETest PRIVATE
    JzRELib
    GTest::gtest
)

# add tests to CTest
include(GoogleTest)
gtest_discover_tests(JzRETest)
