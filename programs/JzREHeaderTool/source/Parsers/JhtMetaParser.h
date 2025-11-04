/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "Generators/JhtIGenerator.h"

/**
 * @brief Meta Parser
 */
class JhtMetaParser {
public:
    JhtMetaParser(const std::string project_input_file,
                  const std::string include_file_path,
                  const std::string include_path,
                  const std::string include_sys,
                  const std::string module_name);

    /**
     * @brief Destructor
     */
    ~JhtMetaParser();

    /**
     * @brief Parse all files
     *
     * @return int
     */
    int parse();

    /**
     * @brief Generate reflection files
     */
    void generateFiles();

    /**
     * @brief Finish
     */
    void finish();

private:
    bool                                             parseProject();
    void                                             buildClassAST(const CXCursor &cursor, std::vector<std::string> &currentNamespace);
    std::string                                      getIncludeFile(std::string name);
    std::vector<std::pair<std::string, std::string>> extractProperties(const CXCursor &cursor) const;
    std::string                                      getCursorDisplayName(const CXCursor &cursor) const;
    std::string                                      getCursorSpelling(const CXCursor &cursor) const;
    std::string                                      getCursorSourceFile(const CXCursor &cursor) const;
    bool                                             isCursorDefinition(const CXCursor &cursor) const;
    CXType                                           getCursorType(const CXCursor &cursor) const;
    std::vector<CXCursor>                            getCursorChildren(const CXCursor &cursor) const;

private:
    std::string              m_project_input_file;
    std::vector<std::string> m_work_paths;
    std::string              m_module_name;
    std::string              m_sys_include;
    std::string              m_source_include_file_name;

    CXIndex           m_index;
    CXTranslationUnit m_translation_unit;

    std::unordered_map<std::string, std::string>  m_type_table;
    std::unordered_map<std::string, SchemaModule> m_schema_modules;

    std::vector<const char *> m_arguments = {
        {"-x", "c++",              // 指定语言为C++
         "-std=c++20",             // C++20标准
         "-D__JzRE_HEADER_TOOL__", // 定义宏
         "-DNDEBUG",               // 禁用调试
         "-D__clang__",            // 标识为Clang
         "-w",                     // 禁用所有警告
         "-MG", "-M",              // 依赖关系生成
         "-ferror-limit=0",        // 无错误限制
         "-o clangLog.txt"}        // 输出日志文件
    };

    std::vector<JhtIGenerator *> m_generators;

    bool m_is_show_errors;
};
