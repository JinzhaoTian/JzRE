cmake_minimum_required(VERSION 3.25.0)

project(RuntimeExample VERSION 0.0.1 LANGUAGES CXX C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(APPLE)
    add_link_options("-Wl,-no_warn_duplicate_libraries")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(JzRE_REPOSITORY_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(JzRE_OUTPUT_DIR ${CMAKE_BINARY_DIR}/RuntimeExample)
set(JzRE_ENGINE_SHADER_SOURCE_DIR "${JzRE_REPOSITORY_ROOT}/src/EngineContent/ShaderSource")
set(JzRE_ENGINE_SHADER_OUTPUT_DIR "${JzRE_OUTPUT_DIR}/EngineContent/Shaders")

list(APPEND CMAKE_MODULE_PATH "${JzRE_REPOSITORY_ROOT}/cmake/modules")
include(JzShaderCook)
option(JzRE_RUNTIME_EXAMPLE_SYNC_ENGINE_ASSETS "Synchronize RuntimeExample EngineContent models/textures" ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${JzRE_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${JzRE_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${JzRE_OUTPUT_DIR})

foreach(CONFIG_TYPE DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${JzRE_OUTPUT_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${JzRE_OUTPUT_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${JzRE_OUTPUT_DIR})
endforeach()

if(NOT TARGET JzREShaderTool)
    add_subdirectory("${JzRE_REPOSITORY_ROOT}/programs/JzREShaderTool" "${CMAKE_CURRENT_BINARY_DIR}/JzREShaderTool")
endif()
if(NOT TARGET JzRERuntime)
    add_subdirectory("${JzRE_REPOSITORY_ROOT}/src/Runtime" "${CMAKE_CURRENT_BINARY_DIR}/Runtime")
endif()

JzRE_add_shader_cook_target(
    TARGET_NAME RuntimeExampleCookEngineShaders
    SOURCE_DIR ${JzRE_ENGINE_SHADER_SOURCE_DIR}
    OUTPUT_DIR ${JzRE_ENGINE_SHADER_OUTPUT_DIR}
    TOOL_TARGET JzREShaderTool
)

if(JzRE_RUNTIME_EXAMPLE_SYNC_ENGINE_ASSETS)
    add_custom_target(RuntimeExampleSyncAssets ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${JzRE_OUTPUT_DIR}/EngineContent
        COMMAND ${CMAKE_COMMAND} -E make_directory ${JzRE_OUTPUT_DIR}/EngineContent/Models
        COMMAND ${CMAKE_COMMAND} -E make_directory ${JzRE_OUTPUT_DIR}/EngineContent/Textures
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/Models
                ${JzRE_OUTPUT_DIR}/EngineContent/Models
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/Textures
                ${JzRE_OUTPUT_DIR}/EngineContent/Textures
        COMMENT "Synchronizing RuntimeExample EngineContent models/textures"
    )
endif()

add_executable(
    RuntimeExample
    main.cpp
)

source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES main.cpp
)

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
    JzRuntimeFunction
    JzRuntimeResource
    JzRuntimePlatform
    JzRuntimeCore
)

if(TARGET RuntimeExampleCookEngineShaders)
    add_dependencies(RuntimeExample RuntimeExampleCookEngineShaders)
endif()

if(TARGET RuntimeExampleSyncAssets)
    add_dependencies(RuntimeExample RuntimeExampleSyncAssets)
endif()
