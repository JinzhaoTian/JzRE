#pragma once

#include "CommonTypes.h"

namespace JzRE {

#define JzRE_REFLECTABLE()            \
    static const char *getClassName() \
    {                                 \
        return __VA_ARGS__;           \
    }                                 \
    friend class JzReflectionGenerator;

#define JzRE_REFLECT_FIELD(type, name) \
    type name

#define JzRE_REFLECT_METHOD(ret, name, ...) \
    ret name(__VA_ARGS__)

struct JzReflectFieldInfo {
    String                        name;
    String                        type;
    Size                          offset;
    std::function<void *(void *)> getter;
};

struct JzReflectMethodInfo {
    String                                             name;
    String                                             returnType;
    std::vector<String>                                parameterTypes;
    std::function<void *(void *, std::vector<void *>)> invoker;
};

struct JzReflectClassInfo {
    String                                          name;
    std::unordered_map<String, JzReflectFieldInfo>  fields;
    std::unordered_map<String, JzReflectMethodInfo> methods;
    std::function<void *()>                         createInstance;
};

class JzReflectionRegistry {
public:
    static JzReflectionRegistry &getInstance()
    {
        static JzReflectionRegistry instance;
        return instance;
    }

    void registerClass(const JzReflectClassInfo &classInfo)
    {
        classes[classInfo.name] = classInfo;
    }

    const JzReflectClassInfo *getClassInfo(const String &className) const
    {
        auto it = classes.find(className);
        return it != classes.end() ? &it->second : nullptr;
    }

    void printAllClasses() const
    {
        for (const auto &pair : classes) {
            std::cout << "Class: " << pair.first << std::endl;
            for (const auto &field : pair.second.fields) {
                std::cout << "  Field: " << field.second.type << " " << field.first << std::endl;
            }
            for (const auto &method : pair.second.methods) {
                std::cout << "  Method: " << method.second.returnType << " " << method.first << "()" << std::endl;
            }
        }
    }

private:
    std::unordered_map<String, JzReflectClassInfo> classes;
};

} // namespace JzRE