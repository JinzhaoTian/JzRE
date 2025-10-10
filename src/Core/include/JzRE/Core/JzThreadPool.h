/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <atomic>
#include <type_traits>
#include "JzRE/Core/JzRETypes.h"
#include "JzRE/Core/JzLogger.h"

namespace JzRE {
/**
 * @brief Thread Pool
 */
class JzThreadPool {
public:
    explicit JzThreadPool(Size num_threads = std::thread::hardware_concurrency()) :
        m_stop(false)
    {
        m_workers.reserve(num_threads);
        for (Size i = 0; i < num_threads; ++i) {
            m_workers.emplace_back([this]() {
                _WorkerThread();
            });
        }
    }

    JzThreadPool(const JzThreadPool &) = delete;

    JzThreadPool &operator=(const JzThreadPool &) = delete;

    ~JzThreadPool()
    {
        Stop();
    }

    template <typename F, typename... Args>
    auto Submit(F &&f, Args &&...args)
        -> std::future<std::invoke_result_t<F, Args...>>
    {
        using return_type = std::invoke_result_t<F, Args...>;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<return_type> result = task->get_future();

        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            if (m_stop) {
                throw std::runtime_error("Submit on stopped ThreadPool");
            }
            m_tasks.emplace([task]() { (*task)(); });
            m_pendingTasks++;
        }

        m_condition.notify_one();

        return result;
    }

    void Stop()
    {
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_stop = true;
        }

        m_condition.notify_all();

        for (auto &worker : m_workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

    Size GetThreadCount() const
    {
        return m_workers.size();
    }

    Size GetPendingTaskCount() const
    {
        return m_pendingTasks;
    }

private:
    void _WorkerThread()
    {
        while (true) {
            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lock(m_queueMutex);
                m_condition.wait(lock, [this] {
                    return m_stop || !m_tasks.empty();
                });

                if (m_stop && m_tasks.empty()) {
                    return;
                }

                task = std::move(m_tasks.front());
                m_tasks.pop();
            }

            try {
                task();
                m_pendingTasks--;
            } catch (const std::exception &e) {
                JzRE_LOG_ERROR("ThreadPool task exception: {}", e.what());
                m_pendingTasks--;
            }
        }
    }

private:
    std::vector<std::thread>          m_workers;
    std::queue<std::function<void()>> m_tasks;
    std::mutex                        m_queueMutex;
    std::condition_variable           m_condition;
    std::atomic<Bool>                 m_stop{false};
    std::atomic<Size>                 m_pendingTasks{0};
};

} // namespace JzRE