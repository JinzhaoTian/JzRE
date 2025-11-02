/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "Generators/JhtIGenerator.h"

/**
 * @brief Code Generator
 */
class JhtCodeGenerator : public JhtIGenerator {
public:
    /**
     * @brief Delete default constructor
     */
    JhtCodeGenerator() = delete;

    /**
     * @brief Constructor
     *
     * @param sourceDirectory
     * @param getIncludeFunc
     */
    JhtCodeGenerator(std::string sourceDirectory, std::function<std::string(std::string)> getIncludeFunc);

    /**
     * @brief Destructor
     */
    virtual ~JhtCodeGenerator() override;

    /**
     * @brief Generate reflection files
     *
     * @param path
     * @param schema
     *
     * @return int
     */
    virtual int generate(std::string path, SchemaModule schema) override;

    /**
     * @brief Finish
     */
    virtual void finish() override;

protected:
    virtual void        prepareStatus(std::string path) override;
    virtual std::string processFileName(std::string path) override;

private:
    std::vector<std::string> m_headerFiles;
    std::vector<std::string> m_sourceFiles;
};
