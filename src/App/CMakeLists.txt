file(GLOB JzRE_SOURCES CONFIGURE_DEPENDS 
    "src/*.cpp"
)

add_executable(
    JzRE
    ${JzRE_SOURCES}
)

target_compile_features(
    JzRE
    PUBLIC
    cxx_std_20
)

target_include_directories(
    JzRE
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    JzRE
    PUBLIC
    JzREEditor
)