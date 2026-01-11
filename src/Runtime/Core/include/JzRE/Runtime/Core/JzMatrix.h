/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <cmath>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"

namespace JzRE {

/**
 * @brief Matrix Class
 */
template <U16 M, U16 N, typename T = F32>
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

    // matrix-vector operations

    // 4*4 matrices only

    // protected:
    T m_Data[M * N];
};

using JzMat2 = JzMatrix<2, 2, F32>;
using JzMat3 = JzMatrix<3, 3, F32>;
using JzMat4 = JzMatrix<4, 4, F32>;

class JzMat4x4 : public JzMatrix<4, 4, F32> {
public:
    inline JzMat4x4()
    {
        std::memset(m_Data, 0, sizeof(m_Data));
        m_Data[0] = m_Data[5] = m_Data[10] = m_Data[15] = 1.0f;
    }

    inline JzMat4x4(
        F32 m00, F32 m01, F32 m02, F32 m03,
        F32 m10, F32 m11, F32 m12, F32 m13,
        F32 m20, F32 m21, F32 m22, F32 m23,
        F32 m30, F32 m31, F32 m32, F32 m33)
    {
        this->At(0, 0) = m00;
        this->At(0, 1) = m01;
        this->At(0, 2) = m02;
        this->At(0, 3) = m03;
        this->At(1, 0) = m10;
        this->At(1, 1) = m11;
        this->At(1, 2) = m12;
        this->At(1, 3) = m13;
        this->At(2, 0) = m20;
        this->At(2, 1) = m21;
        this->At(2, 2) = m22;
        this->At(2, 3) = m23;
        this->At(3, 0) = m30;
        this->At(3, 1) = m31;
        this->At(3, 2) = m32;
        this->At(3, 3) = m33;
    }

    inline JzMat4x4(const JzVector<4, F32> &v0, const JzVector<4, F32> &v1, const JzVector<4, F32> &v2, const JzVector<4, F32> &v3)
    {
        this->At(0, 0) = v0[0];
        this->At(0, 1) = v0[1];
        this->At(0, 2) = v0[2];
        this->At(0, 3) = v0[3];
        this->At(1, 0) = v1[0];
        this->At(1, 1) = v1[1];
        this->At(1, 2) = v1[2];
        this->At(1, 3) = v1[3];
        this->At(2, 0) = v2[0];
        this->At(2, 1) = v2[1];
        this->At(2, 2) = v2[2];
        this->At(2, 3) = v2[3];
        this->At(3, 0) = v3[0];
        this->At(3, 1) = v3[1];
        this->At(3, 2) = v3[2];
        this->At(3, 3) = v3[3];
    }

    inline JzMat4x4 Mul(const JzMatrix<4, 4, F32> &other) const
    {
        JzMat4x4 result;
        for (U16 i = 0; i < 4; ++i) {
            for (U16 j = 0; j < 4; ++j) {
                result.At(i, j) = this->At(i, 0) * other.At(0, j) + this->At(i, 1) * other.At(1, j) + this->At(i, 2) * other.At(2, j) + this->At(i, 3) * other.At(3, j);
            }
        }
        return result;
    }

    inline JzVector<4, F32> Mul(const JzVector<4, F32> &v) const
    {
        JzVec4 result;
        for (U16 i = 0; i < 4; ++i) {
            result[i] = this->At(i, 0) * v[0] + this->At(i, 1) * v[1] + this->At(i, 2) * v[2] + this->At(i, 3) * v[3];
        }
        return result;
    }

    static JzMat4x4 Identity()
    {
        return JzMat4x4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);
    }

    static JzMat4x4 Translate(const JzVector<3, F32> &v)
    {
        return JzMat4x4(
            1.0f, 0.0f, 0.0f, v[0],
            0.0f, 1.0f, 0.0f, v[1],
            0.0f, 0.0f, 1.0f, v[2],
            0.0f, 0.0f, 0.0f, 1.0f);
    }

    static JzMat4x4 Scale(const JzVector<3, F32> &v)
    {
        return JzMat4x4(
            v[0], 0.0f, 0.0f, 0.0f,
            0.0f, v[1], 0.0f, 0.0f,
            0.0f, 0.0f, v[2], 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);
    }

    static JzMat4x4 RotateX(F32 angle)
    {
        F32 c = std::cos(angle);
        F32 s = std::sin(angle);
        return JzMat4x4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, c, -s, 0.0f,
            0.0f, s, c, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);
    }

    static JzMat4x4 RotateY(F32 angle)
    {
        F32 c = std::cos(angle);
        F32 s = std::sin(angle);
        return JzMat4x4(
            c, 0.0f, s, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            -s, 0.0f, c, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);
    }

    static JzMat4x4 RotateZ(F32 angle)
    {
        F32 c = std::cos(angle);
        F32 s = std::sin(angle);
        return JzMat4x4(
            c, -s, 0.0f, 0.0f,
            s, c, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);
    }

    static JzMat4x4 LookAt(const JzVector<3, F32> &eye, const JzVector<3, F32> &center, const JzVector<3, F32> &up)
    {
        JzVector<3, F32> z = (eye - center).Normalized();
        JzVector<3, F32> x = up.Cross(z).Normalized();
        JzVector<3, F32> y = z.Cross(x).Normalized();

        return JzMat4x4(
            x[0], x[1], x[2], -x.Dot(eye),
            y[0], y[1], y[2], -y.Dot(eye),
            z[0], z[1], z[2], -z.Dot(eye),
            0.0f, 0.0f, 0.0f, 1.0f);
    }

    static JzMat4x4 Perspective(F32 fov, F32 aspect, F32 zNear, F32 zFar)
    {
        F32 tanHalfFov = std::tan(fov / 2.0f);
        F32 zRange     = zNear - zFar;
        return JzMat4x4(
            1.0f / (aspect * tanHalfFov), 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f / tanHalfFov, 0.0f, 0.0f,
            0.0f, 0.0f, -(-zNear - zFar) / zRange, 2.0f * zFar * zNear / zRange,
            0.0f, 0.0f, -1.0f, 0.0f);
    }

    static JzMat4x4 Orthographics(F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar)
    {
        F32 width  = right - left;
        F32 height = top - bottom;
        F32 depth  = zFar - zNear;

        return JzMat4x4(
            2.0f / width, 0.0f, 0.0f, -(right + left) / width,
            0.0f, 2.0f / height, 0.0f, -(top + bottom) / height,
            0.0f, 0.0f, -2.0f / depth, -(zFar + zNear) / depth,
            0.0f, 0.0f, 0.0f, 1.0f);
    }
};

} // namespace JzRE