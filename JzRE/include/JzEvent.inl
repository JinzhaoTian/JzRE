#pragma once

#include "JzEvent.h"

namespace JzRE {

template <class... ArgsTypes>
ListenerID JzEvent<ArgsTypes...>::AddListener(Callback callback)
{
    ListenerID listenerID = m_availableListenerID++;
    m_listeners.emplace(listenerID, callback);
    return listenerID;
}

template <class... ArgsTypes>
Bool JzEvent<ArgsTypes...>::RemoveListener(ListenerID listenerID)
{
    return m_listeners.erase(listenerID) != 0;
}

template <class... ArgsTypes>
ListenerID JzEvent<ArgsTypes...>::operator+=(Callback callback)
{
    return AddListener(callback);
}

template <class... ArgsTypes>
Bool JzEvent<ArgsTypes...>::operator-=(ListenerID listenerID)
{
    return RemoveListener(listenerID);
}

template <class... ArgsTypes>
void JzEvent<ArgsTypes...>::RemoveAllListeners()
{
    m_listeners.clear();
    m_availableListenerID = 0;
}

template <class... ArgsTypes>
U64 JzEvent<ArgsTypes...>::GetListenerCount() const
{
    return m_listeners.size();
}

template <class... ArgsTypes>
void JzEvent<ArgsTypes...>::Invoke(ArgsTypes... args)
{
    for (const auto &listener : m_listeners) {
        listener.second(args...);
    }
}

} // namespace JzRE