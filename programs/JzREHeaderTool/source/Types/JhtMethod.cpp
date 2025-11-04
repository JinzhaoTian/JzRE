/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JhtMethod.h"
#include "JhtClass.h"
#include "Parsers/meta_data_config.h"

JhtMethod::JhtMethod(const Cursor &cursor, const std::vector<std::string> &current_namespace, JhtClass *parent) :
    JhtType(cursor, current_namespace),
    m_parent(parent),
    m_name(cursor.getSpelling()) { }

bool JhtMethod::shouldCompile(void) const
{
    return isAccessible();
}

bool JhtMethod::isAccessible(void) const
{
    return ((m_parent->m_metaData.getFlag(NativeProperty::Methods) || m_parent->m_metaData.getFlag(NativeProperty::All)) && !m_metaData.getFlag(NativeProperty::Disable)) || (m_parent->m_metaData.getFlag(NativeProperty::WhiteListMethods) && m_metaData.getFlag(NativeProperty::Enable));
}
