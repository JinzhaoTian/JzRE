# Runtime Platform Layer - RHI, Graphics Backends, Platform Abstractions
add_library(JzRuntimePlatform STATIC)

# Collect all RHI sources
file(GLOB_RECURSE PLATFORM_HEADERS CONFIGURE_DEPENDS "include/*.h")
file(GLOB RHI_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

# OpenGL backend
file(GLOB OPENGL_SOURCES CONFIGURE_DEPENDS "OpenGL/*.cpp")
file(GLOB OPENGL_HEADERS CONFIGURE_DEPENDS "OpenGL/*.h")

# Vulkan backend (conditional)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Vulkan" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Vulkan")
    file(GLOB VULKAN_SOURCES CONFIGURE_DEPENDS "Vulkan/*.cpp")
    file(GLOB VULKAN_HEADERS CONFIGURE_DEPENDS "Vulkan/*.h")
endif()

# Platform-specific sources
if(WIN32)
    file(GLOB PLATFORM_SPECIFIC_SOURCES CONFIGURE_DEPENDS "Windows/*.cpp")
    file(GLOB PLATFORM_SPECIFIC_HEADERS CONFIGURE_DEPENDS "Windows/*.h")
elseif(APPLE)
    file(GLOB PLATFORM_SPECIFIC_SOURCES CONFIGURE_DEPENDS "macOS/*.cpp" "macOS/*.mm")
    file(GLOB PLATFORM_SPECIFIC_HEADERS CONFIGURE_DEPENDS "macOS/*.h")
else()
    file(GLOB PLATFORM_SPECIFIC_SOURCES CONFIGURE_DEPENDS "Linux/*.cpp")
    file(GLOB PLATFORM_SPECIFIC_HEADERS CONFIGURE_DEPENDS "Linux/*.h")
endif()

# Combine all sources
set(ALL_PLATFORM_SOURCES
    ${PLATFORM_HEADERS}
    ${RHI_SOURCES}
    ${OPENGL_SOURCES}
    ${OPENGL_HEADERS}
    ${VULKAN_SOURCES}
    ${VULKAN_HEADERS}
    ${PLATFORM_SPECIFIC_SOURCES}
    ${PLATFORM_SPECIFIC_HEADERS}
)

target_sources(JzRuntimePlatform PRIVATE ${ALL_PLATFORM_SOURCES})

# Organize sources in Visual Studio filters
source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES ${ALL_PLATFORM_SOURCES}
)

# Set target folder in Visual Studio
set_target_properties(JzRuntimePlatform PROPERTIES FOLDER "Runtime")

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

