cmake_minimum_required(VERSION 3.25.0)

# set project name
project(JzRE VERSION 0.0.1 LANGUAGES CXX C)

# set c/c++ standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set output path
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/JzRE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/JzRE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/JzRE)

# copy resources to build directory
file(COPY ${CMAKE_SOURCE_DIR}/resources/ 
     DESTINATION ${CMAKE_BINARY_DIR}/JzRE/)

# project root path
set(JzRE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/src")

# add programs
add_subdirectory(programs)

# add src module
add_subdirectory(src)

# add examples
add_subdirectory(examples)

# add tests module
option(BUILD_TESTS "Build the tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()