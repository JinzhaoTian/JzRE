#pragma once

#include "CommonTypes.h"
#include <functional>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <typeinfo>
#include <any>

namespace JzRE {

// 反射属性标记宏 - 这些会被HeaderTool识别
#define JzRE_CLASS(...) __attribute__((annotate("JzRE_CLASS:" #__VA_ARGS__)))
#define JzRE_PROPERTY(...) __attribute__((annotate("JzRE_PROPERTY:" #__VA_ARGS__)))
#define JzRE_FUNCTION(...) __attribute__((annotate("JzRE_FUNCTION:" #__VA_ARGS__)))

// 生成的代码中使用的宏
#define JzRE_GENERATED_BODY() \
    friend class JzReflectionGenerator; \
    public: \
    virtual const char* GetClassNameStatic() const { return GetStaticClass().GetName(); } \
    static const JzReflectClassInfo& GetStaticClass(); \
    private:

// 属性访问器宏
#define JzRE_DECLARE_PROPERTY_GETTER(ClassName, PropertyName, PropertyType) \
    static std::any Get##PropertyName(void* instance) { \
        return static_cast<ClassName*>(instance)->PropertyName; \
    }

#define JzRE_DECLARE_PROPERTY_SETTER(ClassName, PropertyName, PropertyType) \
    static void Set##PropertyName(void* instance, const std::any& value) { \
        static_cast<ClassName*>(instance)->PropertyName = std::any_cast<PropertyType>(value); \
    }

// 属性元数据类型
enum class EJzPropertyFlags : uint32_t {
    None = 0,
    EditAnywhere = 1 << 0,
    BlueprintReadOnly = 1 << 1,
    BlueprintReadWrite = 1 << 2,
    VisibleAnywhere = 1 << 3,
    Category = 1 << 4,
    Transient = 1 << 5,
    SaveGame = 1 << 6
};

inline EJzPropertyFlags operator|(EJzPropertyFlags a, EJzPropertyFlags b) {
    return static_cast<EJzPropertyFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline EJzPropertyFlags operator&(EJzPropertyFlags a, EJzPropertyFlags b) {
    return static_cast<EJzPropertyFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

// 反射信息结构体
struct JzReflectFieldInfo {
    String name;
    String type;
    String category;
    Size offset;
    Size size;
    EJzPropertyFlags flags;
    std::function<std::any(void*)> getter;
    std::function<void(void*, const std::any&)> setter;
    std::unordered_map<String, String> metadata;
    
    JzReflectFieldInfo() : offset(0), size(0), flags(EJzPropertyFlags::None) {}
};

struct JzReflectParameterInfo {
    String name;
    String type;
    Size size;
    bool isConst;
    bool isReference;
    bool isPointer;
};

struct JzReflectMethodInfo {
    String name;
    String returnType;
    std::vector<JzReflectParameterInfo> parameters;
    std::function<std::any(void*, const std::vector<std::any>&)> invoker;
    std::unordered_map<String, String> metadata;
    bool isConst;
    bool isStatic;
    bool isVirtual;
    
    JzReflectMethodInfo() : isConst(false), isStatic(false), isVirtual(false) {}
};

struct JzReflectClassInfo {
    String name;
    String namespaceName;
    Size size;
    std::unordered_map<String, JzReflectFieldInfo> fields;
    std::unordered_map<String, JzReflectMethodInfo> methods;
    std::function<void*()> createInstance;
    std::function<void(void*)> destroyInstance;
    std::unordered_map<String, String> metadata;
    
    // 父类信息
    std::vector<String> baseClasses;
    
    JzReflectClassInfo() : size(0) {}
    
    const char* GetName() const { return name.c_str(); }
    Size GetSize() const { return size; }
    
    const JzReflectFieldInfo* GetField(const String& fieldName) const {
        auto it = fields.find(fieldName);
        return it != fields.end() ? &it->second : nullptr;
    }
    
    const JzReflectMethodInfo* GetMethod(const String& methodName) const {
        auto it = methods.find(methodName);
        return it != methods.end() ? &it->second : nullptr;
    }
    
    std::vector<String> GetFieldNames() const {
        std::vector<String> names;
        for (const auto& pair : fields) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    std::vector<String> GetMethodNames() const {
        std::vector<String> names;
        for (const auto& pair : methods) {
            names.push_back(pair.first);
        }
        return names;
    }
};

// 反射注册中心
class JzReflectionRegistry {
public:
    static JzReflectionRegistry& GetInstance() {
        static JzReflectionRegistry instance;
        return instance;
    }

    void RegisterClass(const JzReflectClassInfo& classInfo) {
        classes[classInfo.name] = classInfo;
    }

    const JzReflectClassInfo* GetClassInfo(const String& className) const {
        auto it = classes.find(className);
        return it != classes.end() ? &it->second : nullptr;
    }
    
    std::vector<String> GetAllClassNames() const {
        std::vector<String> names;
        for (const auto& pair : classes) {
            names.push_back(pair.first);
        }
        return names;
    }

    void PrintAllClasses() const {
        for (const auto& pair : classes) {
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
            
            for (const auto& field : pair.second.fields) {
                std::cout << "  Field: " << field.second.type << " " << field.first;
                if (!field.second.category.empty()) {
                    std::cout << " (Category: " << field.second.category << ")";
                }
                std::cout << std::endl;
            }
            
            for (const auto& method : pair.second.methods) {
                std::cout << "  Method: " << method.second.returnType << " " << method.first << "(";
                for (size_t i = 0; i < method.second.parameters.size(); ++i) {
                    if (i > 0) std::cout << ", ";
                    std::cout << method.second.parameters[i].type << " " << method.second.parameters[i].name;
                }
                std::cout << ")";
                if (method.second.isConst) std::cout << " const";
                if (method.second.isStatic) std::cout << " [static]";
                if (method.second.isVirtual) std::cout << " [virtual]";
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    }

    // 获取具有特定属性的类
    std::vector<String> GetClassesWithProperty(const String& propertyName) const {
        std::vector<String> result;
        for (const auto& pair : classes) {
            if (pair.second.fields.find(propertyName) != pair.second.fields.end()) {
                result.push_back(pair.first);
            }
        }
        return result;
    }

    // 运行时类型创建
    template<typename T>
    T* CreateInstance(const String& className) const {
        const auto* classInfo = GetClassInfo(className);
        if (classInfo && classInfo->createInstance) {
            return static_cast<T*>(classInfo->createInstance());
        }
        return nullptr;
    }

private:
    std::unordered_map<String, JzReflectClassInfo> classes;
};

// 注册帮助宏
#define JzRE_REGISTER_CLASS(ClassName) \
    namespace { \
        static bool ClassName##_registered = []() { \
            extern const JzReflectClassInfo& Get##ClassName##ClassInfo(); \
            JzReflectionRegistry::GetInstance().RegisterClass(Get##ClassName##ClassInfo()); \
            return true; \
        }(); \
    }

} // namespace JzRE