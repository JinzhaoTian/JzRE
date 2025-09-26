file(GLOB JzRE_SOURCES CONFIGURE_DEPENDS 
    "src/*.cpp"
)

add_executable(JzRE ${JzRE_SOURCES})

target_include_directories(JzRE PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(JzRE
    PUBLIC
    JzREEditor
)