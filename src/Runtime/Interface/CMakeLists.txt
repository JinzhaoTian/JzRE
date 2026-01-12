# Runtime Interface Layer - Runtime interface framework
add_library(JzREInterface STATIC)

file(GLOB_RECURSE RUNTIME_APP_SOURCES "src/*.cpp")
file(GLOB_RECURSE RUNTIME_APP_HEADERS "include/*.h")

target_sources(JzREInterface PRIVATE ${RUNTIME_APP_SOURCES} ${RUNTIME_APP_HEADERS})

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
