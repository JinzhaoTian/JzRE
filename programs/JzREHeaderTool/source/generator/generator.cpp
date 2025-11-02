/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "generator/generator.h"
#include <filesystem>

#include "Types/JhtClass.h"

namespace Generator {
void GeneratorInterface::prepareStatus(std::string path)
{
    if (!std::filesystem::exists(path)) {
        std::filesystem::create_directories(path);
    }
}
void GeneratorInterface::genClassRenderData(std::shared_ptr<JhtClass> class_temp, kainjow::mustache::data &class_def)
{
    class_def.set("class_name", class_temp->getClassName());
    class_def.set("class_base_class_size", std::to_string(class_temp->m_baseClasses.size()));
    class_def.set("class_need_register", true);

    if (class_temp->m_baseClasses.size() > 0) {
        kainjow::mustache::data class_base_class_defines(kainjow::mustache::data::type::list);
        class_def.set("class_has_base", true);
        for (int index = 0; index < class_temp->m_baseClasses.size(); ++index) {
            kainjow::mustache::data class_base_class_def;
            class_base_class_def.set("class_base_class_name", class_temp->m_baseClasses[index]->name);
            class_base_class_def.set("class_base_class_index", std::to_string(index));
            class_base_class_defines.push_back(class_base_class_def);
        }
        class_def.set("class_base_class_defines", class_base_class_defines);
    }

    kainjow::mustache::data class_field_defines = kainjow::mustache::data::type::list;
    genClassFieldRenderData(class_temp, class_field_defines);
    class_def.set("class_field_defines", class_field_defines);

    kainjow::mustache::data class_method_defines = kainjow::mustache::data::type::list;
    genClassMethodRenderData(class_temp, class_method_defines);
    class_def.set("class_method_defines", class_method_defines);
}
void GeneratorInterface::genClassFieldRenderData(std::shared_ptr<JhtClass> class_temp, kainjow::mustache::data &feild_defs)
{
    static const std::string vector_prefix = "std::vector<";

    for (auto &field : class_temp->m_fields) {
        if (!field->shouldCompile())
            continue;
        kainjow::mustache::data filed_define;

        filed_define.set("class_field_name", field->m_name);
        filed_define.set("class_field_type", field->m_type);
        filed_define.set("class_field_display_name", field->m_displayName);
        bool is_vector = field->m_type.find(vector_prefix) == 0;
        filed_define.set("class_field_is_vector", is_vector);
        feild_defs.push_back(filed_define);
    }
}

void GeneratorInterface::genClassMethodRenderData(std::shared_ptr<JhtClass> class_temp, kainjow::mustache::data &method_defs)
{
    for (auto &method : class_temp->m_methods) {
        if (!method->shouldCompile())
            continue;
        kainjow::mustache::data method_define;

        method_define.set("class_method_name", method->m_name);
        method_defs.push_back(method_define);
    }
}
} // namespace Generator
