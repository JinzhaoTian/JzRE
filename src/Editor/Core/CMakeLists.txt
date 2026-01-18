# Editor Core Module - Core editor components
add_library(JzEditorCore STATIC)

file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE CORE_HEADERS CONFIGURE_DEPENDS "include/*.h")

target_sources(JzEditorCore PRIVATE ${CORE_SOURCES} ${CORE_HEADERS})

# Organize sources in Visual Studio filters
source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES ${CORE_SOURCES} ${CORE_HEADERS}
)

# Set target folder in Visual Studio
set_target_properties(JzEditorCore PROPERTIES FOLDER "Editor")

target_compile_features(
    JzEditorCore
    PUBLIC
    cxx_std_20
)

target_include_directories(
    JzEditorCore
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# External dependencies
find_package(imgui CONFIG REQUIRED)

target_link_libraries(
    JzEditorCore
    PUBLIC
    imgui::imgui
    JzRERuntime
    JzEditorUI
    JzEditorPanels
)
