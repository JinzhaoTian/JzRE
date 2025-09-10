# Find gtest package
find_package(GTest CONFIG REQUIRED)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/src)

# Test source files
file(GLOB TEST_SOURCES "*.cpp")

# Create test executable
add_executable(JzRETests ${TEST_SOURCES})

# Set target properties
set_target_properties(JzRETests PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Link libraries
target_link_libraries(JzRETests PRIVATE
    JzRELib
    GTest::gtest
)

# Add tests to CTest
include(GoogleTest)
gtest_discover_tests(JzRETests)

# Set output directory
set_target_properties(JzRETests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
)
