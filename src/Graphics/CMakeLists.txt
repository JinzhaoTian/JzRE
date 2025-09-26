file(GLOB JzREGraphics_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/*.cpp"
    # "${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/*.cpp"
)

add_library(
    JzREGraphics
    STATIC
    ${JzREGraphics_SOURCES}
)

target_compile_features(
    JzREGraphics
    PUBLIC
    cxx_std_20
)

target_include_directories(
    JzREGraphics
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/"
)

target_link_libraries(JzREGraphics
    PUBLIC
    ${OPENGL_LIBS}
    glad::glad
    JzRECore
    JzRERHI
)