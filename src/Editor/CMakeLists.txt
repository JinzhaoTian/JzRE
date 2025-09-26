file(GLOB JzREEditor_SOURCES CONFIGURE_DEPENDS 
    "src/*.cpp"
)

add_library(
    JzREEditor
    STATIC
    ${JzREEditor_SOURCES}
)

target_include_directories(
    JzREEditor
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    JzREEditor
    PUBLIC
    imgui::imgui
    JzRECore
    JzRERHI
    JzREGraphics
    JzREResource
    JzREECS
    JzREPlatform
    JzREUI
)