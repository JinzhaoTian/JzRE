file(GLOB JzREECS_SOURCES CONFIGURE_DEPENDS 
    "src/*.cpp"
)

add_library(
    JzREECS
    STATIC
    ${JzREECS_SOURCES}
)

target_compile_features(
    JzREECS
    PUBLIC
    cxx_std_20
)

target_include_directories(
    JzREECS
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    JzREECS
    PUBLIC
    JzRECore
    JzRERHI
    JzREResource
)