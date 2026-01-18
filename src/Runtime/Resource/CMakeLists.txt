# Runtime Resource Layer - Resource loading and management
add_library(JzRuntimeResource STATIC)

file(GLOB_RECURSE RESOURCE_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE RESOURCE_HEADERS CONFIGURE_DEPENDS "include/*.h")

target_sources(JzRuntimeResource PRIVATE ${RESOURCE_SOURCES} ${RESOURCE_HEADERS})

# Organize sources in Visual Studio filters
source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES ${RESOURCE_SOURCES} ${RESOURCE_HEADERS}
)

# Set target folder in Visual Studio
set_target_properties(JzRuntimeResource PROPERTIES FOLDER "Runtime")

target_include_directories(JzRuntimeResource
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# External dependencies
find_package(assimp CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(Freetype REQUIRED)

target_link_libraries(JzRuntimeResource
    PUBLIC
        JzRuntimeCore
        JzRuntimePlatform
        assimp::assimp
        Freetype::Freetype
)

target_include_directories(JzRuntimeResource PRIVATE ${Stb_INCLUDE_DIR})

