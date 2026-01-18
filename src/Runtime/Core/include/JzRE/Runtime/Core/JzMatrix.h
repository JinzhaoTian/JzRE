/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <cmath>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"

namespace JzRE {

// Forward declaration
template <U16 M, U16 N, typename T = F32>
class JzMatrix;

/**
 * @brief Base Matrix Class (for arbitrary dimensions)
 *
 * This is the generic template for matrices of any dimension.
 * For dimensions 2x2, 3x3, and 4x4, specialized templates with union-based member access are provided.
 */
template <U16 M, U16 N, typename T>
class JzMatrix {
public:
    /**
     * @brief Constructor
     */
    inline JzMatrix()
    {
        for (U16 i = 0; i < M; ++i) {
            for (U16 j = 0; j < N; ++j) {
                At(i, j) = T(0);
            }
        }
    }

    /**
     * @brief Constructor
     *
     * @param value The value to initialize the matrix with
     */
    inline JzMatrix(const T &value)
    {
        for (U16 i = 0; i < M; ++i) {
            for (U16 j = 0; j < N; ++j) {
                At(i, j) = value;
            }
        }
    }

    /**
     * @brief Constructor
     *
     * @param data The data to initialize the matrix with
     */
    inline JzMatrix(const T *data)
    {
        std::memcpy(m_Data, data, sizeof(m_Data));
    }

    /**
     * @brief Copy Constructor
     */
    inline JzMatrix(const JzMatrix &other)
    {
        std::memcpy(m_Data, other.m_Data, sizeof(m_Data));
    }

    inline JzMatrix<M, N, T> &operator=(const JzMatrix<M, N, T> &other)
    {
        std::memcpy(m_Data, other.m_Data, sizeof(m_Data));
        return *this;
    }

    inline JzMatrix<M, N, T> &operator+=(const JzMatrix<M, N, T> &other)
    {
        for (U16 i = 0; i < M; ++i) {
            for (U16 j = 0; j < N; ++j) {
                At(i, j) += other.At(i, j);
            }
        }
        return *this;
    }

    inline JzMatrix<M, N, T> &operator-=(const JzMatrix<M, N, T> &other)
    {
        for (U16 i = 0; i < M; ++i) {
            for (U16 j = 0; j < N; ++j) {
                At(i, j) -= other.At(i, j);
            }
        }
        return *this;
    }

    inline JzMatrix<M, N, T> &operator*=(const T &value)
    {
        for (U16 i = 0; i < M; ++i) {
            for (U16 j = 0; j < N; ++j) {
                At(i, j) *= value;
            }
        }
        return *this;
    }

    inline JzMatrix<M, N, T> &operator/=(const T &value)
    {
        for (U16 i = 0; i < M; ++i) {
            for (U16 j = 0; j < N; ++j) {
                At(i, j) /= value;
            }
        }
        return *this;
    }

    inline JzMatrix<M, N, T> operator+(const JzMatrix<M, N, T> &other) const
    {
        JzMatrix<M, N, T> result  = *this;
        result                   += other;
        return result;
    }

    inline JzMatrix<M, N, T> operator-(const JzMatrix<M, N, T> &other) const
    {
        JzMatrix<M, N, T> result  = *this;
        result                   -= other;
        return result;
    }

    template <U16 R>
    inline JzMatrix<M, R, T> operator*(const JzMatrix<N, R, T> &other) const
    {
        JzMatrix<M, R, T> result;
        for (U16 r = 0; r < M; ++r) {
            for (U16 c = 0; c < R; ++c) {
                T sum = T(0);
                for (U16 i = 0; i < N; ++i) {
                    sum += At(r, i) * other.At(i, c);
                }
                result.At(r, c) = sum;
            }
        }
        return result;
    }

    inline JzMatrix<M, N, T> operator*(const T &value) const
    {
        JzMatrix<M, N, T> result  = *this;
        result                   *= value;
        return result;
    }

    inline JzMatrix<M, N, T> operator/(const T &value) const
    {
        JzMatrix<M, N, T> result  = *this;
        result                   /= value;
        return result;
    }

    inline JzMatrix<N, M, T> Transpose() const
    {
        JzMatrix<N, M, T> result;
        for (U16 i = 0; i < M; ++i) {
            for (U16 j = 0; j < N; ++j) {
                result.At(j, i) = At(i, j);
            }
        }
        return result;
    }

