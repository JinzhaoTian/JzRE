/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <cmath>
#include <type_traits>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include "JzRE/Runtime/Core/JzRETypes.h"

namespace JzRE {

/**
 * @brief Concept of Vector Value Type
 */
template <typename T>
concept VectorValueType = std::is_arithmetic_v<T>;

// Forward declaration
template <U16 N, typename T = F32>
class JzVector;

/**
 * @brief Base Vector Class (for N > 4)
 *
 * This is the generic template for vectors with dimensions greater than 4.
 * For dimensions 2, 3, and 4, specialized templates with union-based member access are provided.
 */
template <U16 N, typename T>
class JzVector {
public:
    /**
     * @brief Constructor
     *
     * @param value The value to initialize the vector with
     */
    inline JzVector(const T &value)
    {
        for (U16 i = 0; i < N; ++i) {
            m_Data[i] = value;
        }
    }

    /**
     * @brief Constructor
     *
     * @tparam M The size of the vector
     * @tparam Args The arguments to initialize the vector with
     * @param other The other vector to initialize the vector with
     * @param args The arguments to initialize the vector with
     */
    template <U16 M, VectorValueType... Args>
    inline JzVector(const JzVector<M> &other, Args... args)
    {
        if constexpr (sizeof...(args) == 0) {
            std::memcpy(m_Data, &other[0], std::clamp(M, U16(0), N) * sizeof(T));
        } else if constexpr ((M + sizeof...(args)) >= N) {
            std::memcpy(m_Data, &other[0], std::clamp(M, U16(0), N) * sizeof(T));
            T data[] = {args...};
            std::memcpy(m_Data + M, data, sizeof(data));
        } else {
            std::memcpy(m_Data, &other[0], std::clamp(M, U16(0), N) * sizeof(T));
            T data[] = {args...};
            std::memcpy(m_Data + M, data, sizeof(data));

            for (U16 i = M + sizeof...(args); i < N; ++i) {
                m_Data[i] = T();
            }
        }
    }

    /**
     * @brief Constructor
     *
     * @tparam Args The arguments to initialize the vector with
     * @param args The arguments to initialize the vector with
     */
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

    /**
     * @brief Constructor
     *
     * @param values The values to initialize the vector with
     */
    inline JzVector(const T *values)
    {
        std::memcpy(m_Data, values, N * sizeof(T));
    }

    /**
     * @brief Constructor
     *
     * @tparam U The type of the source vector
     * @param source_ The source vector to initialize the vector with
     */
    template <typename U>
    inline JzVector(const JzVector<N, U> &source_)
    {
        (*this) = source_;
    }

