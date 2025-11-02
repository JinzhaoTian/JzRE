/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JhtClass.h"
#include "meta/meta_data_config.h"
#include "meta/meta_utils.h"

JhtBaseClass::JhtBaseClass(const Cursor &cursor) :
    name(Utils::getTypeNameWithoutNamespace(cursor.getType())) { }

JhtClass::JhtClass(const Cursor &cursor, const std::vector<std::string> &current_namespace) :
    JhtType(cursor, current_namespace),
    m_name(cursor.getDisplayName()),
    m_qualifiedName(Utils::getTypeNameWithoutNamespace(cursor.getType())),
    m_displayName(Utils::getNameWithoutFirstM(m_qualifiedName))
{
    Utils::replaceAll(m_name, " ", "");
    Utils::replaceAll(m_name, "Piccolo::", "");

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