    inline const T &At(U16 i, U16 j) const
    {
        return m_Data[i * N + j];
    }

    inline T &At(U16 i, U16 j)
    {
        return m_Data[i * N + j];
    }

    inline const T &operator()(U16 i, U16 j) const
    {
        return At(i, j);
    }

    inline T &operator()(U16 i, U16 j)
    {
        return At(i, j);
    }

    inline const T *operator[](U16 i) const
    {
        return m_Data + i * N;
    }

    inline T *operator[](U16 i)
    {
        return m_Data + i * N;
    }

    /**
     * @brief Get a pointer to the underlying data array
     *
     * @return Pointer to the first element
     */
    inline T *Data()
    {
        return m_Data;
    }

    /**
     * @brief Get a const pointer to the underlying data array
     *
     * @return Const pointer to the first element
     */
    inline const T *Data() const
    {
        return m_Data;
    }

private:
    T m_Data[M * N];
};

/**
 * @brief 2x2 Matrix Specialization with union-based member access
 *
 * Provides direct member access via m00, m01, m10, m11.
 */
template <typename T>
class JzMatrix<2, 2, T> {
public:
    union {
        struct {
            T m00, m01;
            T m10, m11;
        };
        T data[4];
    };

    inline JzMatrix() :
        m00(T(0)), m01(T(0)), m10(T(0)), m11(T(0)) { }

    inline JzMatrix(const T &value) :
        m00(value), m01(value), m10(value), m11(value) { }

    inline JzMatrix(T m00_, T m01_, T m10_, T m11_) :
        m00(m00_), m01(m01_), m10(m10_), m11(m11_) { }

    inline JzMatrix(const T *values)
    {
        std::memcpy(data, values, sizeof(data));
    }

    inline JzMatrix(const JzMatrix &other)
    {
        std::memcpy(data, other.data, sizeof(data));
    }

    inline JzMatrix<2, 2, T> &operator=(const JzMatrix<2, 2, T> &other)
    {
        std::memcpy(data, other.data, sizeof(data));
        return *this;
    }

    inline const T &At(U16 i, U16 j) const
    {
        return data[i * 2 + j];
    }

    inline T &At(U16 i, U16 j)
    {
        return data[i * 2 + j];
    }

    inline const T &operator()(U16 i, U16 j) const
    {
        return At(i, j);
    }

    inline T &operator()(U16 i, U16 j)
    {
        return At(i, j);
    }

    inline const T *operator[](U16 i) const
    {
        return data + i * 2;
    }

    inline T *operator[](U16 i)
    {
        return data + i * 2;
    }

    inline JzMatrix<2, 2, T> operator+(const JzMatrix<2, 2, T> &other) const
    {
        return JzMatrix<2, 2, T>(m00 + other.m00, m01 + other.m01, m10 + other.m10, m11 + other.m11);
    }

    inline JzMatrix<2, 2, T> operator-(const JzMatrix<2, 2, T> &other) const
    {
        return JzMatrix<2, 2, T>(m00 - other.m00, m01 - other.m01, m10 - other.m10, m11 - other.m11);
    }

    inline JzMatrix<2, 2, T> operator*(const JzMatrix<2, 2, T> &other) const
    {
        return JzMatrix<2, 2, T>(
            m00 * other.m00 + m01 * other.m10,
            m00 * other.m01 + m01 * other.m11,
            m10 * other.m00 + m11 * other.m10,
            m10 * other.m01 + m11 * other.m11);
    }

    inline JzMatrix<2, 2, T> operator*(const T &value) const
    {
        return JzMatrix<2, 2, T>(m00 * value, m01 * value, m10 * value, m11 * value);
    }

    inline JzMatrix<2, 2, T> Transpose() const
    {
        return JzMatrix<2, 2, T>(m00, m10, m01, m11);
    }

    inline T Determinant() const
    {
        return m00 * m11 - m01 * m10;
    }

    inline T *Data()
    {
        return data;
    }

    inline const T *Data() const
    {
        return data;
    }

