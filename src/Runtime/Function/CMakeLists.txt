# Runtime Function Layer - Rendering, Scene, ECS, Input, Window
add_library(JzRuntimeFunction STATIC)

# Collect all sources by subsystem
file(GLOB_RECURSE FUNCTION_HEADERS "include/*.h" "include/*.inl")

# ECS sources
file(GLOB ECS_SOURCES "src/ECS/*.cpp")
file(GLOB ENTT_SOURCES "src/ECS/EnTT/*.cpp")

# Rendering sources
file(GLOB RENDERING_SOURCES "src/Rendering/*.cpp")

# Scene sources
file(GLOB SCENE_SOURCES "src/Scene/*.cpp")

# Input sources
file(GLOB INPUT_SOURCES "src/Input/*.cpp")

# Window sources
file(GLOB WINDOW_SOURCES "src/Window/*.cpp")

target_sources(JzRuntimeFunction PRIVATE
    ${FUNCTION_HEADERS}
    ${ECS_SOURCES}
    ${ENTT_SOURCES}
    ${RENDERING_SOURCES}
    ${SCENE_SOURCES}
    ${INPUT_SOURCES}
    ${WINDOW_SOURCES}
)

target_include_directories(JzRuntimeFunction
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# EnTT dependency (header-only)
find_package(EnTT CONFIG QUIET)
if(EnTT_FOUND)
    target_link_libraries(JzRuntimeFunction PUBLIC EnTT::EnTT)
endif()

target_link_libraries(JzRuntimeFunction
    PUBLIC
        JzRuntimeCore
        JzRuntimePlatform
        JzRuntimeResource
)
