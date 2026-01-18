# Runtime Interface Layer
add_library(JzRERuntime STATIC)

file(GLOB_RECURSE RUNTIME_APP_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE RUNTIME_APP_HEADERS CONFIGURE_DEPENDS "include/*.h")

target_sources(JzRERuntime PRIVATE ${RUNTIME_APP_SOURCES} ${RUNTIME_APP_HEADERS})

# Organize sources in Visual Studio filters
source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES ${RUNTIME_APP_SOURCES} ${RUNTIME_APP_HEADERS}
)

# Set target folder in Visual Studio
set_target_properties(JzRERuntime PROPERTIES FOLDER "Runtime")

target_compile_features(JzRERuntime PUBLIC cxx_std_20)

target_include_directories(JzRERuntime
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link top-level runtime layer (includes Core, Platform, Resource transitively)
target_link_libraries(JzRERuntime 
    PUBLIC
        JzRuntimeFunction
)
