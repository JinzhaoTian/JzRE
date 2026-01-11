/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <functional>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzThreadPool.h"

namespace JzRE {
/**
 * @brief Task Queue
 */
class JzTaskQueue {
public:
    enum class JzETaskPriority {
        High,
        Normal,
        Low
    };

    explicit JzTaskQueue(Size numThreads = 2);
    ~JzTaskQueue();

    template <typename F, typename... Args>
    auto Submit(JzETaskPriority priority, F &&f, Args &&...args)
        -> std::future<std::invoke_result_t<F, Args...>>;

    void WaitForAll();
    Size GetPendingCount() const;
    void Stop();

private:
    struct TaskItem {
        JzETaskPriority       priority;
        std::function<void()> task;

        Bool operator<(const TaskItem &other) const
        {
            return static_cast<I32>(priority) < static_cast<I32>(other.priority);
        }
    };

private:
    std::unique_ptr<JzThreadPool> m_threadPool;
    std::priority_queue<TaskItem> m_priorityQueue;
    std::mutex                    m_queueMutex;
    std::condition_variable       m_condition;
    std::atomic<Bool>             m_stop{false};
    std::thread                   m_dispatcherThread;
};

template <typename F, typename... Args>
auto JzTaskQueue::Submit(JzETaskPriority priority, F &&f, Args &&...args)
    -> std::future<std::invoke_result_t<F, Args...>>
{
    using return_type = std::invoke_result_t<F, Args...>;

    auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> result = task->get_future();

    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        if (m_stop) {
            throw std::runtime_error("Submit on stopped TaskQueue");
        }

        m_priorityQueue.push({priority, [task]() { (*task)(); }});
    }

    m_condition.notify_one();

    return result;
}

} // namespace JzRE