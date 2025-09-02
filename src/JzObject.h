#pragma once

#include "CommonTypes.h"
#include "JzReflectable.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <typeindex>

namespace JzRE {

// 前向声明
class JzClass;
class JzProperty;
class JzMethod;
class JzObject;

// 模板函数声明
template<typename T>
String GetTypeName();

template<typename T>
String ValueToString(const T& value);

template<typename T>
Bool StringToValue(const String& str, T& value);

/**
 * @brief 属性访问器接口
 */
class JzPropertyAccessor {
public:
    virtual ~JzPropertyAccessor() = default;
    
    /**
     * @brief 获取属性值（以字符串形式）
     */
    virtual String GetValueAsString(const JzObject* object) const = 0;
    
    /**
     * @brief 设置属性值（从字符串）
     */
    virtual Bool SetValueFromString(JzObject* object, const String& value) const = 0;
    
    /**
     * @brief 获取属性类型名称
     */
    virtual String GetTypeName() const = 0;
    
    /**
     * @brief 检查属性是否可读
     */
    virtual Bool IsReadable() const = 0;
    
    /**
     * @brief 检查属性是否可写
     */
    virtual Bool IsWritable() const = 0;
};

/**
 * @brief 具体类型的属性访问器
 */
template<typename ClassType, typename PropertyType>
class JzTypedPropertyAccessor : public JzPropertyAccessor {
public:
    using PropertyPtr = PropertyType ClassType::*;
    using Getter = std::function<PropertyType(const ClassType*)>;
    using Setter = std::function<void(ClassType*, const PropertyType&)>;
    
    JzTypedPropertyAccessor(PropertyPtr propertyPtr)
        : m_propertyPtr(propertyPtr) {}
    
    JzTypedPropertyAccessor(Getter getter, Setter setter)
        : m_getter(getter), m_setter(setter) {}
    
    String GetValueAsString(const JzObject* object) const override;
    Bool SetValueFromString(JzObject* object, const String& value) const override;
    String GetTypeName() const override;
    Bool IsReadable() const override { return m_propertyPtr != nullptr || m_getter != nullptr; }
    Bool IsWritable() const override { return m_propertyPtr != nullptr || m_setter != nullptr; }

private:
    PropertyPtr m_propertyPtr = nullptr;
    Getter m_getter;
    Setter m_setter;
};

/**
 * @brief 属性信息类
 */
class JzProperty {
public:
    JzProperty(const String& name, std::unique_ptr<JzPropertyAccessor> accessor,
              const JzPropertyMetadata& metadata = JzPropertyMetadata(""))
        : m_name(name), m_accessor(std::move(accessor)), m_metadata(metadata) {}
    
    const String& GetName() const { return m_name; }
    const JzPropertyMetadata& GetMetadata() const { return m_metadata; }
    
    String GetValue(const JzObject* object) const {
        return m_accessor ? m_accessor->GetValueAsString(object) : "";
    }
    
    Bool SetValue(JzObject* object, const String& value) const {
        return m_accessor ? m_accessor->SetValueFromString(object, value) : false;
    }
    
    String GetTypeName() const {
        return m_accessor ? m_accessor->GetTypeName() : "unknown";
    }
    
    Bool IsReadable() const { return m_accessor && m_accessor->IsReadable(); }
    Bool IsWritable() const { return m_accessor && m_accessor->IsWritable(); }

private:
    String m_name;
    std::unique_ptr<JzPropertyAccessor> m_accessor;
    JzPropertyMetadata m_metadata;
};

/**
 * @brief 方法信息类
 */
class JzMethod {
public:
    JzMethod(const String& name, const String& returnType, const std::vector<String>& paramTypes)
        : m_name(name), m_returnType(returnType), m_paramTypes(paramTypes) {}
    
    const String& GetName() const { return m_name; }
    const String& GetReturnType() const { return m_returnType; }
    const std::vector<String>& GetParameterTypes() const { return m_paramTypes; }
    
    // TODO: 添加方法调用功能

private:
    String m_name;
    String m_returnType;
    std::vector<String> m_paramTypes;
};

/**
 * @brief 类反射信息
 */
class JzClass {
public:
    JzClass(const String& name, const String& parentName = "")
        : m_name(name), m_parentName(parentName) {}
    
    const String& GetName() const { return m_name; }
    const String& GetParentName() const { return m_parentName; }
    
    void AddProperty(std::unique_ptr<JzProperty> property) {
        m_properties[property->GetName()] = std::move(property);
    }
    
    void AddMethod(std::unique_ptr<JzMethod> method) {
        m_methods[method->GetName()] = std::move(method);
    }
    
    const JzProperty* GetProperty(const String& name) const {
        auto it = m_properties.find(name);
        return it != m_properties.end() ? it->second.get() : nullptr;
    }
    
