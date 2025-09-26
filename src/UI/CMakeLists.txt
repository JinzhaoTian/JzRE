file(GLOB JzREUI_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(JzREUI STATIC
    ${JzREUI_SOURCES}
)

target_include_directories(JzREUI PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(JzREUI 
    PRIVATE
    JzCore
    imgui::imgui
)