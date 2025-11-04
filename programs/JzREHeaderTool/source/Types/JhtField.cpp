/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include <algorithm>
#include <string>

#include "JhtField.h"
#include "JhtClass.h"

JhtField::JhtField(const CXCursor &cursor, const std::vector<std::string> &current_namespace, JhtClass *parent) :
    JhtType(cursor, current_namespace),
    m_parent(parent)
{
    m_name = getCursorSpelling(cursor);

    m_isConst = clang_isConstQualifiedType(getCursorType(cursor)) ? true : false;

    m_displayName = m_name.starts_with("m_") && m_name.size() > 2 ? m_name.substr(2) : m_name;

    m_type = getCursorDisplayName(cursor);

    m_type.erase(std::remove(m_type.begin(), m_type.end(), ' '), m_type.end());

    constexpr std::string_view prefix = "JzRE::";
    for (size_t pos = 0; (pos = m_type.find(prefix, pos)) != std::string::npos; pos += 0) {
        m_type.erase(pos, prefix.length());
    }

    const auto   default_str = getProperty("default");
    const size_t leftPos     = default_str.find('\"') + 1;
    const size_t rightPos    = default_str.rfind('\"');
    if (leftPos > 0 && rightPos != std::string::npos && leftPos < rightPos) {
        m_default = default_str.substr(leftPos, rightPos - leftPos);
    } else {
        m_default = default_str;
    }
}

bool JhtField::shouldCompile(void) const
{
    return isAccessible();
}

bool JhtField::isAccessible(void) const
{
    return ((m_parent->getFlag(NativeProperty::Fields) || m_parent->getFlag(NativeProperty::All)) && !getFlag(NativeProperty::Disable)) || (m_parent->getFlag(NativeProperty::WhiteListFields) && getFlag(NativeProperty::Enable));
}
