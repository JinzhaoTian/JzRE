cmake_minimum_required(VERSION 3.25.0)

# set project name
project(JzRE VERSION 0.0.1 LANGUAGES CXX C)

# set c/c++ standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Suppress duplicate library warnings on macOS
# This can be caused by vcpkg packages having overlapping transitive dependencies
if(APPLE)
    add_link_options("-Wl,-no_warn_duplicate_libraries")
endif()

# Enable folder organization in Visual Studio
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Add CMake modules path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# Include helper modules
include(JzSourceGroups)
include(JzShaderCook)

option(JzRE_ENABLE_ENGINE_SHADER_COOK "Cook engine shader artifacts during build" ON)

# set output path (works for both single-config and multi-config generators)
set(JzRE_OUTPUT_DIR ${CMAKE_BINARY_DIR}/JzRE)

# For single-config generators (Ninja, Makefiles)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${JzRE_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${JzRE_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${JzRE_OUTPUT_DIR})

# For multi-config generators (Visual Studio, Xcode)
# These override the per-config subdirectories
foreach(CONFIG_TYPE DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${JzRE_OUTPUT_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${JzRE_OUTPUT_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${JzRE_OUTPUT_DIR})
endforeach()

# Keep non-shader engine assets synchronized for runtime samples.
add_custom_target(JzRESyncEngineAssets ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${JzRE_OUTPUT_DIR}/EngineContent
    COMMAND ${CMAKE_COMMAND} -E make_directory ${JzRE_OUTPUT_DIR}/EngineContent/Models
    COMMAND ${CMAKE_COMMAND} -E make_directory ${JzRE_OUTPUT_DIR}/EngineContent/Textures
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/examples/RuntimeExample/Models
            ${JzRE_OUTPUT_DIR}/EngineContent/Models
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/examples/RuntimeExample/Textures
            ${JzRE_OUTPUT_DIR}/EngineContent/Textures
    COMMENT "Synchronizing EngineContent models/textures"
)

# project root path
set(JzRE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(JzRE_ENGINE_SHADER_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/EngineContent/ShaderSource")
set(JzRE_ENGINE_SHADER_OUTPUT_DIR "${JzRE_OUTPUT_DIR}/EngineContent/Shaders")

# add programs
add_subdirectory(programs)

if(JzRE_ENABLE_ENGINE_SHADER_COOK)
    # Cook engine shader source manifests into runtime-ready EngineContent artifacts.
    jzre_add_shader_cook_target(
        TARGET_NAME JzRECookEngineShaders
        SOURCE_DIR ${JzRE_ENGINE_SHADER_SOURCE_DIR}
        OUTPUT_DIR ${JzRE_ENGINE_SHADER_OUTPUT_DIR}
        TOOL_TARGET JzREShaderTool
    )
else()
    add_custom_target(JzRECookEngineShaders
        COMMAND ${CMAKE_COMMAND} -E echo "JzRECookEngineShaders disabled"
        VERBATIM
    )
endif()

# add src module
add_subdirectory(src)

if(TARGET JzRERuntime)
    add_dependencies(JzRERuntime JzRESyncEngineAssets)
    add_dependencies(JzRERuntime JzRECookEngineShaders)
endif()

# add examples
add_subdirectory(examples)

# add tests module
option(BUILD_TESTS "Build the tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
