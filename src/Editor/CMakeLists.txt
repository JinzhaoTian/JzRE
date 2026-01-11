# Editor module - depends on Runtime
file(GLOB_RECURSE JzEditor_SOURCES CONFIGURE_DEPENDS 
    "src/*.cpp"
)

add_library(
    JzEditor
    STATIC
    ${JzEditor_SOURCES}
)

target_compile_features(
    JzEditor
    PUBLIC
    cxx_std_20
)

target_include_directories(
    JzEditor
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# External dependencies
find_package(imgui CONFIG REQUIRED)

target_link_libraries(
    JzEditor
    PUBLIC
    imgui::imgui
    JzRuntime
)