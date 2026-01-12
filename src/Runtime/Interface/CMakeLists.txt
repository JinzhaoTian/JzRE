# Runtime Interface Layer - Runtime interface framework
add_library(JzREInterface STATIC)

file(GLOB_RECURSE RUNTIME_APP_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE RUNTIME_APP_HEADERS CONFIGURE_DEPENDS "include/*.h")

target_sources(JzREInterface PRIVATE ${RUNTIME_APP_SOURCES} ${RUNTIME_APP_HEADERS})

# Organize sources in Visual Studio filters
source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES ${RUNTIME_APP_SOURCES} ${RUNTIME_APP_HEADERS}
)

# Set target folder in Visual Studio
set_target_properties(JzREInterface PROPERTIES FOLDER "Runtime")

target_include_directories(JzREInterface
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(JzREInterface
    PUBLIC
        JzRuntimeCore
        JzRuntimePlatform
        JzRuntimeResource
        JzRuntimeFunction
)

