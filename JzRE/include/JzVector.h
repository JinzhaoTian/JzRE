#pragma once

#include "CommonTypes.h"

namespace JzRE {

template <typename T>
concept VectorValueType = std::is_arithmetic_v<T>;

template <U16 N, typename T = F32>
class JzVector {
public:
    // constructors
    inline JzVector(const T &value)
    {
        for (U16 i = 0; i < N; ++i) {
            m_Data[i] = value;
        }
    }

    template <U16 M, VectorValueType... Args>
    inline JzVector(const JzVector<M> &other, Args... args)
    {
        if constexpr (sizeof...(args) == 0) {
            std::memcpy(m_Data, other.m_Data, std::clamp(M, U16(0), N) * sizeof(T));
        } else if constexpr ((M + sizeof...(args)) >= N) {
            std::memcpy(m_Data, other.m_Data, std::clamp(M, U16(0), N) * sizeof(T));
            T data[] = {args...};
            std::memcpy(m_Data + M, data, sizeof(data));
        } else {
            std::memcpy(m_Data, other.m_Data, std::clamp(M, U16(0), N) * sizeof(T));
            T data[] = {args...};
            std::memcpy(m_Data + M, data, sizeof(data));

            for (U16 i = M + sizeof...(args); i < N; ++i) {
                m_Data[i] = T();
            }
        }
    }

    template <VectorValueType... Args>
    inline JzVector(Args... args)
    {
        static_assert(sizeof...(args) <= N, "Invalid number of arguments");
        if constexpr (sizeof...(args) == 0) {
            for (U16 i = 0; i < N; ++i) {
                m_Data[i] = T();
            }
        } else {
            T data[] = {static_cast<T>(args)...};
            std::memcpy(m_Data, data, sizeof(data));
        }
    }

    inline JzVector(const T *values)
    {
        std::memcpy(m_Data, values, N * sizeof(T));
    }

    template <typename U>
    inline JzVector(const JzVector<N, U> &source_)
    {
        (*this) = source_;
    }

    inline JzVector<N, T> &operator=(const JzVector<N, T> &other)
    {
        std::memcpy(m_Data, other.m_Data, sizeof(m_Data));
        return *this;
    }

    inline T &operator[](U16 index)
    {
        return m_Data[index];
    }

    inline const T &operator[](U16 index) const
    {
        return m_Data[index];
    }

    inline JzVector<N, T> operator+(const JzVector<N, T> &other) const
    {
        JzVector<N, T> result;
        for (U16 i = 0; i < N; ++i) {
            result[i] = m_Data[i] + other[i];
        }
        return result;
    }

    inline JzVector<N, T> operator-(const JzVector<N, T> &other) const
    {
        JzVector<N, T> result;
        for (U16 i = 0; i < N; ++i) {
            result[i] = m_Data[i] - other[i];
        }
        return result;
    }

    inline JzVector<N, T> operator*(const JzVector<N, T> &other) const
    {
        JzVector<N, T> result;
        for (U16 i = 0; i < N; ++i) {
            result[i] = m_Data[i] * other[i];
        }
        return result;
    }

    inline JzVector<N, T> operator/(const JzVector<N, T> &other) const
    {
        JzVector<N, T> result;
        for (U16 i = 0; i < N; ++i) {
            result[i] = m_Data[i] / other[i];
        }
        return result;
    }

    inline JzVector<N, T> operator*(const T &value) const
    {
        JzVector<N, T> result;
        for (U16 i = 0; i < N; ++i) {
            result[i] = m_Data[i] * value;
        }
        return result;
    }

    inline JzVector<N, T> operator/(const T &value) const
    {
        JzVector<N, T> result;
        for (U16 i = 0; i < N; ++i) {
            result[i] = m_Data[i] / value;
        }
        return result;
    }

    inline JzVector<N, T> &operator+=(const JzVector<N, T> &other)
    {
        for (U16 i = 0; i < N; ++i) {
            m_Data[i] += other[i];
        }
        return *this;
    }

    inline JzVector<N, T> &operator-=(const JzVector<N, T> &other)
    {
        for (U16 i = 0; i < N; ++i) {
            m_Data[i] -= other[i];
        }
        return *this;
    }

