/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include <fstream>

#include "Templates/JhtTemplateManager.h"

JhtTemplateManager *JhtTemplateManager::getInstance()
{
    static JhtTemplateManager *m_pInstance;
    if (nullptr == m_pInstance)
        m_pInstance = new JhtTemplateManager();
    return m_pInstance;
}

void JhtTemplateManager::loadTemplate(std::string path, std::string name)
{
    std::string templatePath = path + "/../template/" + name + ".mustache";

    std::ifstream      iFile(templatePath);
    std::string        line_string;
    std::ostringstream template_stream;
    if (false == iFile.is_open()) {
        iFile.close();
        return;
    }
    while (std::getline(iFile, line_string)) {
        template_stream << line_string << std::endl;
    }
    iFile.close();

    m_templates.insert_or_assign(name, template_stream.str());
}

std::string JhtTemplateManager::renderByTemplate(std::string templateName, kainjow::mustache::data &templateData)
{
    if (m_templates.find(templateName) == m_templates.end()) {
        return "";
    }
    kainjow::mustache::mustache tmpl(m_templates[templateName]);
    return tmpl.render(templateData);
}
