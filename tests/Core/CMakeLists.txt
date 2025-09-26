# set test source files
file(GLOB TEST_JzRECore_SOURCES CONFIGURE_DEPENDS 
    "*.cpp"
)

# create test executable target
add_executable(Test_JzRECore
    ${TEST_JzRECore_SOURCES}
)

# set test executable target compile properties
target_compile_features(Test_JzRECore PUBLIC cxx_std_20)

# set test executable target link libraries
target_link_libraries(Test_JzRECore PRIVATE
    JzRECore
    GTest::gtest
)

# add tests to CTest
include(GoogleTest)
gtest_discover_tests(Test_JzRECore)
