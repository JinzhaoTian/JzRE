/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JhtField.h"
#include "JhtClass.h"
#include "meta/meta_data_config.h"
#include "meta/meta_utils.h"

JhtField::JhtField(const Cursor &cursor, const std::vector<std::string> &current_namespace, JhtClass *parent) :
    JhtType(cursor, current_namespace),
    m_isConst(cursor.getType().IsConst()),
    m_parent(parent),
    m_name(cursor.getSpelling()),
    m_displayName(Utils::getNameWithoutFirstM(m_name)),
    m_type(Utils::getTypeNameWithoutNamespace(cursor.getType()))
{
    Utils::replaceAll(m_type, " ", "");
    Utils::replaceAll(m_type, "Piccolo::", "");

    auto ret_string = Utils::getStringWithoutQuot(m_metaData.getProperty("default"));
    m_default       = ret_string;
}

bool JhtField::shouldCompile(void) const
{
    return isAccessible();
}

bool JhtField::isAccessible(void) const
{
    return ((m_parent->m_metaData.getFlag(NativeProperty::Fields) || m_parent->m_metaData.getFlag(NativeProperty::All)) && !m_metaData.getFlag(NativeProperty::Disable)) || (m_parent->m_metaData.getFlag(NativeProperty::WhiteListFields) && m_metaData.getFlag(NativeProperty::Enable));
}
