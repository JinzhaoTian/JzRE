set(JzRE_HEADERTOOL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/JzRE")

set(JzRE_HEADERTOOL_PARAMS_PATH "${JzRE_HEADERTOOL_PATH}/precompile.json")
configure_file("${CMAKE_SOURCE_DIR}/scripts/precompile/precompile.json.in" 
                ${JzRE_HEADERTOOL_PARAMS_PATH})

if (WIN32)
	set(JzRE_HEADERTOOL ${JzRE_HEADERTOOL_PATH}/JzREHeaderTool.exe)
    set(SYS_INCLUDE "*") 
elseif (APPLE)
    find_program(XCRUN_EXECUTABLE xcrun)
    if(NOT XCRUN_EXECUTABLE)
      message(FATAL_ERROR "xcrun not found!!!")
    endif()

    execute_process(
      COMMAND ${XCRUN_EXECUTABLE} --sdk macosx --show-sdk-platform-path
      OUTPUT_VARIABLE osx_sdk_platform_path_test
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

	set(JzRE_HEADERTOOL ${JzRE_HEADERTOOL_PATH}/JzREHeaderTool)
    set(SYS_INCLUDE "${osx_sdk_platform_path_test}/../../Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1") 
elseif (LINUX)
	set(JzRE_HEADERTOOL ${JzRE_HEADERTOOL_PATH}/JzREHeaderTool)
    set(SYS_INCLUDE "/usr/include/c++/9/") 
endif()

set(JzRE_PARSER_INPUT ${CMAKE_BINARY_DIR}/parser_header.h)

set(JzRE_PRECOMPILE_TARGET "JzRE_PreCompile")

# Called first time when building target 
add_custom_target(${JzRE_PRECOMPILE_TARGET} ALL
    COMMAND
        ${CMAKE_COMMAND} -E echo "**** [Precompile] BEGIN "
    COMMAND
        ${JzRE_HEADERTOOL} "${JzRE_HEADERTOOL_PARAMS_PATH}" "${JzRE_PARSER_INPUT}" "${JzRE_ROOT}" ${SYS_INCLUDE} "JzRE"
    COMMAND
        ${CMAKE_COMMAND} -E echo "**** [Precompile] finished"
)
