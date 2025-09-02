# 反射工具相关的CMake配置

# 查找Python解释器
find_package(Python3 COMPONENTS Interpreter REQUIRED)

# 设置反射工具路径和输出目录
set(REFLECTION_TOOL_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/reflection_tool.py")
set(REFLECTION_OUTPUT_DIR "${CMAKE_BINARY_DIR}/generated")

# 创建输出目录
file(MAKE_DIRECTORY ${REFLECTION_OUTPUT_DIR})

# 检查Python工具的可用性
message(STATUS "检查反射工具依赖...")

# 检查libclang是否可用
execute_process(
    COMMAND ${Python3_EXECUTABLE} -c "import clang.cindex; print('libclang available')"
    RESULT_VARIABLE LIBCLANG_CHECK_RESULT
    OUTPUT_QUIET
    ERROR_QUIET
)

if(LIBCLANG_CHECK_RESULT EQUAL 0)
    message(STATUS "libclang可用，将使用完整AST解析")
    set(REFLECTION_USE_AST TRUE)
else()
    message(STATUS "libclang不可用，将回退到正则表达式解析")
    message(STATUS "要获得最佳性能，请安装: pip install libclang")
    set(REFLECTION_USE_AST FALSE)
endif()

# 函数：检查文件是否包含反射宏
function(check_reflection_macros FILE_PATH RESULT_VAR)
    file(READ ${FILE_PATH} FILE_CONTENT)
    # 检查是否包含任何反射宏，但排除宏定义本身
    set(HAS_MACROS FALSE)
    if(FILE_CONTENT MATCHES "JZCLASS[ \t]*\\(" AND NOT FILE_CONTENT MATCHES "#define[ \t]+JZCLASS")
        set(HAS_MACROS TRUE)
    elseif(FILE_CONTENT MATCHES "JZPROPERTY[ \t]*\\(" AND NOT FILE_CONTENT MATCHES "#define[ \t]+JZPROPERTY")
        set(HAS_MACROS TRUE)
    elseif(FILE_CONTENT MATCHES "JZMETHOD[ \t]*\\(" AND NOT FILE_CONTENT MATCHES "#define[ \t]+JZMETHOD")
        set(HAS_MACROS TRUE)
    elseif(FILE_CONTENT MATCHES "JZREFLECTION_FILE[ \t]*\\(\\)" AND NOT FILE_CONTENT MATCHES "#define[ \t]+JZREFLECTION_FILE")
        set(HAS_MACROS TRUE)
    endif()
    set(${RESULT_VAR} ${HAS_MACROS} PARENT_SCOPE)
endfunction()

# 收集所有需要处理的头文件
file(GLOB_RECURSE ALL_HEADER_FILES "${PROJECT_SOURCE_DIR}/src/*.h")

# 过滤掉已生成的文件和不包含反射宏的文件
set(REFLECTION_HEADER_FILES)
set(GENERATED_REFLECTION_FILES)

foreach(HEADER_FILE ${ALL_HEADER_FILES})
    # 跳过生成的文件
    if(HEADER_FILE MATCHES ".*\\.generated\\.(h|cpp)$")
        continue()
    endif()
    
    # 检查是否包含反射宏
    check_reflection_macros(${HEADER_FILE} HAS_REFLECTION_MACROS)
    
    if(HAS_REFLECTION_MACROS)
        list(APPEND REFLECTION_HEADER_FILES ${HEADER_FILE})
        
        # 计算生成文件的路径
        get_filename_component(HEADER_NAME_WE ${HEADER_FILE} NAME_WE)
        set(GENERATED_HEADER "${REFLECTION_OUTPUT_DIR}/${HEADER_NAME_WE}.generated.h")
        set(GENERATED_SOURCE "${REFLECTION_OUTPUT_DIR}/${HEADER_NAME_WE}.generated.cpp")
        
        list(APPEND GENERATED_REFLECTION_FILES ${GENERATED_HEADER} ${GENERATED_SOURCE})
        
        # 为每个头文件创建单独的生成规则
        add_custom_command(
            OUTPUT ${GENERATED_HEADER} ${GENERATED_SOURCE}
            COMMAND ${Python3_EXECUTABLE} ${REFLECTION_TOOL_SCRIPT}
                --source-dir "${PROJECT_SOURCE_DIR}/src"
                --output-dir "${REFLECTION_OUTPUT_DIR}"
                --single-file "${HEADER_FILE}"
                $<$<BOOL:${CMAKE_VERBOSE_MAKEFILE}>:--verbose>
            DEPENDS ${HEADER_FILE} ${REFLECTION_TOOL_SCRIPT}
            COMMENT "生成 ${HEADER_NAME_WE} 的反射代码"
            VERBATIM
        )
        
        message(STATUS "发现反射文件: ${HEADER_FILE}")
    endif()
endforeach()

list(LENGTH REFLECTION_HEADER_FILES REFLECTION_FILE_COUNT)
message(STATUS "找到 ${REFLECTION_FILE_COUNT} 个反射头文件")

# 创建主要的反射代码生成目标
add_custom_target(
    generate_reflection_code_target
    DEPENDS ${GENERATED_REFLECTION_FILES}
    COMMENT "生成所有C++反射代码"
)

# 添加一个目标来强制重新生成所有反射代码
add_custom_target(
    regenerate_reflection_code
    COMMAND ${CMAKE_COMMAND} -E remove ${GENERATED_REFLECTION_FILES}
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target generate_reflection_code_target
    COMMENT "强制重新生成所有反射代码"
    VERBATIM
)

# 设置生成文件的属性
set_source_files_properties(
    ${GENERATED_REFLECTION_FILES}
    PROPERTIES GENERATED TRUE
)

# 创建一个检查脚本来验证生成的文件
set(VERIFICATION_SCRIPT "${CMAKE_BINARY_DIR}/verify_reflection.cmake")
file(WRITE ${VERIFICATION_SCRIPT} "
foreach(file ${GENERATED_REFLECTION_FILES})
    if(NOT EXISTS \"\${file}\")
        message(FATAL_ERROR \"反射文件未生成: \${file}\")
    endif()
endforeach()
message(STATUS \"所有反射文件验证通过\")
")

# 添加验证目标
add_custom_target(
    verify_reflection_code
    COMMAND ${CMAKE_COMMAND} -P ${VERIFICATION_SCRIPT}
    DEPENDS generate_reflection_code_target
    COMMENT "验证反射代码生成"
)

# 导出变量供父项目使用
set(REFLECTION_GENERATED_FILES ${GENERATED_REFLECTION_FILES} PARENT_SCOPE)
set(REFLECTION_OUTPUT_DIRECTORY ${REFLECTION_OUTPUT_DIR} PARENT_SCOPE)
set(REFLECTION_TARGET generate_reflection_code_target PARENT_SCOPE)
set(REFLECTION_HEADER_FILES ${REFLECTION_HEADER_FILES} PARENT_SCOPE)

# 开发者工具目标
add_custom_target(
    run_reflection_tool
    COMMAND ${Python3_EXECUTABLE} ${REFLECTION_TOOL_SCRIPT}
        --source-dir "${PROJECT_SOURCE_DIR}/src"
        --output-dir "${REFLECTION_OUTPUT_DIR}"
        --verbose
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "手动运行反射工具（用于开发和调试）"
    VERBATIM
)

# 清理生成的反射文件
add_custom_target(
    clean_reflection_code
    COMMAND ${CMAKE_COMMAND} -E remove ${GENERATED_REFLECTION_FILES}
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${REFLECTION_OUTPUT_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${REFLECTION_OUTPUT_DIR}
    COMMENT "清理生成的反射代码"
    VERBATIM
)
