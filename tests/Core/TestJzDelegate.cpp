/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include <gtest/gtest.h>

#include "JzRE/Runtime/Core/JzDelegate.h"

using namespace JzRE;

// ---------------------------------------------------------------------------
// Add / Broadcast
// ---------------------------------------------------------------------------

TEST(JzDelegate, BroadcastToSingleListener)
{
    JzDelegate<int> delegate;

    int received = -1;
    delegate.Add([&received](int v) { received = v; });

    delegate.Broadcast(42);
    EXPECT_EQ(received, 42);
}

TEST(JzDelegate, BroadcastToMultipleListeners)
{
    JzDelegate<int> delegate;

    int a = 0, b = 0, c = 0;
    delegate.Add([&a](int v) { a = v; });
    delegate.Add([&b](int v) { b = v; });
    delegate.Add([&c](int v) { c = v; });

    delegate.Broadcast(7);
    EXPECT_EQ(a, 7);
    EXPECT_EQ(b, 7);
    EXPECT_EQ(c, 7);
}

TEST(JzDelegate, BroadcastToEmptyDelegateIsNoop)
{
    JzDelegate<> delegate;
    EXPECT_NO_THROW(delegate.Broadcast());
}

// ---------------------------------------------------------------------------
// Handles are unique
// ---------------------------------------------------------------------------

TEST(JzDelegate, AddsReturnUniqueHandles)
{
    JzDelegate<> delegate;

    DelegateHandle h1 = delegate.Add([]() {});
    DelegateHandle h2 = delegate.Add([]() {});
    DelegateHandle h3 = delegate.Add([]() {});

    EXPECT_NE(h1, h2);
    EXPECT_NE(h2, h3);
    EXPECT_NE(h1, h3);
}

// ---------------------------------------------------------------------------
// Remove
// ---------------------------------------------------------------------------

TEST(JzDelegate, RemoveStopsListenerFromReceivingBroadcasts)
{
    JzDelegate<int> delegate;

    int received = 0;
    DelegateHandle handle = delegate.Add([&received](int v) { received = v; });

    delegate.Broadcast(1);
    EXPECT_EQ(received, 1);

    const Bool removed = delegate.Remove(handle);
    EXPECT_TRUE(removed);

    delegate.Broadcast(2);
    EXPECT_EQ(received, 1); // must not have changed
}

TEST(JzDelegate, RemoveReturnsFalseForUnknownHandle)
{
    JzDelegate<> delegate;
    EXPECT_FALSE(delegate.Remove(9999u));
}

TEST(JzDelegate, RemoveIsIdempotent)
{
    JzDelegate<> delegate;
    DelegateHandle h = delegate.Add([]() {});

    EXPECT_TRUE(delegate.Remove(h));
    EXPECT_FALSE(delegate.Remove(h));
}

// ---------------------------------------------------------------------------
// Operator overloads (+= / -=)
// ---------------------------------------------------------------------------

TEST(JzDelegate, OperatorPlusEqualsAddsListener)
{
    JzDelegate<int> delegate;

    int received = 0;
    delegate += [&received](int v) { received = v; };

    delegate.Broadcast(99);
    EXPECT_EQ(received, 99);
}

TEST(JzDelegate, OperatorMinusEqualsRemovesListener)
{
    JzDelegate<int> delegate;

    int received = 0;
    DelegateHandle h = (delegate += [&received](int v) { received = v; });

    delegate -= h;
    delegate.Broadcast(55);
    EXPECT_EQ(received, 0); // must not have been called
}

// ---------------------------------------------------------------------------
// Count
// ---------------------------------------------------------------------------

TEST(JzDelegate, CountReflectsAddAndRemove)
{
    JzDelegate<> delegate;
    EXPECT_EQ(delegate.Count(), 0u);

    DelegateHandle h1 = delegate.Add([]() {});
    EXPECT_EQ(delegate.Count(), 1u);

    DelegateHandle h2 = delegate.Add([]() {});
    EXPECT_EQ(delegate.Count(), 2u);

    delegate.Remove(h1);
    EXPECT_EQ(delegate.Count(), 1u);

    delegate.Remove(h2);
    EXPECT_EQ(delegate.Count(), 0u);
}

// ---------------------------------------------------------------------------
// Clear
// ---------------------------------------------------------------------------

TEST(JzDelegate, ClearRemovesAllListeners)
{
    JzDelegate<int> delegate;

    int calls = 0;
    delegate.Add([&calls](int) { ++calls; });
    delegate.Add([&calls](int) { ++calls; });
    delegate.Add([&calls](int) { ++calls; });

    EXPECT_EQ(delegate.Count(), 3u);

    delegate.Clear();
    EXPECT_EQ(delegate.Count(), 0u);

    delegate.Broadcast(1);
    EXPECT_EQ(calls, 0);
}

TEST(JzDelegate, ClearResetsHandleCounterSoNewHandlesMayReuse)
{
    JzDelegate<> delegate;
    delegate.Add([]() {});
    delegate.Add([]() {});

    delegate.Clear();
    EXPECT_EQ(delegate.Count(), 0u);

    // After clear the delegate is usable again
    DelegateHandle h = delegate.Add([]() {});
    EXPECT_EQ(delegate.Count(), 1u);
    EXPECT_TRUE(delegate.Remove(h));
}

// ---------------------------------------------------------------------------
// Multi-argument broadcast
// ---------------------------------------------------------------------------

TEST(JzDelegate, BroadcastMultipleArguments)
{
    JzDelegate<int, float, bool> delegate;

    int   ri = 0;
    float rf = 0.0f;
    bool  rb = false;

    delegate.Add([&](int i, float f, bool b) {
        ri = i;
        rf = f;
        rb = b;
    });

    delegate.Broadcast(3, 1.5f, true);
    EXPECT_EQ(ri, 3);
    EXPECT_FLOAT_EQ(rf, 1.5f);
    EXPECT_TRUE(rb);
}
