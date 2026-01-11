# Runtime Platform Layer - RHI, Graphics Backends, Platform Abstractions
add_library(JzRuntimePlatform STATIC)

# Collect all RHI sources
file(GLOB_RECURSE PLATFORM_HEADERS "include/*.h")
file(GLOB RHI_SOURCES "src/*.cpp")

# OpenGL backend
file(GLOB OPENGL_SOURCES "OpenGL/*.cpp")
file(GLOB OPENGL_HEADERS "OpenGL/*.h")

# Vulkan backend (conditional)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Vulkan" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Vulkan")
    file(GLOB VULKAN_SOURCES "Vulkan/*.cpp")
    file(GLOB VULKAN_HEADERS "Vulkan/*.h")
endif()

# Platform-specific sources
if(WIN32)
    file(GLOB PLATFORM_SPECIFIC_SOURCES "Windows/*.cpp")
elseif(APPLE)
    file(GLOB PLATFORM_SPECIFIC_SOURCES "macOS/*.cpp" "macOS/*.mm")
else()
    file(GLOB PLATFORM_SPECIFIC_SOURCES "Linux/*.cpp")
endif()

target_sources(JzRuntimePlatform PRIVATE
    ${PLATFORM_HEADERS}
    ${RHI_SOURCES}
    ${OPENGL_SOURCES}
    ${OPENGL_HEADERS}
    ${VULKAN_SOURCES}
    ${VULKAN_HEADERS}
    ${PLATFORM_SPECIFIC_SOURCES}
)

target_include_directories(JzRuntimePlatform
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan
)

# ============================================
# External Dependencies (self-contained)
# ============================================

# Graphics API dependencies
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

# Platform-specific libraries
if(WIN32)
    find_package(OpenGL REQUIRED)
    set(JZ_PLATFORM_LIBS opengl32 gdi32)
    set(JZ_OPENGL_LIBS OpenGL::GL)
elseif(APPLE)
    find_library(COCOA_LIB Cocoa REQUIRED)
    find_library(IOKIT_LIB IOKit REQUIRED)
    find_library(COREVIDEO_LIB CoreVideo REQUIRED)
    find_library(OPENGL_LIB OpenGL REQUIRED)
    find_library(GLUT_LIB GLUT REQUIRED)
    find_library(UNIFORMTYPEIDENTIFIERS_LIB UniformTypeIdentifiers REQUIRED)
    set(JZ_PLATFORM_LIBS ${COCOA_LIB} ${IOKIT_LIB} ${COREVIDEO_LIB} ${UNIFORMTYPEIDENTIFIERS_LIB})
    set(JZ_OPENGL_LIBS ${OPENGL_LIB} ${GLUT_LIB})
elseif(UNIX)
    find_package(OpenGL REQUIRED)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(X11 REQUIRED x11)
    set(JZ_PLATFORM_LIBS ${X11_LIBRARIES} ${CMAKE_DL_LIBS})
    set(JZ_OPENGL_LIBS OpenGL::GL)
endif()

# Threading support
find_package(Threads REQUIRED)

target_link_libraries(JzRuntimePlatform
    PUBLIC
        JzRuntimeCore
        glad::glad
        glfw
        Threads::Threads
        ${JZ_OPENGL_LIBS}
        ${JZ_PLATFORM_LIBS}
)
