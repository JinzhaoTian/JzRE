file(GLOB JzREGraphics_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/*.cpp"
)

add_library(JzREGraphics STATIC
    ${JzREGraphics_SOURCES}
)

target_include_directories(JzREGraphics PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/"
    "${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/"
)

target_link_libraries(JzREGraphics
    PRIVATE
    OPENGL_LIBS
    JzRECore
    JzRERHI
)