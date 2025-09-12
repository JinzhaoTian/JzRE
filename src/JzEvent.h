#pragma once

#include <functional>
#include "JzRETypes.h"

namespace JzRE {

/**
 * @brief The Listener ID for registering events callbacks
 */
using ListenerID = U64;

/**
 * @brief A Event class that contains a list of callbacks
 * @tparam ArgsTypes The types of the arguments of the event
 */
template <class... ArgsTypes>
class JzEvent {
public:
    /**
     * @brief Callback type for the event
     */
    using Callback = std::function<void(ArgsTypes...)>;

    /**
     * @brief Add a listener to this event
     *
     * @param callback
     * @return ListenerID
     */
    ListenerID AddListener(Callback callback);

    /**
     * @brief Remove a listener from this event
     *
     * @param listenerID
     * @return Bool
     */
    Bool RemoveListener(ListenerID listenerID);

    /**
     * @brief Add a listener to this event
     *
     * @param callback
     * @return ListenerID
     */
    ListenerID operator+=(Callback callback);

    /**
     * @brief Remove a listener from this event
     *
     * @param listenerID
     * @return Bool
     */
    Bool operator-=(ListenerID listenerID);

    /**
     * @brief Remove all listeners from this event
     */
    void RemoveAllListeners();

    /**
     * @brief Get the number of listeners for this event
     *
     */
    U64 GetListenerCount() const;

    /**
     * @brief Call every callbacks attached to this event
     *
     * @param args
     */
    void Invoke(ArgsTypes... args);

private:
    std::unordered_map<ListenerID, Callback> m_listeners;
    ListenerID                               m_availableListenerID = 0;
};
} // namespace JzRE

#include "JzEvent.inl"
