/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JhtType.h"

class JhtClass;

/**
 * @brief Language Method Type
 */
class JhtMethod : public JhtType {
public:
    /**
     * @brief Constructor
     *
     * @param cursor
     * @param currentNamespace
     * @param parent
     */
    JhtMethod(const Cursor &cursor, const std::vector<std::string> &currentNamespace, JhtClass *parent = nullptr);

    /**
     * @brief Destructor
     */
    virtual ~JhtMethod() { }

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
    std::string m_name;
};