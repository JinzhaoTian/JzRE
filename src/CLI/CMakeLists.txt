# CLI module - provides reusable command implementation and executable entrypoint.
find_package(nlohmann_json CONFIG REQUIRED)

file(GLOB_RECURSE CLI_HEADERS CONFIGURE_DEPENDS "include/*.h")
file(GLOB_RECURSE CLI_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
list(FILTER CLI_SOURCES EXCLUDE REGEX ".*/src/main\\.cpp$")

add_library(JzCLI OBJECT)
target_sources(JzCLI PRIVATE ${CLI_HEADERS} ${CLI_SOURCES})
target_compile_features(
    JzCLI
    PUBLIC
    cxx_std_20
)
target_include_directories(
    JzCLI
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(
    JzCLI
    PUBLIC
    JzRERuntime
    PRIVATE
    nlohmann_json::nlohmann_json
)
source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES ${CLI_HEADERS} ${CLI_SOURCES}
)
set_target_properties(JzCLI PROPERTIES FOLDER "CLI")

add_executable(JzRE src/main.cpp)
target_compile_features(
    JzRE
    PUBLIC
    cxx_std_20
)
target_link_libraries(
    JzRE
    PRIVATE
    JzCLI
    JzRERuntime
    JzRuntimeFunction
    JzRuntimeResource
    JzRuntimePlatform
    JzRuntimeCore
)
if(TARGET JzREShaderTool)
    add_dependencies(JzRE JzREShaderTool)
endif()
source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)
set_target_properties(JzRE PROPERTIES
    FOLDER "CLI"
    OUTPUT_NAME "JzRE"
)
