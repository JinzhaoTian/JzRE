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
    bool        parseProject();
    void        buildClassAST(const Cursor &cursor, std::vector<std::string> &currentNamespace);
    std::string getIncludeFile(std::string name);

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

    std::vector<const char *>    arguments = {{"-x",
                                               "c++",
                                               "-std=c++20",
                                               "-D__JzRE_HEADER_TOOL__",
                                               "-DNDEBUG",
                                               "-D__clang__",
                                               "-w",
                                               "-MG",
                                               "-M",
                                               "-ferror-limit=0",
                                               "-o clangLog.txt"}};
    std::vector<JhtIGenerator *> m_generators;

    bool m_is_show_errors;
};
