/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JhtType.h"
#include "Parsers/meta_data_config.h"

JhtType::JhtType(const Cursor &cursor, const std::vector<std::string> &currentNamespace) :
    m_metaData(cursor),
    m_rootCursor(cursor),
    m_namespace(currentNamespace),
    m_enabled(m_metaData.getFlag(NativeProperty::Enable)) { }

const MetaInfo &JhtType::getMetaData() const
{
    return m_metaData;
}

std::string JhtType::getSourceFile() const
{
    return m_rootCursor.getSourceFile();
}

std::vector<std::string> JhtType::getCurrentNamespace() const
{
    return m_namespace;
}