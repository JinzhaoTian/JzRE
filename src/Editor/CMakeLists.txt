add_library(JzREEditor STATIC)

file(GLOB JzREEditor_SOURCES CONFIGURE_DEPENDS 
    "src/*.cpp"
)

target_include_directories(JzREEditor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(JzREEditor PRIVATE
    ${JzREEditor_SOURCES}
)

target_link_libraries(JzREEditor
    PRIVATE
    JzRECore
    JzRERHI
    JzREECS
    JzREResource
    JzREPlatform
    JzREUI
    imgui::imgui
)