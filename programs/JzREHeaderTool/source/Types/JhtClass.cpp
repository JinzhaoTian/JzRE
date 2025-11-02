/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JhtClass.h"
#include "meta/meta_data_config.h"

JhtBaseClass::JhtBaseClass(const Cursor &cursor)
{
    name = cursor.getType().GetDisplayName();
}

JhtClass::JhtClass(const Cursor &cursor, const std::vector<std::string> &current_namespace) :
    JhtType(cursor, current_namespace),
    m_name(cursor.getDisplayName()),
    m_qualifiedName(cursor.getType().GetDisplayName())
{
    m_displayName = m_qualifiedName.starts_with("m_") && m_qualifiedName.size() > 2 ? m_qualifiedName.substr(2) : m_qualifiedName;

    m_name.erase(std::remove(m_name.begin(), m_name.end(), ' '), m_name.end());

    constexpr std::string_view prefix = "JzRE::";
    for (size_t pos = 0; (pos = m_name.find(prefix, pos)) != std::string::npos; pos += 0) {
        m_name.erase(pos, prefix.length());
    }

    for (auto &child : cursor.getChildren()) {
        switch (child.getKind()) {
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
    return m_metaData.getFlag(NativeProperty::All) || m_metaData.getFlag(NativeProperty::Fields) || m_metaData.getFlag(NativeProperty::WhiteListFields);
}

bool JhtClass::shouldCompileMethods() const
{
    return m_metaData.getFlag(NativeProperty::All) || m_metaData.getFlag(NativeProperty::Methods) || m_metaData.getFlag(NativeProperty::WhiteListMethods);
}

std::string JhtClass::getClassName()
{
    return m_name;
}

bool JhtClass::isAccessible() const
{
    return m_enabled;
}