    static JzMatrix<2, 2, T> Identity()
    {
        return JzMatrix<2, 2, T>(T(1), T(0), T(0), T(1));
    }
};

/**
 * @brief 3x3 Matrix Specialization with union-based member access
 *
 * Provides direct member access via m00, m01, m02, m10, m11, m12, m20, m21, m22.
 */
template <typename T>
class JzMatrix<3, 3, T> {
public:
    union {
        struct {
            T m00, m01, m02;
            T m10, m11, m12;
            T m20, m21, m22;
        };
        T data[9];
    };

    inline JzMatrix() :
        m00(T(0)), m01(T(0)), m02(T(0)), m10(T(0)), m11(T(0)), m12(T(0)), m20(T(0)), m21(T(0)), m22(T(0)) { }

    inline JzMatrix(const T &value) :
        m00(value), m01(value), m02(value), m10(value), m11(value), m12(value), m20(value), m21(value), m22(value) { }

    inline JzMatrix(
        T m00_, T m01_, T m02_,
        T m10_, T m11_, T m12_,
        T m20_, T m21_, T m22_) :
        m00(m00_), m01(m01_), m02(m02_), m10(m10_), m11(m11_), m12(m12_), m20(m20_), m21(m21_), m22(m22_)
    {
    }

    inline JzMatrix(const T *values)
    {
        std::memcpy(data, values, sizeof(data));
    }

    inline JzMatrix(const JzMatrix &other)
    {
        std::memcpy(data, other.data, sizeof(data));
    }

    inline JzMatrix<3, 3, T> &operator=(const JzMatrix<3, 3, T> &other)
    {
        std::memcpy(data, other.data, sizeof(data));
        return *this;
    }

    inline const T &At(U16 i, U16 j) const
    {
        return data[i * 3 + j];
    }

    inline T &At(U16 i, U16 j)
    {
        return data[i * 3 + j];
    }

    inline const T &operator()(U16 i, U16 j) const
    {
        return At(i, j);
    }

    inline T &operator()(U16 i, U16 j)
    {
        return At(i, j);
    }

    inline const T *operator[](U16 i) const
    {
        return data + i * 3;
    }

    inline T *operator[](U16 i)
    {
        return data + i * 3;
    }

    inline JzMatrix<3, 3, T> operator+(const JzMatrix<3, 3, T> &other) const
    {
        return JzMatrix<3, 3, T>(
            m00 + other.m00, m01 + other.m01, m02 + other.m02,
            m10 + other.m10, m11 + other.m11, m12 + other.m12,
            m20 + other.m20, m21 + other.m21, m22 + other.m22);
    }

    inline JzMatrix<3, 3, T> operator-(const JzMatrix<3, 3, T> &other) const
    {
        return JzMatrix<3, 3, T>(
            m00 - other.m00, m01 - other.m01, m02 - other.m02,
            m10 - other.m10, m11 - other.m11, m12 - other.m12,
            m20 - other.m20, m21 - other.m21, m22 - other.m22);
    }

    inline JzMatrix<3, 3, T> operator*(const JzMatrix<3, 3, T> &other) const
    {
        return JzMatrix<3, 3, T>(
            m00 * other.m00 + m01 * other.m10 + m02 * other.m20,
            m00 * other.m01 + m01 * other.m11 + m02 * other.m21,
            m00 * other.m02 + m01 * other.m12 + m02 * other.m22,
            m10 * other.m00 + m11 * other.m10 + m12 * other.m20,
            m10 * other.m01 + m11 * other.m11 + m12 * other.m21,
            m10 * other.m02 + m11 * other.m12 + m12 * other.m22,
            m20 * other.m00 + m21 * other.m10 + m22 * other.m20,
            m20 * other.m01 + m21 * other.m11 + m22 * other.m21,
            m20 * other.m02 + m21 * other.m12 + m22 * other.m22);
    }

    inline JzMatrix<3, 3, T> operator*(const T &value) const
    {
        return JzMatrix<3, 3, T>(
            m00 * value, m01 * value, m02 * value,
            m10 * value, m11 * value, m12 * value,
            m20 * value, m21 * value, m22 * value);
    }

    inline JzMatrix<3, 3, T> Transpose() const
    {
        return JzMatrix<3, 3, T>(
            m00, m10, m20,
            m01, m11, m21,
            m02, m12, m22);
    }

