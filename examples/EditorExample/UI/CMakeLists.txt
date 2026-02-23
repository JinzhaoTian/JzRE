# Editor UI Module - ImGui-based UI widgets
add_library(JzEditorUI STATIC)

file(GLOB_RECURSE UI_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE UI_HEADERS CONFIGURE_DEPENDS "include/*.h")

target_sources(JzEditorUI PRIVATE ${UI_SOURCES} ${UI_HEADERS})

# Organize sources in Visual Studio filters
source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES ${UI_SOURCES} ${UI_HEADERS}
)

# Set target folder in Visual Studio
set_target_properties(JzEditorUI PROPERTIES FOLDER "Editor")

target_compile_features(
    JzEditorUI
    PUBLIC
    cxx_std_20
)

target_include_directories(
    JzEditorUI
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# External dependencies
find_package(imgui CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

target_link_libraries(
    JzEditorUI
    PUBLIC
    imgui::imgui
    Vulkan::Vulkan
    JzEditorCore
)
