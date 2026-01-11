# Runtime Core Layer - Basic utilities and types
add_library(JzRuntimeCore STATIC)

file(GLOB_RECURSE CORE_SOURCES "src/*.cpp")
file(GLOB_RECURSE CORE_HEADERS "include/*.h" "include/*.inl")

target_sources(JzRuntimeCore PRIVATE ${CORE_SOURCES} ${CORE_HEADERS})

target_include_directories(JzRuntimeCore
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# External dependencies
find_package(spdlog CONFIG REQUIRED)

target_link_libraries(JzRuntimeCore
    PUBLIC
        spdlog::spdlog
)
