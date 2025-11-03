/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "Generators/JhtSerializerGenerator.h"
#include "Templates/JhtTemplateManager.h"
#include "Utils/JhtUtils.h"

JhtSerializerGenerator::JhtSerializerGenerator(std::string                             sourceDirectory,
                                               std::function<std::string(std::string)> getIncludeFunc) :
    JhtIGenerator(sourceDirectory + "/_generated/serializer", sourceDirectory, getIncludeFunc)
{
    prepareStatus(m_outPath);
}

JhtSerializerGenerator::~JhtSerializerGenerator() { }

void JhtSerializerGenerator::prepareStatus(std::string path)
{
    JhtIGenerator::prepareStatus(path);
    JhtTemplateManager::getInstance()->loadTemplate(m_rootPath, "allSerializer.h");
    JhtTemplateManager::getInstance()->loadTemplate(m_rootPath, "allSerializer.ipp");
    JhtTemplateManager::getInstance()->loadTemplate(m_rootPath, "commonSerializerGenFile");
    return;
}

std::string JhtSerializerGenerator::processFileName(std::string path)
{
    auto relativeDir = std::filesystem::path(path).filename().replace_extension("serializer.gen.h").string();
    return m_outPath + "/" + relativeDir;
}

int JhtSerializerGenerator::generate(std::string path, SchemaModule schema)
{
    std::string file_path = processFileName(path);

    kainjow::mustache::data muatache_data;
    kainjow::mustache::data include_headfiles(kainjow::mustache::data::type::list);
    kainjow::mustache::data class_defines(kainjow::mustache::data::type::list);

    include_headfiles.push_back(
        kainjow::mustache::data("headfile_name", JhtUtils::MakeRelativePath(m_rootPath, path).string()));
    for (auto class_temp : schema.classes) {
        if (!class_temp->shouldCompileFields())
            continue;

        kainjow::mustache::data class_def;
        genClassRenderData(class_temp, class_def);

        // deal base class
        for (int index = 0; index < class_temp->m_baseClasses.size(); ++index) {
            auto include_file = m_getIncludeFunc(class_temp->m_baseClasses[index]->name);
            if (!include_file.empty()) {
                auto include_file_base = processFileName(include_file);
                if (file_path != include_file_base) {
                    include_headfiles.push_back(kainjow::mustache::data(
                        "headfile_name", JhtUtils::MakeRelativePath(m_rootPath, include_file_base).string()));
                }
            }
        }
        for (auto field : class_temp->m_fields) {
            if (!field->shouldCompile())
                continue;
            // deal vector
            if (field->m_type.find("std::vector") == 0) {
                auto include_file = m_getIncludeFunc(field->m_name);
                if (!include_file.empty()) {
                    auto include_file_base = processFileName(include_file);
                    if (file_path != include_file_base) {
                        include_headfiles.push_back(kainjow::mustache::data(
                            "headfile_name", JhtUtils::MakeRelativePath(m_rootPath, include_file_base).string()));
                    }
                }
            }
            // deal normal
        }
        class_defines.push_back(class_def);
        m_classDefines.push_back(class_def);
    }

    muatache_data.set("class_defines", class_defines);
    muatache_data.set("include_headfiles", include_headfiles);
    std::string render_string = JhtTemplateManager::getInstance()->renderByTemplate("commonSerializerGenFile", muatache_data);
    JhtUtils::SaveFile(render_string, file_path);

    m_headerFiles.push_back(
        kainjow::mustache::data("headfile_name", JhtUtils::MakeRelativePath(m_rootPath, file_path).string()));
    return 0;
}

void JhtSerializerGenerator::finish()
{
    kainjow::mustache::data mustache_data;
    mustache_data.set("class_defines", m_classDefines);
    mustache_data.set("include_headfiles", m_headerFiles);

    std::string render_string = JhtTemplateManager::getInstance()->renderByTemplate("allSerializer.h", mustache_data);
    JhtUtils::SaveFile(render_string, m_outPath + "/all_serializer.h");
    render_string = JhtTemplateManager::getInstance()->renderByTemplate("allSerializer.ipp", mustache_data);
    JhtUtils::SaveFile(render_string, m_outPath + "/all_serializer.ipp");
}
