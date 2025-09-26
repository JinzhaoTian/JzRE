file(GLOB JzREUI_SOURCES CONFIGURE_DEPENDS 
    "src/*.cpp"
)

add_library(JzREUI STATIC
    ${JzREUI_SOURCES}
)

target_include_directories(JzREUI PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(JzREUI
    PUBLIC
    imgui::imgui
    JzRECore
    JzRERHI
)