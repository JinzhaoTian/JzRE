cmake_minimum_required(VERSION 3.15.0)

# set project name
project(JzRE)

# set c/c++ standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# add cmake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# include JzRE reflection module
include(JzREReflection)

# add module
add_subdirectory(tools)
add_subdirectory(src)
