/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include <atomic>
#include <future>
#include <stdexcept>
#include <vector>

#include <gtest/gtest.h>

#include "JzRE/Runtime/Core/JzThreadPool.h"

using namespace JzRE;

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

TEST(JzThreadPool, ConstructorCreatesRequestedThreads)
{
    JzThreadPool pool(4);
    EXPECT_EQ(pool.GetThreadCount(), 4u);
}

TEST(JzThreadPool, SingleThreadPoolIsValid)
{
    JzThreadPool pool(1);
    EXPECT_EQ(pool.GetThreadCount(), 1u);
}

// ---------------------------------------------------------------------------
// Submit and return values
// ---------------------------------------------------------------------------

TEST(JzThreadPool, SubmitReturnsFutureWithCorrectValue)
{
    JzThreadPool pool(2);

    auto future = pool.Submit([]() { return 42; });
    EXPECT_EQ(future.get(), 42);
}

TEST(JzThreadPool, SubmitWithArguments)
{
    JzThreadPool pool(2);

    auto future = pool.Submit([](int a, int b) { return a + b; }, 3, 7);
    EXPECT_EQ(future.get(), 10);
}

TEST(JzThreadPool, SubmitVoidTask)
{
    JzThreadPool pool(2);

    std::atomic<bool> executed{false};
    auto              future = pool.Submit([&executed]() { executed.store(true); });

    future.get(); // wait for completion
    EXPECT_TRUE(executed.load());
}

// ---------------------------------------------------------------------------
// Concurrent tasks
// ---------------------------------------------------------------------------

TEST(JzThreadPool, ConcurrentTasksAllExecute)
{
    constexpr int kTaskCount = 64;
    JzThreadPool  pool(4);

    std::atomic<int>          counter{0};
    std::vector<std::future<void>> futures;
    futures.reserve(kTaskCount);

    for (int i = 0; i < kTaskCount; ++i) {
        futures.push_back(pool.Submit([&counter]() {
            counter.fetch_add(1, std::memory_order_relaxed);
        }));
    }

    for (auto &f : futures) {
        f.get();
    }

    EXPECT_EQ(counter.load(), kTaskCount);
}

TEST(JzThreadPool, TasksProduceCorrectSumConcurrently)
{
    constexpr int kTaskCount = 100;
    JzThreadPool  pool(8);

    std::vector<std::future<int>> futures;
    futures.reserve(kTaskCount);

    for (int i = 1; i <= kTaskCount; ++i) {
        futures.push_back(pool.Submit([i]() { return i; }));
    }

    int sum = 0;
    for (auto &f : futures) {
        sum += f.get();
    }

    // 1 + 2 + ... + 100 = 5050
    EXPECT_EQ(sum, 5050);
}

// ---------------------------------------------------------------------------
// Pending task count
// ---------------------------------------------------------------------------

TEST(JzThreadPool, PendingTaskCountReachesZeroAfterCompletion)
{
    JzThreadPool pool(2);

    auto future = pool.Submit([]() { return 0; });
    future.get();

    // All tasks should be done by the time get() returns
    EXPECT_EQ(pool.GetPendingTaskCount(), 0u);
}

// ---------------------------------------------------------------------------
// Stop behaviour
// ---------------------------------------------------------------------------

TEST(JzThreadPool, SubmitAfterStopThrows)
{
    JzThreadPool pool(2);
    pool.Stop();

    EXPECT_THROW(pool.Submit([]() {}), std::runtime_error);
}

TEST(JzThreadPool, StopIsIdempotent)
{
    JzThreadPool pool(2);
    pool.Stop();
    EXPECT_NO_THROW(pool.Stop());
}

// ---------------------------------------------------------------------------
// Task ordering – ordering is not guaranteed, but all tasks must complete
// ---------------------------------------------------------------------------

TEST(JzThreadPool, AllSubmittedTasksEventuallyComplete)
{
    JzThreadPool pool(3);

    constexpr int             kN = 200;
    std::vector<std::future<int>> futures;
    futures.reserve(kN);

    for (int i = 0; i < kN; ++i) {
        futures.push_back(pool.Submit([i]() { return i; }));
    }

    int count = 0;
    for (auto &f : futures) {
        f.get();
        ++count;
    }

    EXPECT_EQ(count, kN);
}
