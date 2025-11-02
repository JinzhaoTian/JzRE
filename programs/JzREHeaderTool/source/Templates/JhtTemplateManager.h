/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <string>
#include <unordered_map>

#include <mustache.hpp>

class JhtTemplateManager {
public:
    /**
     * @brief Get the Instance object
     *
     * @return JhtTemplateManager*
     */
    static JhtTemplateManager *getInstance();

    /**
     * @brief Load a Template
     *
     * @param path
     * @param name
     */
    void loadTemplate(std::string path, std::string name);

    /**
     * @brief Render by Template
     *
     * @param name
     * @param data
     *
     * @return std::string
     */
    std::string renderByTemplate(std::string name, kainjow::mustache::data &data);

private:
    JhtTemplateManager()                                      = default;
    ~JhtTemplateManager()                                     = default;
    JhtTemplateManager(const JhtTemplateManager &)            = delete;
    JhtTemplateManager &operator=(const JhtTemplateManager &) = delete;

private:
    std::unordered_map<std::string, std::string> m_templates;
};
