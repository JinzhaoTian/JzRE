/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include <thread>
#include <vector>

#include <gtest/gtest.h>

#include "JzRE/Runtime/Core/JzPlatformEvent.h"
#include "JzRE/Runtime/Core/JzPlatformEventQueue.h"

using namespace JzRE;

namespace {

struct KeyEvent : public JzPlatformEvent {
    int keyCode = 0;
};

struct MouseEvent : public JzPlatformEvent {
    float x = 0.0f;
    float y = 0.0f;
};

struct ResizeEvent : public JzPlatformEvent {
    int width  = 0;
    int height = 0;
};

} // namespace

// ---------------------------------------------------------------------------
// JzPlatformEventWrapper – construction and validity
// ---------------------------------------------------------------------------

TEST(JzPlatformEventWrapper, DefaultConstructedIsInvalid)
{
    JzPlatformEventWrapper wrapper;
    EXPECT_FALSE(wrapper.IsValid());
}

TEST(JzPlatformEventWrapper, TypedConstructionIsValid)
{
    KeyEvent evt;
    evt.keyCode = 65;

    JzPlatformEventWrapper wrapper(std::move(evt));
    EXPECT_TRUE(wrapper.IsValid());
}

TEST(JzPlatformEventWrapper, AsReturnsCorrectPointerForMatchingType)
{
    KeyEvent evt;
    evt.keyCode = 87;

    JzPlatformEventWrapper wrapper(KeyEvent{evt});
    auto *ptr = wrapper.As<KeyEvent>();
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->keyCode, 87);
}

TEST(JzPlatformEventWrapper, SameTypeHasConsistentTypeId)
{
    KeyEvent k1{};
    KeyEvent k2{};
    JzPlatformEventWrapper w1(std::move(k1));
    JzPlatformEventWrapper w2(std::move(k2));

    EXPECT_EQ(w1.GetTypeId(), w2.GetTypeId());
}

// ---------------------------------------------------------------------------
// JzPlatformEventWrapper – move semantics
// ---------------------------------------------------------------------------

TEST(JzPlatformEventWrapper, MoveConstructorTransfersOwnership)
{
    KeyEvent evt;
    evt.keyCode = 55;

    JzPlatformEventWrapper original(std::move(evt));
    EXPECT_TRUE(original.IsValid());

    JzPlatformEventWrapper moved(std::move(original));
    EXPECT_TRUE(moved.IsValid());
    EXPECT_FALSE(original.IsValid()); // source must be empty after move

    auto *ptr = moved.As<KeyEvent>();
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->keyCode, 55);
}

TEST(JzPlatformEventWrapper, MoveAssignmentTransfersOwnership)
{
    MouseEvent evt;
    evt.x = 1.0f;
    evt.y = 2.0f;

    JzPlatformEventWrapper src(std::move(evt));
    JzPlatformEventWrapper dst;

    dst = std::move(src);

    EXPECT_TRUE(dst.IsValid());
    EXPECT_FALSE(src.IsValid());

    auto *ptr = dst.As<MouseEvent>();
    ASSERT_NE(ptr, nullptr);
    EXPECT_FLOAT_EQ(ptr->x, 1.0f);
    EXPECT_FLOAT_EQ(ptr->y, 2.0f);
}

// ---------------------------------------------------------------------------
// JzPlatformEventQueue – basic operations
// ---------------------------------------------------------------------------

TEST(JzPlatformEventQueue, InitiallyEmpty)
{
    JzPlatformEventQueue queue;
    EXPECT_TRUE(queue.IsEmpty());
    EXPECT_EQ(queue.Size(), 0u);
}

TEST(JzPlatformEventQueue, PushIncreasesSize)
{
    JzPlatformEventQueue queue;

    queue.Push(KeyEvent{});
    EXPECT_EQ(queue.Size(), 1u);

    queue.Push(MouseEvent{});
    EXPECT_EQ(queue.Size(), 2u);
}

