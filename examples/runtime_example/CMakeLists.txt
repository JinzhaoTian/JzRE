add_executable(
    RuntimeExample
    main.cpp
)

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
