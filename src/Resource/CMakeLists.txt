add_library(JzREResource STATIC)

file(GLOB JzREResource_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

target_include_directories(JzREResource PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(JzREResource PRIVATE
     ${JzREResource_SOURCES}
)

target_link_libraries(JzREResource 
    PRIVATE
    JzRECore
)