# Editor module - depends on Runtime, UI, Panels, and Core
add_subdirectory(Core)
add_subdirectory(UI)
add_subdirectory(Panels)
add_subdirectory(Application)

# Main executable - JzRE Editor Application
add_executable(
    JzRE
    main.cpp
)

# Organize main.cpp in Visual Studio filters
source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES main.cpp
)

# Set target folder in Visual Studio
set_target_properties(JzRE PROPERTIES FOLDER "Editor")

target_compile_features(
    JzRE
    PUBLIC
    cxx_std_20
)

target_link_libraries(
    JzRE
    PUBLIC
    JzEditor
)
