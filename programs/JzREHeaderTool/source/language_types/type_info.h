#pragma once

#include "cursor/cursor.h"

#include "meta/meta_info.h"

class TypeInfo {
public:
    TypeInfo(const Cursor &cursor, const std::vector<std::string> &current_namespace);
    virtual ~TypeInfo(void) { }

    const MetaInfo &getMetaData(void) const;

    std::string getSourceFile(void) const;

    std::vector<std::string> getCurrentNamespace() const;

    Cursor &getCurosr();

protected:
    MetaInfo m_meta_data;

    bool m_enabled;

    std::string m_alias_cn;

    std::vector<std::string> m_namespace;

private:
    // cursor that represents the root of this language type
    Cursor m_root_cursor;
};