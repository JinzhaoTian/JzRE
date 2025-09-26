add_library(JzREECS STATIC)

file(GLOB JzREECS_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

target_include_directories(JzREECS PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(JzREECS PRIVATE
     ${JzREECS_SOURCES}
)

target_link_libraries(JzREECS
    PRIVATE
    JzRECore
)