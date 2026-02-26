/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include <gtest/gtest.h>

#include "JzRE/Runtime/Core/JzServiceContainer.h"

using namespace JzRE;

namespace {

struct ServiceA {
    int value = 0;
};

struct ServiceB {
    float value = 0.0f;
};

struct ServiceC {
    bool active = false;
};

class JzServiceContainerTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        // Start each test with a clean slate
        JzServiceContainer::Init();
    }

    void TearDown() override
    {
        JzServiceContainer::Init();
    }
};

} // namespace

// ---------------------------------------------------------------------------
// Provide / Get
// ---------------------------------------------------------------------------

TEST_F(JzServiceContainerTest, ProvideAndGetReturnsRegisteredService)
{
    ServiceA a;
    a.value = 42;

    JzServiceContainer::Provide<ServiceA>(a);

    ServiceA &retrieved = JzServiceContainer::Get<ServiceA>();
    EXPECT_EQ(retrieved.value, 42);
}

TEST_F(JzServiceContainerTest, GetReturnsReferenceToSameObject)
{
    ServiceA a;
    a.value = 1;

    JzServiceContainer::Provide<ServiceA>(a);

    // Mutate through retrieved reference
    JzServiceContainer::Get<ServiceA>().value = 99;

    EXPECT_EQ(a.value, 99);
}

TEST_F(JzServiceContainerTest, MultipleDistinctTypesCoexist)
{
    ServiceA a;
    a.value = 10;
    ServiceB b;
    b.value = 3.14f;
    ServiceC c;
    c.active = true;

    JzServiceContainer::Provide<ServiceA>(a);
    JzServiceContainer::Provide<ServiceB>(b);
    JzServiceContainer::Provide<ServiceC>(c);

    EXPECT_EQ(JzServiceContainer::Get<ServiceA>().value, 10);
    EXPECT_FLOAT_EQ(JzServiceContainer::Get<ServiceB>().value, 3.14f);
    EXPECT_TRUE(JzServiceContainer::Get<ServiceC>().active);
}

// ---------------------------------------------------------------------------
// Has
// ---------------------------------------------------------------------------

TEST_F(JzServiceContainerTest, HasReturnsFalseBeforeProvide)
{
    EXPECT_FALSE(JzServiceContainer::Has<ServiceA>());
}

TEST_F(JzServiceContainerTest, HasReturnsTrueAfterProvide)
{
    ServiceA a;
    JzServiceContainer::Provide<ServiceA>(a);
    EXPECT_TRUE(JzServiceContainer::Has<ServiceA>());
}

TEST_F(JzServiceContainerTest, HasDoesNotAffectOtherTypes)
{
    ServiceA a;
    JzServiceContainer::Provide<ServiceA>(a);

    EXPECT_TRUE(JzServiceContainer::Has<ServiceA>());
    EXPECT_FALSE(JzServiceContainer::Has<ServiceB>());
}

// ---------------------------------------------------------------------------
// Remove
// ---------------------------------------------------------------------------

TEST_F(JzServiceContainerTest, RemoveUnregistersService)
{
    ServiceA a;
    JzServiceContainer::Provide<ServiceA>(a);
    EXPECT_TRUE(JzServiceContainer::Has<ServiceA>());

    JzServiceContainer::Remove<ServiceA>();
    EXPECT_FALSE(JzServiceContainer::Has<ServiceA>());
}

TEST_F(JzServiceContainerTest, RemoveDoesNotAffectOtherServices)
{
    ServiceA a;
    ServiceB b;

    JzServiceContainer::Provide<ServiceA>(a);
    JzServiceContainer::Provide<ServiceB>(b);

    JzServiceContainer::Remove<ServiceA>();

    EXPECT_FALSE(JzServiceContainer::Has<ServiceA>());
    EXPECT_TRUE(JzServiceContainer::Has<ServiceB>());
}

TEST_F(JzServiceContainerTest, RemoveOnUnregisteredTypeIsNoop)
{
    EXPECT_NO_THROW(JzServiceContainer::Remove<ServiceA>());
    EXPECT_FALSE(JzServiceContainer::Has<ServiceA>());
}

// ---------------------------------------------------------------------------
// Init
// ---------------------------------------------------------------------------

TEST_F(JzServiceContainerTest, InitClearsAllServices)
{
    ServiceA a;
    ServiceB b;

    JzServiceContainer::Provide<ServiceA>(a);
    JzServiceContainer::Provide<ServiceB>(b);

    EXPECT_TRUE(JzServiceContainer::Has<ServiceA>());
    EXPECT_TRUE(JzServiceContainer::Has<ServiceB>());

    JzServiceContainer::Init();

    EXPECT_FALSE(JzServiceContainer::Has<ServiceA>());
    EXPECT_FALSE(JzServiceContainer::Has<ServiceB>());
}

TEST_F(JzServiceContainerTest, ReprovideAfterInit)
{
    ServiceA a;
    a.value = 5;

    JzServiceContainer::Provide<ServiceA>(a);
    JzServiceContainer::Init();

    ServiceA a2;
    a2.value = 99;
    JzServiceContainer::Provide<ServiceA>(a2);

    EXPECT_EQ(JzServiceContainer::Get<ServiceA>().value, 99);
}

// ---------------------------------------------------------------------------
// Overwrite existing service
// ---------------------------------------------------------------------------

TEST_F(JzServiceContainerTest, ProvideOverwritesExistingRegistration)
{
    ServiceA a1;
    a1.value = 1;
    JzServiceContainer::Provide<ServiceA>(a1);

    ServiceA a2;
    a2.value = 2;
    JzServiceContainer::Provide<ServiceA>(a2);

    EXPECT_EQ(JzServiceContainer::Get<ServiceA>().value, 2);
}
