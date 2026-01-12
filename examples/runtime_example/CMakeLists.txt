add_executable(
    RuntimeExample
    main.cpp
)

# Organize sources in Visual Studio filters
source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES main.cpp
)

# Set target folder in Visual Studio
set_target_properties(RuntimeExample PROPERTIES FOLDER "Examples")

target_compile_features(
    RuntimeExample
    PUBLIC
    cxx_std_20
)

target_link_libraries(
    RuntimeExample
    PUBLIC
    JzRERuntime
)
