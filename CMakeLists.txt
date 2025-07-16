cmake_minimum_required(VERSION 3.15.0)

# set project name
project(JzRE)

# set vcpkg
include(cmake/configure_vcpkg.cmake)

# set c/c++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
    ${PLATFORM_LIBS}
    ${OPENGL_LIBS}
    glfw
    glad::glad
    glm::glm
    imgui::imgui
    assimp::assimp
)

# Set output directory
set_target_properties(JzRE PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
)