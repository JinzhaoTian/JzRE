/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JhtType.h"
#include "Utils/JhtUtils.h"

JhtType::JhtType(const CXCursor &cursor, const std::vector<std::string> &currentNamespace) :
    m_rootCursor(cursor),
    m_namespace(currentNamespace)
{
    // 提取注解属性
    auto children = getCursorChildren(cursor);
    for (auto &child : children) {
        if (clang_getCursorKind(child) != CXCursor_AnnotateAttr)
            continue;

        auto properties = extractProperties(child);
        for (auto &prop : properties)
            m_properties[prop.first] = prop.second;
    }

    m_enabled = getFlag(NativeProperty::Enable);
}

bool JhtType::getFlag(const std::string &key) const
{
    return m_properties.find(key) != m_properties.end();
}

std::string JhtType::getCursorSpelling(const CXCursor &cursor) const
{
    return clang_getCString(clang_getCursorSpelling(cursor));
}

std::string JhtType::getProperty(const std::string &key) const
{
    auto search = m_properties.find(key);
    return search == m_properties.end() ? "" : search->second;
}

std::vector<CXCursor> JhtType::getCursorChildren(const CXCursor &cursor) const
{
    std::vector<CXCursor> children;

    auto visitor = [](CXCursor cursor, CXCursor parent, CXClientData data) {
        auto container = static_cast<std::vector<CXCursor> *>(data);
        container->emplace_back(cursor);

        if (clang_getCursorKind(cursor) == CXCursor_LastPreprocessing)
            return CXChildVisit_Break;

        return CXChildVisit_Continue;
    };

    clang_visitChildren(cursor, visitor, &children);
    return children;
}

std::string JhtType::getCursorDisplayName(const CXCursor &cursor) const
{
    return clang_getCString(clang_getCursorDisplayName(cursor));
}

CXType JhtType::getCursorType(const CXCursor &cursor) const
{
    return clang_getCursorType(cursor);
}

std::vector<std::pair<std::string, std::string>> JhtType::extractProperties(const CXCursor &cursor) const
{
    std::vector<std::pair<std::string, std::string>> ret_list;
    auto                                             propertyList = getCursorDisplayName(cursor);
    auto                                             properties   = JhtUtils::Split(propertyList, ",");

    static const std::string white_space_string = " \t\r\n";

    for (auto &property_item : properties) {
        auto item_details = JhtUtils::Split(property_item, ":");
        auto temp_string  = JhtUtils::Trim(item_details[0], white_space_string);
        if (temp_string.empty()) {
            continue;
        }
        ret_list.emplace_back(temp_string,
                              item_details.size() > 1 ? JhtUtils::Trim(item_details[1], white_space_string) : "");
    }
    return ret_list;
}

std::string JhtType::getSourceFile() const
{
    // 使用JhtMetaParser中的工具函数
    // 这里需要访问JhtMetaParser的静态方法或通过其他方式
    return ""; // 实际实现需要调整
}

std::vector<std::string> JhtType::getCurrentNamespace() const
{
    return m_namespace;
}