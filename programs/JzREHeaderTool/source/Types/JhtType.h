/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "clang-c/Index.h"

namespace NativeProperty {
const auto All = "All";

const auto Fields  = "Fields";
const auto Methods = "Methods";

const auto Enable  = "Enable";
const auto Disable = "Disable";

const auto WhiteListFields  = "WhiteListFields";
const auto WhiteListMethods = "WhiteListMethods";

} // namespace NativeProperty

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
    JhtType(const CXCursor &cursor, const std::vector<std::string> &currentNamespace);

    /**
     * @brief Destructor
     */
    virtual ~JhtType() { }

    /**
     * @brief Get the Meta Info
     *
     * @return const MetaInfo&
     */
    const std::unordered_map<std::string, std::string> &getProperties() const;

    bool getFlag(const std::string &key) const;

    std::string getProperty(const std::string &key) const;

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

    std::string getCursorSpelling(const CXCursor &cursor) const;

    std::vector<CXCursor> getCursorChildren(const CXCursor &cursor) const;

    std::string getCursorDisplayName(const CXCursor &cursor) const;

    CXType getCursorType(const CXCursor &cursor) const;

    std::vector<std::pair<std::string, std::string>> extractProperties(const CXCursor &cursor) const;

protected:
    std::unordered_map<std::string, std::string> m_properties;
    std::vector<std::string>                     m_namespace;
    bool                                         m_enabled;

private:
    CXCursor m_rootCursor;
};