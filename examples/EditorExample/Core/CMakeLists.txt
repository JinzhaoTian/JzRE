# Editor Core Module - Header-only core editor components
# This is an INTERFACE library because it contains only headers (no .cpp files)
add_library(JzEditorCore INTERFACE)

file(GLOB_RECURSE CORE_HEADERS CONFIGURE_DEPENDS "include/*.h" "include/*.inl")

# Add headers as interface sources for IDE visibility
target_sources(JzEditorCore INTERFACE ${CORE_HEADERS})

target_compile_features(
    JzEditorCore
    INTERFACE
    cxx_std_20
)

target_include_directories(
    JzEditorCore
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    JzEditorCore
    INTERFACE
    JzRERuntime
    JzRuntimeFunction
    JzRuntimeResource
    JzRuntimePlatform
    JzRuntimeCore
)
