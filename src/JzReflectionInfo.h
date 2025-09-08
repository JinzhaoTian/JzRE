#pragma once

#include "CommonTypes.h"

namespace JzRE {

/**
 * @brief JzRE Reflection class property info
 */
struct JzReflectedClassPropertyInfo {
    String                             name;
    String                             type;
    String                             category;
    std::vector<String>                flags;
    std::unordered_map<String, String> metadata;
    Size                               offset;
};

/**
 * @brief JzRE Reflection class method info
 */
struct JzReflectedClassMethodInfo {
    String                                 name;
    String                                 returnType;
    std::vector<std::pair<String, String>> parameters; // type, name
    std::vector<String>                    flags;
    std::unordered_map<String, String>     metadata;
    Bool                                   isConst;
    Bool                                   isStatic;
    Bool                                   isVirtual;
};

/**
 * @brief JzRE Reflection class info
 */
struct JzReflectedClassInfo {
    String                                    name;
    String                                    namespaceName;
    String                                    fullName;
    String                                    headerFile;
    std::vector<String>                       baseClasses;
    std::vector<JzReflectedClassPropertyInfo> properties;
    std::vector<JzReflectedClassMethodInfo>   methods;
    std::unordered_map<String, String>        metadata;
    Size                                      size; // in bytes

    JzReflectedClassInfo() :
        size(0) { }

    const String GetName() const
    {
        return name;
    }

    Size GetSize() const
    {
        return size;
    }
};

} // namespace JzRE