    inline T Determinant() const
    {
        return m00 * (m11 * m22 - m12 * m21) - m01 * (m10 * m22 - m12 * m20) + m02 * (m10 * m21 - m11 * m20);
    }

    inline T *Data()
    {
        return data;
    }

    inline const T *Data() const
    {
        return data;
    }

    static JzMatrix<3, 3, T> Identity()
    {
        return JzMatrix<3, 3, T>(
            T(1), T(0), T(0),
            T(0), T(1), T(0),
            T(0), T(0), T(1));
    }
};

/**
 * @brief 4x4 Matrix Specialization with union-based member access
 *
 * Provides direct member access via m00, m01, m02, m03, m10, m11, m12, m13, etc.
 * Also provides row accessors: row0, row1, row2, row3.
 */
template <typename T>
class JzMatrix<4, 4, T> {
public:
    union {
        struct {
            T m00, m01, m02, m03;
            T m10, m11, m12, m13;
            T m20, m21, m22, m23;
            T m30, m31, m32, m33;
        };
        T data[16];
    };

    inline JzMatrix() :
        m00(T(0)), m01(T(0)), m02(T(0)), m03(T(0)), m10(T(0)), m11(T(0)), m12(T(0)), m13(T(0)), m20(T(0)), m21(T(0)), m22(T(0)), m23(T(0)), m30(T(0)), m31(T(0)), m32(T(0)), m33(T(0)) { }

    inline JzMatrix(const T &value) :
        m00(value), m01(value), m02(value), m03(value), m10(value), m11(value), m12(value), m13(value), m20(value), m21(value), m22(value), m23(value), m30(value), m31(value), m32(value), m33(value) { }

    inline JzMatrix(
        T m00_, T m01_, T m02_, T m03_,
        T m10_, T m11_, T m12_, T m13_,
        T m20_, T m21_, T m22_, T m23_,
        T m30_, T m31_, T m32_, T m33_) :
        m00(m00_), m01(m01_), m02(m02_), m03(m03_), m10(m10_), m11(m11_), m12(m12_), m13(m13_), m20(m20_), m21(m21_), m22(m22_), m23(m23_), m30(m30_), m31(m31_), m32(m32_), m33(m33_)
    {
    }

    inline JzMatrix(const T *values)
    {
        std::memcpy(data, values, sizeof(data));
    }

    inline JzMatrix(const JzMatrix &other)
    {
        std::memcpy(data, other.data, sizeof(data));
    }

    /**
     * @brief Construct from four row vectors
     */
    inline JzMatrix(const JzVector<4, T> &v0, const JzVector<4, T> &v1, const JzVector<4, T> &v2, const JzVector<4, T> &v3)
    {
        m00 = v0.x;
        m01 = v0.y;
        m02 = v0.z;
        m03 = v0.w;
        m10 = v1.x;
        m11 = v1.y;
        m12 = v1.z;
        m13 = v1.w;
        m20 = v2.x;
        m21 = v2.y;
        m22 = v2.z;
        m23 = v2.w;
        m30 = v3.x;
        m31 = v3.y;
        m32 = v3.z;
        m33 = v3.w;
    }

    inline JzMatrix<4, 4, T> &operator=(const JzMatrix<4, 4, T> &other)
    {
        std::memcpy(data, other.data, sizeof(data));
        return *this;
    }

    inline const T &At(U16 i, U16 j) const
    {
        return data[i * 4 + j];
    }

    inline T &At(U16 i, U16 j)
    {
        return data[i * 4 + j];
    }

    inline const T &operator()(U16 i, U16 j) const
    {
        return At(i, j);
    }

    inline T &operator()(U16 i, U16 j)
    {
        return At(i, j);
    }

    inline const T *operator[](U16 i) const
    {
        return data + i * 4;
    }

    inline T *operator[](U16 i)
    {
        return data + i * 4;
    }

    inline JzMatrix<4, 4, T> &operator+=(const JzMatrix<4, 4, T> &other)
    {
        for (U16 i = 0; i < 16; ++i) {
            data[i] += other.data[i];
        }
        return *this;
    }

    inline JzMatrix<4, 4, T> &operator-=(const JzMatrix<4, 4, T> &other)
    {
        for (U16 i = 0; i < 16; ++i) {
            data[i] -= other.data[i];
        }
        return *this;
    }

