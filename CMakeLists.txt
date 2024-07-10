cmake_minimum_required(VERSION 3.15.0)

# 设置项目名称
project(JzRE)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)


# 指定头文件目录
include_directories(${PROJECT_SOURCE_DIR}/JzRE/include)

# 查找源文件
file(GLOB JZRE_SOURCES "${PROJECT_SOURCE_DIR}/JzRE/source/*.cpp")

# 添加可执行文件
add_executable(JzRE main.cpp ${JZRE_SOURCES})