    const JzMethod* GetMethod(const String& name) const {
        auto it = m_methods.find(name);
        return it != m_methods.end() ? it->second.get() : nullptr;
    }
    
    std::vector<const JzProperty*> GetAllProperties() const {
        std::vector<const JzProperty*> properties;
        for (const auto& pair : m_properties) {
            properties.push_back(pair.second.get());
        }
        return properties;
    }
    
    std::vector<const JzMethod*> GetAllMethods() const {
        std::vector<const JzMethod*> methods;
        for (const auto& pair : m_methods) {
            methods.push_back(pair.second.get());
        }
        return methods;
    }

private:
    String m_name;
    String m_parentName;
    std::unordered_map<String, std::unique_ptr<JzProperty>> m_properties;
    std::unordered_map<String, std::unique_ptr<JzMethod>> m_methods;
};

/**
 * @brief 反射注册中心
 */
class JzReflectionRegistry {
public:
    static JzReflectionRegistry& Get() {
        static JzReflectionRegistry instance;
        return instance;
    }
    
    void RegisterClass(std::unique_ptr<JzClass> classInfo) {
        m_classes[classInfo->GetName()] = std::move(classInfo);
    }
    
    JzClass* GetClass(const String& name) {
        auto it = m_classes.find(name);
        return it != m_classes.end() ? it->second.get() : nullptr;
    }
    
    template<typename ClassType, typename PropertyType>
    void RegisterProperty(const String& className, const String& propertyName, 
                         PropertyType ClassType::* propertyPtr,
                         const JzPropertyMetadata& metadata = JzPropertyMetadata("")) {
        auto classInfo = GetClass(className);
        if (!classInfo) {
            classInfo = new JzClass(className);
            RegisterClass(std::unique_ptr<JzClass>(classInfo));
        }
        
        auto accessor = std::make_unique<JzTypedPropertyAccessor<ClassType, PropertyType>>(propertyPtr);
        auto property = std::make_unique<JzProperty>(propertyName, std::move(accessor), metadata);
        classInfo->AddProperty(std::move(property));
    }
    
    std::vector<String> GetAllClassNames() const {
        std::vector<String> names;
        for (const auto& pair : m_classes) {
            names.push_back(pair.first);
        }
        return names;
    }

private:
    std::unordered_map<String, std::unique_ptr<JzClass>> m_classes;
};

/**
 * @brief 反射对象基类
 * 所有需要反射功能的类都应该继承自此类
 */
class JzObject {
public:
    JzObject() = default;
    virtual ~JzObject() = default;
    
    /**
     * @brief 获取对象的类反射信息
     */
    virtual JzClass* GetClass() const {
        return StaticClass();
    }
    
    /**
     * @brief 获取静态类反射信息
     */
    static JzClass* StaticClass() {
        static JzClass* s_class = nullptr;
        if (!s_class) {
            s_class = JzReflectionRegistry::Get().GetClass("JzObject");
            if (!s_class) {
                auto classInfo = std::make_unique<JzClass>("JzObject");
                s_class = classInfo.get();
                JzReflectionRegistry::Get().RegisterClass(std::move(classInfo));
            }
        }
        return s_class;
    }
    
    /**
     * @brief 检查是否为指定类型的实例
     */
    template<typename T>
    Bool IsA() const {
        return dynamic_cast<const T*>(this) != nullptr;
    }
    
    /**
     * @brief 安全转换为指定类型
     */
    template<typename T>
    T* Cast() {
        return dynamic_cast<T*>(this);
    }
    
    template<typename T>
    const T* Cast() const {
        return dynamic_cast<const T*>(this);
    }
    
    /**
     * @brief 获取属性值
     */
    String GetPropertyValue(const String& propertyName) const {
        auto classInfo = GetClass();
        if (classInfo) {
            auto property = classInfo->GetProperty(propertyName);
            if (property) {
                return property->GetValue(this);
            }
        }
        return "";
    }
    
    /**
     * @brief 设置属性值
     */
    Bool SetPropertyValue(const String& propertyName, const String& value) {
        auto classInfo = GetClass();
        if (classInfo) {
            auto property = classInfo->GetProperty(propertyName);
            if (property) {
                return property->SetValue(this, value);
            }
        }
        return false;
    }
    
    /**
     * @brief 获取所有属性名称
     */
    std::vector<String> GetPropertyNames() const {
        std::vector<String> names;
        auto classInfo = GetClass();
        if (classInfo) {
            auto properties = classInfo->GetAllProperties();
            for (const auto* property : properties) {
                names.push_back(property->GetName());
            }
        }
        return names;
    }
};

} // namespace JzRE
