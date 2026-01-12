# Runtime Core Layer - Basic utilities and types
add_library(JzRuntimeCore STATIC)

file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE CORE_HEADERS CONFIGURE_DEPENDS "include/*.h" "include/*.inl")

target_sources(JzRuntimeCore PRIVATE ${CORE_SOURCES} ${CORE_HEADERS})

# Organize sources in Visual Studio filters
source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES ${CORE_SOURCES} ${CORE_HEADERS}
)

# Set target folder in Visual Studio
set_target_properties(JzRuntimeCore PROPERTIES FOLDER "Runtime")

target_include_directories(JzRuntimeCore
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# External dependencies
find_package(spdlog CONFIG REQUIRED)

target_link_libraries(JzRuntimeCore
    PUBLIC
        spdlog::spdlog
)
