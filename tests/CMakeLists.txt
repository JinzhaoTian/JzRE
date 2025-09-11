# find gtest package
find_package(GTest CONFIG REQUIRED)

# enable testing
enable_testing()

# set test source files
file(GLOB TEST_SOURCES "*.cpp")

# add test executable
add_executable(JzRETests ${TEST_SOURCES})

# set target properties
set_target_properties(JzRETests PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# set link libraries
target_link_libraries(JzRETests PRIVATE
    JzRELib
    GTest::gtest
)

# add tests to CTest
include(GoogleTest)
gtest_discover_tests(JzRETests)
