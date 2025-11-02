/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include <map>
#include <filesystem>

#include "Generators/JhtCodeGenerator.h"
#include "meta/meta_utils.h"
#include "Templates/JhtTemplateManager.h"

JhtCodeGenerator::JhtCodeGenerator(std::string                             sourceDirectory,
                                   std::function<std::string(std::string)> getIncludeFunc) :
    JhtIGenerator(sourceDirectory + "/_generated/reflection", sourceDirectory, getIncludeFunc)
{
    prepareStatus(m_outPath);
}

JhtCodeGenerator::~JhtCodeGenerator() { }

void JhtCodeGenerator::prepareStatus(std::string path)
{
    JhtIGenerator::prepareStatus(path);
    JhtTemplateManager::getInstance()->loadTemplate(m_rootPath, "commonReflectionFile");
    JhtTemplateManager::getInstance()->loadTemplate(m_rootPath, "allReflectionFile");
    return;
}

std::string JhtCodeGenerator::processFileName(std::string path)
{
    auto relativeDir = std::filesystem::path(path).filename().replace_extension("reflection.gen.h").string();
    return m_outPath + "/" + relativeDir;
}

int JhtCodeGenerator::generate(std::string path, SchemaModule schema)
{
    static const std::string vector_prefix = "std::vector<";

    std::string file_path = processFileName(path);

    kainjow::mustache::data mustache_data;
    kainjow::mustache::data include_headfiles(kainjow::mustache::data::type::list);
    kainjow::mustache::data class_defines(kainjow::mustache::data::type::list);

    include_headfiles.push_back(
        kainjow::mustache::data("headfile_name", Utils::makeRelativePath(m_rootPath, path).string()));

    std::map<std::string, bool> class_names;
    // class defs
    for (auto class_temp : schema.classes) {
        if (!class_temp->shouldCompile())
            continue;

        class_names.insert_or_assign(class_temp->getClassName(), false);
        class_names[class_temp->getClassName()] = true;

        std::vector<std::string>                                   field_names;
        std::map<std::string, std::pair<std::string, std::string>> vector_map;

        kainjow::mustache::data class_def;
        kainjow::mustache::data vector_defines(kainjow::mustache::data::type::list);

        genClassRenderData(class_temp, class_def);
        for (auto field : class_temp->m_fields) {
            if (!field->shouldCompile())
                continue;
            field_names.emplace_back(field->m_name);
            bool is_array = field->m_type.find(vector_prefix) == 0;
            if (is_array) {
                std::string array_useful_name = field->m_type;

                Utils::formatQualifiedName(array_useful_name);

                std::string item_type = field->m_type;

                item_type = Utils::getNameWithoutContainer(item_type);

                vector_map[field->m_type] = std::make_pair(array_useful_name, item_type);
            }
        }

        if (vector_map.size() > 0) {
            if (nullptr == class_def.get("vector_exist")) {
                class_def.set("vector_exist", true);
            }
            for (auto vector_item : vector_map) {
                std::string             array_useful_name = vector_item.second.first;
                std::string             item_type         = vector_item.second.second;
                kainjow::mustache::data vector_define;
                vector_define.set("vector_useful_name", array_useful_name);
                vector_define.set("vector_type_name", vector_item.first);
                vector_define.set("vector_element_type_name", item_type);
                vector_defines.push_back(vector_define);
            }
        }
        class_def.set("vector_defines", vector_defines);
        class_defines.push_back(class_def);
    }

    mustache_data.set("class_defines", class_defines);
    mustache_data.set("include_headfiles", include_headfiles);

    std::string tmp = Utils::convertNameToUpperCamelCase(std::filesystem::path(path).stem().string(), "_");
    mustache_data.set("sourefile_name_upper_camel_case", tmp);

    std::string render_string =
        JhtTemplateManager::getInstance()->renderByTemplate("commonReflectionFile", mustache_data);
    Utils::saveFile(render_string, file_path);

    m_sourceFiles.emplace_back(tmp);

    m_headerFiles.emplace_back(Utils::makeRelativePath(m_rootPath, file_path).string());
    return 0;
}

void JhtCodeGenerator::finish()
{
    kainjow::mustache::data mustache_data;
    kainjow::mustache::data include_headfiles = kainjow::mustache::data::type::list;
    kainjow::mustache::data sourefile_names   = kainjow::mustache::data::type::list;

    for (auto &head_file : m_headerFiles) {
        include_headfiles.push_back(kainjow::mustache::data("headfile_name", head_file));
    }
    for (auto &sourefile_name_upper_camel_case : m_sourceFiles) {
        sourefile_names.push_back(kainjow::mustache::data("sourefile_name_upper_camel_case", sourefile_name_upper_camel_case));
    }
    mustache_data.set("include_headfiles", include_headfiles);
    mustache_data.set("sourefile_names", sourefile_names);
    std::string render_string =
        JhtTemplateManager::getInstance()->renderByTemplate("allReflectionFile", mustache_data);
    Utils::saveFile(render_string, m_outPath + "/all_reflection.h");
}
