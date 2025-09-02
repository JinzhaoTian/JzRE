#include "JzObject.h"
#include <sstream>
#include <typeinfo>

namespace JzRE {

// ==================== 字符串转换辅助函数 ====================

template <typename T>
String ValueToString(const T &value)
{
    if constexpr (std::is_same_v<T, String>) {
        return value;
    } else if constexpr (std::is_same_v<T, Bool>) {
        return value ? "true" : "false";
    } else if constexpr (std::is_arithmetic_v<T>) {
        return std::to_string(value);
    } else {
        return "UnknownType";
    }
}

template <typename T>
Bool StringToValue(const String &str, T &value)
{
    if constexpr (std::is_same_v<T, String>) {
        value = str;
        return true;
    } else if constexpr (std::is_same_v<T, Bool>) {
        if (str == "true" || str == "1") {
            value = true;
            return true;
        } else if (str == "false" || str == "0") {
            value = false;
            return true;
        }
        return false;
    } else if constexpr (std::is_same_v<T, I32>) {
        try {
            value = std::stoi(str);
            return true;
        } catch (...) {
            return false;
        }
    } else if constexpr (std::is_same_v<T, U32>) {
        try {
            value = static_cast<U32>(std::stoul(str));
            return true;
        } catch (...) {
            return false;
        }
    } else if constexpr (std::is_same_v<T, I64>) {
        try {
            value = std::stoll(str);
            return true;
        } catch (...) {
            return false;
        }
    } else if constexpr (std::is_same_v<T, U64>) {
        try {
            value = std::stoull(str);
            return true;
        } catch (...) {
            return false;
        }
    } else if constexpr (std::is_same_v<T, F32>) {
        try {
            value = std::stof(str);
            return true;
        } catch (...) {
            return false;
        }
    } else if constexpr (std::is_same_v<T, F64>) {
        try {
            value = std::stod(str);
            return true;
        } catch (...) {
            return false;
        }
    }
    return false;
}

template <typename T>
String GetTypeName()
{
    if constexpr (std::is_same_v<T, String>) {
        return "String";
    } else if constexpr (std::is_same_v<T, Bool>) {
        return "Bool";
    } else if constexpr (std::is_same_v<T, I32>) {
        return "I32";
    } else if constexpr (std::is_same_v<T, U32>) {
        return "U32";
    } else if constexpr (std::is_same_v<T, I64>) {
        return "I64";
    } else if constexpr (std::is_same_v<T, U64>) {
        return "U64";
    } else if constexpr (std::is_same_v<T, F32>) {
        return "F32";
    } else if constexpr (std::is_same_v<T, F64>) {
        return "F64";
    } else {
        return typeid(T).name();
    }
}

// ==================== JzTypedPropertyAccessor 实现 ====================

template <typename ClassType, typename PropertyType>
String JzTypedPropertyAccessor<ClassType, PropertyType>::GetValueAsString(const JzObject *object) const
{
    if (!object) return "";

    const ClassType *typedObject = dynamic_cast<const ClassType *>(object);
    if (!typedObject) return "";

    if (m_propertyPtr) {
        const PropertyType &value = typedObject->*m_propertyPtr;
        return ValueToString(value);
    } else if (m_getter) {
        const PropertyType &value = m_getter(typedObject);
        return ValueToString(value);
    }

    return "";
}

template <typename ClassType, typename PropertyType>
Bool JzTypedPropertyAccessor<ClassType, PropertyType>::SetValueFromString(JzObject *object, const String &value) const
{
    if (!object) return false;

    ClassType *typedObject = dynamic_cast<ClassType *>(object);
    if (!typedObject) return false;

    PropertyType convertedValue;
    if (!StringToValue(value, convertedValue)) {
        return false;
    }

    if (m_propertyPtr) {
        typedObject->*m_propertyPtr = convertedValue;
        return true;
    } else if (m_setter) {
        m_setter(typedObject, convertedValue);
        return true;
    }

    return false;
}

template <typename ClassType, typename PropertyType>
String JzTypedPropertyAccessor<ClassType, PropertyType>::GetTypeName() const
{
    return ::JzRE::GetTypeName<PropertyType>();
}

// ==================== 显式实例化常用类型 ====================

// 为常用类型显式实例化模板，避免链接错误
#define INSTANTIATE_PROPERTY_ACCESSOR(ClassType, PropertyType)                   \
    template class JzTypedPropertyAccessor<ClassType, PropertyType>;             \
    template String ValueToString<PropertyType>(const PropertyType &);           \
    template Bool   StringToValue<PropertyType>(const String &, PropertyType &); \
    template String GetTypeName<PropertyType>();

// 这里暂时为JzObject基类实例化，实际使用时会为具体类型实例化
INSTANTIATE_PROPERTY_ACCESSOR(JzObject, String)
INSTANTIATE_PROPERTY_ACCESSOR(JzObject, Bool)
INSTANTIATE_PROPERTY_ACCESSOR(JzObject, I32)
INSTANTIATE_PROPERTY_ACCESSOR(JzObject, U32)
INSTANTIATE_PROPERTY_ACCESSOR(JzObject, I64)
INSTANTIATE_PROPERTY_ACCESSOR(JzObject, U64)
INSTANTIATE_PROPERTY_ACCESSOR(JzObject, F32)
INSTANTIATE_PROPERTY_ACCESSOR(JzObject, F64)

// ==================== 反射系统初始化 ====================

/**
 * @brief 反射系统初始化器
 */
class JzReflectionInitializer {
public:
    JzReflectionInitializer()
    {
        // 注册基础JzObject类
        auto &registry  = JzReflectionRegistry::Get();
        auto  baseClass = std::make_unique<JzClass>("JzObject");
        registry.RegisterClass(std::move(baseClass));
    }
};

// 全局初始化器，确保反射系统在使用前被初始化
static JzReflectionInitializer s_reflectionInit;

} // namespace JzRE
