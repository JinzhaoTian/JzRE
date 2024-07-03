#pragma once

#include "CommonTypes.h"

namespace JzRE {

template <typename T>
concept VectorValueType = std::is_arithmetic_v<T>;

template <Size N, typename T = F32>
class Vector {
public:
    // constructors
    inline Vector(const T &value) {
        for (Size i = 0; i < N; ++i) {
            m_Data[i] = value;
        }
    }

    template <Size M, VectorValueType... Args>
    inline Vector(const Vector<M> &other, Args... args) {
        if constexpr (sizeof...(args) == 0) {
            std::memcpy(m_Data, other.m_Data, std::clamp(M, Size(0), N) * sizeof(T));
        } else if constexpr ((M + sizeof...(args)) >= N) {
            std::memcpy(m_Data, other.m_Data, std::clamp(M, Size(0), N) * sizeof(T));
            T data[] = {args...};
            std::memcpy(m_Data + M, data, sizeof(data));
        } else {
            std::memcpy(m_Data, other.m_Data, std::clamp(M, Size(0), N) * sizeof(T));
            T data[] = {args...};
            std::memcpy(m_Data + M, data, sizeof(data));

            for (Size i = M + sizeof...(args); i < N; ++i) {
                m_Data[i] = T();
            }
        }
    }

    template <VectorValueType... Args>
    inline Vector(Args... args) {
        static_assert(sizeof...(args) <= N, "Invalid number of arguments");
        if constexpr (sizeof...(args) == 0) {
            for (Size i = 0; i < N; ++i) {
                m_Data[i] = T();
            }
        } else {
            T data[] = {static_cast<T>(args)...};
            std::memcpy(m_Data, data, sizeof(data));
        }
    }

    inline Vector(const T *values) {
        std::memcpy(m_Data, values, N * sizeof(T));
    }

    template <typename U>
    inline Vector(const Vector<N, U> &source_) {
        (*this) = source_;
    }

    inline Vector<N, T> &operator=(const Vector<N, T> &other) {
        std::memcpy(m_Data, other.m_Data, sizeof(m_Data));
        return *this;
    }

    inline T &operator[](Size index) {
        return m_Data[index];
    }

    inline const T &operator[](Size index) const {
        return m_Data[index];
    }

    inline Vector<N, T> operator+(const Vector<N, T> &other) const {
        Vector<N, T> result;
        for (Size i = 0; i < N; ++i) {
            result[i] = m_Data[i] + other[i];
        }
        return result;
    }

    inline Vector<N, T> operator-(const Vector<N, T> &other) const {
        Vector<N, T> result;
        for (Size i = 0; i < N; ++i) {
            result[i] = m_Data[i] - other[i];
        }
        return result;
    }

    inline Vector<N, T> operator*(const Vector<N, T> &other) const {
        Vector<N, T> result;
        for (Size i = 0; i < N; ++i) {
            result[i] = m_Data[i] * other[i];
        }
        return result;
    }

    inline Vector<N, T> operator/(const Vector<N, T> &other) const {
        Vector<N, T> result;
        for (Size i = 0; i < N; ++i) {
            result[i] = m_Data[i] / other[i];
        }
        return result;
    }

    inline Vector<N, T> operator*(const T &value) const {
        Vector<N, T> result;
        for (Size i = 0; i < N; ++i) {
            result[i] = m_Data[i] * value;
        }
        return result;
    }

    inline Vector<N, T> operator/(const T &value) const {
        Vector<N, T> result;
        for (Size i = 0; i < N; ++i) {
            result[i] = m_Data[i] / value;
        }
        return result;
    }

    inline Vector<N, T> &operator+=(const Vector<N, T> &other) {
        for (Size i = 0; i < N; ++i) {
            m_Data[i] += other[i];
        }
        return *this;
    }

    inline Vector<N, T> &operator-=(const Vector<N, T> &other) {
        for (Size i = 0; i < N; ++i) {
            m_Data[i] -= other[i];
        }
        return *this;
    }

