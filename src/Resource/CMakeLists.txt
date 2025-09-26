file(GLOB JzREResource_SOURCES CONFIGURE_DEPENDS 
    "src/*.cpp"
)

add_library(
    JzREResource
    STATIC
    ${JzREResource_SOURCES}
)

target_compile_features(
    JzREResource
    PUBLIC
    cxx_std_20
)

target_include_directories(
    JzREResource
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    JzREResource
    PUBLIC
    assimp::assimp
    JzRECore
    JzRERHI
    JzREGraphics
)