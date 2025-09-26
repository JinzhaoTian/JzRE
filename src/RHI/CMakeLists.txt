file(GLOB JzRERHI_SOURCES CONFIGURE_DEPENDS 
    "src/*.cpp"
)

add_library(JzRERHI INTERFACE
    ${JzRERHI_SOURCES}
)

target_include_directories(JzRERHI INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(JzRERHI INTERFACE
    JzRECore
)