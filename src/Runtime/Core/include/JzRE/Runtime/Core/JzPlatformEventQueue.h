/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <mutex>
#include <queue>
#include <vector>

#include "JzRE/Runtime/Core/JzPlatformEvent.h"

namespace JzRE {

/**
 * @brief Thread-safe event queue for Platform layer events.
 *
 * Similar to JzEventQueue but without Function layer dependencies.
 * Used by JzIWindowBackend to queue platform events for consumption
 * by JzWindowSystem.
 */
class JzPlatformEventQueue {
public:
    /**
     * @brief Push an event to the queue
     * @tparam T Event type (must inherit from JzPlatformEvent)
     * @param event The event to push
     */
    template <typename T>
    void Push(T &&event)
    {
        static_assert(std::is_base_of_v<JzPlatformEvent, std::decay_t<T>>,
                      "T must inherit from JzPlatformEvent");
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.emplace(std::forward<T>(event));
    }

    /**
     * @brief Push a batch of events to the queue
     * @tparam T Event type (must inherit from JzPlatformEvent)
     * @param events The events to push
     */
    template <typename T>
    void PushBatch(std::vector<T> &&events)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto &event : events) {
            m_queue.emplace(std::move(event));
        }
    }

    /**
     * @brief Pop a single event from the queue
     * @param outEvent Output parameter for the popped event
     * @return true if an event was popped, false if queue was empty
     */
    bool Pop(JzPlatformEventWrapper &outEvent)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_queue.empty()) {
            return false;
        }
        outEvent = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

    /**
     * @brief Pop a batch of events from the queue
     * @param outEvents Output vector for the popped events
     * @param maxCount Maximum number of events to pop
     * @return Number of events actually popped
     */
    size_t PopBatch(std::vector<JzPlatformEventWrapper> &outEvents, size_t maxCount)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        size_t                      count = 0;
        while (!m_queue.empty() && count < maxCount) {
            outEvents.emplace_back(std::move(m_queue.front()));
            m_queue.pop();
            count++;
        }
        return count;
    }

    /**
     * @brief Clear all events from the queue
     */
    void Clear()
    {
        std::lock_guard<std::mutex>        lock(m_mutex);
        std::queue<JzPlatformEventWrapper> empty;
        std::swap(m_queue, empty);
    }

    /**
     * @brief Get the current size of the queue
     * @return Number of events in the queue
     */
    size_t Size() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

    /**
     * @brief Check if the queue is empty
     * @return true if empty, false otherwise
     */
    bool IsEmpty() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

private:
    std::queue<JzPlatformEventWrapper> m_queue;
    mutable std::mutex                 m_mutex;
};

} // namespace JzRE