    inline JzVector<N, T> &operator*=(const JzVector<N, T> &other)
    {
        for (U16 i = 0; i < N; ++i) {
            m_Data[i] *= other[i];
        }
        return *this;
    }

    inline JzVector<N, T> &operator/=(const JzVector<N, T> &other)
    {
        for (U16 i = 0; i < N; ++i) {
            m_Data[i] /= other[i];
        }
        return *this;
    }

    inline JzVector<N, T> &operator*=(const T &value)
    {
        for (U16 i = 0; i < N; ++i) {
            m_Data[i] *= value;
        }
        return *this;
    }

    inline JzVector<N, T> &operator/=(const T &value)
    {
        for (U16 i = 0; i < N; ++i) {
            m_Data[i] /= value;
        }
        return *this;
    }

    inline bool operator==(const JzVector<N, T> &other) const
    {
        for (U16 i = 0; i < N; ++i) {
            if ((m_Data[i] - other[i]) > std::numeric_limits<T>::epsilon()) {
                return false;
            }
        }
        return true;
    }

    inline JzVector<N, T> operator-() const
    {
        JzVector<N, T> result;
        for (U16 i = 0; i < N; ++i) {
            result[i] = -m_Data[i];
        }
        return result;
    }

    inline bool operator!=(const JzVector<N, T> &other) const
    {
        return !(*this == other);
    }

    inline T Length() const
    {
        T result = 0;
        for (U16 i = 0; i < N; ++i) {
            result += m_Data[i] * m_Data[i];
        }
        return static_cast<T>(std::sqrt(result));
    }

    inline T LengthSquared() const
    {
        T result = 0;
        for (U16 i = 0; i < N; ++i) {
            result += m_Data[i] * m_Data[i];
        }
        return result;
    }

    inline T Dot(const JzVector<N, T> &other) const
    {
        T result = 0;
        for (U16 i = 0; i < N; ++i) {
            result += m_Data[i] * other[i];
        }
        return result;
    }

    inline JzVector<N, T> Pow(F32 power) const
    {
        JzVector<N, T> result;
        for (U16 i = 0; i < N; ++i) {
            result[i] = std::pow(m_Data[i], power);
        }
        return result;
    }

    inline JzVector<N, T> Cross(const JzVector<N, T> &other) const
    {
        static_assert(N == 3 || N == 2, "Cross product is only defined for 3D vectors");
        if (N == 2) {
            F32 result = m_Data[0] * other[1] - m_Data[1] * other[0];
            return JzVector<2, T>(result, 0.0f);
        } else if (N == 3) {
            return JzVector<3, T>(
                m_Data[1] * other[2] - m_Data[2] * other[1],
                m_Data[2] * other[0] - m_Data[0] * other[2],
                m_Data[0] * other[1] - m_Data[1] * other[0]);
        }
    }

    inline JzVector<N, T> Normalized() const
    {
        return *this / Length();
    }

    inline void Normalize()
    {
        *this /= Length();
    }

    inline const JzVector &negate()
    {
        for (U16 index = 0; index < N; ++index)
            m_Data[index] = -m_Data[index];
        return *this;
    }

    // element accessors for M <= 4;
    inline T &x()
    {
        return m_Data[0];
    }

    inline T &y()
    {
        return m_Data[1];
    }

    inline T &z()
    {
        return m_Data[2];
    }

    inline T &w()
    {
        return m_Data[3];
    }

    inline const T &x() const
    {
        return m_Data[0];
    }

    inline const T &y() const
    {
        return m_Data[1];
    }

    inline const T &z() const
    {
        return m_Data[2];
    }

    inline const T &w() const
    {
        return m_Data[3];
    }

    // private:
    T m_Data[N];
};

using JzVec2 = JzVector<2, F32>;
using JzVec3 = JzVector<3, F32>;
using JzVec4 = JzVector<4, F32>;

using JzIVec2 = JzVector<2, I32>;
using JzIVec3 = JzVector<3, I32>;
using JzIVec4 = JzVector<4, I32>;

} // namespace JzRE