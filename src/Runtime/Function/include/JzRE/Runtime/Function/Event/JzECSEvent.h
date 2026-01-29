/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <atomic>
#include <type_traits>
#include <utility>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Function/ECS/JzEntity.h"

namespace JzRE {

/**
 * @brief Base class for all ECS events
 */
struct JzECSEvent {
    virtual ~JzECSEvent() = default;
    U64      timestamp;               // Event timestamp
    JzEntity source = INVALID_ENTITY; // Event source entity
    JzEntity target = INVALID_ENTITY; // Event target entity
};

/**
 * @brief Compile-time Event Type ID generation
 */
template <typename T>
struct JzECSEventType {
    static U32 Id()
    {
        static_assert(std::is_base_of_v<JzECSEvent, T>, "T must inherit from JzECSEvent");
        static U32 id = typeIdCounter++;
        return id;
    }

private:
    static inline std::atomic<U32> typeIdCounter{0};
};

/**
 * @brief Type-safe Event Wrapper
 */
class JzECSEventWrapper {
public:
    template <typename T>
    JzECSEventWrapper(T &&event) :
        data(new T(std::forward<T>(event))),
        typeId(JzECSEventType<T>::Id()),
        deleter([](void *ptr) {
            delete static_cast<T *>(ptr);
        })
    { }

    JzECSEventWrapper() :
        data(nullptr),
        typeId(0),
        deleter(nullptr) { }

    // Move constructor
    JzECSEventWrapper(JzECSEventWrapper &&other) noexcept :
        data(other.data),
        typeId(other.typeId),
        deleter(other.deleter)
    {
        other.data    = nullptr;
        other.deleter = nullptr;
    }

    // Move assignment
    JzECSEventWrapper &operator=(JzECSEventWrapper &&other) noexcept
    {
        if (this != &other) {
            if (data && deleter) deleter(data);
            data          = other.data;
            typeId        = other.typeId;
            deleter       = other.deleter;
            other.data    = nullptr;
            other.deleter = nullptr;
        }
        return *this;
    }

    // Disable copy
    JzECSEventWrapper(const JzECSEventWrapper &)            = delete;
    JzECSEventWrapper &operator=(const JzECSEventWrapper &) = delete;

    ~JzECSEventWrapper()
    {
        if (data && deleter)
            deleter(data);
    }

    template <typename T>
    T *As() const
    {
        if (JzECSEventType<T>::Id() == typeId) {
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
