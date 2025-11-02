#pragma once

#include <vector>

#include "cursor_type.h"

class Cursor {
public:
    Cursor(const CXCursor &handle);

    CXCursorKind getKind(void) const;

    std::string getSpelling(void) const;
    std::string getDisplayName(void) const;

    std::string getSourceFile(void) const;

    bool isDefinition(void) const;

    CursorType getType(void) const;

    std::vector<Cursor> getChildren(void) const;
    void                visitChildren(CXCursorVisitor visitor, void *data = nullptr);

private:
    CXCursor m_handle;
};