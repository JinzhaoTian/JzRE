/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <functional>
#include <string>

#include <mustache.hpp>

#include "Types/JhtClass.h"

/**
 * @brief Schema Module
 */
struct SchemaModule {
    std::string                            name;
    std::vector<std::shared_ptr<JhtClass>> classes;
};

/**
 * @brief Interface for Generators
 */
class JhtIGenerator {
public:
    /**
     * @brief Constructor
     *
     * @param outPath
     * @param rootPath
     * @param getIncludeFunc
     */
    JhtIGenerator(std::string outPath, std::string rootPath, std::function<std::string(std::string)> getIncludeFunc) :
        m_outPath(outPath),
        m_rootPath(rootPath),
        m_getIncludeFunc(getIncludeFunc) { }

    /**
     * @brief Destructor
     *
     */
    virtual ~JhtIGenerator() { }

    /**
     * @brief Generate files
     *
     * @param path
     * @param schema
     *
     * @return int
     */
    virtual int generate(std::string path, SchemaModule schema) = 0;

    /**
     * @brief Finish
     */
    virtual void finish() { }

protected:
    virtual void        prepareStatus(std::string path);
    virtual void        genClassRenderData(std::shared_ptr<JhtClass> class_temp, kainjow::mustache::data &class_def);
    virtual void        genClassFieldRenderData(std::shared_ptr<JhtClass> class_temp, kainjow::mustache::data &feild_defs);
    virtual void        genClassMethodRenderData(std::shared_ptr<JhtClass> class_temp, kainjow::mustache::data &method_defs);
    virtual std::string processFileName(std::string path) = 0;

protected:
    std::string                             m_outPath;
    std::string                             m_rootPath;
    std::function<std::string(std::string)> m_getIncludeFunc;
};
