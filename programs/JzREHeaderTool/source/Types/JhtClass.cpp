/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JhtClass.h"

JhtBaseClass::JhtBaseClass(const CXCursor &cursor)
{
    name = clang_getCString(clang_getTypeSpelling(clang_getCursorType(cursor)));
}

JhtClass::JhtClass(const CXCursor &cursor, const std::vector<std::string> &current_namespace) :
    JhtType(cursor, current_namespace)
{
    m_name = clang_getCString(clang_getCursorDisplayName(cursor));

    m_qualifiedName = clang_getCString(clang_getTypeSpelling(clang_getCursorType(cursor)));

    m_displayName = m_qualifiedName.starts_with("m_") && m_qualifiedName.size() > 2 ? m_qualifiedName.substr(2) : m_qualifiedName;

    m_name.erase(std::remove(m_name.begin(), m_name.end(), ' '), m_name.end());

    constexpr std::string_view prefix = "JzRE::";
    for (size_t pos = 0; (pos = m_name.find(prefix, pos)) != std::string::npos; pos += 0) {
        m_name.erase(pos, prefix.length());
    }

    for (auto &child : getCursorChildren(cursor)) {
        switch (child.kind) {
            case CXCursor_CXXBaseSpecifier:
            {
                auto base_class = new JhtBaseClass(child);

                m_baseClasses.emplace_back(base_class);
            } break;
            // field
            case CXCursor_FieldDecl:
                m_fields.emplace_back(new JhtField(child, current_namespace, this));
                break;
            // method
            case CXCursor_CXXMethod:
                m_methods.emplace_back(new JhtMethod(child, current_namespace, this));
            default:
                break;
        }
    }
}

bool JhtClass::shouldCompile() const
{
    return shouldCompileFields() || shouldCompileMethods();
}

bool JhtClass::shouldCompileFields() const
{
    return getFlag(NativeProperty::All) || getFlag(NativeProperty::Fields) || getFlag(NativeProperty::WhiteListFields);
}

bool JhtClass::shouldCompileMethods() const
{
    return getFlag(NativeProperty::All) || getFlag(NativeProperty::Methods) || getFlag(NativeProperty::WhiteListMethods);
}

std::string JhtClass::getClassName()
{
    return m_name;
}

bool JhtClass::isAccessible() const
{
    return m_enabled;
}