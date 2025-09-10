#include <gtest/gtest.h>
#include "CommonTypes.h"

using namespace JzRE;

// 基本测试，确保测试框架正常工作
TEST(BasicTest, TypeDefinitions) {
    // 测试基本类型定义
    U32 unsignedValue = 42;
    I32 signedValue = -42;
    F32 floatValue = 3.14f;
    F64 doubleValue = 3.14159265359;
    
    EXPECT_EQ(unsignedValue, 42u);
    EXPECT_EQ(signedValue, -42);
    EXPECT_FLOAT_EQ(floatValue, 3.14f);
    EXPECT_DOUBLE_EQ(doubleValue, 3.14159265359);
}

TEST(BasicTest, BooleanType) {
    Bool trueValue = true;
    Bool falseValue = false;
    
    EXPECT_TRUE(trueValue);
    EXPECT_FALSE(falseValue);
}

TEST(BasicTest, StringTypes) {
    String str = "Hello, JzRE!";
    EXPECT_EQ(str, "Hello, JzRE!");
    EXPECT_GT(str.length(), 0);
}
