#pragma once

#include "CommonTypes.h"

namespace JzRE {

// 前向声明
class JzClass;
class JzProperty;
class JzMethod;

/**
 * @brief 反射元数据基类
 */
class JzMetadata {
public:
    virtual ~JzMetadata()                 = default;
    virtual const String &GetName() const = 0;
};

/**
 * @brief 属性元数据
 */
class JzPropertyMetadata : public JzMetadata {
public:
    JzPropertyMetadata(const String &name, const String &displayName = "",
                       const String &category = "", const String &tooltip = "") :
        m_name(name), m_displayName(displayName), m_category(category), m_tooltip(tooltip) { }

    const String &GetName() const override
    {
        return m_name;
    }
    const String &GetDisplayName() const
    {
        return m_displayName.empty() ? m_name : m_displayName;
    }
    const String &GetCategory() const
    {
        return m_category;
    }
    const String &GetTooltip() const
    {
        return m_tooltip;
    }

private:
    String m_name;
    String m_displayName;
    String m_category;
    String m_tooltip;
};

} // namespace JzRE

// ==================== 反射宏定义 ====================

/**
 * @brief 标记一个类为可反射的
 * 使用方法: JZCLASS(meta=(DisplayName="My Class", Category="Core"))
 */
#define JZCLASS(...)

/**
 * @brief 标记一个属性为可反射的
 * 使用方法: JZPROPERTY(meta=(DisplayName="Health", Category="Stats", Tooltip="Player health"))
 */
#define JZPROPERTY(...) \
    /* 属性元数据将由反射工具处理 */

/**
 * @brief 标记一个方法为可反射的
 * 使用方法: JZMETHOD(meta=(DisplayName="Take Damage", Category="Combat"))
 */
#define JZMETHOD(...) \
    /* 方法元数据将由反射工具处理 */

/**
 * @brief 生成反射代码的宏
 * 在每个.cpp文件的末尾调用，用于生成静态反射数据
 */
#define JZGENERATE_BODY() \
    /* 此宏的实际内容将由反射工具生成 */

// ==================== 内部使用的宏 ====================

/**
 * @brief 内部使用：声明类的反射信息
 */
#define JZCLASS_DECLARATION(ClassName)                                              \
    JzRE::JzClass *ClassName::s_StaticClass = nullptr;                              \
    JzRE::JzClass *ClassName::StaticClass()                                         \
    {                                                                               \
        if (!s_StaticClass) {                                                       \
            s_StaticClass = JzRE::JzReflectionRegistry::Get().GetClass(#ClassName); \
        }                                                                           \
        return s_StaticClass;                                                       \
    }                                                                               \
    JzRE::JzClass *ClassName::GetClass() const                                      \
    {                                                                               \
        return StaticClass();                                                       \
    }

/**
 * @brief 内部使用：注册属性信息
 */
#define JZREGISTER_PROPERTY(ClassName, PropertyName, PropertyType, ...)          \
    JzRE::JzReflectionRegistry::Get().RegisterProperty<ClassName, PropertyType>( \
        #ClassName, #PropertyName, &ClassName::PropertyName, ##__VA_ARGS__);

/**
 * @brief 内部使用：注册方法信息
 */
#define JZREGISTER_METHOD(ClassName, MethodName, ...)            \
    JzRE::JzReflectionRegistry::Get().RegisterMethod<ClassName>( \
        #ClassName, #MethodName, &ClassName::MethodName, ##__VA_ARGS__);

// ==================== 编译时辅助宏 ====================

/**
 * @brief 获取类型名称的编译时字符串
 */
#define JZTYPE_NAME(Type) #Type

/**
 * @brief 检查是否为可反射类型
 */
#define JZIS_REFLECTABLE(Type) \
    (std::is_base_of_v<JzRE::JzObject, Type>)

/**
 * @brief 用于标记反射工具需要处理的文件
 * 在文件开头使用
 */
#define JZREFLECTION_FILE() \
    /* 标记此文件包含反射代码，由反射工具处理 */

/**
 * @brief 获取枚举值名称（用于枚举反射）
 */
#define JZENUM_VALUE(EnumType, Value) \
    #Value

// ==================== 调试和诊断宏 ====================

#ifdef _DEBUG
#define JZREFLECTION_DEBUG_INFO(msg) \
    /* 调试模式下的反射信息输出 */
#else
#define JZREFLECTION_DEBUG_INFO(msg)
#endif

/**
 * @brief 验证反射宏的正确使用
 */
#define JZCHECK_REFLECTION()                                               \
    static_assert(JZIS_REFLECTABLE(std::remove_pointer_t<decltype(this)>), \
                  "Class must inherit from JzObject to use reflection");