    inline JzMatrix<4, 4, T> &operator*=(const T &value)
    {
        for (U16 i = 0; i < 16; ++i) {
            data[i] *= value;
        }
        return *this;
    }

    inline JzMatrix<4, 4, T> &operator/=(const T &value)
    {
        for (U16 i = 0; i < 16; ++i) {
            data[i] /= value;
        }
        return *this;
    }

    inline JzMatrix<4, 4, T> operator+(const JzMatrix<4, 4, T> &other) const
    {
        JzMatrix<4, 4, T> result  = *this;
        result                   += other;
        return result;
    }

    inline JzMatrix<4, 4, T> operator-(const JzMatrix<4, 4, T> &other) const
    {
        JzMatrix<4, 4, T> result  = *this;
        result                   -= other;
        return result;
    }

    inline JzMatrix<4, 4, T> operator*(const JzMatrix<4, 4, T> &other) const
    {
        return JzMatrix<4, 4, T>(
            m00 * other.m00 + m01 * other.m10 + m02 * other.m20 + m03 * other.m30,
            m00 * other.m01 + m01 * other.m11 + m02 * other.m21 + m03 * other.m31,
            m00 * other.m02 + m01 * other.m12 + m02 * other.m22 + m03 * other.m32,
            m00 * other.m03 + m01 * other.m13 + m02 * other.m23 + m03 * other.m33,
            m10 * other.m00 + m11 * other.m10 + m12 * other.m20 + m13 * other.m30,
            m10 * other.m01 + m11 * other.m11 + m12 * other.m21 + m13 * other.m31,
            m10 * other.m02 + m11 * other.m12 + m12 * other.m22 + m13 * other.m32,
            m10 * other.m03 + m11 * other.m13 + m12 * other.m23 + m13 * other.m33,
            m20 * other.m00 + m21 * other.m10 + m22 * other.m20 + m23 * other.m30,
            m20 * other.m01 + m21 * other.m11 + m22 * other.m21 + m23 * other.m31,
            m20 * other.m02 + m21 * other.m12 + m22 * other.m22 + m23 * other.m32,
            m20 * other.m03 + m21 * other.m13 + m22 * other.m23 + m23 * other.m33,
            m30 * other.m00 + m31 * other.m10 + m32 * other.m20 + m33 * other.m30,
            m30 * other.m01 + m31 * other.m11 + m32 * other.m21 + m33 * other.m31,
            m30 * other.m02 + m31 * other.m12 + m32 * other.m22 + m33 * other.m32,
            m30 * other.m03 + m31 * other.m13 + m32 * other.m23 + m33 * other.m33);
    }

    /**
     * @brief Matrix-vector multiplication
     */
    inline JzVector<4, T> operator*(const JzVector<4, T> &v) const
    {
        return JzVector<4, T>(
            m00 * v.x + m01 * v.y + m02 * v.z + m03 * v.w,
            m10 * v.x + m11 * v.y + m12 * v.z + m13 * v.w,
            m20 * v.x + m21 * v.y + m22 * v.z + m23 * v.w,
            m30 * v.x + m31 * v.y + m32 * v.z + m33 * v.w);
    }

    inline JzMatrix<4, 4, T> operator*(const T &value) const
    {
        JzMatrix<4, 4, T> result  = *this;
        result                   *= value;
        return result;
    }

    inline JzMatrix<4, 4, T> operator/(const T &value) const
    {
        JzMatrix<4, 4, T> result  = *this;
        result                   /= value;
        return result;
    }

    inline JzMatrix<4, 4, T> Transpose() const
    {
        return JzMatrix<4, 4, T>(
            m00, m10, m20, m30,
            m01, m11, m21, m31,
            m02, m12, m22, m32,
            m03, m13, m23, m33);
    }

    /**
     * @brief Get row as a vector
     */
    inline JzVector<4, T> Row(U16 i) const
    {
        return JzVector<4, T>(data[i * 4], data[i * 4 + 1], data[i * 4 + 2], data[i * 4 + 3]);
    }

    /**
     * @brief Get column as a vector
     */
    inline JzVector<4, T> Column(U16 j) const
    {
        return JzVector<4, T>(data[j], data[4 + j], data[8 + j], data[12 + j]);
    }

