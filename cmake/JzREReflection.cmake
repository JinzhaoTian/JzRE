# JzRE Reflection System CMake Module
# This module provides functions to integrate JzREHeaderTool into the build process

# Function to generate reflection code for a target
# Usage: jzre_generate_reflection(target_name [SOURCES source1 source2 ...] [HEADERS header1 header2 ...])
function(jzre_generate_reflection target_name)
    message(${target_name})
    cmake_parse_arguments(JZRE_REFLECTION "" "" "SOURCES;HEADERS" ${ARGN})
    
    # Check if JzREHeaderTool target exists
    if(NOT TARGET JzREHeaderTool)
        message(FATAL_ERROR "JzREHeaderTool target not found. Make sure to add tools subdirectory before calling jzre_generate_reflection.")
    endif()
    
    # Create generated directory
    set(GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
    file(MAKE_DIRECTORY ${GENERATED_DIR})
    
    # Collect all header files if not specified
    set(ALL_HEADERS ${JZRE_REFLECTION_HEADERS})
    if(NOT ALL_HEADERS AND JZRE_REFLECTION_SOURCES)
        foreach(source_file ${JZRE_REFLECTION_SOURCES})
            get_filename_component(file_ext ${source_file} EXT)
            if(file_ext STREQUAL ".h" OR file_ext STREQUAL ".hpp")
                list(APPEND ALL_HEADERS ${source_file})
            endif()
        endforeach()
    endif()
    
    # If no headers specified, scan the target's source directory
    if(NOT ALL_HEADERS)
        get_target_property(target_sources ${target_name} SOURCES)
        if(target_sources)
            foreach(source_file ${target_sources})
                get_filename_component(file_ext ${source_file} EXT)
                if(file_ext STREQUAL ".h" OR file_ext STREQUAL ".hpp")
                    list(APPEND ALL_HEADERS ${source_file})
                endif()
            endforeach()
        else()
            # Fallback: scan source directory
            file(GLOB_RECURSE ALL_HEADERS 
                "${CMAKE_CURRENT_SOURCE_DIR}/*.h" 
                "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
        endif()
    endif()
    
    # Process headers that need reflection
    set(REFLECTION_HEADERS "")
    set(GENERATED_SOURCES "")
    set(GENERATED_HEADERS "")
    
    foreach(header_file ${ALL_HEADERS})
        # Make path absolute
        if(NOT IS_ABSOLUTE ${header_file})
            set(header_file "${CMAKE_CURRENT_SOURCE_DIR}/${header_file}")
        endif()
        
        # Check if file exists and contains JzRE_CLASS macro
        if(EXISTS ${header_file})
            file(READ ${header_file} file_content)
            string(FIND "${file_content}" "JzRE_CLASS" has_reflection)
            if(NOT has_reflection EQUAL -1)
                list(APPEND REFLECTION_HEADERS ${header_file})
                
                # Generate output file names
                get_filename_component(file_name ${header_file} NAME_WE)
                set(generated_header "${GENERATED_DIR}/${file_name}.generated.h")
                set(generated_source "${GENERATED_DIR}/${file_name}.generated.cpp")
                list(APPEND GENERATED_SOURCES ${generated_source})
                list(APPEND GENERATED_HEADERS ${generated_header})
                
                # Add custom command to generate reflection code
                add_custom_command(
                    OUTPUT ${generated_header} ${generated_source}
                    COMMAND $<TARGET_FILE:JzREHeaderTool>
                        -source-root=${CMAKE_CURRENT_SOURCE_DIR}
                        -output=${GENERATED_DIR}
                        -verbose
                        ${header_file}
                    DEPENDS JzREHeaderTool ${header_file}
                    COMMENT "Generating reflection code for ${file_name}.h"
                    VERBATIM
                )
                
                # Mark generated files as generated
                set_source_files_properties(${generated_header} ${generated_source}
                    PROPERTIES GENERATED TRUE)
            endif()
        endif()
    endforeach()
    
    # Add generated sources to target if any
    if(GENERATED_SOURCES)
        target_sources(${target_name} PRIVATE ${GENERATED_SOURCES})
        target_include_directories(${target_name} PRIVATE ${GENERATED_DIR})
        
        # Create a custom target for all generated files
        set(reflection_target "${target_name}_reflection_generated")
        add_custom_target(${reflection_target}
            DEPENDS ${GENERATED_SOURCES} ${GENERATED_HEADERS}
            COMMENT "Generating all reflection files for ${target_name}"
        )
        add_dependencies(${target_name} ${reflection_target})
        
        # Set a property to indicate this target has reflection
        set_target_properties(${target_name} PROPERTIES JZRE_HAS_REFLECTION TRUE)
        set_target_properties(${target_name} PROPERTIES JZRE_GENERATED_DIR ${GENERATED_DIR})
        
        message(STATUS "JzRE Reflection: Generated ${list_length} reflection files for target ${target_name}")
        foreach(header ${REFLECTION_HEADERS})
            get_filename_component(header_name ${header} NAME)
            message(STATUS "  - ${header_name}")
        endforeach()
    else()
        message(STATUS "JzRE Reflection: No reflection files needed for target ${target_name}")
    endif()
endfunction()

# Function to add reflection support to an existing target
# Usage: jzre_add_reflection_to_target(target_name)
function(jzre_add_reflection_to_target target_name)
    jzre_generate_reflection(${target_name})
endfunction()

# Macro to automatically detect and generate reflection for all targets in current directory
macro(jzre_auto_generate_reflection)
    # Get all targets in current directory
    get_directory_property(targets BUILDSYSTEM_TARGETS)
    foreach(target ${targets})
        get_target_property(target_type ${target} TYPE)
        # Only process executable and library targets
        if(target_type STREQUAL "EXECUTABLE" OR 
           target_type STREQUAL "STATIC_LIBRARY" OR 
           target_type STREQUAL "SHARED_LIBRARY")
            jzre_add_reflection_to_target(${target})
        endif()
    endforeach()
endmacro()
