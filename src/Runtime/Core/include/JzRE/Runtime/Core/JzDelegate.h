/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <functional>
#include <unordered_map>
#include "JzRE/Runtime/Core/JzRETypes.h"

namespace JzRE {

/**
 * @brief The Listener ID for registering delegate callbacks
 */
using DelegateHandle = U64;

/**
 * @brief A Delegate class that contains a list of callbacks (Multicast Delegate)
 * @tparam ArgsTypes The types of the arguments of the delegate
 */
template <class... ArgsTypes>
class JzDelegate {
public:
    /**
     * @brief Callback type
     */
    using Callback = std::function<void(ArgsTypes...)>;

    /**
     * @brief Add a listener
     *
     * @param callback
     * @return DelegateHandle
     */
    DelegateHandle Add(Callback callback) {
        DelegateHandle handle = m_availableHandle++;
        m_listeners.emplace(handle, std::move(callback));
        return handle;
    }

    /**
     * @brief Remove a listener
     *
     * @param handle
     * @return Bool
     */
    Bool Remove(DelegateHandle handle) {
        return m_listeners.erase(handle) != 0;
    }

    /**
     * @brief Operator wrapper for Add
     */
    DelegateHandle operator+=(Callback callback) {
        return Add(std::move(callback));
    }

    /**
     * @brief Operator wrapper for Remove
     */
    Bool operator-=(DelegateHandle handle) {
        return Remove(handle);
    }

    /**
     * @brief Remove all listeners
     */
    void Clear() {
        m_listeners.clear();
        m_availableHandle = 0;
    }

    /**
     * @brief Get count
     */
    U64 Count() const {
        return m_listeners.size();
    }

    /**
     * @brief Broadcast to all listeners
     *
     * @param args
     */
    void Broadcast(ArgsTypes... args) {
        for (const auto &listener : m_listeners) {
            listener.second(args...);
        }
    }

private:
    std::unordered_map<DelegateHandle, Callback> m_listeners;
    DelegateHandle                               m_availableHandle = 0;
};

} // namespace JzRE
