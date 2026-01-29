/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <atomic>
#include <type_traits>
#include <utility>

#include "JzRE/Runtime/Core/JzRETypes.h"

namespace JzRE {

/**
 * @brief Lightweight event base for Platform layer.
 *
 * No entity dependency - can be used in Platform layer.
 * Function layer converts these to JzREEvent with entity context.
 */
struct JzPlatformEvent {
    virtual ~JzPlatformEvent() = default;
    U64 timestamp{0};
    U32 windowId{0}; // Window identifier for multi-window support
};

/**
 * @brief Compile-time type ID for platform events
 */
template <typename T>
struct JzPlatformEventType {
    static U32 Id()
    {
        static_assert(std::is_base_of_v<JzPlatformEvent, T>,
                      "T must inherit from JzPlatformEvent");
        static U32 id = s_counter++;
        return id;
    }

private:
    static inline std::atomic<U32> s_counter{0};
};

/**
 * @brief Type-erased wrapper for platform events
 */
class JzPlatformEventWrapper {
public:
    template <typename T>
    JzPlatformEventWrapper(T &&event) :
        m_data(new T(std::forward<T>(event))),
        m_typeId(JzPlatformEventType<std::decay_t<T>>::Id()),
        m_deleter([](void *ptr) {
            delete static_cast<std::decay_t<T> *>(ptr);
        })
    { }

    JzPlatformEventWrapper() :
        m_data(nullptr),
        m_typeId(0),
        m_deleter(nullptr) { }

    // Move constructor
    JzPlatformEventWrapper(JzPlatformEventWrapper &&other) noexcept :
        m_data(other.m_data),
        m_typeId(other.m_typeId),
        m_deleter(other.m_deleter)
    {
        other.m_data    = nullptr;
        other.m_deleter = nullptr;
    }

    // Move assignment
    JzPlatformEventWrapper &operator=(JzPlatformEventWrapper &&other) noexcept
    {
        if (this != &other) {
            if (m_data && m_deleter) m_deleter(m_data);
            m_data          = other.m_data;
            m_typeId        = other.m_typeId;
            m_deleter       = other.m_deleter;
            other.m_data    = nullptr;
            other.m_deleter = nullptr;
        }
        return *this;
    }

    // Disable copy
    JzPlatformEventWrapper(const JzPlatformEventWrapper &)            = delete;
    JzPlatformEventWrapper &operator=(const JzPlatformEventWrapper &) = delete;

    ~JzPlatformEventWrapper()
    {
        if (m_data && m_deleter)
            m_deleter(m_data);
    }

    template <typename T>
    T *As() const
    {
        if (JzPlatformEventType<T>::Id() == m_typeId) {
            return static_cast<T *>(m_data);
        }
        return nullptr;
    }

    U32 GetTypeId() const
    {
        return m_typeId;
    }

    Bool IsValid() const
    {
        return m_data != nullptr;
    }

private:
    void *m_data;
    U32   m_typeId;
    void (*m_deleter)(void *);
};

} // namespace JzRE
