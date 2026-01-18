# Editor Panels Module - Editor panel windows
add_library(JzEditorPanels STATIC)

file(GLOB_RECURSE PANELS_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE PANELS_HEADERS CONFIGURE_DEPENDS "include/*.h")

target_sources(JzEditorPanels PRIVATE ${PANELS_SOURCES} ${PANELS_HEADERS})

# Organize sources in Visual Studio filters
source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES ${PANELS_SOURCES} ${PANELS_HEADERS}
)

# Set target folder in Visual Studio
set_target_properties(JzEditorPanels PROPERTIES FOLDER "Editor")

target_compile_features(
    JzEditorPanels
    PUBLIC
    cxx_std_20
)

target_include_directories(
    JzEditorPanels
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    # Access Editor include directory for shared headers like JzContext
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# External dependencies
find_package(imgui CONFIG REQUIRED)

target_link_libraries(
    JzEditorPanels
    PUBLIC
    imgui::imgui
    JzRERuntime
    JzEditorUI
)
