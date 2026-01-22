/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
#include <entt/entt.hpp>
#include "JzRE/Runtime/Core/JzRETypes.h"

namespace JzRE {

// Using entt::entity for Entity
using Entity = entt::entity;

/**
 * @brief Base class for all ECS events
 */
struct JzBaseEvent {
    virtual ~JzBaseEvent() = default;
    U64    timestamp; // Event timestamp
    Entity source{entt::null};   // Event source entity
    Entity target{entt::null};   // Event target entity
};

/**
 * @brief Compile-time Event Type ID generation
 */
template <typename T>
struct JzEventType {
    static constexpr U32 Id() {
        static_assert(std::is_base_of_v<JzBaseEvent, T>, "T must inherit from JzBaseEvent");
        static U32 id = typeIdCounter++;
        return id;
    }

private:
    static inline std::atomic<U32> typeIdCounter{0};
};

/**
 * @brief Type-safe Event Wrapper
 */
class JzEventWrapper {
public:
    template <typename T>
    JzEventWrapper(T &&event)
        : data(new T(std::forward<T>(event))), typeId(JzEventType<T>::Id()), deleter([](void *ptr) { delete static_cast<T *>(ptr); })
    {}

    JzEventWrapper() : data(nullptr), typeId(0), deleter(nullptr) {}
    
    // Move constructor
    JzEventWrapper(JzEventWrapper &&other) noexcept
        : data(other.data), typeId(other.typeId), deleter(other.deleter)
    {
        other.data = nullptr;
        other.deleter = nullptr;
    }

    // Move assignment
    JzEventWrapper &operator=(JzEventWrapper &&other) noexcept
    {
        if (this != &other) {
            if (data && deleter) deleter(data);
            data = other.data;
            typeId = other.typeId;
            deleter = other.deleter;
            other.data = nullptr;
            other.deleter = nullptr;
        }
        return *this;
    }

    // Disable copy
    JzEventWrapper(const JzEventWrapper &) = delete;
    JzEventWrapper &operator=(const JzEventWrapper &) = delete;

    ~JzEventWrapper()
    {
        if (data && deleter) deleter(data);
    }

    template <typename T>
    T *As() const
    {
        if (JzEventType<T>::Id() == typeId) {
            return static_cast<T *>(data);
        }
        return nullptr;
    }

    U32 GetTypeId() const
    {
        return typeId;
    }

private:
    void *data;
    U32   typeId;
    void (*deleter)(void *);
};

} // namespace JzRE
