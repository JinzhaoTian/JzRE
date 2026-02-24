# JzShaderCook.cmake
# Helper for creating shader cook targets backed by JzREShaderTool.

function(JzRE_add_shader_cook_target)
    set(options "")
    set(oneValueArgs TARGET_NAME SOURCE_DIR OUTPUT_DIR TOOL_TARGET)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "" ${ARGN})

    if(NOT ARG_TARGET_NAME)
        message(FATAL_ERROR "JzRE_add_shader_cook_target: TARGET_NAME is required")
    endif()
    if(NOT ARG_SOURCE_DIR)
        message(FATAL_ERROR "JzRE_add_shader_cook_target: SOURCE_DIR is required")
    endif()
    if(NOT ARG_OUTPUT_DIR)
        message(FATAL_ERROR "JzRE_add_shader_cook_target: OUTPUT_DIR is required")
    endif()
    if(NOT ARG_TOOL_TARGET)
        message(FATAL_ERROR "JzRE_add_shader_cook_target: TOOL_TARGET is required")
    endif()

    file(GLOB_RECURSE SHADER_SOURCE_MANIFESTS CONFIGURE_DEPENDS
         "${ARG_SOURCE_DIR}/*.jzshader.src.json")

    set(COOK_COMMANDS
        COMMAND ${CMAKE_COMMAND} -E make_directory "${ARG_OUTPUT_DIR}")

    foreach(SHADER_MANIFEST IN LISTS SHADER_SOURCE_MANIFESTS)
        list(APPEND COOK_COMMANDS
             COMMAND $<TARGET_FILE:${ARG_TOOL_TARGET}>
                     --input "${SHADER_MANIFEST}"
                     --output-dir "${ARG_OUTPUT_DIR}")
    endforeach()

    if(SHADER_SOURCE_MANIFESTS)
        add_custom_target(${ARG_TARGET_NAME} ALL
            ${COOK_COMMANDS}
            DEPENDS ${ARG_TOOL_TARGET} ${SHADER_SOURCE_MANIFESTS}
            COMMENT "Cooking shaders from ${ARG_SOURCE_DIR}"
            VERBATIM
        )
    else()
        add_custom_target(${ARG_TARGET_NAME} ALL
            COMMAND ${CMAKE_COMMAND} -E make_directory "${ARG_OUTPUT_DIR}"
            DEPENDS ${ARG_TOOL_TARGET}
            COMMENT "No shader manifests found in ${ARG_SOURCE_DIR}"
            VERBATIM
        )
    endif()
endfunction()
