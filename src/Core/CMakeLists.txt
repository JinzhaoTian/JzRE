file(GLOB SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(JzRECore STATIC ${SOURCES})

target_include_directories(JzRECore PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(JzRECore PUBLIC
    spdlog::spdlog
)