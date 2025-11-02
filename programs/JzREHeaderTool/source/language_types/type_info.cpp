#include "type_info.h"
#include "meta/meta_data_config.h"

TypeInfo::TypeInfo(const Cursor &cursor, const std::vector<std::string> &current_namespace) :
    m_meta_data(cursor),
    m_enabled(m_meta_data.getFlag(NativeProperty::Enable)),
    m_root_cursor(cursor),
    m_namespace(current_namespace) { }

const MetaInfo &TypeInfo::getMetaData(void) const
{
    return m_meta_data;
}

std::string TypeInfo::getSourceFile(void) const
{
    return m_root_cursor.getSourceFile();
}

std::vector<std::string> TypeInfo::getCurrentNamespace() const
{
    return m_namespace;
}

Cursor &TypeInfo::getCurosr()
{
    return m_root_cursor;
}
