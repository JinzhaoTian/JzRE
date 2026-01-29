/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzECSEvent.h"

namespace JzRE {

/**
 * @brief Interface for event handlers
 */
class JzIEventHandler {
public:
    virtual ~JzIEventHandler()                               = default;
    virtual void HandleEvent(const JzECSEventWrapper &event) = 0;
    virtual U32  GetEventType() const                        = 0;
    virtual I32  GetPriority() const
    {
        return 0;
    }
};

/**
 * @brief Template implementation of event handler
 */
template <typename T>
class JzEventHandler : public JzIEventHandler {
public:
    using HandlerFunc = std::function<void(const T &)>;

    JzEventHandler(HandlerFunc func, I32 priority = 0) :
        m_handler(std::move(func)),
        m_priority(priority) { }

    void HandleEvent(const JzECSEventWrapper &event) override
    {
        if (auto *typedEvent = event.As<T>()) {
            m_handler(*typedEvent);
        }
    }

    U32 GetEventType() const override
    {
        return JzECSEventType<T>::Id();
    }

    I32 GetPriority() const override
    {
        return m_priority;
    }

private:
    HandlerFunc m_handler;
    I32         m_priority;
};

} // namespace JzRE