    inline T *Data()
    {
        return data;
    }

    inline const T *Data() const
    {
        return data;
    }

    static JzMatrix<4, 4, T> Identity()
    {
        return JzMatrix<4, 4, T>(
            T(1), T(0), T(0), T(0),
            T(0), T(1), T(0), T(0),
            T(0), T(0), T(1), T(0),
            T(0), T(0), T(0), T(1));
    }

    static JzMatrix<4, 4, T> Translate(const JzVector<3, T> &v)
    {
        return JzMatrix<4, 4, T>(
            T(1), T(0), T(0), v.x,
            T(0), T(1), T(0), v.y,
            T(0), T(0), T(1), v.z,
            T(0), T(0), T(0), T(1));
    }

    static JzMatrix<4, 4, T> Scale(const JzVector<3, T> &v)
    {
        return JzMatrix<4, 4, T>(
            v.x, T(0), T(0), T(0),
            T(0), v.y, T(0), T(0),
            T(0), T(0), v.z, T(0),
            T(0), T(0), T(0), T(1));
    }

    static JzMatrix<4, 4, T> RotateX(T angle)
    {
        T c = std::cos(angle);
        T s = std::sin(angle);
        return JzMatrix<4, 4, T>(
            T(1), T(0), T(0), T(0),
            T(0), c, -s, T(0),
            T(0), s, c, T(0),
            T(0), T(0), T(0), T(1));
    }

    static JzMatrix<4, 4, T> RotateY(T angle)
    {
        T c = std::cos(angle);
        T s = std::sin(angle);
        return JzMatrix<4, 4, T>(
            c, T(0), s, T(0),
            T(0), T(1), T(0), T(0),
            -s, T(0), c, T(0),
            T(0), T(0), T(0), T(1));
    }

    static JzMatrix<4, 4, T> RotateZ(T angle)
    {
        T c = std::cos(angle);
        T s = std::sin(angle);
        return JzMatrix<4, 4, T>(
            c, -s, T(0), T(0),
            s, c, T(0), T(0),
            T(0), T(0), T(1), T(0),
            T(0), T(0), T(0), T(1));
    }

    static JzMatrix<4, 4, T> LookAt(const JzVector<3, T> &eye, const JzVector<3, T> &center, const JzVector<3, T> &up)
    {
        JzVector<3, T> z = (eye - center).Normalized();
        JzVector<3, T> x = up.Cross(z).Normalized();
        JzVector<3, T> y = z.Cross(x).Normalized();

        return JzMatrix<4, 4, T>(
            x.x, x.y, x.z, -x.Dot(eye),
            y.x, y.y, y.z, -y.Dot(eye),
            z.x, z.y, z.z, -z.Dot(eye),
            T(0), T(0), T(0), T(1));
    }

    static JzMatrix<4, 4, T> Perspective(T fov, T aspect, T zNear, T zFar)
    {
        T tanHalfFov = std::tan(fov / T(2));
        T zRange     = zNear - zFar;
        return JzMatrix<4, 4, T>(
            T(1) / (aspect * tanHalfFov), T(0), T(0), T(0),
            T(0), T(1) / tanHalfFov, T(0), T(0),
            T(0), T(0), -(-zNear - zFar) / zRange, T(2) * zFar * zNear / zRange,
            T(0), T(0), T(-1), T(0));
    }

    static JzMatrix<4, 4, T> Orthographic(T left, T right, T bottom, T top, T zNear, T zFar)
    {
        T width  = right - left;
        T height = top - bottom;
        T depth  = zFar - zNear;

        return JzMatrix<4, 4, T>(
            T(2) / width, T(0), T(0), -(right + left) / width,
            T(0), T(2) / height, T(0), -(top + bottom) / height,
            T(0), T(0), T(-2) / depth, -(zFar + zNear) / depth,
            T(0), T(0), T(0), T(1));
    }
};

using JzMat2   = JzMatrix<2, 2, F32>;
using JzMat3   = JzMatrix<3, 3, F32>;
using JzMat4   = JzMatrix<4, 4, F32>;
using JzMat2x2 = JzMatrix<2, 2, F32>;
using JzMat3x3 = JzMatrix<3, 3, F32>;
using JzMat4x4 = JzMatrix<4, 4, F32>;

} // namespace JzRE