#pragma once

#include <unordered_map>

#include "cursor/cursor.h"

class MetaInfo {
public:
    MetaInfo(const Cursor &cursor);

    std::string getProperty(const std::string &key) const;

    bool getFlag(const std::string &key) const;

private:
    std::vector<std::pair<std::string, std::string>> extractProperties(const Cursor &cursor) const;

private:
    std::unordered_map<std::string, std::string> m_properties;
};