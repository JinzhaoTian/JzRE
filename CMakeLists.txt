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

# Suppress duplicate library warnings on macOS
# This can be caused by vcpkg packages having overlapping transitive dependencies
if(APPLE)
    add_link_options("-Wl,-no_warn_duplicate_libraries")
endif()

# Enable folder organization in Visual Studio
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Add CMake modules path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Include helper modules
include(JzSourceGroups)

# set output path (works for both single-config and multi-config generators)
set(JzRE_OUTPUT_DIR ${CMAKE_BINARY_DIR}/JzRE)

# For single-config generators (Ninja, Makefiles)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${JzRE_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${JzRE_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${JzRE_OUTPUT_DIR})

# For multi-config generators (Visual Studio, Xcode)
# These override the per-config subdirectories
foreach(CONFIG_TYPE DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${JzRE_OUTPUT_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${JzRE_OUTPUT_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${JzRE_OUTPUT_DIR})
endforeach()

# copy resources to build directory (at configure time)
file(COPY ${CMAKE_SOURCE_DIR}/resources/ 
     DESTINATION ${JzRE_OUTPUT_DIR}/)

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
