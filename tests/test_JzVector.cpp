#include <gtest/gtest.h>
#include "JzVector.h"

using namespace JzRE;

class JzVectorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 设置测试数据
    }

    void TearDown() override {
        // 清理测试数据
    }
};

// 测试向量构造函数
TEST_F(JzVectorTest, Constructor) {
    // 测试默认构造
    JzVec3 v1(0.0f);
    EXPECT_FLOAT_EQ(v1.x(), 0.0f);
    EXPECT_FLOAT_EQ(v1.y(), 0.0f);
    EXPECT_FLOAT_EQ(v1.z(), 0.0f);

    // 测试参数构造
    JzVec3 v2(1.0f, 2.0f, 3.0f);
    EXPECT_FLOAT_EQ(v2.x(), 1.0f);
    EXPECT_FLOAT_EQ(v2.y(), 2.0f);
    EXPECT_FLOAT_EQ(v2.z(), 3.0f);

    // 测试拷贝构造
    JzVec3 v3(v2);
    EXPECT_FLOAT_EQ(v3.x(), 1.0f);
    EXPECT_FLOAT_EQ(v3.y(), 2.0f);
    EXPECT_FLOAT_EQ(v3.z(), 3.0f);
}

// 测试向量运算符
TEST_F(JzVectorTest, Operators) {
    JzVec3 v1(1.0f, 2.0f, 3.0f);
    JzVec3 v2(4.0f, 5.0f, 6.0f);

    // 测试加法
    JzVec3 result = v1 + v2;
    EXPECT_FLOAT_EQ(result.x(), 5.0f);
    EXPECT_FLOAT_EQ(result.y(), 7.0f);
    EXPECT_FLOAT_EQ(result.z(), 9.0f);

    // 测试减法
    result = v2 - v1;
    EXPECT_FLOAT_EQ(result.x(), 3.0f);
    EXPECT_FLOAT_EQ(result.y(), 3.0f);
    EXPECT_FLOAT_EQ(result.z(), 3.0f);

    // 测试标量乘法
    result = v1 * 2.0f;
    EXPECT_FLOAT_EQ(result.x(), 2.0f);
    EXPECT_FLOAT_EQ(result.y(), 4.0f);
    EXPECT_FLOAT_EQ(result.z(), 6.0f);

    // 测试相等性
    JzVec3 v3(1.0f, 2.0f, 3.0f);
    EXPECT_TRUE(v1 == v3);
    EXPECT_FALSE(v1 == v2);

    // 测试索引访问
    EXPECT_FLOAT_EQ(v1[0], 1.0f);
    EXPECT_FLOAT_EQ(v1[1], 2.0f);
    EXPECT_FLOAT_EQ(v1[2], 3.0f);
}

// 测试向量数学函数
TEST_F(JzVectorTest, MathFunctions) {
    JzVec3 v1(3.0f, 4.0f, 0.0f);

    // 测试长度计算
    EXPECT_FLOAT_EQ(v1.Length(), 5.0f);
    EXPECT_FLOAT_EQ(v1.LengthSquared(), 25.0f);

    // 测试点积
    JzVec3 v2(1.0f, 0.0f, 0.0f);
    EXPECT_FLOAT_EQ(v1.Dot(v2), 3.0f);

    // 测试单位化
    JzVec3 normalized = v1.Normalized();
    EXPECT_FLOAT_EQ(normalized.Length(), 1.0f);

    // 测试叉积
    JzVec3 v3(1.0f, 0.0f, 0.0f);
    JzVec3 v4(0.0f, 1.0f, 0.0f);
    JzVec3 cross = v3.Cross(v4);
    EXPECT_FLOAT_EQ(cross.x(), 0.0f);
    EXPECT_FLOAT_EQ(cross.y(), 0.0f);
    EXPECT_FLOAT_EQ(cross.z(), 1.0f);
}

// 测试向量负号
TEST_F(JzVectorTest, Negation) {
    JzVec3 v1(1.0f, -2.0f, 3.0f);
    JzVec3 negated = -v1;
    
    EXPECT_FLOAT_EQ(negated.x(), -1.0f);
    EXPECT_FLOAT_EQ(negated.y(), 2.0f);
    EXPECT_FLOAT_EQ(negated.z(), -3.0f);
}

// 测试向量复合赋值操作符
TEST_F(JzVectorTest, CompoundAssignmentOperators) {
    JzVec3 v1(1.0f, 2.0f, 3.0f);
    JzVec3 v2(4.0f, 5.0f, 6.0f);

    // 测试 +=
    v1 += v2;
    EXPECT_FLOAT_EQ(v1.x(), 5.0f);
    EXPECT_FLOAT_EQ(v1.y(), 7.0f);
    EXPECT_FLOAT_EQ(v1.z(), 9.0f);

    // 测试 -=
    v1 -= v2;
    EXPECT_FLOAT_EQ(v1.x(), 1.0f);
    EXPECT_FLOAT_EQ(v1.y(), 2.0f);
    EXPECT_FLOAT_EQ(v1.z(), 3.0f);

    // 测试 *=
    v1 *= 2.0f;
    EXPECT_FLOAT_EQ(v1.x(), 2.0f);
    EXPECT_FLOAT_EQ(v1.y(), 4.0f);
    EXPECT_FLOAT_EQ(v1.z(), 6.0f);

    // 测试 /=
    v1 /= 2.0f;
    EXPECT_FLOAT_EQ(v1.x(), 1.0f);
    EXPECT_FLOAT_EQ(v1.y(), 2.0f);
    EXPECT_FLOAT_EQ(v1.z(), 3.0f);
}

// 测试不同维度的向量
TEST_F(JzVectorTest, DifferentDimensions) {
    // 2D向量测试
    JzVec2 v2d(1.0f, 2.0f);
    EXPECT_FLOAT_EQ(v2d.x(), 1.0f);
    EXPECT_FLOAT_EQ(v2d.y(), 2.0f);

    // 4D向量测试
    JzVec4 v4d(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_FLOAT_EQ(v4d.x(), 1.0f);
    EXPECT_FLOAT_EQ(v4d.y(), 2.0f);
    EXPECT_FLOAT_EQ(v4d.z(), 3.0f);
    EXPECT_FLOAT_EQ(v4d.w(), 4.0f);

    // 整数向量测试
    JzIVec3 iv(1, 2, 3);
    EXPECT_EQ(iv.x(), 1);
    EXPECT_EQ(iv.y(), 2);
    EXPECT_EQ(iv.z(), 3);
}
