#include "JzReflectable.h"

namespace JzRE {
/**
 * @brief JzRE reflection registry
 */
class JzReflectionRegistry {
public:
    static JzReflectionRegistry &GetInstance()
    {
        static JzReflectionRegistry instance;
        return instance;
    }

    void RegisterClass(const JzReflectedClassInfo &classInfo)
    {
        classes[classInfo.name] = classInfo;
    }

    const JzReflectedClassInfo *GetClassInfo(const String &className) const
    {
        auto it = classes.find(className);
        return it != classes.end() ? &it->second : nullptr;
    }
    
    const JzReflectedClassInfo *FindClass(const String &className) const
    {
        return GetClassInfo(className);
    }
    
    const std::unordered_map<String, JzReflectedClassInfo>& GetAllClasses() const
    {
        return classes;
    }

    std::vector<String> GetAllClassNames() const
    {
        std::vector<String> names;
        for (const auto &pair : classes) {
            names.push_back(pair.first);
        }
        return names;
    }

    void PrintAllClasses() const
    {
        for (const auto &pair : classes) {
            std::cout << "Class: " << pair.first << std::endl;
            std::cout << "  Size: " << pair.second.size << " bytes" << std::endl;

            if (!pair.second.baseClasses.empty()) {
                std::cout << "  Base Classes: ";
                for (size_t i = 0; i < pair.second.baseClasses.size(); ++i) {
                    if (i > 0) std::cout << ", ";
                    std::cout << pair.second.baseClasses[i];
                }
                std::cout << std::endl;
            }

            for (const auto &field : pair.second.properties) {
                std::cout << "  Field: " << field.type << " " << field.name;
                if (!field.category.empty()) {
                    std::cout << " (Category: " << field.category << ")";
                }
                std::cout << std::endl;
            }

            for (const auto &method : pair.second.methods) {
                std::cout << "  Method: " << method.returnType << " " << method.name << "(";
                for (Size i = 0; i < method.parameters.size(); ++i) {
                    if (i > 0) std::cout << ", ";
                    std::cout << method.parameters[i].first << " " << method.parameters[i].second;
                }
                std::cout << ")";
                if (method.isConst) std::cout << " const";
                if (method.isStatic) std::cout << " [static]";
                if (method.isVirtual) std::cout << " [virtual]";
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    }

private:
    std::unordered_map<String, JzReflectedClassInfo> classes;
};
} // namespace JzRE