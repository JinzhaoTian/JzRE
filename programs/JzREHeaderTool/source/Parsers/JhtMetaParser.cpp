/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include <filesystem>
#include <fstream>

#include "Parsers/JhtMetaParser.h"
#include "Generators/JhtCodeGenerator.h"
#include "Generators/JhtSerializerGenerator.h"
#include "Types/JhtClass.h"
#include "Utils/JhtUtils.h"

JhtMetaParser::JhtMetaParser(const std::string project_input_file,
                             const std::string include_file_path,
                             const std::string include_path,
                             const std::string sys_include,
                             const std::string module_name) :
    m_project_input_file(project_input_file),
    m_source_include_file_name(include_file_path),
    m_index(nullptr),
    m_translation_unit(nullptr),
    m_sys_include(sys_include),
    m_module_name(module_name),
    m_is_show_errors(false)
{
    m_work_paths = JhtUtils::Split(include_path, ";");

    m_generators.emplace_back(new JhtSerializerGenerator(
        m_work_paths[0], std::bind(&JhtMetaParser::getIncludeFile, this, std::placeholders::_1)));
    m_generators.emplace_back(new JhtCodeGenerator(
        m_work_paths[0], std::bind(&JhtMetaParser::getIncludeFile, this, std::placeholders::_1)));
}

JhtMetaParser::~JhtMetaParser()
{
    for (auto item : m_generators) {
        delete item;
    }
    m_generators.clear();

    if (m_translation_unit)
        clang_disposeTranslationUnit(m_translation_unit);

    if (m_index)
        clang_disposeIndex(m_index);
}

bool JhtMetaParser::parseProject()
{
    bool result = true;
    std::cout << "Parsing project file: " << m_project_input_file << std::endl;

    std::fstream include_txt_file(m_project_input_file, std::ios::in);

    if (include_txt_file.fail()) {
        std::cout << "Could not load file: " << m_project_input_file << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << include_txt_file.rdbuf();

    std::string context = buffer.str();

    auto         inlcude_files = JhtUtils::Split(context, ";");
    std::fstream include_file;

    include_file.open(m_source_include_file_name, std::ios::out);
    if (!include_file.is_open()) {
        std::cout << "Could not open the Source Include file: " << m_source_include_file_name << std::endl;
        return false;
    }

    std::cout << "Generating the Source Include file: " << m_source_include_file_name << std::endl;

    std::string output_filename = JhtUtils::GetFileName(m_source_include_file_name);

    if (output_filename.empty()) {
        output_filename = "META_INPUT_HEADER_H";
    } else {
        JhtUtils::Replace(output_filename, ".", "_");
        JhtUtils::Replace(output_filename, " ", "_");
        JhtUtils::ToUpper(output_filename);
    }
    include_file << "#ifndef __" << output_filename << "__" << std::endl;
    include_file << "#define __" << output_filename << "__" << std::endl;

    for (auto include_item : inlcude_files) {
        std::string temp_string(include_item);
        JhtUtils::Replace(temp_string, '\\', '/');
        include_file << "#include  \"" << temp_string << "\"" << std::endl;
    }

    include_file << "#endif" << std::endl;
    include_file.close();
    return result;
}

int JhtMetaParser::parse()
{
    bool parse_include_ = parseProject();
    if (!parse_include_) {
        std::cerr << "Parsing project file error! " << std::endl;
        return -1;
    }

    std::cerr << "Parsing the whole project..." << std::endl;
    int is_show_errors      = m_is_show_errors ? 1 : 0;
    m_index                 = clang_createIndex(true, is_show_errors);
    std::string pre_include = "-I";
    std::string sys_include_temp;
    if (!(m_sys_include == "*")) {
        sys_include_temp = pre_include + m_sys_include;
        m_arguments.emplace_back(sys_include_temp.c_str());
    }

    auto paths = m_work_paths;
    for (int index = 0; index < paths.size(); ++index) {
        paths[index] = pre_include + paths[index];

        m_arguments.emplace_back(paths[index].c_str());
    }

    std::filesystem::path input_path(m_source_include_file_name);
    if (!std::filesystem::exists(input_path)) {
        std::cerr << input_path << " is not exist" << std::endl;
        return -2;
    }

    m_translation_unit = clang_createTranslationUnitFromSourceFile(m_index, m_source_include_file_name.c_str(), static_cast<int>(m_arguments.size()), m_arguments.data(), 0, nullptr);
    auto cursor        = clang_getTranslationUnitCursor(m_translation_unit);

    std::vector<std::string> temp_namespace;

    buildClassAST(cursor, temp_namespace);

    temp_namespace.clear();

    return 0;
}

void JhtMetaParser::generateFiles()
{
    std::cerr << "Start generate runtime schemas(" << m_schema_modules.size() << ")..." << std::endl;
    for (auto &schema : m_schema_modules) {
        for (auto &generator_iter : m_generators) {
            generator_iter->generate(schema.first, schema.second);
        }
    }
}

void JhtMetaParser::finish()
{
    for (auto generator_iter : m_generators) {
        generator_iter->finish();
    }
}

void JhtMetaParser::buildClassAST(const Cursor &cursor, std::vector<std::string> &currentNamespace)
{
    for (auto &child : cursor.getChildren()) {
        auto kind = child.getKind();

        // actual definition and a class or struct
        if (child.isDefinition() && (kind == CXCursor_ClassDecl || kind == CXCursor_StructDecl)) {
            auto class_ptr = std::make_shared<JhtClass>(child, currentNamespace);

            if (class_ptr->shouldCompile()) {
                auto file = class_ptr->getSourceFile();
                m_schema_modules[file].classes.emplace_back(class_ptr);
                m_type_table[class_ptr->m_displayName] = file;
            }
        } else {
            if (kind == CXCursor_Namespace) {
                auto display_name = cursor.getDisplayName();
                if (!display_name.empty()) {
                    currentNamespace.emplace_back(display_name);
                    buildClassAST(cursor, currentNamespace);
                    currentNamespace.pop_back();
                }
            }
        }
    }
}

std::string JhtMetaParser::getIncludeFile(std::string name)
{
    auto iter = m_type_table.find(name);
    return iter == m_type_table.end() ? std::string() : iter->second;
}
