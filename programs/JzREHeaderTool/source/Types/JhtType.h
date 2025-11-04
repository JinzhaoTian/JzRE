/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "Parsers/meta_info.h"

/**
 * @brief Base class for Language Types
 */
class JhtType {
public:
    /**
     * @brief Constructor
     *
     * @param cursor
     * @param currentNamespace
     */
    JhtType(const Cursor &cursor, const std::vector<std::string> &currentNamespace);

    /**
     * @brief Destructor
     */
    virtual ~JhtType() { }

    /**
     * @brief Get the Meta Info
     *
     * @return const MetaInfo&
     */
    const MetaInfo &getMetaData() const;

    /**
     * @brief Get the Source File object
     *
     * @return std::string
     */
    std::string getSourceFile() const;

    /**
     * @brief Get the Current Namespace object
     *
     * @return std::vector<std::string>
     */
    std::vector<std::string> getCurrentNamespace() const;

protected:
    MetaInfo                 m_metaData;
    std::vector<std::string> m_namespace;
    bool                     m_enabled;

private:
    Cursor m_rootCursor;
};