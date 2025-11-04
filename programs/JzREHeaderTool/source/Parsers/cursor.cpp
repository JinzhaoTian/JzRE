#include "cursor.h"

Cursor::Cursor(const CXCursor &handle) :
    m_handle(handle) { }

CXCursorKind Cursor::getKind(void) const
{
    return m_handle.kind;
}

std::string Cursor::getSpelling(void) const
{
    return clang_getCString(clang_getCursorSpelling(m_handle));
}

std::string Cursor::getDisplayName(void) const
{
    return clang_getCString(clang_getCursorDisplayName(m_handle));
}

std::string Cursor::getSourceFile(void) const
{
    auto range = clang_Cursor_getSpellingNameRange(m_handle, 0, 0);

    auto start = clang_getRangeStart(range);

    CXFile   file;
    unsigned line, column, offset;

    clang_getFileLocation(start, &file, &line, &column, &offset);

    return clang_getCString(clang_getFileName(file));
}

bool Cursor::isDefinition(void) const
{
    return clang_isCursorDefinition(m_handle);
}

CursorType Cursor::getType(void) const
{
    return clang_getCursorType(m_handle);
}

std::vector<Cursor> Cursor::getChildren(void) const
{
    std::vector<Cursor> children;

    auto visitor = [](CXCursor cursor, CXCursor parent, CXClientData data) {
        auto container = static_cast<std::vector<Cursor> *>(data);

        container->emplace_back(cursor);

        if (cursor.kind == CXCursor_LastPreprocessing)
            return CXChildVisit_Break;

        return CXChildVisit_Continue;
    };

    clang_visitChildren(m_handle, visitor, &children);

    return children;
}

void Cursor::visitChildren(CXCursorVisitor visitor, void *data)
{
    clang_visitChildren(m_handle, visitor, data);
}
