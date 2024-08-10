cmake_minimum_required(VERSION 3.15.0)

# set project name
project(JzRE)

# set c/c++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# set c/c++ compiler
if(MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -static")
endif()

set(THIRD_PARTY ${PROJECT_SOURCE_DIR}/dependencies)

# set third party include path
include_directories(
    ${THIRD_PARTY}/include
    ${THIRD_PARTY}/include/glad/
    ${THIRD_PARTY}/include/glfw/
    ${THIRD_PARTY}/include/imgui/
)

# set third party libs
find_library(GLFW_LIB NAMES glfw3 PATHS "${THIRD_PARTY}/lib/glfw")

add_library(glad STATIC ${THIRD_PARTY}/lib/glad/glad.c)

find_library(ASSIMP_LIB NAMES assimp PATHS "${THIRD_PARTY}/lib/assimp")

file(GLOB IMGUI_SRC
    ${THIRD_PARTY}/include/imgui/*.cpp
    ${THIRD_PARTY}/include/imgui/backend/imgui_impl_glfw.cpp
    ${THIRD_PARTY}/include/imgui/backend/imgui_impl_opengl3.cpp
)
add_library(imgui STATIC ${IMGUI_SRC})

# set include path
include_directories(${PROJECT_SOURCE_DIR}/JzRE/include)

# set source files
file(GLOB JZRE_SOURCES "${PROJECT_SOURCE_DIR}/JzRE/source/*.cpp")

# add exe target
add_executable(JzRE main.cpp ${JZRE_SOURCES})

# link
target_link_libraries(JzRE ${GLFW_LIB} glad)
target_link_libraries(JzRE ${ASSIMP_LIB} -lz)
target_link_libraries(JzRE imgui)
target_link_libraries(JzRE opengl32 gdi32)