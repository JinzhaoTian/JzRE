file(GLOB JzRECore_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(JzRECore STATIC
    ${JzRECore_SOURCES}
)

target_include_directories(JzRECore PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(JzRECore PUBLIC
    spdlog::spdlog
)