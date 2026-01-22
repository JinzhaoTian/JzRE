# Editor Core Module - Core editor components
add_library(JzEditor STATIC)

file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE CORE_HEADERS CONFIGURE_DEPENDS "include/*.h")

target_sources(JzEditor PRIVATE ${CORE_SOURCES} ${CORE_HEADERS})

# Organize sources in Visual Studio filters
source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES ${CORE_SOURCES} ${CORE_HEADERS}
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

target_link_libraries(
    JzEditor
    PUBLIC
    JzEditorPanels
)
