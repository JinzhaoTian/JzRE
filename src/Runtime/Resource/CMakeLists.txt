# Runtime Resource Layer - Resource loading and management
add_library(JzRuntimeResource STATIC)

file(GLOB_RECURSE RESOURCE_SOURCES "src/*.cpp")
file(GLOB_RECURSE RESOURCE_HEADERS "include/*.h")

target_sources(JzRuntimeResource PRIVATE ${RESOURCE_SOURCES} ${RESOURCE_HEADERS})

target_include_directories(JzRuntimeResource
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# External dependencies
find_package(assimp CONFIG REQUIRED)
find_package(Stb REQUIRED)

target_link_libraries(JzRuntimeResource
    PUBLIC
        JzRuntimeCore
        JzRuntimePlatform
        assimp::assimp
)

target_include_directories(JzRuntimeResource PRIVATE ${Stb_INCLUDE_DIR})
