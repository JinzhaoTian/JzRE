/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include <gtest/gtest.h>
#include "JzRE/Core/JzMatrix.h"

using namespace JzRE;

class TestJzMatrix : public ::testing::Test { };

// Test matrix constructors
TEST_F(TestJzMatrix, Constructor)
{
    // Test default constructor
    JzMat4 m1;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_FLOAT_EQ(m1.At(i, j), 0.0f);
        }
    }

    // Test value-based constructor
    JzMat4 m2(1.0f);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_FLOAT_EQ(m2.At(i, j), 1.0f);
        }
    }

    // Test copy constructor
    JzMat4 m3(m2);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_FLOAT_EQ(m3.At(i, j), 1.0f);
        }
    }
}

// Test matrix assignment
TEST_F(TestJzMatrix, Assignment)
{
    JzMat4 m1(1.0f);
    JzMat4 m2;
    m2 = m1;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_FLOAT_EQ(m2.At(i, j), 1.0f);
        }
    }
}

// Test matrix arithmetic operators
TEST_F(TestJzMatrix, ArithmeticOperators)
{
    JzMat4 m1(1.0f);
    JzMat4 m2(2.0f);

    // Test addition
    JzMat4 result_add = m1 + m2;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_FLOAT_EQ(result_add.At(i, j), 3.0f);
        }
    }

    // Test subtraction
    JzMat4 result_sub = m2 - m1;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_FLOAT_EQ(result_sub.At(i, j), 1.0f);
        }
    }

    // Test scalar multiplication
    JzMat4 result_mul_scalar = m1 * 2.0f;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_FLOAT_EQ(result_mul_scalar.At(i, j), 2.0f);
        }
    }

    // Test scalar division
    JzMat4 result_div_scalar = m2 / 2.0f;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_FLOAT_EQ(result_div_scalar.At(i, j), 1.0f);
        }
    }
}

// Test matrix-matrix multiplication
TEST_F(TestJzMatrix, MatrixMultiplication)
{
    JzMat2 m1;
    m1.At(0, 0) = 1;
    m1.At(0, 1) = 2;
    m1.At(1, 0) = 3;
    m1.At(1, 1) = 4;

    JzMat2 m2;
    m2.At(0, 0) = 5;
    m2.At(0, 1) = 6;
    m2.At(1, 0) = 7;
    m2.At(1, 1) = 8;

    JzMat2 result = m1 * m2;
    EXPECT_FLOAT_EQ(result.At(0, 0), 19.0f);
    EXPECT_FLOAT_EQ(result.At(0, 1), 22.0f);
    EXPECT_FLOAT_EQ(result.At(1, 0), 43.0f);
    EXPECT_FLOAT_EQ(result.At(1, 1), 50.0f);
}

// Test compound assignment operators
TEST_F(TestJzMatrix, CompoundAssignment)
{
    JzMat4 m1(1.0f);
    JzMat4 m2(2.0f);

    // Test +=
    m1 += m2;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_FLOAT_EQ(m1.At(i, j), 3.0f);
        }
    }

    // Test -=
    m1 -= m2;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_FLOAT_EQ(m1.At(i, j), 1.0f);
        }
    }

    // Test /=
    JzMat4 m3(4.0f);
    m3 /= 2.0f;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_FLOAT_EQ(m3.At(i, j), 2.0f);
        }
    }
}

// Test transpose
TEST_F(TestJzMatrix, Transpose)
{
    JzMat2 m;
    m.At(0, 0) = 1;
    m.At(0, 1) = 2;
    m.At(1, 0) = 3;
    m.At(1, 1) = 4;

    JzMat2 mt = m.Transpose();
    EXPECT_FLOAT_EQ(mt.At(0, 0), 1.0f);
    EXPECT_FLOAT_EQ(mt.At(0, 1), 3.0f);
    EXPECT_FLOAT_EQ(mt.At(1, 0), 2.0f);
    EXPECT_FLOAT_EQ(mt.At(1, 1), 4.0f);
}

// Test element access
TEST_F(TestJzMatrix, ElementAccess)
{
    JzMat2 m;
    m.At(0, 1) = 5.0f;
    EXPECT_FLOAT_EQ(m.At(0, 1), 5.0f);

    m(1, 0) = 10.0f;
    EXPECT_FLOAT_EQ(m(1, 0), 10.0f);
}