    inline JzVector<N, T> &operator=(const JzVector<N, T> &other)
    {
        std::memcpy(m_Data, &other[0], sizeof(m_Data));
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
            if constexpr (std::is_floating_point_v<T>) {
                if (std::abs(m_Data[i] - other[i]) > std::numeric_limits<T>::epsilon()) {
                    return false;
                }
            } else {
                if (m_Data[i] != other[i]) {
                    return false;
                }
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
    T m_Data[N];
};

/**
 * @brief 2D Vector Specialization with union-based member access
 *
 * Provides direct member access via x, y (or u, v for texture coordinates).
 */
template <typename T>
class JzVector<2, T> {
public:
    union {
        struct {
            T x, y;
        };
        struct {
            T u, v;
        };
        struct {
            T r, g;
        };
        T data[2];
    };

    inline JzVector() :
        x(T()), y(T()) { }

    inline JzVector(const T &value) :
        x(value), y(value) { }

    inline JzVector(T x_, T y_) :
        x(x_), y(y_) { }

    inline JzVector(const T *values) :
        x(values[0]), y(values[1]) { }

    template <typename U>
    inline JzVector(const JzVector<2, U> &source) :
        x(static_cast<T>(source.x)), y(static_cast<T>(source.y))
    { }

    inline JzVector<2, T> &operator=(const JzVector<2, T> &other)
    {
        x = other.x;
        y = other.y;
        return *this;
    }

    inline T &operator[](U16 index)
    {
        return data[index];
    }

    inline const T &operator[](U16 index) const
    {
        return data[index];
    }

    inline JzVector<2, T> operator+(const JzVector<2, T> &other) const
    {
        return JzVector<2, T>(x + other.x, y + other.y);
    }

    inline JzVector<2, T> operator-(const JzVector<2, T> &other) const
    {
        return JzVector<2, T>(x - other.x, y - other.y);
    }

    inline JzVector<2, T> operator*(const JzVector<2, T> &other) const
    {
        return JzVector<2, T>(x * other.x, y * other.y);
    }

    inline JzVector<2, T> operator/(const JzVector<2, T> &other) const
    {
        return JzVector<2, T>(x / other.x, y / other.y);
    }

    inline JzVector<2, T> operator*(const T &value) const
    {
        return JzVector<2, T>(x * value, y * value);
    }

    inline JzVector<2, T> operator/(const T &value) const
    {
        return JzVector<2, T>(x / value, y / value);
    }

    inline JzVector<2, T> &operator+=(const JzVector<2, T> &other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    inline JzVector<2, T> &operator-=(const JzVector<2, T> &other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    inline JzVector<2, T> &operator*=(const JzVector<2, T> &other)
    {
        x *= other.x;
        y *= other.y;
        return *this;
    }

    inline JzVector<2, T> &operator/=(const JzVector<2, T> &other)
    {
        x /= other.x;
        y /= other.y;
        return *this;
    }

    inline JzVector<2, T> &operator*=(const T &value)
    {
        x *= value;
        y *= value;
        return *this;
    }

    inline JzVector<2, T> &operator/=(const T &value)
    {
        x /= value;
        y /= value;
        return *this;
    }

    inline bool operator==(const JzVector<2, T> &other) const
    {
        if constexpr (std::is_floating_point_v<T>) {
            return std::abs(x - other.x) <= std::numeric_limits<T>::epsilon() && std::abs(y - other.y) <= std::numeric_limits<T>::epsilon();
        } else {
            return x == other.x && y == other.y;
        }
    }

    inline JzVector<2, T> operator-() const
    {
        return JzVector<2, T>(-x, -y);
    }

    inline bool operator!=(const JzVector<2, T> &other) const
    {
        return !(*this == other);
    }

    inline T Length() const
    {
        return static_cast<T>(std::sqrt(x * x + y * y));
    }

    inline T LengthSquared() const
    {
        return x * x + y * y;
    }

    inline T Dot(const JzVector<2, T> &other) const
    {
        return x * other.x + y * other.y;
    }

    inline JzVector<2, T> Pow(F32 power) const
    {
        return JzVector<2, T>(std::pow(x, power), std::pow(y, power));
    }

    /**
     * @brief 2D Cross product (returns the z-component of the 3D cross product)
     */
    inline T Cross(const JzVector<2, T> &other) const
    {
        return x * other.y - y * other.x;
    }

    inline JzVector<2, T> Normalized() const
    {
        return *this / Length();
    }

    inline void Normalize()
    {
        *this /= Length();
    }

    inline const JzVector<2, T> &negate()
    {
        x = -x;
        y = -y;
        return *this;
    }

    inline T *Data()
    {
        return data;
    }

    inline const T *Data() const
    {
        return data;
    }
};

/**
 * @brief 3D Vector Specialization with union-based member access
 *
 * Provides direct member access via x, y, z (or r, g, b for colors).
 */
template <typename T>
class JzVector<3, T> {
public:
    union {
        struct {
            T x, y, z;
        };
        struct {
            T r, g, b;
        };
        T data[3];
    };

    inline JzVector() :
        x(T()), y(T()), z(T()) { }

    inline JzVector(const T &value) :
        x(value), y(value), z(value) { }

    inline JzVector(T x_, T y_, T z_) :
        x(x_), y(y_), z(z_) { }

    inline JzVector(const T *values) :
        x(values[0]), y(values[1]), z(values[2]) { }

    /**
     * @brief Construct from a 2D vector and a z component
     */
    inline JzVector(const JzVector<2, T> &v2, T z_) :
        x(v2.x), y(v2.y), z(z_) { }

    template <typename U>
    inline JzVector(const JzVector<3, U> &source) :
        x(static_cast<T>(source.x)), y(static_cast<T>(source.y)), z(static_cast<T>(source.z))
    { }

    inline JzVector<3, T> &operator=(const JzVector<3, T> &other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }

    inline T &operator[](U16 index)
    {
        return data[index];
    }

    inline const T &operator[](U16 index) const
    {
        return data[index];
    }

    inline JzVector<3, T> operator+(const JzVector<3, T> &other) const
    {
        return JzVector<3, T>(x + other.x, y + other.y, z + other.z);
    }

    inline JzVector<3, T> operator-(const JzVector<3, T> &other) const
    {
        return JzVector<3, T>(x - other.x, y - other.y, z - other.z);
    }

    inline JzVector<3, T> operator*(const JzVector<3, T> &other) const
    {
        return JzVector<3, T>(x * other.x, y * other.y, z * other.z);
    }

    inline JzVector<3, T> operator/(const JzVector<3, T> &other) const
    {
        return JzVector<3, T>(x / other.x, y / other.y, z / other.z);
    }

    inline JzVector<3, T> operator*(const T &value) const
    {
        return JzVector<3, T>(x * value, y * value, z * value);
    }

    inline JzVector<3, T> operator/(const T &value) const
    {
        return JzVector<3, T>(x / value, y / value, z / value);
    }

    inline JzVector<3, T> &operator+=(const JzVector<3, T> &other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    inline JzVector<3, T> &operator-=(const JzVector<3, T> &other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    inline JzVector<3, T> &operator*=(const JzVector<3, T> &other)
    {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }

    inline JzVector<3, T> &operator/=(const JzVector<3, T> &other)
    {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        return *this;
    }

    inline JzVector<3, T> &operator*=(const T &value)
    {
        x *= value;
        y *= value;
        z *= value;
        return *this;
    }

    inline JzVector<3, T> &operator/=(const T &value)
    {
        x /= value;
        y /= value;
        z /= value;
        return *this;
    }

    inline bool operator==(const JzVector<3, T> &other) const
    {
        if constexpr (std::is_floating_point_v<T>) {
            return std::abs(x - other.x) <= std::numeric_limits<T>::epsilon() && std::abs(y - other.y) <= std::numeric_limits<T>::epsilon() && std::abs(z - other.z) <= std::numeric_limits<T>::epsilon();
        } else {
            return x == other.x && y == other.y && z == other.z;
        }
    }

    inline JzVector<3, T> operator-() const
    {
        return JzVector<3, T>(-x, -y, -z);
    }

    inline bool operator!=(const JzVector<3, T> &other) const
    {
        return !(*this == other);
    }

    inline T Length() const
    {
        return static_cast<T>(std::sqrt(x * x + y * y + z * z));
    }

    inline T LengthSquared() const
    {
        return x * x + y * y + z * z;
    }

    inline T Dot(const JzVector<3, T> &other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    inline JzVector<3, T> Pow(F32 power) const
    {
        return JzVector<3, T>(std::pow(x, power), std::pow(y, power), std::pow(z, power));
    }

    /**
     * @brief 3D Cross product
     */
    inline JzVector<3, T> Cross(const JzVector<3, T> &other) const
    {
        return JzVector<3, T>(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x);
    }

    inline JzVector<3, T> Normalized() const
    {
        return *this / Length();
    }

    inline void Normalize()
    {
        *this /= Length();
    }

    inline const JzVector<3, T> &negate()
    {
        x = -x;
        y = -y;
        z = -z;
        return *this;
    }

    inline T *Data()
    {
        return data;
    }

    inline const T *Data() const
    {
        return data;
    }
};

/**
 * @brief 4D Vector Specialization with union-based member access
 *
 * Provides direct member access via x, y, z, w (or r, g, b, a for colors).
 */
template <typename T>
class JzVector<4, T> {
public:
    union {
        struct {
            T x, y, z, w;
        };
        struct {
            T r, g, b, a;
        };
        T data[4];
    };

    inline JzVector() :
        x(T()), y(T()), z(T()), w(T()) { }

    inline JzVector(const T &value) :
        x(value), y(value), z(value), w(value) { }

    inline JzVector(T x_, T y_, T z_, T w_) :
        x(x_), y(y_), z(z_), w(w_) { }

    inline JzVector(const T *values) :
        x(values[0]), y(values[1]), z(values[2]), w(values[3]) { }

    /**
     * @brief Construct from a 3D vector and a w component
     */
    inline JzVector(const JzVector<3, T> &v3, T w_) :
        x(v3.x), y(v3.y), z(v3.z), w(w_) { }

    /**
     * @brief Construct from a 2D vector and z, w components
     */
    inline JzVector(const JzVector<2, T> &v2, T z_, T w_) :
        x(v2.x), y(v2.y), z(z_), w(w_) { }

    template <typename U>
    inline JzVector(const JzVector<4, U> &source) :
        x(static_cast<T>(source.x)), y(static_cast<T>(source.y)), z(static_cast<T>(source.z)), w(static_cast<T>(source.w))
    { }

    inline JzVector<4, T> &operator=(const JzVector<4, T> &other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
        w = other.w;
        return *this;
    }

    inline T &operator[](U16 index)
    {
        return data[index];
    }

    inline const T &operator[](U16 index) const
    {
        return data[index];
    }

    inline JzVector<4, T> operator+(const JzVector<4, T> &other) const
    {
        return JzVector<4, T>(x + other.x, y + other.y, z + other.z, w + other.w);
    }

    inline JzVector<4, T> operator-(const JzVector<4, T> &other) const
    {
        return JzVector<4, T>(x - other.x, y - other.y, z - other.z, w - other.w);
    }

    inline JzVector<4, T> operator*(const JzVector<4, T> &other) const
    {
        return JzVector<4, T>(x * other.x, y * other.y, z * other.z, w * other.w);
    }

    inline JzVector<4, T> operator/(const JzVector<4, T> &other) const
    {
        return JzVector<4, T>(x / other.x, y / other.y, z / other.z, w / other.w);
    }

    inline JzVector<4, T> operator*(const T &value) const
    {
        return JzVector<4, T>(x * value, y * value, z * value, w * value);
    }

    inline JzVector<4, T> operator/(const T &value) const
    {
        return JzVector<4, T>(x / value, y / value, z / value, w / value);
    }

    inline JzVector<4, T> &operator+=(const JzVector<4, T> &other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }

    inline JzVector<4, T> &operator-=(const JzVector<4, T> &other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }

    inline JzVector<4, T> &operator*=(const JzVector<4, T> &other)
    {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        w *= other.w;
        return *this;
    }

    inline JzVector<4, T> &operator/=(const JzVector<4, T> &other)
    {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        w /= other.w;
        return *this;
    }

    inline JzVector<4, T> &operator*=(const T &value)
    {
        x *= value;
        y *= value;
        z *= value;
        w *= value;
        return *this;
    }

    inline JzVector<4, T> &operator/=(const T &value)
    {
        x /= value;
        y /= value;
        z /= value;
        w /= value;
        return *this;
    }

    inline bool operator==(const JzVector<4, T> &other) const
    {
        if constexpr (std::is_floating_point_v<T>) {
            return std::abs(x - other.x) <= std::numeric_limits<T>::epsilon() && std::abs(y - other.y) <= std::numeric_limits<T>::epsilon() && std::abs(z - other.z) <= std::numeric_limits<T>::epsilon() && std::abs(w - other.w) <= std::numeric_limits<T>::epsilon();
        } else {
            return x == other.x && y == other.y && z == other.z && w == other.w;
        }
    }

    inline JzVector<4, T> operator-() const
    {
        return JzVector<4, T>(-x, -y, -z, -w);
    }

    inline bool operator!=(const JzVector<4, T> &other) const
    {
        return !(*this == other);
    }

    inline T Length() const
    {
        return static_cast<T>(std::sqrt(x * x + y * y + z * z + w * w));
    }

    inline T LengthSquared() const
    {
        return x * x + y * y + z * z + w * w;
    }

    inline T Dot(const JzVector<4, T> &other) const
    {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }

    inline JzVector<4, T> Pow(F32 power) const
    {
        return JzVector<4, T>(std::pow(x, power), std::pow(y, power), std::pow(z, power), std::pow(w, power));
    }

    inline JzVector<4, T> Normalized() const
    {
        return *this / Length();
    }

    inline void Normalize()
    {
        *this /= Length();
    }

    inline const JzVector<4, T> &negate()
    {
        x = -x;
        y = -y;
        z = -z;
        w = -w;
        return *this;
    }

    /**
     * @brief Get the xyz components as a 3D vector
     */
    inline JzVector<3, T> xyz() const
    {
        return JzVector<3, T>(x, y, z);
    }

    /**
     * @brief Get the xy components as a 2D vector
     */
    inline JzVector<2, T> xy() const
    {
        return JzVector<2, T>(x, y);
    }

    inline T *Data()
    {
        return data;
    }

    inline const T *Data() const
    {
        return data;
    }
};

using JzVec2 = JzVector<2, F32>;
using JzVec3 = JzVector<3, F32>;
using JzVec4 = JzVector<4, F32>;

using JzIVec2 = JzVector<2, I32>;
using JzIVec3 = JzVector<3, I32>;
using JzIVec4 = JzVector<4, I32>;

} // namespace JzRE