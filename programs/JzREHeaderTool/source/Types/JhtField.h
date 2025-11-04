/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JhtType.h"

class JhtClass;

/**
 * @brief Language Field Type
 */
class JhtField : public JhtType {
public:
    /**
     * @brief Constructor
     *
     * @param cursor
     * @param currentNamespace
     * @param parent
     */
    JhtField(const CXCursor &cursor, const std::vector<std::string> &currentNamespace, JhtClass *parent = nullptr);

    /**
     * @brief Destructor
     */
    virtual ~JhtField() { }

    /**
     * @brief
     *
     * @return bool
     */
    bool shouldCompile() const;

    /**
     * @brief
     *
     * @return bool
     */
    bool isAccessible(void) const;

public:
    JhtClass   *m_parent;
    bool        m_isConst;
    std::string m_name;
    std::string m_displayName;
    std::string m_type;
    std::string m_default;
};