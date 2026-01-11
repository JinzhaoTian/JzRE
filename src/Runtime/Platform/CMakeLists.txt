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

# External dependencies
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

target_link_libraries(JzRuntimePlatform
    PUBLIC
        JzRuntimeCore
        glad::glad
        glfw
        ${OPENGL_LIBS}
        ${PLATFORM_LIBS}
)
