add_library(JzRERHI INTERFACE)

target_include_directories(JzRERHI INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(JzRERHI INTERFACE
    JzCore
)