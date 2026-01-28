# Runtime Function Layer - Rendering, Scene, ECS, Window
add_library(JzRuntimeFunction STATIC)

# Collect all sources by subsystem
file(GLOB_RECURSE FUNCTION_HEADERS CONFIGURE_DEPENDS "include/*.h" "include/*.inl")

# ECS sources (all files now in ECS/ directory, including input system)
file(GLOB ECS_SOURCES CONFIGURE_DEPENDS "src/ECS/*.cpp")

# Scene sources
file(GLOB SCENE_SOURCES CONFIGURE_DEPENDS "src/Scene/*.cpp")

# Combine all sources
set(ALL_FUNCTION_SOURCES
    ${FUNCTION_HEADERS}
    ${ECS_SOURCES}
    ${SCENE_SOURCES}
)

target_sources(JzRuntimeFunction PRIVATE ${ALL_FUNCTION_SOURCES})

# Organize sources in Visual Studio filters
source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES ${ALL_FUNCTION_SOURCES}
)

# Set target folder in Visual Studio
set_target_properties(JzRuntimeFunction PROPERTIES FOLDER "Runtime")

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
        JzRuntimeResource
)

