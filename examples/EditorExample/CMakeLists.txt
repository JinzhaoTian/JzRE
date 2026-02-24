cmake_minimum_required(VERSION 3.25.0)

project(EditorExample VERSION 0.0.1 LANGUAGES CXX C)

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
set(JzRE_OUTPUT_DIR ${CMAKE_BINARY_DIR}/EditorExample)
set(JzRE_ENGINE_SHADER_SOURCE_DIR "${JzRE_REPOSITORY_ROOT}/src/EngineContent/ShaderSource")
set(JzRE_EDITOR_SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/EditorContent/ShaderSource")
set(JzRE_ENGINE_SHADER_OUTPUT_DIR "${JzRE_OUTPUT_DIR}/EngineContent/Shaders")

list(APPEND CMAKE_MODULE_PATH "${JzRE_REPOSITORY_ROOT}/cmake/modules")
include(JzShaderCook)
option(JzRE_EDITOR_ENABLE_ENGINE_SHADER_COOK "Cook engine shader artifacts for EditorExample" ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${JzRE_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${JzRE_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${JzRE_OUTPUT_DIR})

foreach(CONFIG_TYPE DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${JzRE_OUTPUT_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${JzRE_OUTPUT_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${JzRE_OUTPUT_DIR})
endforeach()

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resources/" DESTINATION ${JzRE_OUTPUT_DIR}/)

if(JzRE_EDITOR_ENABLE_ENGINE_SHADER_COOK)
    if(NOT TARGET JzREShaderTool)
        add_subdirectory("${JzRE_REPOSITORY_ROOT}/programs/JzREShaderTool" "${CMAKE_CURRENT_BINARY_DIR}/JzREShaderTool")
    endif()
endif()
if(NOT TARGET JzRERuntime)
    add_subdirectory("${JzRE_REPOSITORY_ROOT}/src/Runtime" "${CMAKE_CURRENT_BINARY_DIR}/Runtime")
endif()

if(JzRE_EDITOR_ENABLE_ENGINE_SHADER_COOK)
    JzRE_add_shader_cook_target(
        TARGET_NAME JzRECookEngineShaders
        SOURCE_DIR ${JzRE_ENGINE_SHADER_SOURCE_DIR}
        OUTPUT_DIR ${JzRE_ENGINE_SHADER_OUTPUT_DIR}
        TOOL_TARGET JzREShaderTool
    )
    JzRE_add_shader_cook_target(
        TARGET_NAME JzRECookEditorShaders
        SOURCE_DIR ${JzRE_EDITOR_SHADER_SOURCE_DIR}
        OUTPUT_DIR ${JzRE_ENGINE_SHADER_OUTPUT_DIR}
        TOOL_TARGET JzREShaderTool
    )
    add_custom_target(JzRECookAllShaders ALL
        DEPENDS JzRECookEngineShaders JzRECookEditorShaders
    )
else()
    add_custom_target(JzRECookAllShaders
        COMMAND ${CMAKE_COMMAND} -E echo "JzRECookAllShaders disabled"
        VERBATIM
    )
endif()

add_subdirectory(Core)
add_subdirectory(UI)
add_subdirectory(Panels)
add_subdirectory(Application)

add_executable(
    EditorExample
    main.cpp
)

source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES main.cpp
)

set_target_properties(EditorExample PROPERTIES FOLDER "Examples/EditorExample")

target_compile_features(
    EditorExample
    PUBLIC
    cxx_std_20
)

target_link_libraries(
    EditorExample
    PUBLIC
    JzEditor
)

add_dependencies(EditorExample JzRECookAllShaders)
