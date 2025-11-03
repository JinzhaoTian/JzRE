/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "Generators/JhtIGenerator.h"

/**
 * @brief Serializer Generator
 */
class JhtSerializerGenerator : public JhtIGenerator {
public:
    /**
     * @brief Delete default constructor
     */
    JhtSerializerGenerator() = delete;

    /**
     * @brief Constructor
     *
     * @param sourceDirectory
     * @param getIncludeFunc
     */
    JhtSerializerGenerator(std::string sourceDirectory, std::function<std::string(std::string)> getIncludeFunc);

    /**
     * @brief Destructor
     */
    virtual ~JhtSerializerGenerator() override;

    /**
     * @brief
     *
     * @param path
     * @param schema
     * @return int
     */
    virtual int generate(std::string path, SchemaModule schema) override;

    /**
     * @brief
     */
    virtual void finish() override;

protected:
    virtual void        prepareStatus(std::string path) override;
    virtual std::string processFileName(std::string path) override;

private:
    kainjow::mustache::data m_classDefines{kainjow::mustache::data::type::list};
    kainjow::mustache::data m_headerFiles{kainjow::mustache::data::type::list};
};
