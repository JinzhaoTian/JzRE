cmake_minimum_required(VERSION 3.15.0)

# set project name
project(JzRE)

# set c/c++ standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set generated code output path
set(GENERATED_DIR ${CMAKE_BINARY_DIR}/generated)
include_directories(${GENERATED_DIR})

# add modules
add_subdirectory(tools)

# Function to generate reflection code
function(generate_reflection_code target_name)
    # Parse arguments
    set(options)
    set(oneValueArgs OUTPUT_DIR)
    set(multiValueArgs HEADER_FILES INCLUDE_PATHS)
    cmake_parse_arguments(GEN_REFLECT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    # Set default output directory
    if(NOT GEN_REFLECT_OUTPUT_DIR)
        set(GEN_REFLECT_OUTPUT_DIR ${GENERATED_DIR})
    endif()
    
    # Ensure output directory exists
    file(MAKE_DIRECTORY ${GEN_REFLECT_OUTPUT_DIR})
    
    # Create list of header files to process
    set(HEADER_FILES_TO_PROCESS)
    foreach(header ${GEN_REFLECT_HEADER_FILES})
        if(NOT IS_ABSOLUTE ${header})
            set(header ${CMAKE_CURRENT_SOURCE_DIR}/${header})
        endif()
        list(APPEND HEADER_FILES_TO_PROCESS ${header})
    endforeach()
    
    # Generated files
    set(GENERATED_HEADER ${GEN_REFLECT_OUTPUT_DIR}/JzRE.generated.h)
    set(GENERATED_SOURCE ${GEN_REFLECT_OUTPUT_DIR}/JzRE.generated.cpp)
    
    # Build include path arguments
    set(INCLUDE_ARGS)
    foreach(include_path ${GEN_REFLECT_INCLUDE_PATHS})
        list(APPEND INCLUDE_ARGS -I${include_path})
    endforeach()
    
    # Add standard include paths
    list(APPEND INCLUDE_ARGS 
        -I${CMAKE_CURRENT_SOURCE_DIR}
        -I${PROJECT_SOURCE_DIR}/src
        -std=c++20
    )
    
    # Create custom command to generate reflection code
    add_custom_command(
        OUTPUT ${GENERATED_HEADER} ${GENERATED_SOURCE}
        COMMAND JzREHeaderTool
            --output-dir=${GEN_REFLECT_OUTPUT_DIR}
            --source-root=${PROJECT_SOURCE_DIR}
            --verbose
            ${HEADER_FILES_TO_PROCESS}
            --
            ${INCLUDE_ARGS}
        DEPENDS JzREHeaderTool ${HEADER_FILES_TO_PROCESS}
        COMMENT "Generating reflection code for ${target_name}"
        VERBATIM
    )
    
    # Create custom target for generated files
    add_custom_target(${target_name}_reflection
        DEPENDS ${GENERATED_HEADER} ${GENERATED_SOURCE}
    )
    
    # Set variables for parent scope
    set(${target_name}_GENERATED_HEADER ${GENERATED_HEADER} PARENT_SCOPE)
    set(${target_name}_GENERATED_SOURCE ${GENERATED_SOURCE} PARENT_SCOPE)
    set(${target_name}_REFLECTION_TARGET ${target_name}_reflection PARENT_SCOPE)
endfunction()

add_subdirectory(src)
