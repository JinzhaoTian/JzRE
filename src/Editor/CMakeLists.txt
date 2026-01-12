# Editor module - depends on Runtime
file(GLOB_RECURSE JzEditor_HEADERS CONFIGURE_DEPENDS "include/*.h")
file(GLOB_RECURSE JzEditor_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

# Exclude main.cpp from library sources if it exists
list(FILTER JzEditor_SOURCES EXCLUDE REGEX ".*main\\.cpp$")

add_library(
    JzEditor
    STATIC
    ${JzEditor_SOURCES}
    ${JzEditor_HEADERS}
)

# Organize sources in Visual Studio filters
source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES ${JzEditor_SOURCES} ${JzEditor_HEADERS}
)

# Set target folder in Visual Studio
set_target_properties(JzEditor PROPERTIES FOLDER "Editor")

target_compile_features(
    JzEditor
    PUBLIC
    cxx_std_20
)

target_include_directories(
    JzEditor
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# External dependencies
find_package(imgui CONFIG REQUIRED)

target_link_libraries(
    JzEditor
    PUBLIC
    imgui::imgui
    JzRERuntime
)

# Main executable - JzRE Editor Application
add_executable(
    JzRE
    src/main.cpp
)

# Organize main.cpp in Visual Studio filters
source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES src/main.cpp
)

# Set target folder in Visual Studio
set_target_properties(JzRE PROPERTIES FOLDER "Editor")

target_compile_features(
    JzRE
    PUBLIC
    cxx_std_20
)

target_link_libraries(
    JzRE
    PUBLIC
    JzEditor
)
