# Runtime Platform Layer - RHI, Graphics Backends, Platform Abstractions
add_library(JzRuntimePlatform OBJECT)

# Collect all RHI sources from src/ subdirectories
file(GLOB_RECURSE PLATFORM_HEADERS CONFIGURE_DEPENDS "include/*.h")
file(GLOB_RECURSE PLATFORM_COMMON_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
list(FILTER PLATFORM_COMMON_SOURCES EXCLUDE REGEX ".*/src/OpenGL/.*")
list(FILTER PLATFORM_COMMON_SOURCES EXCLUDE REGEX ".*/src/Vulkan/.*")
list(FILTER PLATFORM_COMMON_SOURCES EXCLUDE REGEX ".*/src/Windows/.*")
list(FILTER PLATFORM_COMMON_SOURCES EXCLUDE REGEX ".*/src/Linux/.*")
list(FILTER PLATFORM_COMMON_SOURCES EXCLUDE REGEX ".*/src/macOS/.*")

# OpenGL backend - from src/OpenGL/
file(GLOB OPENGL_SOURCES CONFIGURE_DEPENDS "src/OpenGL/*.cpp")
file(GLOB OPENGL_HEADERS CONFIGURE_DEPENDS "include/JzRE/Runtime/Platform/OpenGL/*.h")

# Vulkan backend
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/JzRE/Runtime/Platform/Vulkan"
   AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/JzRE/Runtime/Platform/Vulkan")
    file(GLOB VULKAN_HEADERS CONFIGURE_DEPENDS "include/JzRE/Runtime/Platform/Vulkan/*.h")
    file(GLOB VULKAN_SOURCES CONFIGURE_DEPENDS "src/Vulkan/*.cpp")
endif()

# Platform-specific sources - from src/{Platform}/
if(WIN32)
    file(GLOB PLATFORM_SPECIFIC_SOURCES CONFIGURE_DEPENDS "src/Windows/*.cpp")
elseif(APPLE)
    file(GLOB PLATFORM_SPECIFIC_SOURCES CONFIGURE_DEPENDS "src/macOS/*.cpp" "src/macOS/*.mm")
else()
    file(GLOB PLATFORM_SPECIFIC_SOURCES CONFIGURE_DEPENDS "src/Linux/*.cpp")
endif()

# Combine all sources
set(ALL_PLATFORM_SOURCES
    ${PLATFORM_HEADERS}
    ${PLATFORM_COMMON_SOURCES}
    ${OPENGL_SOURCES}
    ${OPENGL_HEADERS}
    ${VULKAN_SOURCES}
    ${VULKAN_HEADERS}
    ${PLATFORM_SPECIFIC_SOURCES}
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
)

# ============================================
# External Dependencies (self-contained)
# ============================================

# Graphics API dependencies
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(unofficial-spirv-reflect CONFIG REQUIRED)

# Platform-specific libraries
if(WIN32)
    find_package(OpenGL REQUIRED)
    set(JzRE_PLATFORM_LIBS opengl32 gdi32)
    set(JzRE_OPENGL_LIBS OpenGL::GL)
    set(JzRE_VULKAN_LIBS Vulkan::Vulkan)
elseif(APPLE)
    find_library(COCOA_LIB Cocoa REQUIRED)
    find_library(IOKIT_LIB IOKit REQUIRED)
    find_library(COREVIDEO_LIB CoreVideo REQUIRED)
    find_library(OPENGL_LIB OpenGL REQUIRED)
    find_library(GLUT_LIB GLUT REQUIRED)
    find_library(UNIFORMTYPEIDENTIFIERS_LIB UniformTypeIdentifiers REQUIRED)
    set(JzRE_PLATFORM_LIBS ${COCOA_LIB} ${IOKIT_LIB} ${COREVIDEO_LIB} ${UNIFORMTYPEIDENTIFIERS_LIB})
    set(JzRE_OPENGL_LIBS ${OPENGL_LIB} ${GLUT_LIB})
    set(JzRE_VULKAN_LIBS Vulkan::Vulkan)
elseif(UNIX)
    find_package(OpenGL REQUIRED)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(X11 REQUIRED x11)
    set(JzRE_PLATFORM_LIBS ${X11_LIBRARIES} ${CMAKE_DL_LIBS})
    set(JzRE_OPENGL_LIBS OpenGL::GL)
    set(JzRE_VULKAN_LIBS Vulkan::Vulkan)
endif()

# Threading support
find_package(Threads REQUIRED)

target_link_libraries(JzRuntimePlatform
    PUBLIC
        JzRuntimeCore
        glad::glad
        glfw
        Threads::Threads
        ${JzRE_OPENGL_LIBS}
        ${JzRE_VULKAN_LIBS}
        ${JzRE_PLATFORM_LIBS}
        unofficial::spirv-reflect
)
