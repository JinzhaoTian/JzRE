/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JhtMethod.h"
#include "JhtClass.h"

JhtMethod::JhtMethod(const CXCursor &cursor, const std::vector<std::string> &current_namespace, JhtClass *parent) :
    JhtType(cursor, current_namespace),
    m_parent(parent)
{
    m_name = getCursorSpelling(cursor);
}

bool JhtMethod::shouldCompile() const
{
    return isAccessible();
}

bool JhtMethod::isAccessible() const
{
    return ((m_parent->getFlag(NativeProperty::Methods) || m_parent->getFlag(NativeProperty::All)) && !getFlag(NativeProperty::Disable)) || (m_parent->getFlag(NativeProperty::WhiteListMethods) && getFlag(NativeProperty::Enable));
}
