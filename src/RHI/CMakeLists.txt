file(GLOB JzRERHI_SOURCES CONFIGURE_DEPENDS 
    "src/*.cpp"
)

add_library(JzRERHI STATIC
    ${JzRERHI_SOURCES}
)

target_include_directories(JzRERHI PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(JzRERHI
    PUBLIC
    JzRECore
)