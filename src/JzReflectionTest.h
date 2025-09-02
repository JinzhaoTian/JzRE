#pragma once

#include "CommonTypes.h"

namespace JzRE {

// 前向声明
class JzProperty;

/**
 * @brief 反射系统测试类
 */
class JzReflectionTest {
public:
    /**
     * @brief 运行所有反射测试
     */
    static void RunAllTests();

    /**
     * @brief 测试基础反射功能
     */
    static void TestBasicReflection();

    /**
     * @brief 测试属性访问功能
     */
    static void TestPropertyAccess();

    /**
     * @brief 测试反射注册表
     */
    static void TestReflectionRegistry();

    /**
     * @brief 测试类型信息功能
     */
    static void TestTypeInfo();

private:
    /**
     * @brief 打印属性详细信息
     */
    static void PrintPropertyInfo(const JzProperty* property);
};

} // namespace JzRE
