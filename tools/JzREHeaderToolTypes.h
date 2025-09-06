#pragma once

#include <string>
#include <vector>
#include <unordered_map>

/**
 * @brief JzRE Tool reflected class property info
 */
struct JzREHeaderToolReflectedClassPropertyInfo {
    std::string                                  name;
    std::string                                  type;
    std::string                                  category;
    std::vector<std::string>                     flags;
    std::unordered_map<std::string, std::string> metadata;
    size_t                                       offset;

    JzREHeaderToolReflectedClassPropertyInfo() :
        offset(0) { }
};

/**
 * @brief JzRE Tool reflected class method info
 */
struct JzREHeaderToolReflectedClassMethodInfo {
    std::string                                      name;
    std::string                                      returnType;
    std::vector<std::pair<std::string, std::string>> parameters; // type, name
    std::vector<std::string>                         flags;
    std::unordered_map<std::string, std::string>     metadata;
    bool                                             isConst;
    bool                                             isStatic;
    bool                                             isVirtual;

    JzREHeaderToolReflectedClassMethodInfo() :
        isConst(false), isStatic(false), isVirtual(false) { }
};

/**
 * @brief JzRE Tool reflected class info
 */
struct JzREHeaderToolReflectedClassInfo {
    std::string                                           name;
    std::string                                           namespaceName;
    std::string                                           fullName;
    std::string                                           headerFile;
    std::vector<std::string>                              baseClasses;
    std::vector<JzREHeaderToolReflectedClassPropertyInfo> properties;
    std::vector<JzREHeaderToolReflectedClassMethodInfo>   methods;
    std::unordered_map<std::string, std::string>          metadata;
    size_t                                                sizeInBytes;

    JzREHeaderToolReflectedClassInfo() :
        sizeInBytes(0) { }

    std::string GetQualifiedName() const
    {
        if (namespaceName.empty()) {
            return name;
        }
        return namespaceName + "::" + name;
    }
};