    inline Vector<N, T> &operator*=(const Vector<N, T> &other) {
        for (Size i = 0; i < N; ++i) {
            m_Data[i] *= other[i];
        }
        return *this;
    }

    inline Vector<N, T> &operator/=(const Vector<N, T> &other) {
        for (Size i = 0; i < N; ++i) {
            m_Data[i] /= other[i];
        }
        return *this;
    }

    inline Vector<N, T> &operator*=(const T &value) {
        for (Size i = 0; i < N; ++i) {
            m_Data[i] *= value;
        }
        return *this;
    }

    inline Vector<N, T> &operator/=(const T &value) {
        for (Size i = 0; i < N; ++i) {
            m_Data[i] /= value;
        }
        return *this;
    }

    inline bool operator==(const Vector<N, T> &other) const {
        for (Size i = 0; i < N; ++i) {
            if ((m_Data[i] - other[i]) > std::numeric_limits<T>::epsilon()) {
                return false;
            }
        }
        return true;
    }

    inline Vector<N, T> operator-() const {
        Vector<N, T> result;
        for (Size i = 0; i < N; ++i) {
            result[i] = -m_Data[i];
        }
        return result;
    }

    inline bool operator!=(const Vector<N, T> &other) const {
        return !(*this == other);
    }

    inline T Length() const {
        T result = 0;
        for (Size i = 0; i < N; ++i) {
            result += m_Data[i] * m_Data[i];
        }
        return static_cast<T>(std::sqrt(result));
    }

    inline T LengthSquared() const {
        T result = 0;
        for (Size i = 0; i < N; ++i) {
            result += m_Data[i] * m_Data[i];
        }
        return result;
    }

    inline T Dot(const Vector<N, T> &other) const {
        T result = 0;
        for (Size i = 0; i < N; ++i) {
            result += m_Data[i] * other[i];
        }
        return result;
    }

    inline Vector<N, T> Pow(F32 power) const {
        Vector<N, T> result;
        for (Size i = 0; i < N; ++i) {
            result[i] = std::pow(m_Data[i], power);
        }
        return result;
    }

    inline Vector<N, T> Cross(const Vector<N, T> &other) const {
        static_assert(N == 3 || N == 2, "Cross product is only defined for 3D vectors");
        if (N == 2) {
            F32 result = m_Data[0] * other[1] - m_Data[1] * other[0];
            return Vector<2, T>(result, 0.0f);
        } else if (N == 3) {
            return Vector<3, T>(
                m_Data[1] * other[2] - m_Data[2] * other[1],
                m_Data[2] * other[0] - m_Data[0] * other[2],
                m_Data[0] * other[1] - m_Data[1] * other[0]);
        }
    }

    inline Vector<N, T> Normalized() const {
        return *this / Length();
    }

    inline void Normalize() {
        *this /= Length();
    }

    inline const Vector &negate() {
        for (Size index = 0; index < N; ++index)
            m_Data[index] = -m_Data[index];
        return *this;
    }

    // element accessors for M <= 4;
    inline T &x() {
        return m_Data[0];
    }

    inline T &y() {
        return m_Data[1];
    }

    inline T &z() {
        return m_Data[2];
    }

    inline T &w() {
        return m_Data[3];
    }

    inline const T &x() const {
        return m_Data[0];
    }

    inline const T &y() const {
        return m_Data[1];
    }

    inline const T &z() const {
        return m_Data[2];
    }

    inline const T &w() const {
        return m_Data[3];
    }

    // private:
    T m_Data[N];
};

using Vec2 = Vector<2, F32>;
using Vec3 = Vector<3, F32>;
using Vec4 = Vector<4, F32>;

using IVec2 = Vector<2, I32>;
using IVec3 = Vector<3, I32>;
using IVec4 = Vector<4, I32>;

} // namespace Jzre