# JzSourceGroups.cmake
# Helper functions for organizing source files in Visual Studio

# Function to organize sources into Visual Studio filters based on directory structure
# Usage: jzre_setup_source_groups(target_name)
function(jzre_setup_source_groups TARGET_NAME)
    get_target_property(TARGET_SOURCES ${TARGET_NAME} SOURCES)
    get_target_property(TARGET_SOURCE_DIR ${TARGET_NAME} SOURCE_DIR)
    
    if(TARGET_SOURCES)
        # Group sources based on their file path relative to the target's source directory
        source_group(
            TREE "${TARGET_SOURCE_DIR}"
            FILES ${TARGET_SOURCES}
        )
    endif()
endfunction()

# Function to add sources with automatic source grouping
# Usage: jzre_add_sources(target_name
#            HEADERS header1.h header2.h ...
#            SOURCES source1.cpp source2.cpp ...
#            [BASE_DIR base_directory]
#        )
function(jzre_add_sources TARGET_NAME)
    set(options "")
    set(oneValueArgs BASE_DIR)
    set(multiValueArgs HEADERS SOURCES)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    # Default base directory is current source dir
    if(NOT ARG_BASE_DIR)
        set(ARG_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()
    
    # Combine all sources
    set(ALL_FILES ${ARG_HEADERS} ${ARG_SOURCES})
    
    # Add sources to target
    if(ALL_FILES)
        target_sources(${TARGET_NAME} PRIVATE ${ALL_FILES})
        
        # Create source groups based on directory structure
        source_group(
            TREE "${ARG_BASE_DIR}"
            FILES ${ALL_FILES}
        )
    endif()
endfunction()

# Function to collect and group all sources in a directory tree
# Usage: jzre_collect_sources(
#            HEADERS_VAR out_headers
#            SOURCES_VAR out_sources
#            BASE_DIR directory
#            [INCLUDE_EXTENSIONS h hpp inl]
#            [SOURCE_EXTENSIONS cpp c cc cxx]
#        )
function(jzre_collect_sources)
    set(options "")
    set(oneValueArgs HEADERS_VAR SOURCES_VAR BASE_DIR)
    set(multiValueArgs INCLUDE_EXTENSIONS SOURCE_EXTENSIONS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    # Default extensions
    if(NOT ARG_INCLUDE_EXTENSIONS)
        set(ARG_INCLUDE_EXTENSIONS h hpp inl)
    endif()
    if(NOT ARG_SOURCE_EXTENSIONS)
        set(ARG_SOURCE_EXTENSIONS cpp c cc cxx mm)
    endif()
    
    # Build glob patterns for headers
    set(HEADER_PATTERNS "")
    foreach(EXT ${ARG_INCLUDE_EXTENSIONS})
        list(APPEND HEADER_PATTERNS "${ARG_BASE_DIR}/*.${EXT}")
    endforeach()
    
    # Build glob patterns for sources
    set(SOURCE_PATTERNS "")
    foreach(EXT ${ARG_SOURCE_EXTENSIONS})
        list(APPEND SOURCE_PATTERNS "${ARG_BASE_DIR}/*.${EXT}")
    endforeach()
    
    # Collect files
    file(GLOB_RECURSE COLLECTED_HEADERS CONFIGURE_DEPENDS ${HEADER_PATTERNS})
    file(GLOB_RECURSE COLLECTED_SOURCES CONFIGURE_DEPENDS ${SOURCE_PATTERNS})
    
    # Return results
    set(${ARG_HEADERS_VAR} ${COLLECTED_HEADERS} PARENT_SCOPE)
    set(${ARG_SOURCES_VAR} ${COLLECTED_SOURCES} PARENT_SCOPE)
endfunction()

# Macro to set folder for a target in Visual Studio
# Usage: jzre_set_target_folder(target_name folder_path)
macro(jzre_set_target_folder TARGET_NAME FOLDER_PATH)
    set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "${FOLDER_PATH}")
endmacro()
