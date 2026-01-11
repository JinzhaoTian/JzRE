# JzRE Application - Core runtime library and main executable

# App library (contains JzRERuntime, JzREHub logic)
file(GLOB JzREApp_SOURCES CONFIGURE_DEPENDS 
    "src/JzRERuntime.cpp"
    "src/JzREHub.cpp"
)

add_library(
    JzREApp
    STATIC
    ${JzREApp_SOURCES}
)

target_compile_features(
    JzREApp
    PUBLIC
    cxx_std_20
)

target_include_directories(
    JzREApp
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    JzREApp
    PUBLIC
    JzEditor
)

# Main executable - Editor application
file(GLOB JzRE_MAIN_SOURCES CONFIGURE_DEPENDS 
    "src/main.cpp"
    "src/JzREInstance.cpp"
)

add_executable(
    JzRE
    ${JzRE_MAIN_SOURCES}
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
    JzREApp
)