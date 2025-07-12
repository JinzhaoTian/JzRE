cmake_minimum_required(VERSION 3.15.0)

set(VCPKG_ROOT ${CMAKE_SOURCE_DIR}/vcpkg)

# set vcpkg toolchain file path
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file" FORCE)

# 设置 vcpkg 相关变量
if(DEFINED ENV{VCPKG_TARGET_TRIPLET})
    set(VCPKG_TARGET_TRIPLET $ENV{VCPKG_TARGET_TRIPLET} CACHE STRING "Vcpkg target triplet")
else()
    # 根据平台设置默认 triplet
    if(WIN32)
        set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "Vcpkg target triplet")
    elseif(APPLE)
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
            set(VCPKG_TARGET_TRIPLET "arm64-osx" CACHE STRING "Vcpkg target triplet")
        else()
            set(VCPKG_TARGET_TRIPLET "x64-osx" CACHE STRING "Vcpkg target triplet")
        endif()
    else()
        set(VCPKG_TARGET_TRIPLET "x64-linux" CACHE STRING "Vcpkg target triplet")
    endif()
endif()

# 输出 vcpkg 配置信息
message(STATUS "Using vcpkg root: ${VCPKG_ROOT}")
message(STATUS "Using vcpkg triplet: ${VCPKG_TARGET_TRIPLET}")

# Enable position independent code for shared libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Platform-specific settings
if(WIN32)
    # Windows specific settings
    if(MINGW)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -static")
    endif()
    # Add Windows-specific definitions
    add_definitions(-DWIN32_LEAN_AND_MEAN -DNOMINMAX)
elseif(APPLE)
    # macOS specific settings
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
elseif(UNIX AND NOT APPLE)
    # Linux specific settings
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

# set project name
project(JzRE)

# set c/c++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find packages using vcpkg
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Stb REQUIRED)

# Platform-specific libraries and OpenGL
if(WIN32)
    find_package(OpenGL REQUIRED)
    set(PLATFORM_LIBS opengl32 gdi32)
    set(OPENGL_LIBS OpenGL::OpenGL)
elseif(APPLE)
    # On macOS, OpenGL is part of the system frameworks
    find_library(COCOA_LIB Cocoa)
    find_library(IOKIT_LIB IOKit)
    find_library(COREVIDEO_LIB CoreVideo)
    find_library(OPENGL_LIB OpenGL)
    find_library(GLUT_LIB GLUT)
    set(PLATFORM_LIBS ${COCOA_LIB} ${IOKIT_LIB} ${COREVIDEO_LIB})
    set(OPENGL_LIBS ${OPENGL_LIB} ${GLUT_LIB})
elseif(UNIX AND NOT APPLE)
    find_package(OpenGL REQUIRED)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(X11 REQUIRED x11)
    set(PLATFORM_LIBS ${X11_LIBRARIES} ${CMAKE_DL_LIBS})
    set(OPENGL_LIBS OpenGL::OpenGL)
endif()

# set include path
include_directories(${PROJECT_SOURCE_DIR}/JzRE/include)

# set source files
file(GLOB JZRE_SOURCES "${PROJECT_SOURCE_DIR}/JzRE/source/*.cpp")

# add exe target
add_executable(JzRE main.cpp ${JZRE_SOURCES})

# Set target properties
set_target_properties(JzRE PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Link libraries using vcpkg targets
target_link_libraries(JzRE PRIVATE
    glfw
    glad::glad
    glm::glm
    imgui::imgui
    assimp::assimp
    ${PLATFORM_LIBS}
    ${OPENGL_LIBS}
)

# Compiler-specific warnings
if(MSVC)
    target_compile_options(JzRE PRIVATE /W4)
else()
    target_compile_options(JzRE PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Set output directory
set_target_properties(JzRE PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
)