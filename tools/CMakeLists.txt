# 反射工具相关的CMake配置

# 查找Python解释器
find_package(Python3 COMPONENTS Interpreter REQUIRED)

# 设置反射工具路径
set(REFLECTION_TOOL_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/reflection_tool.py")
set(REFLECTION_OUTPUT_DIR "${CMAKE_BINARY_DIR}/generated")

# 创建输出目录
file(MAKE_DIRECTORY ${REFLECTION_OUTPUT_DIR})

# 获取所有需要处理的头文件
file(GLOB_RECURSE REFLECTION_HEADER_FILES 
    "${PROJECT_SOURCE_DIR}/src/*.h"
)

# 过滤掉已生成的文件
list(FILTER REFLECTION_HEADER_FILES EXCLUDE REGEX ".*\\.generated\\.(h|cpp)$")

# 创建自定义目标来运行反射工具
add_custom_target(
    generate_reflection_code_target
    COMMAND ${Python3_EXECUTABLE} ${REFLECTION_TOOL_SCRIPT}
        --source-dir "${PROJECT_SOURCE_DIR}/src"
        --output-dir "${REFLECTION_OUTPUT_DIR}"
        --verbose
    DEPENDS ${REFLECTION_HEADER_FILES}
    COMMENT "生成C++反射代码"
    VERBATIM
)

# 设置生成的文件列表
set(GENERATED_REFLECTION_FILES)
foreach(HEADER_FILE ${REFLECTION_HEADER_FILES})
    # 只检查实际调用JZREFLECTION_FILE()的文件，不包括定义该宏的文件
    file(READ ${HEADER_FILE} HEADER_CONTENT)
    # 检查是否有JZREFLECTION_FILE()调用，但不是宏定义
    if(HEADER_CONTENT MATCHES "JZREFLECTION_FILE\\(\\)" AND NOT HEADER_CONTENT MATCHES "#define[ \t]+JZREFLECTION_FILE")
        get_filename_component(HEADER_NAME_WE ${HEADER_FILE} NAME_WE)
        list(APPEND GENERATED_REFLECTION_FILES 
            "${REFLECTION_OUTPUT_DIR}/${HEADER_NAME_WE}.generated.h"
            "${REFLECTION_OUTPUT_DIR}/${HEADER_NAME_WE}.generated.cpp"
        )
    endif()
endforeach()

# 设置生成文件的属性
set_source_files_properties(
    ${GENERATED_REFLECTION_FILES}
    PROPERTIES GENERATED TRUE
)

# 导出变量供父项目使用
set(REFLECTION_GENERATED_FILES ${GENERATED_REFLECTION_FILES} PARENT_SCOPE)
set(REFLECTION_OUTPUT_DIRECTORY ${REFLECTION_OUTPUT_DIR} PARENT_SCOPE)
set(REFLECTION_TARGET generate_reflection_code_target PARENT_SCOPE)

# 创建一个辅助目标用于运行反射工具
add_custom_target(
    run_reflection_tool
    COMMAND ${Python3_EXECUTABLE} ${REFLECTION_TOOL_SCRIPT}
        --source-dir "${PROJECT_SOURCE_DIR}/src"
        --output-dir "${REFLECTION_OUTPUT_DIR}"
        --verbose
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "手动运行反射工具"
    VERBATIM
)