TEST(JzPlatformEventQueue, PopReturnsTrueAndDequeues)
{
    JzPlatformEventQueue queue;

    KeyEvent evt;
    evt.keyCode = 13;
    queue.Push(std::move(evt));

    JzPlatformEventWrapper out;
    const bool popped = queue.Pop(out);

    EXPECT_TRUE(popped);
    EXPECT_TRUE(out.IsValid());
    EXPECT_TRUE(queue.IsEmpty());

    auto *ptr = out.As<KeyEvent>();
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->keyCode, 13);
}

TEST(JzPlatformEventQueue, PopOnEmptyQueueReturnsFalse)
{
    JzPlatformEventQueue   queue;
    JzPlatformEventWrapper out;
    EXPECT_FALSE(queue.Pop(out));
}

TEST(JzPlatformEventQueue, FIFOOrder)
{
    JzPlatformEventQueue queue;

    KeyEvent e1;
    e1.keyCode = 1;
    KeyEvent e2;
    e2.keyCode = 2;
    KeyEvent e3;
    e3.keyCode = 3;

    queue.Push(std::move(e1));
    queue.Push(std::move(e2));
    queue.Push(std::move(e3));

    JzPlatformEventWrapper out;

    queue.Pop(out);
    ASSERT_EQ(out.As<KeyEvent>()->keyCode, 1);

    queue.Pop(out);
    ASSERT_EQ(out.As<KeyEvent>()->keyCode, 2);

    queue.Pop(out);
    ASSERT_EQ(out.As<KeyEvent>()->keyCode, 3);
}

// ---------------------------------------------------------------------------
// PushBatch / PopBatch
// ---------------------------------------------------------------------------

TEST(JzPlatformEventQueue, PushBatchAddsAllEvents)
{
    JzPlatformEventQueue queue;

    std::vector<KeyEvent> batch(5);
    for (int i = 0; i < 5; ++i) {
        batch[i].keyCode = i;
    }

    queue.PushBatch(std::move(batch));
    EXPECT_EQ(queue.Size(), 5u);
}

TEST(JzPlatformEventQueue, PopBatchReturnsUpToMaxCount)
{
    JzPlatformEventQueue queue;

    for (int i = 0; i < 10; ++i) {
        KeyEvent e;
        e.keyCode = i;
        queue.Push(std::move(e));
    }

    std::vector<JzPlatformEventWrapper> out;
    const size_t popped = queue.PopBatch(out, 4);

    EXPECT_EQ(popped, 4u);
    EXPECT_EQ(out.size(), 4u);
    EXPECT_EQ(queue.Size(), 6u);
}

TEST(JzPlatformEventQueue, PopBatchEmptyQueueReturnsZero)
{
    JzPlatformEventQueue                queue;
    std::vector<JzPlatformEventWrapper> out;
    EXPECT_EQ(queue.PopBatch(out, 10), 0u);
}

// ---------------------------------------------------------------------------
// Clear
// ---------------------------------------------------------------------------

TEST(JzPlatformEventQueue, ClearEmptiesQueue)
{
    JzPlatformEventQueue queue;
    queue.Push(KeyEvent{});
    queue.Push(MouseEvent{});

    queue.Clear();
    EXPECT_TRUE(queue.IsEmpty());
    EXPECT_EQ(queue.Size(), 0u);
}

// ---------------------------------------------------------------------------
// Thread safety
// ---------------------------------------------------------------------------

TEST(JzPlatformEventQueue, ConcurrentPushesProduceCorrectCount)
{
    JzPlatformEventQueue queue;

    constexpr int   kThreads    = 8;
    constexpr int   kPerThread  = 50;

    std::vector<std::thread> threads;
    threads.reserve(kThreads);

    for (int t = 0; t < kThreads; ++t) {
        threads.emplace_back([&queue]() {
            for (int i = 0; i < kPerThread; ++i) {
                KeyEvent evt{};
                queue.Push(std::move(evt));
            }
        });
    }

    for (auto &th : threads) {
        th.join();
    }

    EXPECT_EQ(queue.Size(), static_cast<size_t>(kThreads * kPerThread));
}
