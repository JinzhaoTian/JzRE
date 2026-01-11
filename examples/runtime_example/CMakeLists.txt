add_executable(
    RuntimeExample
    main.cpp
)

target_compile_features(
    RuntimeExample
    PUBLIC
    cxx_std_20
)

target_include_directories(
    RuntimeExample
    PUBLIC
    ${CMAKE_SOURCE_DIR}/src/App/include
)

target_link_libraries(
    RuntimeExample
    PUBLIC
    JzREApp
)
