/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JhtType.h"
#include "JhtField.h"
#include "JhtMethod.h"

/**
 * @brief Language Class's Base Class Type
 */
struct JhtBaseClass {
    JhtBaseClass(const Cursor &cursor);

    std::string name;
};

/**
 * @brief Language Class Type
 */
class JhtClass : public JhtType {
    friend class JhtField;
    friend class JhtMethod;
    friend class MetaParser;

public:
    /**
     * @brief Constructor
     *
     * @param cursor
     * @param currentNamespace
     */
    JhtClass(const Cursor &cursor, const std::vector<std::string> &currentNamespace);

    /**
     * @brief
     *
     * @return bool
     */
    virtual bool shouldCompile() const;

    /**
     * @brief
     *
     * @return bool
     */
    bool shouldCompileFields() const;

    /**
     * @brief
     *
     * @return bool
     */
    bool shouldCompileMethods() const;

    /**
     * @brief Get the Class Name object
     *
     * @return std::string
     */
    std::string getClassName();

    /**
     * @brief
     *
     * @return bool
     */
    bool isAccessible() const;

public:
    std::string                                m_name;
    std::string                                m_qualifiedName;
    std::string                                m_displayName;
    std::vector<std::shared_ptr<JhtBaseClass>> m_baseClasses;
    std::vector<std::shared_ptr<JhtField>>     m_fields;
    std::vector<std::shared_ptr<JhtMethod>>    m_methods;
};
