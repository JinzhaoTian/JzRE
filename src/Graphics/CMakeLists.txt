# add platform-specific libraries and OpenGL
if(WIN32)
    find_package(OpenGL REQUIRED)
    set(OPENGL_LIBS ${OpenGL})
elseif(APPLE)
    find_library(OPENGL_LIB OpenGL)
    find_library(GLUT_LIB GLUT)
    set(OPENGL_LIBS ${OPENGL_LIB} ${GLUT_LIB})
elseif(UNIX AND NOT APPLE)
    find_package(OpenGL REQUIRED)
    set(OPENGL_LIBS ${OpenGL})
endif()

file(GLOB JzREGraphics_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/*.cpp"
)

add_library(JzREGraphics STATIC
    ${JzREGraphics_SOURCES}
)

target_include_directories(JzREGraphics PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/"
)

target_link_libraries(JzREGraphics
    PUBLIC
    OPENGL_LIBS
    glad::glad
    JzRECore
    JzRERHI
)