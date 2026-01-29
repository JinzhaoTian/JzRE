/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <queue>
#include <mutex>
#include <vector>
#include "JzECSEvent.h"

namespace JzRE {

/**
 * @brief Thread-safe Event Queue
 */
class JzEventQueue {
public:
    template <typename T>
    void Push(T &&event)
    {
        static_assert(std::is_base_of_v<JzECSEvent, T>, "T must inherit from JzECSEvent");
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.emplace(std::forward<T>(event));
    }

    template <typename T>
    void PushBatch(std::vector<T> &&events)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto &event : events) {
            m_queue.emplace(std::move(event));
        }
    }

    bool Pop(JzECSEventWrapper &outEvent)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_queue.empty()) {
            return false;
        }
        outEvent = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

    size_t PopBatch(std::vector<JzECSEventWrapper> &outEvents, size_t maxCount)
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

    void Clear()
    {
        std::lock_guard<std::mutex>   lock(m_mutex);
        std::queue<JzECSEventWrapper> empty;
        std::swap(m_queue, empty);
    }

    size_t Size() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

    void PushWrapper(JzECSEventWrapper &&wrapper)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.emplace(std::move(wrapper));
    }

private:
    std::vector<JzECSEventWrapper> m_batchBuffer; // Not used in this basic impl
    std::queue<JzECSEventWrapper>  m_queue;
    mutable std::mutex             m_mutex;
};

} // namespace JzRE
