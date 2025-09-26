file(GLOB JzRE_SOURCES CONFIGURE_DEPENDS 
    "src/*.cpp"
)

add_executable(JzRE ${JzRE_SOURCES})

target_include_directories(JzRECore PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(JzRE
    PRIVATE
    JzRECore
    JzRERHI
    JzREGraphics
    JzREECS
    JzREResource
    JzREUI
    JzREPlatform
    JzREEditor
)