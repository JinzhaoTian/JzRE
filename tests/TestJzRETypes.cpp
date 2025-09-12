/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRETypes.h"
#include <gtest/gtest.h>

// 基本测试，确保测试框架正常工作
TEST(BasicTest, TypeDefinitions)
{
    // 测试基本类型定义
    JzRE::U32 unsignedValue = 42;
    EXPECT_EQ(unsignedValue, 42u);

    JzRE::I32 signedValue = -42;
    EXPECT_EQ(signedValue, -42);

    JzRE::F32 floatValue = 3.14f;
    EXPECT_FLOAT_EQ(floatValue, 3.14f);

    JzRE::F64 doubleValue = 3.14159265359;
    EXPECT_DOUBLE_EQ(doubleValue, 3.14159265359);
}

TEST(BasicTest, BooleanType)
{
    JzRE::Bool trueValue = true;
    EXPECT_TRUE(trueValue);

    JzRE::Bool falseValue = false;
    EXPECT_FALSE(falseValue);
}

TEST(BasicTest, StringTypes)
{
    JzRE::String str = "Hello, JzRE!";
    EXPECT_EQ(str, "Hello, JzRE!");
    EXPECT_GT(str.length(), 0);
}
